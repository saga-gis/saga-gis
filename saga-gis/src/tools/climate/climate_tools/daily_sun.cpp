
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
//                     daily_sun.cpp                     //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#include "daily_sun.h"
#include "climate_tools.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double SG_Range_Set_0_to_24(double Value)
{
	Value = fmod(Value, 24.);

	return( Value < 0. ? Value	+ 24. : Value );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDaily_Sun::CDaily_Sun(void)
{
	Set_Name		(_TL("Sunrise and Sunset"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"This tool calculates the time of sunrise and sunset and the resulting day length "
		"for each cell of the target grid. The target grid needs to provide information "
		"about its coordinate system. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("", "TARGET" , _TL("Target System"), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("", "SUNRISE", _TL("Sunrise"      ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "SUNSET" , _TL("Sunset"       ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "LENGTH" , _TL("Day Length"   ), _TL(""), PARAMETER_OUTPUT);

	//-----------------------------------------------------
	Parameters.Add_Date(
		"", "DAY"	, _TL("Day of Month"),
		_TL(""),
		CSG_DateTime::Now().Get_JDN()
	);

	Parameters.Add_Choice(
		"", "TIME"	, _TL("Time"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("local"),
			_TL("world")
		)
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDaily_Sun::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDaily_Sun::On_Execute(void)
{
	CSG_Grid *pTarget = Parameters("TARGET")->asGrid();

	if( !pTarget->Get_Projection().is_Okay() )
	{
		Error_Set(_TL("target's spatial reference system is undefined"));

		return( false );
	}

	CSG_Grid Lon(Get_System()), Lat(Get_System());

	SG_RUN_TOOL_ExitOnError("pj_proj4", 17,	// geographic coordinate grids
			SG_TOOL_PARAMETER_SET("GRID", pTarget)
		&&	SG_TOOL_PARAMETER_SET("LON" , &Lon)
		&&	SG_TOOL_PARAMETER_SET("LAT" , &Lat)
	)

	Lon *= M_DEG_TO_RAD;
	Lat *= M_DEG_TO_RAD;

	//-----------------------------------------------------
	CSG_Grid *pSunset   = Parameters("SUNSET" )->asGrid();
	CSG_Grid *pSunrise  = Parameters("SUNRISE")->asGrid();
	CSG_Grid *pDuration = Parameters("LENGTH" )->asGrid();

	bool bWorld = Parameters("TIME")->asInt() == 1;

	//-----------------------------------------------------
	CSG_DateTime Time(Parameters("DAY")->asDate()->Get_Date());

	Time.Reset_Time();

	Message_Add(Time.Format("\n%A, %d. %B %Y"), false);

	//-----------------------------------------------------
	double Dec, RA, RAm, T;

	SG_Get_Sun_Position(Time, RA, Dec);

	T   = ((int)Time.Get_JDN() - 2451545. ) / 36525.;	// Number of Julian centuries since 2000/01/01 at 12 UT (JDN = 2451545.)

	RAm = fmod(18.71506921 + 2400.0513369 * T + (2.5862e-5 - 1.72e-9 * T) * T*T, 24.);
	RA  = fmod(RA * 12. / M_PI, 24.);	if( RA < 0. ) RA += 24.;

	T   = 1.0027379 * (RAm - RA);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pTarget->is_NoData(x, y) )
			{
				pSunrise ->Set_NoData(x, y);
				pSunset  ->Set_NoData(x, y);
				pDuration->Set_NoData(x, y);
			}
			else
			{
				double dT = (sin(-Lat.asDouble(x, y) / 60.) - sin(Lat.asDouble(x, y)) * sin(Dec)) / (cos(Lat.asDouble(x, y)) * cos(Dec));

				if( dT > 1. )
				{
					pSunrise ->Set_NoData(x, y);
					pSunset  ->Set_NoData(x, y);
					pDuration->Set_Value (x, y,  0.);
				}
				else if( dT < -1. )
				{
					pSunrise ->Set_NoData(x, y);
					pSunset  ->Set_NoData(x, y);
					pDuration->Set_Value (x, y, 24.);
				}
				else
				{
					dT = acos(dT) * 12. / M_PI;

					double Sunrise = SG_Range_Set_0_to_24(12. - dT - T);
					double Sunset  = SG_Range_Set_0_to_24(12. + dT - T);

					pDuration->Set_Value(x, y, Sunset - Sunrise);

					if( bWorld )
					{
						Sunrise = SG_Range_Set_0_to_24(Sunrise - M_RAD_TO_DEG * Lon.asDouble(x, y) / 15.);
						Sunset  = SG_Range_Set_0_to_24(Sunset  - M_RAD_TO_DEG * Lon.asDouble(x, y) / 15.);
					}

					pSunrise->Set_Value(x, y, Sunrise);
					pSunset ->Set_Value(x, y, Sunset );
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSolarRadiation::CSolarRadiation(void)
{
	Set_Name		(_TL("Daily Solar Radiation"));

	Set_Author		("O.Conrad (c) 2021");

	Set_Description	(_TW(
		"This tool calculates the daily solar radiation (Rg) based on "
		"the date and the latitudinal position for incoming top of "
		"atmosphere radiation (R0) estimation and the sunshine duration (Sd) "
		"provided as percentage of its potential maximum (S0). It uses "
		"a simple empiric formula:\n"
		"Rg = R0 * (0.19 + 0.55 * Sd/S0)"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("", "LATITUDE", _TL("Latitude"       ), _TL("[Degree]"), PARAMETER_INPUT );
	Parameters.Add_Grid("", "SOLARRAD", _TL("Solar Radiation"), _TL("[J/cm²]" ), PARAMETER_OUTPUT);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"MONTH"   , _TL("Month"),
		_TL(""),
		CSG_DateTime::Get_Month_Choices(), CSG_DateTime::Get_Current_Month()
	);

	Parameters.Add_Int("TIME",
		"DAY"     , _TL("Day of Month"),
		_TL(""),
		CSG_DateTime::Get_Current_Day(), 1, true, 31, true
	);

	Parameters.Add_Double("",
		"SUNSHINE", _TL("Sunshine Duration"),
		_TL("Daily sunshine duration as percentage of its potential maximum."),
		50., 0., true, 100., true
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSolarRadiation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::On_Execute(void)
{
	CSG_Grid *pLat = Parameters("LATITUDE")->asGrid();
	CSG_Grid *pSR  = Parameters("SOLARRAD")->asGrid();

	pSR->Fmt_Name("%s [%s, %d]", _TL("Solar Radiation"), Parameters("MONTH")->asString(), Parameters("DAY")->asInt());
	pSR->Set_Unit(SG_T("J/cm²"));

	CSG_DateTime Date(
		(CSG_DateTime::TSG_DateTime)Parameters("DAY"  )->asInt(),
		(CSG_DateTime::Month       )Parameters("MONTH")->asInt()
	);

	int    DayOfYear = Date.Get_DayOfYear();

	double Sunshine  = Parameters("SUNSHINE")->asDouble() / 100.;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double SR = 100. * CT_Get_Radiation_Daily_TopOfAtmosphere(DayOfYear, pLat->asDouble(x, y)); // top of atmosphere radiation: 100 * [MJ/m2] >> [J/cm2]

			SR *= 0.19 + 0.55 * Sunshine; // ToA radiation >> global radiation

			pSR->Set_Value(x, y, SR);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSolarPosition::CSolarPosition(void)
{
	Set_Name		(_TL("Solar Position"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"This tool calculates the solar position for requested "
		"latitude, date range and daily time range."
	));

	//-----------------------------------------------------
	Parameters.Add_Table("", "TABLE", _TL("Solar Position"), _TL(""), PARAMETER_OUTPUT);

//	Parameters.Add_Double("", "LATITUDE", _TL("Latitude"), _TL(""), 53., -90., true, 90., true);
	Parameters.Add_Degree("", "LATITUDE", _TL("Latitude"), _TL(""), 53., -90., true, 90., true);

	Parameters.Add_Node  ("", "DATE", _TL("Date"), _TL(""));
	Parameters.Add_Date  ("DATE", "DATE_FROM", _TL("From"), _TL(""), CSG_DateTime("2000-01-01").Get_JDN());
	Parameters.Add_Date  ("DATE", "DATE_TO"  , _TL("To"  ), _TL(""), CSG_DateTime("2000-12-31").Get_JDN());
	Parameters.Add_Int   ("DATE", "DATE_STEP", _TL("Step"), _TL(""), 5, 0, true);

	Parameters.Add_Node  ("", "TIME", _TL("Time"), _TL("hour"));
	Parameters.Add_Double("TIME", "TIME_FROM", _TL("From"), _TL(""),  1.);
	Parameters.Add_Double("TIME", "TIME_TO"  , _TL("To"  ), _TL(""), 24.);
	Parameters.Add_Double("TIME", "TIME_STEP", _TL("Step"), _TL(""),  1., 0., true);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSolarPosition::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("DATE_STEP") )
	{
		pParameters->Set_Enabled("DATE_TO", pParameter->asInt() > 0);
	}

	if( pParameter->Cmp_Identifier("TIME_STEP") )
	{
		pParameters->Set_Enabled("TIME_TO", pParameter->asDouble() > 0.);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarPosition::On_Execute(void)
{
	CSG_Table &Table = *Parameters("TABLE")->asTable(); Table.Destroy();
	Table.Set_Name(_TL("Solar Position"));
	Table.Add_Field(_TL("JDN"           ), SG_DATATYPE_Double);
	Table.Add_Field(_TL("Date"          ), SG_DATATYPE_Date  );
	Table.Add_Field(_TL("Time"          ), SG_DATATYPE_String);
	Table.Add_Field(_TL("Azimuth"       ), SG_DATATYPE_Double);
	Table.Add_Field(_TL("Height"        ), SG_DATATYPE_Double);
	Table.Add_Field(_TL("Refraction"    ), SG_DATATYPE_Double);
	Table.Add_Field(_TL("Visible Height"), SG_DATATYPE_Double);

	//-----------------------------------------------------
	double Date_Start = Parameters("DATE_FROM")->asDouble();
	double Date_Stop  = Parameters("DATE_TO"  )->asDouble();
	double Date_Step  = Parameters("DATE_STEP")->asDouble();

	if( Date_Stop < Date_Start || Date_Step <= 0. )
	{
		Date_Stop = Date_Start; Date_Step = 1.;
	}

	//-----------------------------------------------------
	double Time_Start = Parameters("TIME_FROM")->asDouble();
	double Time_Stop  = Parameters("TIME_TO"  )->asDouble();
	double Time_Step  = Parameters("TIME_STEP")->asDouble();

	if( Time_Stop < Time_Start || Time_Step <= 0. )
	{
		Time_Stop = Time_Start; Time_Step = 1.;
	}

	double Latitude = Parameters("LATITUDE")->asDouble() * M_DEG_TO_RAD, Height, Azimuth;

	//-----------------------------------------------------
	for(double Date=Date_Start; Date<=Date_Stop; Date+=Date_Step)
	{
		CSG_String ISODate(CSG_DateTime(Date).Format_ISODate());

		for(double Time=Time_Start; Time<=Time_Stop; Time+=Time_Step)
		{
			Process_Set_Text("%s: %s %02d%02.2f", _TL("Time"), ISODate.c_str(), (int)Time, (Time - (int)Time) * 60.);

			double JDN = ((int)Date) + (Time - 12.) / 24.;

			SG_Get_Sun_Position(JDN, 0., Latitude, Height, Azimuth);

			if( 1 )
			{
				Azimuth = fmod(Azimuth + M_PI_360, M_PI_360);
			}

			CSG_Table_Record &Record = *Table.Add_Record();

			Record.Set_Value(0, JDN);
			Record.Set_Value(1, ISODate);
			Record.Set_Value(2, CSG_String::Format("%02d:%02d", (int)Time, (int)((Time - (int)Time) * 60.)));
			Record.Set_Value(3, M_RAD_TO_DEG * Azimuth);
			Record.Set_Value(4, M_RAD_TO_DEG * Height);

			double Refraction = SG_Get_Sun_Refraction(Height, true);

			if( Refraction < 0. )
			{
				Record.Set_NoData(5); Record.Set_Value(6, 0.);
			}
			else
			{
				Record.Set_Value(5, M_RAD_TO_DEG * Refraction);
				Record.Set_Value(6, M_RAD_TO_DEG * (Height + Refraction));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
