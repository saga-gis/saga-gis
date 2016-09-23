/**********************************************************
 * Version $Id: treeline.cpp 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     treeline.cpp                      //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "treeline.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTreeLine_Base::Add_Parameters(CSG_Parameters &Parameters)
{
	//-----------------------------------------------------
	Parameters.Add_Grid_List(NULL, "T"   , _TL("Mean Temperature"   ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List(NULL, "TMIN", _TL("Minimum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List(NULL, "TMAX", _TL("Maximum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List(NULL, "P"   , _TL("Precipitation"      ), _TL(""), PARAMETER_INPUT);
//	Parameters.Add_Grid_List(NULL, "CLDS", _TL("Cloudiness"         ), _TL(""), PARAMETER_INPUT);
//	Parameters.Add_Double(Parameters("CLDS"),
//		"CLDS_DEF"		, _TL("Default"),
//		_TL("Default cloudiness value."),
//		50.0, 0.0, true, 100.0, true
//	);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const(NULL,
		"TSWC"			,	_TL("Total Soil Water Capacity"),
		_TL("Total soil water capacity (mm H2O)."),
		220.0, 0.0, true
	);

	Parameters.Add_Double(Parameters("TSWC"),
		"USWC"			, _TL("Upper Soil Water Capacity"),
		_TL(""),
		10.0, 0.0, true
	);

	Parameters.Add_Double(Parameters("TSWC"),
		"LSW_RESIST"	, _TL("Transpiration Resistance"),
		_TL(""),
		1.0, 0.1, true
	);

	Parameters.Add_Double(Parameters("TSWC"),
		"SW_MIN"		, _TL("Minimum Soil Water Content (Percent)"),
		_TL(""),
		2.0, 0.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Double(NULL,
		"DT_MIN"		, _TL("Threshold Temperature"),
		_TL("Threshold temperature (C) that constrains the growing season."),
		0.9
	);

	Parameters.Add_Double(NULL,
		"SMT_MIN"		, _TL("Minimum Mean Temperature"),
		_TL("Minimum mean temperature (C) for all days of the growing season."),
		6.4
	);

	Parameters.Add_Int(NULL,
		"LGS_MIN"		, _TL("Minimum Length"),
		_TL("Minimum length (days) of the growing season."),
		94, 1, true
	);

	Parameters.Add_Double(NULL,
		"LAT_DEF"		, _TL("Default Latitude"),
		_TL(""),
		50.0, -90.0, true, 90.0, true
	);
}

//---------------------------------------------------------
bool CTreeLine_Base::Set_Parameters(CSG_Parameters &Parameters)
{
	//-----------------------------------------------------
	m_pT   	= Parameters("T"   )->asGridList();
	m_pTmin	= Parameters("TMIN")->asGridList();
	m_pTmax	= Parameters("TMAX")->asGridList();
	m_pP   	= Parameters("P"   )->asGridList();

	if( m_pT   ->Get_Count() != 12
	||  m_pTmin->Get_Count() != 12
	||  m_pTmax->Get_Count() != 12
	||  m_pP   ->Get_Count() != 12 )
	{
		SG_UI_Msg_Add_Error(_TL("there has to be one input grid for each month"));

		return( false );
	}

	//-----------------------------------------------------
	m_TSWC	= Parameters("TSWC")->asDouble();
	m_pTSWC	= Parameters("TSWC")->asGrid  ();

	//-----------------------------------------------------
	m_Lat_Default	= Parameters("LAT_DEF")->asDouble() * M_DEG_TO_RAD;

	m_pLat	= NULL;

	if( m_pT->asGrid(0)->Get_Projection().is_Okay() )
	{
		bool		bResult;
		CSG_Grid	Lon(m_pT->asGrid(0)->Get_System());
		m_Lat.Create(m_pT->asGrid(0)->Get_System());

		SG_RUN_TOOL(bResult, "pj_proj4", 17,	// geographic coordinate grids
				SG_TOOL_PARAMETER_SET("GRID", m_pT->asGrid(0))
			&&	SG_TOOL_PARAMETER_SET("LON" , &Lon)
			&&	SG_TOOL_PARAMETER_SET("LAT" , &m_Lat)
		)

		if( bResult )
		{
			m_pLat	= &m_Lat;
			m_pLat->Set_Scaling(M_DEG_TO_RAD);
		}
	}

	//-----------------------------------------------------
	m_Parms. DT_min		= Parameters( "DT_MIN"   )->asDouble();
	m_Parms.SMT_min		= Parameters("SMT_MIN"   )->asDouble();
	m_Parms.LGS_min		= Parameters("LGS_MIN"   )->asDouble();
	m_Parms.SWC_Surface	= Parameters("USWC"      )->asDouble();
	m_Parms.SW_min		= Parameters("LSW_RESIST")->asDouble() / 100.0;
	m_Parms.SW_Resist	= Parameters("LSW_RESIST")->asDouble();
	m_Parms.SW_MaxIter	= 64;
}

//---------------------------------------------------------
bool CTreeLine_Base::Get_Monthly(double Monthly[12], int x, int y, CSG_Parameter_Grid_List *pMonthly, double Default)
{
	if( pMonthly->Get_Count() == 12 )
	{
		bool	bOkay	= true;

		for(int iMonth=0; iMonth<12; iMonth++)
		{
			if( pMonthly->asGrid(iMonth)->is_NoData(x, y) )
			{
				bOkay	= false;

				Monthly[iMonth]	= Default;
			}
			else
			{
				Monthly[iMonth]	= pMonthly->asGrid(iMonth)->asDouble(x, y);
			}
		}

		return( bOkay );
	}

	for(int iMonth=0; iMonth<12; iMonth++)
	{
		Monthly[iMonth]	= Default;
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTreeLine::CTreeLine(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Tree Line"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Tree line estimation."
	));

	Add_Reference("Paulsen, J. / Körner, C.", "2014",
		"A climate-based model to predict potential treeline position around the globe",
		"Alpine Botany, 124:1, 1–12. doi:10.1007/s00035-014-0124-0.",
		SG_T("http://link.springer.com/article/10.1007%2Fs00035-014-0124-0"), _TL("online")
	);

	//-----------------------------------------------------
//	Parameters.Add_Grid(NULL,
//		"TREELINE"	, _TL("Tree Line Height"),
//		_TL(""),
//		PARAMETER_OUTPUT
//	);

	Parameters.Add_Grid(NULL,
		"SMT"		, _TL("Mean Temperature"),
		_TL("Mean temperature of the growing season."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(NULL,
		"LGS"		, _TL("Length"),
		_TL("Number of days of the growing season."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	//-----------------------------------------------------
	CTreeLine_Base::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTreeLine::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTreeLine::On_Execute(void)
{
	if( !CTreeLine_Base::Set_Parameters(Parameters) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pSMT	= Parameters("SMT")->asGrid();
	CSG_Grid	*pLGS	= Parameters("LGS")->asGrid();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
#ifndef _DEBUG
		#pragma omp parallel for
#endif
		for(int x=0; x<Get_NX(); x++)
		{
			CTreeGrowingSeason	TreeGrowth(m_Parms,
				m_pTSWC && !m_pTSWC->is_NoData(x, y) ? m_pTSWC->asDouble(x, y) : m_TSWC,
				m_pLat ? m_pLat->asDouble(x, y) : m_Lat_Default
			);

			if( Get_Monthly(TreeGrowth.Get_Monthly(PARM_T   ), x, y, m_pT   )
			&&  Get_Monthly(TreeGrowth.Get_Monthly(PARM_TMIN), x, y, m_pTmin)
			&&  Get_Monthly(TreeGrowth.Get_Monthly(PARM_TMAX), x, y, m_pTmax)
			&&  Get_Monthly(TreeGrowth.Get_Monthly(PARM_P   ), x, y, m_pP   )
			&&  TreeGrowth.Calculate() )
			{
				pLGS->Set_Value(x, y, TreeGrowth.Get_LGS());

				if( TreeGrowth.Get_LGS() > 0 )
				{
					pSMT->Set_Value(x, y, TreeGrowth.Get_SMT());
				}
				else
				{
					pSMT->Set_NoData(x, y);
				}
			}
			else
			{
				pLGS->Set_NoData(x, y);
				pSMT->Set_NoData(x, y);
			}
		}
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
CTreeLine_Interactive::CTreeLine_Interactive(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Tree Line"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Tree line estimation."
	));

	Add_Reference("Paulsen, J. / Körner, C.", "2014",
		"A climate-based model to predict potential treeline position around the globe",
		"Alpine Botany, 124:1, 1–12. doi:10.1007/s00035-014-0124-0.",
		SG_T("http://link.springer.com/article/10.1007%2Fs00035-014-0124-0"), _TL("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Table(NULL,
		"SUMMARY"	, _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(NULL,
		"DAILY"		, _TL("Daily"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	CTreeLine_Base::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTreeLine_Interactive::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTreeLine_Interactive::On_Execute(void)
{
	if( !CTreeLine_Base::Set_Parameters(Parameters) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pSummary	= Parameters("SUMMARY")->asTable();
	m_pSummary->Destroy();
	m_pSummary->Set_Name(CSG_String::Format("%s [%s]", _TL("Tree Growth"), _TL("Summary")));
	m_pSummary->Add_Field("NAME" , SG_DATATYPE_String);
	m_pSummary->Add_Field("VALUE", SG_DATATYPE_Double);
	m_pSummary->Add_Record()->Set_Value(0, _TL("X"));
	m_pSummary->Add_Record()->Set_Value(0, _TL("Y"));
	m_pSummary->Add_Record()->Set_Value(0, _TL("Latitude"));
	m_pSummary->Add_Record()->Set_Value(0, _TL("Length of Growing Season"));
	m_pSummary->Add_Record()->Set_Value(0, _TL("Mean Temperature"));

	//-----------------------------------------------------
	m_pDaily	= Parameters("DAILY")->asTable();
	m_pDaily->Destroy();
	m_pDaily->Set_Name(CSG_String::Format("%s [%s]", _TL("Tree Line"), _TL("Climate")));
	m_pDaily->Add_Field("T"   , SG_DATATYPE_Double);
	m_pDaily->Add_Field("P"   , SG_DATATYPE_Double);
	m_pDaily->Add_Field("SNOW", SG_DATATYPE_Double);
	m_pDaily->Add_Field("ETP" , SG_DATATYPE_Double);
	m_pDaily->Add_Field("SW_0", SG_DATATYPE_Double);
	m_pDaily->Add_Field("SW_1", SG_DATATYPE_Double);
	m_pDaily->Set_Record_Count(365);

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CTreeLine_Interactive::On_Execute_Finish(void)
{
	m_Lat.Destroy();

	return( true );
}

//---------------------------------------------------------
bool CTreeLine_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if( Mode != TOOL_INTERACTIVE_LDOWN )
	{
		return( false );
	}

	//-----------------------------------------------------
	int	x, y;

	if( !Get_System()->Get_World_to_Grid(x, y, ptWorld) || !Get_System()->is_InGrid(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	Lat	= m_pLat ? m_pLat->asDouble(x, y) : m_Lat_Default;

	CTreeGrowingSeason	TreeGrowth(m_Parms,
		m_pTSWC && !m_pTSWC->is_NoData(x, y) ? m_pTSWC->asDouble(x, y) : m_TSWC, Lat		
	);

	if( Get_Monthly(TreeGrowth.Get_Monthly(PARM_T   ), x, y, m_pT   )
	&&  Get_Monthly(TreeGrowth.Get_Monthly(PARM_TMIN), x, y, m_pTmin)
	&&  Get_Monthly(TreeGrowth.Get_Monthly(PARM_TMAX), x, y, m_pTmax)
	&&  Get_Monthly(TreeGrowth.Get_Monthly(PARM_P   ), x, y, m_pP   ) )
	{
		TreeGrowth.Calculate();

		for(int iDay=0; iDay<365; iDay++)
		{
			CSG_Table_Record	*pRecord	= m_pDaily->Get_Record(iDay);

			pRecord->Set_Value(0, TreeGrowth.Get_T   (iDay));
			pRecord->Set_Value(1, TreeGrowth.Get_P   (iDay));
			pRecord->Set_Value(2, TreeGrowth.Get_Snow(iDay));
			pRecord->Set_Value(3, TreeGrowth.Get_ET  (iDay));
			pRecord->Set_Value(4, TreeGrowth.Get_SW_0(iDay));
			pRecord->Set_Value(5, TreeGrowth.Get_SW_1(iDay));
		}

		//-------------------------------------------------
		m_pSummary->Get_Record(0)->Set_Value(1, ptWorld.Get_X());
		m_pSummary->Get_Record(1)->Set_Value(1, ptWorld.Get_Y());
		m_pSummary->Get_Record(2)->Set_Value(1, M_RAD_TO_DEG * Lat);
		m_pSummary->Get_Record(3)->Set_Value(1, TreeGrowth.Get_LGS());
		m_pSummary->Get_Record(4)->Set_Value(1, TreeGrowth.Get_SMT());

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
CTreeGrowingSeason::CTreeGrowingSeason(const TTGS_Parameters &Parms, double TSWC, double Latitude)
{
	m_Parms		= Parms;

	m_SWC[0]	= TSWC > m_Parms.SWC_Surface ? m_Parms.SWC_Surface : TSWC;
	m_SWC[1]	= TSWC > m_Parms.SWC_Surface ? TSWC - m_Parms.SWC_Surface : 0.0;

	m_Lat		= Latitude;

	m_Daily[0].Create(365);
	m_Daily[1].Create(365);
	m_Daily[2].Create(365);
	m_Daily[3].Create(365);
	m_Snow    .Create(365);
	m_ET      .Create(365);
	m_SW[0]   .Create(365);
	m_SW[1]   .Create(365);
}

//---------------------------------------------------------
bool CTreeGrowingSeason::Calculate(void)
{
	memset(m_Growing, 0, 365 * sizeof(int));

	Set_Temperature();
	Set_Snow_Depth();
	Set_Soil_Water();

	return( Get_TGS() );

//	return( Set_Temperature() && Set_Snow_Depth() && Set_Soil_Water() );
}

//---------------------------------------------------------
bool CTreeGrowingSeason::Get_TGS(void)
{
	const double	*T	= m_Daily[PARM_T];

	m_TGS.Create();

	for(int iDay=0; iDay<365; iDay++)
	{
		if( !m_Growing[iDay] )
		{
			m_TGS	+= T[iDay];
		}
	}

	return( m_TGS.Get_Count() >= m_Parms.LGS_min && m_TGS.Get_Mean() >= m_Parms.SMT_min );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTreeGrowingSeason::Set_Daily_Spline(CSG_Vector &Daily, const double *Monthly)
{
	static const int	MidOfMonth[12]	=
	// JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT  NOV  DEC
	//	 0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334
	{	15,  45,  74, 105, 135, 166, 196, 227, 258, 288, 319, 349	};

	CSG_Spline	Spline;

	Spline.Add(MidOfMonth[11] - 365, Monthly[11]);
	Spline.Add(MidOfMonth[10] - 365, Monthly[10]);

	for(int iMonth=0; iMonth<12; iMonth++)
	{
		Spline.Add(MidOfMonth[iMonth], Monthly[iMonth]);
	}

	Spline.Add(MidOfMonth[ 0] + 365, Monthly[ 0]);
	Spline.Add(MidOfMonth[ 1] + 365, Monthly[ 1]);

	for(int iDay=0; iDay<365; iDay++)
	{
		Daily[iDay]	= Spline.Get_Value(iDay);
	}
}

//---------------------------------------------------------
void CTreeGrowingSeason::Set_Daily_Events(CSG_Vector &Daily_P, const double *Monthly_P, const double *Monthly_T)
{
	static const int	nDaysOfMonth[12]	=
	// JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT  NOV  DEC
	{	31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31	};

	for(int iMonth=0, iDay=0; iMonth<12; iDay+=nDaysOfMonth[iMonth++])
	{
		double	dEvent	= Monthly_T[iMonth] < 5 ? 5 : Monthly_T[iMonth] < 10 ? 10 : 20;
		int		nEvents	= (int)(0.5 + Monthly_P[iMonth] / dEvent);

		if( nEvents < 1 )
		{
			nEvents	= 1;
		}
		else if( nEvents > nDaysOfMonth[iMonth] )
		{
			nEvents	= nDaysOfMonth[iMonth];
		}

		dEvent	= Monthly_P[iMonth] / nEvents;

		int	Step	= nDaysOfMonth[iMonth] / nEvents;

		for(int iEvent=0, jDay=Step/2; iEvent<nEvents; iEvent++, jDay+=Step)
		{
			Daily_P[iDay + jDay]	= dEvent;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTreeGrowingSeason::Set_Temperature(void)
{
	Set_Daily_Spline(m_Daily[PARM_T], m_Monthly[PARM_T]);

	//-----------------------------------------------------
	const double	*T	= m_Daily[PARM_T];

	for(int iDay=0; iDay<365; iDay++)
	{
		if( T[iDay] < m_Parms.DT_min )
		{
			m_Growing[iDay]	|= TGS_COLD;
		}
	}

	return( Get_TGS() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTreeGrowingSeason::Set_Snow_Depth(void)
{
	Set_Daily_Events(m_Daily[PARM_P], m_Monthly[PARM_P], m_Monthly[PARM_T]);

	//-----------------------------------------------------
	const double	*T	= m_Daily[PARM_T];
	const double	*P	= m_Daily[PARM_P];

	int		iStart	= Snow_Get_Start(T);

	if( iStart < 0 )	// no frost at all
	{
		if( T[0] < 0.0 )
		{
			m_Snow	= 1.0;
		}
		else
		{
			m_Snow	= 0.0;
		}

		return( true );
	}

	//-----------------------------------------------------
	int	nSnow_Last, nSnow	= 0;

	double	Snow	= 0.0;

	m_Snow	= 0.0;

	do
	{
		nSnow_Last	= nSnow;	nSnow	= 0;

		for(int iDay=iStart; iDay<iStart+365; iDay++)
		{
			int	i	= iDay % 365;

			if( T[i] < 0.0 )		// snow accumulation
			{
				Snow	+= P[i];
			}
			else if( Snow > 0.0 )	// snow melt
			{
				Snow	-= Snow_Get_SnowMelt(Snow, T[i], P[i]);
			}

			if( Snow > 0.0 )
			{
				nSnow++;
			}

			m_Snow[i]	= Snow;
		}
	}
	while( nSnow != nSnow_Last && nSnow < 365 );

	//-----------------------------------------------------
	for(int iDay=0; iDay<365; iDay++)
	{
		if( m_Snow[iDay] > 0.0 )
		{
			m_Growing[iDay]	|= TGS_SNOW;
		}
	}

	return( Get_TGS() );
}

//---------------------------------------------------------
int CTreeGrowingSeason::Snow_Get_Start(const double *T)
{
	int	iMax = -1, nMax = 0;

	for(int iDay=0; iDay<365; iDay++)
	{
		if( T[iDay] <= 0.0 )
		{
			int	i = iDay + 1, n = 0;

			while( T[i % 365] > 0.0 )	{	i++; n++;	}

			if( nMax < n )
			{
				nMax	= n;
				iMax	= i - 1;
			}
		}
	}

	return( iMax % 365 );
}

//---------------------------------------------------------
inline double CTreeGrowingSeason::Snow_Get_SnowMelt(double Snow, double T, double P)
{
	if( T > 0.0 )
	{
		double	dSnow	= T * (0.84 + 0.125 * P);

		return( dSnow > Snow ? Snow : dSnow );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTreeGrowingSeason::Set_Soil_Water(void)
{
	const double	*T	= m_Daily[PARM_T];
	const double	*P	= m_Daily[PARM_P];

	//-----------------------------------------------------
	Soil_Set_ET(m_ET.Get_Data());

	//-----------------------------------------------------
	int	iStart	= Soil_Get_Start(P, m_ET), iPass	= 0;

	double	SW[2], SW_Last[2];

	SW[0]	= 0.5 * m_SWC[0];
	SW[1]	= 0.5 * m_SWC[1];

	do
	{
		SW_Last[0]	= SW[0];
		SW_Last[1]	= SW[1];

		for(int iDay=iStart; iDay<iStart+365; iDay++)
		{
			int	i	= iDay % 365;

			if( T[i] > 0.0 )
			{
				//---------------------------------------------
				// upper soil layer

				double	dSW	= P[i];

				if( m_Snow[i] > 0.0 )
				{
					dSW	+= Snow_Get_SnowMelt(m_Snow[i], T[i], P[i]);
				}
				else
				{
					dSW	-= m_ET[i];
				}

				SW[0]	+= dSW;

				if( SW[0] > m_SWC[0] )	// more water in upper soil layer than its capacity
				{
					dSW		= SW[0] - m_SWC[0];
					SW[0]	= m_SWC[0];
				}
				else if( SW[0] < 0.0 )	// evapotranspiration exceeds available water
				{
					dSW		= m_SWC[1] > 0.0 ? SW[0] * pow(SW[1] / m_SWC[1], m_Parms.SW_Resist) : 0.0;	// positive: runoff, negative: loss by evapotranspiration not covered by upper layer, with water loss resistance;
				//	dSW		= m_SWC[1] > 0.0 ? (SW[0] > -SW[1] ? SW[0] : -SW[1]) * sqrt(SW[1] / m_SWC[1]) : 0.0;

					SW[0]	= 0.0;
				}
				else
				{
					dSW		= 0.0;
				}

				//---------------------------------------------
				// lower soil layer

				SW[1]	+= dSW;

				if( SW[1] > m_SWC[1] )	// more water in lower soil layer than its capacity
				{
					SW[1]	= m_SWC[1];
				}
				else if( SW[1] < 0.0 )	// evapotranspiration exceeds available water
				{
					SW[1]	= 0.0;
				}
			}

			//---------------------------------------------
			m_SW[0][i]	= SW[0];
			m_SW[1][i]	= SW[1];
		}
	}
	while( iPass++ <= 1 || (SW_Last[0] != SW[0] && iPass <= m_Parms.SW_MaxIter) );

	//-----------------------------------------------------
	for(int iDay=0; iDay<365; iDay++)
	{
		if( m_SW[0][iDay] <= 0.0 && m_SW[1][iDay] < m_Parms.SW_min * m_SWC[1] )
		{
			m_Growing[iDay]	|= TGS_DRY;
		}
		else
		{
			m_Growing[iDay]	= m_Growing[iDay];
		}
	}

	return( Get_TGS() );
}

//---------------------------------------------------------
int CTreeGrowingSeason::Soil_Get_Start(const double *P, const double *ET)
{
	int	iMax = 0, nMax = 0;

	for(int iDay=0; iDay<365; iDay++)
	{
		if( P[iDay] <= 0.0 )
		{
			int	i = iDay + 1, n = 0;

			while( P[i % 365] > 0.0 )	{	i++; n++;	}

			if( nMax < n )
			{
				nMax	= n;
				iMax	= i - 1;
			}
		}
	}

	return( iMax % 365 );
}

//---------------------------------------------------------
#include "etp_hargreave.h"

//---------------------------------------------------------
void CTreeGrowingSeason::Soil_Set_ET(double *ET)
{
	Set_Daily_Spline(m_Daily[PARM_TMIN], m_Monthly[PARM_TMIN]);
	Set_Daily_Spline(m_Daily[PARM_TMAX], m_Monthly[PARM_TMAX]);

	const double	*T		= m_Daily[PARM_T   ];
	const double	*Tmin	= m_Daily[PARM_TMIN];
	const double	*Tmax	= m_Daily[PARM_TMAX];

	for(int i=0; i<365; i++)
	{
		ET[i]	= Get_PET_Hargreave(i + 1, m_Lat, T[i], Tmin[i], Tmax[i]);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
