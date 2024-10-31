
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
//                  classification.cpp                   //
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
#include "classification.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CClassification::CClassification(void)
{
	Set_Name		(_TL("Compound Terrain Classification"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"A one-step tool for the creation of selected terrain classifications."
		"For details look at the description of the tools used for processing:<ul>"
		"<li>[ta_morphometry] Surface Specific Points</li>"
		"<li>[ta_morphometry] Curvature Classification</li>"
		"<li>[ta_morphometry] Fuzzy Landform Element Classification</li>"
		"<li>[ta_morphometry] Morphometric Features</li>"
		"<li>[ta_morphometry] TPI Based Landform Classification</li>"
		"<li>[ta_morphometry] Terrain Surface Classification (Iwahashi and Pike)</li>"
		"<li>[ta_lighting] Geomorphons</li>"
		"</ul>"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid  ("", "ELEVATION"   , _TL("Elevation"                          ), _TL(""), PARAMETER_INPUT );

	Parameters.Add_Grid  ("", "SPECPOINTS"  , _TL("Surface Specific Points"            ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Char);
	Parameters.Add_Grid  ("", "CURVATURE"   , _TL("Curvature Classification"           ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Char);
	Parameters.Add_Grid  ("", "FUZZY"       , _TL("Fuzzy Landform Classification"      ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Char);
	Parameters.Add_Grid  ("", "MORPHOMETRIC", _TL("Morphometric Features"              ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Char);
	Parameters.Add_Grid  ("", "TPI"         , _TL("Topographic Position Classification"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Char);
	Parameters.Add_Grid  ("", "GEOMORPHONS" , _TL("Geomorphons"                        ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Char);
	Parameters.Add_Grid  ("", "TERRAINSURF" , _TL("Terrain Surface Classification"     ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Char);

	Parameters.Add_Int("", "SCALE", _TL("Scale"), _TL("Targeted scale (cells). Does not affect \"Surface Specific Points\" and  \"Fuzzy Landform Classification\"."), 10, 2, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CClassification::On_Execute(void)
{
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     ,  3, // Surface Specific Points
		    SG_TOOL_PARAMETER_SET("ELEVATION"    , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("RESULT"       , Parameters("SPECPOINTS"))
		&&  SG_TOOL_PARAMETER_SET("METHOD"       , 3) // Flow Direction (up and down)
	//	&&  SG_TOOL_PARAMETER_SET("METHOD"       , 4) // Peucker & Douglas
	)

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     ,  4, // Curvature Classification
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("CLASSES"      , Parameters("CURVATURE"))
		&&  SG_TOOL_PARAMETER_SET("STRAIGHT"     , Parameters("SCALE")->asInt() * Get_Cellsize())
		&&  SG_TOOL_PARAMETER_SET("SMOOTH"       , 2.)
	)

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 25, // Fuzzy Landform Element Classification
		    SG_TOOL_PARAMETER_SET("ELEVATION"    , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("FORM"         , Parameters("FUZZY"))
	)

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 23, // Morphometric Features
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("FEATURES"     , Parameters("MORPHOMETRIC"))
		&&  SG_TOOL_PARAMETER_SET("SIZE"         , Parameters("SCALE")->asInt())
	)

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 19, // TPI Based Landform Classification
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("LANDFORMS"    , Parameters("TPI"))
		&&  SG_TOOL_PARAMETER_SET("RADIUS_A.MIN" ,  0.)
	//	&&  SG_TOOL_PARAMETER_SET("RADIUS_A.MAX" ,  2 * Get_Cellsize())
		&&  SG_TOOL_PARAMETER_SET("RADIUS_A.MAX" , Parameters("SCALE")->asInt() * Get_Cellsize() * 0.2)
		&&  SG_TOOL_PARAMETER_SET("RADIUS_B.MIN" ,  0.)
		&&  SG_TOOL_PARAMETER_SET("RADIUS_B.MAX" , Parameters("SCALE")->asInt() * Get_Cellsize())
	)

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_lighting"        ,  8, // Geomorphons
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("GEOMORPHONS"  , Parameters("GEOMORPHONS"))
		&&  SG_TOOL_PARAMETER_SET("RADIUS"       , Parameters("SCALE")->asInt() * Get_Cellsize())
	)

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 22, // Terrain Surface Classification (Iwahashi and Pike)
		    SG_TOOL_PARAMETER_SET("DEM"          , Parameters("ELEVATION"))
		&&  SG_TOOL_PARAMETER_SET("LANDFORMS"    , Parameters("TERRAINSURF"))
		&&  SG_TOOL_PARAMETER_SET("CONV_SCALE"   , Parameters("SCALE")->asInt())
		&&  SG_TOOL_PARAMETER_SET("TEXT_SCALE"   , Parameters("SCALE")->asInt())
	)

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
