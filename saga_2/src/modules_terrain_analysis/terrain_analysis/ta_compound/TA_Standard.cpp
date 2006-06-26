
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
	CParameter	*pNode;

	//-----------------------------------------------------
	Set_Name(_TL("Standard Terrain Analysis"));

	Set_Author(_TL("Copyrights (c) 2005 by Olaf Conrad"));

	Set_Description(
		_TL("Standard Terrain Analysis")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SHADE"		, _TL("Analytical Hillshading"),
		"",
		PARAMETER_OUTPUT
	);

	pNode	= Parameters.Add_Node(
		NULL	, "SHADE_NODE"	, _TL("Analytical Hillshading"),
		""
	);

	Parameters.Add_Value(
		pNode	, "SHADE_AZIM"	, _TL("Azimuth []"),
		"",
		PARAMETER_TYPE_Double	, 315
	);

	Parameters.Add_Value(
		pNode	, "SHADE_DECL"	, _TL("Declination []"),
		"",
		PARAMETER_TYPE_Double	, 45
	);

	Parameters.Add_Value(
		pNode	, "SHADE_EXAG"	, _TL("Exaggeration"),
		"",
		PARAMETER_TYPE_Double	, 4
	);

	Parameters.Add_Grid(
		NULL	, "SLOPE"		, _TL("Slope"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "ASPECT"		, _TL("Aspect"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CURV"		, _TL("Curvature"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "HCURV"		, _TL("Plan Curvature"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "VCURV"		, _TL("Profile Curvature"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONVERGENCE"	, _TL("Convergence Index"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CURVCLASS"	, _TL("Curvature Classification"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CAREA"		, _TL("Catchment Area"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "WETNESS"		, _TL("Wetness Index"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "STREAMPOW"	, _TL("Stream Power"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "LSFACTOR"	, _TL("LS-Factor"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHNL_GRID"	, _TL("Channel Network"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "CHNL_SHAPES"	, _TL("Channel Network"),
		"",
		PARAMETER_OUTPUT		, SHAPE_TYPE_Line
	);

	pNode	= Parameters.Add_Node(
		NULL	, "CHNL_NODE"	, _TL("Channel Network"),
		""
	);

	Parameters.Add_Value(
		pNode	, "CHNL_INIT"	, _TL("Network Density [%]"),
		"",
		PARAMETER_TYPE_Double	, 5.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Grid(
		NULL	, "CHNL_ALTI"	, _TL("Altitude above Channel Network"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHNL_BASE"	, _TL("Channel Network Base Level"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "BASINS"		, _TL("Watershed Subbasins"),
		"",
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

	if(	!Pit_Eliminator.Get_Parameters()->Set_Parameter("DEM"		, Parameters("ELEVATION"))

	||	!Pit_Eliminator.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Analytical Hillshading"));

	CHillShade	HillShade;

	if(	!HillShade.Get_Parameters()->Set_Parameter("ELEVATION"		, Parameters("ELEVATION"))
	||	!HillShade.Get_Parameters()->Set_Parameter("SHADE"			, Parameters("SHADE"))
	||	!HillShade.Get_Parameters()->Set_Parameter("AZIMUTH"		, Parameters("SHADE_AZIM"))
	||	!HillShade.Get_Parameters()->Set_Parameter("DECLINATION"	, Parameters("SHADE_DECL"))
	||	!HillShade.Get_Parameters()->Set_Parameter("EXAGGERATION"	, Parameters("SHADE_EXAG"))

	||	!HillShade.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Basic Morphometry"));

	CMorphometry	Morphometry;

	if(	!Morphometry.Get_Parameters()->Set_Parameter("ELEVATION"	, Parameters("ELEVATION"))
	||	!Morphometry.Get_Parameters()->Set_Parameter("SLOPE"		, Parameters("SLOPE"))
	||	!Morphometry.Get_Parameters()->Set_Parameter("ASPECT"		, Parameters("ASPECT"))
	||	!Morphometry.Get_Parameters()->Set_Parameter("CURV"			, Parameters("CURV"))
	||	!Morphometry.Get_Parameters()->Set_Parameter("HCURV"		, Parameters("HCURV"))
	||	!Morphometry.Get_Parameters()->Set_Parameter("VCURV"		, Parameters("VCURV"))

	||	!Morphometry.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Convergence Index"));

	CConvergence	Convergence;

	if(	!Convergence.Get_Parameters()->Set_Parameter("ELEVATION"	, Parameters("ELEVATION"))
	||	!Convergence.Get_Parameters()->Set_Parameter("RESULT"		, Parameters("CONVERGENCE"))

	||	!Convergence.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Curvature Classification"));

	CCurvature_Classification	Curvature_Classification;

	if(	!Curvature_Classification.Get_Parameters()->Set_Parameter("CPLAN"	, Parameters("HCURV"))
	||	!Curvature_Classification.Get_Parameters()->Set_Parameter("CPROF"	, Parameters("VCURV"))
	||	!Curvature_Classification.Get_Parameters()->Set_Parameter("CLASS"	, Parameters("CURVCLASS"))

	||	!Curvature_Classification.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Flow Accumulation"));

	CFlow_Parallel	Flow_Parallel;

	if(	!Flow_Parallel.Get_Parameters()->Set_Parameter("ELEVATION"	, Parameters("ELEVATION"))
	||	!Flow_Parallel.Get_Parameters()->Set_Parameter("CAREA"		, Parameters("CAREA"))

	||	!Flow_Parallel.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Topographic Indices"));

	CTopographicIndices	TopographicIndices;

	if(	!TopographicIndices.Get_Parameters()->Set_Parameter("SLOPE"		, Parameters("SLOPE"))
	||	!TopographicIndices.Get_Parameters()->Set_Parameter("AREA"		, Parameters("CAREA"))
	||	!TopographicIndices.Get_Parameters()->Set_Parameter("WETNESS"	, Parameters("WETNESS"))
	||	!TopographicIndices.Get_Parameters()->Set_Parameter("STREAMPOW"	, Parameters("STREAMPOW"))
	||	!TopographicIndices.Get_Parameters()->Set_Parameter("LSFACTOR"	, Parameters("LSFACTOR"))

	||	!TopographicIndices.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Channel Network"));

	double	d	= Parameters("CAREA")->asGrid()->Get_Percentile(Parameters("CHNL_INIT")->asDouble());
	CGrid	Grid(Parameters("ELEVATION")->asGrid(), GRID_TYPE_Byte);

	CChannelNetwork	ChannelNetwork;

	if(	!ChannelNetwork.Get_Parameters()->Set_Parameter("ELEVATION"	, Parameters("ELEVATION"))
	||	!ChannelNetwork.Get_Parameters()->Set_Parameter("CHNLNTWRK"	, Parameters("CHNL_GRID"))
	||	!ChannelNetwork.Get_Parameters()->Set_Parameter("CHNLROUTE"	, PARAMETER_TYPE_Grid, &Grid)
	||	!ChannelNetwork.Get_Parameters()->Set_Parameter("SHAPES"	, Parameters("CHNL_SHAPES"))
	||	!ChannelNetwork.Get_Parameters()->Set_Parameter("INIT_GRID"	, Parameters("CAREA"))
	||	!ChannelNetwork.Get_Parameters()->Set_Parameter("INIT_VALUE", PARAMETER_TYPE_Double, d)

	||	!ChannelNetwork.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Watershed Basins"));

	CWatersheds	Watersheds;

	if(	!Watersheds.Get_Parameters()->Set_Parameter("ELEVATION"	, Parameters("ELEVATION"))
	||	!Watersheds.Get_Parameters()->Set_Parameter("CHANNELS"	, Parameters("CHNL_GRID"))
	||	!Watersheds.Get_Parameters()->Set_Parameter("BASINS"	, Parameters("BASINS"))

	||	!Watersheds.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Vertical Distance to Channel Network"));

	CChannelNetwork_Altitude	ChannelNetwork_Altitude;

	if(	!ChannelNetwork_Altitude.Get_Parameters()->Set_Parameter("ELEVATION"	, Parameters("ELEVATION"))
	||	!ChannelNetwork_Altitude.Get_Parameters()->Set_Parameter("CHANNELS"		, Parameters("CHNL_GRID"))
	||	!ChannelNetwork_Altitude.Get_Parameters()->Set_Parameter("ALTITUDE"		, Parameters("CHNL_ALTI"))
	||	!ChannelNetwork_Altitude.Get_Parameters()->Set_Parameter("BASELEVEL"	, Parameters("CHNL_BASE"))

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
