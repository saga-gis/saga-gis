
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
//                    climate_tools.h                    //
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
// Free Software Foundation; version >=2 of the License. //
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
#ifndef HEADER_INCLUDED__climate_tools_H
#define HEADER_INCLUDED__climate_tools_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double	CT_Get_Radiation_Daily_TopOfAtmosphere	(int DayOfYear, double Latitude, bool bWaterEquivalent = true);

//---------------------------------------------------------
double	CT_Get_ETpot_Hargreave					(double R0                     , double T, double Tmin, double Tmax);
double	CT_Get_ETpot_Hargreave					(int DayOfYear, double Latitude, double T, double Tmin, double Tmax);

bool	CT_Get_ETpot_Hargreave_DailyFromMonthly	(CSG_Vector &ETpot, double Latitude, const double T[12], const double Tmin[12], const double Tmax[12]);


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	CT_Get_Daily_Splined					(CSG_Vector &Daily  , const double Monthly  [12]);

//---------------------------------------------------------
bool	CT_Get_Daily_Precipitation				(CSG_Vector &Daily_P, const double Monthly_P[12], const double Monthly_T[12]);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCT_Snow_Accumulation
{
public:
	CCT_Snow_Accumulation(void);
	CCT_Snow_Accumulation(const CCT_Snow_Accumulation &Snow_Accumulation);

	virtual ~CCT_Snow_Accumulation(void);

	bool						Calculate				(const double T[365], const double P[365]);

	int							Get_Snow_Days			(void)		const	{	return( m_nSnow      );	}
	double						Get_Snow				(int iDay)	const	{	return( m_Snow[iDay] );	}

	operator const double *								(void)		const	{	return( m_Snow       );	}
	double						operator []				(int iDay)	const	{	return( m_Snow[iDay] );	}

	static double				Get_SnowMelt			(double Snow, double T, double P);


private:

	int							m_nSnow;

	CSG_Vector					m_Snow;


	int							Get_Start				(const double *T);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCT_Soil_Water
{
public:
	CCT_Soil_Water(void);
	CCT_Soil_Water(const CCT_Soil_Water &Soil_Water);

	virtual ~CCT_Soil_Water(void);

	bool						Create					(const CCT_Soil_Water &Soil_Water);

	bool						Set_Capacity			(int Layer, double Value);
	double						Get_Capacity			(int Layer)	const	{	return( m_SW_Capacity  [Layer] );	}

	bool						Set_ET_Resistance		(int Layer, double Value);
	double						Get_ET_Resistance		(int Layer)	const	{	return( m_SW_Resistance[Layer] );	}

	bool						Calculate				(const double T[365], const double P[365], const double ETpot[365], const double Snow[365]);

	double						Get_SW_0				(int iDay)	const	{	return( m_SW[0][iDay] );	}
	double						Get_SW_1				(int iDay)	const	{	return( m_SW[1][iDay] );	}

	const double *				Get_SW_0				(void)		const	{	return( m_SW[0] );	}
	const double *				Get_SW_1				(void)		const	{	return( m_SW[1] );	}


private:

	double						m_SW_Capacity[2], m_SW_Resistance[2];

	CSG_Vector					m_SW[2];


	int							Get_Start				(const double *P, const double *ETpot);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCT_Water_Balance
{
public:

	//-----------------------------------------------------
	enum EMonthly
	{
		MONTHLY_T	= 0,
		MONTHLY_Tmin,
		MONTHLY_Tmax,
		MONTHLY_P,
		MONTHLY_COUNT
	};

	//-----------------------------------------------------
	enum EDaily
	{
		DAILY_T	= 0,
		DAILY_P,
		DAILY_ETpot,
		DAILY_Snow,
		DAILY_COUNT
	};


public:
	CCT_Water_Balance(void);
	CCT_Water_Balance(const CCT_Water_Balance &Copy);

	bool						Set_Monthly				(int Type, const double Values[12]);
	bool						Set_Monthly				(int Type, int x, int y, CSG_Parameter_Grid_List *pMonthly, double Default = -1.0);;

	CCT_Soil_Water &			Get_Soil				(void)		{	return( m_Soil );	}

	virtual bool				Calculate				(double TSWC, double Latitude);

	double						Get_T					(int iDay)	{	return( m_Daily[DAILY_T    ][iDay] );	}
	double						Get_P					(int iDay)	{	return( m_Daily[DAILY_P    ][iDay] );	}
	double						Get_ETpot				(int iDay)	{	return( m_Daily[DAILY_ETpot][iDay] );	}
	double						Get_Snow				(int iDay)	{	return( m_Snow              [iDay] );	}
	double						Get_SW_0				(int iDay)	{	return( m_Soil.Get_SW_0     (iDay) );	}
	double						Get_SW_1				(int iDay)	{	return( m_Soil.Get_SW_1     (iDay) );	}


protected:

	CSG_Vector					m_Monthly[MONTHLY_COUNT], m_Daily[DAILY_COUNT];

	CCT_Snow_Accumulation		m_Snow;

	CCT_Soil_Water				m_Soil;


	bool						Set_Soil_Capacity		(double SWC);
	const double *				Set_ETpot				(double Latitude, const double Tmin[12], const double Tmax[12]);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__climate_tools_H
