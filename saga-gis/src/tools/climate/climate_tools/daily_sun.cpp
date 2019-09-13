
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDaily_Sun::CDaily_Sun(void)
{
	//-----------------------------------------------------
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
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDaily_Sun::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double SG_Range_Set_0_to_24(double Value)
{
	Value	= fmod(Value, 24.0);

	return( Value < 0.0 ? Value	+ 24.0 : Value );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDaily_Sun::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pTarget	= Parameters("TARGET")->asGrid();

	if( !pTarget->Get_Projection().is_Okay() )
	{
		Error_Set(_TL("target's spatial reference system is undefined"));

		return( false );
	}

	CSG_Grid	Lon(Get_System()), Lat(Get_System());

	SG_RUN_TOOL_ExitOnError("pj_proj4", 17,	// geographic coordinate grids
			SG_TOOL_PARAMETER_SET("GRID", pTarget)
		&&	SG_TOOL_PARAMETER_SET("LON" , &Lon)
		&&	SG_TOOL_PARAMETER_SET("LAT" , &Lat)
	)

	Lon	*= M_DEG_TO_RAD;
	Lat	*= M_DEG_TO_RAD;

	//-----------------------------------------------------
	CSG_Grid	*pSunset	= Parameters("SUNSET" )->asGrid();
	CSG_Grid	*pSunrise	= Parameters("SUNRISE")->asGrid();
	CSG_Grid	*pDuration	= Parameters("LENGTH" )->asGrid();

	bool	bWorld	= Parameters("TIME")->asInt() == 1;

	//-----------------------------------------------------
	CSG_DateTime	Time(Parameters("DAY")->asDate()->Get_Date());

	Time.Reset_Time();

	Message_Add(Time.Format("\n%A, %d. %B %Y"), false);

	//-----------------------------------------------------
	double	Dec, RA, RAm, T;

	SG_Get_Sun_Position(Time, RA, Dec);

	T	= ((int)Time.Get_JDN() - 2451545.0 ) / 36525.0;	// Number of Julian centuries since 2000/01/01 at 12 UT (JDN = 2451545.0)

	RAm	= fmod(18.71506921 + 2400.0513369 * T + (2.5862e-5 - 1.72e-9 * T) * T*T, 24.0);
	RA	= fmod(RA * 12.0 / M_PI, 24.0);	if( RA < 0.0 ) RA += 24.0;

	T	= 1.0027379 * (RAm - RA);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
	//	#pragma omp parallel for
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
				double	dT	= (sin(-Lat.asDouble(x, y) / 60.0) - sin(Lat.asDouble(x, y)) * sin(Dec)) / (cos(Lat.asDouble(x, y)) * cos(Dec));

				if( dT > 1.0 )
				{
					pSunrise ->Set_NoData(x, y);
					pSunset  ->Set_NoData(x, y);
					pDuration->Set_Value (x, y,  0.0);
				}
				else if( dT < -1.0 )
				{
					pSunrise ->Set_NoData(x, y);
					pSunset  ->Set_NoData(x, y);
					pDuration->Set_Value (x, y, 24.0);
				}
				else
				{
					dT	= acos(dT) * 12.0 / M_PI;

					double	Sunrise	= SG_Range_Set_0_to_24(12.0 - dT - T);
					double	Sunset	= SG_Range_Set_0_to_24(12.0 + dT - T);

					pDuration->Set_Value(x, y, Sunset - Sunrise);

					if( bWorld )
					{
						Sunrise	= SG_Range_Set_0_to_24(Sunrise - M_RAD_TO_DEG * Lon.asDouble(x, y) / 15.0);
						Sunset	= SG_Range_Set_0_to_24(Sunset  - M_RAD_TO_DEG * Lon.asDouble(x, y) / 15.0);
					}

					pSunrise ->Set_Value(x, y, Sunrise);
					pSunset  ->Set_Value(x, y, Sunset );
				}
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
