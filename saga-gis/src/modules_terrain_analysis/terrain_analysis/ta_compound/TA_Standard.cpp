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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name(_TL("Standard Terrain Analysis"));

	Set_Author		(SG_T("(c) 2005 by O.Conrad"));

	Set_Description(
		_TL("Standard Terrain Analysis")
	);

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

	pNode	= Parameters.Add_Node(
		NULL	, "SHADE_NODE"	, _TL("Analytical Hillshading"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "SHADE_AZIM"	, _TL("Azimuth []"),
		_TL(""),
		PARAMETER_TYPE_Double	, 315
	);

	Parameters.Add_Value(
		pNode	, "SHADE_DECL"	, _TL("Declination []"),
		_TL(""),
		PARAMETER_TYPE_Double	, 45
	);

	Parameters.Add_Value(
		pNode	, "SHADE_EXAG"	, _TL("Exaggeration"),
		_TL(""),
		PARAMETER_TYPE_Double	, 4
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
		NULL	, "CURV"		, _TL("Curvature"),
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
		NULL	, "CURVCLASS"	, _TL("Curvature Classification"),
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

	Parameters.Add_Grid(
		NULL	, "CHNL_GRID"	, _TL("Channel Network"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "CHNL_SHAPES"	, _TL("Channel Network"),
		_TL(""),
		PARAMETER_OUTPUT		, SHAPE_TYPE_Line
	);

	pNode	= Parameters.Add_Node(
		NULL	, "CHNL_NODE"	, _TL("Channel Network"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "CHNL_INIT"	, _TL("Network Density [%]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 5.0, 0.0, true, 100.0, true
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

	Parameters.Add_Grid(
		NULL	, "BASINS"		, _TL("Watershed Subbasins"),
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
#define RUN_MODULE(Library, Module, Condition)	{\
	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T(Library), Module);\
	\
	if(	pModule == NULL )\
	{\
		return( false );\
	}\
	\
	Process_Set_Text(pModule->Get_Name());\
	\
	pModule->Set_Managed(false);\
	\
	if( !(Condition) || !pModule->Execute() )\
	{\
		pModule->Set_Managed(true);\
	\
		return( false );\
	}\
	\
	pModule->Set_Managed(true);\
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTA_Standard::On_Execute(void)
{
	//-----------------------------------------------------
	RUN_MODULE("ta_preprocessor"	, 2,
			pModule->Get_Parameters()->Set_Parameter(SG_T("DEM")			, Parameters("ELEVATION"))
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_lighting"		, 0,
			pModule->Get_Parameters()->Set_Parameter(SG_T("ELEVATION")		, Parameters("ELEVATION"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("SHADE")			, Parameters("SHADE"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("AZIMUTH")		, Parameters("SHADE_AZIM"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("DECLINATION")	, Parameters("SHADE_DECL"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("EXAGGERATION")	, Parameters("SHADE_EXAG"))
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_morphometry"		, 0,
			pModule->Get_Parameters()->Set_Parameter(SG_T("ELEVATION")		, Parameters("ELEVATION"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("SLOPE")			, Parameters("SLOPE"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("ASPECT")			, Parameters("ASPECT"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CURV")			, Parameters("CURV"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("HCURV")			, Parameters("HCURV"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("VCURV")			, Parameters("VCURV"))
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_morphometry"		, 1,
			pModule->Get_Parameters()->Set_Parameter(SG_T("ELEVATION")		, Parameters("ELEVATION"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("RESULT")			, Parameters("CONVERGENCE"))
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_morphometry"		, 4,
			pModule->Get_Parameters()->Set_Parameter(SG_T("CPLAN")			, Parameters("HCURV"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CPROF")			, Parameters("VCURV"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CLASS")			, Parameters("CURVCLASS"))
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_hydrology"		, 0,
			pModule->Get_Parameters()->Set_Parameter(SG_T("ELEVATION")		, Parameters("ELEVATION"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CAREA")			, Parameters("CAREA"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("Method")			, 4)
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_hydrology"		, 20,
			pModule->Get_Parameters()->Set_Parameter(SG_T("SLOPE")			, Parameters("SLOPE"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("AREA")			, Parameters("CAREA"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("TWI")			, Parameters("WETNESS"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CONV")			, 1)
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_hydrology"		, 22,
			pModule->Get_Parameters()->Set_Parameter(SG_T("SLOPE")			, Parameters("SLOPE"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("AREA")			, Parameters("CAREA"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("LS")				, Parameters("LSFACTOR"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CONV")			, 1)
	)

	//-----------------------------------------------------
	double	d	= Parameters("CAREA")->asGrid()->Get_Percentile(Parameters("CHNL_INIT")->asDouble());
	CSG_Grid	Grid(Parameters("ELEVATION")->asGrid(), SG_DATATYPE_Byte);

	RUN_MODULE("ta_channels"		, 0,
			pModule->Get_Parameters()->Set_Parameter(SG_T("ELEVATION")		, Parameters("ELEVATION"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CHNLNTWRK")		, Parameters("CHNL_GRID"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CHNLROUTE")		, &Grid)
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("SHAPES")			, Parameters("CHNL_SHAPES"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("INIT_GRID")		, Parameters("CAREA"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("INIT_VALUE")		, d)
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_channels"		, 1,
			pModule->Get_Parameters()->Set_Parameter(SG_T("ELEVATION")		, Parameters("ELEVATION"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CHANNELS")		, Parameters("CHNL_GRID"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("BASINS")			, Parameters("BASINS"))
	)

	//-----------------------------------------------------
	RUN_MODULE("ta_channels"		, 3,
			pModule->Get_Parameters()->Set_Parameter(SG_T("ELEVATION")		, Parameters("ELEVATION"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CHANNELS")		, Parameters("CHNL_GRID"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("ALTITUDE")		, Parameters("CHNL_ALTI"))
		&&	pModule->Get_Parameters()->Set_Parameter(SG_T("BASELEVEL")		, Parameters("CHNL_BASE"))
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
