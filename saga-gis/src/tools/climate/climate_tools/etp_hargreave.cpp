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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "climate_tools.h"

#include "etp_hargreave.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPET_Hargreave_Grid::CPET_Hargreave_Grid(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("ETpot (after Hargreaves, Grid)"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Estimation of daily potential evapotranspiration from daily average, minimum and maximum temperatures "
		"using Hargreave's empirical equation. In order to estimate extraterrestrial net radiation "
		"geographic latitude of observation and Julian day have to be supplied too. "
	));

	Add_Reference("Ambikadevi, K.M.", "2004",
		"Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts.",
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst.",
		SG_T("http://scholarworks.umass.edu/cee_ewre/22/")
	);

	Add_Reference("Hargraeves, G.H., Samani, Z.A.", "1985",
		"Reference crop evapotranspiration from ambient air temperatures.",
		"Paper presented in ASAE Regional Meeting, Grand Junction, Colorado.",
		SG_T("http://cagesun.nmsu.edu/~zsamani/papers/Hargreaves_Samani_85.pdf")
	);

	Add_Reference("Allen, R.G., Pereira, L.S., Raes, D., Smith, M.", "1998",
		"Crop evapotranspiration - Guidelines for computing crop water requirements.",
		"FAO Irrigation and drainage paper 56.",
		SG_T("http://www.fao.org/docrep/X0490E/x0490e00.htm#Contents")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("", "T"    , _TL("Mean Temperature"            ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "T_MIN", _TL("Minimum Temperature"         ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "T_MAX", _TL("Maximum Temperature"         ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "PET"  , _TL("Potential Evapotranspiration"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Double("",
		"LAT"	, _TL("Latitude [Degree]"),
		_TL(""),
		53.0, -90.0, true, 90.0, true
	);

	Parameters.Add_Choice("",
		"TIME"	, _TL("Time"),
		_TL(""),
		CSG_String::Format("%s|%s|",
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
int CPET_Hargreave_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("T") )
	{
		pParameters->Set_Enabled("LAT", pParameter->asGrid() && pParameter->asGrid()->Get_Projection().is_Okay() == false);
	}

	if( pParameter->Cmp_Identifier("TIME") )
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
	CSG_Grid	*pTavg	= Parameters("T"    )->asGrid();
	CSG_Grid	*pTmin	= Parameters("T_MIN")->asGrid();
	CSG_Grid	*pTmax	= Parameters("T_MAX")->asGrid();
	CSG_Grid	*pPET	= Parameters("PET"  )->asGrid();

	//-----------------------------------------------------
	CSG_Grid	Lat, *pLat	= NULL;

	if( pTavg->Get_Projection().is_Okay() )
	{
		bool	bResult;

		CSG_Grid	Lon(*Get_System());

		Lat.Create(*Get_System());

		SG_RUN_TOOL(bResult, "pj_proj4", 17,	// geographic coordinate grids
				SG_TOOL_PARAMETER_SET("GRID", pTavg)
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
			if( pTavg->is_NoData(x, y) || pTmin->is_NoData(x, y) || pTmax->is_NoData(x, y) || (pLat && pLat->is_NoData(x, y)) )
			{
				pPET->Set_NoData(x, y);
			}
			else
			{
				double	PET	= CT_Get_ETpot_Hargreave(pLat ? CT_Get_Radiation_Daily_TopOfAtmosphere(Day, pLat->asDouble(x, y)) : R0_const,
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
	//-----------------------------------------------------
	Set_Name		(_TL("ETpot (after Hargreaves, Table)"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Estimation of daily potential evapotranspiration from daily average, minimum and maximum temperatures "
		"using Hargreave's empirical equation. In order to estimate extraterrestrial net radiation "
		"geographic latitude of observation and Julian day have to be supplied too. "
	));

	Add_Reference("Ambikadevi, K.M.", "2004",
		"Simulation of Evapotranspiration and Rainfall-runoff for the Stillwater River Watershed in Central Massachusetts.",
		"Environmental & Water Resources Engineering Masters Projects, University of Massachusetts, Amherst.",
		SG_T("http://scholarworks.umass.edu/cee_ewre/22/")
	);

	Add_Reference("Hargraeves, G.H., Samani, Z.A.", "1985",
		"Reference crop evapotranspiration from ambient air temperatures.",
		"Paper presented in ASAE Regional Meeting, Grand Junction, Colorado.",
		SG_T("http://cagesun.nmsu.edu/~zsamani/papers/Hargreaves_Samani_85.pdf")
	);

	Add_Reference("Allen, R.G., Pereira, L.S., Raes, D., Smith, M.", "1998",
		"Crop evapotranspiration - Guidelines for computing crop water requirements.",
		"FAO Irrigation and drainage paper 56.",
		SG_T("http://www.fao.org/docrep/X0490E/x0490e00.htm#Contents")
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"	, _TL("Data"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"JD"	, _TL("Julian Day"),
		_TL("")
	);

	Parameters.Add_Table_Field("TABLE",
		"T"		, _TL("Mean Temperature"),
		_TL("")
	);

	Parameters.Add_Table_Field("TABLE",
		"T_MIN"	, _TL("Minimum Temperature"),
		_TL("")
	);

	Parameters.Add_Table_Field("TABLE",
		"T_MAX"	, _TL("Maximum Temperature"),
		_TL("")
	);

	Parameters.Add_Double("TABLE",
		"LAT"	, _TL("Latitude"),
		_TL(""),
		53.0, -90.0, true, 90.0, true
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
	Lat		= Parameters("LAT"  )->asDouble();
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
			pRecord->Set_Value(fET, CT_Get_ETpot_Hargreave(CT_Get_Radiation_Daily_TopOfAtmosphere(
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
	//-----------------------------------------------------
	Set_Name		(_TL("Daily to Hourly ETpot"));

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
		53.0, -90.0, true, 90.0, true
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

	sinLat	= sin(Parameters("LAT")->asDouble() * M_DEG_TO_RAD);
	cosLat	= cos(Parameters("LAT")->asDouble() * M_DEG_TO_RAD);
	sinHgt	= 0.0;	// -0.0145;	// >> -50'' desired height of horizon

	pHours->Destroy();
	pHours->Fmt_Name("%s [%s]", pDays->Get_Name(), _TL("h"));
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
