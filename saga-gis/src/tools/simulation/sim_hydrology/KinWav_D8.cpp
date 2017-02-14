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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define Beta_0	(3.0 / 5.0)
#define Beta_1	(3.0 / 5.0 - 1.0)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKinWav_D8::CKinWav_D8(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Overland Flow - Kinematic Wave D8"));

	Set_Author		("O. Conrad (c) 2003");

	Set_Description	(_TW(
		"Overland Flow - Kinematic Wave D8"
	));

	Add_Reference("Johnson, D.L., Miller, A.C.", "1997",
		"A spatially distributed hydrological model utilizing raster data structures",
		"Computers & Geosciences, Vol.23, No.3, pp.267-272.",
		SG_T("http://www.sciencedirect.com/science/article/pii/S0098300496000842")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const(NULL,
		"ROUGHNESS"		, _TL("Manning Roughness"),
		_TL(""),
		0.03, 0.0, true
	);

	Parameters.Add_Grid(NULL,
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
	Parameters.Add_Table(NULL,
		"GAUGES_FLOW"	, _TL("Flow at Gauges"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(Parameters("GAUGES_FLOW"),
		"GAUGES"		, _TL("Gauges"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	//-----------------------------------------------------
	Parameters.Add_Double(NULL,
		"TIME_SPAN"		, _TL("Simulation Time [h]"),
		_TL(""),
		24.0, 0.0, true
	);

	Parameters.Add_Double(NULL,
		"TIME_STEP"		, _TL("Simulation Time Step [h]"),
		_TL(""),
		0.1, 0.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Node(NULL,
		"MODEL"		, _TL("Model"), _TL(""));

	Parameters.Add_Int(Parameters("MODEL"),
		"MAXITER"		, _TL("Maximum Iterations"),
		_TL(""),
		100, 1, true
	);

	Parameters.Add_Double(Parameters("MODEL"),
		"EPSILON"		, _TL("Epsilon"),
		_TL(""),
		0.0001, 0.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(NULL,
		"P_DISTRIB"		, _TL("Precipitation"),
		_TL("Kind of initializing Precipitation Event"),
		CSG_String::Format("%s|%s|%s|",
			_TL("Homogenous"),
			_TL("Above Elevation"),
			_TL("Left Half")
		)
	);

	Parameters.Add_Double(Parameters("P_DISTRIB"),
		"P_THRESHOLD"	, _TL("Above Elevation"),
		_TL(""),
		0.0
	);

	Parameters.Add_Double(Parameters("P_DISTRIB"),
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

	for(double Time=0.0; Time<=Time_Span && Process_Get_Okay(); Time+=m_dTime)
	{
		Process_Set_Text(CSG_String::Format("%s [h]: %0.2f (%0.2f)", _TL("Simulation Time"), Time, Time_Span));

		Set_Flow(Time);

		for(sLong n=0; n<m_pDEM->Get_NCells() && Process_Get_Okay(); n++)
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

				m_Flow_Last.Set_Value(x, y, Flow);
				m_pFlow   ->Set_Value(x, y, 0.0);
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
	int		Direction	= m_Direction.asChar(x, y);

	if( Direction >= 0 )
	{
		m_pFlow->Set_Value(x, y, 
			Set_Runoff(
				m_pFlow		->asDouble(x, y),
				m_Flow_Last	 .asDouble(x, y),
				m_Alpha		 .asDouble(x, y),
				Get_UnitLength(Direction), 0.0, 0.0
			)
		);

		m_pFlow->Add_Value(Get_xTo(Direction, x), Get_yTo(Direction, y), m_Flow_Last.asDouble(x, y));
	}
}

//---------------------------------------------------------
double CKinWav_D8::Set_Runoff(double q_Up, double q_Last, double alpha, double dL, double r, double r_Last)
{
	double	dTdL, d, c, q, Res, dRes, dR;

	//-----------------------------------------------------
	dTdL	= m_dTime / dL;
	dR		= m_dTime / 2.0 * (r + r_Last);


	//-----------------------------------------------------
	// 1. Initial estimation of q...

	if( q_Last + q_Up != 0.0 )
	{
		d	= alpha * Beta_0 * pow((q_Last + q_Up) / 2.0, Beta_1);
		q	= ( dTdL * q_Up + q_Last * d + dR ) / ( dTdL + d );
	}
	else
	{
		q	= dR;
	}


	//-----------------------------------------------------
	// 2. Newton-Raphson...

	c	= dTdL * q_Up + alpha * pow(q_Last, Beta_0) + dR;

	for(int i=0; i<m_MaxIter; i++)
	{
		if( q <= 0 )
		{
			return( dR );
		}

		Res		= dTdL * q + alpha		    * pow(q, Beta_0) - c;
		dRes	= dTdL     + alpha * Beta_0 * pow(q, Beta_1);
//		if( dRes == 0.0 )	{	return( 0.0 );	}

		d		= Res / dRes;
		q		-= d;

		if( fabs(d) < m_Epsilon )
		{
			break;
		}
	}

	return( q < 0.0 ? 0.0 : q );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKinWav_D8::Initialize(void)
{
	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM" )->asGrid();
	m_pFlow		= Parameters("FLOW")->asGrid();

	m_MaxIter	= Parameters("MAXITER")->asInt();
	m_Epsilon	= Parameters("EPSILON")->asDouble();

	//-----------------------------------------------------
	if( !m_pDEM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	m_Direction.Create(*Get_System(), SG_DATATYPE_Char);
	m_Alpha    .Create(*Get_System(), SG_DATATYPE_Float);
	m_Flow_Last.Create(*Get_System(), SG_DATATYPE_Float);

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
			int		iMax = -1;

			if( m_pDEM->is_NoData(x, y) )
			{
				m_pFlow->Set_NoData(x, y);
			}
			else
			{
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

				if( iMax >= 0 )
				{
					double	Alpha	= pRoughness && !pRoughness->is_NoData(x, y) ? pRoughness->asDouble(x, y) : Roughness;

					Alpha	= pow(Alpha / sqrt(dMax), Beta_0);

					m_Alpha.Set_Value(x, y, Alpha < 10.0 ? Alpha : 10.0);
				}
			}

			m_Direction.Set_Value(x, y, iMax);
		}
	}

	//-----------------------------------------------------
	Gauges_Initialise();

	return( true );
}

//---------------------------------------------------------
bool CKinWav_D8::Finalize(void)
{
	m_Direction.Destroy();
	m_Alpha    .Destroy();
	m_Flow_Last.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
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
