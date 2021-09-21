
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     sim_hydrology                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   overland_flow.cpp                   //
//                                                       //
//                 Copyright (C) 2020 by                 //
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
#include "overland_flow.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COverland_Flow::COverland_Flow(void)
{
	Set_Name		(_TL("Overland Flow"));

	Set_Author		("O.Conrad (c) 2020");

	Set_Description	(_TW(
		"A simple overland flow simulation."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const("",
		"ROUGHNESS"	, _TL("Roughness"),
		_TL("Ks Strickler = 1/n Manning"),
		20., 0., true
	);

	Parameters.Add_Choice("ROUGHNESS",
		"STRICKLER"	, _TL("Type"),
		_TL("Ks Strickler = 1/n Gauckler-Manning"),
		CSG_String::Format("%s|%s",
			SG_T("Strickler Ks, [m^1/3 / s]"),
			SG_T("Gauckler-Manning n, [s / m^1/3]")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("",
		"INTER_MAX"	, _TL("Interception Capacity [mm]"),
		_TL(""),
		0., 0., true
	);

	Parameters.Add_Grid_or_Const("",
		"POND_MAX"	, _TL("Ponding Capacity [mm]"),
		_TL(""),
		0., 0., true
	);

	Parameters.Add_Grid_or_Const("",
		"INFIL_MAX"	, _TL("Infiltration Capacity [mm/h]"),
		_TL(""),
		0., 0., true
	);

	Parameters.Add_Grid("",
		"INTERCEPT"	, _TL("Interception [mm]"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"PONDING"	, _TL("Ponding [mm]"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"INFILTRAT"	, _TL("Infiltration [mm]"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"FLOW"		, _TL("Flow [mm]"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"VELOCITY"	, _TL("Velocity [m/h]"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_System("",
		"WEATHER"	, _TL("Weather Grid System"),
		_TL("")
	);

	Parameters.Add_Grid_or_Const("WEATHER",
		"PRECIP"	, _TL("Precipitation [mm/h]"),
		_TL(""),
		0., 0., true
	);

	Parameters.Add_Grid_or_Const("WEATHER",
		"ET_POT"	, _TL("Potential Evapotranspiration [mm/h]"),
		_TL(""),
		0., 0., true
	);

	Parameters.Add_Bool("",
		"RESET"		, _TL("Reset"),
		_TL("If checked storages (flow, ponding, interception) and sinks (infiltration) will be set to zero."),
		true
	);

	Parameters.Add_Double("",
		"TIME_STOP"	, _TL("Simulation Time [h]"),
		_TL("Simulation time in hours."),
		6., 0., true
	);

	Parameters.Add_Double("",
		"TIME_STEP"	, _TL("Time Step Adjustment"),
		_TL("Choosing a lower value will result in a better numerical precision but also in a longer calculation time."),
		0.5, 0.01, true, 1., true
	);

//	Parameters.Add_Double("",
//		"V_MIN"		, _TL("Minimum Velocity [m/h]"),
//		_TL(""),
//		0., 0., true
//	);

	Parameters.Add_Bool("",
		"FLOW_OUT"	, _TL("Overland Flow Summary"),
		_TL("Report the amount of overland flow that left the covered area."),
		false
	);

	Parameters.Add_Double("",
		"TIME_UPDATE", _TL("Map Update Frequency [Minutes]"),
		_TL("Map update frequency in minutes. Set to zero to update each simulation time step."),
		1., 0., true
	);

	Parameters.Add_Bool("TIME_UPDATE", "UPDATE_FLOW_FIXED"                     , _TL("Fixed Color Stretch for Flow"    ), _TL(""), false);
	Parameters.Add_Range(              "UPDATE_FLOW_FIXED", "UPDATE_FLOW_RANGE", _TL("Fixed Color Stretch"             ), _TL(""),   0., 1., 0., true);

	Parameters.Add_Bool("TIME_UPDATE", "UPDATE_VELO_FIXED"                     , _TL("Fixed Color Stretch for Velocity"), _TL(""), true);
	Parameters.Add_Range(              "UPDATE_VELO_FIXED", "UPDATE_VELO_RANGE", _TL("Fixed Color Stretch"             ), _TL(""), 750., 1., 0., true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int COverland_Flow::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("STRICKLER") && (*pParameters)("ROUGHNESS")->asDouble() > 0. )
	{
		pParameters->Set_Parameter("ROUGHNESS_DEFAULT", 1. / (*pParameters)("ROUGHNESS")->asDouble());
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int COverland_Flow::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("INTER_MAX") )
	{
		pParameters->Set_Enabled("INTERCEPT", pParameter->asGrid() || pParameter->asDouble() > 0.);
	}

	if( pParameter->Cmp_Identifier("POND_MAX" ) )
	{
		pParameters->Set_Enabled("PONDING"  , pParameter->asGrid() || pParameter->asDouble() > 0.);
	}

	if( pParameter->Cmp_Identifier("INFIL_MAX") )
	{
		pParameters->Set_Enabled("INFILTRAT", pParameter->asGrid() || pParameter->asDouble() > 0.);
	}

	if( pParameter->Cmp_Identifier("UPDATE_FLOW_FIXED") )
	{
		pParameters->Set_Enabled("UPDATE_FLOW_RANGE", pParameter->asBool());
	}

	if( pParameter->Cmp_Identifier("VELOCITY") )
	{
		pParameters->Set_Enabled("UPDATE_VELO_FIXED", pParameter->asDataObject() != NULL);
	}

	if( pParameter->Cmp_Identifier("UPDATE_VELO_FIXED") )
	{
		pParameters->Set_Enabled("UPDATE_VELO_RANGE", pParameter->asBool());
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COverland_Flow::On_Execute(void)
{
	if( !Initialize() )
	{
		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	double	Update_Last	= 0., Update = Parameters("TIME_UPDATE")->asDouble() / 60.;	// from minutes to hours

	double	Time, Time_Stop	= Parameters("TIME_STOP")->asDouble();

	for(Time=0.; Time<=Time_Stop && Set_Time_Stamp(Time); Time+=m_dTime)
	{
		SG_UI_ProgressAndMsg_Lock(true);

		Do_Time_Step();

		if( Time >= Update_Last )
		{
			if( Update > 0. )
			{
				Update_Last	= Update * (1. + floor(Time / Update));
			}

			Do_Updates();
		}

		SG_UI_ProgressAndMsg_Lock(false);
	}

	//-----------------------------------------------------
	double	s	= Time;
	int		h	= (int)s; s = 60. * (s - h);
	int		m	= (int)s; s = 60. * (s - m);

	Message_Fmt("\n____\n%s: %02dh %02dm %02fs (= %g %s)\n", _TL("Simulation Time"), h, m, s, Time, _TL("hours"));

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COverland_Flow::Initialize(void)
{
	m_pDEM           = Parameters("DEM"      )->asGrid  ();

	m_pRoughness     = Parameters("ROUGHNESS")->asGrid  ();
	m_Roughness      = Parameters("ROUGHNESS")->asDouble();

	m_pPrecipitation = Parameters("PRECIP"   )->asGrid  ();
	m_Precipitation  = Parameters("PRECIP"   )->asDouble();

	m_pETpot         = Parameters("ET_POT"   )->asGrid  ();
	m_ETpot          = Parameters("ET_POT"   )->asDouble();

	m_pIntercept_max = Parameters("INTER_MAX")->asGrid  ();
	m_Intercept_max  = Parameters("INTER_MAX")->asDouble();
	m_pIntercept     = m_pIntercept_max || m_Intercept_max > 0.
	                 ? Parameters("INTERCEPT")->asGrid  () : NULL;

	m_pPonding_max   = Parameters("POND_MAX" )->asGrid  ();
	m_Ponding_max    = Parameters("POND_MAX" )->asDouble();
	m_pPonding       = m_pPonding_max   || m_Ponding_max   > 0.
	                 ? Parameters("PONDING"  )->asGrid  () : NULL;

	m_pInfiltrat_max = Parameters("INFIL_MAX")->asGrid  ();
	m_Infiltrat_max  = Parameters("INFIL_MAX")->asDouble();
	m_pInfiltrat     = m_pInfiltrat_max || m_Infiltrat_max > 0.
	                 ? Parameters("INFILTRAT")->asGrid  () : NULL;

	m_pFlow          = Parameters("FLOW"     )->asGrid  ();
	m_pVelocity      = Parameters("VELOCITY" )->asGrid  ();

	m_bStrickler     = Parameters("STRICKLER")->asInt() == 0;

	m_vMin  = 0.; // = Parameters("V_MIN"    )->asDouble();

	m_bFlow_Out      = Parameters("FLOW_OUT" )->asBool  ();
	m_Flow_Out		 = 0.;

	//-----------------------------------------------------
	if( Parameters("RESET")->asBool() )
	{
		#pragma omp parallel
		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				if( m_pIntercept ) m_pIntercept->Set_NoData(x, y);
				if( m_pPonding   ) m_pPonding  ->Set_NoData(x, y);
				if( m_pInfiltrat ) m_pInfiltrat->Set_NoData(x, y);
				if( m_pFlow      ) m_pFlow     ->Set_NoData(x, y);
			}
			else
			{
				if( m_pIntercept ) m_pIntercept->Set_Value(x, y, 0.);
				if( m_pPonding   ) m_pPonding  ->Set_Value(x, y, 0.);
				if( m_pInfiltrat ) m_pInfiltrat->Set_Value(x, y, 0.);
				if( m_pFlow      ) m_pFlow     ->Set_Value(x, y, 0.);
			}
		}

		CSG_Colors	Colors(5, SG_COLORS_WHITE_BLUE); Colors.Set_Color(0, 240, 240, 240);

		DataObject_Set_Colors(m_pIntercept, Colors);
		DataObject_Set_Colors(m_pPonding  , Colors);
		DataObject_Set_Colors(m_pInfiltrat, Colors);
		DataObject_Set_Colors(m_pFlow     , Colors);
	}

	if( m_pVelocity )
	{
		m_pVelocity->Set_NoData_Value(0.);

		CSG_Colors	Colors(11, SG_COLORS_RAINBOW); Colors.Set_Color(0, 255, 255, 255);

		DataObject_Set_Colors(m_pVelocity , Colors);
	}

	DataObject_Update(m_pFlow, SG_UI_DATAOBJECT_SHOW);	// show in new map

	//-----------------------------------------------------
	m_Flow.Create(Get_System()       , SG_DATATYPE_Float);
	m_v   .Create(Get_System(), 9, 0., SG_DATATYPE_Float);

	return( true );
}

//---------------------------------------------------------
bool COverland_Flow::Finalize(void)
{
	m_Flow.Destroy();
	m_v   .Destroy();

	if( !Process_Get_Okay() )
	{
		SG_UI_Process_Set_Okay();

		if( m_pIntercept ) m_pIntercept->Update(true);
		if( m_pPonding   ) m_pPonding  ->Update(true);
		if( m_pInfiltrat ) m_pInfiltrat->Update(true);
		if( m_pFlow      ) m_pFlow     ->Update(true);
	}

	if( m_bFlow_Out )
	{
		double	Flow_Sum = 0., Inf_Sum = 0.;

		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				Flow_Sum	+= m_pFlow->asDouble(x, y);

				if( m_pInfiltrat )
				{
					Inf_Sum	+= m_pInfiltrat->asDouble(x, y);
				}
			}
		}

		Message_Fmt("\n____\n%s\n%s:\t%g\n%s:\t%g\n%s:\t%g\n%s:\t%g\n", _TL("Overland Flow Summary"),
			_TL("flow in area"),   Flow_Sum,
			_TL("flow out"    ), m_Flow_Out,
			_TL("infiltration"),    Inf_Sum,
			_TL("total"       ), m_Flow_Out + Flow_Sum + Inf_Sum
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COverland_Flow::Do_Updates(void)
{
	DataObject_Update(m_pIntercept);
	DataObject_Update(m_pInfiltrat);

	if( Parameters("UPDATE_FLOW_FIXED")->asBool() == false )
	{
		DataObject_Update(m_pFlow);
	}
	else
	{
		DataObject_Update(m_pFlow,
			Parameters("UPDATE_FLOW_RANGE.MIN")->asDouble(),
			Parameters("UPDATE_FLOW_RANGE.MAX")->asDouble()
		);
	}

	if( Parameters("UPDATE_VELO_FIXED")->asBool() == false )
	{
		DataObject_Update(m_pVelocity);
	}
	else
	{
		DataObject_Update(m_pVelocity,
			Parameters("UPDATE_VELO_RANGE.MIN")->asDouble(),
			Parameters("UPDATE_VELO_RANGE.MAX")->asDouble()
		);
	}

	return( true );
}

//---------------------------------------------------------
bool COverland_Flow::Set_Time_Stamp(double Time)
{
	double	s	= Time;
	int		h	= (int)s; s = 60. * (s - h);
	int		m	= (int)s; s = 60. * (s - m);

	Process_Set_Text(CSG_String::Format("%s: %02d:%02d:%02d %s: %.2f [min]",
		_TL("Time"), h, m, (int)s,
		_TL("Step"), 60. * m_dTime
	));

	return( Process_Get_Okay() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COverland_Flow::Do_Time_Step(void)
{
	m_vMax	= 0.;

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		Get_Velocity(x, y);
	}

	//-----------------------------------------------------
	if( m_vMax > 0. )
	{
		m_dTime	= Parameters("TIME_STEP")->asDouble() * Get_Cellsize() / m_vMax;	// Courant–Friedrichs–Lewy (CFL) condition

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			Set_Flow_Lateral(x, y);
		}
	}
	else
	{
		m_dTime	= 1. / 60.;	// 1 min
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		Set_Flow_Vertical(x, y);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_GRID_OR_CONST(g, c)	{ double v; if( !g || !g->Get_Value(Get_System().Get_Grid_to_World(x, y), v) ) { v = c; } return( v > 0. ? m_dTime * v : 0. ); }

//---------------------------------------------------------
inline double COverland_Flow::Get_Precipitation(int x, int y)
{
	GET_GRID_OR_CONST(m_pPrecipitation, m_Precipitation);
}

//---------------------------------------------------------
inline double COverland_Flow::Get_ETpot(int x, int y)
{
	GET_GRID_OR_CONST(m_pETpot, m_ETpot);
}

//---------------------------------------------------------
#undef GET_GRID_OR_CONST


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_GRID_OR_CONST(g, c)	(g && !g->is_NoData(x, y) ? g->asDouble(x, y) : c)

//---------------------------------------------------------
inline double COverland_Flow::Get_Roughness(int x, int y)
{
	double	Value = GET_GRID_OR_CONST(m_pRoughness, m_Roughness);

	return( Value > 0. ? (m_bStrickler ? Value : 1. / Value) : 0. );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_Intercept_max(int x, int y)
{
	double	Value = GET_GRID_OR_CONST(m_pIntercept_max, m_Intercept_max);

	return( Value > 0. ? Value : 0. );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_Ponding(int x, int y)
{
	double	Value = GET_GRID_OR_CONST(m_pPonding_max, m_Ponding_max);

	return( Value > 0. ? Value : 0. );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_Infiltration(int x, int y)
{
	double	Value = GET_GRID_OR_CONST(m_pInfiltrat_max, m_Infiltrat_max);

	return( Value > 0. ? m_dTime * Value : 0. );
}

//---------------------------------------------------------
#undef GET_GRID_OR_CONST


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double COverland_Flow::Get_Surface(int x, int y)
{
	return( m_pDEM->asDouble(x, y) + m_pFlow->asDouble(x, y) / 1000. );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool COverland_Flow::Get_Neighbour(int x, int y, int i, int &ix, int &iy)
{
	return( m_pDEM->is_InGrid(ix = Get_xTo(i, x), iy = Get_yTo(i, y)) );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_Slope(int x, int y, int i)
{
	int ix, iy; double dz;

	if     ( Get_Neighbour(x, y, i    , ix, iy) )
	{
		dz	= Get_Surface( x,  y) - Get_Surface(ix, iy);
	}
	else if( Get_Neighbour(x, y, i + 4, ix, iy) )
	{
		dz	= Get_Surface(ix, iy) - Get_Surface( x,  y);
	}
	else
	{
		return( 0. );
	}

	return( dz > 0. ? dz / Get_Length(i) : 0. ); // the tangens of slope
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double COverland_Flow::Get_Velocity(double Flow, double Slope, double Roughness)
{
	// V = Ks * T^2/3 * S^1/2
	//   V : velocity [m/s]
	//   T : water depth [m]
	//   S : tangens of slope
	//   Ks: Strickler roughness factor (= 1/n, n: Manning roughness)

	return( 3600 * Roughness * pow(Flow / 1000., 2. / 3.) * sqrt(Slope) ); // 3600 [m/s] => 1 [m/h]
}

//---------------------------------------------------------
bool COverland_Flow::Get_Velocity(int x, int y)
{
	if( m_pDEM->is_NoData(x, y) )
	{
		return( false );
	}

	double	Flow = m_pFlow->asDouble(x, y);

	if( Flow > 0. )
	{
		double	vMax = 0., vSum = 0.;

		for(int i=0; i<8; i++)
		{
			double	Slope	= Get_Slope(x, y, i);

			if( Slope > 0. )
			{
				double v = Get_Velocity(Flow, Slope, Get_Roughness(x, y)); if( v < m_vMin ) { v = m_vMin; }

				if( vMax < v )
				{
					vMax = v;
				}

				vSum	+= v;

				m_v[i].Set_Value(x, y, v);
			}
			else
			{
				m_v[i].Set_Value(x, y, 0.);
			}
		}

		if( m_vMax < vMax )
		{
			#pragma omp critical
			{
				if( m_vMax < vMax )	// could have been changed by another thread after the comparison outside the critical section
				{
					m_vMax = vMax;
				}
			}
		}

		//-------------------------------------------------
		m_v[8].Set_Value(x, y, vSum);

		if( m_pVelocity )
		{
			m_pVelocity->Set_Value(x, y, vMax);	// 1/3600 * [m/h] => [m/s]
		}
	}
	else
	{
		if( m_pVelocity )
		{
			m_pVelocity->Set_Value(x, y, 0.);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double COverland_Flow::Get_Flow_Lateral(int x, int y, int i, bool bInverse)
{
	if( bInverse )
	{
		if( !m_pDEM->is_InGrid(x = Get_xTo(i, x), y = Get_yTo(i, y)) )
		{
			return( 0. );
		}

		i	= (i + 4) % 8;
	}

	double	Flow, v;

	if( (Flow = m_pFlow->asDouble(x, y)) > 0. && (v = m_v[i].asDouble(x, y)) > 0. )
	{
		Flow	= Flow * v / m_v[8].asDouble(x, y) * m_dTime * v / Get_Length(i);

		if( m_bFlow_Out && !bInverse && !is_InGrid(Get_xTo(i, x), Get_yTo(i, y)) )
		{
			#pragma omp atomic
				m_Flow_Out	+= Flow;
		}

		return( Flow );
	}

	return( 0. );
}

//---------------------------------------------------------
bool COverland_Flow::Set_Flow_Lateral(int x, int y)
{
	double	iFlow, Flow = m_pFlow->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		if     ( (iFlow = Get_Flow_Lateral(x, y, i, false)) > 0. )	// downslope flow leaving cell
		{
			Flow	-= iFlow;
		}
		else if( (iFlow = Get_Flow_Lateral(x, y, i,  true)) > 0. )	// upslope flow entering cell
		{
			Flow	+= iFlow;
		}
	}

	m_Flow.Set_Value(x, y, Flow > 0. ? Flow : 0.);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COverland_Flow::Set_Flow_Vertical(int x, int y)
{
	if( m_pDEM->is_NoData(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	P     = Get_Precipitation(x, y);

	double	I     = m_pIntercept ? m_pIntercept->asDouble(x, y) : 0.;
	double	Imax  = Get_Intercept_max(x, y);

	if( Imax > 0. )
	{
		double	Deficit	= Imax - I;

		if( Deficit > P )
		{
			I += P;
			P  = 0.;
		}
		else
		{
			I += Deficit;
			P -= Deficit;
		}
	}

	double	Q    = P + m_Flow.asDouble(x, y) + (m_pPonding ? m_pPonding->asDouble(x, y) : 0.);

	//-----------------------------------------------------
	if( Q > 0. )
	{
		double	ETpot = Get_ETpot(x, y);

		if( ETpot >= I + Q )
		{
			I  = 0.;
			Q  = 0.;
		}
		else
		{
			double	dIQ =  I / Q;
			double	sIQ = (I + Q) - ETpot;

			I    = sIQ * (     dIQ);
			Q    = sIQ * (1. - dIQ);
		}
	}
	else if( I > .0 )
	{
		double	ETpot = Get_ETpot(x, y);

		I    = I > ETpot ? I - ETpot : 0.;
	}

	if( m_pIntercept )
	{
		m_pIntercept->Set_Value(x, y, I);
	}

	//-----------------------------------------------------
	if( Q > 0. )
	{
		double	Infiltration	= Get_Infiltration(x, y);

		if( Infiltration >= Q )
		{
			m_pInfiltrat->Add_Value(x, y, Q);

			Q	= 0.;
		}
		else if( Infiltration > 0. )
		{
			m_pInfiltrat->Add_Value(x, y, Infiltration);

			Q	-= Infiltration;
		}
	}

	//-----------------------------------------------------
	if( Q > 0. )
	{
		double	Ponding_max	= Get_Ponding(x, y);

		if( Ponding_max >= Q )
		{
			m_pPonding->Set_Value(x, y, Q);

			Q	= 0.;
		}
		else if( Ponding_max > 0. )
		{
			m_pPonding->Set_Value(x, y, Ponding_max);

			Q	-= Ponding_max;
		}
	}

	//-----------------------------------------------------
	m_pFlow->Set_Value(x, y, Q);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
