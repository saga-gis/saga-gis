/**********************************************************
 * Version $Id: TA_Standard.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_compound                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    TA_Standard.cpp                    //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "TA_Standard.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTA_Standard::CTA_Standard(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Basic Terrain Analysis"));

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"A selection of basic parameters and objects to be derived from a Digital Terrain Model using standard settings."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid  (NULL, "ELEVATION"  , _TL("Elevation"                 ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid  (NULL, "SHADE"      , _TL("Analytical Hillshading"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "SLOPE"      , _TL("Slope"                     ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "ASPECT"     , _TL("Aspect"                    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "HCURV"      , _TL("Plan Curvature"            ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "VCURV"      , _TL("Profile Curvature"         ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "CONVERGENCE", _TL("Convergence Index"         ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "SINKS"      , _TL("Closed Depressions"        ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "FLOW"       , _TL("Total Catchment Area"      ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "WETNESS"    , _TL("Topographic Wetness Index" ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "LSFACTOR"   , _TL("LS-Factor"                 ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Shapes(NULL, "CHANNELS"   , _TL("Channel Network"           ), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Line);
	Parameters.Add_Shapes(NULL, "BASINS"     , _TL("Drainage Basins"           ), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Polygon);
	Parameters.Add_Grid  (NULL, "CHNL_BASE"  , _TL("Channel Network Base Level"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "CHNL_DIST"  , _TL("Channel Network Distance"  ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "VALL_DEPTH" , _TL("Valley Depth"              ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  (NULL, "RSP"        , _TL("Relative Slope Position"   ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Value(
		NULL	, "THRESHOLD"	, _TL("Channel Density"), 
		_TL("Strahler order to begin a channel."), 
		PARAMETER_TYPE_Int, 5, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTA_Standard::On_Execute(void)
{
	CSG_Grid	DEMP(*Get_System(), SG_DATATYPE_Float);
	CSG_Grid	TMP1(*Get_System(), SG_DATATYPE_Float);
	CSG_Grid	TMP2(*Get_System(), SG_DATATYPE_Float);

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("ta_preprocessor"    , 2,
			SG_MODULE_PARAMETER_SET("DEM"          , Parameters("ELEVATION"))
		&&	SG_MODULE_PARAMETER_SET("DEM_PREPROC"  , &DEMP)	// >> preprocessed DEM
	)

	Parameters("SINKS")->asGrid()->Set_NoData_Value(0.0);
//	Parameters("SINKS")->asGrid()->Assign(&(TMP2 = DEMP - *Parameters("ELEVATION")->asGrid()));

	SG_RUN_MODULE_ExitOnError("grid_calculus"      , 1,	// grid calculator
			SG_MODULE_PARAMETER_SET("RESULT"       , Parameters("SINKS"))
		&&	SG_MODULE_PARAMETER_SET("FORMULA"      , SG_T("g1 - g2"))
		&&	SG_MODULE_PARAMETER_SET("NAME"         , _TL("Closed Depressions"))
		&&	SG_MODULE_PARAMLIST_ADD("GRIDS"        , &DEMP)
		&&	SG_MODULE_PARAMLIST_ADD("GRIDS"        , Parameters("ELEVATION")->asGrid())
	)

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("ta_lighting"        , 0,
			SG_MODULE_PARAMETER_SET("ELEVATION"    , &DEMP)
		&&	SG_MODULE_PARAMETER_SET("SHADE"        , Parameters("SHADE"))
	)

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("ta_morphometry"     , 0,
			SG_MODULE_PARAMETER_SET("ELEVATION"    , &DEMP)
		&&	SG_MODULE_PARAMETER_SET("SLOPE"        , Parameters("SLOPE"))
		&&	SG_MODULE_PARAMETER_SET("ASPECT"       , Parameters("ASPECT"))
		&&	SG_MODULE_PARAMETER_SET("C_CROS"       , (CSG_Grid *)NULL)
		&&	SG_MODULE_PARAMETER_SET("C_LONG"       , (CSG_Grid *)NULL)
	)

	SG_RUN_MODULE_ExitOnError("grid_filter"        , 0,
			SG_MODULE_PARAMETER_SET("INPUT"        , &DEMP)
		&&	SG_MODULE_PARAMETER_SET("RESULT"       , &TMP1)
		&&	SG_MODULE_PARAMETER_SET("RADIUS"       , 3)
	)

	SG_RUN_MODULE_ExitOnError("ta_morphometry"     , 0,
			SG_MODULE_PARAMETER_SET("ELEVATION"    , &TMP1)
		&&	SG_MODULE_PARAMETER_SET("SLOPE"        , &TMP2)
		&&	SG_MODULE_PARAMETER_SET("ASPECT"       , &TMP2)
		&&	SG_MODULE_PARAMETER_SET("C_CROS"       , Parameters("HCURV"))
		&&	SG_MODULE_PARAMETER_SET("C_LONG"       , Parameters("VCURV"))
	)

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("ta_morphometry"     , 1,
			SG_MODULE_PARAMETER_SET("ELEVATION"    , &TMP1)
		&&	SG_MODULE_PARAMETER_SET("RESULT"       , Parameters("CONVERGENCE"))
	)

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("ta_hydrology"       , 0,
			SG_MODULE_PARAMETER_SET("ELEVATION"    , &DEMP)	// << preprocessed DEM
		&&	SG_MODULE_PARAMETER_SET("FLOW"         , Parameters("FLOW"))
		&&	SG_MODULE_PARAMETER_SET("METHOD"       , 4)		// MFD
	)

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("ta_hydrology"       , 19,
			SG_MODULE_PARAMETER_SET("DEM"          , &DEMP)
		&&	SG_MODULE_PARAMETER_SET("TCA"          , Parameters("FLOW"))
		&&	SG_MODULE_PARAMETER_SET("WIDTH"        , &TMP2)
		&&	SG_MODULE_PARAMETER_SET("SCA"          , &TMP1)	// >> specific catchment area
		&&	SG_MODULE_PARAMETER_SET("METHOD"       , 1)
	)

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("ta_hydrology"       , 20,
			SG_MODULE_PARAMETER_SET("SLOPE"        , Parameters("SLOPE"))
		&&	SG_MODULE_PARAMETER_SET("AREA"         , &TMP1)	// << specific catchment area
		&&	SG_MODULE_PARAMETER_SET("TWI"          , Parameters("WETNESS"))
		&&	SG_MODULE_PARAMETER_SET("CONV"         , 0)
	)

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("ta_hydrology"       , 22,
			SG_MODULE_PARAMETER_SET("SLOPE"        , Parameters("SLOPE"))
		&&	SG_MODULE_PARAMETER_SET("AREA"         , &TMP1)	// << specific catchment area
		&&	SG_MODULE_PARAMETER_SET("LS"           , Parameters("LSFACTOR"))
		&&	SG_MODULE_PARAMETER_SET("CONV"         , 0)
	)

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("ta_channels"        , 5,
			SG_MODULE_PARAMETER_SET("DEM"          , &DEMP)	// << preprocessed DEM
		&&	SG_MODULE_PARAMETER_SET("SEGMENTS"     , Parameters("CHANNELS"))
		&&	SG_MODULE_PARAMETER_SET("BASINS"       , Parameters("BASINS"))
		&&	SG_MODULE_PARAMETER_SET("ORDER"        , &TMP1)
		&&	SG_MODULE_PARAMETER_SET("THRESHOLD"    , Parameters("THRESHOLD"))
	)

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("ta_channels"        , 3,
			SG_MODULE_PARAMETER_SET("ELEVATION"    , &DEMP)
		&&	SG_MODULE_PARAMETER_SET("CHANNELS"     , &TMP1)
		&&	SG_MODULE_PARAMETER_SET("DISTANCE"     , Parameters("CHNL_DIST"))
		&&	SG_MODULE_PARAMETER_SET("BASELEVEL"    , Parameters("CHNL_BASE"))
	)

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("grid_tools"         , 34,	// invert grid
			SG_MODULE_PARAMETER_SET("GRID"         , &DEMP)
		&&	SG_MODULE_PARAMETER_SET("INVERSE"      , &TMP1)
	)

	SG_RUN_MODULE_ExitOnError("ta_channels"        ,  6,	// strahler order
			SG_MODULE_PARAMETER_SET("DEM"          , &TMP1)
		&&	SG_MODULE_PARAMETER_SET("STRAHLER"     , &TMP2)
	)

	TMP2.Set_NoData_Value_Range(0, 4);

	SG_RUN_MODULE_ExitOnError("ta_channels"        ,  3,	// vertical channel network distance
			SG_MODULE_PARAMETER_SET("ELEVATION"    , &TMP1)
		&&	SG_MODULE_PARAMETER_SET("CHANNELS"     , &TMP2)
		&&	SG_MODULE_PARAMETER_SET("DISTANCE"     , Parameters("VALL_DEPTH"))
	)

	Parameters("VALL_DEPTH")->asGrid()->Set_Name(_TL("Valley Depth"));

	SG_RUN_MODULE_ExitOnError("grid_calculus"      ,  1,	// grid calculator
			SG_MODULE_PARAMETER_SET("RESULT"       , Parameters("RSP"))
		&&	SG_MODULE_PARAMETER_SET("FORMULA"      , SG_T("g1 / (g1 + g2)"))
		&&	SG_MODULE_PARAMETER_SET("NAME"         , _TL("Relative Slope Position"))
		&&	SG_MODULE_PARAMLIST_ADD("GRIDS"        , Parameters("CHNL_DIST" )->asGrid())
		&&	SG_MODULE_PARAMLIST_ADD("GRIDS"        , Parameters("VALL_DEPTH")->asGrid())
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
