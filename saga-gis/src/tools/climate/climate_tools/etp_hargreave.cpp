/**********************************************************
 * Version $Id: etp_hargreave.cpp 911 2011-11-11 11:11:11Z oconrad $
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
//                   etp_hargreave.cpp                   //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#include "etp_hargreave.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double	Get_Radiation_TopOfAtmosphere	(int DayOfYear, double Latitude_Rad)
{
	double	sinLat	= sin(Latitude_Rad);
	double	cosLat	= cos(Latitude_Rad);
	double	tanLat	= tan(Latitude_Rad);

	double	JD		= DayOfYear * M_PI * 2.0 / 365.0;

	double	dR		= 0.033  * cos(JD) + 1.0;	// relative distance between sun and earth on any Julian day

	double	SunDec	= 0.4093 * sin(JD - 1.405);	// solar declination

	double	d		= -tanLat * tan(SunDec);	// sunset hour angle
	double	SunSet	= acos(d < -1 ? -1 : d < 1 ? d : 1);

	double	R0		= 15.392 * dR * (SunSet * sinLat * sin(SunDec) + cosLat * cos(SunDec) * sin(SunSet));

	return( R0 );	// water equivalent of extraterrestrial radiation (mm/day)
}

//---------------------------------------------------------
double	Get_PET_Hargreave	(double R0, double Tmean, double Tmin, double Tmax)
{
	double	ETpot	= 0.0023 * R0 * (Tmean + 17.8) * sqrt(Tmax - Tmin);	// reference crop evapotranspiration mm per day

	return( ETpot > 0.0 ? ETpot : 0.0 );
}

//---------------------------------------------------------
double	Get_PET_Hargreave	(int DayOfYear, double Latitude_Rad, double Tmean, double Tmin, double Tmax)
{
	double	R0	= Get_Radiation_TopOfAtmosphere(DayOfYear, Latitude_Rad);

	return( Get_PET_Hargreave(R0, Tmean, Tmin, Tmax) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPET_Hargreave_Grid::CPET_Hargreave_Grid(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("PET (after Hargreaves, Grid)"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Estimation of daily potential evapotranspiration from daily average, minimum and maximum temperatures "
		"using Hargreave's empirical equation. In order to estimate extraterrestrial net radiation "
		"geographic latitude of observation and Julian day have to be supplied too. "
		"\nReferences:\n"
		"- Ambikadevi, K.M. (2004): Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts. "
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst "
		"<a target=\"_blank\" href=\"http://scholarworks.umass.edu/cee_ewre/22/\">online</a>\n"
		"- Hargraeves, G.H., Samani, Z.A. (1985): Reference crop evapotranspiration from ambient air temperatures. "
		"Paper presented in ASAE Regional Meeting, Grand Junction, Colorado. "
		"<a target=\"_blank\" href=\"http://cagesun.nmsu.edu/~zsamani/papers/Hargreaves_Samani_85.pdf\">online</a>\n"
		"Allen, R.G., Pereira, L.S., Raes, D., Smith, M. (1998): Crop evapotranspiration - Guidelines for computing crop water requirements. "
		"FAO Irrigation and drainage paper 56. "
		"<a target=\"_blank\" href=\"http://www.fao.org/docrep/X0490E/x0490e00.htm#Contents\">online</a>\n"
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(NULL, "T"    , _TL("Mean Temperature"            ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "T_MIN", _TL("Minimum Temperature"         ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "T_MAX", _TL("Maximum Temperature"         ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "PET"  , _TL("Potential Evapotranspiration"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Value(
		NULL	, "LAT"		, _TL("Latitude [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double, 53.0, -90.0, true, 90.0, true
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "TIME"	, _TL("Time"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("day"),
			_TL("month")
		), 0
	);

	Parameters.Add_Choice(
		pNode	, "MONTH"	, _TL("Month"),
		_TL(""),
		CSG_DateTime::Get_Month_Choices(), CSG_DateTime::Get_Current_Month()
	);

	Parameters.Add_Value(
		pNode	, "DAY"		, _TL("Day of Month"),
		_TL(""),
		PARAMETER_TYPE_Int, CSG_DateTime::Get_Current_Day(), 1, true, 31, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPET_Hargreave_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "T") )
	{
		pParameters->Set_Enabled("LAT", pParameter->asGrid() && pParameter->asGrid()->Get_Projection().is_Okay() == false);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TIME") )
	{
		pParameters->Set_Enabled("DAY", pParameter->asInt() == 0);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPET_Hargreave_Grid::On_Execute(void)
{
	//-----------------------------------------------------
//	const int	DaysBefore[12]	= {   0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334 };
//	const int	DaysCount [12]	= {  31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };

	//-----------------------------------------------------
	CSG_Grid	*pTavg	= Parameters("T"    )->asGrid();
	CSG_Grid	*pTmin	= Parameters("T_MIN")->asGrid();
	CSG_Grid	*pTmax	= Parameters("T_MAX")->asGrid();
	CSG_Grid	*pPET	= Parameters("PET"  )->asGrid();

	//-----------------------------------------------------
	CSG_Grid	Lat, *pLat	= NULL;

	if( pTavg->Get_Projection().is_Okay() )
	{
		bool	bResult;

		Lat.Create(*Get_System());

		SG_RUN_TOOL(bResult, "pj_proj4", 17,	// geographic coordinate grids
				SG_TOOL_PARAMETER_SET("GRID", pTavg)
			&&	SG_TOOL_PARAMETER_SET("LAT" , &Lat)
		)

		if( bResult )
		{
			pLat	= &Lat;	pLat->Set_Scaling(M_DEG_TO_RAD);
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

	double	R0_const	= Get_Radiation_TopOfAtmosphere(Day, Parameters("LAT")->asDouble() * M_DEG_TO_RAD);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
#ifndef _DEBUG
		#pragma omp parallel for
#endif
		for(int x=0; x<Get_NX(); x++)
		{
			if( pTavg->is_NoData(x, y) || pTmin->is_NoData(x, y) || pTmax->is_NoData(x, y) || (pLat && pLat->is_NoData(x, y)) )
			{
				pPET->Set_NoData(x, y);
			}
			else
			{
				double	PET	= Get_PET_Hargreave(pLat ? Get_Radiation_TopOfAtmosphere(Day, pLat->asDouble(x, y)) : R0_const,
					pTavg->asDouble(x, y),
					pTmin->asDouble(x, y),
					pTmax->asDouble(x, y)
				);

				pPET->Set_Value(x, y, bDaily ? PET : PET * nDays);
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
CPET_Hargreave_Table::CPET_Hargreave_Table(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("PET (after Hargreaves, Table)"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Estimation of daily potential evapotranspiration from daily average, minimum and maximum temperatures "
		"using Hargreave's empirical equation. In order to estimate extraterrestrial net radiation "
		"geographic latitude of observation and Julian day have to be supplied too. "
		"\nReferences:\n"
		"- Ambikadevi, K.M. (2004): Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts. "
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst "
		"<a target=\"_blank\" href=\"http://scholarworks.umass.edu/cee_ewre/22/\">online</a>\n"
		"- Hargraeves, G.H., Samani, Z.A. (1985): Reference crop evapotranspiration from ambient air temperatures. "
		"Paper presented in ASAE Regional Meeting, Grand Junction, Colorado. "
		"<a target=\"_blank\" href=\"http://cagesun.nmsu.edu/~zsamani/papers/Hargreaves_Samani_85.pdf\">online</a>\n"
		"FAO Irrigation and drainage paper 56. "
		"<a target=\"_blank\" href=\"http://www.fao.org/docrep/X0490E/x0490e00.htm#Contents\">online</a>\n"
	));


	//-----------------------------------------------------
	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"			, _TL("Data"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "JD"				, _TL("Julian Day"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "T"				, _TL("Mean Temperature"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "T_MIN"			, _TL("Minimum Temperature"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "T_MAX"			, _TL("Maximum Temperature"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "LAT"				, _TL("Latitude"),
		_TL(""),
		PARAMETER_TYPE_Double, 53.0, -90.0, true, 90.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPET_Hargreave_Table::On_Execute(void)
{
	//-----------------------------------------------------
	int			fDay, fT, fTmin, fTmax, fET;
	double		Lat;
	CSG_Table	*pTable;

	pTable	= Parameters("TABLE")->asTable ();
	Lat		= Parameters("LAT"  )->asDouble() * M_DEG_TO_RAD;
	fDay	= Parameters("JD"   )->asInt   ();
	fT		= Parameters("T"    )->asInt   ();
	fTmin	= Parameters("T_MIN")->asInt   ();
	fTmax	= Parameters("T_MAX")->asInt   ();
	fET		= pTable->Get_Field_Count();

	pTable->Add_Field("ET", SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		if( pRecord->is_NoData(fDay) || pRecord->is_NoData(fTmin) || pRecord->is_NoData(fTmax) )
		{
			pRecord->Set_NoData(fET);
		}
		else
		{
			pRecord->Set_Value(fET, Get_PET_Hargreave(Get_Radiation_TopOfAtmosphere(
				pRecord->asInt   (fDay ), Lat),
				pRecord->asDouble(fT   ),
				pRecord->asDouble(fTmin),
				pRecord->asDouble(fTmax))
			);
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
CPET_Day_To_Hour::CPET_Day_To_Hour(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Daily to Hourly PET"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Derive hourly from daily evapotranspiration using sinusoidal distribution. "
		"\nReferences:\n"
		"- Ambikadevi, K.M. (2004): Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts. "
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst "
		"<a target=\"_blank\" href=\"http://scholarworks.umass.edu/cee_ewre/22/\">online</a>\n"
	));


	//-----------------------------------------------------
	pNode	= Parameters.Add_Table(
		NULL	, "DAYS"			, _TL("Daily Data"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "JD"				, _TL("Julian Day"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "ET"				, _TL("Evapotranspiration"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "P"				, _TL("Precipitation"),
		_TL(""),
		true
	);

	Parameters.Add_Table(
		NULL	, "HOURS"			, _TL("Hourly Data"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		pNode	, "LAT"				, _TL("Latitude"),
		_TL(""),
		PARAMETER_TYPE_Double, 53.0, -90.0, true, 90.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPET_Day_To_Hour::On_Execute(void)
{
	int			fJD, fET, fP;
	double		sinLat, cosLat, sinHgt;
	CSG_Table	*pDays, *pHours;

	//-----------------------------------------------------
	pDays	= Parameters("DAYS" )->asTable();
	pHours	= Parameters("HOURS")->asTable();
	fJD		= Parameters("JD"   )->asInt();
	fET		= Parameters("ET"   )->asInt();
	fP		= Parameters("P"    )->asInt();

	sinLat	= sin(Parameters("LAT")	->asDouble() * M_DEG_TO_RAD);
	cosLat	= cos(Parameters("LAT")	->asDouble() * M_DEG_TO_RAD);
	sinHgt	= 0.0;	// -0.0145;	// >> -50'' desired height of horizon

	pHours->Destroy();
	pHours->Set_Name(CSG_String::Format("%s [%s]", pDays->Get_Name(), _TL("h")));
	pHours->Add_Field("JULIAN_DAY", SG_DATATYPE_Int);
	pHours->Add_Field("HOUR"      , SG_DATATYPE_Int);
	pHours->Add_Field("ET"        , SG_DATATYPE_Double);

	if( fP >= 0 )
	{
		pHours->Add_Field("P", SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(int iDay=0; iDay<pDays->Get_Count() && Set_Progress(iDay, pDays->Get_Count()); iDay++)
	{
		CSG_Table_Record	*pDay	= pDays->Get_Record(iDay);

		int		JD;
		double	P, ET, D, dT, fT, sRise, sSet;

		JD		= pDay->asInt   (fJD);
		ET		= pDay->asDouble(fET);

		if( fP >= 0 )
		{
			P	= pDay->asDouble(fP );
		}

		D		= 0.40954 * sin(0.0172 * (JD - 79.349740));	// sun's declination
		dT		= 12.0 * acos((sinHgt - sinLat * sin(D)) / (cosLat * cos(D))) / M_PI;

		fT		= -0.1752 * sin(0.033430 * JD + 0.5474) - 0.1340 * sin(0.018234 * JD - 0.1939);
		sRise	= 12.0 - dT - fT;
		sSet	= 12.0 + dT - fT;

		for(int iHour=0; iHour<24; iHour++)
		{
			CSG_Table_Record	*pHour	= pHours->Add_Record();

			pHour->Set_Value(0, JD);
			pHour->Set_Value(1, iHour);

			if( fP >= 0 )
			{
				pHour->Set_Value(3, P / 24.0);
			}

			if( sRise <= iHour && iHour <= sSet )
			{
				pHour->Set_Value(2, ET * (1.0 - cos(2.0 * M_PI * (iHour - sRise) / (sSet - sRise))) / 2.0);
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
