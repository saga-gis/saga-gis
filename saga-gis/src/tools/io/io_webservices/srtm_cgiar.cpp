
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
//                    srtm_cgiar.cpp                     //
//                                                       //
//                 Copyrights (C) 2024                   //
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
#include "srtm_cgiar.h"

//---------------------------------------------------------
#define VRT_NAME "srtm_tiles"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSRTM_CGIAR::CSRTM_CGIAR(void)
{
	Set_Name		(_TL("CGIAR CSI SRTM"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"Prepare 3arcsec SRTM data for target areas of your choice. "
		"Builds a local database in the chosen directory with the "
		"original CGIAR CSI SRTM tiles. If not done yet all tiles "
		"covering  the requested area are downloaded from the "
		"CGIAR CSI server. "
	));

	Add_Reference("Jarvis A., H.I. Reuter, A.  Nelson, E. Guevara", "2008",
		"Hole-filled seamless SRTM data V4",
		"International Centre for Tropical Agriculture (CIAT).",
		SG_T("https://srtm.csi.cgiar.org/"), SG_T("srtm.csi.cgiar.org")
	);

	Add_Reference("Reuter  H.I,  A.  Nelson,  A.  Jarvis", "2007",
		"An evaluation of void filling interpolation methods for SRTM data",
		"International Journal of Geographic Information Science, 21:9, 983-1008.",
		SG_T("https://doi.org/10.1080/13658810601169899"), SG_T("doi:10.1080/13658810601169899")
	);

	Add_Reference("https://srtm.csi.cgiar.org/",
		SG_T("SRTM 90m DEM Digital Elevation Database")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_Output("",
		"SRTM"       , _TL("SRTM"),
		_TL("")
	);

	Parameters.Add_FilePath("",
		"TILES"      , _TL("Local Tiles Directory"),
		_TL("Download location for SRTM tiles. If requested tile is already present download will be skipped"),
		NULL, NULL, true, true
	);

	Parameters.Add_Bool("TILES",
		"DELZIP"     , _TL("Delete Zip Files"),
		_TL("Do not keep zip files after download"),
		true
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

	Parameters.Add_Double("EXTENT", "XMIN", _TL("West"   ), _TL(""),  270360.);
	Parameters.Add_Double("EXTENT", "XMAX", _TL("East"   ), _TL(""),  931320.);
	Parameters.Add_Double("EXTENT", "YMIN", _TL("South"  ), _TL(""), 5225850.);
	Parameters.Add_Double("EXTENT", "YMAX", _TL("North"  ), _TL(""), 6111540.);
	Parameters.Add_Int   ("EXTENT", "NX"  , _TL("Columns"), _TL(""),    7345, 1, true);
	Parameters.Add_Int   ("EXTENT", "NY"  , _TL("Rows"   ), _TL(""),    9842, 1, true);

	Parameters.Add_Double("",
		"BUFFER"     , _TL("Buffer"),
		_TL("add buffer (map units) to extent"),
		0., 0., true
	);

	Parameters.Add_Double("",
		"CELLSIZE"   , _TL("Cellsize"),
		_TL(""),
		90., 0.0001, true
	);

	m_CRS.Create(Parameters); Parameters.Set_Parameter("CRS_STRING", "epsg:25832"); m_CRS.On_Parameter_Changed(&Parameters, Parameters("CRS_STRING"));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSRTM_CGIAR::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CSRTM_CGIAR::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CSRTM_CGIAR::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
int CSRTM_CGIAR::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
bool CSRTM_CGIAR::On_Execute(void)
{
	CSG_String Directory = Parameters("TILES")->asString();

	if( !SG_Dir_Exists(Directory) )
	{
		Error_Set("no or invalid directory specified for local tiles database");

		return( false );
	}

	//--------------------------------------------------------
	CSG_Rect Extent, Extent_GCS; CSG_Projection Projection;

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
	if( !Projection.is_Geographic() )
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

		if( !SG_Get_Projected(&AoI, NULL, CSG_Projection::Get_GCS_WGS84()) )
		{
			Error_Set("failed to project target to geographic coordinates");

			return( false );
		}

		Extent_GCS = AoI.Get_Extent(); // Extent_GCS.Inflate(10 * 3 / 3600, false)
	}
	else
	{
		Extent_GCS = Extent;
	}

	//--------------------------------------------------------
	if( !Provide_Tiles(Directory, Extent_GCS, Parameters["DELZIP"].asBool()) )
	{
		return( false );
	}

	//--------------------------------------------------------
	CSG_Data_Manager Data;

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 0);

	if( !pTool || !pTool->Reset() || !pTool->Set_Manager(&Data)
	||  !pTool->Set_Parameter("FILES"      , SG_File_Make_Path(Directory, VRT_NAME, "vrt"))
	||  !pTool->Set_Parameter("EXTENT"     , 1) // "user defined"
	||  !pTool->Set_Parameter("EXTENT_XMIN", Extent_GCS.xMin)
	||  !pTool->Set_Parameter("EXTENT_XMAX", Extent_GCS.xMax)
	||  !pTool->Set_Parameter("EXTENT_XMAX", Extent_GCS.xMax)
	||  !pTool->Set_Parameter("EXTENT_YMIN", Extent_GCS.yMin)
	||  !pTool->Set_Parameter("EXTENT_YMAX", Extent_GCS.yMax)
	||  !pTool->Execute() )
	{
		Error_Fmt("failed to execute tool \'%s\'", _TL("Import Raster"));

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		return( false );
	}

	CSG_Grid *pGrid = pTool->Get_Parameter("GRIDS")->asGridList()->Get_Grid(0);

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	//--------------------------------------------------------
	if( Projection.is_Geographic() )
	{
		pGrid->Set_Name("CGIAR CSI SRTM");

		Parameters.Set_Parameter("SRTM", pGrid);

		return( true );
	}

	Process_Set_Text("%s...", _TL("projection"));

	pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 4);

	if( !pTool || !pTool->Reset() || !pTool->Set_Manager(&Data)
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

	Data.Delete(pGrid); pGrid = pTool->Get_Parameter("GRID")->asGrid();

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	//--------------------------------------------------------
	pGrid->Set_Name("CGIAR CSI SRTM");

	Parameters.Set_Parameter("SRTM", pGrid);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSRTM_CGIAR::Provide_Tiles(const CSG_String &Directory, CSG_Rect Extent, bool DeleteZip)
{
	const double Cellsize = 3. / 3600.;

	Extent.Inflate(Cellsize);

	int Cols[2] = { 1 + int((Extent.xMin + 180.) / 5.), 1 + int((Extent.xMax + 180.) / 5.) };
	if( Cols[1] < Cols[0] ) { int Col = Cols[0]; Cols[0] = Cols[1]; Cols[1] = Col; }
	if( Cols[0] < 1 ) { Cols[0] = 1; } else if( Cols[1] > 72 ) { Cols[1] = 72; }

	int Rows[2] = { 1 + int(( 60. - Extent.yMax) / 5.), 1 + int(( 60. - Extent.yMin) / 5.) };
	if( Rows[1] < Rows[0] ) { int Row = Rows[0]; Rows[0] = Rows[1]; Cols[1] = Row; }
	if( Rows[0] < 1 ) { Rows[0] = 1; } else if( Rows[1] > 24 ) { Rows[1] = 24; }

	int nAdded = 0, nFailed = 0, nFound = 0;

	for(int Col=Cols[0]; Col<=Cols[1]; Col++)
	{
		for(int Row=Rows[0]; Row<=Rows[1]; Row++)
		{
			if( Col >= 1 && Col <= 72 && Row >= 1 && Row <= 24 )
			{
				int Result = Provide_Tile(Directory, CSG_String::Format("srtm_%02d_%02d", Col, Row), DeleteZip);

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
	}

	if( nFailed > 0 )
	{
		Message_Fmt("\n%d download(s) of %d failed", nFailed, nFailed + nAdded);
	}

	if( (nAdded + nFound > 0) || !SG_File_Exists(SG_File_Make_Path(Directory, VRT_NAME, "vrt")) )
	{
		Update_VRT(Directory, VRT_NAME);
	}

	return( nAdded + nFound > 0 );
}

//---------------------------------------------------------
int CSRTM_CGIAR::Provide_Tile(const CSG_String &Directory, const CSG_String &Name, bool DeleteZip)
{
	CSG_String Local_File = SG_File_Make_Path(Directory, Name, "tif");

	if( SG_File_Exists(Local_File) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	Process_Set_Text("%s.zip: %s...", Name.c_str(), _TL("downloading"));

	CSG_CURL Connection("https://srtm.csi.cgiar.org/wp-content/uploads/files/srtm_5x5/TIFF/");

	if( !Connection.is_Connected() )
	{
		Error_Set(_TL("failed to connect to server."));

		return( -1 );
	}

	CSG_String Zip_File = SG_File_Make_Path(Directory, Name, "zip");

	if( !Connection.Request(Name + ".zip", Zip_File.c_str()) )
	{
		Error_Set(_TL("failed to request file."));

		return( -1 );
	}

	//-----------------------------------------------------
	Process_Set_Text("%s.tif: %s...", Name.c_str(), _TL("extracting"));

	CSG_File_Zip Zip(Zip_File);

	if( !Zip.is_Open() || !Zip.Extract(Name + ".tif") )
	{
		Error_Set(_TL("failed to extract zip file."));

		return( -1 );
	}

	Zip.Close();

	if( DeleteZip )
	{
		SG_File_Delete(SG_File_Make_Path(Directory, Name, "zip"));
	}

	//-----------------------------------------------------
	return( 1 );
}

//---------------------------------------------------------
bool CSRTM_CGIAR::Update_VRT(const CSG_String &Directory, const CSG_String &VRT_Name)
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

	if( !pTool
	||  !pTool->Set_Parameter("FILES"   , Tiles)
	||  !pTool->Set_Parameter("VRT_NAME", SG_File_Make_Path(Directory, VRT_Name, "vrt"))
	||  !pTool->Execute() )
	{
		Error_Set(_TL("failed to update Virtual Raster Tiles file"));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
