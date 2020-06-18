
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
		"ROUGHNESS"	, _TL("Manning Roughness"),
		_TL(""),
		0.03, 0., true
	);

	Parameters.Add_Grid_or_Const("",
		"PRECIP"	, _TL("Precipitation [mm/h]"),
		_TL(""),
		0., 0., true
	);

	Parameters.Add_Grid_or_Const("",
		"ET_POT"	, _TL("Potential Evapotranspiration [mm/h]"),
		_TL(""),
		0., 0., true
	);

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
		"VELOCITY"	, _TL("Velocity [m/s]"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Bool("",
		"RESET"	, _TL("Reset"),
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

	Parameters.Add_Double("",
		"TIME_UPDATE", _TL("Map Update Frequency [min]"),
		_TL(""),
		1., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

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

	double	Time_Stop	= Parameters("TIME_STOP")->asDouble();

	for(double Time=0.; Time<=Time_Stop && Set_Time_Stamp(Time); Time+=m_dTime)
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
	DataObject_Update(m_pFlow     );
	DataObject_Update(m_pVelocity, 0., 10.);

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
	m_vMax.Create(SG_OMP_Get_Max_Num_Threads());
	m_vMax.Assign(0.);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		Get_Velocity(x, y);
	}

	for(int i=1; i<SG_OMP_Get_Max_Num_Threads(); i++)
	{
		if( m_vMax[0] < m_vMax[i] )
		{
			m_vMax[0] = m_vMax[i];
		}
	}

	//-----------------------------------------------------
	if( m_vMax[0] > 0. )
	{
		m_dTime	= Parameters("TIME_STEP")->asDouble() / m_vMax[0];	// Courant–Friedrichs–Lewy (CFL) condition

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			Set_Flow_Lateral(x, y);
		}
	}
	else
	{
		m_dTime	= 1. / 60.;
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
#define GET_GRID_OR_CONST(g, c)	(g && !g->is_NoData(x, y) ? g->asDouble(x, y) : c)

//---------------------------------------------------------
inline double COverland_Flow::Get_Roughness(int x, int y)
{
	double	Roughness	  = GET_GRID_OR_CONST(m_pRoughness, m_Roughness);

	return( Roughness > 0. ? Roughness : 0. );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_Precipitation(int x, int y)
{
	double	Precipitation = GET_GRID_OR_CONST(m_pPrecipitation, m_Precipitation);

	return( Precipitation > 0. ? m_dTime * Precipitation : 0. );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_ETpot(int x, int y)
{
	double	ETpot	      = GET_GRID_OR_CONST(m_pETpot, m_ETpot);

	return( ETpot > 0. ? m_dTime * ETpot : 0. );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_Intercept_max(int x, int y)
{
	double	Intercept_max = GET_GRID_OR_CONST(m_pIntercept_max, m_Intercept_max);

	return( Intercept_max > 0. ? Intercept_max : 0. );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_Ponding(int x, int y)
{
	double	Ponding_max = GET_GRID_OR_CONST(m_pPonding_max, m_Ponding_max);

	return( Ponding_max > 0. ? Ponding_max : 0. );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_Infiltration(int x, int y)
{
	double	Infiltrat_max = GET_GRID_OR_CONST(m_pInfiltrat_max, m_Infiltrat_max);

	return( Infiltrat_max > 0. ? m_dTime * Infiltrat_max : 0. );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_Surface(int x, int y)
{
	return( m_pDEM->asDouble(x, y) + m_pFlow->asDouble(x, y) / 1000. );
}

//---------------------------------------------------------
inline double COverland_Flow::Get_Velocity(double Flow, double Slope, double Roughness)
{
	return( 3600. * pow(Flow / 1000., 2. / 3.) * sqrt(Slope) / Roughness );
}

//---------------------------------------------------------
inline bool COverland_Flow::Get_Neighbour(int x, int y, int i, int &ix, int &iy)
{
	ix	= Get_xTo(i, x);
	iy	= Get_yTo(i, y);

	return( m_pDEM->is_InGrid(ix, iy) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double COverland_Flow::Get_Gradient(int x, int y, int i)
{
	int	ix, iy;	double	dz;

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

	return( dz > 0. ? dz / Get_Length(i) : 0. );
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
		double	dzSum = 0., vMax = 0.;

		for(int i=0; i<8; i++)
		{
			double	dz	= Get_Gradient(x, y, i);

			if( dz > 0. )
			{
				dzSum	+= dz;

				double	v	= Get_Velocity(Flow, dz, Get_Roughness(x, y)) / Get_Length(i);

				if( vMax < v )
				{
					vMax = v;
				}

				m_v[i].Set_Value(x, y, v);
			}
			else
			{
				m_v[i].Set_Value(x, y, 0.);
			}
		}

		if( m_vMax[SG_OMP_Get_Thread_Num()] < vMax )
		{
			m_vMax[SG_OMP_Get_Thread_Num()] = vMax;
		}

		m_v[8].Set_Value(x, y, dzSum);

		if( m_pVelocity )
		{
			m_pVelocity->Set_Value(x, y, vMax);
		}

		return( true );
	}

	//-----------------------------------------------------
	m_v[8].Set_Value(x, y, 0.);

	if( m_pVelocity )
	{
		m_pVelocity->Set_Value(x, y, 0.);
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
		x	= Get_xTo(i, x);
		y	= Get_yTo(i, y);
		i	= (i + 4) % 8;
	}

	if( m_pDEM->is_InGrid(x, y) )
	{
		double	v	= m_v[i].asDouble(x, y);

		if( v > 0. )
		{
			return( v * m_pFlow->asDouble(x, y) * Get_Gradient(x, y, i) / m_v[8].asDouble(x, y) );
		}
	}

	return( 0. );
}

//---------------------------------------------------------
bool COverland_Flow::Set_Flow_Lateral(int x, int y)
{
	double	Flow = m_pFlow->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		double	dFlow	= Get_Flow_Lateral(x, y, i, false);

		if( dFlow > 0. )
		{
			Flow	-= m_dTime * dFlow;
		}
		else if( (dFlow = Get_Flow_Lateral(x, y, i, true)) > 0. )
		{
			Flow	+= m_dTime * dFlow;
		}
	}

	m_Flow.Set_Value(x, y, Flow);

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

		if( ETpot > I + Q )
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

		if( Infiltration > Q )
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

		if( Ponding_max > Q )
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
