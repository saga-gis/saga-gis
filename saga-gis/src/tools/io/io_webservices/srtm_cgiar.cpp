
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSRTM_CGIAR::CSRTM_CGIAR(void)
{
	Set_Name		(_TL("CGIAR SRTM"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"Prepare 3arcsec SRTM data for target area. "
	));

	Add_Reference("https://wiki.openstreetmap.org/wiki/Nominatim",
		SG_T("Nominatim at OpenStreetMap Wiki")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_Output("",
		"SRTM"       , _TL("SRTM"),
		_TL("")
	);

	Parameters.Add_FilePath("",
		"TILES"      , _TL("Tiles"),
		_TL(""),
		NULL, NULL, true, true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"EXTENT"     , _TL("Extent"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("user defined"),
			_TL("grid system"),
			_TL("shapes extent")
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

	Parameters.Add_Double("EXTENT", "XMIN", _TL("Left"   ), _TL(""));
	Parameters.Add_Double("EXTENT", "XMAX", _TL("Right"  ), _TL(""));
	Parameters.Add_Double("EXTENT", "YMIN", _TL("Bottom" ), _TL(""));
	Parameters.Add_Double("EXTENT", "YMAX", _TL("Top"    ), _TL(""));
	Parameters.Add_Int   ("EXTENT", "NX"  , _TL("Columns"), _TL(""), 1, 1, true);
	Parameters.Add_Int   ("EXTENT", "NY"  , _TL("Rows"   ), _TL(""), 1, 1, true);

	Parameters.Add_Double("",
		"BUFFER"     , _TL("Buffer"),
		_TL("add buffer (map units) to extent"),
		0., 0., true
	);

	Parameters.Add_Double("",
		"RESOLUTION" , _TL("Resolution"),
		_TL(""),
		90., 0.0001, true
	);

	m_CRS.Create(Parameters);
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
		pParameters->Set_Enabled("GRID_SYSTEM", pParameter->asInt() == 1);
		pParameters->Set_Enabled("GRID"       , pParameter->asInt() == 1);
		pParameters->Set_Enabled("SHAPES"     , pParameter->asInt() == 2);
	}

	if( pParameters->Get_Name().Cmp(Get_Name()) == 0 )
	{
		CSG_Data_Object *pObject =
			(*pParameters)["EXTENT"].asInt() == 1 ? (*pParameters)["GRID"  ].asDataObject() :
			(*pParameters)["EXTENT"].asInt() == 2 ? (*pParameters)["SHAPES"].asDataObject() : NULL;

		pParameters->Set_Enabled("CRS_PICKER", !pObject || !pObject->Get_Projection().is_Okay());
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
		return( false );
	}

	//--------------------------------------------------------
	CSG_Rect Extent, Extent_GCS; CSG_Projection Projection;

	switch( Parameters("EXTENT")->asInt() )
	{
	default: // user defined
		Extent.Create(
			Parameters("XMIN")->asDouble(), Parameters("YMIN")->asDouble(),
			Parameters("XMAX")->asDouble(), Parameters("YMAX")->asDouble()
		);
		break;

	case  1: // grid system
		Extent.Create(Parameters("GRID")->asGrid()->Get_Extent());
		Projection  = Parameters("GRID")->asGrid()->Get_Projection();
		break;

	case  2: // shapes extent
		Extent.Create(Parameters("SHAPES")->asShapes()->Get_Extent());
		Projection  = Parameters("SHAPES")->asShapes()->Get_Projection();
		break;
	}

	if( Parameters("BUFFER")->asDouble() > 0. )
	{
		Extent.Inflate(Parameters("BUFFER")->asDouble(), false);
	}

	if( !Projection.is_Okay() )
	{
		m_CRS.Get_CRS(Projection);

		if( !Projection.is_Okay() )
		{
			return( false );
		}
	}

	double Resolution = Parameters("RESOLUTION")->asDouble();

	//--------------------------------------------------------
	if( !Projection.is_Geographic() )
	{
		Extent.xMin = Resolution * floor(Extent.xMin / Resolution);
		Extent.xMax = Resolution * ceil (Extent.xMax / Resolution);
		Extent.yMin = Resolution * floor(Extent.yMin / Resolution);
		Extent.yMax = Resolution * ceil (Extent.yMax / Resolution);

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
	if( !Provide_Tiles(Directory, Extent_GCS) )
	{
		return( false );
	}

	//--------------------------------------------------------
	CSG_Data_Manager Data;

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 0);

	if( !pTool || !pTool->Reset() || !pTool->Set_Manager(&Data)
	||  !pTool->Set_Parameter("FILES"      , SG_File_Make_Path(Directory, "srtm_global", "vrt"))
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
		Parameters.Set_Parameter("SRTM", pGrid);

		return( true );
	}

	pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 4);

	if( !pTool || !pTool->Reset() || !pTool->Set_Manager(&Data)
	||  !pTool->Set_Parameter("CRS_STRING"       , Projection.Get_WKT())
	||  !pTool->Set_Parameter("SOURCE"           , pGrid)
	||  !pTool->Set_Parameter("RESAMPLING"       ,  3) // B-Spline
	||  !pTool->Set_Parameter("DATA_TYPE"        , 10) // Preserve
	||  !pTool->Set_Parameter("TARGET_DEFINITION",  0)  // 'user defined'
	||  !pTool->Set_Parameter("TARGET_USER_SIZE" , Resolution)
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

	if( nAdded > 0 || !SG_File_Exists(SG_File_Make_Path(Directory, "srtm_global", "vrt")) )
	{
		Update_VRT(Directory, "srtm_global");
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
	CSG_File_Zip Zip_Stream(Zip_File);

	if( !Zip_Stream.is_Open() || !Zip_Stream.Get_File(Name + ".tif") )
	{
		Error_Set(_TL("failed to open zip file."));

		return( -1 );
	}

	CSG_File Tif_Stream(Local_File, SG_FILE_W, true);

	if( !Tif_Stream.is_Open() )
	{
		Error_Set(_TL("failed to create tif file."));

		return( -1 );
	}

	char *Buffer[1024];

	while( !Zip_Stream.is_EOF() )
	{
		size_t nBytes = Zip_Stream.Read(Buffer, sizeof(char), 1024);

		Tif_Stream.Write(Buffer, 1, nBytes);
	}

	Zip_Stream.Close();

	//-----------------------------------------------------
	if( DeleteZip )
	{
		SG_File_Delete(SG_File_Make_Path(Directory, Name, "zip"));
	}

	return( 1 );
}

//---------------------------------------------------------
bool CSRTM_CGIAR::Update_VRT(const CSG_String &Directory, const CSG_String &VRT_Name)
{
	CSG_Strings Files;

	if( !SG_Dir_List_Files(Files, Directory) || Files.Get_Count() < 1 )
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
