/**********************************************************
 * Version $Id: climate_tools.cpp 1380 2012-04-26 12:02:19Z reklov_w $
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
//                   climate_tools.cpp                   //
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
#include "climate_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double	CT_Get_Radiation_TopOfAtmosphere	(int DayOfYear, double Latitude)
{
	double	sinLat	= sin(Latitude * M_DEG_TO_RAD);
	double	cosLat	= cos(Latitude * M_DEG_TO_RAD);
	double	tanLat	= tan(Latitude * M_DEG_TO_RAD);

	double	JD		= DayOfYear * M_PI * 2.0 / 365.0;

	double	dR		= 0.033  * cos(JD) + 1.0;	// relative distance between sun and earth on any Julian day

	double	SunDec	= 0.4093 * sin(JD - 1.405);	// solar declination

	double	d		= -tanLat * tan(SunDec);	// sunset hour angle
	double	SunSet	= acos(d < -1 ? -1 : d < 1 ? d : 1);

	double	R0		= 15.392 * dR * (SunSet * sinLat * sin(SunDec) + cosLat * cos(SunDec) * sin(SunSet));

	return( R0 );	// water equivalent of extraterrestrial radiation (mm/day)
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double	CT_Get_ETpot_Hargreave	(double R0, double T, double Tmin, double Tmax)
{
	double	ETpot	= 0.0023 * R0 * (T + 17.8) * sqrt(Tmax - Tmin);	// reference crop evapotranspiration mm per day

	return( ETpot > 0.0 ? ETpot : 0.0 );
}

//---------------------------------------------------------
double	CT_Get_ETpot_Hargreave	(int DayOfYear, double Latitude, double T, double Tmin, double Tmax)
{
	double	R0	= CT_Get_Radiation_TopOfAtmosphere(DayOfYear, Latitude);

	return( CT_Get_ETpot_Hargreave(R0, T, Tmin, Tmax) );
}

//---------------------------------------------------------
bool	CT_Get_ETpot_Hargreave_DailyFromMonthly	(CSG_Vector &ETpot, double Latitude, const double T[12], const double Tmin[12], const double Tmax[12])
{
	CSG_Vector	dT, dTmin, dTmax;

	CT_Get_Daily_Splined(dT   , T   );
	CT_Get_Daily_Splined(dTmin, Tmin);
	CT_Get_Daily_Splined(dTmax, Tmax);

	ETpot.Create(365);

	for(int i=0; i<365; i++)
	{
		ETpot[i]	= CT_Get_ETpot_Hargreave(i + 1, Latitude, dT[i], dTmin[i], dTmax[i]);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	CT_Get_Daily_Splined(CSG_Vector &Daily, const double Monthly[12])
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

	Daily.Create(365);

	for(int iDay=0; iDay<365; iDay++)
	{
		Daily[iDay]	= Spline.Get_Value(iDay);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	CT_Get_Daily_Precipitation(CSG_Vector &Daily_P, const double Monthly_P[12], const double Monthly_T[12])
{
	static const int	nDaysOfMonth[12]	=
	// JAN  FEB  MAR  APR  MAY  JUN  JUL  AUG  SEP  OCT  NOV  DEC
	{	31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31	};

	Daily_P.Create(365);

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

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCT_Snow_Accumulation::CCT_Snow_Accumulation(void)
{
	m_Snow.Create(365);
	m_nSnow	= 0;
}

//---------------------------------------------------------
CCT_Snow_Accumulation::CCT_Snow_Accumulation(const CCT_Snow_Accumulation &Snow_Accumulation)
{
	m_Snow.Create(Snow_Accumulation.m_Snow);
	m_nSnow	= Snow_Accumulation.m_nSnow;
}

//---------------------------------------------------------
CCT_Snow_Accumulation::~CCT_Snow_Accumulation(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Snow_Accumulation::Calculate(const double T[365], const double P[365])
{
	//-----------------------------------------------------
	int		iStart	= Get_Start(T);

	if( iStart < 0 )	// no frost/non-frost change
	{
		if( T[0] < 0.0 )	// no day without frost
		{
			double	Snow	= 0.0;

			for(int i=0; i<365; i++)
			{
				Snow	+= P[i];
			}

			m_Snow	= Snow;	// precipitation accumulated over one year
			m_nSnow	= 365;
		}
		else				// no frost at all
		{
			m_Snow	= 0.0;
			m_nSnow	= 0;
		}

		return( true );
	}

	//-----------------------------------------------------
	int	nSnow, iPass = 0, maxIter = 64;

	double	Snow	= 0.0;

	m_Snow	= 0.0;
	m_nSnow	= 0;	// days with snow cover

	do
	{
		nSnow	= m_nSnow;	m_nSnow	= 0;

		for(int iDay=iStart; iDay<iStart+365; iDay++)
		{
			int	i	= iDay % 365;

			if( T[i] < 0.0 )		// snow accumulation
			{
				Snow	+= P[i];
			}
			else if( Snow > 0.0 )	// snow melt
			{
				Snow	-= Get_SnowMelt(Snow, T[i], P[i]);
			}

			if( Snow > 0.0 )
			{
				m_nSnow++;
			}

			m_Snow[i]	= Snow;
		}
	}
	while( m_nSnow != nSnow && m_nSnow < 365 && iPass++ < maxIter );

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCT_Snow_Accumulation::Get_Start(const double *T)
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


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CCT_Snow_Accumulation::Get_SnowMelt(double Snow, double T, double P)
{
	if( T > 0.0 && Snow > 0.0 )
	{
		double	dSnow	= T * (0.84 + 0.125 * P);

		return( dSnow > Snow ? Snow : dSnow );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCT_Soil_Water::CCT_Soil_Water(void)
{
	m_SW_Capacity  [0]	=  20;
	m_SW_Capacity  [1]	= 200;

	m_SW_Resistance[0]	= 0.0;
	m_SW_Resistance[1]	= 1.0;
}

//---------------------------------------------------------
CCT_Soil_Water::CCT_Soil_Water(const CCT_Soil_Water &Soil_Water)
{
	Create(Soil_Water);
}

//---------------------------------------------------------
bool CCT_Soil_Water::Create(const CCT_Soil_Water &Soil_Water)
{
	m_SW_Capacity  [0]	= Soil_Water.m_SW_Capacity  [0];
	m_SW_Capacity  [1]	= Soil_Water.m_SW_Capacity  [1];

	m_SW_Resistance[0]	= Soil_Water.m_SW_Resistance[0];
	m_SW_Resistance[1]	= Soil_Water.m_SW_Resistance[1];

	return( true );
}

//---------------------------------------------------------
CCT_Soil_Water::~CCT_Soil_Water(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Soil_Water::Set_Capacity(int Layer, double Value)
{
	if( Layer >= 0 && Layer <= 1 && Value >= 0.0 )
	{
		m_SW_Capacity[Layer]	= Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CCT_Soil_Water::Set_ET_Resistance(int Layer, double Value)
{
	if( Layer >= 0 && Layer <= 1 && Value > 0.0 )
	{
		m_SW_Resistance[Layer]	= Value;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Soil_Water::Calculate(const double T[365], const double P[365], const double ETpot[365], const double Snow[365])
{
	//-----------------------------------------------------
	int	iStart	= Get_Start(P, ETpot), iPass = 0, maxIter = 64;

	double	SW[2], SW_Last[2];

	m_SW[0].Create(365);
	m_SW[1].Create(365);

	SW[0]	= 0.5 * m_SW_Capacity[0];
	SW[1]	= 0.5 * m_SW_Capacity[1];

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

				if( Snow[i] > 0.0 )
				{
					dSW	+= CCT_Snow_Accumulation::Get_SnowMelt(Snow[i], T[i], P[i]);
				}
				else
				{
					dSW	-= ETpot[i];
				}

				SW[0]	+= dSW;

				if( SW[0] > m_SW_Capacity[0] )	// more water in upper soil layer than its capacity
				{
					dSW		= SW[0] - m_SW_Capacity[0];
					SW[0]	= m_SW_Capacity[0];
				}
				else if( SW[0] < 0.0 )	// evapotranspiration exceeds available water
				{
				//	dSW		= m_SW_Capacity[1] > 0.0 ? (SW[0] > -SW[1] ? SW[0] : -SW[1]) * sqrt(SW[1] / m_SW_Capacity[1]) : 0.0;
					dSW		= m_SW_Capacity[1] > 0.0 ? SW[0] * pow(SW[1] / m_SW_Capacity[1], m_SW_Resistance[1]) : 0.0;	// positive: runoff, negative: loss by evapotranspiration not covered by upper layer, with water loss resistance;
					SW[0]	= 0.0;
				}
				else
				{
					dSW		= 0.0;
				}

				//---------------------------------------------
				// lower soil layer

				SW[1]	+= dSW;

				if( SW[1] > m_SW_Capacity[1] )	// more water in lower soil layer than its capacity
				{
					SW[1]	= m_SW_Capacity[1];
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
	while( iPass++ <= 1 || (SW_Last[0] != SW[0] && iPass <= maxIter) );

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCT_Soil_Water::Get_Start(const double *P, const double *ETpot)
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCT_Water_Balance::CCT_Water_Balance(void)
{}

//---------------------------------------------------------
CCT_Water_Balance::CCT_Water_Balance(const CCT_Water_Balance &Copy)
{
	m_Soil.Create(Copy.m_Soil);

	for(int i=0; i<MONTHLY_COUNT; i++)
	{
		m_Monthly[i].Create(Copy.m_Monthly[i]);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Water_Balance::Set_Monthly(int Type, const double Values[12])
{
	return( m_Monthly[Type].Create(12, (double *)Values) );
}

//---------------------------------------------------------
bool CCT_Water_Balance::Set_Monthly(int Type, int x, int y, CSG_Parameter_Grid_List *pMonthly, double Default)
{
	m_Monthly[Type].Create(12);

	double *Monthly	= m_Monthly[Type].Get_Data();

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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Water_Balance::Calculate(double SWC, double Latitude)
{
	//-----------------------------------------------------
	Set_Soil_Capacity(SWC);

	//-----------------------------------------------------
	CT_Get_Daily_Splined      (m_Daily[DAILY_T], m_Monthly[MONTHLY_T]);
	CT_Get_Daily_Precipitation(m_Daily[DAILY_P], m_Monthly[MONTHLY_P], m_Monthly[MONTHLY_T]);

	m_Snow.Calculate(m_Daily[DAILY_T], m_Daily[DAILY_P]);

	m_Soil.Calculate(m_Daily[DAILY_T], m_Daily[DAILY_P], Set_ETpot(Latitude, m_Monthly[MONTHLY_Tmin], m_Monthly[MONTHLY_Tmax]), m_Snow);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Water_Balance::Set_Soil_Capacity(double SWC)
{
	if( SWC < m_Soil.Get_Capacity(0) )
	{
		m_Soil.Set_Capacity(0, SWC);
		m_Soil.Set_Capacity(1, 0.0);
	}
	else
	{
		m_Soil.Set_Capacity(1, SWC - m_Soil.Get_Capacity(0));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const double * CCT_Water_Balance::Set_ETpot(double Latitude, const double Tmin[12], const double Tmax[12])
{
	CSG_Vector	dTmin, dTmax;

	CT_Get_Daily_Splined(dTmin, Tmin);
	CT_Get_Daily_Splined(dTmax, Tmax);

	CT_Get_ETpot_Hargreave_DailyFromMonthly(m_Daily[DAILY_ETpot], Latitude, m_Daily[DAILY_T], dTmin, dTmax);

	return( m_Daily[DAILY_ETpot] );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
