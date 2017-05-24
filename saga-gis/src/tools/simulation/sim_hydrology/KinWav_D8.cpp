/**********************************************************
 * Version $Id: KinWav_D8.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    Kinematic_Wave                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    KinWav_D8.cpp                      //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "KinWav_D8.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKinWav_D8::CKinWav_D8(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Kinematic Wave Overland Flow"));

	Set_Author		("O. Conrad (c) 2003");

	Set_Description	(_TW(
		"This is a simple tool that simulates overland flow by a kinematic wave approach. "
		"It is not designed for operational usage. Rather it should give an idea about "
		"some principles of dynamic simulation techniques and thus it might become a "
		"starting point for more sophisticated and applicable simulation tools. "
	));

	Add_Reference("Johnson, D.L., Miller, A.C.", "1997",
		"A spatially distributed hydrological model utilizing raster data structures",
		"Computers & Geosciences, Vol.23, No.3, pp.267-272.",
		SG_T("http://www.sciencedirect.com/science/article/pii/S0098300496000842")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const("",
		"ROUGHNESS"		, _TL("Manning Roughness"),
		_TL(""),
		0.03, 0.0, true
	);

	Parameters.Add_Grid("",
		"FLOW"			, _TL("Runoff"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool(Parameters("FLOW"),
		"FLOW_RESET"	, _TL("Reset"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"GAUGES_FLOW"	, _TL("Flow at Gauges"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes("GAUGES_FLOW",
		"GAUGES"		, _TL("Gauges"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"TIME_SPAN"		, _TL("Simulation Time [h]"),
		_TL(""),
		24.0, 0.0, true
	);

	Parameters.Add_Double("",
		"TIME_STEP"		, _TL("Simulation Time Step [h]"),
		_TL(""),
		0.1, 0.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"MODEL"			, _TL("Model Options"), _TL("")
	);

	Parameters.Add_Int("MODEL",
		"MAXITER"		, _TL("Maximum Iterations"),
		_TL(""),
		100, 1, true
	);

	Parameters.Add_Double("MODEL",
		"EPSILON"		, _TL("Epsilon"),
		_TL(""),
		0.0001, 0.0, true
	);

	Parameters.Add_Choice("MODEL",
		"ROUTING"		, _TL("Flow Routing"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Deterministic 8"),
			_TL("Multiple Flow Direction")
		), 1
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"P_DISTRIB"		, _TL("Precipitation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Homogenous"),
			_TL("Above Elevation"),
			_TL("Left Half")
		)
	);

	Parameters.Add_Double("P_DISTRIB",
		"P_THRESHOLD"	, _TL("Above Elevation"),
		_TL(""),
		0.0
	);

	Parameters.Add_Double("P_DISTRIB",
		"P_RATE"		, _TL("Precipitation [mm]"),
		_TL(""),
		10.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CKinWav_D8::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "GAUGES_FLOW") )
	{
		pParameters->Set_Enabled("GAUGES", pParameter->asTable() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "P_DISTRIB") )
	{
		pParameters->Set_Enabled("P_THRESHOLD", pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKinWav_D8::On_Execute(void)
{
	//-----------------------------------------------------
	if( !Initialize() )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	Time_Span	= Parameters("TIME_SPAN")->asDouble();

	m_dTime	= Parameters("TIME_STEP")->asDouble();

	for(double Time=0.0; Time<=Time_Span && Set_Progress(Time, Time_Span); Time+=m_dTime)
	{
		Process_Set_Text(CSG_String::Format("%s [h]: %0.2f (%0.2f)", _TL("Simulation Time"), Time, Time_Span));

		Set_Flow(Time);

		for(sLong n=0; n<m_pDEM->Get_NCells(); n++)
		{
			int	x, y;

			if( m_pDEM->Get_Sorted(n, x, y) )
			{
				Set_Runoff(x, y);
			}
		}

		DataObject_Update(m_pFlow, 0.0, 100.0);

		Gauges_Set_Flow(Time);
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CKinWav_D8::Set_Flow(double Time)
{
	int		P_Distribution	= Parameters("P_DISTRIB")->asInt();
	double	P_Threshold		= Parameters("P_THRESHOLD")->asDouble();

	double	P	= Time > 0.0 ? 0.0 : Parameters("P_RATE")->asDouble();

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				double	Flow	= m_pFlow->asDouble(x, y);

				if( P > 0.0 )
				{
					switch( P_Distribution )
					{
					default:
						Flow	+= P;
						break;

					case  1:
						if( P_Threshold <= m_pDEM->asDouble(x, y) )
						{
							Flow	+= P;
						}
						break;

					case  2:
						if( x <= Get_NX() / 2 )
						{
							Flow	+= P;
						}
						break;
					}
				}

				m_Flow_t0.Set_Value(x, y, Flow);
				m_pFlow ->Set_Value(x, y, 0.0);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CKinWav_D8::Set_Runoff(int x, int y)
{
	const double	m	= (5.0 / 3.0);

	//-----------------------------------------------------
	if( m_pFlow->is_NoData(x, y) )
	{
		return;
	}

	//-----------------------------------------------------
	double	Q		= m_pFlow ->asDouble(x, y);
	double	Q_t0	= m_Flow_t0.asDouble(x, y);

	if( Q_t0 + Q <= 0.0 )
	{
		return;
	}

	//-----------------------------------------------------
	// 1. Initial estimation of q...

	double	Alpha	= m_Alpha.asDouble(x, y);
	double	dt_dl	= m_dTime / m_Length.asDouble(x, y);
	double	dQ;

	dQ	= Alpha * m * pow((Q_t0 + Q) / 2, m - 1);
	dQ	= (dt_dl * Q + Q_t0 * dQ) / (dt_dl + dQ);

	//-----------------------------------------------------
	// 2. Newton-Raphson...

	double	C	= dt_dl * Q + Alpha * pow(Q_t0, m);

	for(int i=0; dQ>0.0 && i<m_MaxIter; i++)
	{
		double	r	= dt_dl * dQ + Alpha     * pow(dQ, m    ) - C;
		double	d	= dt_dl      + Alpha * m * pow(dQ, m - 1);

		dQ	-= (d = r / d);

		if( fabs(d) < m_Epsilon )
		{
			break;
		}
	}

	//-----------------------------------------------------
	dQ	*= m_dTime;

	Set_Runoff(x, y, dQ);

	Q	= Q + Q_t0 - dQ;

	m_pFlow->Set_Value(x, y, Q > 0.0 ? Q : 0.0);
}

//---------------------------------------------------------
void CKinWav_D8::Set_Runoff(int x, int y, double Q)
{
	if( Q > 0.0 )
	{
		switch( m_Routing )
		{
		default:
			{
				int	i	= m_dFlow->asInt(x, y);

				if( i >= 0 )
				{
					m_pFlow->Add_Value(Get_xTo(i, x), Get_yTo(i, y), Q);
				}
			}
			break;

		case  1:
			{
				for(int i=0; i<8; i++)
				{
					double	d	= m_dFlow[i].asDouble(x, y);

					if( d > 0.0 )
					{
						m_pFlow->Add_Value(Get_xTo(i, x), Get_yTo(i, y), d * Q);
					}
				}
			}
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKinWav_D8::Initialize(void)
{
	const double	m	= (5.0 / 3.0);

	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM" )->asGrid();
	m_pFlow		= Parameters("FLOW")->asGrid();

	m_MaxIter	= Parameters("MAXITER")->asInt   ();
	m_Epsilon	= Parameters("EPSILON")->asDouble();
	m_Routing	= Parameters("ROUTING")->asInt   ();

	//-----------------------------------------------------
	if( !m_pDEM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	m_Flow_t0.Create(*Get_System(), SG_DATATYPE_Float);
	m_Length .Create(*Get_System(), SG_DATATYPE_Float);
	m_Alpha  .Create(*Get_System(), SG_DATATYPE_Float);

	switch( m_Routing )
	{
	default:
		m_dFlow	= new CSG_Grid(*Get_System(), SG_DATATYPE_Char);
		break;

	case  1:
		m_dFlow	= new CSG_Grid[8];

		for(int i=0; i<8; i++)
		{
			m_dFlow[i].Create(*Get_System(), SG_DATATYPE_Float);
		}
		break;
	}

	//-----------------------------------------------------
	if( Parameters("FLOW_RESET")->asBool() )
	{
		m_pFlow->Assign(0.0);
		DataObject_Set_Colors(m_pFlow, 11, SG_COLORS_WHITE_BLUE);
		DataObject_Update(m_pFlow, 0.0, 100.0, SG_UI_DATAOBJECT_SHOW);
	}

	//-----------------------------------------------------
	CSG_Grid	*pRoughness	= Parameters("ROUGHNESS")->asGrid  ();
	double		  Roughness	= Parameters("ROUGHNESS")->asDouble();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			double	Slope, Aspect;

			if( !m_pDEM->Get_Gradient(x, y, Slope, Aspect) )
			{
				m_pFlow->Set_NoData(x, y);
			}
			else
			{
				double	n	= pRoughness && !pRoughness->is_NoData(x, y) ? pRoughness->asDouble(x, y) : Roughness;

				m_Alpha.Set_Value(x, y, pow(sqrt(tan(Slope)) / (n < 0.001 ? 0.001 : n), m));

				switch( m_Routing )
				{
				default: Set_D8 (x, y); break;
				case  1: Set_MFD(x, y); break;
				}
			}
		}
	}

	//-----------------------------------------------------
	Gauges_Initialise();

	return( true );
}

//---------------------------------------------------------
bool CKinWav_D8::Finalize(void)
{
	switch( m_Routing )
	{
	default: delete  (m_dFlow); break;
	case  1: delete[](m_dFlow); break;
	}

	m_Flow_t0.Destroy();
	m_Length .Destroy();
	m_Alpha  .Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CKinWav_D8::Set_D8(int x, int y)
{
	int		iMax = -1;
	double	d, dMax = 0.0, z = m_pDEM->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) && (d = (z - m_pDEM->asDouble(ix, iy)) / Get_Length(i)) > dMax )
		{
			dMax	= d;
			iMax	= i;
		}
	}

	m_dFlow->Set_Value(x, y, iMax);

	m_Length.Set_Value(x, y, Get_Length(iMax));
}

//---------------------------------------------------------
void CKinWav_D8::Set_MFD(int x, int y)
{
	int		i;
	double	d, dz[8], dzSum = 0.0, z = m_pDEM->asDouble(x, y);

	for(i=0; i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) && (d = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
		{
			dzSum	+=	(dz[i] = d / Get_Length(i));
		}
		else
		{
			dz[i]	= 0.0;
		}
	}

	if( dzSum > 0.0 )
	{
		for(i=0, d=0.0; i<8; i++)
		{
			d	+= Get_Length(i) * dz[i] / dzSum;

			m_dFlow[i].Set_Value(x, y, dz[i] / dzSum);
		}

		m_Length.Set_Value(x, y, d);
	}
	else
	{
		m_Length.Set_Value(x, y, Get_Cellsize());
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKinWav_D8::Gauges_Initialise(void)
{
	//-----------------------------------------------------
	m_pGauges		= Parameters("GAUGES"     )->asShapes();
	m_pGauges_Flow	= Parameters("GAUGES_FLOW")->asTable();

	if( m_pGauges_Flow == NULL )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( m_pGauges == NULL )
	{
		DataObject_Add(m_pGauges = SG_Create_Shapes(SHAPE_TYPE_Point, _TL("Gauges")));

		Parameters("GAUGES")->Set_Value(m_pGauges);

		m_pGauges->Add_Field(_TL("ID"), SG_DATATYPE_Int);

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !m_pDEM->is_NoData(x, y) )
				{
					bool	bBorder	= false;
					bool	bLowest	= true;

					for(int i=0; i<8; i++)
					{
						int	ix	= Get_xTo(i, x);
						int	iy	= Get_yTo(i, y);

						if( !m_pDEM->is_InGrid(ix, iy) )
						{
							bBorder	= true;
						}
						else if( m_pDEM->asDouble(ix, iy) < m_pDEM->asDouble(x, y) )
						{
							bLowest	= false;
						}
					}

					if( bLowest && bBorder )
					{
						CSG_Shape	*pGauge	= m_pGauges->Add_Shape();

						pGauge->Add_Point(Get_System()->Get_Grid_to_World(x, y));
						pGauge->Set_Value(0, m_pGauges->Get_Count() + 1);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	m_pGauges_Flow->Destroy();
	m_pGauges_Flow->Set_Name(_TL("Outlet Hydrographs"));
	m_pGauges_Flow->Add_Field("TIME", SG_DATATYPE_Double);

	for(int i=0; i<m_pGauges->Get_Count(); i++)
	{
		m_pGauges_Flow->Add_Field(CSG_String::Format("GAUGE_%02d", i + 1), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CKinWav_D8::Gauges_Set_Flow(double Time)
{
	if( m_pGauges_Flow && m_pGauges_Flow->Get_Field_Count() == m_pGauges->Get_Count() + 1 )
	{
		CSG_Table_Record	*pRecord	= m_pGauges_Flow->Add_Record();

		pRecord->Set_Value(0, Time);

		for(int i=0; i<m_pGauges->Get_Count(); i++)
		{
			double	Flow;

			if( m_pFlow->Get_Value(m_pGauges->Get_Shape(i)->Get_Point(0), Flow) )
			{
				pRecord->Set_Value(i + 1, Flow);
			}
		}

		DataObject_Update(m_pGauges_Flow);

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
