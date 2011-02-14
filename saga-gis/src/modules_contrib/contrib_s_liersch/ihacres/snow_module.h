////////////////////////////////////////////////////////////////
// File:
// snow_module.h
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

#ifndef HEADER_INCLUDED__snow_module_H
#define HEADER_INCLUDED__snow_module_H

#include <vector>   // used for storing date string values in array

typedef std::vector<double> vector_d;
//--------------------------------------------------------------

class CSnowParms
{
public:
	CSnowParms() {
		T_Rain = 0.0;
		T_Melt = 0.0;
		DD_FAC = 0.0;
	}
	double			T_Rain;
	double			T_Melt;
	double			DD_FAC;
};

//--------------------------------------------------------------

class CSnowModule
{
public:
	CSnowModule(int size);
	CSnowModule(double *temperature, double *precipitation, int size,
				double T_Rain, double T_Melt, double DD_FAC);

	// using double vectors as input instead of double*
	CSnowModule(vector_d temperature, vector_d precipitation,
				double T_Rain, double T_Melt, double DD_FAC);

	~CSnowModule();

	//----------------------------------------------
	// PUBLIC FUNCTIONS
	//----------------------------------------------
	void			InitParms(int size);
	double			Get_SnowStorage(unsigned int i)	{ return( i < m_size ? m_pSnowStorage[i] : -9999); }
	double*			Get_SnowStorage(double *snow_storage, int size);
	double			Get_MeltRate(unsigned int i)		{ return( i < m_size ? m_pMeltRate[i] : -9999); }
	double*			Get_MeltRate(double *melt_rate, int size);
	double			Get_T_Rain()			{ return(m_T_Rain); }
	double			Get_T_Melt()			{ return(m_T_Melt); }
	double			Get_DD_FAC()			{ return(m_DD_FAC); }

	bool			Set_T_Rain(double value);
	bool			Set_T_Melt(double value);
	bool			Set_DD_FAC(double value);

	bool			Calc_SnowModule(double *temperature, double *precipitation, unsigned int size,// pointer size (temperature, precip)
									double T_Rain, double T_Melt, double DD_FAC);

	//----------------------------------------------

private:
	void			_ZeroPointers();
	// variables
	double*			m_pSnowStorage;
	double*			m_pMeltRate;
	unsigned int		m_size; // number of values in arrays, time steps
	double			m_T_Rain;
	double			m_T_Melt;
	double			m_DD_FAC;

//--------------------------------------------------------------
//
// FUNCTIONS FOR VECTORS
//
//--------------------------------------------------------------
public:
	bool			Calc_SnowModule(vector_d temperature, vector_d precipitation,
									double T_Rain, double T_Melt, double DD_FAC);


};

#endif /* HEADER_INCLUDED__snow_module_H */
