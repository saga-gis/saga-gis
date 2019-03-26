
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

	bool						Add_Day				(int Day, double ATmean, double ATmax, double SIrel, double DayLength = 24.0);

	int							Get_Onset_Parent	(void)	const	{	return( m_YD_Onset[0] );	}
	double						Get_State_Parent	(bool bLimit = false)	const	{	return( bLimit && m_ATsum_eff >= m_DDminimum ? 1.0 : m_ATsum_eff / m_DDminimum );	}

	double						Get_State_Filial	(size_t Generation, bool bLimit = false)	const	{	return( Get_State(Generation, 0, bLimit) );	}
	int							Get_Onset_Filial	(size_t Generation)	const	{	return( m_YD_Onset[1 + 2 * Generation] ); }

	double						Get_State_Sister	(size_t Generation, bool bLimit = false)	const	{	return( Get_State(Generation, 1, bLimit) );	}
	int							Get_Onset_Sister	(size_t Generation)	const	{	return( m_YD_Onset[2 + 2 * Generation] ); }

	int							Get_Generations		(double minState = 0.6)	const;


private:

	//-----------------------------------------------------
	// model parameters

	double				m_DTminimum, m_DToptimum, m_FAminimum, m_DDminimum, m_DDtotal;

	//-----------------------------------------------------
	// state variables

	int					m_State, m_YD_Begin, m_YD_End, m_YD_End_Onset;

	double				m_ATsum_eff;

	CSG_Array_Int		m_YD_Onset;

	CSG_Matrix			m_BTsum_eff;


	double				Get_State			(int Generation, int Sister, bool bLimit)	const
	{
		double	BTsum_eff	= Generation < 3 ? m_BTsum_eff[Sister][Generation] : 0.0;

		return( BTsum_eff < 0.0 ? 0.0 : bLimit && BTsum_eff >= m_DDtotal ? 1.0 : BTsum_eff / m_DDtotal );
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

		return( BTsum_Diff > 0.0 ? BTsum_Diff : 0.0 );
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

	m_State			= BROOD_STATE_BEFORE;
	m_YD_Onset.Create(1 + 2 * 3);
	m_YD_Onset		= 0;

	m_ATsum_eff		= 0.0;
	m_BTsum_eff.Create(3, 2);
	m_BTsum_eff		= -1.0;

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

	m_State			= PhenIps.m_State;
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
	m_State			= BROOD_STATE_BEFORE;
	m_YD_Onset		= 0;	// invalid day

	m_ATsum_eff		= 0.0;
	m_BTsum_eff		= -1.0;

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
	Parameters.Add_Double("", "DDminimum", _TL("Minimum Thermal Sum for Infestation"    ), _TL("Degree Days"), 140, 0.0, true);
	Parameters.Add_Double("", "DDtotal"  , _TL("Thermal Sum for Total Development"      ), _TL("Degree Days"), 557, 0.0, true);

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
bool CPhenIps::Add_Day(int Day, double ATmean, double ATmax, double SIrel, double DayLength)
{
	if( Day < m_YD_Begin )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Day >= m_YD_End_Onset )
	{
		m_State	= BROOD_STATE_AFTER;
	}

	//-----------------------------------------------------
	if( m_State == BROOD_STATE_BEFORE && Get_Onset(ATmax) )	// Compute the day of year of the onset of swarming of 'Ips typographus'.
	{
		m_State	= BROOD_STATE_BREEDING;

		m_YD_Onset[0]	= Day;

		m_BTsum_eff[0][0]	= 0.0;
	}

	//-----------------------------------------------------
	if( m_State > BROOD_STATE_BEFORE )
	{
		double	BTeff	= Get_BTmean(ATmean, SIrel) - m_DTminimum;
		double	BTmax	= Get_BTmax (ATmax , SIrel);

		if( BTmax > m_DToptimum )
		{
			BTeff	-= Get_BTsum_Diff(BTmax);
		}

		if( BTeff < 0.0 )
		{
			BTeff	= 0.0;
		}

		//-------------------------------------------------
		for(int i=0; i<3; i++)
		{
			if( m_BTsum_eff[0][i] < 0.0 && Get_State_Filial(i - 1) > 1.0 && ATmax > m_FAminimum && DayLength >= 14.5 )
			{
				m_BTsum_eff[0][i]	= BTeff;
			}
			else if( m_BTsum_eff[0][i] >= 0.0 )
			{
				m_BTsum_eff[0][i]	+= BTeff;

				if( m_YD_Onset[1 + 2 * i] == 0 && m_BTsum_eff[0][i] >= 1.0 )
				{
					m_YD_Onset[1 + 2 * i]	= Day;
				}

				if( m_BTsum_eff[1][i] < 0.0 && Get_State_Filial(i) > 0.5 && ATmax > m_FAminimum && DayLength >= 14.5 )
				{
					m_BTsum_eff[1][i]	= BTeff;
				}
				else if( m_BTsum_eff[1][i] >= 0.0 )
				{
					m_BTsum_eff[1][i]	+= BTeff;

					if( m_YD_Onset[2 + 2 * i] == 0 && m_BTsum_eff[1][i] >= 1.0 )
					{
						m_YD_Onset[2 + 2 * i]	= Day;
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

	for(int i=0; i<3; i++)
	{
		if( Get_State_Filial(i) >= minState )	{	n++;	}
		if( Get_State_Sister(i) >= minState )	{	n++;	}
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
		50.0, -90.0, true, 90.0, true
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
		pRecord->Set_Value(2, PhenIps.Get_State_Parent(   bLimit));
		pRecord->Set_Value(3, PhenIps.Get_State_Filial(0, bLimit));
		pRecord->Set_Value(4, PhenIps.Get_State_Sister(0, bLimit));
		pRecord->Set_Value(5, PhenIps.Get_State_Filial(1, bLimit));
		pRecord->Set_Value(6, PhenIps.Get_State_Sister(1, bLimit));
		pRecord->Set_Value(7, PhenIps.Get_State_Filial(2, bLimit));
		pRecord->Set_Value(8, PhenIps.Get_State_Sister(2, bLimit));
	}

	//-----------------------------------------------------
	#define ADD_SUMMARY(name, value)	{ CSG_Table_Record *pR = pSummary->Add_Record(); pR->Set_Value(0, name); pR->Set_Value(1, value); }

	CSG_Table	*pSummary	= Parameters("SUMMARY")->asTable();

	pSummary->Destroy();
	pSummary->Set_Name(_TL("Summary"));
	pSummary->Add_Field("VARIABLE", SG_DATATYPE_String);
	pSummary->Add_Field("VALUE"   , SG_DATATYPE_Double);

	ADD_SUMMARY(_TL("Day of Infestion"), PhenIps.Get_Onset_Parent());

	int	i, n	= 0;

	for(i=0; i<3; i++)
	{
		if( PhenIps.Get_State_Filial(i) > 0.6 )
		{
			n++;
		}
	}

	ADD_SUMMARY(_TL("1st Filial State"), PhenIps.Get_State_Filial(0, false));
	ADD_SUMMARY(_TL("1st Sister State"), PhenIps.Get_State_Sister(0, false));
	ADD_SUMMARY(_TL("2nd Filial State"), PhenIps.Get_State_Filial(1, false));
	ADD_SUMMARY(_TL("2nd Sister State"), PhenIps.Get_State_Sister(1, false));
	ADD_SUMMARY(_TL("3rd Filial State"), PhenIps.Get_State_Filial(2, false));
	ADD_SUMMARY(_TL("3rd Sister State"), PhenIps.Get_State_Sister(2, false));

	ADD_SUMMARY(_TL("1st Filial Grown"), PhenIps.Get_Onset_Filial(0));
	ADD_SUMMARY(_TL("1st Sister Grown"), PhenIps.Get_Onset_Sister(0));
	ADD_SUMMARY(_TL("2nd Filial Grown"), PhenIps.Get_Onset_Filial(1));
	ADD_SUMMARY(_TL("2nd Sister Grown"), PhenIps.Get_Onset_Sister(1));
	ADD_SUMMARY(_TL("3rd Filial Grown"), PhenIps.Get_Onset_Filial(2));
	ADD_SUMMARY(_TL("3rd Sister Grown"), PhenIps.Get_Onset_Sister(2));

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
	Set_Name		(CSG_String::Format("PhenIps (%s)", _TL("Grids")));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(CPhenIps::Get_Description());

	ADD_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Grid_List("", "ATmean", _TL("Mean Temperature"   ), _TL("Degree Celsius"), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "ATmax" , _TL("Maximum Temperature"), _TL("Degree Celsius"), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "SIrel" , _TL("Solar Irradiation"  ), _TL("Wh/m^2"        ), PARAMETER_INPUT);

	Parameters.Add_Grid("", "ONSET" , _TL("Onset Day of Infestation"       ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
	Parameters.Add_Grid("", "NUMBER", _TL("Potential Number of Generations"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);

	Parameters.Add_Grid("", "ONSET_FILIAL_1", _TL("Onset Day of 1st Filial Generation"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
	Parameters.Add_Grid("", "ONSET_SISTER_1", _TL("Onset Day of 1st Sister Generation"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
	Parameters.Add_Grid("", "ONSET_FILIAL_2", _TL("Onset Day of 2nd Filial Generation"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
	Parameters.Add_Grid("", "ONSET_SISTER_2", _TL("Onset Day of 2nd Sister Generation"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
	Parameters.Add_Grid("", "ONSET_FILIAL_3", _TL("Onset Day of 3rd Filial Generation"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);
	Parameters.Add_Grid("", "ONSET_SISTER_3", _TL("Onset Day of 3rd Sister Generation"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Short);

	Parameters.Add_Grid("", "STATE_FILIAL_1", _TL("State of 1st Filial Generation"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "STATE_SISTER_1", _TL("State of 1st Sister Generation"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "STATE_FILIAL_2", _TL("State of 2nd Filial Generation"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "STATE_SISTER_2", _TL("State of 2nd Sister Generation"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "STATE_FILIAL_3", _TL("State of 3rd Filial Generation"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "STATE_SISTER_3", _TL("State of 3rd Sister Generation"    ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Double("", "LATITUDE", _TL("Latitude"), _TL(""), 50.0, -90.0, true, 90.0, true);

	//-----------------------------------------------------
	CPhenIps::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPhenIps_Grids::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("ATmean") )
	{
		pParameters->Set_Enabled("LATITUDE", pParameter->asGrid() && !pParameter->asGrid()->Get_Projection().is_Okay());
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPhenIps_Grids::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pATmean = Parameters("ATmean")->asGridList();
	CSG_Parameter_Grid_List	*pATmax  = Parameters("ATmax" )->asGridList();
	CSG_Parameter_Grid_List	*pSIrel  = Parameters("SIrel" )->asGridList();

	if( pATmean->Get_Grid_Count() != 12
	||  pATmax ->Get_Grid_Count() != 12
	||  pSIrel ->Get_Grid_Count() != 12 )
	{
		Error_Set(_TL("there has to be one input grid for each month"));

		return( false );
	}

	//-----------------------------------------------------
	#define GRID_INIT(g, b)	if( g ) { g->Set_NoData_Value(0.0); DataObject_Set_Colors(g, 11, SG_COLORS_RAINBOW, b); }

	CSG_Grid	*pOnset		= Parameters("ONSET" )->asGrid(); GRID_INIT(pOnset, true);
	CSG_Grid	*pNumber	= Parameters("NUMBER")->asGrid();

	CSG_Grid	*pOnsets[6], *pStates[6];
	{
		for(int i=1, f=0, s=1; i<=3; i++, f+=2, s+=2)
		{
			pOnsets[f]	= Parameters(CSG_String::Format("ONSET_FILIAL_%d", i))->asGrid(); GRID_INIT(pOnsets[f],  true);
			pOnsets[s]	= Parameters(CSG_String::Format("ONSET_SISTER_%d", i))->asGrid(); GRID_INIT(pOnsets[s],  true);
			pStates[f]	= Parameters(CSG_String::Format("STATE_FILIAL_%d", i))->asGrid(); GRID_INIT(pStates[f], false);
			pStates[s]	= Parameters(CSG_String::Format("STATE_SISTER_%d", i))->asGrid(); GRID_INIT(pStates[s], false);
		}
	}

	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pNumber, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		#define ADD_LUT_CLASS(id, color, name)	{ CSG_Table_Record *pClass = pLUT->asTable()->Add_Record();\
			pClass->Set_Value(0, color);\
			pClass->Set_Value(1, name );\
			pClass->Set_Value(2, ""   );\
			pClass->Set_Value(3, id   );\
			pClass->Set_Value(4, id   );\
		}

		pLUT->asTable()->Del_Records();

		ADD_LUT_CLASS(0, SG_GET_RGB(  0, 127,   0), _TL("no generation"));
		ADD_LUT_CLASS(1, SG_GET_RGB(255, 255,   0), _TL("1 generation"));
		ADD_LUT_CLASS(2, SG_GET_RGB(255, 191,   0), _TL("1 generation + sister brood"));
		ADD_LUT_CLASS(3, SG_GET_RGB(255, 127,   0), _TL("2 generations"));
		ADD_LUT_CLASS(4, SG_GET_RGB(255,   0,   0), _TL("2 generations + sister brood"));
		ADD_LUT_CLASS(5, SG_GET_RGB(191,   0,  63), _TL("3 generations"));
		ADD_LUT_CLASS(6, SG_GET_RGB(127,   0, 127), _TL("3 generations + sister brood"));

		DataObject_Set_Parameter(pNumber, pLUT);
		DataObject_Set_Parameter(pNumber, "COLORS_TYPE", 1);	// Classified
	}

	//-----------------------------------------------------
	double	Latitude	= Parameters("LATITUDE")->asDouble();

	CSG_Grid	Lat, *pLat	= NULL;

	CSG_Projection	Projection;

	if( Get_Projection(Projection) && Projection.is_Okay() )
	{
		bool	bResult;

		CSG_Grid	Lon(Get_System()); Lon.Get_Projection().Create(Projection);

		Lat.Create(Get_System());

		SG_RUN_TOOL(bResult, "pj_proj4", 17,	// geographic coordinate grids
			    SG_TOOL_PARAMETER_SET("GRID", &Lon)
			&&	SG_TOOL_PARAMETER_SET("LON" , &Lon)
			&&	SG_TOOL_PARAMETER_SET("LAT" , &Lat)
		)

		if( bResult )
		{
			pLat	= &Lat;
		}
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

			if( !Get_Daily(x, y, pATmean, ATmean)
			||  !Get_Daily(x, y, pATmax , ATmax )
			||  !Get_Daily(x, y, pSIrel , SIrel ) )
			{
				pOnset ->Set_NoData(x, y);
				pNumber->Set_NoData(x, y);

				for(int i=0; i<6; i++)
				{
					if( pOnsets[i] ) pOnsets[i]->Set_NoData(x, y);
					if( pStates[i] ) pStates[i]->Set_NoData(x, y);
				}

				continue;
			}

			//---------------------------------------------
			CPhenIps	PhenIps(_PhenIps);

			for(int Day=0; Day<365; Day++)
			{
				double	DayLength	= SG_Get_Day_Length(1 + Day, pLat ? pLat->asDouble(x, y) : Latitude);

				PhenIps.Add_Day(1 + Day, ATmean[Day], ATmax[Day], SIrel[Day], DayLength);
			}

			//---------------------------------------------
			pOnset ->Set_Value(x, y, PhenIps.Get_Onset_Parent());
			pNumber->Set_Value(x, y, PhenIps.Get_Generations(0.6));

			for(int i=0, f=0, s=1; i<3; i++, f+=2, s+=2)
			{
				if( pOnsets[f] ) pOnsets[f]->Set_Value(x, y, PhenIps.Get_Onset_Filial(i));
				if( pOnsets[s] ) pOnsets[s]->Set_Value(x, y, PhenIps.Get_Onset_Sister(i));
				if( pStates[f] ) pStates[f]->Set_Value(x, y, PhenIps.Get_State_Filial(i));
				if( pStates[s] ) pStates[s]->Set_Value(x, y, PhenIps.Get_State_Sister(i));
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
bool CPhenIps_Grids::Get_Daily(int x, int y, CSG_Parameter_Grid_List *pValues, CSG_Vector &Values)
{
	//-----------------------------------------------------
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
//#' Compute the day of year of the onset of swarming of 'Ips typographus'.
//#'
//#' @param year integer. The current year for which the onset of swarming should be calculated.
//#' @param init RasterLayer object. This raster is used to initialize the raster that will contain
//#'   the day of year of onset of swarming. It must have the same properties as the temperature
//#'   rasters that are used to calculate the day of year, with all values being zero.
//#' @param DT data.table. Contains the paths to all rasters needed for computation. These are the
//#'   daily rasters of maximum air temperature. The table's key must be set to the following columns
//#'   (in this order): 'variable', 'scenario', 'year', 'date'. Besides the key columns it must have a
//#'   column containing the raster paths named 'path'. To generate a table that fullfills these
//#'   criteria use the pfadfinder() function of this package.
//#' @param scenario character. If the onset of swarming for multiple climate model predictions is to
//#'   be computed, this argument is used to differ between them.
//#' @param threshold integer. Threshold of degree days that is needed for 'Ips typographus' to start
//#'   swarming.
//
//# TODO: wäre es nicht besser threshold fest zu codieren und nicht als Funktionsargument zuzulassen?
//
//onset <- function(year,init, DT, scenario, threshold)
//{
//  Y <- year
//  S <- scenario
//
//  # Tag des Jahres für 01. April und 31. August bestimmen:
//  start <- as.integer(strftime(as.Date(paste0(Y, "0401"), format = "%Y%m%d"), format = "%j"))
//  end <- as.integer(strftime(as.Date(paste0(Y, "0831"), format = "%Y%m%d"), format = "%j"))
//
//  # Aufnahmeraster initialisieren:
//  sum_eff_temp <- init
//
//  day_onset_of_infestation <- init
//  day_onset_of_infestation_switch <- init
//
//  # Tag des Befallsbeginns berechnen:
//  for (current_day in start:end)
//  {
//    # Maximale Lufttemperatur des aktuellen Tages über dem Entwicklungsminimum berechnen:
//    eff_temp_current_day <- raster::raster(rgdal::readGDAL(DT[.("tadx", S, Y)][current_day, path])) - 8.3
//    eff_temp_current_day[eff_temp_current_day < 0] <- 0
//
//    # Effektive maximale Lufttemperatur des aktuellen Tages zur Temperatursumme hinzufügen:
//    sum_eff_temp <- sum_eff_temp + eff_temp_current_day
//
//    # Bedingung für den ersten Befallsbeginn: Temperatursumme über dem Grenzwert, aktuelle maximale Lufttemperatur über 16.5°C.
//    condition_for_onset <- (sum_eff_temp >= threshold) & (eff_temp_current_day > 16.5) & (day_onset_of_infestation_switch == 0)
//
//    # Speichern des Tages für die Zellen, an denen die Bedingung zutrifft. "Switch" für diese Zellen umlegen:
//    day_onset_of_infestation[condition_for_onset] <- current_day
//    day_onset_of_infestation_switch[condition_for_onset] <- 1
//
//  }
//
//  # Wert für Zellen an denen kein Befall eintrat:
//  day_onset_of_infestation[day_onset_of_infestation==0] <- -999
//
//  return(day_onset_of_infestation)
//}
//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//btmean.func <- function(x, y) return(-0.173 + 0.0008518 * y + 1.054 * x)
//
//btmax.func <- function(x, y) return(1.656 + 0.002955 * y + 0.534 * x + 0.01884 * x^2)
//
//diff.btsum.func <- function(x) return((-310.667 + 9.603 * x)/24)
//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//#' Compute the degree days of effective bark temperature according to the PHENIPS model.
//#'
//#' @param year integer. The current year for which degree days should be calculated.
//#' @param dir.onset character. Path to the raster that contains the day of onset of swarming of ips
//#'   typographus for the year that degree days are computed.
//#' @param init RasterLayer object. This raster is used to initialize the raster that will contain
//#'   the degree days. It must have the same properties as the temperatur and irradiation rasters
//#'   that are used to calculate degree days, with all values being zero.
//#' @param DT data.table. Contains the paths to all rasters needed for computation. These are the
//#'   daily rasters of mean and maximum air temperature as well as the horizontal irradiation. The
//#'   table's key must be set to the following columns (in this order): 'variable', 'scenario',
//#'   'year', 'date'. Besides the key columns it must have a column containing the raster paths named
//#'   'path'. To generate a table that fullfills these criteria use the pfadfinder() function of this
//#'   package.
//#' @param scenario character. If the degree days for multiple climate model predictions are to be
//#'   computed, this argument is used to differ between them.
//#' @param unitconv numeric. Factor for unit conversion. Is multiplied to the raster values to get
//#'   the desired unit of gloabal irradiation (Wh/m^2).
//
//degreedays <- function(year, dir.onset, init, DT, scenario, unitconv)
//{
//  Y <- year
//  S <- scenario
//
//  # Tag des Jahres für 01. April und 31. August bestimmen:
//  start <- as.integer(strftime(as.Date(paste0(Y, "0401"), format = "%Y%m%d"), format = "%j"))
//  endofbrood <- as.integer(strftime(as.Date(paste0(Y, "0831"), format = "%Y%m%d"), format = "%j"))
//  end <- as.integer(strftime(as.Date(paste0(Y, "1031"), format = "%Y%m%d"), format = "%j"))        # Im Original-Skript wird die Temperatursumme bis Dezember gerechnet. Im Artikel wird jedoch immer wieder erwähnt, dass die Temperaturen nach Oktober oft unterhalb der unteren Entwicklungsgrenze liegen. Um die Recheneit zu verkürzen, berechne ich die Temperaturumme also nur bis Oktober.
//
//  # Raster mit Beginn des Befalls einlesen:
//  day_onset_of_infestation <- raster::raster(dir.onset)
//
//  # Raster erstellen, welche die Summe der effektiven Rindentemperatur, vor und nach dem Beginn der Diapause aufnehmen:
//  dd_eff_bark_temperature <- init
//  dd_eff_bark_temp_after_diapause <- init
//
//  for(current_day in start:end)
//  {
//    # Raster für diesen Tag lesen:
//    mean_temperature_current_day <- raster::raster(rgdal::readGDAL(DT[.("tadm", S, Y)][current_day, path]))
//    max_temperature_current_day <- raster::raster(rgdal::readGDAL(DT[.("tadx", S, Y)][current_day, path]))
//    horizontal_global_irradiation_current_day <- raster::raster(rgdal::readGDAL(DT[.("grhds", S, Y)][current_day, path]))*unitconv
//    # todo: Einheit anpassen!
//
//    # Mittlere Rindentemperatur berechnen:
//    mean_bark_temperature_current_day <- btmean.func(mean_temperature_current_day, horizontal_global_irradiation_current_day)
//
//    # Effektive Rindentemperatur berechnen. LINEAR! Korrektur bei Werten von btmax > 30.4°C:
//    mean_eff_bark_temperature_current_day <- mean_bark_temperature_current_day - 8.3
//
//    # Maximale Rindentemperatur berechnen (Name der Variable muss noch geändert werden!)
//    max_temperature_current_day <- btmax.func(max_temperature_current_day, horizontal_global_irradiation_current_day)
//
//    # Check, ob Korrektur notwendig ist:
//    correction_is_needed <- raster::cellStats(max_temperature_current_day, 'max') > 30.4
//
//    if(correction_is_needed)
//    {
//      # Differenz zwischen linearer und nichtlinearer effektiver Rindentemperatur berechnen:
//      correction_for_linear <- diff.btsum.func(max_temperature_current_day)
//      correction_for_linear[correction_for_linear < 0] <- 0
//
//      # Bestimmung der Zellen, an denen die Korrektur durchzuführen ist:
//      cells_that_need_correction <- max_temperature_current_day > 30.4
//
//      # Subtraktion des Korrekturwertes an den oben bestimmten Zellen:
//      mean_eff_bark_temperature_current_day[cells_that_need_correction] <-
//        mean_eff_bark_temperature_current_day[cells_that_need_correction] - correction_for_linear[cells_that_need_correction]
//    }
//
//    # Werte unter 0, sowie Zellen, an denen der Ausflug noch nicht stattgefunden hat, auf 0 setzen:
//    mean_eff_bark_temperature_current_day[(mean_eff_bark_temperature_current_day < 0) | (day_onset_of_infestation > current_day)] <- 0
//
//    # Summe der effektiven Rindentemperatur berechnen (vor und nach der Diapause):
//    if(current_day<=endofbrood)
//    {
//      dd_eff_bark_temperature <- dd_eff_bark_temperature + mean_eff_bark_temperature_current_day
//    }
//    else
//    {
//      dd_eff_bark_temp_after_diapause <- dd_eff_bark_temp_after_diapause + mean_eff_bark_temperature_current_day
//    }
//  }
//
//  # Temperatursumme nach der Diapause auf maximal 557 degree days beschränken:
//  dd_eff_bark_temp_after_diapause[dd_eff_bark_temp_after_diapause > 557] <- 557
//
//  # Temperatursumme vor und nach der Diapause addieren:
//  dd_eff_bark_temperature <- dd_eff_bark_temperature + dd_eff_bark_temp_after_diapause
//
//  return(raster::stack(dd_eff_bark_temperature, dd_eff_bark_temp_after_diapause))
//}
//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
