
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
//                evapotranspiration.cpp                 //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "climate_tools.h"

#include "evapotranspiration.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CETpot_Day_To_Hour::CETpot_Day_To_Hour(void)
{
	Set_Name		(_TL("Daily to Hourly Evapotranspiration"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Derive hourly from daily evapotranspiration using sinusoidal distribution. "
	));

	Add_Reference("Ambikadevi, K.M.", "2004",
		"Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts.",
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst.",
		SG_T("http://scholarworks.umass.edu/cee_ewre/22/")
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"DAYS"	, _TL("Daily Data"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("DAYS",
		"JD"	, _TL("Julian Day"),
		_TL("")
	);

	Parameters.Add_Table_Field("DAYS",
		"ET"	, _TL("Evapotranspiration"),
		_TL("")
	);

	Parameters.Add_Table_Field("DAYS",
		"P"		, _TL("Precipitation"),
		_TL(""),
		true
	);

	Parameters.Add_Table("",
		"HOURS"	, _TL("Hourly Data"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("DAYS",
		"LAT"	, _TL("Latitude"),
		_TL(""),
		53., -90., true, 90., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CETpot_Day_To_Hour::On_Execute(void)
{
	CSG_Table	*pDays	= Parameters("DAYS" )->asTable();

	CSG_Table	*pHours	= Parameters("HOURS")->asTable();

	pHours->Destroy();
	pHours->Fmt_Name("%s [%s]", pDays->Get_Name(), _TL("h"));
	pHours->Add_Field("JULIAN_DAY", SG_DATATYPE_Int);
	pHours->Add_Field("HOUR"      , SG_DATATYPE_Int);
	pHours->Add_Field("ET"        , SG_DATATYPE_Double);

	int	fJD	= Parameters("JD")->asInt();
	int	fET	= Parameters("ET")->asInt();
	int	fP	= Parameters("P" )->asInt();

	if( fP >= 0 )
	{
		pHours->Add_Field("P", SG_DATATYPE_Double);
	}

	double	sinLat	= sin(Parameters("LAT")->asDouble() * M_DEG_TO_RAD);
	double	cosLat	= cos(Parameters("LAT")->asDouble() * M_DEG_TO_RAD);
	double	sinHgt	= 0.;	// -0.0145;	// >> -50'' desired height of horizon

	//-----------------------------------------------------
	for(int iDay=0; iDay<pDays->Get_Count() && Set_Progress(iDay, pDays->Get_Count()); iDay++)
	{
		CSG_Table_Record	*pDay	= pDays->Get_Record(iDay);

		double	ET, D, dT, fT, sRise, sSet;

		int	JD	= pDay->asInt(fJD);

		ET		= pDay->asDouble(fET);

		D		= 0.40954 * sin(0.0172 * (JD - 79.349740));	// sun's declination
		dT		= 12. * acos((sinHgt - sinLat * sin(D)) / (cosLat * cos(D))) / M_PI;

		fT		= -0.1752 * sin(0.033430 * JD + 0.5474) - 0.1340 * sin(0.018234 * JD - 0.1939);
		sRise	= 12. - dT - fT;
		sSet	= 12. + dT - fT;

		for(int iHour=0; iHour<24; iHour++)
		{
			CSG_Table_Record	*pHour	= pHours->Add_Record();

			pHour->Set_Value(0, JD);
			pHour->Set_Value(1, iHour);

			if( fP >= 0 )
			{
				pHour->Set_Value(3, pDay->asDouble(fP) / 24.);
			}

			if( sRise <= iHour && iHour <= sSet )
			{
				pHour->Set_Value(2, ET * (1. - cos(2. * M_PI * (iHour - sRise) / (sSet - sRise))) / 2.);
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
CETpot_Table::CETpot_Table(void)
{
	Set_Name		(_TL("Evapotranspiration (Table)"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Estimation of daily potential evapotranspiration from weather data with different methods. "
		"Besides mean daily temperature it depends on the chosen method which additional data has to "
		"be provided. In order to estimate extraterrestrial net radiation some of the methods need "
		"to know the location's geographic latitude and the date's Julian day number. "
	));

	Add_Reference("Allen, R.G., Pereira, L.S., Raes, D., Smith, M.", "1998",
		"Crop evapotranspiration - Guidelines for computing crop water requirements",
		"FAO Irrigation and drainage paper 56.",
		SG_T("http://www.fao.org/docrep/X0490E/x0490e00.htm#Contents")
	);

	Add_Reference("Ambikadevi, K.M.", "2004",
		"Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts",
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst.",
		SG_T("http://scholarworks.umass.edu/cee_ewre/22/")
	);

	Add_Reference("DVWK", "1996",
		"Ermittlung der Verdunstung von Land- u. Wasserflaechen",
		"Merkblaetter 238/1996."
	);

	Add_Reference("Hargreaves, G.H., Samani, Z.A.", "1985",
		"Reference crop evapotranspiration from ambient air temperatures",
		"Paper presented in ASAE Regional Meeting, Grand Junction, Colorado.",
		SG_T("http://cagesun.nmsu.edu/~zsamani/papers/Hargreaves_Samani_85.pdf")
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"	, _TL("Data"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE", "T"	   , _TL("Mean Temperature"   ), _TL("[Degree C]"));
	Parameters.Add_Table_Field("TABLE", "T_MIN", _TL("Minimum Temperature"), _TL("[Degree C]"));
	Parameters.Add_Table_Field("TABLE", "T_MAX", _TL("Maximum Temperature"), _TL("[Degree C]"));
	Parameters.Add_Table_Field("TABLE", "RH"   , _TL("Relative Humidity"  ), _TL("[Percent]" ));
	Parameters.Add_Table_Field("TABLE", "SR"   , _TL("Solar Radiation"    ), _TL("daily mean of wind speed at 2m above ground [m/s][Degree C]"));
	Parameters.Add_Table_Field("TABLE", "WS"   , _TL("Wind Speed"         ), _TL("daily sum of global radiation [J/cm^2]"));
	Parameters.Add_Table_Field("TABLE", "DATE" , _TL("Date"               ), _TL(""));

	Parameters.Add_Table_Field("TABLE", "ET"   , _TL("Evapotranspiration" ), _TL(""), true);

	Parameters.Add_Double("TABLE",
		"LAT"	, _TL("Latitude"),
		_TL(""),
		53., -90., true, 90., true
	);

	Parameters.Add_Choice("",
		"METHOD", _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Turc"),
			_TL("Hargreave"),
			_TL("Penman (simplified)")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CETpot_Table::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("T_MIN", pParameter->asInt() == 1);
		pParameters->Set_Enabled("T_MAX", pParameter->asInt() == 1);
		pParameters->Set_Enabled("RH"   , pParameter->asInt() != 1);
		pParameters->Set_Enabled("SR"   , pParameter->asInt() != 1);
		pParameters->Set_Enabled("WS"   , pParameter->asInt() == 2);
		pParameters->Set_Enabled("DATE" , pParameter->asInt() != 0);
		pParameters->Set_Enabled("LAT"  , pParameter->asInt() != 0);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CETpot_Table::On_Execute(void)
{
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	int	fET     = pTable->Get_Field_Count();

	pTable->Add_Field("ET", SG_DATATYPE_Double);

	int	Method  = Parameters("METHOD")->asInt();
	int	fT      = Parameters("T"     )->asInt();
	int	fTmin   = Parameters("T_MIN" )->asInt();
	int	fTmax   = Parameters("T_MAX" )->asInt();
	int	fRH     = Parameters("RH"    )->asInt();
	int	fSR     = Parameters("SR"    )->asInt();
	int	fWS     = Parameters("WS"    )->asInt();
	int	fDate   = Parameters("DATE"  )->asInt();
	double	Lat	= Parameters("LAT")->asDouble();

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	&V	= *pTable->Get_Record(iRecord);

		double	ET	= -1.;

		if( !V.is_NoData(fT) )
		{
			switch( Method )
			{
			case  0: // Turc
				if( !V.is_NoData(fSR) && !V.is_NoData(fRH) )
				{
					ET = CT_Get_ETpot_Turc(
						V.asDouble(fT),
						V.asDouble(fSR),
						V.asDouble(fRH)
					);
				}
				break;

			case  1: // Hargreaves
				if( !V.is_NoData(fTmin) && !V.is_NoData(fTmax) && !V.is_NoData(fDate) )
				{
					CSG_DateTime	Date(V.asString(fDate));

					ET = CT_Get_ETpot_Hargreave(
						V.asDouble(fT),
						V.asDouble(fTmin),
						V.asDouble(fTmax),
						Date.Get_DayOfYear(), Lat
					);
				}
				break;

			case  2: // Penman 1956 (simplified)
				if( !V.is_NoData(fSR) && !V.is_NoData(fRH) && !V.is_NoData(fWS) && !V.is_NoData(fDate) )
				{
					CSG_DateTime	Date(V.asString(fDate));

					ET = CT_Get_ETpot_Penman(
						V.asDouble(fT),
						V.asDouble(fSR),
						V.asDouble(fRH),
						V.asDouble(fWS),
						Date.Get_DayOfYear(), Lat
					);
				}
				break;
			}
		}

		if( ET < 0. )
		{
			V.Set_NoData(fET);
		}
		else
		{
			V.Set_Value(fET, ET);
		}
	}

	//-----------------------------------------------------
	DataObject_Update(pTable);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CETpot_Grid::CETpot_Grid(void)
{
	Set_Name		(_TL("Evapotranspiration (Grid)"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Estimation of daily potential evapotranspiration from weather data with different methods. "
		"Besides mean daily temperature it depends on the chosen method which additional data has to "
		"be provided. In order to estimate extraterrestrial net radiation some of the methods need "
		"to know the location's geographic latitude and the date's Julian day number. "
	));

	Add_Reference("Allen, R.G., Pereira, L.S., Raes, D., Smith, M.", "1998",
		"Crop evapotranspiration - Guidelines for computing crop water requirements.",
		"FAO Irrigation and drainage paper 56.",
		SG_T("http://www.fao.org/docrep/X0490E/x0490e07.htm#an%20alternative%20equation%20for%20eto%20when%20weather%20data%20are%20missing")
	);

	Add_Reference("Ambikadevi, K.M.", "2004",
		"Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts.",
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst.",
		SG_T("http://scholarworks.umass.edu/cee_ewre/22/")
	);

	Add_Reference("Hargreaves, G.H., Samani, Z.A.", "1985",
		"Reference crop evapotranspiration from ambient air temperatures.",
		"Paper presented in ASAE Regional Meeting, Grand Junction, Colorado.",
		SG_T("http://cagesun.nmsu.edu/~zsamani/papers/Hargreaves_Samani_85.pdf")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("", "T"    , _TL("Mean Temperature"   ), _TL(""), 10.);
	Parameters.Add_Grid_or_Const("", "T_MIN", _TL("Minimum Temperature"), _TL(""),  0.);
	Parameters.Add_Grid_or_Const("", "T_MAX", _TL("Maximum Temperature"), _TL(""), 20.);
	Parameters.Add_Grid_or_Const("", "RH"   , _TL("Relative Humidity"  ), _TL(""), 50., 0., true, 100., true);
	Parameters.Add_Grid_or_Const("", "SR"   , _TL("Solar Radiation"    ), _TL(""),  2., 0., true);
	Parameters.Add_Grid_or_Const("", "WS"   , _TL("Wind Speed"         ), _TL(""),  5., 0., true);

	Parameters.Add_Grid("",
		"ET"   , _TL("Potential Evapotranspiration"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"METHOD", _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Turc"),
			_TL("Hargreave"),
			_TL("Penman (simplified)")
		), 0
	);

	Parameters.Add_Double("",
		"LAT"	, _TL("Latitude [Degree]"),
		_TL(""),
		53., -90., true, 90., true
	);

	Parameters.Add_Choice("",
		"TIME"	, _TL("Time"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("day"),
			_TL("month")
		), 0
	);

	Parameters.Add_Choice("TIME",
		"MONTH"	, _TL("Month"),
		_TL(""),
		CSG_DateTime::Get_Month_Choices(), CSG_DateTime::Get_Current_Month()
	);

	Parameters.Add_Int("TIME",
		"DAY"	, _TL("Day of Month"),
		_TL(""),
		CSG_DateTime::Get_Current_Day(), 1, true, 31, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CETpot_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("T_MIN", pParameter->asInt() == 1);
		pParameters->Set_Enabled("T_MAX", pParameter->asInt() == 1);
		pParameters->Set_Enabled("RH"   , pParameter->asInt() != 1);
		pParameters->Set_Enabled("SR"   , pParameter->asInt() != 1);
		pParameters->Set_Enabled("WS"   , pParameter->asInt() == 2);
		pParameters->Set_Enabled("TIME" , pParameter->asInt() != 0);
		pParameters->Set_Enabled("LAT"  , pParameter->asInt() != 0);
	}

	if( pParameter->Cmp_Identifier("METHOD") || pParameter->Cmp_Identifier("T") )
	{
		pParameters->Set_Enabled("LAT", (*pParameters)("METHOD")->asInt() != 0 // Turc
			&& (*pParameters)("T")->asGrid() && !(*pParameters)("T")->asGrid()->Get_Projection().is_Okay()
		);
	}

	if( pParameter->Cmp_Identifier("TIME") )
	{
		pParameters->Set_Enabled("DAY", pParameter->asInt() == 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CETpot_Grid::On_Execute(void)
{
	CSG_Grid *pT    = Parameters("T"    )->asGrid(); double T    = Parameters("T"    )->asDouble();
	CSG_Grid *pTmin = Parameters("T_MIN")->asGrid(); double Tmin = Parameters("T_MIN")->asDouble();
	CSG_Grid *pTmax = Parameters("T_MAX")->asGrid(); double Tmax = Parameters("T_MAX")->asDouble();
	CSG_Grid *pRH   = Parameters("RH"   )->asGrid(); double RH   = Parameters("RH"   )->asDouble();
	CSG_Grid *pSR   = Parameters("SR"   )->asGrid(); double SR   = Parameters("SR"   )->asDouble();
	CSG_Grid *pWS   = Parameters("WS"   )->asGrid(); double WS   = Parameters("WS"   )->asDouble();
	CSG_Grid *pET   = Parameters("ET"   )->asGrid();

	int	Method	= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	CSG_Grid	Lat, *pLat	= NULL;

	if( Method != 0 && pT->Get_Projection().is_Okay() )
	{
		bool bResult; CSG_Grid Lon(Get_System()); Lat.Create(Get_System());

		SG_RUN_TOOL(bResult, "pj_proj4", 17,	// geographic coordinate grids
				SG_TOOL_PARAMETER_SET("GRID", pT)
			&&	SG_TOOL_PARAMETER_SET("LON" , &Lon)
			&&	SG_TOOL_PARAMETER_SET("LAT" , &Lat)
		)

		if( bResult )
		{
			pLat	= &Lat;
		}
	}

	//-----------------------------------------------------
	int		bDaily	= Parameters("TIME")->asInt() == 0;

	CSG_DateTime	Date(
		(CSG_DateTime::TSG_DateTime)(bDaily ? Parameters("DAY")->asInt() : 15),
		(CSG_DateTime::Month)Parameters("MONTH")->asInt()
	);

	int		Day		= Date.Get_DayOfYear();
	int		nDays	= Date.Get_NumberOfDays((CSG_DateTime::Month)Parameters("MONTH")->asInt());

	double	R0_const	= CT_Get_Radiation_Daily_TopOfAtmosphere(Day, Parameters("LAT")->asDouble());

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			double	ET	= -1.;

			if( !pT || !pT->is_NoData(x, y) )
			{
				switch( Method )
				{
				case  0: // Turc
					if( (!pRH || !pRH->is_NoData(x, y))
					&&  (!pSR || !pSR->is_NoData(x, y)) )
					{
						ET	= CT_Get_ETpot_Turc(
							pT  ? pT ->asDouble(x, y) : T ,
							pSR ? pSR->asDouble(x, y) : SR,
							pRH ? pRH->asDouble(x, y) : RH
						);
					}
					break;

				case  1: // Hargreaves
					if( (!pTmin || !pTmin->is_NoData(x, y))
					&&  (!pTmax || !pTmax->is_NoData(x, y)) )
					{
						ET	= CT_Get_ETpot_Hargreave(
							pT    ? pT   ->asDouble(x, y) : T   ,
							pTmin ? pTmin->asDouble(x, y) : Tmin,
							pTmax ? pTmax->asDouble(x, y) : Tmax,
							pLat ? CT_Get_Radiation_Daily_TopOfAtmosphere(Day, pLat->asDouble(x, y)) : R0_const
						);
					}
					break;

				case  2: // Penman 1956 (simplified)
					if( (!pSR || !pSR->is_NoData(x, y))
					&&  (!pRH || !pRH->is_NoData(x, y))
					&&  (!pWS || !pWS->is_NoData(x, y)) )
					{
						ET	= CT_Get_ETpot_Penman(
							pT  ? pT ->asDouble(x, y) : T ,
							pSR ? pSR->asDouble(x, y) : SR,
							pRH ? pRH->asDouble(x, y) : RH,
							pWS ? pWS->asDouble(x, y) : WS,
							pLat ? CT_Get_Radiation_Daily_TopOfAtmosphere(Day, pLat->asDouble(x, y)) : R0_const
						);
					}
					break;
				}
			}

			if( ET < 0. )
			{
				pET->Set_NoData(x, y);
			}
			else
			{
				pET->Set_Value(x, y, bDaily ? ET : ET * nDays);
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
