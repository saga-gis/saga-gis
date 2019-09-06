
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                    sim_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               timed_flow_accumulation.cpp             //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "timed_flow_accumulation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTimed_Flow_Accumulation::CTimed_Flow_Accumulation(void)
{
	Set_Name		(_TL("Quasi-Dynamic Flow Accumulation"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"This tool estimates the flow distribution at a specified time "
		"after a rainfall event. It is based on a standard flow accumulation "
		"calculation (e.g. O'Callaghan & Mark 1984, Freeman 1991) that "
		"has an additional function to accumulate the flow travel time. "
		"Final flow accumulation becomes that part of the total flow through "
		"that corresponds to the portion of flow travel time that exceeds "
		"the targeted time. "
		"The travel time is estimated with the empirical Manning equation "
		"(e.g. Dingman 1994, Freeman et al. 1998, Manning 1891). "
	));

	Add_Reference("Dingman, S.L.", "1994",
		"Physical Hydrology",
		"Macmillan Publishing, 575p."
	);

	Add_Reference("Freeman, G. E., Copeland, R. R., Rahmeyer, W., & Derrick, D. L.", "1998",
		"Field Determination of Manning’s n Value for Shrubs and Woody Vegetation",
		"Engineering Approaches to Ecosystem Restoration.",
		SG_T("https://doi.org/10.1061%2F40382%281998%297"), SG_T("doi:10.1061/40382(1998)7.")
	);

	Add_Reference("Freeman, G.T.", "1991",
		"Calculating catchment area with divergent flow based on a regular grid",
		"Computers and Geosciences, 17:413-22.",
		SG_T("https://www.sciencedirect.com/science/article/pii/009830049190048I"), SG_T("ScienceDirect")
	);

	Add_Reference("Manning, R.", "1891",
		"On the flow of water in open channels and pipes",
		"Transactions of the Institution of Civil Engineers of Ireland. 20: 161–207."
	);

	Add_Reference("O'Callaghan, J.F. & Mark, D.M.", "1984",
		"The extraction of drainage networks from digital elevation data",
		"Computer Vision, Graphics and Image Processing, 28:323-344.",
		SG_T("https://www.sciencedirect.com/science/article/pii/S0734189X84800110"), SG_T("ScienceDirect")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL("[m]"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const("",
		"FLOW_K"	, _TL("Manning-Strickler Coefficient"),
		_TL("Manning-Strickler coefficient for flow travel time estimation (reciprocal of Manning's Roughness Coefficient)"),
		20., 0., true
	);

	Parameters.Add_Grid("",
		"FLOW_ACC"	, _TL("Flow Accumulation"),
		_TL("[mm]"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"FLOW"		, _TL("Flow Through"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"TIME_MEAN"	, _TL("Flow Travel Time"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"TIME_CONC"	, _TL("Flow Concentration"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"VELOCITY"	, _TL("Flow Velocity"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"TIME"		, _TL("Time"),
		_TL("minutes"),
		5., 0.001, true
	);

	Parameters.Add_Choice("",
		"ROUTING"	, _TL("Flow Routing"),
		_TL(""),
		CSG_String::Format("%s|%s",
			SG_T("D8"),
			SG_T("MFD")
		), 1
	);

	Parameters.Add_Choice("",
		"FLOW_DEPTH", _TL("Flow Depth"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("constant"),
			_TL("dynamic")
		), 1
	);

	Parameters.Add_Double("FLOW_DEPTH",
		"FLOW_CONST", _TL("Constant Flow Depth"),
		_TL("[mm]"),
		10., 0., true
	);

	Parameters.Add_Bool("",
		"RESET"		, _TL("Reset"),
		_TL("Resets flow accumulation raster."),
		true
	);

	Parameters.Add_Double("",
		"RAIN"		, _TL("Rain"),
		_TL("The flow portion [mm] added to each raster cell before simulation starts."),
		10., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTimed_Flow_Accumulation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FLOW_DEPTH") )
	{
		pParameters->Set_Enabled("FLOW_CONST", pParameter->asInt() == 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTimed_Flow_Accumulation::On_Execute(void)
{
	CSG_Grid	Temp[3];

	#define	CHECK_TEMP(iTemp, pGrid) if( !pGrid ) { pGrid = &Temp[iTemp]; pGrid->Create(Get_System()); }

	m_pDEM	= Parameters("DEM"      )->asGrid  ();
	m_pK	= Parameters("FLOW_K"   )->asGrid  ();
	m_K		= Parameters("FLOW_K"   )->asDouble();
	m_pAccu	= Parameters("FLOW_ACC" )->asGrid  ();
	m_pFlow	= Parameters("FLOW"     )->asGrid  (); CHECK_TEMP(0, m_pFlow);
	m_pTime	= Parameters("TIME_MEAN")->asGrid  (); CHECK_TEMP(1, m_pTime);
	m_pConc	= Parameters("TIME_CONC")->asGrid  (); CHECK_TEMP(2, m_pConc);

	CSG_Grid	*pVelocity	= Parameters("VELOCITY")->asGrid();

	if( pVelocity )
	{
		pVelocity->Set_Unit("m/min");
	}

	//-----------------------------------------------------
	double	Rain	= Parameters("RAIN")->asDouble();

	if( Parameters("RESET")->asBool() )
	{
		m_pFlow->Assign(Rain);

		DataObject_Set_Colors(m_pAccu, 11, SG_COLORS_YELLOW_BLUE);
		DataObject_Set_Colors(m_pFlow, 11, SG_COLORS_YELLOW_BLUE);
	}
	else
	{
		m_pFlow->Assign(m_pAccu);

		if( Rain > 0. )
		{
			m_pFlow->Add(Rain);
		}
	}

	m_pAccu->Set_Unit("mm" );
	m_pFlow->Set_Unit("mm" );
	m_pTime->Set_Unit("min");
	m_pConc->Set_Unit("min");

	//-----------------------------------------------------
	m_Time	= Parameters("TIME")->asDouble();	// minutes

	int	Routing	= Parameters("ROUTING")->asInt();

	m_R	= Parameters("FLOW_DEPTH")->asInt() != 0 ? 0.	// dynamic
		: Parameters("FLOW_CONST")->asDouble() * 0.001;	// [mm] to [m]

	m_pTime->Assign(0.);
	m_pConc->Assign(0.);

	//-----------------------------------------------------
	for(sLong i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
	{
		int	x, y;	m_pDEM->Get_Sorted(i, x, y, true, false);

		if( m_pDEM->is_NoData(x, y) )
		{
			m_pAccu->Set_NoData(x, y);
			m_pFlow->Set_NoData(x, y);
			m_pTime->Set_NoData(x, y);
			m_pConc->Set_NoData(x, y);

			if( pVelocity )
			{
				pVelocity->Set_NoData(x, y);
			}
		}
		else
		{
			double	Flow	= m_pFlow->asDouble(x, y);
			double	Time	= m_pTime->asDouble(x, y);

			//---------------------------------------------
			if( Time > m_Time )	// drop flow portion that reached the cell after specified time
			{
				double	dFlow	= m_Time / Time;

				m_pFlow->Set_Value(x, y, Flow *       dFlow );
				m_pAccu->Set_Value(x, y, Flow * (1. - dFlow));
				m_pTime->Set_Value(x, y, m_Time);
			}
			else // if( Time <= maxTime )
			{
				m_pAccu->Set_Value(x, y, 0.);
				m_pTime->Set_Value(x, y, Time);
			}

			//---------------------------------------------
			if( pVelocity )
			{
				pVelocity->Set_Value(x, y, Get_Velocity(x, y));
			}

			//---------------------------------------------
			switch( Routing )
			{
			default: Set_D8 (x, y); break;
			case  1: Set_MFD(x, y); break;
			}

			//---------------------------------------------
			if( Flow <= 0. )	// masking
			{
				m_pTime->Set_NoData(x, y);
				m_pConc->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CTimed_Flow_Accumulation::Get_K(int x, int y)
{
	return( m_pK && !m_pK->is_InGrid(x, y) ? m_pK->asDouble(x, y) : m_K );
}

//---------------------------------------------------------
inline double CTimed_Flow_Accumulation::Get_R(int x, int y)
{
	if( m_R > 0. )
	{
		return( m_R );	// constant flow depth
	}

	if( m_pFlow->is_InGrid(x, y) && m_pConc->is_InGrid(x, y) )
	{
		const double	D	= 0.0;

		double	d	= m_pConc->asDouble(x, y) / m_Time;	// flow concentration

		d	= m_pFlow->asDouble(x, y) * (d <= 0. ? 1. : d > 1. ? D : 1. - (1 - D) * d);	// reduce the flow depth for less concentrated flow through

		return( (d < 5. ? 5. : d) * 0.001 );	// [mm] to [m]
	//	return( d * m_pFlow->asDouble(x, y) * 0.001 );	// [mm] to [m]
	}

	return( 0. );
}

//---------------------------------------------------------
/**
* k: the inverse Gauckler–Manning coefficient (k = 1 / n) [m^1/3 / sec]
* R: hydraulic radius (here: flow depth) [m]
* I: tangens of slope [m / m]
*/
//---------------------------------------------------------
inline double CTimed_Flow_Accumulation::Get_Velocity(double k, double R, double I)
{
	if( R > 0. && I > 0. )
	{
		double	v	= k * pow(R, 2. / 3.) * sqrt(I);	// simplified Manning equation, [m/sec]

		return( v *   60. );	// [m / min]
	//	return( v * 3600. );	// [m / h]
	}

	return( 0. );
}

//---------------------------------------------------------
inline double CTimed_Flow_Accumulation::Get_Velocity(int x, int y)
{
	double	Slope, Aspect;

	if( !m_pDEM->Get_Gradient(x, y, Slope, Aspect) || Slope <= 0. )
	{
		return( 0. );
	}

	return( Get_Velocity(Get_K(x, y), Get_R(x, y), atan(Slope)) );
}

//---------------------------------------------------------
inline double CTimed_Flow_Accumulation::Get_Travel_Time(int x, int y)
{
	double	v	= Get_Velocity(Get_K(x, y), Get_R(x, y), Get_Cellsize());

	return( v > 0. ? Get_Cellsize() / v : 0. );
}

//---------------------------------------------------------
inline double CTimed_Flow_Accumulation::Get_Travel_Time(int x, int y, int Direction)
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		int	ix, iy;	double	dz	= m_pDEM->asDouble(x, y);

		if     ( m_pDEM->is_InGrid(ix = Get_xTo  (Direction, x), iy = Get_yTo  (Direction, y)) )
		{
			dz	= dz - m_pDEM->asDouble(ix, iy);
		}
		else if( m_pDEM->is_InGrid(ix = Get_xFrom(Direction, x), iy = Get_yFrom(Direction, y)) )
		{
			dz	= m_pDEM->asDouble(ix, iy) - dz;
		}

		if( dz > 0. )
		{
			double	dl	= Get_Length(Direction);

			double	v	= Get_Velocity(Get_K(x, y), Get_R(x, y), dz / dl);

			return( v > 0. ? dl / v : 0. );
		}
	}

	return( 0. );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CTimed_Flow_Accumulation::Add_Flow(int x, int y, int Direction, double Proportion)
{
	if( Proportion > 0. )
	{
		double	Flow	= m_pFlow->asDouble(x, y);

		if( Flow > 0. )
		{
			int	ix	= Get_xTo(Direction, x);
			int	iy	= Get_yTo(Direction, y);

			m_pFlow->Add_Value(ix, iy, Proportion * Flow);

			double	Time	= m_pTime->asDouble(x, y);

			if( m_pConc->asDouble(ix, iy) < Time )
			{
				m_pConc->Set_Value(ix, iy, Time);
			}

			Time	+= Get_Travel_Time(x, y, Direction);

			if( Time > 0. )
			{
				m_pTime->Add_Value(ix, iy, Proportion * Time);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTimed_Flow_Accumulation::Get_D8(int x, int y, int &Direction)
{
	double	dz, dzMax = 0., z = m_pDEM->asDouble(x, y);

	Direction	= -1;

	for(int i=0; i<8; i++)
	{
		int	ix = Get_xTo(i, x), iy = Get_yTo(i, y);

		if     ( m_pDEM->is_InGrid(ix = Get_xTo  (i, x), iy = Get_yTo  (i, y)) )
		{
			if( (dz = (z - m_pDEM->asDouble(ix, iy) / Get_Length(i))) > dzMax )
			{
				dzMax = dz; Direction =  i;
			}
		}
		else if( m_pDEM->is_InGrid(ix = Get_xFrom(i, x), iy = Get_yFrom(i, y)) )
		{
			if( (dz = (m_pDEM->asDouble(ix, iy) - z) / Get_Length(i)) > dzMax )
			{
				dzMax = dz; Direction = -1;	// flow is routed outside
			}
		}
	}

	return( Direction >= 0 );
}

//---------------------------------------------------------
bool CTimed_Flow_Accumulation::Set_D8(int x, int y)
{
	int	Direction;

	if( Get_D8(x, y, Direction) )
	{
		Add_Flow(x, y, Direction);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTimed_Flow_Accumulation::Get_MFD(int x, int y, double dz[8])
{
	const double	Convergence	= 1.1;

	double	d, dzSum = 0., z = m_pDEM->asDouble(x, y);

	for(int i=0, ix, iy; i<8; i++)
	{
		dz[i]	= 0.;

		if     ( m_pDEM->is_InGrid(ix = Get_xTo  (i, x), iy = Get_yTo  (i, y)) )
		{
			if( (d = z - m_pDEM->asDouble(ix, iy)) > 0. )
			{
				dzSum	+= (dz[i] = pow(d / Get_Length(i), Convergence));
			}
		}
		else if( m_pDEM->is_InGrid(ix = Get_xFrom(i, x), iy = Get_yFrom(i, y)) )
		{
			if( (d = z - m_pDEM->asDouble(ix, iy)) < 0. )
			{
				dzSum	-= (dz[i] = pow(d / Get_Length(i), Convergence));	// flow is routed outside
			}
		}
	}

	for(int i=0; i<8; i++)
	{
		dz[i]	= dzSum > 0. ? dz[i] / dzSum : 0.;
	}

	return( dzSum > 0. );
}

//---------------------------------------------------------
bool CTimed_Flow_Accumulation::Set_MFD(int x, int y)
{
	double	dz[8];

	if( Get_MFD(x, y, dz) )
	{
		for(int i=0; i<8; i++)
		{
			Add_Flow(x, y, i, dz[i]);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
