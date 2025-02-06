
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
//                 usgs_earthquakes.cpp                  //
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
#include "usgs_earthquakes.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CUSGS_Earthquakes::CUSGS_Earthquakes(void)
{
	Set_Name		(_TL("USGS Earthquake Catalog"));

	Set_Author		("O.Conrad (c) 2025");

	Set_Description	(_TW(
		"Request earthquake data from the USGS Earthquake Catalog."
	));

	Add_Reference("https://earthquake.usgs.gov/earthquakes/search/", SG_T("USGS Earthquake Catalog"));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"EARTHQUAKES", _TL("Earthquakes"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	//-----------------------------------------------------
	Parameters.Add_Date("",
		"DATE_START" , _TL("Start"),
		_TL(""),
		CSG_DateTime::Now().Get_JDN() - 7.
	);

	Parameters.Add_Date("",
		"DATE_END"   , _TL("End"),
		_TL(""),
		CSG_DateTime::Now().Get_JDN()
	);

	Parameters.Add_Range("",
		"MAGNITUDE"  , _TL("Magnitude"),
		_TL(""),
		4., 10., 0., true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"EXTENT"     , _TL("Extent"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("user defined"),
			_TL("shapes extent"),
			_TL("grid system extent")
		), 0
	);

	Parameters.Add_Grid_System("EXTENT",
		"GRID_SYSTEM", _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Grid("GRID_SYSTEM",
		"GRID"       , _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes("EXTENT",
		"SHAPES"     , _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Double("EXTENT", "XMIN", _TL("West"   ), _TL(""),  -180.);
	Parameters.Add_Double("EXTENT", "XMAX", _TL("East"   ), _TL(""),   180.);
	Parameters.Add_Double("EXTENT", "YMIN", _TL("South"  ), _TL(""),   -90.);
	Parameters.Add_Double("EXTENT", "YMAX", _TL("North"  ), _TL(""),    90.);

	Parameters.Add_Double("",
		"BUFFER"     , _TL("Buffer"),
		_TL("add buffer (map units) to extent"),
		0., 0., true
	);

	m_CRS.Create(Parameters); Parameters.Set_Parameter("CRS_STRING", "epsg:4326"); m_CRS.On_Parameter_Changed(&Parameters, Parameters("CRS_STRING"));
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CUSGS_Earthquakes::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CUSGS_Earthquakes::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CUSGS_Earthquakes::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_CRS.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CUSGS_Earthquakes::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("EXTENT") )
	{
		pParameters->Set_Enabled("XMIN"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("XMAX"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("YMIN"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("YMAX"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("SHAPES"     , pParameter->asInt() == 1);
		pParameters->Set_Enabled("GRID_SYSTEM", pParameter->asInt() == 2);
		pParameters->Set_Enabled("BUFFER"     , pParameter->asInt() == 1 || pParameter->asInt() == 2);
	}

	if( (*pParameters)("EXTENT") )//pParameters->Get_Name().Cmp(Get_Name()) == 0 )
	{
		CSG_Data_Object *pObject =
			(*pParameters)["EXTENT"].asInt() == 1 ? (*pParameters)["SHAPES"].asDataObject() :
			(*pParameters)["EXTENT"].asInt() == 2 ? (*pParameters)["GRID"  ].asDataObject() : NULL;

		pParameters->Set_Enabled("CRS_PICKER", !SG_Get_Data_Manager().Exists(pObject) || !pObject->Get_Projection().is_Okay());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CUSGS_Earthquakes::On_Execute(void)
{
	CSG_Rect Extent, Extent_GCS; CSG_Projection Projection;

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
		Extent.Create(Parameters("GRID_SYSTEM")->asGrid_System()->Get_Extent());

		if( Parameters("BUFFER")->asDouble() > 0. )
		{
			Extent.Inflate(Parameters("BUFFER")->asDouble(), false);
		}

		if( Parameters("GRID")->asGrid() )
		{
			Projection = Parameters("GRID")->asGrid()->Get_Projection();
		}
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

	//-----------------------------------------------------
	if( !Projection.is_Geographic() )
	{
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

	//-----------------------------------------------------
	CSG_CURL Connection("https://earthquake.usgs.gov");

	CSG_String Request("/fdsnws/event/1/query?format=xml");

	Request += CSG_String::Format("&%s=%s", SG_T("starttime"   ), Parameters["DATE_START"].asString());
	Request += CSG_String::Format("&%s=%s", SG_T("endtime"     ), Parameters["DATE_END"  ].asString());

	Request += CSG_String::Format("&%s=%f", SG_T("minlongitude"), Extent_GCS.xMin);
	Request += CSG_String::Format("&%s=%f", SG_T("maxlongitude"), Extent_GCS.xMax);
	Request += CSG_String::Format("&%s=%f", SG_T("minlatitude" ), Extent_GCS.yMin);
	Request += CSG_String::Format("&%s=%f", SG_T("maxlatitude" ), Extent_GCS.yMax);

	Request += CSG_String::Format("&%s=%f", SG_T("minmagnitude"), Parameters["MAGNITUDE.MIN"].asDouble());
	Request += CSG_String::Format("&%s=%f", SG_T("maxmagnitude"), Parameters["MAGNITUDE.MAX"].asDouble());

	//-----------------------------------------------------
	Message_Fmt("\n%s:\n  %s\n", _TL("requesting"), Request.c_str());

	Process_Set_Text                               ("%s: %s...", _TL("requesting"), _TL("USGS Earthquake Catalog"));
	SG_UI_Process_Set_Busy(true, CSG_String::Format("%s: %s...", _TL("requesting"), _TL("USGS Earthquake Catalog")));

	CSG_String Response;

	if( !Connection.Request(Request, Response) )
	{
		SG_UI_Process_Set_Busy(false);

		Message_Fmt("\n%s:\"https://earthquake.usgs.gov%s\"", _TL("Request failed."), Request.c_str());

		return( false );
	}

	SG_UI_Process_Set_Busy(false);

	//-----------------------------------------------------
	if( Response.is_Empty() )
	{
		Error_Fmt  ("%s:\n\"https://earthquake.usgs.gov%s\"", _TL("Request failed."), Request.c_str());

		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData Data;

	if( !Data.from_XML(Response) || !Data.Cmp_Name("q:quakeml") || !Data("eventParameters") )
	{
		Error_Fmt  ("%s:\n\"https://earthquake.usgs.gov%s\"", _TL("Invalid QuakeML Format."), Request.c_str());

		return( false );
	}

	//-----------------------------------------------------
	struct SFields { TSG_Data_Type type; const char *id, *tag, *prop; };

	const struct SFields Fields[] = {
		{ SG_DATATYPE_String, "SourceID"              , "origin"                            , "catalog:dataid" },
		{ SG_DATATYPE_String, "Time"                  , "origin.time.value"                             , NULL },
		{ SG_DATATYPE_String, "Creation Time"         , "creationInfo.creationTime"                     , NULL },
		{ SG_DATATYPE_Double, "Horizontal Uncertainty", "origin.originUncertainty.horizontalUncertainty", NULL },
		{ SG_DATATYPE_Double, "Depth"                 , "origin.depth.value"                            , NULL },
		{ SG_DATATYPE_Double, "Depth Uncertainty"     , "origin.depth.uncertainty"                      , NULL },
		{ SG_DATATYPE_Double, "Magnitude"             , "magnitude.mag.value"                           , NULL },
		{ SG_DATATYPE_Double, "Magnitude Uncertainty" , "magnitude.mag.uncertainty"                     , NULL },
		{ SG_DATATYPE_String, "Magnitude Type"        , "magnitude.type"                                , NULL },
		{ SG_DATATYPE_String, "Type"                  , "type"                                          , NULL },
		{ SG_DATATYPE_String, "Description"           , "description.text"                              , NULL },
		{ SG_DATATYPE_Undefined, NULL , NULL, NULL }
	};

	CSG_Shapes &Points = *Parameters["EARTHQUAKES"].asShapes();

	Points.Create(SHAPE_TYPE_Point, NULL, NULL, SG_VERTEX_TYPE_XYZ);
	Points.Fmt_Name("%s [%s - %s]", _TL("Earthquakes"), Parameters["DATE_START"].asString(), Parameters["DATE_END"].asString());
	Points.Get_Projection().Set_GCS_WGS84();

	for(int Field=0; Fields[Field].type != SG_DATATYPE_Undefined; Field++)
	{
		Points.Add_Field(Fields[Field].id, Fields[Field].type);
	}

	//-----------------------------------------------------
	CSG_MetaData &Events = Data["eventParameters"];

	for(int i=0; i<Events.Get_Count(); i++)
	{
		CSG_MetaData &Event = Events[i]; CSG_Point_3D p;

		if( Event.Get_Content("origin.longitude.value", p.x)
		&&  Event.Get_Content("origin.latitude.value" , p.y) )
		{
			Event.Get_Content("origin.depth.value"    , p.z);

			CSG_Shape &Point = *Points.Add_Shape(); Point.Set_Point(p);

			for(int Field=0; Field<Points.Get_Field_Count(); Field++)
			{
				CSG_String Value;

				if( Fields[Field].prop == NULL )
				{
					Event.Get_Content(Fields[Field].tag, Value);
				}
				else if( Event(Fields[Field].tag) )
				{
					Event[Fields[Field].tag].Get_Property(Fields[Field].prop, Value);
				}

				if( Value.is_Empty() )
				{
					Point.Set_NoData(Field);
				}
				else
				{
					Point.Set_Value(Field, Value);
				}
			}
		}
	}

	//--------------------------------------------------------
	if( Points.Get_Projection() != Projection )
	{
		Process_Set_Text("%s...", _TL("projection"));

		CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 2); // Coordinate Transformation (Shapes)

		CSG_Data_Manager Manager; Manager.Add(&Points);

		if( !pTool || !pTool->Set_Manager(&Manager)
		||  !pTool->Set_Parameter("CRS_STRING", Projection.Get_WKT())
		||  !pTool->Set_Parameter("SOURCE"    , &Points)
		||  !pTool->Set_Parameter("COPY"      , false)
		||  !pTool->Set_Parameter("PARALLEL"  , true)
		||  !pTool->Execute() )
		{
			Error_Fmt("failed to execute tool \'%s\'", _TL("Coordinate Transformation (Grid)"));

			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

			return( false );
		}

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
	}

	//-----------------------------------------------------
	Points.Get_MetaData().Del_Child("USGS Earthquake Catalog Request");

	CSG_MetaData &MetaData = *Points.Get_MetaData().Add_Child("USGS Earthquake Catalog Request");

	MetaData.Add_Child("Request"          , "https://earthquake.usgs.gov" + Request);
	MetaData.Add_Child("Date Start"       , Parameters["DATE_START"].asString());
	MetaData.Add_Child("Date End"         , Parameters["DATE_END"  ].asString());
	MetaData.Add_Child("Extent West"      , Extent_GCS.xMin);
	MetaData.Add_Child("Extent East"      , Extent_GCS.xMax);
	MetaData.Add_Child("Extent South"     , Extent_GCS.yMin);
	MetaData.Add_Child("Extent North"     , Extent_GCS.yMax);
	MetaData.Add_Child("Minimum Magnitude", Parameters["MAGNITUDE.MIN"].asDouble());
	MetaData.Add_Child("Maximum Magnitude", Parameters["MAGNITUDE.MAX"].asDouble());

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
