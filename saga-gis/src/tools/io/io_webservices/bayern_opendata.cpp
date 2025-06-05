
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    io_webservices                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  bayern_opendata.cpp                  //
//                                                       //
//                 Copyrights (C) 2025                   //
//                     Olaf Conrad                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "bayern_opendata.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CBayernOpenData_DGM1::CBayernOpenData_DGM1(void)
{
	Set_Name		("DGM1 (OpenData, Bayern)");

	Set_Author		("O.Conrad (c) 2025");

	Set_Description	(_TW(
		"This tool provides easy-to-use access to the "
		SG_T("<i>\'Digitales Geländemodell 1m (DGM1)\'</i> ")
		"elevation data from the OpenData server of the "
		"<i>Bayrische Vermessungsverwaltung</i>. "
		"It uses a local database in the chosen directory which provides "
		"the original tiles. If the tiles covering the requested area are "
		"not found in this directory the tool tries to download these "
		"from the server. "
	));

	Add_Reference("https://geodaten.bayern.de/opengeodata/OpenDataDetail.html?pn=dgm1",
		SG_T("Bayrische Vermessungsverwaltung - OpenData")
	);

	//-----------------------------------------------------
	m_ServerPath = "https://download1.bayernwolke.de/a/dgm/dgm1/";
	m_VRT_Name   = "dgm1";

	//-----------------------------------------------------
	Parameters.Add_Grid_Output("",
		"RESULT"     , _TL("DGM1"),
		_TL("")
	);

	Parameters.Add_FilePath("",
		"TILES"      , _TL("Local Tiles Directory"),
		_TL("Download location for tiles. If requested tile is already present download will be skipped."),
		NULL, NULL, true, true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"EXTENT"     , _TL("Extent"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("user defined"),
			_TL("shapes extent"),
			_TL("grid system extent"),
			_TL("grid system")
		), 0
	);

	Parameters.Add_Grid_System("EXTENT",
		"GRID_SYSTEM", _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Grid("GRID_SYSTEM",
		"GRID"       , _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("EXTENT",
		"SHAPES"     , _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Double("EXTENT", "XMIN", _TL("West"   ), _TL(""),  649000.);
	Parameters.Add_Double("EXTENT", "XMAX", _TL("East"   ), _TL(""),  650000.);
	Parameters.Add_Double("EXTENT", "YMIN", _TL("South"  ), _TL(""), 5253000.);
	Parameters.Add_Double("EXTENT", "YMAX", _TL("North"  ), _TL(""), 5254000.);
	Parameters.Add_Int   ("EXTENT", "NX"  , _TL("Columns"), _TL(""),    1000, 1, true);
	Parameters.Add_Int   ("EXTENT", "NY"  , _TL("Rows"   ), _TL(""),    1000, 1, true);

	Parameters.Add_Double("",
		"BUFFER"     , _TL("Buffer"),
		_TL("add buffer (map units) to extent"),
		0., 0., true
	);

	Parameters.Add_Double("",
		"CELLSIZE"   , _TL("Cellsize"),
		_TL(""),
		1., 1., true
	);

	m_CRS.Create(Parameters); Parameters.Set_Parameter("CRS_STRING", "epsg:25832"); m_CRS.On_Parameter_Changed(&Parameters, Parameters("CRS_STRING"));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBayernOpenData_DGM1::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CBayernOpenData_DGM1::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CBayernOpenData_DGM1::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("CELLSIZE") || (pParameter->Get_Parent() && pParameter->Get_Parent()->Cmp_Identifier("EXTENT")) )
	{
		double Cellsize = (*pParameters)("CELLSIZE")->asDouble();
		double xMin     = (*pParameters)("XMIN"    )->asDouble();
		double yMin     = (*pParameters)("YMIN"    )->asDouble();
		int    NX       = (*pParameters)("NX"      )->asInt   ();
		int    NY       = (*pParameters)("NY"      )->asInt   ();

		if( pParameter->Cmp_Identifier("CELLSIZE") )
		{
			NX = 1 + (int)(((*pParameters)("XMAX")->asDouble() - xMin) / Cellsize);
			NY = 1 + (int)(((*pParameters)("YMAX")->asDouble() - yMin) / Cellsize);
		}

		if( pParameter->Cmp_Identifier("XMAX") ) { xMin = pParameter->asDouble() - Cellsize * NX; }
		if( pParameter->Cmp_Identifier("YMAX") ) { yMin = pParameter->asDouble() - Cellsize * NY; }

		CSG_Grid_System System(Cellsize, xMin, yMin, NX, NY);

		if( System.is_Valid() )
		{
			(*pParameters)("XMIN")->Set_Value(System.Get_XMin());
			(*pParameters)("XMAX")->Set_Value(System.Get_XMax());
			(*pParameters)("YMIN")->Set_Value(System.Get_YMin());
			(*pParameters)("YMAX")->Set_Value(System.Get_YMax());
			(*pParameters)("NX"  )->Set_Value(System.Get_NX  ());
			(*pParameters)("NY"  )->Set_Value(System.Get_NY  ());
		}
	}

	m_CRS.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CBayernOpenData_DGM1::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("EXTENT") )
	{
		pParameters->Set_Enabled("XMIN"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("XMAX"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("YMIN"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("YMAX"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("NX"         , pParameter->asInt() == 0);
		pParameters->Set_Enabled("NY"         , pParameter->asInt() == 0);
		pParameters->Set_Enabled("SHAPES"     , pParameter->asInt() == 1);
		pParameters->Set_Enabled("GRID_SYSTEM", pParameter->asInt() >= 2);
		pParameters->Set_Enabled("CELLSIZE"   , pParameter->asInt() != 3);
		pParameters->Set_Enabled("BUFFER"     , pParameter->asInt() == 1 || pParameter->asInt() == 2);
	}

	if( pParameters->Get_Name().Cmp(Get_Name()) == 0 )
	{
		CSG_Data_Object *pObject =
			(*pParameters)["EXTENT"].asInt() == 1 ? (*pParameters)["SHAPES"].asDataObject() :
			(*pParameters)["EXTENT"].asInt() >= 2 ? (*pParameters)["GRID"  ].asDataObject() : NULL;

		pParameters->Set_Enabled("CRS_PICKER", !SG_Get_Data_Manager().Exists(pObject) || !pObject->Get_Projection().is_Okay());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBayernOpenData_DGM1::On_Execute(void)
{
	CSG_String Directory = Parameters("TILES")->asString();

	if( !SG_Dir_Exists(Directory) )
	{
		Message_Fmt("\n%s: %s\n", _TL("Warning"), _TL("No or invalid directory specified for local tiles database! Using temporary folder instead!"));

		Directory = SG_File_Make_Path(SG_Dir_Get_Temp(), Get_Name());

		if( !SG_Dir_Create(Directory, true) )
		{
			Error_Fmt("%s: %s", _TL("failed to create temporary tiles directory"), Directory.c_str());

			return( false );
		}
	}

	//--------------------------------------------------------
	CSG_Rect Extent, Extent_25832; CSG_Projection Projection, Projection_25832(25832);

	double Cellsize = Parameters("CELLSIZE")->asDouble();

	switch( Parameters("EXTENT")->asInt() )
	{
	default: // user defined
		Extent.Create(
			Parameters("XMIN")->asDouble(), Parameters("YMIN")->asDouble(),
			Parameters("XMAX")->asDouble(), Parameters("YMAX")->asDouble()
		);
		break;

	case  1: // shapes extent
		Extent.Create(Parameters("SHAPES")->asShapes()->Get_Extent());
		Projection  = Parameters("SHAPES")->asShapes()->Get_Projection();

		if( Parameters("BUFFER")->asDouble() > 0. )
		{
			Extent.Inflate(Parameters("BUFFER")->asDouble(), false);
		}
		break;

	case  2: // grid system extent
		Extent.Create(Parameters("GRID")->asGrid()->Get_Extent());
		Projection  = Parameters("GRID")->asGrid()->Get_Projection();

		if( Parameters("BUFFER")->asDouble() > 0. )
		{
			Extent.Inflate(Parameters("BUFFER")->asDouble(), false);
		}
		break;

	case  3: // grid system
		Cellsize    = Parameters("GRID")->asGrid()->Get_System().Get_Cellsize();
		Extent.Create(Parameters("GRID")->asGrid()->Get_Extent());
		Projection  = Parameters("GRID")->asGrid()->Get_Projection();
		break;
	}

	if( !Projection.is_Okay() )
	{
		m_CRS.Get_CRS(Projection);

		if( !Projection.is_Okay() )
		{
			return( false );
		}
	}

	//--------------------------------------------------------
	if( Projection != Projection_25832 )
	{
		if( Parameters("EXTENT")->asInt() != 3 ) // grid system
		{
			Extent.xMin = Cellsize * floor(Extent.xMin / Cellsize);
			Extent.xMax = Cellsize * ceil (Extent.xMax / Cellsize);
			Extent.yMin = Cellsize * floor(Extent.yMin / Cellsize);
			Extent.yMax = Cellsize * ceil (Extent.yMax / Cellsize);
		}

		CSG_Shapes AoI(SHAPE_TYPE_Point); AoI.Get_Projection() = Projection;

		AoI.Add_Shape()->Add_Point(Extent.Get_XMin   (), Extent.Get_YMin   ());
		AoI.Add_Shape()->Add_Point(Extent.Get_XMin   (), Extent.Get_YCenter());
		AoI.Add_Shape()->Add_Point(Extent.Get_XMin   (), Extent.Get_YMax   ());
		AoI.Add_Shape()->Add_Point(Extent.Get_XCenter(), Extent.Get_YMax   ());
		AoI.Add_Shape()->Add_Point(Extent.Get_XMax   (), Extent.Get_YMax   ());
		AoI.Add_Shape()->Add_Point(Extent.Get_XMax   (), Extent.Get_YCenter());
		AoI.Add_Shape()->Add_Point(Extent.Get_XMax   (), Extent.Get_YMin   ());
		AoI.Add_Shape()->Add_Point(Extent.Get_XCenter(), Extent.Get_YMin   ());

		if( !SG_Get_Projected(&AoI, NULL, Projection_25832) )
		{
			Error_Set("failed to project target to geographic coordinates");

			return( false );
		}

		Extent_25832 = AoI.Get_Extent();
	}
	else
	{
		Extent_25832 = Extent;
	}

	//--------------------------------------------------------
	if( !Provide_Tiles(Directory, Extent_25832) )
	{
		return( false );
	}

	//--------------------------------------------------------
	CSG_Data_Manager Data;

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 0);

	if( !pTool || !pTool->Reset() || !pTool->Set_Manager(&Data)
	||  !pTool->Set_Parameter("FILES"      , SG_File_Make_Path(Directory, m_VRT_Name, "vrt"))
	||  !pTool->Set_Parameter("EXTENT"     , 1) // "user defined"
	||  !pTool->Set_Parameter("EXTENT_XMIN", Extent_25832.xMin)
	||  !pTool->Set_Parameter("EXTENT_XMAX", Extent_25832.xMax)
	||  !pTool->Set_Parameter("EXTENT_XMAX", Extent_25832.xMax)
	||  !pTool->Set_Parameter("EXTENT_YMIN", Extent_25832.yMin)
	||  !pTool->Set_Parameter("EXTENT_YMAX", Extent_25832.yMax)
	||  !pTool->Execute() )
	{
		Error_Fmt("failed to execute tool \'%s\'", _TL("Import Raster"));

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		return( false );
	}

	CSG_Grid *pGrid = pTool->Get_Parameter("GRIDS")->asGridList()->Get_Grid(0);

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	//--------------------------------------------------------
	if( Projection == Projection_25832 && Cellsize == 1. )
	{
		pGrid->Set_Name("DGM1");

		Parameters.Set_Parameter("RESULT", pGrid);

		return( true );
	}

	Process_Set_Text("%s...", _TL("projection"));

	pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 4);

	if( !pTool || !pTool->Set_Manager(&Data)
	||  !pTool->Set_Parameter("CRS_STRING"       , Projection.Get_WKT())
	||  !pTool->Set_Parameter("SOURCE"           , pGrid)
	||  !pTool->Set_Parameter("RESAMPLING"       , 3) // B-Spline
	||  !pTool->Set_Parameter("DATA_TYPE"        , 8) // 4 byte floating point
	||  !pTool->Set_Parameter("TARGET_DEFINITION", 0) // 'user defined'
	||  !pTool->Set_Parameter("TARGET_USER_SIZE" , Cellsize)
	||  !pTool->Set_Parameter("TARGET_USER_XMAX" , Extent.xMax)
	||  !pTool->Set_Parameter("TARGET_USER_XMIN" , Extent.xMin)
	||  !pTool->Set_Parameter("TARGET_USER_YMAX" , Extent.yMax)
	||  !pTool->Set_Parameter("TARGET_USER_YMIN" , Extent.yMin)
	||  !pTool->Execute() )
	{
		Error_Fmt("failed to execute tool \'%s\'", _TL("Coordinate Transformation (Grid)"));

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		return( false );
	}

	pGrid = pTool->Get_Parameter("GRID")->asGrid(); Data.Delete(pGrid, true);

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	//--------------------------------------------------------
	pGrid->Set_Name("DGM1");

	Parameters.Set_Parameter("RESULT", pGrid);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBayernOpenData_DGM1::Provide_Tiles(const CSG_String &Directory, CSG_Rect Extent)
{
	CSG_Rect_Int Tiles(
		(int)floor(Extent.xMin / 1000.), (int)floor(Extent.yMax / 1000.),
		(int)floor(Extent.xMax / 1000.), (int)floor(Extent.yMin / 1000.)
	);

	int nAdded = 0, nFailed = 0, nFound = 0, i = 0, n = (1 + Tiles.Get_XRange()) * (1 + Tiles.Get_YRange());

	for(int Row=Tiles.yMin; Process_Get_Okay() && Row<=Tiles.yMax; Row++)
	{
		for(int Col=Tiles.xMin; Set_Progress(i++, n) && Col<=Tiles.xMax; Col++)
		{
			int Result = Provide_Tile(Directory, Col, Row);

			if( Result > 0 )
			{
				nAdded  += 1;
			}
			else if( Result < 0 )
			{
				nFailed += 1;
			}
			else
			{
				nFound  += 1;
			}
		}
	}

	if( nFailed > 0 )
	{
		Message_Fmt("\n%d download(s) of %d failed", nFailed, nFailed + nAdded);
	}

	if( (nAdded + nFound > 0) || !SG_File_Exists(SG_File_Make_Path(Directory, m_VRT_Name, "vrt")) )
	{
		Update_VRT(Directory);
	}

	return( nAdded + nFound > 0 );
}

//---------------------------------------------------------
int CBayernOpenData_DGM1::Provide_Tile(const CSG_String &Directory, int Col, int Row)
{
	CSG_String File = CSG_String::Format("%d_%d.tif", Col, Row);

	CSG_String Local_File = SG_File_Make_Path(Directory, File);

	if( SG_File_Exists(Local_File) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s: %s%s...", _TL("requesting file"), m_ServerPath.c_str(), File.c_str());

	Process_Set_Text("%s: %s...", File.c_str(), _TL("downloading"));

//	SG_UI_Process_Set_Busy(true, CSG_String::Format("%s: %s%s...", _TL("downloading"), m_ServerPath.c_str(), File.c_str()));

	CSG_CURL Connection(m_ServerPath);

	if( !Connection.Request(File, Local_File.c_str()) )
	{
//		SG_UI_Process_Set_Busy(false);

		Message_Fmt(_TL("failed"));

		Error_Fmt("%s:\n\n%s%s", _TL("failed to request file from server"), m_ServerPath.c_str(), File.c_str());

		return( -1 );
	}

//	SG_UI_Process_Set_Busy(false);

	//-----------------------------------------------------
	Message_Fmt(_TL("okay"));

	return( 1 );
}

//---------------------------------------------------------
bool CBayernOpenData_DGM1::Update_VRT(const CSG_String &Directory)
{
	CSG_Strings Files;

	if( !SG_Dir_List_Files(Files, Directory, "tif") || Files.Get_Count() < 1 )
	{
		Error_Set(_TL("no files found in directory"));

		return( false );
	}

	CSG_String Tiles;

	for(int i=0; i<Files.Get_Count(); i++)
	{
		Tiles += "\"" + Files[i] + "\" ";
	}

	//-----------------------------------------------------
	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 12);

	SG_UI_ProgressAndMsg_Lock(true);

	bool bResult = pTool
	  && pTool->Set_Parameter("FILES"   , Tiles)
	  && pTool->Set_Parameter("VRT_NAME", SG_File_Make_Path(Directory, m_VRT_Name, "vrt"))
	  && pTool->Execute();

	SG_UI_ProgressAndMsg_Lock(false);

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	if( !bResult )
	{
		Error_Set(_TL("failed to update Virtual Raster Tiles file"));
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
