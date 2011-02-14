/**********************************************************
 * Version $Id$
 *********************************************************/
////////////////////////////////////////////////////////////////
// File:
// snow_module.cpp
//
// A class to calculate a time series of 
// snow melt and snow storage.
//
// Author:
// Stefan Liersch: stefan.liersch@gmail.com, stefan.liersch@ufz.de
// 
// Date:			2007-10-05
// last modified:	2007-10-05
//
//--------------------------------------------------------------
// DESCRIPTION
//--------------------------------------------------------------
// This class provides functions to calculate snow melt processes,
// based on the simple degree-day method.

// INPUT:
// - Daily temperature and precipitation time series
//	 This could be double* pointers or double vectors.
//	 In both cases the output (snow storage and melt rate) will be double* !!!
// - T_Rain : temperature threshold, T < T_Rain precip = snow (range: -10 to +10�C) 
// - T_Melt : temperature threshold, T > T_Melt = Snow storage starts melting (range: -5 to +10�C)
// - DD_FAC : day-degree factor (range: 0.7 to 9.2)

// OUTPUT:
// - double* m_pSnowStorage[i]
// - double* m_pMeltRate[i]

//--------------------------------------------------------------
// REFERENCES
//--------------------------------------------------------------
// - The Encyclopedia of Water
//		http://www.wileywater.com/Contributor/Sample_3.htm
// - Chang, A. T. C., J. L. Foster, P. Gloersen, W. J. Campbell, E. G. Josberger, A. Rango and Z. F. Danes (1987)
//		Estimating snowpack parameters in the Colorado River basin.
//		In: Proc. Large Scale Effects of Seasonal Snow Cover,
//		IAHS Publ. No. 166, 343-353.
// - Singh, P. and Singh, V.P. (2001)
//		Snow and Glacier Hydrology.
//		Kluwer Academic Publishers, Dordrecht, The Netherlands, p. 221
//--------------------------------------------------------------
// ToDo
//--------------------------------------------------------------
// - implementation for vector instead of double*
//--------------------------------------------------------------

//--------------------------------------------------------------
#include <iostream>
#include "snow_module.h"
//--------------------------------------------------------------

//--------------------------------------------------------------
#define max(a, b)	(((a) > (b)) ? (a) : (b))
//--------------------------------------------------------------

//--------------------------------------------------------------
// CONSTRUCTORS
//--------------------------------------------------------------

CSnowModule::CSnowModule(int size)
{
	InitParms(size);
}
//--------------------------------------------------------------
// double* INPUT
//
CSnowModule::CSnowModule(double *temperature, double *precipitation, int size,
						 double T_Rain, double T_Melt, double DD_FAC)
{
	m_T_Rain = T_Rain;
	m_T_Melt = T_Melt;
	m_DD_FAC = DD_FAC;
	m_size = size;
	InitParms(m_size);
	
	// perform snow storage and snow melt calculations
	Calc_SnowModule(temperature, precipitation, m_size, T_Rain, T_Melt, DD_FAC);
}
//--------------------------------------------------------------
// vector INPUT
//
CSnowModule::CSnowModule(vector_d temperature, vector_d precipitation,
						 double T_Rain, double T_Melt, double DD_FAC)
{
	m_T_Rain = T_Rain;
	m_T_Melt = T_Melt;
	m_DD_FAC = DD_FAC;
	m_size = temperature.size();
	InitParms(m_size);
	
	// perform snow storage and snow melt calculations
	Calc_SnowModule(temperature, precipitation, T_Rain, T_Melt, DD_FAC);
}

//--------------------------------------------------------------
// DESTRUCTOR
//--------------------------------------------------------------
CSnowModule::~CSnowModule()
{
	if (m_pSnowStorage)	delete[] m_pSnowStorage;
	if (m_pMeltRate)	delete[] m_pMeltRate;
}

//--------------------------------------------------------------
//
// PUBLIC FUNCTIONS
//
//--------------------------------------------------------------
void CSnowModule::InitParms(int size)
{
	m_size = size;
	m_pSnowStorage = new double[m_size];
	m_pMeltRate = new double[m_size];
	_ZeroPointers();
}
//--------------------------------------------------------------
double *CSnowModule::Get_SnowStorage(double *snow_storage, int size)
{
	for (int i = 0; i < size; i++)
		snow_storage[i] = m_pSnowStorage[i];
	return(snow_storage);
}
//--------------------------------------------------------------
double *CSnowModule::Get_MeltRate(double *melt_rate, int size)
{
	for (int i = 0; i < size; i++)
		melt_rate[i] = m_pMeltRate[i];
	return(melt_rate);
}
//--------------------------------------------------------------
//
//	SET FUNCTIONS
//
//--------------------------------------------------------------
bool CSnowModule::Set_T_Rain(double value)
{
	if (value >= -10.0 && value <= 10.0)
	{
		m_T_Rain = value;
		return(true);
	} else {
		return(false);
	}
}
//--------------------------------------------------------------
bool CSnowModule::Set_T_Melt(double value)
{
	if (value >= -5.0 && value <= 10.0)
	{
		m_T_Melt = value;
		return(true);
	} else {
		return(false);
	}
}
//--------------------------------------------------------------
bool CSnowModule::Set_DD_FAC(double value)
{
	if (value >= 0.7 && value <= 9.2)
	{
		m_DD_FAC = value;
		return(true);
	} else {
		return(false);
	}
}

