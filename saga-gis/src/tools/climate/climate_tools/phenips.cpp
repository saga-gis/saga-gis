
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
double SG_Get_Gaussian(double m, double s, double x)
{
	s = 2. * s*s; x -= m;

	return( 1. / sqrt(M_PI * s) * exp(-x*x / s) );
}


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

	bool						Add_Day				(int DayOfYear, double ATmean, double ATmax, double SIrel, double DayLength = 24.);

	double						Get_Parent_ATsum	(                   bool bNorm = false)	const	{	return( !bNorm ? m_ATsum_eff : m_ATsum_eff / m_DDminimum );	}
	double						Get_Filial_BTsum	(size_t Generation, bool bNorm = false)	const	{	return( _Get_BTsum(Generation, 0, bNorm) );	}
	double						Get_Sister_BTsum	(size_t Generation, bool bNorm = false)	const	{	return( _Get_BTsum(Generation, 1, bNorm) );	}

	void						Set_Parent_ATsum	(                   double ATsum, bool bNorm = false)	{	m_ATsum_eff = !bNorm ? ATsum : ATsum * m_DDminimum;	}
	void						Set_Filial_BTsum	(size_t Generation, double BTsum, bool bNorm = false)	{	_Set_BTsum(Generation, 0, BTsum, bNorm);	}
	void						Set_Sister_BTsum	(size_t Generation, double BTsum, bool bNorm = false)	{	_Set_BTsum(Generation, 1, BTsum, bNorm);	}

	int							Get_Parent_Onset	(void)				const	{	return( m_YD_Onset[0                 ] );	}
	int							Get_Filial_Onset	(size_t Generation)	const	{	return( m_YD_Onset[1 + 2 * Generation] );	}
	int							Get_Sister_Onset	(size_t Generation)	const	{	return( m_YD_Onset[2 + 2 * Generation] );	}

	void						Set_Parent_Onset	(                   int Day);
	void						Set_Filial_Onset	(size_t Generation, int Day)	{	m_YD_Onset[1 + 2 * Generation] = Day;	}
	void						Set_Sister_Onset	(size_t Generation, int Day)	{	m_YD_Onset[2 + 2 * Generation] = Day;	}

	int							Get_Generations		(double minState = 0.6)	const;

	double						Get_Risk			(void)	const
	{
		double	risk	= 0.;

		for(size_t i=0; m_YD>0 && m_YD_Onset[i]>0 && i<1+2*MAX_GENERATIONS; i++)
		{
			double	r	= m_YD - m_YD_Onset[i];	// days after onset

			if( r >= 0. )
			{
				if( r < m_Risk_DayMax )
				{
					r	+= 1.;
					r	= 1.5 * r / (m_Risk_DayMax + 1.) - 0.5 * pow(r, 3.) / pow(m_Risk_DayMax + 1., 3.);
				}
				else
				{
					r	-= m_Risk_DayMax;
					r	= exp(-r*r / (2. * m_Risk_Decay*m_Risk_Decay));
				}

				if( risk < r )
				{
					risk	= r;
				}
			}
		}

		return( risk );
	}


