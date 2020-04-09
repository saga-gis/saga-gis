
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
		"This is a simple tool that simulates overland flow with a kinematic wave approach. "
		"It is not designed for operational usage. Rather it should give an idea about "
		"some principles of dynamic simulation techniques and thus it might become a "
		"starting point for more sophisticated and applicable simulation tools. "
	));

	Add_Reference("Johnson, D.L., Miller, A.C.", "1997",
		"A spatially distributed hydrological model utilizing raster data structures",
		"Computers & Geosciences, Vol.23, No.3, pp.267-272.",
		SG_T("http://www.sciencedirect.com/science/article/pii/S0098300496000842")
	);

	Add_Reference("MacArthur, R., DeVries, J.J.", "1993",
		"Introduction and Application of Kinematic Wave Routing Techniques Using HEC-1",
		"US Army Corps of Engineers, Institute for Water Resources, Hydrologic Engineering Center (HEC).",
		SG_T("https://www.hec.usace.army.mil/publications/TrainingDocuments/TD-10.pdf"), SG_T("online")
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
		1.0, 0.0, true
	);

	Parameters.Add_Double("",
		"TIME_STEP"		, _TL("Simulation Time Step [min]"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Double("",
		"TIME_UPDATE"	, _TL("Map Update Frequency [min]"),
		_TL("If zero each simulation time step will update the flow map."),
		1.0, 0.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"MODEL"			, _TL("Model Options"),
		_TL("")
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
		CSG_String::Format("%s|%s",
			_TL("Deterministic 8"),
			_TL("Multiple Flow Direction")
		), 1
	);

	Parameters.Add_Bool("MODEL",
		"DYNAMIC"		, _TL("Dynamic Water Surface"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	Parameters.Add_Bool("FLOW",
		"FLOW_RESET"	, _TL("Reset"),
		_TL(""),
		true
	);

	Parameters.Add_Double("FLOW",
		"P_RATE"		, _TL("Precipitation [mm]"),
		_TL(""),
		10.0
	);

	Parameters.Add_Choice("P_RATE",
		"P_DISTRIB"		, _TL("Precipitation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Homogenous"),
			_TL("Above Elevation"),
			_TL("Left Half")
		)
	);

	Parameters.Add_Double("P_RATE",
		"P_THRESHOLD"	, _TL("Above Elevation"),
		_TL(""),
		0.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CKinWav_D8::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("DEM") && pParameter->asGrid() )
	{
		pParameters->Set_Parameter("P_THRESHOLD", pParameter->asGrid()->Get_Mean());
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CKinWav_D8::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FLOW") )
	{
		pParameters->Set_Enabled("FLOW_RESET" , pParameter->asGrid() != NULL);
	}

	if( pParameter->Cmp_Identifier("FLOW_RESET") )
	{
		pParameters->Set_Enabled("P_RATE"     , pParameter->asBool());
	}

	if( pParameter->Cmp_Identifier("P_DISTRIB") )
	{
		pParameters->Set_Enabled("P_THRESHOLD", pParameter->asInt() == 1);
	}

	if( pParameter->Cmp_Identifier("GAUGES_FLOW") )
	{
		pParameters->Set_Enabled("GAUGES"     , pParameter->asPointer() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String	Get_Time_String(double Hour)
{
	double	t	= Hour;

	int	h	= (int)t;	t	= (t - h) *   60;	// to minutes
	int	m	= (int)t;	t	= (t - m) *   60;	// to seconds
	int	s	= (int)t;	t	= (t - s) * 1000;	// to milliseconds

	return( CSG_String::Format("%02d:%02d:%02d.%03d", h, m, s, (int)t) );
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
	double	Time_Span	= Parameters("TIME_SPAN"  )->asDouble();
	double	Time_Step	= Parameters("TIME_STEP"  )->asDouble() / 60.;	// from minutes to hours
	double	Update		= Parameters("TIME_UPDATE")->asDouble() / 60.;	// from minutes to hours
	double	Update_Last	= 0.0;

	m_dt	=  Time_Step * 60;	// minutes >> seconds

	for(double Time=0.0; Time<=Time_Span && Set_Progress(Time, Time_Span); Time+=Time_Step)
	{
		Process_Set_Text("%s: %s (%sh)", _TL("Simulation Time"), Get_Time_String(Time).c_str(), Get_Time_String(Time_Span).c_str());

		SG_UI_ProgressAndMsg_Lock(true);

		Set_Flow();

		if( Process_Get_Okay() && Time >= Update_Last )
		{
			if( Update > 0.0 )
			{
				Update_Last	= Update * (1.0 + floor(Time / Update));
			}

			DataObject_Update(m_pFlow);
		}

		Gauges_Set_Flow(Time);

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
bool CKinWav_D8::Initialize(void)
{
	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM"      )->asGrid  ();
	m_pFlow		= Parameters("FLOW"     )->asGrid  ();

	m_MaxIter	= Parameters("MAXITER"  )->asInt   ();
	m_Epsilon	= Parameters("EPSILON"  )->asDouble();
	m_Routing	= Parameters("ROUTING"  )->asInt   ();

	m_pManning	= Parameters("ROUGHNESS")->asGrid  ();
	m_Manning	= Parameters("ROUGHNESS")->asDouble();

	m_bDynamic	= Parameters("DYNAMIC"  )->asBool  ();

	//-----------------------------------------------------
	if( Parameters("FLOW_RESET")->asBool() )
	{
		int		P_Distribution	= Parameters("P_DISTRIB"  )->asInt();
		double	P_Threshold		= Parameters("P_THRESHOLD")->asDouble();
		double	P				= Parameters("P_RATE"     )->asDouble() / 1000.0;	// mm >> m

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				m_pFlow->Set_Value(x, y, 0.0);
			}
			else switch( P_Distribution )
			{
			default: m_pFlow->Set_Value(x, y, P); break;
			case  1: m_pFlow->Set_Value(x, y, P_Threshold <= m_pDEM->asDouble(x, y) ? P : 0.0); break;
			case  2: m_pFlow->Set_Value(x, y, x <= Get_NX() / 2 ? P : 0.0); break;
			}
		}

		DataObject_Set_Colors(m_pFlow, 11, SG_COLORS_WHITE_BLUE);
		DataObject_Update    (m_pFlow, SG_UI_DATAOBJECT_SHOW);
	}

	//-----------------------------------------------------
	m_Flow.Create(Get_System(), SG_DATATYPE_Float);
	m_dx  .Create(Get_System(), SG_DATATYPE_Float);
	m_Q   .Create(Get_System(), SG_DATATYPE_Float);

	switch( m_Routing )
	{
	default:
		m_dFlow->Create(Get_System(), SG_DATATYPE_Char);
		break;

	case  1:
		for(int i=0; i<8; i++)
		{
			m_dFlow[i].Create(Get_System(), SG_DATATYPE_Float);
		}
		break;
	}

	//-----------------------------------------------------
	if( !m_bDynamic )
	{
		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++) if( !m_pDEM->is_NoData(x, y) )
		{
			switch( m_Routing )
			{
			default: Set_D8 (x, y); break;
			case  1: Set_MFD(x, y); break;
			}
		}
	}

	//-----------------------------------------------------
	m_Flow_Sum = m_Flow_Out = 0.0;

	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		m_Flow_Sum	+= m_pFlow->asDouble(x, y);
	}

	//-----------------------------------------------------
	Gauges_Initialise();

	return( true );
}

//---------------------------------------------------------
bool CKinWav_D8::Finalize(void)
{
	for(int i=0; i<8; i++)
	{
		m_dFlow[i].Destroy();
	}

	m_Flow.Destroy();
	m_dx  .Destroy();
	m_Q   .Destroy();

	//-----------------------------------------------------
	double	Flow_Sum	= 0.0;

	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		Flow_Sum	+= m_pFlow->asDouble(x, y);
	}

	double	Flow_Loss	= m_Flow_Sum - (Flow_Sum + m_Flow_Out);

	Message_Fmt("\n%s", _TL("Flow Accumulation"));
	Message_Fmt("\n%s\t:%f"  , _TL("initial"), m_Flow_Sum);
	Message_Fmt("\n%s\t:%f"  , _TL("current"), m_Flow_Out + Flow_Sum);
	Message_Fmt("\n%s\t:%f"  , _TL("outflow"), m_Flow_Out);
	Message_Fmt("\n%s\t:%f"  , _TL("in area"),   Flow_Sum);
	Message_Fmt("\n%s\t:%f"  , _TL("loss"   ),   Flow_Loss);
	Message_Fmt("\n%s\t:%f%%", _TL("balance"),   Flow_Loss * 100 / m_Flow_Sum);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CKinWav_D8::Get_Surface(int x, int y, double dz[8])
{
	double	dzSum	= 0.0;

	if( !m_pDEM->is_NoData(x, y) )
	{
		double	z	= Get_Surface(x, y);

		for(int i=0, ix, iy; i<8; i++)
		{
			if( m_pDEM->is_InGrid(ix = Get_xTo(i, x), iy = Get_yTo(i, y)) )
			{
				dz[i]	= z > (dz[i] = Get_Surface(ix, iy)) ? (z - dz[i]) / Get_Length(i) : 0.0;
			}
			else if( m_pDEM->is_InGrid(ix = Get_xFrom(i, x), iy = Get_yFrom(i, y)) )
			{
				dz[i]	= z < (dz[i] = Get_Surface(ix, iy)) ? (dz[i] - z) / Get_Length(i) : 0.0;
			}
			else
			{
				dz[i]	= 0.0;
			}

			dzSum	+= dz[i];
		}
	}

	return( dzSum );
}

//---------------------------------------------------------
void CKinWav_D8::Set_D8(int x, int y)
{
	double	dz[8], dx = 0.0;
	
	if( Get_Surface(x, y, dz) > 0.0 )
	{
		int		iMax	= -1;
		double	dMax	= 0.0;

		for(int i=0; i<8; i++)
		{
			if( dz[i] > dMax )
			{
				iMax = i; dMax = dz[i];
			}
		}

		m_dFlow->Set_Value(x, y, iMax);

		if( iMax >= 0 )
		{
			dx	= Get_Length(iMax);
		}
	}

	m_dx.Set_Value(x, y, dx);
}

//---------------------------------------------------------
void CKinWav_D8::Set_MFD(int x, int y)
{
	double	dz[8], dx = 0.0, dzSum;

	if( (dzSum = Get_Surface(x, y, dz)) > 0.0 )
	{
		if( dzSum > 0.0 )
		{
			for(int i=0; i<8; i++)
			{
				dx	+= Get_Length(i) * dz[i] / dzSum;

				m_dFlow[i].Set_Value(x, y, dz[i] / dzSum);
			}
		}
	}

	m_dx.Set_Value(x, y, dx);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CKinWav_D8::Set_Flow(void)
{
	int	y;

	#pragma omp parallel for private(y)
	for(y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++) if( !m_pDEM->is_NoData(x, y) )
	{
		m_Flow  .Set_Value(x, y, m_pFlow->asDouble(x, y));
		m_pFlow->Set_Value(x, y, 0.0);

		m_Q     .Set_Value(x, y, Get_Q(x, y));

		if( m_bDynamic )
		{
			switch( m_Routing )
			{
			default: Set_D8 (x, y); break;
			case  1: Set_MFD(x, y); break;
			}
		}
	}

	#pragma omp parallel for private(y)
	for(y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++) if( !m_pDEM->is_NoData(x, y) )
	{
		Set_Runoff(x, y);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CKinWav_D8::Get_Surface(int x, int y)
{
	return( m_bDynamic ? m_pDEM->asDouble(x, y) + m_Flow.asDouble(x, y) : m_pDEM->asDouble(x, y) );
}

//---------------------------------------------------------
inline double CKinWav_D8::Get_Gradient(int x, int y)
{
	#define GET_DZ(i)	{ int ix, iy, ii = 2 * i; dz[i]\
		= m_pDEM->is_InGrid(ix = Get_xTo  (ii), iy = Get_yTo  (ii)) ?     Get_Surface(ix, iy) - z\
		: m_pDEM->is_InGrid(ix = Get_xFrom(ii), iy = Get_yFrom(ii)) ? z - Get_Surface(ix, iy) : 0.0; }

	double	z	= Get_Surface(x, y), dz[4];

	GET_DZ(0); GET_DZ(1); GET_DZ(2); GET_DZ(3);

	double	G	= (dz[0] - dz[2]) / (2. * Get_Cellsize());
    double	H	= (dz[1] - dz[3]) / (2. * Get_Cellsize());

	z	= sqrt(G*G + H*H);

	return( z );
}

//---------------------------------------------------------
inline double CKinWav_D8::Get_Manning(int x, int y)
{
	double	Manning	= m_pManning && !m_pManning->is_NoData(x, y) ? m_pManning->asDouble(x, y) : m_Manning;

	return( Manning <= 0.001 ? 0.001 : Manning );
}

//---------------------------------------------------------
inline double CKinWav_D8::Get_Alpha(int x, int y)
{
	const double	Manning_k	= 1.;	// 1.4859 for English units: pow(1 m, 1/3) / s = pow(3.2808399 ft, 1/3) / s = 1.4859 * pow(1 ft, 1/3) / s

	double	Manning	= m_pManning && !m_pManning->is_NoData(x, y) ? m_pManning->asDouble(x, y) : m_Manning;

	return( (Manning_k / Manning) * sqrt(Get_Gradient(x, y)) );
}

//---------------------------------------------------------
inline double CKinWav_D8::Get_Q(int x, int y)
{
	double	Flow	= m_Flow.asDouble(x, y);

	return( Flow > 0.0 ? Get_Alpha(x, y) * pow(Flow, 5./3.) : 0.0 );
}

//---------------------------------------------------------
void CKinWav_D8::Get_Upslope(int x, int y, double &F, double &Q)
{
	double	n	= F = Q = 0.0;

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) )
		{
			switch( m_Routing )
			{
			default:
			{
				if( m_dFlow->asInt(ix, iy) == (i + 4) % 8 )
				{
					F	+= m_Flow.asDouble(ix, iy);
					Q	+= m_Q   .asDouble(ix, iy);
					n	++;
				}
			}
			break;

			case  1:
			{
				double	d	= m_dFlow[(i + 4) % 8].asDouble(x, y);

				if( d > 0.0 )
				{
					F	+= d * m_Flow.asDouble(ix, iy);
					Q	+= d * m_Q   .asDouble(ix, iy);
					n	+= d;
				}
			}
			break;
			}
		}
	}

	if( n > 0.0 )
	{
		F	/= n;
		Q	/= n;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CKinWav_D8::Set_Runoff(int x, int y)
{
	const double	m	= 5./3.;

	//-----------------------------------------------------
	double	Fup, F	= m_Flow.asDouble(x, y);
	double	Qup, Q	= m_Q   .asDouble(x, y);

	if( F <= 0.0 )
	{
		return;
	}

	double	dx	= m_dx.asDouble(x, y);

	if( dx <= 0.0 )
	{
		m_pFlow->Add_Value(x, y, F);

		return;
	}

	Get_Upslope(x, y, Fup, Qup);

	//-----------------------------------------------------
	// 1. Initial estimation of q...

	double	a		= Get_Alpha(x, y);
	double	dtdx	= m_dt / dx;
	double	q		= (Q + Qup) / 2.0;

//	double	F0	= q * m_dt + F - a * m * dtdx * pow((F + Fup) / 2, m - 1) * (F - Fup);

	//-----------------------------------------------------
	// 2. Newton-Raphson...

	double	C	= a * pow(F, m);

	for(int i=0; q>0.0 && i<m_MaxIter; i++)
	{
		double	r	= m_dt * q + a     * pow(q, m    ) - C;
		double	d	= m_dt     + a * m * pow(q, m - 1);

		q	-= (d = r / d);

		if( fabs(d) < m_Epsilon )
		{
			break;
		}
	}

	//-----------------------------------------------------
	if( q <= 0.0 )
	{
		m_pFlow->Add_Value(x, y, F);
	}
	else
	{
		double	dF	= q * dtdx;

		if( F > dF )
		{
			m_pFlow->Add_Value(x, y, F - dF);
		}
		else
		{
			dF	= F;
		}

		Set_Runoff(x, y, dF);
	}
}

//---------------------------------------------------------
void CKinWav_D8::Set_Runoff(int x, int y, double dF)
{
	switch( m_Routing )
	{
	default: {
		int	i	= m_dFlow->asInt(x, y);

		if( i >= 0 )
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) )
			{
				m_pFlow->Add_Value(Get_xTo(i, x), Get_yTo(i, y), dF);
			}
			else
			{
				m_Flow_Out	+= dF;
			}
		}
	}	break;

	case  1: {
		for(int i=0; i<8; i++)
		{
			double	d	= m_dFlow[i].asDouble(x, y);

			if( d > 0.0 )
			{
				int	ix	= Get_xTo(i, x);
				int	iy	= Get_yTo(i, y);

				if( m_pDEM->is_InGrid(ix, iy) )
				{
					m_pFlow->Add_Value(Get_xTo(i, x), Get_yTo(i, y), d * dF);
				}
				else
				{
					m_Flow_Out	+= d * dF;
				}
			}
		}
	}	break;
	}
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

						pGauge->Add_Point(Get_System().Get_Grid_to_World(x, y));
						pGauge->Set_Value(0, m_pGauges->Get_Count() + 1);
					}
				}
			}
		}

		DataObject_Update(m_pGauges);
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