//--------------------------------------------------------------
//
//	CALCULATE Snow storage and melt rate
//
//--------------------------------------------------------------

//--------------------------------------------------------------
// double* INPUT
//--------------------------------------------------------------
bool CSnowModule::Calc_SnowModule(double *temperature, double *precipitation, unsigned int size,
								  double T_Rain, double T_Melt, double DD_FAC)
{
	// still missing!!!
	// what happens with precipitation at the same day as 
	// snowmelt is calculated?
	// is it correct to handle this in excess rainfall function only?

	if (size != m_size) return(false);

	_ZeroPointers();
	m_T_Rain = T_Rain;
	m_T_Melt = T_Melt;
	m_DD_FAC = DD_FAC;

	double T_diff;

	for (unsigned int i = 1; i < size; i++)
	{
		// calculate snow accumulation
		if (temperature[i] < T_Rain)
		{
			m_pSnowStorage[i] = m_pSnowStorage[i-1] + precipitation[i];
			m_pMeltRate[i] = 0.0;
		}

		// calculate snowmelt
		if (temperature[i] > T_Melt)
		{
			T_diff = temperature[i] - T_Melt;
			m_pMeltRate[i] = DD_FAC * max(0.0,T_diff);
			if (m_pMeltRate[i] > m_pSnowStorage[i-1])
			{
				m_pMeltRate[i] = m_pSnowStorage[i-1];
				m_pSnowStorage[i] = 0.0;
			}
			m_pSnowStorage[i] = m_pSnowStorage[i-1] - m_pMeltRate[i];
		}

		// if temperature between T_Rain and T_Melt
		if ((temperature[i] > T_Rain) && (temperature[i] < T_Melt))
		{
			/*
			T_diff = temperature[i] - T_Melt;
			m_pMeltRate[i] = (DD_FAC * max(0.0, T_diff)) / 2;
			*/
			m_pMeltRate[i] = 0.0;
			if (precipitation[i] > 0.0) m_pMeltRate[i] = precipitation[i] / 2;
			if (m_pMeltRate[i] > m_pSnowStorage[i-1])
			{
				m_pMeltRate[i] = m_pSnowStorage[i-1];
				m_pSnowStorage[i] = 0.0;
			}
			m_pSnowStorage[i] = m_pSnowStorage[i-1] - m_pMeltRate[i];
		}

		if (m_pSnowStorage[i] < 0.0) m_pSnowStorage[i] = 0.0;
	}
	return(true);
}

//--------------------------------------------------------------
// vector INPUT
//--------------------------------------------------------------
bool CSnowModule::Calc_SnowModule(vector_d temperature, vector_d precipitation,
								  double T_Rain, double T_Melt, double DD_FAC)
{
	// still missing!!!
	// what happens with precipitation at the same day as 
	// snowmelt is calculated?
	// is it correct to handle this in excess rainfall function only?

	if (temperature.size() != m_size || precipitation.size() != m_size)
		return(false);

	int size = m_size;

	_ZeroPointers();
	m_T_Rain = T_Rain;
	m_T_Melt = T_Melt;
	m_DD_FAC = DD_FAC;

	double T_diff;

	for (int i = 1; i < size; i++)
	{
		// calculate snow accumulation
		if (temperature[i] < T_Rain)
		{
			m_pSnowStorage[i] = m_pSnowStorage[i-1] + precipitation[i];
			m_pMeltRate[i] = 0.0;
		}

		// calculate snowmelt
		if (temperature[i] > T_Melt)
		{
			T_diff = temperature[i] - T_Melt;
			m_pMeltRate[i] = DD_FAC * max(0.0,T_diff);
			if (m_pMeltRate[i] > m_pSnowStorage[i-1])
			{
				m_pMeltRate[i] = m_pSnowStorage[i-1];
				m_pSnowStorage[i] = 0.0;
			}
			m_pSnowStorage[i] = m_pSnowStorage[i-1] - m_pMeltRate[i];
		}

		// if temperature between T_Rain and T_Melt
		if ((temperature[i] > T_Rain) && (temperature[i] < T_Melt))
		{
			/*
			T_diff = temperature[i] - T_Melt;
			m_pMeltRate[i] = (DD_FAC * max(0.0, T_diff)) / 2;
			*/
			m_pMeltRate[i] = 0.0;
			if (precipitation[i] > 0.0) m_pMeltRate[i] = precipitation[i] / 2;
			if (m_pMeltRate[i] > m_pSnowStorage[i-1])
			{
				m_pMeltRate[i] = m_pSnowStorage[i-1];
				m_pSnowStorage[i] = 0.0;
			}
			m_pSnowStorage[i] = m_pSnowStorage[i-1] - m_pMeltRate[i];
		}

		if (m_pSnowStorage[i] < 0.0) m_pSnowStorage[i] = 0.0;
	}
	return(true);
}

//--------------------------------------------------------------
//
// PRIVATE FUNCTIONS
//
//--------------------------------------------------------------

void CSnowModule::_ZeroPointers()
{
	// This is important, because only on days where snow
	// accumulats or melts a value is assigned to the pointers
	if (m_pSnowStorage != NULL && m_pMeltRate != NULL && m_size > 0)
	{
		for (unsigned int i = 0; i < m_size; i++)
		{
			m_pSnowStorage[i]	= 0.0;
			m_pMeltRate[i]		= 0.0;
		}
	}
}
