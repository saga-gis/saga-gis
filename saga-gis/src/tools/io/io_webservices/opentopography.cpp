
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
//                  opentopography.cpp                   //
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
#include "opentopography.h"


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
struct SDEMTypes { double size; CSG_String type, name; } Types[] =
{
	{   90., "SRTMGL3"        , "SRTM GL3 90m"                        },
	{   30., "SRTMGL1"        , "SRTM GL1 30m"                        },
	{   30., "SRTMGL1_E"      , "SRTM GL1 Ellipsoidal 30m"            },
	{   30., "AW3D30"         , "ALOS World 3D 30m"                   },
	{   30., "AW3D30_E"       , "ALOS World 3D Ellipsoidal, 30m"      },
	{  500., "SRTM15Plus"     , "Global Bathymetry SRTM15+ V2.1 500m" },
	{   30., "NASADEM"        , "NASADEM Global DEM"                  },
	{   30., "COP30"          , "Copernicus Global DSM 30m"           },
	{   90., "COP90"          , "Copernicus Global DSM 90m"           },
	{   30., "EU_DTM"         , "EU DTM 30m"                          },
	{ 1000., "GEDI_L3"        , "GEDI_L3 DTM 1000m"                   },
	{  500., "GEBCOIceTopo"   , "Global Ice Bathymetry 500m"          },
	{  500., "GEBCOSubIceTopo", "Global Sub-Ice Bathymetry 500m"      }
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenTopography::COpenTopography(bool bLogin)
{
	Set_Name		(_TL("Global OpenTopography"));

	Set_Author		("O.Conrad (c) 2025");

	Set_Description	(_TW(
		"Download and optionally project global digital elevation model (DEM) "
		"data for the area of your interest provided by the OpenTopography project. "
		"The download needs a personal <b>API key</b> which can be obtained easily "
		"after login at the OpenTopography Data Portal.\n"
		"\nAvailable DEM products are: "
		"<ul>"
		"<li>SRTMGL3 (SRTM GL3 90m)</li>"
		"<li>SRTMGL1 (SRTM GL1 30m)</li>"
		"<li>SRTMGL1_E (SRTM GL1 Ellipsoidal 30m)</li>"
		"<li>AW3D30 (ALOS World 3D 30m)</li>"
		"<li>AW3D30_E (ALOS World 3D Ellipsoidal, 30m)</li>"
		"<li>SRTM15Plus (Global Bathymetry SRTM15+ V2.1 500m)</li>"
		"<li>NASADEM (NASADEM Global DEM)</li>"
		"<li>COP30 (Copernicus Global DSM 30m)</li>"
		"<li>COP90 (Copernicus Global DSM 90m)</li>"
		"<li>EU_DTM (DTM 30m)</li>"
		"<li>GEDI_L3 (DTM 1000m)</li>"
		"<li>GEBCOIceTopo (Global Bathymetry 500m)</li>"
		"<li>GEBCOSubIceTopo (Global Bathymetry 500m)</li>"
		"</ul>"
	));

	Add_Reference("https://opentopography.org/"       , SG_T("OpenTopography Homepage"   ));
	Add_Reference("https://portal.opentopography.org/", SG_T("OpenTopography Data Portal"));


	//-----------------------------------------------------
	Parameters.Add_Grid_Output("",
		"RESULT"     , _TL("Grid"),
		_TL("")
	);

	Parameters.Add_Choice("",
		"DEMTYPE"    , _TL("DEM Type"),
		_TL(""),
		"{SRTMGL3}"         "SRTM GL3 90m|"
		"{SRTMGL1}"         "SRTM GL1 30m|"
		"{SRTMGL1_E}"       "SRTM GL1 Ellipsoidal 30m|"
		"{AW3D30}"          "ALOS World 3D 30m|"
		"{AW3D30_E}"        "ALOS World 3D Ellipsoidal, 30m|"
		"{SRTM15Plus}"      "Global Bathymetry SRTM15+ V2.1 500m|"
		"{NASADEM}"         "NASADEM Global DEM|"
		"{COP30}"           "Copernicus Global DSM 30m|"
		"{COP90}"           "Copernicus Global DSM 90m|"
		"{EU_DTM}"          "EU DTM 30m|"
		"{GEDI_L3}"         "GEDI_L3 DTM 1000m|"
		"{GEBCOIceTopo}"    "Global Bathymetry 500m (Ice)|"
		"{GEBCOSubIceTopo}" "Global Bathymetry 500m (Sub-Ice)|",
		3 // => AW3D30
	);

	Parameters.Add_String("",
		"API_KEY"    , _TL("API Key"),
		_TL(""),
		"", false, true
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

	Parameters.Add_Double("EXTENT", "XMIN", _TL("West"   ), _TL(""),  538350.);
	Parameters.Add_Double("EXTENT", "XMAX", _TL("East"   ), _TL(""),  598020.);
	Parameters.Add_Double("EXTENT", "YMIN", _TL("South"  ), _TL(""), 5906910.);
	Parameters.Add_Double("EXTENT", "YMAX", _TL("North"  ), _TL(""), 5965170.);
	Parameters.Add_Int   ("EXTENT", "NX"  , _TL("Columns"), _TL(""),    1990, 1, true);
	Parameters.Add_Int   ("EXTENT", "NY"  , _TL("Rows"   ), _TL(""),    1943, 1, true);

	Parameters.Add_Double("",
		"BUFFER"     , _TL("Buffer"),
		_TL("add buffer (map units) to extent"),
		0., 0., true
	);

	Parameters.Add_Choice("",
		"CELLSIZEDEF", _TL("Take Cellsize"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("data type resolution"),
			_TL("user defined")
		), 0
	);

	Parameters.Add_Double("CELLSIZEDEF",
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
bool COpenTopography::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool COpenTopography::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int COpenTopography::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("CELLSIZE") || (pParameter->Get_Parent() && pParameter->Get_Parent()->Cmp_Identifier("EXTENT")) )
	{
		double Cellsize = (*pParameters)("CELLSIZEDEF")->asInt() == 1
			? (*pParameters)("CELLSIZE")->asDouble() // user defined
			: Types[(*pParameters)("DEMTYPE")->asInt()].size;

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
int COpenTopography::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
		pParameters->Set_Enabled("CELLSIZEDEF", pParameter->asInt() != 3);
		pParameters->Set_Enabled("BUFFER"     , pParameter->asInt() == 1 || pParameter->asInt() == 2);
	}

	if( pParameter->Cmp_Identifier("CELLSIZEDEF") )
	{
		pParameters->Set_Enabled("CELLSIZE"   , pParameter->asInt() == 1); // user defined
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
bool COpenTopography::On_Execute(void)
{
	CSG_Rect Extent, Extent_GCS; CSG_Projection Projection; int Type = Parameters("DEMTYPE")->asInt();

	double Cellsize = Parameters("CELLSIZEDEF")->asInt() == 1 ? Parameters("CELLSIZE")->asDouble() : Types[Type].size;

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
	CSG_CURL Connection("https://portal.opentopography.org");

	CSG_String Request = CSG_String::Format("/API/globaldem?demtype=%s&south=%f&north=%f&west=%f&east=%f&outputFormat=GTiff&API_Key=%s",
		Types[Type].type.c_str(),
		Extent_GCS.yMin, Extent_GCS.yMax,
		Extent_GCS.xMin, Extent_GCS.xMax,
		Parameters("API_KEY")->asString()
	);

	Message_Fmt("\nrequesting:\n  demtype = %s, south = %f, north = %f, west = %f, east = %f\n",
		Types[Type].type.c_str(),
		Extent_GCS.yMin, Extent_GCS.yMax,
		Extent_GCS.xMin, Extent_GCS.xMax
	);

	CSG_String File(SG_File_Get_Name_Temp("sg")); SG_File_Delete(File); File += ".tif";

	SG_UI_Process_Set_Busy(true, CSG_String::Format("%s: %s...", _TL("downloading"), Types[Type].name.c_str()));

	if( !Connection.Request(Request, File.c_str()) )
	{
		SG_UI_Process_Set_Busy(false);

		Message_Fmt("\n%s [%s]", _TL("Request failed."), Request.c_str());

		return( false );
	}

	SG_UI_Process_Set_Busy(false);

	//-----------------------------------------------------
	SG_UI_Msg_Lock(true);
	CSG_Grid *pGrid = SG_Create_Grid(File);
	SG_UI_Msg_Lock(false);

	if( !pGrid->is_Valid() )
	{
		Message_Fmt("\n%s [%s]", _TL("Raster import failed."), File.c_str());

		return( false );
	}

	SG_File_Delete(File); // remove temporary file

	//--------------------------------------------------------
	if( Projection.is_Geographic() || Projection.is_Geodetic() || !pGrid->Get_Projection().is_Okay() )
	{
		if( Parameters("RESULT")->asGrid() )
		{
			Parameters("RESULT")->asGrid()->Create(*pGrid); delete(pGrid);

			pGrid = Parameters("RESULT")->asGrid();
		}
		else
		{
			Parameters.Set_Parameter("RESULT", pGrid);
		}

		pGrid->Set_Name(Types[Type].name);

		return( true );
	}

	//--------------------------------------------------------
	Process_Set_Text("%s...", _TL("projection"));

	CSG_Data_Manager Data; Data.Add(pGrid);

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 4);

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
	pGrid->Set_Name(Types[Type].name);

	if( Parameters("RESULT")->asGrid() )
	{
		Parameters("RESULT")->asGrid()->Create(*pGrid); delete(pGrid);
	}
	else
	{
		Parameters.Set_Parameter("RESULT", pGrid);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
