
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     sim_air_flow                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   cold_air_flow.cpp                   //
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
#include "cold_air_flow.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCold_Air_Flow::CCold_Air_Flow(void)
{
	Set_Name		(_TL("Cold Air Flow"));

	Set_Author		("O.Conrad, H.Dietrich (c) 2020");

	Set_Description	(_TW(
		"A simple cold air flow simulation."
	));

	Add_Reference("Schwab, A.", "2000",
		"Reliefanalytische Verfahren zur Abschaetzung naechtlicher Kaltluftbewegungen",
		"Freiburger Geographische Hefte, 61."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const("",
		"PRODUCTION", _TL("Production"),
		_TL("Rate of cold air production [m/h]."),
		1., 0., true
	);

	Parameters.Add_Grid_or_Const("",
		"FRICTION"	, _TL("Surface Friction Coefficient"),
		_TL("Surface friction coefficient."),
		1., 0., true
	);

	Parameters.Add_Grid("",
		"AIR"		, _TL("Cold Air Height"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"VELOCITY"	, _TL("Velocity"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"RESET"		, _TL("Reset"),
		_TL(""),
		true
	);

	Parameters.Add_Double("",
		"TIME_STOP"	, _TL("Simulation Time [h]"),
		_TL("Simulation time in hours."),
		6., 0., true
	);

	Parameters.Add_Double("",
		"TIME_UPDATE", _TL("Map Update Frequency [min]"),
		_TL(""),
		10., 0., true
	);

	Parameters.Add_Choice("",
		"EDGE"		, _TL("Edge"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("closed"),
			_TL("open")
		), 1
	);

	Parameters.Add_Double("",
		"DELAY"		, _TL("Time Step Adjustment"),
		_TL("Choosing a lower value will result in a better numerical precision but also in a longer calculation time."),
		0.5, 0.01, true, 1., true
	);

	Parameters.Add_Double("",
		"T_AIR"		, _TL("Surrounding Air Temperature"),
		_TL("Surrounding air temperature [degree Celsius]."),
		15., -273.15, true
	);

	Parameters.Add_Double("",
		"T_AIR_COLD", _TL("Cold Air Temperature"),
		_TL("Cold air temperature [degree Celsius]."),
		5., -273.15, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCold_Air_Flow::On_Execute(void)
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
		Get_Velocity();

		Set_Air();

		if( Time >= Update_Last )
		{
			if( Update > 0. )
			{
				Update_Last	= Update * (1. + floor(Time / Update));
			}

			DataObject_Update(m_pAir);
			DataObject_Update(m_pVelocity);
		}
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCold_Air_Flow::Initialize(void)
{
	m_pDEM			= Parameters("DEM"       )->asGrid  ();

	m_pProduction	= Parameters("PRODUCTION")->asGrid  ();
	m_Production	= Parameters("PRODUCTION")->asDouble();

	m_pFriction		= Parameters("FRICTION"  )->asGrid  ();
	m_Friction		= Parameters("FRICTION"  )->asDouble();

	m_pAir			= Parameters("AIR"       )->asGrid  ();
	m_pVelocity		= Parameters("VELOCITY"  )->asGrid  ();

	m_bEdge			= Parameters("EDGE"      )->asInt   () == 1;
	m_Delay			= Parameters("DELAY"     )->asDouble();

	double	T		= Parameters("T_AIR"     )->asDouble() + 273.15;
	double	Tcold	= Parameters("T_AIR_COLD")->asDouble() + 273.15;
	m_g_dT			= 9.80665 * (T - Tcold) / T;

	m_dTime			= 1. / 360.;	// initial time step := 1 sec.

	//-----------------------------------------------------
	if( Parameters("RESET")->asBool() )
	{
		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				m_pAir->Set_NoData(x, y);
			}
			else
			{
				m_pAir->Set_Value(x, y, Get_Production(x, y));
			}
		}
	}

	DataObject_Set_Colors(m_pAir, 11, SG_COLORS_WHITE_BLUE, false);
//	m_pAir->Set_NoData_Value_Range(0., 1.);
	DataObject_Update    (m_pAir, SG_UI_DATAOBJECT_SHOW);

	//-----------------------------------------------------
	if( !m_pVelocity )
	{
		if( !m_Velocity.Create(Get_System()) )
		{
			return( false );
		}

		m_pVelocity	= &m_Velocity;
	}

	m_pVelocity->Set_Unit("m/h");

	//-----------------------------------------------------
	m_dz .Create(Get_System(), 9, 0., SG_DATATYPE_Float);
	m_Air.Create(Get_System()       , SG_DATATYPE_Float);

	return( true );
}

//---------------------------------------------------------
bool CCold_Air_Flow::Finalize(void)
{
	m_dz      .Destroy();
	m_Air     .Destroy();
	m_Velocity.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCold_Air_Flow::Set_Time_Stamp(double Time)
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
inline double CCold_Air_Flow::Get_Production(int x, int y)
{
	double	Production	= m_pProduction ? m_pProduction->is_NoData(x, y) ? 0. : m_pProduction->asDouble(x, y) : m_Production;

	return( Production > 0. ? Production * m_dTime : 0. );
}

//---------------------------------------------------------
inline double CCold_Air_Flow::Get_Friction(int x, int y)
{
	double	Friction	= m_pFriction && !m_pFriction->is_NoData(x, y) ? m_pFriction->asDouble(x, y) : m_Friction;

	return( Friction > 0. ? Friction : 0. );
}

//---------------------------------------------------------
inline double CCold_Air_Flow::Get_Surface(int x, int y)
{
	return( m_pDEM->asDouble(x, y) + m_pAir->asDouble(x, y) );
}

//---------------------------------------------------------
inline double CCold_Air_Flow::Get_Gradient(int x, int y)
{
	double	dz[4], z = Get_Surface(x, y);

	for(int i=0, j=0, ix, iy; i<4; i++, j+=2)
	{
		if     ( m_pDEM->is_InGrid(ix = Get_xTo  (j, x), iy = Get_yTo  (j, y)) )
		{
			dz[i]	= Get_Surface(ix, iy) - z;
		}
		else if( m_pDEM->is_InGrid(ix = Get_xFrom(j, x), iy = Get_yFrom(j, y)) )
		{
			dz[i]	= z - Get_Surface(ix, iy);
		}
		else
		{
			dz[i]	= 0.;
		}
	}

	double G	= (dz[0] - dz[2]) / (2. * Get_Cellsize());
	double H	= (dz[1] - dz[3]) / (2. * Get_Cellsize());

	return( atan(sqrt(G*G + H*H)) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CCold_Air_Flow::Get_Neighbour(int x, int y, int i, int &ix, int &iy)
{
	ix	= Get_xTo(i, x);
	iy	= Get_yTo(i, y);

	return( m_pDEM->is_InGrid(ix, iy) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CCold_Air_Flow::Get_Velocity(int x, int y)
{
	double	dzSum = 0., v = 0., Air = m_pAir->asDouble(x, y);

	if( Air > 0. )
	{
		double	z = Get_Surface(x, y);

		for(int i=0, ix, iy; i<8; i++)
		{
			double	dz	= 0.;

			if( Get_Neighbour(x, y, i, ix, iy) )
			{
				if( (dz = z - Get_Surface(ix, iy)) > 0. )
				{
					dzSum	+= (dz = dz / Get_Length(i));
				}
			}
			else if( m_bEdge && Get_Neighbour(x, y, i + 4, ix, iy) )	// open edge
			{
				if( (dz = Get_Surface(ix, iy) - z) > 0. )
				{
					dzSum	+= (dz = dz / Get_Length(i));

					dz	= 0.;
				}
			}

			m_dz[i].Set_Value(x, y, dz);
		}

		//---------------------------------------------
		if( dzSum > 0. )
		{
			double	s = Get_Gradient(x, y);

			v	= 3600. * sqrt(m_g_dT * (Air / m_Friction) * sin(s));	// [m/h]
		}
	}

	m_Air       .Set_Value(x, y,   Air);
	m_dz[8]     .Set_Value(x, y, dzSum);
	m_pVelocity->Set_Value(x, y,     v);

	return( v );
}

//---------------------------------------------------------
bool CCold_Air_Flow::Get_Velocity(void)
{
	CSG_Vector	vMax(SG_OMP_Get_Max_Num_Threads());

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				double	v = Get_Velocity(x, y);

				if( vMax[SG_OMP_Get_Thread_Num()] < v )
				{
					vMax[SG_OMP_Get_Thread_Num()] = v;
				}
			}
		}
	}

	//-----------------------------------------------------
	for(int i=1; i<SG_OMP_Get_Max_Num_Threads(); i++)
	{
		if( vMax[0] < vMax[i] )
		{
			vMax[0] = vMax[i];
		}
	}

	if( vMax[0] > 0. )
	{
		m_dTime	= m_Delay * Get_Cellsize() / vMax[0];

		return( true );
	}

	m_dTime	*= 2.;

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCold_Air_Flow::Set_Air(void)
{
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				Set_Air(x, y);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CCold_Air_Flow::Set_Air(int x, int y)
{
	double	Air	= m_Air.asDouble(x, y);

	if( Air > 0. )
	{
		double	dzSum	= m_dz[8].asDouble(x, y);
		
		if( dzSum > 0. )
		{
			double	dAir	= Air * m_pVelocity->asDouble(x, y) * m_dTime / Get_Cellsize();

			if( dAir > Air )
			{
				dAir	= Air;
			}

			m_pAir->Add_Value(x, y, -dAir);

			dAir /= dzSum;

			for(int i=0; i<8; i++)
			{
				double	dz	= m_dz[i].asDouble(x, y);

				if( dz > 0. )
				{
					m_pAir->Add_Value(Get_xTo(i, x), Get_yTo(i, y), dz * dAir);
				}
			}
		}
	}

	m_pAir->Add_Value(x, y, Get_Production(x, y));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
