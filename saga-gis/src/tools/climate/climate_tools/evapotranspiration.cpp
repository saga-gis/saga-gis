
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
		SG_T("http://scholarworks.umass.edu/cee_ewre/22/"), SG_T("Link")
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
	Set_Name		(CSG_String::Format("%s (%s)", _TL("Evapotranspiration"), _TL("Table")));

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
		SG_T("http://www.fao.org/docrep/X0490E/x0490e00.htm#Contents"), SG_T("Link")
	);

	Add_Reference("Ambikadevi, K.M.", "2004",
		"Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts",
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst.",
		SG_T("http://scholarworks.umass.edu/cee_ewre/22/"), SG_T("Link")
	);

	Add_Reference("DVWK", "1996",
		"Ermittlung der Verdunstung von Land- u. Wasserflaechen",
		"Merkblaetter 238/1996."
	);

	Add_Reference("Hargreaves, G.H., Samani, Z.A.", "1985",
		"Reference crop evapotranspiration from ambient air temperatures",
		"Paper presented in ASAE Regional Meeting, Grand Junction, Colorado.",
		SG_T("https://elibrary.asabe.org/abstract.asp?aid=26773"), SG_T("Link")
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"	, _TL("Data"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("",
		"RESULT", _TL("Evapotranspiration"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table_Field("TABLE", "T"	   , _TL("Mean Temperature"   ), _TL("[Celsius]"));
	Parameters.Add_Table_Field("TABLE", "T_MIN", _TL("Minimum Temperature"), _TL("[Celsius]"));
	Parameters.Add_Table_Field("TABLE", "T_MAX", _TL("Maximum Temperature"), _TL("[Celsius]"));
	Parameters.Add_Table_Field("TABLE", "RH"   , _TL("Relative Humidity"  ), _TL("[Percent]"));
	Parameters.Add_Table_Field("TABLE", "SR"   , _TL("Solar Radiation"    ), _TL("daily sum of global radiation [J/cm^2]"));
	Parameters.Add_Table_Field("TABLE", "WS"   , _TL("Wind Speed"         ), _TL("daily mean of wind speed at 2m above ground [m/s]"));
	Parameters.Add_Table_Field("TABLE", "P"    , _TL("Air Pressure"       ), _TL("[kPa]"));
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
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Turc"),
			_TL("Hargreave"),
			_TL("Penman (simplified)"),
			_TL("Penman-Monteith")
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
		int	M	= pParameter->asInt();

		pParameters->Set_Enabled("T_MIN", M == 1 || M == 3);
		pParameters->Set_Enabled("T_MAX", M == 1 || M == 3);
		pParameters->Set_Enabled("RH"   , M != 1);
		pParameters->Set_Enabled("SR"   , M != 1);
		pParameters->Set_Enabled("WS"   , M == 2 || M == 3);
		pParameters->Set_Enabled("P"    , M == 3);
		pParameters->Set_Enabled("DATE" , M != 0 && M != 3);
		pParameters->Set_Enabled("LAT"  , M != 0 && M != 3);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CETpot_Table::On_Execute(void)
{
	CSG_Table	*pTable  = Parameters("TABLE")->asTable();

	if( Parameters("RESULT")->asTable() && Parameters("RESULT")->asTable() != pTable )
	{
		Parameters("RESULT")->asTable()->Create(*pTable);

		pTable	= Parameters("RESULT")->asTable();

		pTable->Fmt_Name("%s [%s]", pTable->Get_Name(), _TL("Evapotranspiration"));
	}

	//-----------------------------------------------------
	int	fET     = pTable->Get_Field_Count();

	pTable->Add_Field("ET", SG_DATATYPE_Double);

	int	Method  = Parameters("METHOD")->asInt();
	int	fT      = Parameters("T"     )->asInt();
	int	fTmin   = Parameters("T_MIN" )->asInt();
	int	fTmax   = Parameters("T_MAX" )->asInt();
	int	fRH     = Parameters("RH"    )->asInt();
	int	fSR     = Parameters("SR"    )->asInt();
	int	fWS     = Parameters("WS"    )->asInt();
	int	fP      = Parameters("P"     )->asInt();
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
						V.asDouble(fT   ),
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
						V.asDouble(fT ),
						V.asDouble(fSR),
						V.asDouble(fRH),
						V.asDouble(fWS),
						Date.Get_DayOfYear(), Lat
					);
				}

			case  3: // Penman-Monteith, FAO reference
				if( !V.is_NoData(fTmin) && !V.is_NoData(fTmax) && !V.is_NoData(fSR) && !V.is_NoData(fRH) && !V.is_NoData(fWS) )
				{
					ET = CT_Get_ETpot_FAORef(
						V.asDouble(fT   ),
						V.asDouble(fTmin),
						V.asDouble(fTmax),
						V.asDouble(fSR  ),
						V.asDouble(fRH  ),
						V.asDouble(fWS  ),
						V.asDouble(fP   )
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
	if( pTable != Parameters("TABLE")->asTable() )
	{
		DataObject_Update(pTable);
	}

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
	Set_Name		(CSG_String::Format("%s (%s)", _TL("Evapotranspiration"), _TL("Grid")));

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
		SG_T("http://www.fao.org/docrep/X0490E/x0490e07.htm#an%20alternative%20equation%20for%20eto%20when%20weather%20data%20are%20missing"), SG_T("Link")
	);

	Add_Reference("Ambikadevi, K.M.", "2004",
		"Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts.",
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst.",
		SG_T("http://scholarworks.umass.edu/cee_ewre/22/"), SG_T("Link")
	);

	Add_Reference("Hargreaves, G.H., Samani, Z.A.", "1985",
		"Reference crop evapotranspiration from ambient air temperatures.",
		"Paper presented in ASAE Regional Meeting, Grand Junction, Colorado.",
		SG_T("https://elibrary.asabe.org/abstract.asp?aid=26773"), SG_T("Link")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("", "T"    , _TL("Mean Temperature"   ), _TL("[Celsius]"),  10. , -273.15, true);
	Parameters.Add_Grid_or_Const("", "T_MIN", _TL("Minimum Temperature"), _TL("[Celsius]"),   0. , -273.15, true);
	Parameters.Add_Grid_or_Const("", "T_MAX", _TL("Maximum Temperature"), _TL("[Celsius]"),  20. , -273.15, true);
	Parameters.Add_Grid_or_Const("", "RH"   , _TL("Relative Humidity"  ), _TL("[Percent]"),  50. ,    0.  , true, 100., true);
	Parameters.Add_Grid_or_Const("", "SR"   , _TL("Solar Radiation"    ), _TL("[J/cm^2]" ),   2. ,    0.  , true);
	Parameters.Add_Grid_or_Const("", "WS"   , _TL("Wind Speed"         ), _TL("[m/s]"    ),   5. ,    0.  , true);
	Parameters.Add_Grid_or_Const("", "P"    , _TL("Air Pressure"       ), _TL("[kPa]"    ), 101.3,    0.  , true);

	Parameters.Add_Grid("",
		"ET"		, _TL("Potential Evapotranspiration"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Turc"),
			_TL("Hargreave"),
			_TL("Penman (simplified)"),
			_TL("Penman-Monteith")
		), 0
	);

	Parameters.Add_Bool("",
		"SR_EST"	, _TL("Estimate Solar Radiation"),
		_TL("Estimate solar radiation from date, latitudinal position and sunshine duration as percentage of its potential maximum."),
		false
	);

	Parameters.Add_Double("SR_EST",
		"SUNSHINE"	, _TL("Sunshine Duration"),
		_TL("Daily sunshine duration as percentage of its potential maximum."),
		50., 0., true, 100., true
	);

	Parameters.Add_Choice("",
		"TIME"		, _TL("Time"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("day"),
			_TL("month")
		), 0
	);

	Parameters.Add_Choice("TIME",
		"MONTH"		, _TL("Month"),
		_TL(""),
		CSG_DateTime::Get_Month_Choices(), CSG_DateTime::Get_Current_Month()
	);

	Parameters.Add_Int("TIME",
		"DAY"		, _TL("Day of Month"),
		_TL(""),
		CSG_DateTime::Get_Current_Day(), 1, true, 31, true
	);

	Parameters.Add_Double("",
		"LAT"		, _TL("Latitude"),
		_TL("[Degree]"),
		53., -90., true, 90., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CETpot_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	int       Method  = (*pParameters)["METHOD"].asInt ();
	bool      bCalcSR = (*pParameters)["SR_EST"].asBool();
	CSG_Grid *pT      = (*pParameters)["T"     ].asGrid();

	pParameters->Set_Enabled("T_MIN"   , Method == 1 || Method == 3);
	pParameters->Set_Enabled("T_MAX"   , Method == 1 || Method == 3);
	pParameters->Set_Enabled("RH"      , Method != 1);
	pParameters->Set_Enabled("SR"      , Method != 1 && !bCalcSR);
	pParameters->Set_Enabled("SR_EST"  , Method != 1);
	pParameters->Set_Enabled("SR"      , bCalcSR == false);
	pParameters->Set_Enabled("SUNSHINE", bCalcSR ==  true);
	pParameters->Set_Enabled("WS"      , Method == 2 || Method == 3);
	pParameters->Set_Enabled("P"       , Method == 3);
	pParameters->Set_Enabled("TIME"    , Method == 1 || Method == 2 || bCalcSR);
	pParameters->Set_Enabled("DAY"     , (*pParameters)["TIME"].asInt() == 0);
	pParameters->Set_Enabled("LAT"     , (Method == 1 || Method == 2 || bCalcSR) && !(pT && pT->Get_Projection().is_Okay()));

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CETpot_Grid::On_Execute(void)
{
	CSG_Grid *pT    = Parameters("T"    )->asGrid(); double const_T    = Parameters("T"    )->asDouble();
	CSG_Grid *pTmin = Parameters("T_MIN")->asGrid(); double const_Tmin = Parameters("T_MIN")->asDouble();
	CSG_Grid *pTmax = Parameters("T_MAX")->asGrid(); double const_Tmax = Parameters("T_MAX")->asDouble();
	CSG_Grid *pRH   = Parameters("RH"   )->asGrid(); double const_RH   = Parameters("RH"   )->asDouble();
	CSG_Grid *pSR   = Parameters("SR"   )->asGrid(); double const_SR   = Parameters("SR"   )->asDouble();
	CSG_Grid *pWS   = Parameters("WS"   )->asGrid(); double const_WS   = Parameters("WS"   )->asDouble();
	CSG_Grid *pP    = Parameters("P"    )->asGrid(); double const_P    = Parameters("P"    )->asDouble();
	CSG_Grid *pET   = Parameters("ET"   )->asGrid();

	//-----------------------------------------------------
	int	Method	= Parameters("METHOD")->asInt();

	pET->Fmt_Name("%s [%s]", _TL("Potential Evapotranspiration"), Parameters("METHOD")->asString());

	//-----------------------------------------------------
	int		bDaily	= Parameters("TIME")->asInt() == 0;

	CSG_DateTime	Date(
		(CSG_DateTime::TSG_DateTime)(bDaily ? Parameters("DAY")->asInt() : 15),
		(CSG_DateTime::Month)Parameters("MONTH")->asInt()
	);

	int Day   = Date.Get_DayOfYear();
	int nDays = Date.Get_NumberOfDays((CSG_DateTime::Month)Parameters("MONTH")->asInt());

	//-----------------------------------------------------
	CSG_Grid SR, Lat, *pLat = NULL; double const_Lat = Parameters("LAT")->asDouble();

	if( (Parameters("SR_EST")->asBool() || Method == 1 || Method == 2) && pT->Get_Projection().is_Okay() )
	{
		bool bResult; CSG_Grid Lon(Get_System()); Lat.Create(Get_System());

		SG_RUN_TOOL(bResult, "pj_proj4", 17,	// geographic coordinate grids
				SG_TOOL_PARAMETER_SET("GRID", pT  )
			&&	SG_TOOL_PARAMETER_SET("LON" , &Lon)
			&&	SG_TOOL_PARAMETER_SET("LAT" , &Lat)
		)

		if( bResult )
		{
			pLat	= &Lat;

			if( Parameters("SR_EST")->asBool() )
			{
				double	Sunshine	= Parameters("SUNSHINE")->asDouble() / 100.;

				pSR	= &SR; pSR->Create(Get_System());

				#pragma omp parallel for
				for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
				{
					double SR = 100. * CT_Get_Radiation_Daily_TopOfAtmosphere(Day, pLat->asDouble(x, y), false); // top of atmosphere radiation: 100 * [MJ/m2] >> [J/cm2]

					SR *= 0.19 + 0.55 * Sunshine; // ToA radiation >> global radiation

					pSR->Set_Value(x, y, SR);
				}
			}
		}
	}

	//-----------------------------------------------------
	double const_R0 = CT_Get_Radiation_Daily_TopOfAtmosphere(Day, const_Lat, false); // [MJ/m2/day]

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			double	ET	= -1.;

			#define HAVE(pGrid) (!pGrid || !pGrid->is_NoData(x, y))

			if( HAVE(pT) )
			{
				#define GET_T    (pT    ? pT   ->asDouble(x, y) : const_T   )
				#define GET_Tmin (pTmin ? pTmin->asDouble(x, y) : const_Tmin)
				#define GET_Tmax (pTmax ? pTmax->asDouble(x, y) : const_Tmax)
				#define GET_RH   (pRH   ? pRH  ->asDouble(x, y) : const_RH  )
				#define GET_WS   (pWS   ? pWS  ->asDouble(x, y) : const_WS  )
				#define GET_P    (pP    ? pP   ->asDouble(x, y) : const_P   )
				#define GET_SR   (pSR   ? pSR  ->asDouble(x, y) : const_SR  )
				#define GET_R0   (pLat  ? CT_Get_Radiation_Daily_TopOfAtmosphere(Day, pLat->asDouble(x, y), false) : const_R0) // [MJ/m^2/day]

				switch( Method )
				{
				case  0: // Turc
					if( HAVE(pRH) && HAVE(pSR) )
					{
						ET	= CT_Get_ETpot_Turc(GET_T, GET_SR, GET_RH);
					}
					break;

				case  1: // Hargreaves
					if( HAVE(pTmin) && HAVE(pTmax) )
					{
						ET	= CT_Get_ETpot_Hargreave(GET_T, GET_Tmin, GET_Tmax, GET_R0);
					}
					break;

				case  2: // Penman 1956 (simplified)
					if( HAVE(pSR) && HAVE(pRH) && HAVE(pWS) )
					{
						ET	= CT_Get_ETpot_Penman(GET_T, GET_SR, GET_RH, GET_WS, Day, pLat ? pLat->asDouble(x, y) : const_Lat);
					}
					break;

				case  3: // Penman-Monteith, FAO grass reference
					if( HAVE(pTmin) && HAVE(pTmax) && HAVE(pSR) && HAVE(pRH) && HAVE(pWS) && HAVE(pP) )
					{
						ET	= CT_Get_ETpot_FAORef(GET_T, GET_Tmin, GET_Tmax, GET_SR, GET_RH, GET_WS, GET_P);
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
