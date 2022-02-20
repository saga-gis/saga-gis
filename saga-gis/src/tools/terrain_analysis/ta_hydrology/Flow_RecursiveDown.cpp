
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Flow_RecursiveDown.cpp                 //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Flow_RecursiveDown.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_OUTLET_DIAG__1(in, angle)		(1. - (1. - in) * tan(M_PI_090 - angle))
#define GET_OUTLET_CROSS_1(in, angle)		(in + tan(angle))

#define GET_OUTLET_DIAG__2(in, angle)		(in * tan(angle))
#define GET_OUTLET_CROSS_2(in, angle)		(in - tan(M_PI_090 - angle))

#define GET_LENGTH(a, b)					(sqrt((a)*(a) + (b)*(b)))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_RecursiveDown::CFlow_RecursiveDown(void)
{
	Set_Name		(_TL("Flow Accumulation (Flow Tracing)"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Flow tracing algorithms for calculations of flow accumulation and related parameters. "
		"These algorithms trace the flow of each cell in a DEM separately until it finally "
		"leaves the DEM or ends in a sink.\n"
		"The Rho 8 implementation (Fairfield & Leymarie 1991) adopts the original algorithm only "
		"for the flow routing and will give quite different results."
	));

	Add_Reference("Costa-Cabral, M. & Burges, S.J.", "1994",
		"Digital Elevation Model Networks (DEMON): a model of flow over hillslopes for computation of contributing and dispersal areas",
		"Water Resources Research, 30:1681-1692.",
		SG_T("https://doi.org/10.1029/93WR03512"), SG_T("doi:10.1029/93WR03512")
	);

	Add_Reference("Fairfield, J. & Leymarie, P.", "1991",
		"Drainage networks from grid digital elevation models",
		"Water Resources Research, 27:709-717.",
		SG_T("https://doi.org/10.1029/90WR02658"), SG_T("doi:10.1029/90WR02658")
	);

	Add_Reference("Lea, N.L.", "1992",
		"An aspect driven kinematic routing algorithm",
		"In: Parsons, A.J. & Abrahams, A.D. [Eds.], 'Overland Flow: hydraulics and erosion mechanics', London, 147-175.",
		SG_T("https://doi.org/10.1201/b12648"), SG_T("doi:10.1201/b12648")
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Rho 8"),
			_TL("Kinematic Routing Algorithm"),
			_TL("DEMON")
		), 1
	);

	Parameters.Add_Bool("",
		"CORRECT"	, _TL("Flow Correction"),
		_TL(""),
		true
	);

	Parameters.Add_Double("",
		"MINDQV"	, _TL("Minimum DQV"),
		_TL("DEMON - Minimum Drainage Quota Volume (DQV) for traced flow tubes"),
		0., 0., true, 1., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFlow_RecursiveDown::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("CORRECT", pParameter->asInt() >= 1);
		pParameters->Set_Enabled("MINDQV" , pParameter->asInt() == 2);
	}

	return( CFlow::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveDown::On_Initialize(void)
{
	m_Method     = Parameters("METHOD" )->asInt   ();
	m_bWeighting = Parameters("CORRECT")->asBool  ();
	DEMON_minDQV = Parameters("MINDQV" )->asDouble();

	Lock_Create();

	m_Linear.Create(Get_System(), SG_DATATYPE_Float);

	//-----------------------------------------------------
	m_Dir.Create(Get_System(), SG_DATATYPE_Char );
	m_Dif.Create(Get_System(), SG_DATATYPE_Float);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			double	Slope, Aspect;

			if( !m_pDTM->Get_Gradient(x, y, Slope, Aspect) || Aspect < 0. )
			{
				m_Dir.Set_NoData(x, y);
				m_Dif.Set_NoData(x, y);
			}
			else if( m_Method == 0 )	// Rho 8
			{
				m_Dir.Set_Value(x, y, ((int)(Aspect / M_PI_045) % 8));
				m_Dif.Set_Value(x, y,   fmod(Aspect,  M_PI_045) / M_PI_045);
			}
			else						// KRA, DEMON
			{
				m_Dir.Set_Value(x, y, ((int)(Aspect / M_PI_090) % 4) * 2);
				m_Dif.Set_Value(x, y,   fmod(Aspect,  M_PI_090));
			}
		}
	}
}

