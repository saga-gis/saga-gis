/**********************************************************
 * Version $Id: etp_hargreave.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
CETP_Hargreave::CETP_Hargreave(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("PET (after Hargreave)"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

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
	));


	//-----------------------------------------------------
	// 2. Parameters...

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
		pNode	, "T"				, _TL("Average Temperature"),
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
bool CETP_Hargreave::On_Execute(void)
{
	int			fJD, fT, fTmin, fTmax, fET;
	double		sinLat, cosLat, tanLat;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable	= Parameters("TABLE")	->asTable();
	fJD		= Parameters("JD")		->asInt();
	fT		= Parameters("T")		->asInt();
	fTmin	= Parameters("T_MIN")	->asInt();
	fTmax	= Parameters("T_MAX")	->asInt();

	sinLat	= sin(Parameters("LAT")	->asDouble() * M_DEG_TO_RAD);
	cosLat	= cos(Parameters("LAT")	->asDouble() * M_DEG_TO_RAD);
	tanLat	= tan(Parameters("LAT")	->asDouble() * M_DEG_TO_RAD);

	fET		= pTable->Get_Field_Count();

	pTable->Add_Field(SG_T("ET"), SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		if( pRecord->is_NoData(fJD) || pRecord->is_NoData(fTmin) || pRecord->is_NoData(fTmax) )
		{
			pRecord->Set_NoData(fET);
		}
		else
		{
			int		JD;
			double	T, Tmin, Tmax, dR, SunDir, SunHgt, S0, ET;

			JD		= pRecord->asInt   (fJD  );
			T		= pRecord->asDouble(fT   );
			Tmin	= pRecord->asDouble(fTmin);
			Tmax	= pRecord->asDouble(fTmax);

			// relative distance between sun and earth on any Julian day
			dR		= 1 + 0.033 * cos(JD * 2.0 * M_PI / 365.0);

			// solar declination in radians
			SunHgt	= 0.4093    * sin(JD * 2.0 * M_PI / 365.0 - 1.405);

			// sunset hour angle
			SunDir	= acos(-tanLat * tan(SunHgt));

			// water equivalent of extraterrestrial radiation (mm/day)
			S0		= 15.392 * dR * (SunDir * sinLat * sin(SunHgt) + cosLat * cos(SunHgt) * sin(SunDir));

			ET		= 0.0023 * S0 * sqrt(Tmax - Tmin) * (T + 17.8);

			pRecord->Set_Value(fET, ET);
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
CETP_Day_To_Hour::CETP_Day_To_Hour(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Daily to Hourly PET"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Derive hourly from daily evapotranspiration using sinusoidal distribution. "
		"\nReferences:\n"
		"- Ambikadevi, K.M. (2004): Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts. "
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst "
		"<a target=\"_blank\" href=\"http://scholarworks.umass.edu/cee_ewre/22/\">online</a>\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

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
bool CETP_Day_To_Hour::On_Execute(void)
{
	int			fJD, fET, fP;
	double		sinLat, cosLat, sinHgt;
	CSG_Table	*pDays, *pHours;

	//-----------------------------------------------------
	pDays	= Parameters("DAYS")	->asTable();
	pHours	= Parameters("HOURS")	->asTable();
	fJD		= Parameters("JD")		->asInt();
	fET		= Parameters("ET")		->asInt();
	fP		= Parameters("P")		->asInt();

	sinLat	= sin(Parameters("LAT")	->asDouble() * M_DEG_TO_RAD);
	cosLat	= cos(Parameters("LAT")	->asDouble() * M_DEG_TO_RAD);
	sinHgt	= 0.0;	// -0.0145;	// >> -50'' desired height of horizon

	pHours->Destroy();
	pHours->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pDays->Get_Name(), _TL("h")));
	pHours->Add_Field(SG_T("JULIAN_DAY")	, SG_DATATYPE_Int);
	pHours->Add_Field(SG_T("HOUR")			, SG_DATATYPE_Int);
	pHours->Add_Field(SG_T("ET")			, SG_DATATYPE_Double);

	if( fP >= 0 )
	{
		pHours->Add_Field(SG_T("P")				, SG_DATATYPE_Double);
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
