/**********************************************************
 * Version $Id$
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

	Set_Author		(SG_T("O.Conrad (c) 2005"));

	Set_Description	(_TW(
		"A selection of basic parameters and objects to be derived from a Digital Terrain Model using standard settings."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SHADE"		, _TL("Analytical Hillshading"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "SLOPE"		, _TL("Slope"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "ASPECT"		, _TL("Aspect"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "HCURV"		, _TL("Plan Curvature"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "VCURV"		, _TL("Profile Curvature"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONVERGENCE"	, _TL("Convergence Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CAREA"		, _TL("Catchment Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "WETNESS"		, _TL("Wetness Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "LSFACTOR"	, _TL("LS-Factor"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "CHANNELS"	, _TL("Channel Network"),
		_TL(""),
		PARAMETER_OUTPUT		, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "BASINS"		, _TL("Drainage Basins"),
		_TL(""),
		PARAMETER_OUTPUT		, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Grid(
		NULL	, "CHNL_ALTI"	, _TL("Altitude above Channel Network"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHNL_BASE"	, _TL("Channel Network Base Level"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RUN_MODULE(LIBRARY, MODULE, CONDITION)	{\
	bool	bResult;\
	SG_RUN_MODULE(bResult, LIBRARY, MODULE, CONDITION)\
	if( !bResult ) return( false );\
}

#define SET_PARAMETER(IDENTIFIER, VALUE)	pModule->Get_Parameters()->Set_Parameter(SG_T(IDENTIFIER), VALUE)

//---------------------------------------------------------
bool CTA_Standard::On_Execute(void)
{
	CSG_Grid	TMP1(*Get_System(), SG_DATATYPE_Float);
	CSG_Grid	TMP2(*Get_System(), SG_DATATYPE_Float);

	//-----------------------------------------------------
	RUN_MODULE("ta_preprocessor"		, 2,
			SET_PARAMETER("DEM"			, Parameters("ELEVATION"))
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_lighting"			, 0,
			SET_PARAMETER("ELEVATION"	, Parameters("ELEVATION"))
		&&	SET_PARAMETER("SHADE"		, Parameters("SHADE"))
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_morphometry"			, 0,
			SET_PARAMETER("ELEVATION"	, Parameters("ELEVATION"))
		&&	SET_PARAMETER("SLOPE"		, Parameters("SLOPE"))
		&&	SET_PARAMETER("ASPECT"		, Parameters("ASPECT"))
		&&	SET_PARAMETER("HCURV"		, (CSG_Grid *)NULL)
		&&	SET_PARAMETER("VCURV"		, (CSG_Grid *)NULL)
	)

	RUN_MODULE("grid_filter"			, 0,
			SET_PARAMETER("INPUT"		, Parameters("ELEVATION"))
		&&	SET_PARAMETER("RESULT"		, &TMP1)
		&&	SET_PARAMETER("RADIUS"		, 3)
	)

	RUN_MODULE("ta_morphometry"			, 0,
			SET_PARAMETER("ELEVATION"	, &TMP1)
		&&	SET_PARAMETER("SLOPE"		, &TMP2)
		&&	SET_PARAMETER("ASPECT"		, &TMP2)
		&&	SET_PARAMETER("HCURV"		, Parameters("HCURV"))
		&&	SET_PARAMETER("VCURV"		, Parameters("VCURV"))
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_morphometry"			, 1,
			SET_PARAMETER("ELEVATION"	, Parameters("ELEVATION"))
		&&	SET_PARAMETER("RESULT"		, Parameters("CONVERGENCE"))
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_hydrology"			, 0,
			SET_PARAMETER("ELEVATION"	, Parameters("ELEVATION"))
		&&	SET_PARAMETER("CAREA"		, Parameters("CAREA"))
		&&	SET_PARAMETER("Method"		, 4)
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_hydrology"			, 19,
			SET_PARAMETER("DEM"			, Parameters("ELEVATION"))
		&&	SET_PARAMETER("TCA"			, Parameters("CAREA"))
		&&	SET_PARAMETER("WIDTH"		, &TMP2)
		&&	SET_PARAMETER("SCA"			, &TMP1)
		&&	SET_PARAMETER("METHOD"		, 1)
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_hydrology"			, 20,
			SET_PARAMETER("SLOPE"		, Parameters("SLOPE"))
		&&	SET_PARAMETER("AREA"		, &TMP1)
		&&	SET_PARAMETER("TWI"			, Parameters("WETNESS"))
		&&	SET_PARAMETER("CONV"		, 0)
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_hydrology"			, 22,
			SET_PARAMETER("SLOPE"		, Parameters("SLOPE"))
		&&	SET_PARAMETER("AREA"		, &TMP1)
		&&	SET_PARAMETER("LS"			, Parameters("LSFACTOR"))
		&&	SET_PARAMETER("CONV"		, 0)
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_channels"			, 5,
			SET_PARAMETER("DEM"			, Parameters("ELEVATION"))
		&&	SET_PARAMETER("SEGMENTS"	, Parameters("CHANNELS"))
		&&	SET_PARAMETER("BASINS"		, Parameters("BASINS"))
		&&	SET_PARAMETER("ORDER"		, &TMP1)
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_channels"			, 3,
			SET_PARAMETER("ELEVATION"	, Parameters("ELEVATION"))
		&&	SET_PARAMETER("CHANNELS"	, &TMP1)
		&&	SET_PARAMETER("DISTANCE"	, Parameters("CHNL_ALTI"))
		&&	SET_PARAMETER("BASELEVEL"	, Parameters("CHNL_BASE"))
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