private:

	//-----------------------------------------------------
	// model parameters

	double				m_DTminimum, m_DToptimum, m_FAminimum, m_DDminimum, m_DDtotal, m_Risk_DayMax, m_Risk_Decay;

	//-----------------------------------------------------
	// state variables

	int					m_YD, m_YD_Begin, m_YD_End, m_YD_End_Onset, m_YD_Onset[1 + 2 * MAX_GENERATIONS], m_Brood_State;

	double				m_ATsum_eff, m_BTsum_eff[MAX_GENERATIONS][2];


	void				Add_Filial_BTsum	(size_t Generation, double BTsum)	{	m_BTsum_eff[Generation][0]	+= BTsum;	}
	void				Add_Sister_BTsum	(size_t Generation, double BTsum)	{	m_BTsum_eff[Generation][1]	+= BTsum;	}

	void				_Set_BTsum			(size_t Generation, size_t Sister, double BTsum, bool bNorm)
	{
		if( Generation < MAX_GENERATIONS )
		{
			m_BTsum_eff[Generation][Sister]	= !bNorm ? BTsum : BTsum * m_DDtotal;
		}
	}

	double				_Get_BTsum			(size_t Generation, size_t Sister, bool bNorm)	const
	{
		double	BTsum	= Generation < MAX_GENERATIONS ? m_BTsum_eff[Generation][Sister] : 0.;

		return( !bNorm ? BTsum : BTsum / m_DDtotal );
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
		m_ATsum_eff	+= ATmax - m_DTminimum;	// add to effective maximum air temperature sum

		return( ATmax > m_FAminimum && m_ATsum_eff >= m_DDminimum );	// maximum air temperature above development minimum (= effective maximum air temperature)
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

	m_Risk_DayMax	=   5.0;
	m_Risk_Decay	=  10.0;

	m_YD			=   0;
	m_YD_Begin      =  61;
	m_YD_End_Onset  = 212;
	m_YD_End        = 273;

	m_Brood_State	= BROOD_STATE_BEFORE;

	m_ATsum_eff		= 0.;

	for(size_t i=0; i<MAX_GENERATIONS; i++)
	{
		m_BTsum_eff[i][0]	= -1.;
		m_BTsum_eff[i][1]	= -1.;
	}

	memset(m_YD_Onset, 0, (1 + 2 * MAX_GENERATIONS) * sizeof(int));

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

	m_Risk_DayMax	= PhenIps.m_Risk_DayMax;
	m_Risk_Decay	= PhenIps.m_Risk_Decay;

	m_YD			= PhenIps.m_YD;
	m_YD_Begin		= PhenIps.m_YD_Begin;
	m_YD_End_Onset	= PhenIps.m_YD_End_Onset;
	m_YD_End		= PhenIps.m_YD_End;

	m_Brood_State	= PhenIps.m_Brood_State;

	m_ATsum_eff		= PhenIps.m_ATsum_eff;

	for(size_t i=0; i<MAX_GENERATIONS; i++)
	{
		m_BTsum_eff[i][0]	= PhenIps.m_BTsum_eff[i][0];
		m_BTsum_eff[i][1]	= PhenIps.m_BTsum_eff[i][1];
	}

	memcpy(m_YD_Onset, PhenIps.m_YD_Onset, (1 + 2 * MAX_GENERATIONS) * sizeof(int));

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
	m_ATsum_eff		= 0.;

	for(size_t i=0; i<MAX_GENERATIONS; i++)
	{
		m_BTsum_eff[i][0]	= -1.;
		m_BTsum_eff[i][1]	= -1.;
	}

	memset(m_YD_Onset, 0, (1 + 2 * MAX_GENERATIONS) * sizeof(int));

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
	Parameters.Add_Double("", "DToptimum" , _TL("Developmental Optimum Temperature"      ), _TL("Degree Celsius"),  30.4);
	Parameters.Add_Double("", "DTminimum" , _TL("Developmental Minimum Temperature"      ), _TL("Degree Celsius"),   8.3);
	Parameters.Add_Double("", "FAminimum" , _TL("Minimum Temperature for Flight Activity"), _TL("Degree Celsius"),  16.5);
	Parameters.Add_Double("", "DDminimum" , _TL("Minimum Thermal Sum for Infestation"    ), _TL("Degree Days"   ), 140., 0., true);
	Parameters.Add_Double("", "DDtotal"   , _TL("Thermal Sum for Total Development"      ), _TL("Degree Days"   ), 557., 0., true);

	//-----------------------------------------------------
	Parameters.Add_Double("", "Risk_DayMax", _TL("Day of Maximum Risk after Onset"        ), _TL("Days"          ),   5., 0., true);
	Parameters.Add_Double("", "Risk_Decay" , _TL("Decay of Risk after Maximum"            ), _TL("Days"          ),  10., 1., true);

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
	m_DToptimum		= Parameters("DToptimum"   )->asDouble();
	m_DTminimum		= Parameters("DTminimum"   )->asDouble();
	m_FAminimum		= Parameters("FAminimum"   )->asDouble();
	m_DDminimum		= Parameters("DDminimum"   )->asDouble();
	m_DDtotal		= Parameters("DDtotal"     )->asDouble();

	//-----------------------------------------------------
	m_Risk_DayMax	= Parameters("Risk_DayMax" )->asDouble();
	m_Risk_Decay	= Parameters("Risk_Decay"  )->asDouble();

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
bool CPhenIps::Add_Day(int DayOfYear, double ATmean, double ATmax, double SIrel, double DayLength)
{
	m_YD	= DayOfYear;

	if( m_YD < m_YD_Begin )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( m_YD >= m_YD_End_Onset )
	{
		m_Brood_State	= BROOD_STATE_AFTER;
	}

	//-----------------------------------------------------
	if( m_Brood_State == BROOD_STATE_BEFORE && Get_Onset(ATmax) )	// Compute the day of year of the onset of swarming of 'Ips typographus'.
	{
		m_YD_Onset[0]	= DayOfYear;
		m_Brood_State	= BROOD_STATE_BREEDING;
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
		for(size_t i=0; i<MAX_GENERATIONS; i++)
		{
			if( Get_Filial_BTsum(i) < 0. && bCanHatch && (i < 1 || Get_Filial_BTsum(i - 1, true) > 1.) )
			{
				Set_Filial_BTsum(i, BTeff);
			}
			else if( Get_Filial_BTsum(i) >= 0. )
			{
				Add_Filial_BTsum(i, BTeff);

				if( !Get_Filial_Onset(i) && Get_Filial_BTsum(i, true) >= 1. )
				{
					Set_Filial_Onset(i, m_YD);
				}

				if( Get_Sister_BTsum(i) < 0. && bCanHatch && Get_Filial_BTsum(i, true) > 0.5 )
				{
					Set_Sister_BTsum(i, BTeff);
				}
				else if( Get_Sister_BTsum(i) >= 0. )
				{
					Add_Sister_BTsum(i, BTeff);

					if( !Get_Sister_Onset(i) && Get_Sister_BTsum(i, true) >= 1. )
					{
						Set_Sister_Onset(i, m_YD);
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

	for(int i=0; i<MAX_GENERATIONS; i++)
	{
		if( Get_Filial_BTsum(i, true) >= minState )	{	n++;	}
		if( Get_Sister_BTsum(i, true) >= minState )	{	n++;	}
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
	Parameters.Add_Table("",   "WEATHER", _TL("Weather Data"), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Table_Field("WEATHER", "ATmean", _TL("Mean Temperature"   ), _TL("Degree Celsius"));
	Parameters.Add_Table_Field("WEATHER", "ATmax" , _TL("Maximum Temperature"), _TL("Degree Celsius"));
	Parameters.Add_Table_Field("WEATHER", "SIrel" , _TL("Solar Irradiation"  ), _TL("Wh/m^2"        ));

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
	CSG_Table	&Weather	= *Parameters("WEATHER")->asTable();

	int	ATmean	= Parameters("ATmean")->asInt();
	int	ATmax	= Parameters("ATmax" )->asInt();
	int	SIrel	= Parameters("SIrel" )->asInt();

	if( Weather.Get_Count() < 365 )
	{
		Error_Set(_TL("There have to be at least 365 weather records, one for each day of the year."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pPhenology	= Parameters("PHENOLOGY")->asTable();

	pPhenology->Destroy();
	pPhenology->Set_Name(_TL("Phenology"));

	pPhenology->Add_Field("DayOfYear", SG_DATATYPE_Short );
	pPhenology->Add_Field("DayLength", SG_DATATYPE_Double);
	pPhenology->Add_Field("Risk"     , SG_DATATYPE_Double);
	pPhenology->Add_Field("Parental" , SG_DATATYPE_Double);

	for(size_t i=0; i<MAX_GENERATIONS; i++)
	{
		pPhenology->Add_Field(CSG_String::Format("Filial %d", i + 1), SG_DATATYPE_Double);
		pPhenology->Add_Field(CSG_String::Format("Sister %d", i + 1), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	bool	bLimit		= Parameters("LIMIT"   )->asBool  ();
	double	Latitude	= Parameters("LATITUDE")->asDouble();

	CPhenIps	PhenIps;	PhenIps.Set_Parameters(Parameters);

	//-----------------------------------------------------
	for(int iDay=0, Day=1; iDay<Weather.Get_Count() && Set_Progress(iDay, Weather.Get_Count()); iDay++, Day++)
	{
		double	DayLength	= SG_Get_Day_Length(Day, Latitude);

		PhenIps.Add_Day(Day,
			Weather[iDay].asDouble(ATmean),
			Weather[iDay].asDouble(ATmax ),
			Weather[iDay].asDouble(SIrel ), DayLength
		);

		CSG_Table_Record	*pRecord	= pPhenology->Add_Record();

		pRecord->Set_Value(0, Day);
		pRecord->Set_Value(1, DayLength);
		pRecord->Set_Value(2, PhenIps.Get_Risk());

		double	BTsum	= PhenIps.Get_Parent_ATsum(true);

		pRecord->Set_Value(3, BTsum > 1. ? 1. : BTsum);

		for(size_t i=0, j=4; i<MAX_GENERATIONS; i++, j+=2)
		{
			pRecord->Set_Value((int)j + 0, (BTsum = PhenIps.Get_Filial_BTsum(i, true)) < 0. ? 0. : BTsum > 1. ? 1. : BTsum);
			pRecord->Set_Value((int)j + 1, (BTsum = PhenIps.Get_Sister_BTsum(i, true)) < 0. ? 0. : BTsum > 1. ? 1. : BTsum);
		}
	}

	//-----------------------------------------------------
	#define ADD_SUMMARY(name, value)	{ CSG_Table_Record *pR = pSummary->Add_Record(); pR->Set_Value(0, name); pR->Set_Value(1, value); }

	CSG_Table	*pSummary	= Parameters("SUMMARY")->asTable();

	pSummary->Destroy();
	pSummary->Set_Name(_TL("Summary"));
	pSummary->Add_Field("Variable", SG_DATATYPE_String);
	pSummary->Add_Field("Value"   , SG_DATATYPE_Double);

	ADD_SUMMARY(_TL("Day of Infestion"     ), PhenIps.Get_Parent_Onset());
	ADD_SUMMARY(_TL("Potential Generations"), PhenIps.Get_Generations(0.6));

	for(size_t i=0; i<MAX_GENERATIONS; i++)
	{
		ADD_SUMMARY(CSG_String::Format("%d. %s, %s", i + 1, _TL("Filial Generation"), _TL("Onset Day")), PhenIps.Get_Filial_Onset(i      ));
		ADD_SUMMARY(CSG_String::Format("%d. %s, %s", i + 1, _TL("Filial Generation"), _TL("State"    )), PhenIps.Get_Filial_BTsum(i, true));
		ADD_SUMMARY(CSG_String::Format("%d. %s, %s", i + 1, _TL("Sister Generation"), _TL("Onset Day")), PhenIps.Get_Sister_Onset(i      ));
		ADD_SUMMARY(CSG_String::Format("%d. %s, %s", i + 1, _TL("Sister Generation"), _TL("State"    )), PhenIps.Get_Sister_BTsum(i, true));
	}

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

	for(int i=0; i<MAX_GENERATIONS; i++)
	{
		Parameters.Add_Grid("", CSG_String::Format("ONSET_FILIAL_%d", i + 1), CSG_String::Format("%s, %d. %s", _TL("Onset Day"), i + 1, _TL("Filial Generation")), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
		Parameters.Add_Grid("", CSG_String::Format("ONSET_SISTER_%d", i + 1), CSG_String::Format("%s, %d. %s", _TL("Onset Day"), i + 1, _TL("Sister Generation")), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
	}

	for(int i=0; i<MAX_GENERATIONS; i++)
	{
		Parameters.Add_Grid("", CSG_String::Format("BTSUM_FILIAL_%d", i + 1), CSG_String::Format("%s, %d. %s", _TL("State"    ), i + 1, _TL("Filial Generation")), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Float);
		Parameters.Add_Grid("", CSG_String::Format("BTSUM_SISTER_%d", i + 1), CSG_String::Format("%s, %d. %s", _TL("State"    ), i + 1, _TL("Sister Generation")), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Float);
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

	m_pOnset	= Parameters("ONSET")->asGrid(); GRID_RESET(m_pOnset, true);

	for(int i=0; i<MAX_GENERATIONS; i++)
	{
		m_pOnsets[i][0]	= Parameters(CSG_String::Format("ONSET_FILIAL_%d", i + 1))->asGrid(); GRID_RESET(m_pOnsets[i][0],  true);
		m_pBTsums[i][0]	= Parameters(CSG_String::Format("BTSUM_FILIAL_%d", i + 1))->asGrid(); GRID_RESET(m_pBTsums[i][0], false);

		m_pOnsets[i][1]	= Parameters(CSG_String::Format("ONSET_SISTER_%d", i + 1))->asGrid(); GRID_RESET(m_pOnsets[i][1],  true);
		m_pBTsums[i][1]	= Parameters(CSG_String::Format("BTSUM_SISTER_%d", i + 1))->asGrid(); GRID_RESET(m_pBTsums[i][1], false);
	}

	//-----------------------------------------------------
	m_pGenerations	= Parameters("GENERATIONS")->asGrid();

	CSG_Parameter	*pLUT	= CSG_Tool::DataObject_Get_Parameter(m_pGenerations, "LUT");

	if( bReset && pLUT && pLUT->asTable() )
	{
		m_pGenerations->Assign(0.);

		#define ADD_LUT_CLASS(id, color, name)	{ CSG_Table_Record *pClass = pLUT->asTable()->Add_Record();\
			pClass->Set_Value(0, color);\
			pClass->Set_Value(1, name );\
			pClass->Set_Value(2, ""   );\
			pClass->Set_Value(3, id   );\
			pClass->Set_Value(4, id   );\
		}

		pLUT->asTable()->Del_Records();

		CSG_Colors	Colors(2 * MAX_GENERATIONS, SG_COLORS_YELLOW_RED);

		ADD_LUT_CLASS(0, SG_GET_RGB(000, 127, 000), _TL("no generation"));

		for(int i=1, j=1; i<=MAX_GENERATIONS; i++, j+=2)
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

	for(int i=0; i<MAX_GENERATIONS; i++)
	{
		if( m_pOnsets[i][0] ) m_pOnsets[i][0]->Set_NoData(x, y);
		if( m_pOnsets[i][1] ) m_pOnsets[i][1]->Set_NoData(x, y);
		if( m_pBTsums[i][0] ) m_pBTsums[i][0]->Set_NoData(x, y);
		if( m_pBTsums[i][1] ) m_pBTsums[i][1]->Set_NoData(x, y);
	}

	if( m_pGenerations ) m_pGenerations->Set_NoData(x, y);

	return( true );
}

//---------------------------------------------------------
inline bool CPhenIps_Grids::Set_Values(int x, int y, const CPhenIps &PhenIps)
{
	if( m_pOnset && m_pOnset->asInt(x, y) < 1 ) m_pOnset->Set_Value(x, y, PhenIps.Get_Parent_Onset());

	for(int i=0; i<MAX_GENERATIONS; i++)
	{
		if( m_pOnsets[i][0] && m_pOnsets[i][0]->asInt(x, y) < 1 ) m_pOnsets[i][0]->Set_Value(x, y, PhenIps.Get_Filial_Onset(i));
		if( m_pOnsets[i][1] && m_pOnsets[i][1]->asInt(x, y) < 1 ) m_pOnsets[i][1]->Set_Value(x, y, PhenIps.Get_Sister_Onset(i));

		if( m_pBTsums[i][0] ) m_pBTsums[i][0]->Set_Value(x, y, PhenIps.Get_Filial_BTsum(i));
		if( m_pBTsums[i][1] ) m_pBTsums[i][1]->Set_Value(x, y, PhenIps.Get_Sister_BTsum(i));
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

	Parameters.Set_Enabled("Risk_DayMax", false);
	Parameters.Set_Enabled("Risk_Decay" , false);
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

	Parameters.Add_Grid("",
		"RISK"		, _TL("Risk"),
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

	CSG_Grid	*pRisk	= Parameters("RISK")->asGrid();

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

				PhenIps.Set_Parent_Onset(m_pOnset->asInt(x, y));

				PhenIps.Set_Parent_ATsum(pATsum_eff->asDouble(x, y));

				if( m_pOnset->asInt(x, y) > 0 )
				{
					for(int i=0; i<MAX_GENERATIONS; i++)
					{
						PhenIps.Set_Filial_Onset(i, m_pOnsets[i][0]->asInt   (x, y));
						PhenIps.Set_Sister_Onset(i, m_pOnsets[i][1]->asInt   (x, y));

						PhenIps.Set_Filial_BTsum(i, m_pBTsums[i][0]->asDouble(x, y));
						PhenIps.Set_Sister_BTsum(i, m_pBTsums[i][1]->asDouble(x, y));
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

					pATsum_eff->Set_Value(x, y, PhenIps.Get_Parent_ATsum());

					pRisk->Set_Value(x, y, PhenIps.Get_Risk());
				}
			}

			if( !bOkay )
			{
				Set_NoData(x, y);

				pATsum_eff->Set_NoData(x, y);

				pRisk->Set_NoData(x, y);
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
