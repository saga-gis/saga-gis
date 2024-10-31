
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_compound                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     channels.cpp                      //
//                                                       //
//                 Copyright (C) 2024 by                 //
//                      Olaf Conrad                      //
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
#include "channels.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CChannels::CChannels(void)
{
	Set_Name		(_TL("Compound Channel Network Analysis"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"A one-step tool for the creation of selected terrain parameters related to channels."
		"For details look at the description of the tools used for processing:<ul>"
		"<li>[ta_preproc] Sink Removal</li>"
		"<li>[ta_preproc] Fill Sinks (Wang & Liu)</li>"
		"<li>[ta_preproc] Breach Depressions</li>"
		"<li>[ta_channels] Channel Network and Drainage Basins</li>"
		"<li>[ta_channels] Watershed Basins (Extended)</li>"
		"<li>[ta_channels] Overland Flow Distance to Channel Network</li>"
		"<li>[ta_channels] Vertical Distance to Channel Network</li>"
		"</ul>"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid  ("", "ELEVATION"    , _TL("Elevation"                        ), _TL(""), PARAMETER_INPUT );

	Parameters.Add_Shapes("", "CHANNELS"     , _TL("Channel Network"                  ), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Line   );
	Parameters.Add_Shapes("", "BASINS"       , _TL("Drainage Basins"                  ), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes("", "SUBBASINS"    , _TL("Drainage Sub-Basins"              ), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes("", "HEADS"        , _TL("Channel Heads"                    ), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Point  );
	Parameters.Add_Shapes("", "MOUTHS"       , _TL("Channel Mouths"                   ), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Point  );
	Parameters.Add_Grid  ("", "FLOW_DISTANCE", _TL("Overland Flow Distance"           ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "FLOW_DISTHORZ", _TL("Horizontal Overland Flow Distance"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "FLOW_DISTVERT", _TL("Vertical Overland Flow Distance"  ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CHNL_BASE"    , _TL("Channel Network Base Level"       ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CHNL_DIST"    , _TL("Channel Network Distance"         ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice("",
		"METHOD_PREPROC", _TL("Preprocessing"), 
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Sink Removal"),
			_TL("Fill Sinks (Wang & Liu)"),
			_TL("Breach Depressions"),
			_TL("none")
		), 0
	);

	Parameters.Add_Int("",
		"THRESHOLD", _TL("Channel Density"), 
		_TL("Strahler order to begin a channel."), 
		5, 1, true
	);

	//-----------------------------------------------------
	if( has_GUI() )
	{
		Parameters.Add_Bool("",
			"MAP"      , _TL("Open Map"), 
			_TL(""), 
			true
		)->Set_UseInCMD(false);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannels::On_Execute(void)
{
	CSG_Grid *pDEM, DEM, Channels;

	switch( Parameters["METHOD_PREPROC"].asInt() )
	{
	default: // Sink Removal
		SG_RUN_TOOL_ExitOnError("ta_preprocessor", 2,
		        SG_TOOL_PARAMETER_SET("DEM"        , Parameters("ELEVATION"))
		    &&  SG_TOOL_PARAMETER_SET("DEM_PREPROC", pDEM = &DEM)
		);
		break;

	case  1: // Fill Sinks (Wang & Liu)
		SG_RUN_TOOL_ExitOnError("ta_preprocessor", 4,
		        SG_TOOL_PARAMETER_SET("ELEV"       , Parameters("ELEVATION"))
		    &&  SG_TOOL_PARAMETER_SET("FILLED"     , pDEM = &DEM)
		);
		break;

	case  2: // Breach Depressions
		SG_RUN_TOOL_ExitOnError("ta_preprocessor", 7,
		        SG_TOOL_PARAMETER_SET("DEM"        , Parameters("ELEVATION"))
		    &&  SG_TOOL_PARAMETER_SET("NOSINKS"    , pDEM = &DEM)
		);
		break;

	case  3: // none
		pDEM = Parameters("ELEVATION")->asGrid();
		break;
	}

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_channels"        ,  5, // Channel Network and Drainage Basins
		    SG_TOOL_PARAMETER_SET("DEM"          , pDEM)
		&&  SG_TOOL_PARAMETER_SET("ORDER"        , &Channels)
		&&  SG_TOOL_PARAMETER_SET("SEGMENTS"     , Parameters("CHANNELS" ))
		&&  SG_TOOL_PARAMETER_SET("THRESHOLD"    , Parameters("THRESHOLD"))
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_channels"        ,  2, // Watershed Basins (Extended)
		    SG_TOOL_PARAMETER_SET("DEM"          , pDEM)
		&&  SG_TOOL_PARAMETER_SET("CHANNELS"     , &Channels)
		&&  SG_TOOL_PARAMETER_SET("V_BASINS"     , Parameters("BASINS"   ))
		&&  SG_TOOL_PARAMETER_SET("V_SUBBASINS"  , Parameters("SUBBASINS"))
		&&  SG_TOOL_PARAMETER_SET("HEADS"        , Parameters("HEADS"    ))
		&&  SG_TOOL_PARAMETER_SET("MOUTHS"       , Parameters("MOUTHS"   ))
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_channels"        ,  4, // Overland Flow Distance to Channel Network
		    SG_TOOL_PARAMETER_SET("ELEVATION"    , pDEM)
		&&  SG_TOOL_PARAMETER_SET("CHANNELS"     , &Channels)
		&&  SG_TOOL_PARAMETER_SET("DISTANCE"     , Parameters("FLOW_DISTANCE"))
		&&  SG_TOOL_PARAMETER_SET("DISTVERT"     , Parameters("FLOW_DISTVERT"))
		&&  SG_TOOL_PARAMETER_SET("DISTHORZ"     , Parameters("FLOW_DISTHORZ"))
		&&  SG_TOOL_PARAMETER_SET("BOUNDARY"     , true)
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_channels"        ,  3, // Vertical Distance to Channel Network
		    SG_TOOL_PARAMETER_SET("ELEVATION"    , pDEM)
		&&  SG_TOOL_PARAMETER_SET("CHANNELS"     , &Channels)
		&&  SG_TOOL_PARAMETER_SET("DISTANCE"     , Parameters("CHNL_DIST"))
		&&  SG_TOOL_PARAMETER_SET("BASELEVEL"    , Parameters("CHNL_BASE"))
	);

	//-----------------------------------------------------
	if( has_GUI() && Parameters["MAP"].asBool() )
	{
		CSG_Data_Object *pObject;

		pObject = Parameters["CHNL_DIST"].asDataObject();
		DataObject_Set_Colors   (pObject, 5, SG_COLORS_TOPOGRAPHY);
		DataObject_Set_Parameter(pObject, "DISPLAY_SHADING", (int)1); // traditional

		pObject = Parameters["BASINS"   ].asDataObject();
	//	DataObject_Set_Parameter(pObject, "SINGLE_COLOR"   , (int)SG_COLOR_GREY_LIGHT);
		DataObject_Set_Parameter(pObject, "DISPLAY_BRUSH"  , (int)1); // transparent
		DataObject_Set_Parameter(pObject, "OUTLINE_COLOR"  , (int)SG_COLOR_RED);
		DataObject_Set_Parameter(pObject, "OUTLINE_SIZE"   , (int)4);

		pObject = Parameters["SUBBASINS"].asDataObject();
		DataObject_Set_Parameter(pObject, "DISPLAY_BRUSH"  , (int)1); // transparent
		DataObject_Set_Parameter(pObject, "OUTLINE_COLOR"  , (int)SG_COLOR_RED);
		DataObject_Set_Parameter(pObject, "OUTLINE_SIZE"   , (int)1);

		pObject = Parameters["CHANNELS" ].asDataObject();
		DataObject_Set_Parameter(pObject, "SINGLE_COLOR"   , (int)SG_COLOR_BLUE);
		DataObject_Set_Parameter(pObject, "SIZE_ATTRIB"    , (int) 4); // ORDER
		DataObject_Set_Parameter(pObject, "SIZE_RANGE.MIN" , (int) 2);
		DataObject_Set_Parameter(pObject, "SIZE_RANGE.MIN" , (int)10);

		pObject = Parameters["HEADS"    ].asDataObject();
		DataObject_Set_Parameter(pObject, "SIZE_DEFAULT"   , (int)3);
		DataObject_Set_Parameter(pObject, "SINGLE_COLOR"   , (int)SG_COLOR_WHITE);
		DataObject_Set_Parameter(pObject, "OUTLINE_COLOR"  , (int)SG_COLOR_RED);

		pObject = Parameters["MOUTHS"   ].asDataObject();
		DataObject_Set_Parameter(pObject, "SIZE_DEFAULT"   , (int)3);
		DataObject_Set_Parameter(pObject, "SINGLE_COLOR"   , (int)SG_COLOR_WHITE);
		DataObject_Set_Parameter(pObject, "OUTLINE_COLOR"  , (int)SG_COLOR_BLUE);

		DataObject_Update(Parameters["CHNL_DIST"].asDataObject(), SG_UI_DATAOBJECT_SHOW_MAP       );
		DataObject_Update(Parameters["BASINS"   ].asDataObject(), SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE);
		DataObject_Update(Parameters["SUBBASINS"].asDataObject(), SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE);
		DataObject_Update(Parameters["CHANNELS" ].asDataObject(), SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE);
		DataObject_Update(Parameters["HEADS"    ].asDataObject(), SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE);
		DataObject_Update(Parameters["MOUTHS"   ].asDataObject(), SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
