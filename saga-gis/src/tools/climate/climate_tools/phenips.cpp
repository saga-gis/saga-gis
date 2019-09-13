
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       ips-pro                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     phenips.cpp                       //
//                                                       //
//                 Copyrights (C) 2019                   //
//                     Olaf Conrad                       //
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
#include "phenips.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPhenIps
{
public:

	enum
	{
		BROOD_STATE_BEFORE	= 0,
		BROOD_STATE_BREEDING,
		BROOD_STATE_AFTER
	};

								CPhenIps			(void);
	bool						Create				(void);

								CPhenIps			(const CPhenIps &PhenIps);
	bool						Create				(const CPhenIps &PhenIps);

	virtual ~CPhenIps(void);

	bool						Destroy				(void);

	static const CSG_String &	Get_Description		(void);

	static bool					Add_Parameters		(CSG_Parameters &Parameters);
	bool						Set_Parameters		(CSG_Parameters &Parameters);

	bool						Add_Day				(int Day, double ATmean, double ATmax, double SIrel, double DayLength = 24.);

	void						Set_ATsum_eff		(double Value)	{	m_ATsum_eff = Value;	}
	double						Get_ATsum_eff		(void)	const	{	return( m_ATsum_eff );	}

	double						Get_Parent_State	(                   bool bLimit = false)	const	{	return( bLimit && m_ATsum_eff >= m_DDminimum ? 1. : m_ATsum_eff / m_DDminimum );	}
	double						Get_Filial_State	(size_t Generation, bool bLimit = false)	const	{	return( _Get_State(Generation, 0, bLimit) );	}
	double						Get_Sister_State	(size_t Generation, bool bLimit = false)	const	{	return( _Get_State(Generation, 1, bLimit) );	}

	int							Get_Parent_Onset	(void)				const	{	return( m_YD_Onset[0                 ] );	}
	int							Get_Filial_Onset	(size_t Generation)	const	{	return( m_YD_Onset[1 + 2 * Generation] );	}
	int							Get_Sister_Onset	(size_t Generation)	const	{	return( m_YD_Onset[2 + 2 * Generation] );	}

	void						Set_Filial_State	(size_t Generation, double Value)	{	_Set_State(Generation, 0, Value);	}
	void						Set_Sister_State	(size_t Generation, double Value)	{	_Set_State(Generation, 1, Value);	}

	void						Set_Parent_Onset	(                   int Day);
	void						Set_Filial_Onset	(size_t Generation, int Day)	{	m_YD_Onset[1 + 2 * Generation] = Day;	}
	void						Set_Sister_Onset	(size_t Generation, int Day)	{	m_YD_Onset[2 + 2 * Generation] = Day;	}

	int							Get_Generations		(double minState = 0.6)	const;


private:

	//-----------------------------------------------------
	// model parameters

	double				m_DTminimum, m_DToptimum, m_FAminimum, m_DDminimum, m_DDtotal;

	//-----------------------------------------------------
	// state variables

	int					m_Brood_State, m_YD_Begin, m_YD_End, m_YD_End_Onset;

	double				m_ATsum_eff;

	CSG_Array_Int		m_YD_Onset;

	CSG_Matrix			m_BTsum_eff;


	void				Add_Filial_State	(size_t Generation, double Value)	{	_Add_State(Generation, 0, Value);	}
	void				Add_Sister_State	(size_t Generation, double Value)	{	_Add_State(Generation, 1, Value);	}

	void				_Add_State			(size_t Generation, size_t Sister, double Value)
	{
		if( Generation < NGENERATIONS )
		{
			m_BTsum_eff[Generation][Sister]	+= Value;
		}
	}

	void				_Set_State			(size_t Generation, size_t Sister, double Value)
	{
		if( Generation < NGENERATIONS )
		{
			m_BTsum_eff[Generation][Sister]	= Value;
		}
	}

	double				_Get_State			(size_t Generation, size_t Sister, bool bLimit)	const
	{
		double	BTsum_eff	= Generation < NGENERATIONS ? m_BTsum_eff[Generation][Sister] : 0.;

		return( BTsum_eff < 0. ? (bLimit ? 0. : -1.) : (bLimit && BTsum_eff >= m_DDtotal ? 1. : BTsum_eff / m_DDtotal) );
	}


	//---------------------------------------------------------
	static double		Get_BTmean			(double ATmean, double SIrel)	// Formula (A.3) (Baier et al. 2007)
	{
		return( -0.173 + 0.0008518 * SIrel + 1.054 * ATmean );
	}

	static double		Get_BTmax			(double ATmax, double SIrel)	// Formula (A.4) (Baier et al. 2007)
	{
		return( 1.656 + 0.002955 * SIrel + 0.534 * ATmax + 0.01884 * ATmax*ATmax );
	}

	static double		Get_BTsum_Diff		(double BTmax)					// Formula (A.5) (Baier et al. 2007)
	{
		double	BTsum_Diff	= (-310.667 + 9.603 * BTmax) / 24.;

		return( BTsum_Diff > 0. ? BTsum_Diff : 0. );
	}

	//---------------------------------------------------------
	bool				Get_Onset			(double ATmax)					// Formula (A.8) (Baier et al. 2007)
	{
		if( ATmax > m_DTminimum )	// maximum air temperature above development minimum (= effective maximum air temperature)
		{
			m_ATsum_eff	+= ATmax - m_DTminimum;	// add to effective maximum air temperature sum
		}

		return( ATmax > m_FAminimum && m_ATsum_eff >= m_DDminimum );
	}
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPhenIps::CPhenIps(void)
{
	Create();
}

bool CPhenIps::Create(void)
{
	m_DToptimum		=  30.4;
	m_DTminimum		=   8.3;
	m_FAminimum		=  16.5;
	m_DDminimum		= 140.0;
	m_DDtotal		= 557.0;

	m_YD_Begin      =  61;
	m_YD_End_Onset  = 212;
	m_YD_End        = 273;

	m_Brood_State	= BROOD_STATE_BEFORE;
	m_YD_Onset.Create(1 + 2 * NGENERATIONS);
	m_YD_Onset		= 0;

	m_ATsum_eff		= 0.;
	m_BTsum_eff.Create(2, NGENERATIONS);
	m_BTsum_eff		= -1.;

	return( true );
}

//---------------------------------------------------------
CPhenIps::CPhenIps(const CPhenIps &PhenIps)
{
	Create(PhenIps);
}

bool CPhenIps::Create(const CPhenIps &PhenIps)
{
	m_DToptimum		= PhenIps.m_DToptimum;
	m_DTminimum		= PhenIps.m_DTminimum;
	m_FAminimum		= PhenIps.m_FAminimum;
	m_DDminimum		= PhenIps.m_DDminimum;
	m_DDtotal		= PhenIps.m_DDtotal;

	m_YD_Begin		= PhenIps.m_YD_Begin;
	m_YD_End_Onset	= PhenIps.m_YD_End_Onset;
	m_YD_End		= PhenIps.m_YD_End;

	m_Brood_State	= PhenIps.m_Brood_State;
	m_YD_Onset.Create(PhenIps.m_YD_Onset);

	m_ATsum_eff		= PhenIps.m_ATsum_eff;
	m_BTsum_eff.Create(PhenIps.m_BTsum_eff);

	return( true );
}

//---------------------------------------------------------
CPhenIps::~CPhenIps(void)
{
	Destroy();
}

bool CPhenIps::Destroy(void)
{
	m_Brood_State	= BROOD_STATE_BEFORE;
	m_YD_Onset		= 0;	// invalid day
	m_ATsum_eff		= 0.;
	m_BTsum_eff		= -1.;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const CSG_String & CPhenIps::Get_Description(void)
{
	static CSG_String	Description(_TL("A comprehensive phenology model of Ips typographus (L.) (Col., Scolytinae) as a tool for hazard rating of bark beetle infestation."));

	return( Description );
}

//---------------------------------------------------------
#define ADD_REFERENCES	{\
	Add_Reference("Baier P., Pennerstorfer J. and Schopf A.", "2007",\
		"PHENIPS — A comprehensive phenology model of Ips typographus (L.) (Col., Scolytinae) as a tool for hazard rating of bark beetle infestation",\
		"Forest Ecology and Management, 249(3): 171-186.",\
		SG_T("https://www.sciencedirect.com/science/article/pii/S0378112707004057"), SG_T("ScienceDirect")\
	);\
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPhenIps::Add_Parameters(CSG_Parameters &Parameters)
{
	Parameters.Add_Double("", "DToptimum", _TL("Developmental Optimum Temperature"      ), _TL("Degree Celsius"),  30.4);
	Parameters.Add_Double("", "DTminimum", _TL("Developmental Minimum Temperature"      ), _TL("Degree Celsius"),   8.3);
	Parameters.Add_Double("", "FAminimum", _TL("Minimum Temperature for Flight Activity"), _TL("Degree Celsius"),  16.5);
	Parameters.Add_Double("", "DDminimum", _TL("Minimum Thermal Sum for Infestation"    ), _TL("Degree Days"), 140, 0., true);
	Parameters.Add_Double("", "DDtotal"  , _TL("Thermal Sum for Total Development"      ), _TL("Degree Days"), 557, 0., true);

	//-----------------------------------------------------
	CSG_DateTime Date; Date.Set_Month(CSG_DateTime::Jan); Date.Set_Day(1);

	Date.Set(01, CSG_DateTime::Apr); Parameters.Add_Date("", "YD_Begin"    , _TL("Begin of Parental Development"), _TL(""), Date.Get_JDN());
	Date.Set(31, CSG_DateTime::Aug); Parameters.Add_Date("", "YD_End_Onset", _TL("End of Breeding"              ), _TL(""), Date.Get_JDN());
	Date.Set(31, CSG_DateTime::Oct); Parameters.Add_Date("", "YD_End"      , _TL("End of Development"           ), _TL(""), Date.Get_JDN());

	return( true );
}

//---------------------------------------------------------
bool CPhenIps::Set_Parameters(CSG_Parameters &Parameters)
{
	m_DToptimum	= Parameters("DToptimum")->asDouble();
	m_DTminimum	= Parameters("DTminimum")->asDouble();
	m_FAminimum	= Parameters("FAminimum")->asDouble();
	m_DDminimum	= Parameters("DDminimum")->asDouble();
	m_DDtotal	= Parameters("DDtotal"  )->asDouble();

	//-----------------------------------------------------
	m_YD_Begin		= Parameters("YD_Begin"    )->asDate()->Get_Date().Get_DayOfYear();
	m_YD_End_Onset	= Parameters("YD_End_Onset")->asDate()->Get_Date().Get_DayOfYear();
	m_YD_End		= Parameters("YD_End"      )->asDate()->Get_Date().Get_DayOfYear();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPhenIps::Set_Parent_Onset(int DayOfYear)
{
	if( DayOfYear < 1 )
	{
		m_YD_Onset[0]	= 0;
		m_Brood_State	= BROOD_STATE_BEFORE;

		for(size_t i=0; i<NGENERATIONS; i++)
		{
			Set_Filial_State(i, -1.);
			Set_Sister_State(i, -1.);
		}
	}
	else
	{
		m_YD_Onset[0]	= DayOfYear;
		m_Brood_State	= BROOD_STATE_BREEDING;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPhenIps::Add_Day(int Day, double ATmean, double ATmax, double SIrel, double DayLength)
{
	if( Day < m_YD_Begin )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Day >= m_YD_End_Onset )
	{
		m_Brood_State	= BROOD_STATE_AFTER;
	}

	//-----------------------------------------------------
	if( m_Brood_State == BROOD_STATE_BEFORE && Get_Onset(ATmax) )	// Compute the day of year of the onset of swarming of 'Ips typographus'.
	{
		Set_Parent_Onset(Day);
	}

	//-----------------------------------------------------
	if( m_Brood_State > BROOD_STATE_BEFORE )
	{
		double	BTeff	= Get_BTmean(ATmean, SIrel) - m_DTminimum;
		double	BTmax	= Get_BTmax (ATmax , SIrel);

		if( BTmax > m_DToptimum )
		{
			BTeff	-= Get_BTsum_Diff(BTmax);
		}

		if( BTeff < 0. )
		{
			BTeff	= 0.;
		}

		bool	bCanHatch	= ATmax > m_FAminimum && DayLength >= 14.5;

		//-------------------------------------------------
		for(size_t i=0; i<NGENERATIONS; i++)
		{
			if( Get_Filial_State(i) < 0. && bCanHatch && (i < 1 || Get_Filial_State(i - 1) > 1.) )
			{
				Set_Filial_State(i, BTeff);
			}
			else if( Get_Filial_State(i) >= 0. )
			{
				Add_Filial_State(i, BTeff);

				if( !Get_Filial_Onset(i) && Get_Filial_State(i) >= 1. )
				{
					Set_Filial_Onset(i, Day);
				}

				if( Get_Sister_State(i) < 0. && bCanHatch && Get_Filial_State(i) > 0.5 )
				{
					Set_Sister_State(i, BTeff);
				}
				else if( Get_Sister_State(i) >= 0. )
				{
					Add_Sister_State(i, BTeff);

					if( !Get_Sister_Onset(i) && Get_Sister_State(i) >= 1. )
					{
						Set_Sister_Onset(i, Day);
					}
				}
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
int CPhenIps::Get_Generations(double minState)	const
{
	int	n	= 0;

	for(int i=0; i<NGENERATIONS; i++)
	{
		if( Get_Filial_State(i) >= minState )	{	n++;	}
		if( Get_Sister_State(i) >= minState )	{	n++;	}
	}

	return( n );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPhenIps_Table::CPhenIps_Table(void)
{
	Set_Name		(CSG_String::Format("PhenIps (%s)", _TL("Table")));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(CPhenIps::Get_Description());

	ADD_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Table("",   "CLIMATE", _TL("Climate Data"), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Table_Field("CLIMATE", "ATmean", _TL("Mean Temperature"   ), _TL("Degree Celsius"));
	Parameters.Add_Table_Field("CLIMATE", "ATmax" , _TL("Maximum Temperature"), _TL("Degree Celsius"));
	Parameters.Add_Table_Field("CLIMATE", "SIrel" , _TL("Solar Irradiation"  ), _TL("Wh/m^2"        ));

	Parameters.Add_Table("", "PHENOLOGY", _TL("Phenology"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Table("", "SUMMARY"  , _TL("Summary"  ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Double("",
		"LATITUDE"	, _TL("Latitude"),
		_TL(""),
		50., -90., true, 90., true
	);

	Parameters.Add_Bool("",
		"LIMIT"		, _TL("Limit" ),
		_TL("Limits state output to a maximum value of 1."),
		true
	);

	//-----------------------------------------------------
	CPhenIps::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPhenIps_Table::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	&Climate	= *Parameters("CLIMATE")->asTable();

	int	ATmean	= Parameters("ATmean")->asInt();
	int	ATmax	= Parameters("ATmax" )->asInt();
	int	SIrel	= Parameters("SIrel" )->asInt();

	if( Climate.Get_Count() < 365 )
	{
		Error_Set(_TL("There have to be at least 365 climate records, one for each day of the year."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pPhenology	= Parameters("PHENOLOGY")->asTable();

	pPhenology->Destroy();
	pPhenology->Set_Name(_TL("Phenology"));
	pPhenology->Add_Field("YD"       , SG_DATATYPE_Short );
	pPhenology->Add_Field("DAYLENGTH", SG_DATATYPE_Double);
	pPhenology->Add_Field("PARENTAL" , SG_DATATYPE_Double);
	pPhenology->Add_Field("FILIAL1"  , SG_DATATYPE_Double);
	pPhenology->Add_Field("SISTER1"  , SG_DATATYPE_Double);
	pPhenology->Add_Field("FILIAL2"  , SG_DATATYPE_Double);
	pPhenology->Add_Field("SISTER2"  , SG_DATATYPE_Double);
	pPhenology->Add_Field("FILIAL3"  , SG_DATATYPE_Double);
	pPhenology->Add_Field("SISTER3"  , SG_DATATYPE_Double);

	//-----------------------------------------------------
	bool	bLimit		= Parameters("LIMIT"   )->asBool  ();
	double	Latitude	= Parameters("LATITUDE")->asDouble();

	CPhenIps	PhenIps;	PhenIps.Set_Parameters(Parameters);

	//-----------------------------------------------------
	for(int Day=0; Day<365 && Set_Progress(Day, 365); Day++)
	{
		double	DayLength	= SG_Get_Day_Length(1 + Day, Latitude);

		PhenIps.Add_Day(1 + Day,
			Climate[Day].asDouble(ATmean),
			Climate[Day].asDouble(ATmax ),
			Climate[Day].asDouble(SIrel ), DayLength
		);

		CSG_Table_Record	*pRecord	= pPhenology->Add_Record();

		pRecord->Set_Value(0, Day + 1);
		pRecord->Set_Value(1, DayLength);
		pRecord->Set_Value(2, PhenIps.Get_Parent_State(   bLimit));
		pRecord->Set_Value(3, PhenIps.Get_Filial_State(0, bLimit));
		pRecord->Set_Value(4, PhenIps.Get_Sister_State(0, bLimit));
		pRecord->Set_Value(5, PhenIps.Get_Filial_State(1, bLimit));
		pRecord->Set_Value(6, PhenIps.Get_Sister_State(1, bLimit));
		pRecord->Set_Value(7, PhenIps.Get_Filial_State(2, bLimit));
		pRecord->Set_Value(8, PhenIps.Get_Sister_State(2, bLimit));
	}

	//-----------------------------------------------------
	#define ADD_SUMMARY(name, value)	{ CSG_Table_Record *pR = pSummary->Add_Record(); pR->Set_Value(0, name); pR->Set_Value(1, value); }

	CSG_Table	*pSummary	= Parameters("SUMMARY")->asTable();

	pSummary->Destroy();
	pSummary->Set_Name(_TL("Summary"));
	pSummary->Add_Field("VARIABLE", SG_DATATYPE_String);
	pSummary->Add_Field("VALUE"   , SG_DATATYPE_Double);

	ADD_SUMMARY(_TL("Day of Infestion"), PhenIps.Get_Parent_Onset());

	int	i, n	= 0;

	for(i=0; i<NGENERATIONS; i++)
	{
		if( PhenIps.Get_Filial_State(i) > 0.6 )
		{
			n++;
		}
	}

	ADD_SUMMARY(_TL("1st Filial State"), PhenIps.Get_Filial_State(0, false));
	ADD_SUMMARY(_TL("1st Sister State"), PhenIps.Get_Sister_State(0, false));
	ADD_SUMMARY(_TL("2nd Filial State"), PhenIps.Get_Filial_State(1, false));
	ADD_SUMMARY(_TL("2nd Sister State"), PhenIps.Get_Sister_State(1, false));
	ADD_SUMMARY(_TL("3rd Filial State"), PhenIps.Get_Filial_State(2, false));
	ADD_SUMMARY(_TL("3rd Sister State"), PhenIps.Get_Sister_State(2, false));

	ADD_SUMMARY(_TL("1st Filial Grown"), PhenIps.Get_Filial_Onset(0));
	ADD_SUMMARY(_TL("1st Sister Grown"), PhenIps.Get_Sister_Onset(0));
	ADD_SUMMARY(_TL("2nd Filial Grown"), PhenIps.Get_Filial_Onset(1));
	ADD_SUMMARY(_TL("2nd Sister Grown"), PhenIps.Get_Sister_Onset(1));
	ADD_SUMMARY(_TL("3rd Filial Grown"), PhenIps.Get_Filial_Onset(2));
	ADD_SUMMARY(_TL("3rd Sister Grown"), PhenIps.Get_Sister_Onset(2));

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPhenIps_Grids::CPhenIps_Grids(void)
{
	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(CPhenIps::Get_Description());

	ADD_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Grid_List("", "ATmean", _TL("Mean Temperature"   ), _TL("Degree Celsius"), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "ATmax" , _TL("Maximum Temperature"), _TL("Degree Celsius"), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "SIrel" , _TL("Solar Irradiation"  ), _TL("Wh/m^2"        ), PARAMETER_INPUT);

	Parameters.Add_Grid("", "GENERATIONS", _TL("Potential Number of Generations"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
	Parameters.Add_Grid("", "ONSET"      , _TL("Onset Day of Infestation"       ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);

	for(int i=0; i<NGENERATIONS; i++)
	{
		Parameters.Add_Grid("", CSG_String::Format("ONSET_FILIAL_%d", i + 1), CSG_String::Format("%s, %d. %s", _TL("Onset Day"), i + 1, _TL("Filial Generation")), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
		Parameters.Add_Grid("", CSG_String::Format("ONSET_SISTER_%d", i + 1), CSG_String::Format("%s, %d. %s", _TL("Onset Day"), i + 1, _TL("Sister Generation")), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
	}

	for(int i=0; i<NGENERATIONS; i++)
	{
		Parameters.Add_Grid("", CSG_String::Format("STATE_FILIAL_%d", i + 1), CSG_String::Format("%s, %d. %s", _TL("State"    ), i + 1, _TL("Filial Generation")), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Float);
		Parameters.Add_Grid("", CSG_String::Format("STATE_SISTER_%d", i + 1), CSG_String::Format("%s, %d. %s", _TL("State"    ), i + 1, _TL("Sister Generation")), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Float);
	}

	Parameters.Add_Grid  ("", "LAT_GRID" , _TL("Latitude"), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Double("", "LAT_CONST", _TL("Latitude"), _TL(""), 50., -90., true, 90., true);

	//-----------------------------------------------------
	CPhenIps::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPhenIps_Grids::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPhenIps_Grids::Initialize(bool bReset)
{
	m_pATmean = Parameters("ATmean")->asGridList();
	m_pATmax  = Parameters("ATmax" )->asGridList();
	m_pSIrel  = Parameters("SIrel" )->asGridList();

	if( m_pATmean->Get_Grid_Count() < 1
	||  m_pATmax ->Get_Grid_Count() < 1
	||  m_pSIrel ->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("missing input data"));

		return( false );
	}

	//-----------------------------------------------------
	#define GRID_RESET(g, b)	if( bReset && g ) { DataObject_Set_Colors(g, 11, SG_COLORS_RAINBOW, b); g->Set_NoData_Value_Range(-2., 0.); }

	m_pOnset	= Parameters("ONSET" )->asGrid(); GRID_RESET(m_pOnset, true);

	for(int i=0; i<NGENERATIONS; i++)
	{
		m_pOnsets[i][0]	= Parameters(CSG_String::Format("ONSET_FILIAL_%d", i + 1))->asGrid(); GRID_RESET(m_pOnsets[i][0],  true);
		m_pStates[i][0]	= Parameters(CSG_String::Format("STATE_FILIAL_%d", i + 1))->asGrid(); GRID_RESET(m_pStates[i][0], false);

		m_pOnsets[i][1]	= Parameters(CSG_String::Format("ONSET_SISTER_%d", i + 1))->asGrid(); GRID_RESET(m_pOnsets[i][1],  true);
		m_pStates[i][1]	= Parameters(CSG_String::Format("STATE_SISTER_%d", i + 1))->asGrid(); GRID_RESET(m_pStates[i][1], false);
	}

	//-----------------------------------------------------
	m_pGenerations	= Parameters("GENERATIONS")->asGrid();

	CSG_Parameter	*pLUT	= CSG_Tool::DataObject_Get_Parameter(m_pGenerations, "LUT");

	if( bReset && pLUT && pLUT->asTable() )
	{
		#define ADD_LUT_CLASS(id, color, name)	{ CSG_Table_Record *pClass = pLUT->asTable()->Add_Record();\
			pClass->Set_Value(0, color);\
			pClass->Set_Value(1, name );\
			pClass->Set_Value(2, ""   );\
			pClass->Set_Value(3, id   );\
			pClass->Set_Value(4, id   );\
		}

		pLUT->asTable()->Del_Records();

		CSG_Colors	Colors(2 * NGENERATIONS, SG_COLORS_YELLOW_RED);

		ADD_LUT_CLASS(0, SG_GET_RGB(000, 127, 000), _TL("no generation"));

		for(int i=1, j=1; i<=NGENERATIONS; i++, j+=2)
		{
			ADD_LUT_CLASS(j + 0, Colors[j - 1], CSG_String::Format("%s %d"     , _TL("generation"), i                     ));
			ADD_LUT_CLASS(j + 1, Colors[j - 0], CSG_String::Format("%s %d + %s", _TL("generation"), i, _TL("sister brood")));
		}

		CSG_Tool::DataObject_Set_Parameter(m_pGenerations, pLUT);
		CSG_Tool::DataObject_Set_Parameter(m_pGenerations, "COLORS_TYPE", 1);	// Classified
	}

	//-----------------------------------------------------
	m_Lat_const	= Parameters("LAT_CONST")->asDouble();
	m_pLat_Grid	= Parameters("LAT_GRID" )->asGrid  ();

	CSG_Projection	Projection;

	if( !m_pLat_Grid && Get_Projection(Projection) && Projection.is_Okay() )
	{
		bool	bResult;

		CSG_Grid	Lon_Grid(Get_System()); Lon_Grid.Get_Projection().Create(Projection);

		m_Lat_Grid.Create(Get_System());

		SG_RUN_TOOL(bResult, "pj_proj4", 17,	// geographic coordinate grids
			   SG_TOOL_PARAMETER_SET("GRID", &  Lon_Grid)
			&& SG_TOOL_PARAMETER_SET("LON" , &  Lon_Grid)
			&& SG_TOOL_PARAMETER_SET("LAT" , &m_Lat_Grid)
		);

		if( bResult )
		{
			m_pLat_Grid	= &m_Lat_Grid;
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CPhenIps_Grids::Finalize(void)
{
	m_Lat_Grid.Destroy();

	return( true );
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CPhenIps_Grids::Get_DayLength(int x, int y, int Day)
{
	return( SG_Get_Day_Length(Day, m_pLat_Grid ? m_pLat_Grid->asDouble(x, y) : m_Lat_const) );
}

//---------------------------------------------------------
inline bool CPhenIps_Grids::Set_NoData(int x, int y)
{
	if( m_pOnset ) m_pOnset->Set_NoData(x, y);

	for(int i=0; i<NGENERATIONS; i++)
	{
		if( m_pOnsets[i][0] ) m_pOnsets[i][0]->Set_NoData(x, y);
		if( m_pOnsets[i][1] ) m_pOnsets[i][1]->Set_NoData(x, y);
		if( m_pStates[i][0] ) m_pStates[i][0]->Set_NoData(x, y);
		if( m_pStates[i][1] ) m_pStates[i][1]->Set_NoData(x, y);
	}

	if( m_pGenerations ) m_pGenerations->Set_NoData(x, y);

	return( true );
}

//---------------------------------------------------------
inline bool CPhenIps_Grids::Set_Values(int x, int y, const CPhenIps &PhenIps)
{
	if( m_pOnset && m_pOnset->asInt(x, y) < 1 ) m_pOnset->Set_Value(x, y, PhenIps.Get_Parent_Onset());

	for(int i=0; i<NGENERATIONS; i++)
	{
		if( m_pOnsets[i][0] && m_pOnsets[i][0]->asInt(x, y) < 1 ) m_pOnsets[i][0]->Set_Value(x, y, PhenIps.Get_Filial_Onset(i));
		if( m_pOnsets[i][1] && m_pOnsets[i][1]->asInt(x, y) < 1 ) m_pOnsets[i][1]->Set_Value(x, y, PhenIps.Get_Sister_Onset(i));

		if( m_pStates[i][0] ) m_pStates[i][0]->Set_Value(x, y, PhenIps.Get_Filial_State(i));
		if( m_pStates[i][1] ) m_pStates[i][1]->Set_Value(x, y, PhenIps.Get_Sister_State(i));
	}

	if( m_pGenerations ) m_pGenerations->Set_Value(x, y, PhenIps.Get_Generations(0.6));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPhenIps_Grids_Annual::CPhenIps_Grids_Annual(void)
{
	Set_Name		(CSG_String::Format("PhenIps (%s, %s)", _TL("Grids"), _TL("Annual")));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPhenIps_Grids_Annual::On_Execute(void)
{
	if( !Initialize(true) )
	{
		Finalize();

		return( false );
	}

	if( ((m_pATmean->Get_Grid_Count() ==  12 && m_pATmax->Get_Grid_Count() ==  12 && m_pSIrel->Get_Grid_Count() ==  12)
	||   (m_pATmean->Get_Grid_Count() >= 365 && m_pATmax->Get_Grid_Count() >= 365 && m_pSIrel->Get_Grid_Count() >= 365)) == false )
	{
		Error_Set(_TL("There has to be either one input grid for each month (12) or for each day (365) of a year."));

		return( false );
	}

	//-----------------------------------------------------
	CPhenIps	_PhenIps;

	_PhenIps.Set_Parameters(Parameters);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Vector	ATmean, ATmax, SIrel;

			if( !Get_Daily(x, y, m_pATmean, ATmean)
			||  !Get_Daily(x, y, m_pATmax , ATmax )
			||  !Get_Daily(x, y, m_pSIrel , SIrel ) )
			{
				Set_NoData(x, y);

				continue;
			}

			//---------------------------------------------
			CPhenIps	PhenIps(_PhenIps);

			for(int Day=0, DayOfYear=1; Day<365; Day++, DayOfYear++)
			{
				PhenIps.Add_Day(DayOfYear, ATmean[Day], ATmax[Day], SIrel[Day], Get_DayLength(x, y, DayOfYear));
			}

			Set_Values(x, y, PhenIps);
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
bool CPhenIps_Grids_Annual::Get_Daily(int x, int y, CSG_Parameter_Grid_List *pValues, CSG_Vector &Values)
{
	if( pValues->Get_Grid_Count() == 12 )
	{
		CSG_Spline	Spline;

		for(int iMonth=0; iMonth<12; iMonth++)
		{
			if( pValues->Get_Grid(iMonth)->is_NoData(x, y) )
			{
				return( false );
			}

			Spline.Add(SG_Get_Day_MidOfMonth(iMonth), pValues->Get_Grid(iMonth)->asDouble(x, y));
		}

		Spline.Add(SG_Get_Day_MidOfMonth(10 - 12), pValues->Get_Grid(10)->asDouble(x, y));
		Spline.Add(SG_Get_Day_MidOfMonth(11 - 12), pValues->Get_Grid(11)->asDouble(x, y));
		Spline.Add(SG_Get_Day_MidOfMonth( 0 + 12), pValues->Get_Grid( 0)->asDouble(x, y));
		Spline.Add(SG_Get_Day_MidOfMonth( 1 + 12), pValues->Get_Grid( 1)->asDouble(x, y));

		Values.Create(365);

		for(int Day=0; Day<365; Day++)
		{
			Values[Day]	= Spline.Get_Value(Day);
		}

		return( true );
	}

	//-----------------------------------------------------
	else if( pValues->Get_Grid_Count() >= 365 )
	{
		Values.Create(pValues->Get_Grid_Count());

		for(int Day=0; Day<pValues->Get_Grid_Count(); Day++)
		{
			if( pValues->Get_Grid(Day)->is_NoData(x, y) )
			{
				return( false );
			}

			Values[Day]	= pValues->Get_Grid(Day)->asDouble(x, y);
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPhenIps_Grids_Days::CPhenIps_Grids_Days(void)
{
	Set_Name		(CSG_String::Format("PhenIps (%s, %s)", _TL("Grids"), _TL("Days")));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"ATSUM_EFF"	, _TL("Air Temperature Sum"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"RESET"		, _TL("Reset"),
		_TL(""),
		true
	);

	Parameters.Add_Date("",
		"DAY"		, _TL("Start Day"),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPhenIps_Grids_Days::On_Execute(void)
{
	if( !Initialize(Parameters("RESET")->asBool()) )
	{
		Finalize();

		return( false );
	}

	int	Day_Start	= Parameters("DAY")->asDate()->Get_Date().Get_DayOfYear();

	int	nDays	= m_pATmean->Get_Grid_Count();

	if( nDays > m_pATmax->Get_Grid_Count() ) nDays = m_pATmax->Get_Grid_Count();
	if( nDays > m_pSIrel->Get_Grid_Count() ) nDays = m_pSIrel->Get_Grid_Count();

	if( nDays < 1 )
	{
		Error_Set(_TL("there has to be a minimum of one day's input for each weather variable"));

		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pATsum_eff	= Parameters("ATSUM_EFF")->asGrid();

	if( Parameters("RESET")->asBool() )
	{
		pATsum_eff->Assign(0.);
	}

	//-----------------------------------------------------
	CPhenIps	_PhenIps;

	_PhenIps.Set_Parameters(Parameters);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bOkay	= pATsum_eff->is_NoData(x, y) == false;

			if( bOkay )
			{
				CPhenIps	PhenIps(_PhenIps);

				PhenIps.Set_ATsum_eff(pATsum_eff->asDouble(x, y));

				PhenIps.Set_Parent_Onset(m_pOnset->asInt(x, y));

				if( m_pOnset->asInt(x, y) > 0 )
				{
					for(int i=0; i<NGENERATIONS; i++)
					{
						PhenIps.Set_Filial_State(i, m_pStates[i][0]->asDouble(x, y));
						PhenIps.Set_Sister_State(i, m_pStates[i][1]->asDouble(x, y));
					}
				}

				//-----------------------------------------
				for(int Day=0, DayOfYear=Day_Start; bOkay && Day<nDays; Day++, DayOfYear++)
				{
					if( m_pATmean->Get_Grid(Day)->is_NoData(x, y)
					||  m_pATmax ->Get_Grid(Day)->is_NoData(x, y)
					||  m_pSIrel ->Get_Grid(Day)->is_NoData(x, y) )
					{
						bOkay	= false;

						continue;
					}

					PhenIps.Add_Day(DayOfYear,
						m_pATmean->Get_Grid(Day)->asDouble(x, y),
						m_pATmax ->Get_Grid(Day)->asDouble(x, y),
						m_pSIrel ->Get_Grid(Day)->asDouble(x, y), Get_DayLength(x, y, DayOfYear)
					);
				}

				//-----------------------------------------
				if( bOkay )
				{
					Set_Values(x, y, PhenIps);

					pATsum_eff->Set_Value(x, y, PhenIps.Get_ATsum_eff());
				}
			}

			if( !bOkay )
			{
				Set_NoData(x, y);

				pATsum_eff->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
