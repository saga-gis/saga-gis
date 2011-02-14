
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

#include "./../ta_preprocessor/Pit_Eliminator.h"
#include "./../ta_lighting/HillShade.h"
#include "./../ta_morphometry/Morphometry.h"
#include "./../ta_morphometry/Convergence.h"
#include "./../ta_morphometry/Curvature_Classification.h"
#include "./../ta_hydrology/Flow_Parallel.h"
#include "./../ta_hydrology/TopographicIndices.h"
#include "./../ta_channels/ChannelNetwork.h"
#include "./../ta_channels/ChannelNetwork_Altitude.h"
#include "./../ta_channels/Watersheds.h"


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
		NULL	, "STREAMPOW"	, _TL("Stream Power"),
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

//---------------------------------------------------------
CTA_Standard::~CTA_Standard(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTA_Standard::On_Execute(void)
{
	//-----------------------------------------------------
	Process_Set_Text(_TL("Pre-Processing"));

	CPit_Eliminator	Pit_Eliminator;

	if(	!Pit_Eliminator.Get_Parameters()->Set_Parameter(SG_T("DEM")		, Parameters("ELEVATION"))

	||	!Pit_Eliminator.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Analytical Hillshading"));

	CHillShade	HillShade;

	if(	!HillShade.Get_Parameters()->Set_Parameter(SG_T("ELEVATION")	, Parameters("ELEVATION"))
	||	!HillShade.Get_Parameters()->Set_Parameter(SG_T("SHADE")		, Parameters("SHADE"))
	||	!HillShade.Get_Parameters()->Set_Parameter(SG_T("AZIMUTH")		, Parameters("SHADE_AZIM"))
	||	!HillShade.Get_Parameters()->Set_Parameter(SG_T("DECLINATION")	, Parameters("SHADE_DECL"))
	||	!HillShade.Get_Parameters()->Set_Parameter(SG_T("EXAGGERATION")	, Parameters("SHADE_EXAG"))

	||	!HillShade.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Basic Morphometry"));

	CMorphometry	Morphometry;

	if(	!Morphometry.Get_Parameters()->Set_Parameter(SG_T("ELEVATION")	, Parameters("ELEVATION"))
	||	!Morphometry.Get_Parameters()->Set_Parameter(SG_T("SLOPE")		, Parameters("SLOPE"))
	||	!Morphometry.Get_Parameters()->Set_Parameter(SG_T("ASPECT")		, Parameters("ASPECT"))
	||	!Morphometry.Get_Parameters()->Set_Parameter(SG_T("CURV")		, Parameters("CURV"))
	||	!Morphometry.Get_Parameters()->Set_Parameter(SG_T("HCURV")		, Parameters("HCURV"))
	||	!Morphometry.Get_Parameters()->Set_Parameter(SG_T("VCURV")		, Parameters("VCURV"))

	||	!Morphometry.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Convergence Index"));

	CConvergence	Convergence;

	if(	!Convergence.Get_Parameters()->Set_Parameter(SG_T("ELEVATION")	, Parameters("ELEVATION"))
	||	!Convergence.Get_Parameters()->Set_Parameter(SG_T("RESULT")		, Parameters("CONVERGENCE"))

	||	!Convergence.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Curvature Classification"));

	CCurvature_Classification	Curvature_Classification;

	if(	!Curvature_Classification.Get_Parameters()->Set_Parameter(SG_T("CPLAN")	, Parameters("HCURV"))
	||	!Curvature_Classification.Get_Parameters()->Set_Parameter(SG_T("CPROF")	, Parameters("VCURV"))
	||	!Curvature_Classification.Get_Parameters()->Set_Parameter(SG_T("CLASS")	, Parameters("CURVCLASS"))

	||	!Curvature_Classification.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Flow Accumulation"));

	CFlow_Parallel	Flow_Parallel;

	if(	!Flow_Parallel.Get_Parameters()->Set_Parameter(SG_T("ELEVATION")	, Parameters("ELEVATION"))
	||	!Flow_Parallel.Get_Parameters()->Set_Parameter(SG_T("CAREA")		, Parameters("CAREA"))

	||	!Flow_Parallel.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Topographic Indices"));

	CTopographicIndices	TopographicIndices;

	if(	!TopographicIndices.Get_Parameters()->Set_Parameter(SG_T("SLOPE")		, Parameters("SLOPE"))
	||	!TopographicIndices.Get_Parameters()->Set_Parameter(SG_T("AREA")		, Parameters("CAREA"))
	||	!TopographicIndices.Get_Parameters()->Set_Parameter(SG_T("WETNESS")		, Parameters("WETNESS"))
	||	!TopographicIndices.Get_Parameters()->Set_Parameter(SG_T("STREAMPOW")	, Parameters("STREAMPOW"))
	||	!TopographicIndices.Get_Parameters()->Set_Parameter(SG_T("LSFACTOR")	, Parameters("LSFACTOR"))

	||	!TopographicIndices.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Channel Network"));

	double	d	= Parameters("CAREA")->asGrid()->Get_Percentile(Parameters("CHNL_INIT")->asDouble());
	CSG_Grid	Grid(Parameters("ELEVATION")->asGrid(), SG_DATATYPE_Byte);

	CChannelNetwork	ChannelNetwork;

	if(	!ChannelNetwork.Get_Parameters()->Set_Parameter(SG_T("ELEVATION")	, Parameters("ELEVATION"))
	||	!ChannelNetwork.Get_Parameters()->Set_Parameter(SG_T("CHNLNTWRK")	, Parameters("CHNL_GRID"))
	||	!ChannelNetwork.Get_Parameters()->Set_Parameter(SG_T("CHNLROUTE")	, PARAMETER_TYPE_Grid, &Grid)
	||	!ChannelNetwork.Get_Parameters()->Set_Parameter(SG_T("SHAPES")		, Parameters("CHNL_SHAPES"))
	||	!ChannelNetwork.Get_Parameters()->Set_Parameter(SG_T("INIT_GRID")	, Parameters("CAREA"))
	||	!ChannelNetwork.Get_Parameters()->Set_Parameter(SG_T("INIT_VALUE")	, PARAMETER_TYPE_Double, d)

	||	!ChannelNetwork.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Watershed Basins"));

	CWatersheds	Watersheds;

	if(	!Watersheds.Get_Parameters()->Set_Parameter(SG_T("ELEVATION")	, Parameters("ELEVATION"))
	||	!Watersheds.Get_Parameters()->Set_Parameter(SG_T("CHANNELS")	, Parameters("CHNL_GRID"))
	||	!Watersheds.Get_Parameters()->Set_Parameter(SG_T("BASINS")		, Parameters("BASINS"))

	||	!Watersheds.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Vertical Distance to Channel Network"));

	CChannelNetwork_Altitude	ChannelNetwork_Altitude;

	if(	!ChannelNetwork_Altitude.Get_Parameters()->Set_Parameter(SG_T("ELEVATION")	, Parameters("ELEVATION"))
	||	!ChannelNetwork_Altitude.Get_Parameters()->Set_Parameter(SG_T("CHANNELS")	, Parameters("CHNL_GRID"))
	||	!ChannelNetwork_Altitude.Get_Parameters()->Set_Parameter(SG_T("ALTITUDE")	, Parameters("CHNL_ALTI"))
	||	!ChannelNetwork_Altitude.Get_Parameters()->Set_Parameter(SG_T("BASELEVEL")	, Parameters("CHNL_BASE"))

	||	!ChannelNetwork_Altitude.Execute() )
	{
		return( false );
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
