
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
//                     hydrology.cpp                     //
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
#include "hydrology.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHydrology::CHydrology(void)
{
	Set_Name		(_TL("Compound Hydrologic Terrain Analysis"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"A one-step tool for the creation of selected terrain parameters related to hydrology."
		"For details look at the description of the tools used for processing:<ul>"
		"<li>[ta_preproc] Sink Removal</li>"
		"<li>[ta_preproc] Fill Sinks (Wang & Liu)</li>"
		"<li>[ta_preproc] Breach Depressions</li>"
		"<li>[ta_hydrology] Flow Accumulation (Top-Down)</li>"
		"<li>[ta_hydrology] Flow Accumulation (Flow Tracing)</li>"
		"<li>[ta_hydrology] Topographic Wetness Index</li>"
		"<li>[ta_hydrology] SAGA Wetness Index</li>"
		"<li>[ta_hydrology] LS Factor</li>"
		"<li>[ta_hydrology] CIT</li>"
		"</ul>"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid  ("", "ELEVATION"  , _TL("Elevation"                     ), _TL(""), PARAMETER_INPUT );

	Parameters.Add_Grid  ("", "TCA"        , _TL("Total Catchment Area"          ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "SCA"        , _TL("Specific Catchment Area"       ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "TWI"        , _TL("Topographic Wetness Index"     ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "TWI_SAGA"   , _TL("SAGA Topographic Wetness Index"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "LS"         , _TL("LS Factor"                     ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "SPI"        , _TL("Stream Power Index"            ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "CIT"        , _TL("Channel Initiation Threshold"  ), _TL(""), PARAMETER_OUTPUT);

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

	Parameters.Add_Choice("",
		"METHOD_FLOWACC" , _TL("Flow Accumulation"), 
		_TL("Algorithm used to calculate total catchment area."),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("Deterministic 8"),
			_TL("Rho 8"),
			_TL("Deterministic Infinity"),
			_TL("Multiple Flow Direction"),
			_TL("Multiple Triangular Flow Direction"),
			_TL("Multiple Maximum Downslope Gradient Based Flow Direction"),
			_TL("Kinematic Routing Algorithm"),
			_TL("DEMON")
		), 3
	);

	Parameters.Add_Choice("",
		"METHOD_LS"      , _TL("LS Factor"), 
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			SG_T("Moore et al. 1991"),
			SG_T("Desmet & Govers 1996"),
			SG_T("Boehner & Selige 2006")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHydrology::On_Execute(void)
{
	CSG_Grid DEM(Get_System());

	switch( Parameters["METHOD_PREPROC"].asInt() )
	{
	default: // Sink Removal
		SG_RUN_TOOL_ExitOnError("ta_preprocessor", 2,
		        SG_TOOL_PARAMETER_SET("DEM"        , Parameters("ELEVATION"))
		    &&  SG_TOOL_PARAMETER_SET("DEM_PREPROC", &DEM)
		);
		break;

	case  1: // Fill Sinks (Wang & Liu)
		SG_RUN_TOOL_ExitOnError("ta_preprocessor", 4,
		        SG_TOOL_PARAMETER_SET("ELEV"       , Parameters("ELEVATION"))
		    &&  SG_TOOL_PARAMETER_SET("FILLED"     , &DEM)
		);
		break;

	case  2: // Breach Depressions
		SG_RUN_TOOL_ExitOnError("ta_preprocessor", 7,
		        SG_TOOL_PARAMETER_SET("DEM"        , Parameters("ELEVATION"))
		    &&  SG_TOOL_PARAMETER_SET("NOSINKS"    , &DEM)
		);
		break;

	case  3: // none
		break;
	}

	//-----------------------------------------------------
	switch( Parameters["METHOD_FLOWACC"].asInt() )
	{
	case  0: // Deterministic 8
		SG_RUN_TOOL_ExitOnError("ta_hydrology"   , 0,
		        SG_TOOL_PARAMETER_SET("ELEVATION", &DEM)
			&&  SG_TOOL_PARAMETER_SET("FLOW"     , Parameters("TCA"))
		    &&  SG_TOOL_PARAMETER_SET("METHOD"   , 0)
		);
		break;

	case  1: // Rho 8
		SG_RUN_TOOL_ExitOnError("ta_hydrology"   , 0,
		        SG_TOOL_PARAMETER_SET("ELEVATION", &DEM)
			&&  SG_TOOL_PARAMETER_SET("FLOW"     , Parameters("TCA"))
		    &&  SG_TOOL_PARAMETER_SET("METHOD"   , 1)
		);
		break;

	case  2: // Deterministic Infinity
		SG_RUN_TOOL_ExitOnError("ta_hydrology"   , 0,
		        SG_TOOL_PARAMETER_SET("ELEVATION", &DEM)
			&&  SG_TOOL_PARAMETER_SET("FLOW"     , Parameters("TCA"))
		    &&  SG_TOOL_PARAMETER_SET("METHOD"   , 3)
		);
		break;

	default: // Multiple Flow Direction"
		SG_RUN_TOOL_ExitOnError("ta_hydrology"   , 0,
		        SG_TOOL_PARAMETER_SET("ELEVATION", &DEM)
			&&  SG_TOOL_PARAMETER_SET("FLOW"     , Parameters("TCA"))
		    &&  SG_TOOL_PARAMETER_SET("METHOD"   , 4)
		);
		break;

	case  4: // Multiple Triangular Flow Direction
		SG_RUN_TOOL_ExitOnError("ta_hydrology"   , 0,
		        SG_TOOL_PARAMETER_SET("ELEVATION", &DEM)
			&&  SG_TOOL_PARAMETER_SET("FLOW"     , Parameters("TCA"))
		    &&  SG_TOOL_PARAMETER_SET("METHOD"   , 5)
		);
		break;

	case  5: // Multiple Maximum Downslope Gradient Based Flow Direction
		SG_RUN_TOOL_ExitOnError("ta_hydrology"   , 0,
		        SG_TOOL_PARAMETER_SET("ELEVATION", &DEM)
			&&  SG_TOOL_PARAMETER_SET("FLOW"     , Parameters("TCA"))
		    &&  SG_TOOL_PARAMETER_SET("METHOD"   , 6)
		);
		break;

	case  6: // Kinematic Routing Algorithm
		SG_RUN_TOOL_ExitOnError("ta_hydrology"   , 2,
		        SG_TOOL_PARAMETER_SET("ELEVATION", &DEM)
			&&  SG_TOOL_PARAMETER_SET("FLOW"     , Parameters("TCA"))
		    &&  SG_TOOL_PARAMETER_SET("METHOD"   , 1)
		);
		break;

	case  7: // DEMON
		SG_RUN_TOOL_ExitOnError("ta_hydrology"   , 2,
		        SG_TOOL_PARAMETER_SET("ELEVATION", &DEM)
			&&  SG_TOOL_PARAMETER_SET("FLOW"     , Parameters("TCA"))
		    &&  SG_TOOL_PARAMETER_SET("METHOD"   , 2)
		);
		break;
	}

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_hydrology"       , 19, // Flow Width and Specific Catchment Area
		    SG_TOOL_PARAMETER_SET("DEM"          , &DEM)
		&&  SG_TOOL_PARAMETER_SET("TCA"          , Parameters("TCA"))
		&&  SG_TOOL_PARAMETER_SET("SCA"          , Parameters("SCA"))
		&&  SG_TOOL_PARAMETER_SET("METHOD"       , 2) // Aspect
	)

	//-----------------------------------------------------
	CSG_Grid Slope; // , Aspect(Get_System());

	SG_RUN_TOOL_ExitOnError("ta_morphometry"     , 0, // Slope, Aspect, Curvatures
		    SG_TOOL_PARAMETER_SET("ELEVATION"    , &DEM)
		&&  SG_TOOL_PARAMETER_SET("SLOPE"        , &Slope)
	)

	SG_RUN_TOOL_ExitOnError("ta_hydrology"       , 20, // Topographic Wetness Index
		    SG_TOOL_PARAMETER_SET("SLOPE"        , &Slope)
		&&  SG_TOOL_PARAMETER_SET("AREA"         , Parameters("SCA"))
		&&  SG_TOOL_PARAMETER_SET("TWI"          , Parameters("TWI"))
	)

	SG_RUN_TOOL_ExitOnError("ta_hydrology"       , 15, // SAGA Wetness Index
		    SG_TOOL_PARAMETER_SET("DEM"          , &DEM)
		&&  SG_TOOL_PARAMETER_SET("TWI"          , Parameters("TWI_SAGA"))
	)

	SG_RUN_TOOL_ExitOnError("ta_hydrology"       , 22, // LS Factor
		    SG_TOOL_PARAMETER_SET("SLOPE"        , &Slope)
		&&  SG_TOOL_PARAMETER_SET("AREA"         , Parameters("SCA"))
		&&  SG_TOOL_PARAMETER_SET("LS"           , Parameters("LS"))
		&&  SG_TOOL_PARAMETER_SET("METHOD"       , Parameters("METHOD_LS")) // Aspect
	)

	SG_RUN_TOOL_ExitOnError("ta_hydrology"       , 21, // Stream Power Index
		    SG_TOOL_PARAMETER_SET("SLOPE"        , &Slope)
		&&  SG_TOOL_PARAMETER_SET("AREA"         , Parameters("SCA"))
		&&  SG_TOOL_PARAMETER_SET("SPI"          , Parameters("SPI"))
	)

	SG_RUN_TOOL_ExitOnError("ta_hydrology"       , 31, // CIT Index
		    SG_TOOL_PARAMETER_SET("SLOPE"        , &Slope)
		&&  SG_TOOL_PARAMETER_SET("AREA"         , Parameters("SCA"))
		&&  SG_TOOL_PARAMETER_SET("CIT"          , Parameters("CIT"))
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
