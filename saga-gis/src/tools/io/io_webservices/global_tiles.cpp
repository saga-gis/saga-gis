
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
//                   global_tiles.cpp                    //
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
#include "global_tiles.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTiles_Provider::CTiles_Provider(bool bLogin)
{
	m_VRT_Name = "global_tiles"; m_Grid_Name = "grid"; m_Grid_Extension = "tif";

	//-----------------------------------------------------
	if( bLogin )
	{
		Parameters.Add_Node("", "LOGIN", _TL("Login"), _TL("Login needed for server downloads."));
		Parameters.Add_String("LOGIN", "USERNAME", _TL("User"    ), _TL(""), "user");
		Parameters.Add_String("LOGIN", "PASSWORD", _TL("Password"), _TL(""), "", false, true);
	}

	Parameters.Add_Grid_Output("",
		"RESULT"     , _TL("Grid"),
		_TL("")
	);

	Parameters.Add_FilePath("",
		"TILES"      , _TL("Local Tiles Directory"),
		_TL("Download location for tiles. If requested tile is already present download will be skipped."),
		NULL, NULL, true, true
	);

	Parameters.Add_Bool("TILES",
		"DELARCHIVE"  , _TL("Delete Archive Files"),
		_TL("Do not keep archive files after download"),
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
bool CTiles_Provider::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CTiles_Provider::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CTiles_Provider::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
int CTiles_Provider::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
bool CTiles_Provider::On_Execute(void)
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
	if( !Provide_Tiles(Directory, Extent_GCS, Parameters["DELARCHIVE"].asBool()) )
	{
		return( false );
	}

	//--------------------------------------------------------
	CSG_Data_Manager Data;

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 0);

	if( !pTool || !pTool->Reset() || !pTool->Set_Manager(&Data)
	||  !pTool->Set_Parameter("FILES"      , SG_File_Make_Path(Directory, m_VRT_Name, "vrt"))
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
		pGrid->Set_Name(m_Grid_Name);

		Parameters.Set_Parameter("RESULT", pGrid);

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
	pGrid->Set_Name(m_Grid_Name);

	Parameters.Set_Parameter("RESULT", pGrid);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTiles_Provider::Provide_Tiles(const CSG_String &Directory, CSG_Rect Extent, bool DeleteArchive)
{
	CSG_Rect_Int Tiles(Get_Tiles(Extent));

	int nAdded = 0, nFailed = 0, nFound = 0;

	for(int Row=Tiles.yMin; Process_Get_Okay() && Row<=Tiles.yMax; Row++)
	{
		for(int Col=Tiles.xMin; Process_Get_Okay() && Col<=Tiles.xMax; Col++)
		{
			int Result = Provide_Tile(Directory, Col, Row, DeleteArchive);

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
int CTiles_Provider::Provide_Tile(const CSG_String &Directory, int Col, int Row, bool DeleteArchive)
{
	CSG_String File = Get_Tile_Name(Col, Row) + "." + m_Grid_Extension;

	CSG_String Local_File = SG_File_Make_Path(Directory, File);

	if( SG_File_Exists(Local_File) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	CSG_String Archive_Name = Get_Tile_Archive(Col, Row);
	CSG_String Archive_File = SG_File_Make_Path(Directory, Archive_Name);

	if( !SG_File_Exists(Archive_File) )
	{
		Process_Set_Text("%s: %s...", File.c_str(), _TL("downloading"));

		const SG_Char *Username = Parameters("USERNAME") ? Parameters("USERNAME")->asString() : NULL;
		const SG_Char *Password = Parameters("PASSWORD") ? Parameters("PASSWORD")->asString() : NULL;

		CSG_CURL Connection(m_ServerPath, Username, Password);

		Message_Fmt("\n%s: %s%s", _TL("requesting file"), m_ServerPath.c_str(), Archive_Name.c_str());

		SG_UI_Process_Set_Busy(true, CSG_String::Format("%s: %s%s...", _TL("Downloading"), m_ServerPath.c_str(), Archive_Name.c_str()));

		if( !Connection.Request(Archive_Name, Archive_File.c_str()) )
		{
			SG_UI_Process_Set_Busy(false);

			Error_Fmt("%s:\n\n%s%s", _TL("failed to request file from server"), m_ServerPath.c_str(), Archive_Name.c_str());

			return( -1 );
		}

		SG_UI_Process_Set_Busy(false);
	}

	//-----------------------------------------------------
	Process_Set_Text("%s: %s...", File.c_str(), _TL("extracting"));

	CSG_Archive Archive(Archive_File);

	if( !Archive.Extract(Get_Tile_Archive_File(Col, Row)) )
	{
		Error_Fmt("%s: %s", _TL("failed to extract file"), Get_Tile_Archive_File(Col, Row).c_str());

		if( DeleteArchive )
		{
			Archive.Close(); SG_File_Delete(Archive_File);
		}

		return( -1 );
	}

	On_Provide_Tile(Col, Row, Archive);

	if( DeleteArchive )
	{
		Archive.Close(); SG_File_Delete(Archive_File);
	}

	//-----------------------------------------------------
	return( 1 );
}

//---------------------------------------------------------
bool CTiles_Provider::Update_VRT(const CSG_String &Directory)
{
	CSG_Strings Files;

	if( !SG_Dir_List_Files(Files, Directory, m_Grid_Extension) || Files.Get_Count() < 1 )
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
	||  !pTool->Set_Parameter("VRT_NAME", SG_File_Make_Path(Directory, m_VRT_Name, "vrt"))
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
CSRTM_CGIAR::CSRTM_CGIAR(void)
{
	Set_Name		(_TL("SRTM (CGIAR CSI)"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"This tool provides easy-to-use access to the "
		"\'NASA Shuttle Radar Topography Mission Global 3 arc second\' "
		"elevation data (about 90 meter resolution) as provided by "
		"the CGIAR CSI server. "
		"It uses a local database in the chosen directory which provides "
		"the original tiles. If the tiles covering the requested area are "
		"not found in this directory the tool tries to download these "
		"from the CGIAR CSI server. "
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
	m_ServerPath     = "https://srtm.csi.cgiar.org/wp-content/uploads/files/srtm_5x5/TIFF/";

	m_Grid_Name      = "CGIAR CSI SRTM";
	m_Grid_Extension = "tif";

	Parameters.Set_Parameter("CELLSIZE", 90.); // default cellsize
}

//---------------------------------------------------------
CSG_Rect_Int CSRTM_CGIAR::Get_Tiles(const CSG_Rect &_Extent) const
{
	const int nCols = 72, nRows = 24; const double Cellsize = 3. / 3600.;

	CSG_Rect Extent(_Extent); Extent.Inflate(Cellsize); // inflate by one cell

	CSG_Rect_Int Tiles(
		(int)((Extent.xMin + 180.) / 5.),
		(int)(( 60. - Extent.yMax) / 5.),
		(int)((Extent.xMax + 180.) / 5.),
		(int)(( 60. - Extent.yMin) / 5.)
	);

	if( Tiles.xMin < 0 ) { Tiles.xMin = 0; } else if( Tiles.xMax >= nCols ) { Tiles.xMax = nCols - 1; }
	if( Tiles.yMin < 0 ) { Tiles.yMin = 0; } else if( Tiles.yMax >= nRows ) { Tiles.yMax = nRows - 1; }

	return( Tiles );
}

//---------------------------------------------------------
CSG_String CSRTM_CGIAR::Get_Tile_Name(int Col, int Row) const
{
	return( CSG_String::Format("srtm_%02d_%02d", 1 + Col, 1 + Row) );
}

//---------------------------------------------------------
CSG_String CSRTM_CGIAR::Get_Tile_Archive(int Col, int Row) const
{
	return( Get_Tile_Name(Col, Row) + ".zip" );
}

//---------------------------------------------------------
CSG_String CSRTM_CGIAR::Get_Tile_Archive_File(int Col, int Row) const
{
	return( Get_Tile_Name(Col, Row) + ".tif" );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSRTM_USGS::CSRTM_USGS(void) : CTiles_Provider(true)
{
	Set_Name		(_TL("SRTM (USGS)"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"This tool provides easy-to-use access to the "
		"\'NASA Shuttle Radar Topography Mission Global 1 arc second\' "
		"elevation data (about 30 meter resolution). "
		"It uses a local database in the chosen directory which provides "
		"the original tiles. If the tiles covering the requested area are "
		"not found in this directory the tool tries to download these "
		"from the USGS server. "
	));

	Add_Reference("Reuter  H.I,  A.  Nelson,  A.  Jarvis", "2007",
		"An evaluation of void filling interpolation methods for SRTM data",
		"International Journal of Geographic Information Science, 21:9, 983-1008.",
		SG_T("https://doi.org/10.1080/13658810601169899"), SG_T("doi:10.1080/13658810601169899")
	);

	Add_Reference("https://lpdaac.usgs.gov/products/srtmgl1v003/",
		SG_T("USGS EarthData SRTMGL1 v003")
	);

	//-----------------------------------------------------
	// https://e4ftl01.cr.usgs.gov/MEASURES/SRTMGL1.003/2000.02.11/N51E009.SRTMGL1.hgt.zip

	m_ServerPath     = "https://e4ftl01.cr.usgs.gov/MEASURES/SRTMGL1.003/2000.02.11/";

	m_Grid_Name      = "USGS SRTM";
	m_Grid_Extension = "hgt";

	Parameters.Set_Parameter("CELLSIZE", 30.); // default cellsize
}

//---------------------------------------------------------
CSG_Rect_Int CSRTM_USGS::Get_Tiles(const CSG_Rect &_Extent) const
{
	const double Cellsize = 1. / 3600.;

	CSG_Rect Extent(_Extent); Extent.Inflate(Cellsize); // inflate by one cell

	CSG_Rect_Int Tiles(
		(int)Extent.xMin, (int)Extent.yMin,
		(int)Extent.xMax, (int)Extent.yMax
	);

	if( Tiles.xMin < -180 ) { Tiles.xMin = -180; } else if( Tiles.xMax > 179 ) { Tiles.xMax = 179; }
	if( Tiles.yMin <  -56 ) { Tiles.yMin =  -56; } else if( Tiles.yMax >  59 ) { Tiles.yMax =  59; }

	return( Tiles );
}

//---------------------------------------------------------
CSG_String CSRTM_USGS::Get_Tile_Name(int Col, int Row) const
{
	return( CSG_String::Format("%c%02d%c%03d.SRTMGL1",
		Row < 0 ? 'S' : 'N',
		Row < 0 ? abs(Row) + 1 : Row,
		Col < 0 ? 'W' : 'E',
		Col < 0 ? abs(Col) + 1 : Col
	));
}

//---------------------------------------------------------
CSG_String CSRTM_USGS::Get_Tile_Archive(int Col, int Row) const
{
	// http://e4ftl01.cr.usgs.gov/MEASURES/SRTMGL1.003/2000.02.11/S56W180.SRTMGL1.hgt.zip

	return( Get_Tile_Name(Col, Row) + ".hgt.zip" );
}

//---------------------------------------------------------
CSG_String CSRTM_USGS::Get_Tile_Archive_File(int Col, int Row) const
{
	return( Get_Tile_Name(Col, Row) + ".tif" );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCopernicus_DEM::CCopernicus_DEM(void)
{
	Set_Name		(_TL("Copernicus DEM"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"This tool provides easy-to-use access to the \'Copernicus DEM\' "
		"global elevation data with 1 arcsec resolution (about 30 meter). "
		"It uses a local database in the chosen directory which provides "
		"the original tiles. If the tiles covering the requested area are "
		"not found in the directory the tool tries to download these "
		"from the Copernicus server. "
	));

	Add_Reference("https://sentinels.copernicus.eu/web/sentinel/-/copernicus-dem-new-direct-data-download-access/",
		SG_T("Copernicus DEM: new direct data download access")
	);

	//-----------------------------------------------------
	Parameters.Add_Bool("", "MASK", _TL("Water Mask"), _TL("Applies ocean water mask."), true);

	//-----------------------------------------------------
	// https://e4ftl01.cr.usgs.gov/MEASURES/SRTMGL1.003/2000.02.11/N51E009.SRTMGL1.hgt.zip

	m_ServerPath     = "https://prism-dem-open.copernicus.eu/pd-desk-open-access/prismDownload/COP-DEM_GLO-30-DGED__2022_1/";

	m_Grid_Name      = "Copernicus DEM";
	m_Grid_Extension = "tif";

	Parameters.Set_Parameter("CELLSIZE", 30.); // default cellsize
}

//---------------------------------------------------------
CSG_Rect_Int CCopernicus_DEM::Get_Tiles(const CSG_Rect &_Extent) const
{
	const double Cellsize = 1. / 3600.;

	CSG_Rect Extent(_Extent); Extent.Inflate(Cellsize); // inflate by one cell

	CSG_Rect_Int Tiles(
		(int)Extent.xMin, (int)Extent.yMin,
		(int)Extent.xMax, (int)Extent.yMax
	);

	if( Tiles.xMin < -180 ) { Tiles.xMin = -180; } else if( Tiles.xMax > 179 ) { Tiles.xMax = 179; }
	if( Tiles.yMin <  -56 ) { Tiles.yMin =  -56; } else if( Tiles.yMax >  59 ) { Tiles.yMax =  59; }

	return( Tiles );
}

//---------------------------------------------------------
CSG_String CCopernicus_DEM::_Get_Tile_Name(int Col, int Row) const
{
	return( CSG_String::Format("Copernicus_DSM_10_%c%02d_00_%c%03d_00",
		Row < 0 ? 'S' : 'N',
		Row < 0 ? abs(Row) + 1 : Row,
		Col < 0 ? 'W' : 'E',
		Col < 0 ? abs(Col) + 1 : Col
	));
}

CSG_String CCopernicus_DEM::Get_Tile_Name(int Col, int Row) const
{
	return( _Get_Tile_Name(Col, Row) + "_DEM" );
}

//---------------------------------------------------------
CSG_String CCopernicus_DEM::Get_Tile_Archive(int Col, int Row) const
{
	return( Get_Tile_Name(Col, Row) + ".tar" );
}

//---------------------------------------------------------
CSG_String CCopernicus_DEM::Get_Tile_Archive_File(int Col, int Row) const
{
#ifdef _SAGA_MSW
	return( _Get_Tile_Name(Col, Row) + "\\DEM\\" + Get_Tile_Name(Col, Row) + ".tif" );
#else
	return( _Get_Tile_Name(Col, Row) +  "/DEM/"  + Get_Tile_Name(Col, Row) + ".tif" );
#endif
}

//---------------------------------------------------------
bool CCopernicus_DEM::On_Provide_Tile(int Col, int Row, CSG_Archive &Archive)
{
	if( !Parameters["MASK"].asBool() )
	{
		return( true );
	}

	CSG_Grid DEM, Mask; CSG_String Name(_Get_Tile_Name(Col, Row)), Directory(SG_File_Get_Path(Archive.Get_Archive()));

	if( DEM.Create(SG_File_Make_Path(Directory, Name + "_DEM.tif")) )
	{
		#ifdef _SAGA_MSW
		CSG_String File(Name + "\\AUXFILES\\" + Name + "_WBM.tif" );
		#else
		CSG_String File(Name +  "/AUXFILES/"  + Name + "_WBM.tif" );
		#endif

		if( Archive.Extract(File) )
		{
			File = SG_File_Make_Path(Directory, Name + "_WBM.tif");

			if( Mask.Create(File) && Mask.Get_System() == DEM.Get_System() )
			{
				for(sLong i=0; i<DEM.Get_NCells(); i++)
				{
					if( Mask.asInt(i) == 1 )
					{
						DEM.Set_NoData(i);
					}
				}

				DEM.Save("");

				SG_File_Delete(File);

				return( true );
			}

			SG_File_Delete(File);
		}
	}

	return( false );
}



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// https://www.eorc.jaxa.jp/ALOS/aw3d30/data/release_v2404/N050E010/N051E011.zip


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
