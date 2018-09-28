
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//               sim_landscape_evolution                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      salem.cpp                        //
//                                                       //
//              Michael Bock, Olaf Conrad                //
//                      (C) 2017                         //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "salem.h"


///////////////////////////////////////////////////////////
//														 //
//						Climate							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSaLEM_Climate::CSaLEM_Climate(void)
{
	m_pTrend	= NULL;
	m_pAnnual	= NULL;
}

//---------------------------------------------------------
void CSaLEM_Climate::Destroy(void)
{
	m_pTrend	= NULL;
	m_pAnnual	= NULL;
}

//---------------------------------------------------------
bool CSaLEM_Climate::Add_Parameters(CSG_Parameters &Parameters, const CSG_String &Parent)
{
	if( !Parent.is_Empty() )
	{
		Parameters.Add_Node(Parent, Parent, _TL("Climate"), _TL(""));
	}

	//-----------------------------------------------------
	Parameters.Add_Table(Parent,
		"TREND"			, _TL("Long-term Temperature Signal"),
		_TL("Long-term temperature signal, used as adjustment for annual scenarios, i.e. to let their original values appear cooler or warmer."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TREND",
		"TREND_YEAR"	, _TL("Year"),
		_TL("Time expected as 1000 years before present (ka BP).")
	);

	Parameters.Add_Table_Field("TREND",
		"TREND_T"		, _TL("Temperature"),
		_TL("Temperature expected as degree Celsius.")
	);

	Parameters.Add_Double("TREND",
		"TREND_T_OFFSET", _TL("Temperature Offset"),
		_TL("Temperature offset (degree Celsius)."),
		31.0
	);

	//-----------------------------------------------------
	Parameters.Add_Table(Parent,
		"ANNUAL"	  , _TL("Annual Climate"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("ANNUAL",
		"ANNUAL_T"     , _TL("Mean Temperature"),
		_TL("")
	);

	Parameters.Add_Table_Field("ANNUAL",
		"ANNUAL_TMIN"  , _TL("Minimum Temperature"),
		_TL("")
	);

	Parameters.Add_Table_Field("ANNUAL",
		"ANNUAL_TMAX"  , _TL("Maximum Temperature"),
		_TL("")
	);

	Parameters.Add_Table_Field("ANNUAL",
		"ANNUAL_P"     , _TL("Precipitation"),
		_TL("")
	);

	Parameters.Add_Choice("ANNUAL",
		"ANNUAL_T_UNIT"	, _TL("Temperature Unit"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Celsius"),
			_TL("Kelvin")
		), 0
	);

	Parameters.Add_Double(Parent,
		"T_LAPSE"		, _TL("Temperature Lapse Rate"),
		_TL("Temperature lapse rate as degree Celsius per 100 meter."),
		0.6, 0.0, true
	);

	Parameters.Add_Bool("T_LAPSE",
		"T_LAPSE_CELL"	, _TL("Temperature Height Correction"),
		_TL("Cellwise temperature correction applying specified temperature lapse rate to surface elevation."),
		true
	);

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSaLEM_Climate::Set_Parameters(CSG_Parameters &Parameters)
{
	m_pTrend	= Parameters("TREND" )->asTable();
	m_pAnnual	= Parameters("ANNUAL")->asTable();

	if( !m_pTrend  || !m_pTrend ->is_Valid() || m_pTrend ->Get_Count() < 1
	||  !m_pAnnual || !m_pAnnual->is_Valid() || m_pAnnual->Get_Count() < 12 )
	{
		return( false );
	}

	m_fTrend_Year	= Parameters("TREND_YEAR"    )->asInt();
	m_fTrend_T		= Parameters("TREND_T"       )->asInt();
	
	m_fAnnual_T		= Parameters("ANNUAL_T"      )->asInt();
	m_fAnnual_Tmin	= Parameters("ANNUAL_TMIN"   )->asInt();
	m_fAnnual_Tmax	= Parameters("ANNUAL_TMAX"   )->asInt();
	m_fAnnual_P		= Parameters("ANNUAL_P"      )->asInt();

	m_TLapse		= Parameters("T_LAPSE"       )->asDouble() / 100.0;
	m_TLapse_bCell	= Parameters("T_LAPSE_CELL"  )->asBool();

	m_T_Offset		= Parameters("TREND_T_OFFSET")->asDouble() - (Parameters("ANNUAL_T_UNIT")->asInt() == 1 ? 273.15 : 0.0);

	m_pTrend->Set_Index(m_fTrend_Year, TABLE_INDEX_Ascending);

	m_Scenario	= 0;

	return( true );
}

//---------------------------------------------------------
bool CSaLEM_Climate::Set_Year(int Year_BP)
{
	if( !m_pTrend || !m_pTrend->is_Valid() || m_pTrend->Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		i	= 0;

	double	kYear_BP	= -0.001 * Year_BP;

	while( i < m_pTrend->Get_Count() && m_pTrend->Get_Record_byIndex(i)->asDouble(m_fTrend_Year) <= kYear_BP )	// kYears BP (ascending)
	{
		i++;
	}

	//-----------------------------------------------------
	if( i >= m_pTrend->Get_Count() )
	{
		m_TTrend	= m_pTrend->Get_Record_byIndex(i - 1)->asDouble(m_fTrend_T);
	}
	else if( i <= 0 )
	{
		m_TTrend	= m_pTrend->Get_Record_byIndex(    0)->asDouble(m_fTrend_T);
	}
	else
	{
		CSG_Table_Record	*pGE	= m_pTrend->Get_Record_byIndex(i);
		CSG_Table_Record	*pLT	= m_pTrend->Get_Record_byIndex(i - 1);

		double	dY	= pGE->asDouble(m_fTrend_Year) - pLT->asDouble(m_fTrend_Year);
		double	dT	= pGE->asDouble(m_fTrend_T   ) - pLT->asDouble(m_fTrend_T   );

		m_TTrend	= pLT->asDouble(m_fTrend_T) + (kYear_BP - pLT->asDouble(m_fTrend_Year)) * dT / dY;
	}

	m_TTrend	+= m_T_Offset;

	return( true );
}

//---------------------------------------------------------
bool CSaLEM_Climate::Set_Month(int Month)
{
	if( Month == 0 )
	{
		m_Scenario	= (m_Scenario + 1) % (m_pAnnual->Get_Count() / 12);	// number of scenario years
	}

	CSG_Table_Record	*pMonth	= m_pAnnual->Get_Record(12 * m_Scenario + Month % 12);

	m_T		= pMonth->asDouble(m_fAnnual_T   ) + m_TTrend;
	m_Tmin	= pMonth->asDouble(m_fAnnual_Tmin) + m_TTrend;
	m_Tmax	= pMonth->asDouble(m_fAnnual_Tmax) + m_TTrend;
	m_P		= pMonth->asDouble(m_fAnnual_P   );

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                        Bedrock                        //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSaLEM_Bedrock::CSaLEM_Bedrock(void)
{
	m_pRocks		= NULL;

	m_Weathering[0]	= NULL;
	m_Weathering[1]	= NULL;
}

//---------------------------------------------------------
void CSaLEM_Bedrock::Destroy(void)
{
	m_pRocks	= NULL;

	if( m_Weathering[0] ) {	delete[](m_Weathering[0]);	m_Weathering[0]	= NULL;	}
	if( m_Weathering[1] ) {	delete[](m_Weathering[1]);	m_Weathering[1]	= NULL;	}
}

//---------------------------------------------------------
bool CSaLEM_Bedrock::Add_Parameters(CSG_Parameters &Parameters, const CSG_String &Parent)
{
	if( !Parent.is_Empty() )
	{
		Parameters.Add_Node(Parent, Parent, _TL("Bedrock"), _TL(""));
	}

	Parameters.Add_Grid_List(Parent, "ROCK_LAYERS", _TL("Lithology"), _TL(""), PARAMETER_INPUT_OPTIONAL);

	CSG_Table	t;

	t.Add_Field(_TL("Frost"   ), SG_DATATYPE_String);
	t.Add_Field(_TL("Chemical"), SG_DATATYPE_String);

	#define ADD_FORMULAS(a, b)	{ CSG_Table_Record *p = t.Add_Record(); p->Set_Value(0, a); p->Set_Value(1, b); }

	ADD_FORMULAS("0.0250 * (0.00175 * R + T - Tmax) / (-Tamp * cos(S))", "0.0006 * (exp(-4*R) - exp(-6*R)) * (P/0.0002)");
	ADD_FORMULAS("0.0250 * (0.00175 * R + T - Tmax) / (-Tamp * cos(S))", "0.0006 * (exp(-4*R) - exp(-6*R)) * (P/0.0002)");
	ADD_FORMULAS("0.0125 * (0.03750 * R + T - Tmax) / (-Tamp * cos(S))", "0.0050 * (exp(-4*R) - exp(-6*R)) * (P/0.0002)");
	ADD_FORMULAS("0.0075 * (0.07500 * R + T - Tmax) / (-Tamp * cos(S))", "0.0075 * (exp(-4*R) - exp(-6*R)) * (P/0.0002)");
	ADD_FORMULAS("0.0250 * (0.03500 * R + T - Tmax) / (-Tamp * cos(S))", "0.0050 * (exp(-4*R) - exp(-6*R)) * (P/0.0002)");
	ADD_FORMULAS("0.0200 * (0.08750 * R + T - Tmax) / (-Tamp * cos(S))", "0.0060 * (exp(-4*R) - exp(-6*R)) * (P/0.0002)");

	Parameters.Add_FixedTable(Parent,
		"WEATHERING", _TL("Weathering Formulas"),
		_TL("Variables that can be used in formulas are 'T', 'Tmin', 'Tmax', 'Tamp' (mean, minimum, maximum, range of temperature), 'P' (precipitation), 'R' (regolith thickness), 'S' (slope gradient)."),
		&t
	);

	Parameters.Add_Node(Parent,
		"DEFAULTS"	, _TL("Defaults"),
		_TL("")
	);

	Parameters.Add_String("DEFAULTS",
		"FROST"		, _TL("Frost Weathering"),
		_TL(""),
		"0.0250 * (0.00175 * R + T - Tmax) / (-Tamp * cos(S))"
	);

	Parameters.Add_String("DEFAULTS",
		"CHEMICAL"	, _TL("Chemical Weathering"),
		_TL(""),
		"0.0002 * exp(-5.0 * R)"
	);

	return( true );
}

//---------------------------------------------------------
bool CSaLEM_Bedrock::Set_Parameters(CSG_Parameters &Parameters)
{
	Destroy();

	m_pRocks	= Parameters("ROCK_LAYERS")->asGridList();

	//-----------------------------------------------------
	int	n	= m_pRocks->Get_Grid_Count();

	m_Weathering[0]	= new CSG_Formula[1 + n];
	m_Weathering[1]	= new CSG_Formula[1 + n];

	m_Weathering[0][n].Set_Formula(Get_Weathering_Formula(Parameters("FROST"   )->asString(), "0"));	// default
	m_Weathering[1][n].Set_Formula(Get_Weathering_Formula(Parameters("CHEMICAL")->asString(), "0"));	// default

	CSG_Table	&Formulas	= *Parameters("WEATHERING")->asTable();

	for(int i=0; i<n; i++)
	{
		if( i < Formulas.Get_Count() )
		{
			m_Weathering[0][i].Set_Formula(Get_Weathering_Formula(Formulas[i].asString(0), m_Weathering[0][n].Get_Formula()));
			m_Weathering[1][i].Set_Formula(Get_Weathering_Formula(Formulas[i].asString(1), m_Weathering[1][n].Get_Formula()));
		}
		else
		{
			m_Weathering[0][i].Set_Formula(m_Weathering[0][n].Get_Formula());
			m_Weathering[1][i].Set_Formula(m_Weathering[1][n].Get_Formula());
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSaLEM_Bedrock::Get_Bedrock_Index(int x, int y, double z) const
{
	int		iMax	= -1;
	double	zMax;

	for(int i=0; i<m_pRocks->Get_Grid_Count(); i++)
	{
		if( !m_pRocks->Get_Grid(i)->is_NoData(x, y) )
		{
			double	iz	= m_pRocks->Get_Grid(i)->asDouble(x, y);

			if( z <= iz && (iMax < 0 || iz < zMax) )
			{
				iMax	= i;
				zMax	= iz;
			}
		}
	}

	return( iMax < 0 ? m_pRocks->Get_Grid_Count() : iMax );
}

//---------------------------------------------------------
CSG_String CSaLEM_Bedrock::Get_Bedrock_Name(int x, int y, double z) const
{
	int		i	= Get_Bedrock_Index(x, y, z);

	if( i < m_pRocks->Get_Grid_Count() )
	{
		return( m_pRocks->Get_Grid(i)->Get_Name() );
	}
	
	return( _TL("unknown") );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	WV_Tmin	,	// Temperature minimum
	WV_Tmax	,	// Temperature maximum
	WV_Tamp	,	// Temperature amplitude
	WV_T	,	// Temperature mean
	WV_P	,	// Precipitation
	WV_S	,	// Slope
	WV_R	,	// Regolith cover thickness
	WV_Count
};

//---------------------------------------------------------
CSG_String CSaLEM_Bedrock::Get_Weathering_Formula(const CSG_String &Formula, const CSG_String &Default)
{
	const char	Var_Code[]	= "abcdefghijklmnopqrstuvwxyz";
	//                         01234567890134567890123456
	//                         V       012345678901234567

	const char	Var_Key[WV_Count][8]	=
	{
		"Tmin",	// Temperature minimum
		"Tmax",	// Temperature maximum
		"Tamp",	// Temperature amplitude
		"T"   ,	// Temperature mean
		"P"   ,	// Precipitation mean
		"S"   ,	// Slope
		"R"    	// Regolith cover thickness
	};

	//-----------------------------------------------------
	CSG_String	s	= Formula; s.Trim(true); s.Trim(false);

	for(int i=0; i<WV_Count; i++)
	{
		s.Replace(Var_Key[i], Var_Code[i]);
	}

	CSG_Formula	f;

	if( !f.Set_Formula(s) )
	{
		f.Get_Error(s);

		return( Default );
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSaLEM_Bedrock::Set_Weathering(double dTime, CSaLEM_Climate &Climate, CSG_Grid &Surface, CSG_Grid &Slope, CSG_Grid &Regolith)
{
	//-----------------------------------------------------
	dTime	*= 0.01 / 12.;

	for(int Month=0; Month<12; Month++)
	{
		Climate.Set_Month(Month);

		#pragma omp parallel for
		for(int y=0; y<Surface.Get_NY(); y++)
		{
			CSG_Vector	Values(WV_Count);

			Values[WV_T   ]	= Climate.Get_T   ();
			Values[WV_Tmin]	= Climate.Get_Tmin();
			Values[WV_Tmax]	= Climate.Get_Tmax();
			Values[WV_Tamp]	= Climate.Get_Tamp();
			Values[WV_P   ]	= Climate.Get_P   ();

			for(int x=0; x<Surface.Get_NX(); x++)
			{
				Values[WV_S]	= Slope   .asDouble(x, y);
				Values[WV_R]	= Regolith.asDouble(x, y);

				if( Climate.Get_TLapse_Cellwise() )
				{
					double	dT	= Climate.Get_TLapse() * Surface.asDouble(x, y);

					Values[WV_T   ]	= Climate.Get_T   () - dT;
					Values[WV_Tmin]	= Climate.Get_Tmin() - dT;
					Values[WV_Tmax]	= Climate.Get_Tmax() - dT;
				}

				if( Values[WV_Tmax] > 0.0 )	// no significant weathering under permanent frost !
				{
					int	i	= Get_Bedrock_Index(x, y, Surface.asDouble(x, y) - Regolith.asDouble(x, y));

					double	dR	= 0.0;

					if( Values[WV_Tmin] < 0.0 )	{	dR	+= m_Weathering[0][i].Get_Value(Values);	}	// frost change
					if( Values[WV_T   ] > 0.0 )	{	dR	+= m_Weathering[1][i].Get_Value(Values);	}	// chemical weathering

					if( dR > 0.0 )
					{
						Regolith.Add_Value(x, y, dTime * dR);
					}
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Tracers							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	TRACER_TID	= 0,
	TRACER_ROCKTYPE,
	TRACER_ORIGIN_X,
	TRACER_ORIGIN_Y,
	TRACER_HEIGHT,
	TRACER_DEPTH,
	TRACER_DISTANCE,
	TRACER_T_WEATHERED,
	TRACER_T_MOV_FIRST,
	TRACER_T_MOV_LAST
};

//---------------------------------------------------------
CSaLEM_Tracers::CSaLEM_Tracers(void)
{
	m_Candidates.Create(SHAPE_TYPE_Point, _TL("Tracers"), NULL, SG_VERTEX_TYPE_XYZ);

	m_Candidates.Add_Field("TID"        , SG_DATATYPE_Int   );
	m_Candidates.Add_Field("ROCKTYPE"   , SG_DATATYPE_String);
	m_Candidates.Add_Field("ORIGIN_X"   , SG_DATATYPE_Double);
	m_Candidates.Add_Field("ORIGIN_Y"   , SG_DATATYPE_Double);
	m_Candidates.Add_Field("HEIGHT"     , SG_DATATYPE_Double);
	m_Candidates.Add_Field("DEPTH"      , SG_DATATYPE_Double);
	m_Candidates.Add_Field("DISTANCE"   , SG_DATATYPE_Double);
	m_Candidates.Add_Field("T_WEATHERED", SG_DATATYPE_Int   );
	m_Candidates.Add_Field("T_MOV_FIRST", SG_DATATYPE_Int   );
	m_Candidates.Add_Field("T_MOV_LAST" , SG_DATATYPE_Int   );
}

//---------------------------------------------------------
void CSaLEM_Tracers::Destroy(void)
{
	m_Candidates.Del_Records();

	m_Bedrock.Destroy();

	if( m_Trim == 1 && m_pPoints )
	{
		for(int i=m_Trim_Points.Get_Count()-1; i>=0; i--)	// remove those tracers and paths that have left the scene
		{
			m_pPoints->Add_Shape(m_Trim_Points.Get_Shape(i)); m_Trim_Points.Del_Shape(i);

			if( m_pLines )
			{
				m_pLines->Add_Shape(m_Trim_Lines.Get_Shape(i)); m_Trim_Lines.Del_Shape(i);
			}
		}

		m_Trim_Points.Destroy();
		m_Trim_Lines .Destroy();
	}

	m_pPoints	= NULL;
	m_pLines	= NULL;
}

//---------------------------------------------------------
bool CSaLEM_Tracers::Add_Parameters(CSG_Parameters &Parameters, const CSG_String &Parent)
{
	if( !Parent.is_Empty() )
	{
		Parameters.Add_Node(Parent, Parent, _TL("Tracers"), _TL(""));
	}

	Parameters.Add_Shapes(Parent,
		"POINTS"	, _TL("Tracer"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(Parent,
		"LINES"		, _TL("Tracer Paths"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice("",
		"TRIM"		, _TL("Trim"),
		_TL("Remove tracers and associated paths that moved out of the investigated area (performance gain)."),
		CSG_String::Format("%s|%s|%s|",
			_TL("no"),
			_TL("temporarily"),
			_TL("permanently")
		), 1
	);

	Parameters.Add_Double("",
		"DIR_RAND"	, _TL("Randomize Direction"),
		_TL("Uncertainty in routing direction expressed as standard deviation (degree)."),
		0.0, 0.0, true
	);

	Parameters.Add_Int("",
		"H_DENSITY"	, _TL("Horizontal Density"),
		_TL("Horizontal tracer density in cells."),
		1, 1, true
	);

	Parameters.Add_Bool("H_DENSITY",
		"H_RANDOM"	, _TL("Randomize"),
		_TL(""),
		true
	);

	Parameters.Add_Double("",
		"V_DENSITY"	, _TL("Vertical Density"),
		_TL("Vertical tracer density in meter."), 0.5, 0.01, true
	);

	Parameters.Add_Bool("V_DENSITY",
		"V_RANDOM"	, _TL("Randomize"),
		_TL(""),
		true
	);

	return( true );
}

//---------------------------------------------------------
int CSaLEM_Tracers::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameters->Cmp_Identifier("TRACERS") )
	{
		if( pParameter->Cmp_Identifier("POINTS") )
		{
			pParameters->Set_Enabled("LINES"    , pParameter->asDataObject() != NULL);
			pParameters->Set_Enabled("TRIM"     , pParameter->asDataObject() != NULL);
			pParameters->Set_Enabled("DIR_RAND" , pParameter->asDataObject() != NULL);
			pParameters->Set_Enabled("H_DENSITY", pParameter->asDataObject() != NULL);
			pParameters->Set_Enabled("V_DENSITY", pParameter->asDataObject() != NULL);
		}
	}

	return( 0 );
}

//---------------------------------------------------------
bool CSaLEM_Tracers::Set_Parameters(CSG_Parameters &Parameters, CSG_Grid *pSurface)
{
	Destroy();

	if( !pSurface || !pSurface->is_Valid() )
	{
		return( false );
	}

	if( (m_pPoints = Parameters("POINTS")->asShapes()) != NULL )
	{
		m_pPoints->Create(SHAPE_TYPE_Point, _TL("Tracers"), &m_Candidates, SG_VERTEX_TYPE_XYZ);

		if( (m_pLines = Parameters("LINES")->asShapes()) != NULL )
		{
			m_pLines->Create(SHAPE_TYPE_Line, _TL("Tracer Paths"), NULL, SG_VERTEX_TYPE_XYZ);
			m_pLines->Add_Field("TID"     , SG_DATATYPE_Int);
			m_pLines->Add_Field("ROCKTYPE", SG_DATATYPE_String);
		}

		m_Trim			= Parameters("TRIM"     )->asInt   ();
		m_dDirection	= Parameters("DIR_RAND" )->asDouble() * M_DEG_TO_RAD;
		m_hDensity		= Parameters("H_DENSITY")->asInt   ();
		m_hRandom		= Parameters("H_RANDOM" )->asBool  ();
		m_vDensity		= Parameters("V_DENSITY")->asDouble();
		m_vRandom		= Parameters("V_RANDOM" )->asBool  ();

		m_Bedrock.Create(*pSurface);

		if( m_Trim == 1 )
		{
			m_Trim_Points.Create(*m_pPoints);

			if( m_pLines )
			{
				m_Trim_Lines.Create(*m_pLines);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
int CSaLEM_Tracers::Add_Tracers(double Time, CSG_Grid &Surface, CSG_Grid &Regolith, const CSaLEM_Bedrock &Bedrock)
{
	int	nAdded	= 0;

	if( m_pPoints )
	{
		for(int y=m_hDensity/2; y<Surface.Get_NY(); y+=m_hDensity)
		{
			if( y == 0 || y >= Surface.Get_NY() - 1 )	continue;

			for(int x=m_hDensity/2; x<Surface.Get_NX(); x+=m_hDensity)
			{
				if( x == 0 || x >= Surface.Get_NX() - 1 || Surface.is_NoData(x, y) )	continue;

				double	z, zBedrock	= Surface.asDouble(x, y) - Regolith.asDouble(x, y);

				while( zBedrock < (z = m_Bedrock.asDouble(x, y)) )
				{
					m_Bedrock.Set_Value(x, y, z - m_vDensity);
					
					z	-= m_vDensity * (m_vRandom ? CSG_Random::Get_Uniform(0, 1) : 0.5);

					CSG_Shape	*pTracer	= m_Candidates.Add_Shape();

					TSG_Point	p	= Surface.Get_System().Get_Grid_to_World(x, y);

					if( m_hRandom )
					{
						p.x	+= CSG_Random::Get_Uniform(-0.5, 0.5) * Surface.Get_Cellsize();
						p.y	+= CSG_Random::Get_Uniform(-0.5, 0.5) * Surface.Get_Cellsize();
					}

					pTracer->Set_Point(p, 0); pTracer->Set_Z(z, 0);
					pTracer->Set_Value(TRACER_ROCKTYPE   , Bedrock.Get_Bedrock_Name(x, y, z));
					pTracer->Set_Value(TRACER_ORIGIN_X   , p.x);
					pTracer->Set_Value(TRACER_ORIGIN_Y   , p.y);
					pTracer->Set_Value(TRACER_HEIGHT     ,   z);
					pTracer->Set_Value(TRACER_DEPTH      , Surface.asDouble(x, y) - z);
					pTracer->Set_Value(TRACER_DISTANCE   , 0.0);
					pTracer->Set_Value(TRACER_T_WEATHERED, Time);

					nAdded++;
				}
			}
		}
	}

	return( nAdded );
}

//---------------------------------------------------------
inline bool CSaLEM_Tracers::Do_Move(double Depth, double dActive)
{
	if( dActive < 0.0 || Depth > dActive )	return( false );

	if( Depth <= 0.0 )	return( true );

	return( CSG_Random::Get_Uniform(0, 1) < (Depth / dActive) );
}

//---------------------------------------------------------
bool CSaLEM_Tracers::Set_Tracers(double Time, double k, CSG_Grid &Surface, CSG_Grid Gradient[3], CSG_Grid &dHin, CSG_Grid &dHout)
{
	if( !m_pPoints )
	{
		return( false );
	}

	CSG_Grid_System	System(Surface.Get_System());

	int		i;

	//-----------------------------------------------------
	for(i=m_Candidates.Get_Count()-1; i>=0; i--)	// candidates' first move ?
	{
		CSG_Shape	*pTracer	= m_Candidates.Get_Shape(i);

		TSG_Point	p	= pTracer->Get_Point(0);

		double		Height;

		if( Surface.Get_Value(p, Height) )
		{
			double	Depth	= Height - pTracer->asDouble(TRACER_HEIGHT);

			if( Depth <= dHout.Get_Value(p) )	// we take dHout as rough estimation of potentially active layer thickness
			{
				pTracer	= m_pPoints->Add_Shape(pTracer);

				pTracer->Set_Value(TRACER_TID, m_pPoints->Get_Count());
				pTracer->Set_Value(TRACER_T_MOV_FIRST, Time);

				m_Candidates.Del_Shape(i);

				if( m_pLines )
				{
					CSG_Shape	*pLine	= m_pLines->Add_Shape();

					pLine->Add_Point(pTracer->Get_Point(0));
					pLine->Set_Z(pTracer->Get_Z(0), pLine->Get_Point_Count() - 1);
					pLine->Set_Value(0, pTracer->asInt   (TRACER_TID     ));
					pLine->Set_Value(1, pTracer->asString(TRACER_ROCKTYPE));
				}
			}
		}
	}

	//-----------------------------------------------------
	#pragma omp parallel for private(i)
	for(i=0; i<m_pPoints->Get_Count(); i++)
	{
		CSG_Shape	*pTracer	= m_pPoints->Get_Shape(i);

		TSG_Point	p	= pTracer->Get_Point(0);

		double		Height;

		if( Surface.Get_Value(p, Height) )
		{
			double	Depth	= Height - pTracer->asDouble(TRACER_HEIGHT);

			if( !Do_Move(Depth, dHout.Get_Value(p)) )	// tracer will not move because it's deeper than the active layer is thick
			{
				pTracer->Set_Value(TRACER_DEPTH, Depth);
			}
			else
			{
				//-----------------------------------------
				{	// aspect driven routing
					double	s, sin_a, cos_a;

					if( Gradient[0].Get_Value(p, s) && Gradient[1].Get_Value(p, sin_a) && Gradient[2].Get_Value(p, cos_a) )
					{
						double	tanS	= tan(s);

						if( m_dDirection > 0.0 )	// directional uncertainty in dependence of slope gradient
						{
							double	a	= CSG_Random::Get_Gaussian(atan2(sin_a, cos_a), exp(-tanS) * m_dDirection);

							sin_a	= sin(a);
							cos_a	= cos(a);
						}

						double	d	= tanS * k * System.Get_Cellsize();

						p.x	+= sin_a * d;
						p.y	+= cos_a * d;
					}
				}

				//-----------------------------------------
				if( p.x != pTracer->Get_Point(0).x || p.y != pTracer->Get_Point(0).y )	// tracer has moved
				{
					pTracer->Add_Value(TRACER_DISTANCE, SG_Get_Distance(p, pTracer->Get_Point(0)));
					pTracer->Set_Value(TRACER_T_MOV_LAST, Time);
					pTracer->Set_Point(p, 0);

					if( Surface.Get_Value(p, Height) )	// returns false once the tracer was moved out of the investigated area
					{
						double	in	= dHin .Get_Value(p);
						double	out	= dHout.Get_Value(p);

						Depth   = in * pow(CSG_Random::Get_Uniform(0, 1), 3);	// bury tracer using some randomness
						Height += (in - out);

						pTracer->Set_Z    (               Height - Depth, 0);
						pTracer->Set_Value(TRACER_HEIGHT, Height - Depth);
						pTracer->Set_Value(TRACER_DEPTH , Depth);
					}

					if( m_pLines )
					{
						CSG_Shape	*pLine	= m_pLines->Get_Shape(i);
						pLine->Add_Point(pTracer->Get_Point(0));
						pLine->Set_Z(Height, pLine->Get_Point_Count() - 1);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_Trim )
	{
		for(i=m_pPoints->Get_Count()-1; i>=0; i--)	// remove those tracers and paths that have left the scene
		{
			if( !Surface.is_InGrid_byPos(m_pPoints->Get_Shape(i)->Get_Point(0)) )
			{
				if( m_Trim == 1 )
				{
					m_Trim_Points.Add_Shape(m_pPoints->Get_Shape(i));
				}

				m_pPoints->Del_Shape(i);

				if( m_pLines )
				{
					if( m_Trim == 1 )
					{
						m_Trim_Lines.Add_Shape(m_pLines->Get_Shape(i));
					}

					m_pLines->Del_Shape(i);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSaLEM::CSaLEM(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("SaLEM"));

	Set_Author		("M.Bock, O.Conrad (c) 2017");

	Set_Description	(_TW(
		"This is the implementation of a Soil and Landscape Evolution Model (SaLEM) "
		"for the spatiotemporal investigation of soil parent material evolution following "
		"a lithologically differentiated approach. The model needs a digital elevation "
		"model and (paleo-)climatic data for the simulation of weathering, erosion and "
		"transport processes. Weathering is controlled by user defined functions in "
		"dependence of climate conditions, local slope, regolith cover and outcropping "
		"bedrock lithology. Lithology can be supplied as a set of grids, of which each "
		"grid represents the top elevation of the underlying bedrock type. "
	));

	Add_Reference(
		"Bock, M., Conrad, O., Guenther, A., Gehrt, E., Baritz, R., and Boehner, J.", "2018",
		"SaLEM (v1.0) - the Soil and Landscape Evolution Model (SaLEM) for simulation of regolith depth in periglacial environments",
		"Geosci. Model Dev., 11, 1641-1652.",
		SG_T("https://doi.org/10.5194/gmd-11-1641-2018")
	);

	Add_Reference(
		"Alley, R.", "2000",
		"The Younger Dryas cold interval as viewed from central Greenland",
		"Quaternary Science Reviews 19: 213-226."
	);

	Add_Reference(
		"Tucker, G.E. & Slingerland, R.", "1994",
		"Erosional dynamics, flexural isostasy, and long-lived escarpments: A numerical modeling study",
		"Journal of Geophysical Research 99: 12229-12243."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"SURFACE_T0"	, _TL("Initial Surface Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_or_Const("",
		"REGOLITH_T0"	, _TL("Initial Regolith Thickness"),
		_TL("Initial sediment cover (m)"),
		0.0, 0.0, true
	);

	Parameters.Add_Grid_or_Const("",
		"ALLOCHTHONE"	, _TL("Allochthone Input"),
		_TL("Allochthone input in meter per year (e.g. of aeolian origin, 'Loess')."),
		0.0, 0.0, true
	);

	Parameters.Add_Grid("",
		"SURFACE"		 , _TL("Surface Elevation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"REGOLITH"		, _TL("Regolith Thickness"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"DIFFERENCE"	, _TL("Difference"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	m_Climate.Add_Parameters(*Parameters.Add_Parameters("", "CLIMATE", _TL("Climate"), _TL(""))->asParameters(), "");
	m_Bedrock.Add_Parameters(*Parameters.Add_Parameters("", "BEDROCK", _TL("Bedrock"), _TL(""))->asParameters(), "");
	m_Tracers.Add_Parameters(*Parameters.Add_Parameters("", "TRACERS", _TL("Tracers"), _TL(""))->asParameters(), "");

	//-----------------------------------------------------
	Parameters.Add_Node("", "TIME", _TL("Simulation Time"), _TL(""));

	Parameters.Add_Int("TIME", "TIME_START", _TL("Start [Years BP]"), _TL(""), 50000);
	Parameters.Add_Int("TIME", "TIME_STOP" , _TL("Stop [Years BP]" ), _TL(""), 10000);
	Parameters.Add_Int("TIME", "TIME_STEP" , _TL("Step [Years]"    ), _TL(""), 100, 1, true);

	//-----------------------------------------------------
	Parameters.Add_Node("", "DIFFUSIVE", _TL("Diffusive Hillslope Processes"), _TL(""));

	Parameters.Add_Double("DIFFUSIVE",
		"DIFFUSIVE_KD", _TL("Diffusivity Coefficient Kd [m^2/a]"),
		_TL(""),
		0.01, 0.0, true
	);

	Parameters.Add_Choice("DIFFUSIVE",
		"DIFFUSIVE_NEIGHBOURS", _TL("Neighbourhood"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Neumann"),
			_TL("Moore")
		), 1
	);

	//-----------------------------------------------------
	if( SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Int("", "UPDATE", _TL("Display Update Frequency"),
			_TL("The frequency (simulation time steps) with which associated maps will be updated. Set to zero to switch off."),
			1, 0, true
		);

		Parameters.Add_Bool  ("UPDATE"    , "UPDATE_VEC", _TL("Vectors"        ), _TL(""), true);

		Parameters.Add_Bool  ("UPDATE"    , "UPDATE_ADJ", _TL("Adjust Regolith"), _TL(""), true);
		Parameters.Add_Double("UPDATE_ADJ", "UPDATE_MIN", _TL("Minimum"        ), _TL(""), 0.0, 0.0, true);
		Parameters.Add_Double("UPDATE_ADJ", "UPDATE_MAX", _TL("Maximum"        ), _TL(""), 2.0, 0.0, true);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSaLEM::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Tracers.On_Parameters_Enable(pParameters, pParameter);

	if( pParameter->Cmp_Identifier("UPDATE") )
	{
		pParameters->Set_Enabled("UPDATE_ADJ", pParameter->asInt() > 0);
		pParameters->Set_Enabled("UPDATE_VEC", pParameter->asInt() > 0);
	}

	if( pParameter->Cmp_Identifier("UPDATE_ADJ") )
	{
		pParameters->Set_Enabled("UPDATE_MIN", pParameter->asBool());
		pParameters->Set_Enabled("UPDATE_MAX", pParameter->asBool());
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSaLEM::On_Execute(void)
{
	if( !Initialize() )
	{
		Error_Set(_TL("initialization failed"));

		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	int	time_start	= -Parameters("TIME_START")->asInt();
	int	time_stop	= -Parameters("TIME_STOP" )->asInt();

	m_dTime	= Parameters("TIME_STEP")->asInt();

 	//-----------------------------------------------------
	int	Update	= Parameters("UPDATE") ? Parameters("UPDATE")->asInt() * m_dTime : 0;

	if( Update > 0 && Parameters("UPDATE_ADJ")->asBool() )
	{
		DataObject_Update(m_pRegolith,
			Parameters("UPDATE_MIN")->asDouble(),
			Parameters("UPDATE_MAX")->asDouble(), true
		);
	}

	//-----------------------------------------------------
	for(m_Time=time_start; m_Time<=time_stop && Set_Progress(m_Time-time_start, time_stop-time_start); m_Time+=m_dTime)
	{
		Process_Set_Text("%s: %d", _TL("Years BP"), -m_Time);

		SG_UI_Progress_Lock(true);

		//-------------------------------------------------
		Set_Gradient   ();

		Set_Allochthone();

		Set_Weathering ();

		Set_Diffusive  ();	// diffusive regolith creep

		//-------------------------------------------------
		if( Update > 0 && ((m_Time - time_start) % Update) == 0 )
		{
			if( Parameters("UPDATE_ADJ")->asBool() )
			{
				DataObject_Update(m_pRegolith,
					Parameters("UPDATE_MIN")->asDouble(),
					Parameters("UPDATE_MAX")->asDouble()
				);
			}
			else
			{
				DataObject_Update(m_pRegolith);
			}

			if( Parameters("UPDATE_VEC")->asBool() )
			{
				DataObject_Update(m_Tracers.Get_Tracers());
				DataObject_Update(m_Tracers.Get_Paths  ());
			}
		}

		SG_UI_Progress_Lock(false);
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSaLEM::Initialize(void)
{
	m_pSurface	= Parameters("SURFACE" )->asGrid();
	m_pRegolith	= Parameters("REGOLITH")->asGrid();

	//-----------------------------------------------------
	m_pSurface->Assign(Parameters("SURFACE_T0")->asGrid());

	if( Parameters("REGOLITH_T0")->asGrid() == NULL )
	{
		m_pRegolith->Assign(Parameters("REGOLITH_T0")->asDouble());
	}
	else if( Parameters("REGOLITH_T0")->asGrid() != m_pRegolith )
	{
		m_pRegolith->Assign(Parameters("REGOLITH_T0")->asGrid());
	}

	//-----------------------------------------------------
	if( !m_Climate.Set_Parameters(*Parameters("CLIMATE")->asParameters())
	||  !m_Bedrock.Set_Parameters(*Parameters("BEDROCK")->asParameters())
	||  !m_Tracers.Set_Parameters(*Parameters("TRACERS")->asParameters(), m_pSurface) )
	{
		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CSaLEM::Finalize(void)
{
	m_Climate.Destroy();
	m_Bedrock.Destroy();
	m_Tracers.Destroy();

	m_Gradient[0].Destroy();
	m_Gradient[1].Destroy();
	m_Gradient[2].Destroy();

	//-----------------------------------------------------
	if( Parameters("DIFFERENCE")->asGrid() )
	{
		CSG_Grid	*pDifference	= Parameters("DIFFERENCE")->asGrid();
		CSG_Grid	*pSurface_T0	= Parameters("SURFACE_T0")->asGrid();

		#pragma omp parallel for
		for(sLong i=0; i<Get_NCells(); i++)
		{
			if( m_pSurface->is_NoData(i) )
			{
				pDifference->Set_NoData(i);
			}
			else
			{
				pDifference->Set_Value(i, m_pSurface->asDouble(i) - pSurface_T0->asDouble(i));
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
bool CSaLEM::Set_Gradient(void)
{
	if( !Get_System()->is_Equal(m_Gradient[0].Get_System()) )
	{
		m_Gradient[0].Create(*Get_System());	// slope gradient [radians]
		m_Gradient[1].Create(*Get_System());	// sine of aspect
		m_Gradient[2].Create(*Get_System());	// cosine of aspect
	}

	#pragma omp parallel for
	for(int y=0; y<m_pSurface->Get_NY(); y++)
	{
		for(int x=0; x<m_pSurface->Get_NX(); x++)
		{
			double	s, a;
			
			bool	bOkay	= m_pSurface->Get_Gradient(x, y, s, a);

			//---------------------------------------------
			if( bOkay )	// the following lines make sure that aspect always points downslope (if possible)
			{
				int	i	= (int)(0.5 + a * 8. / M_PI_360);

				int	ix	= Get_xTo(i, x);
				int	iy	= Get_yTo(i, y);

				if( m_pSurface->is_InGrid(ix, iy) && m_pSurface->asDouble(x, y) < m_pSurface->asDouble(ix, iy) )
				{
					i	= m_pSurface->Get_Gradient_NeighborDir(x, y);

					if( (bOkay = i >= 0) == true )
					{
						a	= i * M_PI_360 / 8.;
						s	= atan((m_pSurface->asDouble(x, y) - m_pSurface->asDouble(ix, iy)) / Get_Length(i));
					}
				}
			}

			//---------------------------------------------
			if( bOkay )
			{
				m_Gradient[0].Set_Value(x, y,     s );
				m_Gradient[1].Set_Value(x, y, sin(a));
				m_Gradient[2].Set_Value(x, y, cos(a));
			}
			else
			{
				m_Gradient[0].Set_NoData(x, y);
				m_Gradient[1].Set_NoData(x, y);
				m_Gradient[2].Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSaLEM::Set_Allochthone(void)
{
	CSG_Grid	*pAllochthone	= Parameters("ALLOCHTHONE")->asGrid  ();
	double  	  Allochthone	= Parameters("ALLOCHTHONE")->asDouble();

	if( pAllochthone || Allochthone > 0.0 )
	{
		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				double	d	= pAllochthone && !pAllochthone->is_NoData(x, y) ? pAllochthone->asDouble(x, y) : Allochthone;

				if( d > 0.0 )
				{
					d	*= m_dTime;

					m_pRegolith->Add_Value(x, y, d);
					m_pSurface ->Add_Value(x, y, d);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSaLEM::Set_Weathering(void)
{
	m_Climate.Set_Year(m_Time);	// Years BP

	m_Bedrock.Set_Weathering(m_dTime, m_Climate, *m_pSurface, m_Gradient[0], *m_pRegolith);

	m_Tracers.Add_Tracers(m_Time, *m_pSurface, *m_pRegolith, m_Bedrock);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Reimplements GOLEM::DiffuseExplIrreg
//---------------------------------------------------------
bool CSaLEM::Set_Diffusive(void)
{
	double	k		= Parameters("DIFFUSIVE_KD")->asDouble() * m_dTime / Get_System()->Get_Cellarea();	// Diffusivity coefficient Kd [m^2/a]
	int	y, iStep	= Parameters("DIFFUSIVE_NEIGHBOURS")->asInt() == 1 ? 1 : 2;

	CSG_Grid	dHin (*Get_System());	// dHin.Assign(0.0);
	CSG_Grid	dHout(*Get_System());

	//-----------------------------------------------------
	for(y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pSurface->is_NoData(x, y) )
			{
				int		i;

				double	qs[8], dHout_Sum = 0.0, z = m_pSurface->asDouble(x, y);

				for(i=0; i<8; i+=iStep)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					qs[i]	= 0.0;

					if( m_pSurface->is_InGrid(ix, iy) )
					{
						double	dz	= z - m_pSurface->asDouble(ix, iy);

						if( dz > 0.0 )
						{
							dHout_Sum	+= (qs[i] = dz * k / Get_System()->Get_UnitLength(i));
						}
					}
					else if( m_pSurface->is_InGrid(ix = Get_xFrom(i, x), iy = Get_yFrom(i, y)) )
					{
						double	dz	= m_pSurface->asDouble(ix, iy) - z;

						if( dz > 0.0 )
						{
							dHout_Sum	+= dz * k / Get_System()->Get_UnitLength(i);
						}
					}
				}

				double	scale	= dHout_Sum > m_pRegolith->asDouble(x, y) ? m_pRegolith->asDouble(x, y) / dHout_Sum : 1.0;

				dHout.Set_Value(x, y, scale * dHout_Sum);

				for(i=0; i<8; i+=iStep)
				{
					if( qs[i] > 0.0 )
					{
						dHin.Add_Value(Get_xTo(i, x), Get_yTo(i, y), scale * qs[i]);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Tracers.Set_Tracers(m_Time, Parameters("DIFFUSIVE_KD")->asDouble() * m_dTime, *m_pSurface, m_Gradient, dHin, dHout);

	//-----------------------------------------------------
	#pragma omp parallel for private(y)
	for(y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pSurface->is_NoData(x, y) )
			{
				double	R	= m_pRegolith->asDouble(x, y);
				double	dH	= dHin.asDouble(x, y) - dHout.asDouble(x, y);

				if( R + dH < 0.0 )
				{
					dH	= -R;
				}

				m_pSurface ->Add_Value(x, y, dH);
				m_pRegolith->Add_Value(x, y, dH);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//		Relevant Original GOLEM Implementations			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// int Dt, // Time step size (years)
// mw,     // Depth at which weathering rate decays to 1/e
// kd,     // Hillslope transport (diffusivity) coefficient

//---------------------------------------------------------
// double elev    [XGRID + 2][YGRID + 2]; // cell elevations
// double chansed [XGRID + 2][YGRID + 2]; // Sediment cover thickness
// double rock    [XGRID + 2][YGRID + 2]; // Thickness of uppermost rock layer
// int    rockType[XGRID + 2][YGRID + 2]; // Lithology of uppermost rock layer
// int    area    [XGRID + 2][YGRID + 2]; // Drainage area in cells

//---------------------------------------------------------
// Lithology structure: Version 5.10 adds fields, Kmn and tc. Kmn is the mean,
// unmodified K value---that is, unmodified by changes in precip frequency.
// If precip frequency/duration varies, the "effective" K value will also
// vary. Thus, Kmn stores the mean, and K the value at any particular
// time, as computed by the CalcPrecip function. If precip doesn't vary
// in time, K and Kmn will be identical. tc is a bedrock erosion threshold,
// the units of which depend on exponents mb and nb. It represents that
// value of Q^mb S^nb at which erosion just begins to be initiated.
// struct Lithology // Lithology record
// {
//   double Kmn; // Erodability by flowing water (mean)
//   double K;   // Erodability by flowing water (time-varying)
//   double T;   // Initial thickness
//   double W;   // Bedrock weathering rate
//   double Scr; // Failure slope
//   double tc;  // Erosion threshold
// }*layer;

//---------------------------------------------------------
// This function calculates the addition to soil thickness
// during a time step, using an analytical solution to the
// equation  dC/dt = W * exp( -C ). With large soil thick-
// nesses, numerical errors in computing e raised to a
// large number can actually result in decreases in the
// computed soil thickness (and increases in the rock
// layer thickness). To avoid this error, the
// computation is only performed for soils less than 100m
// thick.
// Modified for version 5.10 to allow weathering only
// where drainage area, in cells, is less than a specified
// minimum threshold.
//---------------------------------------------------------
/*/Weather()
{
  int i, j;
  double oldsed, kwdivmw;

  for (i = 1; i <= XGRID; i++)
    for (j = 1; j <= YGRID; j++)
      {
        if (area[i][j] < permChannelA && chansed[i][j] <= 100.0)
          {
            kwdivmw = layer[rockType[i][j]].W / mw;
            oldsed = chansed[i][j];
            chansed[i][j] = mw * log(exp(chansed[i][j] / mw) + kwdivmw);
            rock[i][j] -= (chansed[i][j] - oldsed);
            if (rock[i][j] < 0.0)
              {
                rockType[i][j]++;
                rock[i][j] += layer[rockType[i][j]].T;
              }
          }
      }
}/*/

//---------------------------------------------------------
/*/void DiffuseExpl()
{
  double dhin[XGRID + 2][YGRID + 2], dhout[XGRIDP1][YGRIDP1], qsn, qss, qse, qsw, dhtot, scale, k;
  int i, j, jn, js;

  k = kd * Dt / cellarea;

  for (i = 1; i <= XGRID; i++)
    for (j = 1; j <= YGRID; j++)
      dhin[i][j] = 0.0;
  for (i = 1; i <= XGRID; i++)
    for (j = 1; j <= YGRID; j++)
      {
        scale = 1.0;
        jn = (j == YGRID) ? 1 : j + 1;
        js = (j == 1) ? YGRID : j - 1;
        qsn = (elev[i][j] > elev[i][jn]) ? k * (elev[i][j] - elev[i][jn]) : 0.0;
        qss = (elev[i][j] > elev[i][js]) ? k * (elev[i][j] - elev[i][js]) : 0.0;
        qse = (elev[i][j] > elev[i + 1][j]) ? k * (elev[i][j] - elev[i + 1][j]) : 0.0;
        qsw = (elev[i][j] > elev[i - 1][j]) ? k * (elev[i][j] - elev[i - 1][j]) : 0.0;
        if ((dhtot = qsn + qss + qse + qsw) > chansed[i][j])
          scale = chansed[i][j] / dhtot;
        dhout[i][j] = scale * dhtot;
        dhin[i][jn] += scale * qsn;
        dhin[i][js] += scale * qss;
        dhin[i + 1][j] += scale * qse;
        dhin[i - 1][j] += scale * qsw;

      }

  // Adjust elevations, sediment reservoirs and denudation rates
  for (j = 1; j <= YGRID; j++)
    {
      for (i = 1; i <= XGRID; i++)
        {
          chansed[i][j] += dhin[i][j] - dhout[i][j];
          elev[i][j] += dhin[i][j] - dhout[i][j];
        }

      // Accumulate influx at boundaries in sed_yield vbl
      sed_yield += dhin[0][j] + dhin[YGRIDP1][j];
    }
}*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
