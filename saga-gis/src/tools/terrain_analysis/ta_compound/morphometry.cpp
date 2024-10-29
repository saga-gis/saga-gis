
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
//                   morphometry.cpp                     //
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
#include "morphometry.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMorphometry::CMorphometry(void)
{
	Set_Name		(_TL("Compound Morphometric Terrain Analysis"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"A one-step tool for the creation of selected terrain parameters related to morphometry."
		"For details look at the description of the tools used for processing:<ul>"
		"<li>[ta_morphometry] Slope, Aspect, Curvature</li>"
		"<li>[ta_morphometry] Convergence Index (Search Radius)</li>"
		"<li>[ta_morphometry] Upslope and Downslope Curvature</li>"
		"<li>[ta_morphometry] Mass Balance Index</li>"
		"<li>[ta_hydrology] Cell Balance</li>"
		"<li>[ta_morphometry] Topographic Position Index</li>"
		"<li>[ta_morphometry] Terrain Surface Convexity</li>"
		"<li>[ta_morphometry] Terrain Surface Texture</li>"
		"<li>[ta_morphometry] Terrain Ruggedness Index</li>"
		"<li>[ta_morphometry] Vector Ruggedness Measure</li>"
		"<li>[ta_morphometry] Morphometric Protection Index</li>"
		"<li>[ta_lighting] Topographic Openness</li>"
		"</ul>"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid  ("", "ELEVATION"    , _TL("Elevation"                     ), _TL(""), PARAMETER_INPUT );

	Parameters.Add_Grid  ("", "SURFACE_AREA"     , _TL("Real Surface Area"            ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "SLOPE"            , _TL("Slope"                        ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "ASPECT"           , _TL("Aspect"                       ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "NORTHNESS"        , _TL("Northness"                    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "EASTNESS"         , _TL("Eastness"                     ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CONVERGENCE"      , _TL("Convergence Index"            ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CURVE_PLAN"       , _TL("Plan Curvature"               ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CURVE_CROSS"      , _TL("Cross-Sectional Curvature"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CURVE_PROFILE"    , _TL("Profile Curvature"            ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CURVE_LONGITUDE"  , _TL("Longitudinal Curvature"       ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CURVE_DOWNSLOPE"  , _TL("Downslope Curvature"          ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CURVE_UPSLOPE"    , _TL("Upslope Curvature"            ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "MASS_BALANCE"     , _TL("Mass Balance Index"           ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CELL_BALANCE"     , _TL("Cell Balance"                 ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "TPI"              , _TL("Topographic Position Index"   ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "TERRAIN_CONVEXITY", _TL("Terrain Surface Convexity"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "TERRAIN_TEXTURE"  , _TL("Terrain Surface Texture"      ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "TRI"              , _TL("Terrain Ruggedness Index"     ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "VRM"              , _TL("Vector Ruggedness Measure"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "PROTECTION"       , _TL("Morphometric Protection Index"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "OPENNESS_POS"     , _TL("Positive Topographic Openness"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "OPENNESS_NEG"     , _TL("Negative Topographic Openness"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Int("", "SCALE", _TL("Scale"), _TL("Targeted scale (cells). Applies to \"Convergence Index\"."), 4, 1, true);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMorphometry::On_Execute(void)
{
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     ,  6, // Real Surface Area
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"   ))
		&&  SG_TOOL_PARAMETER_SET("AREA"         , Parameters("SURFACE_AREA"))
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     ,  0, // Slope, Aspect, Curvatures
		    SG_TOOL_PARAMETER_SET("ELEVATION"    , Parameters("ELEVATION"      ))
		&&  SG_TOOL_PARAMETER_SET("SLOPE"        , Parameters("SLOPE"          ))
		&&  SG_TOOL_PARAMETER_SET("UNIT_SLOPE"   , 1) // degree
		&&  SG_TOOL_PARAMETER_SET("ASPECT"       , Parameters("ASPECT"         ))
		&&  SG_TOOL_PARAMETER_SET("UNIT_ASPECT"  , 1) // degree
		&&  SG_TOOL_PARAMETER_SET("NORTHNESS"    , Parameters("NORTHNESS"      ))
		&&  SG_TOOL_PARAMETER_SET("EASTNESS"     , Parameters("EASTNESS"       ))
		&&  SG_TOOL_PARAMETER_SET("C_PLAN"       , Parameters("CURVE_PLAN"     ))
		&&  SG_TOOL_PARAMETER_SET("C_CROS"       , Parameters("CURVE_CROSS"    ))
		&&  SG_TOOL_PARAMETER_SET("C_PROF"       , Parameters("CURVE_PROFILE"  ))
		&&  SG_TOOL_PARAMETER_SET("C_LONG"       , Parameters("CURVE_LONGITUDE"))
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     ,  2, // Convergence Index (Search Radius)
		    SG_TOOL_PARAMETER_SET("ELEVATION"    , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("CONVERGENCE"  , Parameters("CONVERGENCE"))
		&&  SG_TOOL_PARAMETER_SET("RADIUS"       , Parameters("SCALE")->asDouble())
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 26, // Upslope and Downslope Curvature
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"      ))
		&&  SG_TOOL_PARAMETER_SET("C_UP"         , Parameters("CURVE_UPSLOPE"  ))
		&&  SG_TOOL_PARAMETER_SET("C_DOWN"       , Parameters("CURVE_DOWNSLOPE"))
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 10, // Mass Balance Index
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"   ))
		&&  SG_TOOL_PARAMETER_SET("MBI"          , Parameters("MASS_BALANCE"))
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_hydrology"       , 10, // Cell Balance
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"   ))
		&&  SG_TOOL_PARAMETER_SET("BALANCE"      , Parameters("CELL_BALANCE"))
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 18, // Topographic Position Index (TPI)
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("TPI"          , Parameters("TPI"      ))
		&&  SG_TOOL_PARAMETER_SET("RADIUS.MIN"   , 0.)
		&&  SG_TOOL_PARAMETER_SET("RADIUS.MAX"   , Parameters("SCALE")->asDouble() * Get_Cellsize())
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 21, // Terrain Surface Convexity
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"        ))
		&&  SG_TOOL_PARAMETER_SET("CONVEXITY"    , Parameters("TERRAIN_CONVEXITY"))
		&&  SG_TOOL_PARAMETER_SET("SCALE"        , Parameters("SCALE")->asDouble())
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 20, // Terrain Surface Texture
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"        ))
		&&  SG_TOOL_PARAMETER_SET("TEXTURE"      , Parameters("TERRAIN_TEXTURE"  ))
		&&  SG_TOOL_PARAMETER_SET("SCALE"        , Parameters("SCALE")->asDouble())
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 16, // Terrain Ruggedness Index (TRI)
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("TRI"          , Parameters("TRI"      ))
		&&  SG_TOOL_PARAMETER_SET("RADIUS"       , Parameters("SCALE")->asDouble())
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 17, // Vector Ruggedness Measure (VRM)
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("VRM"          , Parameters("VRM"      ))
		&&  SG_TOOL_PARAMETER_SET("RADIUS"       , Parameters("SCALE")->asDouble())
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     ,  7, // Morphometric Protection Index
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION" ))
		&&  SG_TOOL_PARAMETER_SET("PROTECTION"   , Parameters("PROTECTION"))
		&&  SG_TOOL_PARAMETER_SET("RADIUS"       , Parameters("SCALE")->asDouble() * Get_Cellsize())
	);

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_lighting"        ,  5, // Topographic Openness
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"   ))
		&&  SG_TOOL_PARAMETER_SET("POS"          , Parameters("OPENNESS_POS"))
		&&  SG_TOOL_PARAMETER_SET("NEG"          , Parameters("OPENNESS_NEG"))
		&&  SG_TOOL_PARAMETER_SET("RADIUS"       , 25 * Get_Cellsize())
		&&  SG_TOOL_PARAMETER_SET("UNIT"         , 1) // degree
	);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