//---------------------------------------------------------
void CFlow_RecursiveDown::On_Finalize(void)
{
	Lock_Destroy();

	m_Dir.Destroy();
	m_Dif.Destroy();

	//-----------------------------------------------------
	if( m_Linear.is_Valid() && m_pDTM->Set_Index() )
	{
		for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
		{
			int	x, y, i;	double	Flow;

			if( m_pDTM->Get_Sorted(n, x, y) && (Flow = m_Linear.asDouble(x, y)) > 0. )
			{
				Add_Flow(x, y, Flow);

				if( (i = m_pDTM->Get_Gradient_NeighborDir(x, y)) >= 0 )
				{
					x	= Get_xTo(i, x);
					y	= Get_yTo(i, y);

					if( m_pDTM->is_InGrid(x, y) )
					{
						m_Linear.Add_Value(x, y, Flow);
					}
				}
			}
		}

		m_Linear.Destroy();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_RecursiveDown::Calculate(void)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y+=m_Step)
	{
		for(int x=0; x<Get_NX(); x+=m_Step)
		{
			Calculate(x, y);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CFlow_RecursiveDown::Calculate(int x, int y)
{
	double Flow = m_pDTM->is_NoData(x, y) ? 0. : m_pWeights ? m_pWeights->asDouble(x, y) : 1.;

	if( Flow > 0. )
	{
		Add_Flow(x, y, Flow);

		m_Val_Input	= m_pVal_Mean && !m_pVal_Input->is_NoData(x, y) ? m_pVal_Input->asDouble(x, y) : 0.;

		Lock_Set(x, y, 1);

		switch( m_Method )
		{
		case  0: Rho8_Start (x, y, Flow); break;
		case  1: KRA_Start  (x, y, Flow); break;
		default: DEMON_Start(x, y, Flow); break;
		}

		Lock_Set(x, y, 0);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline void CFlow_RecursiveDown::Add_Flow(int x, int y, double Flow)
{
	if( m_pFlow     ) m_pFlow    ->Add_Value(x, y, Flow              );
	if( m_pVal_Mean ) m_pVal_Mean->Add_Value(x, y, Flow * m_Val_Input);
}


///////////////////////////////////////////////////////////
//														 //
//						Rho 8							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveDown::Rho8_Start(int x, int y, double Flow)
{
	if( !m_Dir.is_NoData(x, y) )
	{
		int	Dir	= m_Dir.asInt(x, y);

		if( m_Dif.asDouble(x, y) > CSG_Random::Get_Uniform(0, 1) )
		{
			Dir	= (Dir + 1) % 8;
		}

		int	ix	= Get_xTo(Dir, x);
		int	iy	= Get_yTo(Dir, y);

		if( m_pDTM->is_InGrid(ix, iy) )
		{
			if( is_Locked(ix, iy) || m_pDTM->asDouble(x, y) <= m_pDTM->asDouble(ix, iy) )
			{
				m_Linear.Add_Value(ix, iy, Flow);
			}
			else
			{
				Add_Flow  (ix, iy, Flow);

				Lock_Set  (ix, iy, 1);
				Rho8_Start(ix, iy, Flow);
				Lock_Set  (ix, iy, 0);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//				Kinematic Routing Algorithm				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveDown::KRA_Start(int x, int y, double Flow)
{
	int		dir;
	double	dif	= m_Dif.asDouble(x, y);

	if( dif <= M_PI_045 )	// to the top...
	{
		dir		= m_Dir.asInt(x, y);
		dif		= 0.5 + tan(           dif) / 2.;
	}
	else					// to the right...
	{
		dir		= m_Dir.asInt(x, y) + 2;
		dif		= 0.5 - tan(M_PI_090 - dif) / 2.;
	}

	KRA_Trace(x, y, Flow, dir, dif);
}

//---------------------------------------------------------
void CFlow_RecursiveDown::KRA_Trace(int x, int y, double Flow, int Direction, double from)
{
	bool	bLinear;
	int		dir;
	double	dif, to, weight;

	Direction	%= 8;

	x	= Get_xTo(Direction, x);
	y	= Get_yTo(Direction, y);

	//-----------------------------------------------------
	if( m_pDTM->is_InGrid(x, y) && !is_Locked(x, y) )
	{
		Lock_Set(x, y, 1);

		bLinear	= false;
		weight	= 1.;

		dir		= m_Dir.asInt(x, y);
		dif		= m_Dif.asDouble(x, y);

		//-------------------------------------------------
		if( Direction == dir )						// entering from the bottom...
		{
			if( from > 1. - tan(dif) )				// 1. to the right...
			{
				to		= GET_OUTLET_DIAG__1(from, dif);

				KRA_Trace(x, y, Flow, dir + 2, to);

				if( m_bWeighting )
				{
					weight	= GET_LENGTH(1. - from, 1. - to);
				}
			}
			else									// 2. to the top...
			{
				to		= GET_OUTLET_CROSS_1(from, dif);

				KRA_Trace(x, y, Flow, dir + 0, to);

				if( m_bWeighting )
				{
					weight	= GET_LENGTH(1., to - from);
				}
			}
		}
		else if( (8 + Direction - dir) % 8 == 2 )	// entering from the left...
		{
			if( from < tan(M_PI_090 - dif) )		// 1. to the top...
			{
				to		= GET_OUTLET_DIAG__2(from, dif);

				KRA_Trace(x, y, Flow, dir + 0, to);

				if( m_bWeighting )
				{
					weight	= GET_LENGTH(from, to);
				}
			}
			else									// 2. to the right...
			{
				to		= GET_OUTLET_CROSS_2(from, dif);

				KRA_Trace(x, y, Flow, dir + 2, to);

				if( m_bWeighting )
				{
					weight	= GET_LENGTH(1., from - to);
				}
			}
		}
		else										// go linear...
		{
			bLinear	= true;
		}

		//-------------------------------------------------
		if( bLinear )
		{
			m_Linear.Add_Value(x, y, Flow);
		}
		else
		{
			Add_Flow(x, y, weight * Flow);
		}

		Lock_Set(x, y, 0);
	}
}


///////////////////////////////////////////////////////////
//														 //
//		DEMON - Digital Elevation MOdel Network			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveDown::DEMON_Start(int x, int y, double Flow)
{
	double	dif, flow_A, flow_B;

	if( (dif = m_Dif.asDouble(x, y)) < M_PI_045 )	// mostly to the top...
	{
		flow_B	= tan(dif) / 2.;
		flow_A	= 1. - flow_B;
	}
	else											// mostly to the right...
	{
		flow_A	= tan(M_PI_090 - dif) / 2.;
		flow_B	= 1. - flow_A;
	}

	flow_A	*= Flow;
	flow_B	*= Flow;

	if( flow_A <= DEMON_minDQV )
	{
		DEMON_Trace(x, y, Flow , m_Dir.asInt(x, y) + 2, 0., 1.);	// all to the right...
	}
	else if( flow_B <= DEMON_minDQV )
	{
		DEMON_Trace(x, y, Flow , m_Dir.asInt(x, y) + 0, 0., 1.);	// all to the top...
	}
	else
	{
		DEMON_Trace(x, y, flow_A, m_Dir.asInt(x, y) + 0, 0., 1.);	// to the top...
		DEMON_Trace(x, y, flow_B, m_Dir.asInt(x, y) + 2, 0., 1.);	// to the right...
	}
}

//---------------------------------------------------------
void CFlow_RecursiveDown::DEMON_Trace(int _x, int _y, double Flow, int Direction, double from_A, double from_B)
{
	bool	bLinear;
	int		dir;
	double	dif, to_A, to_B, flow_A, flow_B, weight;

	Direction	%= 8;

	int	x	= Get_xTo(Direction, _x);
	int	y	= Get_yTo(Direction, _y);

	//-----------------------------------------------------
	if( !m_pDTM->is_InGrid(x, y) )
	{
		return;
	}
	else if( is_Locked(x, y) || m_pDTM->asDouble(_x, _y) <= m_pDTM->asDouble(x, y) )
	{
		m_Linear.Add_Value(x, y, Flow);
	}
	else
	{
		Lock_Set(x, y, 1);

		bLinear	= false;
		weight	= 1.;

		dir		= m_Dir.asInt(x, y);
		dif		= m_Dif.asDouble(x, y);

		//-------------------------------------------------
		if( Direction == dir )						// entering from the bottom...
		{
			if( from_A >= 1. - tan(dif) )			// 1. completely to the right...
			{
				to_A	= GET_OUTLET_DIAG__1(from_A, dif);
				to_B	= GET_OUTLET_DIAG__1(from_B, dif);

				DEMON_Trace(x, y, Flow, dir + 2, to_A, to_B);

				if( m_bWeighting )
				{
					//weight	= ((1. - from_A) * (1. - to_A) - (1. - from_B) * (1. - to_B)) / 2.;	// area...
					weight	= GET_LENGTH(1. - (from_B + from_A) / 2., 1. - (to_B + to_A) / 2.);
				}
			}
			else if( from_B < 1. - tan(dif) )		// 2. completely to the top...
			{
				to_A	= GET_OUTLET_CROSS_1(from_A, dif);
				to_B	= GET_OUTLET_CROSS_1(from_B, dif);

				DEMON_Trace(x, y, Flow, dir + 0, to_A, to_B);

				if( m_bWeighting )
				{
					//weight	= from_B - from_A;	// area...
					weight	= GET_LENGTH(1., to_A - from_A);
				}
			}
			else									// 3. partly to the right, partly to the top...
			{
				to_A	= GET_OUTLET_CROSS_1(from_A, dif);
				to_B	= GET_OUTLET_DIAG__1(from_B, dif);

				dif		= 1. - tan(dif);

				flow_A	= dif - from_A;
				flow_B	= from_B - dif;
				flow_A	= Flow * flow_A / (flow_A + flow_B);
				flow_B	= Flow - flow_A;

				if( m_bWeighting )
				{
					//weight	= (dif - from_A) + ((1. - dif) - (1. - from_B) * (1. - to_B)) / 2.;	// area...
					if( (weight = (from_A + from_B) / 2.) < dif )	// to the top...
					{
						weight	= GET_LENGTH(1., to_A - from_A);
					}
					else											// to the right...
					{
						weight	= (1. - weight) / (1. - dif) * GET_LENGTH(1., to_A - from_A);
					}
				}

				if( flow_A <= DEMON_minDQV )
				{
					DEMON_Trace(x, y, Flow, dir + 2, 0., to_B);
				}
				else if( flow_B <= DEMON_minDQV )
				{
					DEMON_Trace(x, y, Flow, dir + 0, to_A, 1.);
				}
				else
				{
					DEMON_Trace(x, y, flow_A, dir + 0, to_A, 1.);
					DEMON_Trace(x, y, flow_B, dir + 2, 0., to_B);
				}
			}
		}
		else if( (8 + Direction - dir) % 8 == 2 )	// entering from the left...
		{
			if( from_B <= tan(M_PI_090 - dif) )		// 1. completely to the top...
			{
				to_A	= GET_OUTLET_DIAG__2(from_A, dif);
				to_B	= GET_OUTLET_DIAG__2(from_B, dif);

				DEMON_Trace(x, y, Flow, dir + 0, to_A, to_B);

				if( m_bWeighting )
				{
					//weight	= (from_B * to_B - from_A * to_A) / 2.;	// area...
					weight	= GET_LENGTH((from_A + from_B) / 2., (to_A + to_B) / 2.);
				}
			}
			else if( from_A > tan(M_PI_090 - dif) )	// 2. completely to the right...
			{
				to_A	= GET_OUTLET_CROSS_2(from_A, dif);
				to_B	= GET_OUTLET_CROSS_2(from_B, dif);

				DEMON_Trace(x, y, Flow, dir + 2, to_A, to_B);

				if( m_bWeighting )
				{
					//weight	= from_B - from_A;	// area...
					weight	= GET_LENGTH(1., from_A - to_A);
				}
			}
			else									// 3. partly to the top, partly to the right...
			{
				to_A	= GET_OUTLET_DIAG__2(from_A, dif);
				to_B	= GET_OUTLET_CROSS_2(from_B, dif);

				dif		= tan(M_PI_090 - dif);

				flow_A	= dif - from_A;
				flow_B	= from_B - dif;
				flow_A	= Flow * flow_A / (flow_A + flow_B);
				flow_B	= Flow - flow_A;

				if( m_bWeighting )
				{
					//weight	= (from_B - dif) + (dif - (from_A * to_A)) / 2.;	// area...
					if( (weight = (from_A + from_B) / 2.) > dif )
					{
						weight	= GET_LENGTH(1., from_B - to_B);
					}
					else
					{
						weight	= weight / dif * GET_LENGTH(1., from_B - to_B);
					}
				}

				if( flow_A <= DEMON_minDQV )
				{
					DEMON_Trace(x, y, Flow, dir + 2, 0., to_B);
				}
				else if( flow_B <= DEMON_minDQV )
				{
					DEMON_Trace(x, y, Flow, dir + 0, to_A, 1.);
				}
				else
				{
					DEMON_Trace(x, y, flow_A, dir + 0, to_A, 1.);
					DEMON_Trace(x, y, flow_B, dir + 2, 0., to_B);
				}
			}
		}
		else
		{
			bLinear	= true;
		}

		//-------------------------------------------------
		if( bLinear )
		{
			m_Linear.Add_Value(x, y, Flow);
		}
		else
		{
			if( m_bWeighting )
			{
				Add_Flow(x, y, weight * Flow);
				//Add_Flow(x, y, weight >= Flow ? Flow : weight * Flow);
			}
			else
			{
				Add_Flow(x, y, Flow);
			}
		}

		Lock_Set(x, y, 0);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
