/**********************************************************
 * Version $Id: ihacres_eq.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
///////////////////////////////////////////////////////////
//                                                       //
//                    ihacres_eq.cpp                     //
//                                                       //
//                 Copyright (C) 2006 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com                   //
//                                                       //
//                     2006-08-27                        //
//														 //
//-------------------------------------------------------//

#include <iostream> // used for textfile output (test only)
#include <fstream>  // used for textfile output (test only)

#include "ihacres_eq.h"
#include <math.h> // exp()
//---------------------------------------------------------

///////////////////////////////////////////////////////////
//
//	COMMENTS
//
//---------------------------------------------------------
// 2007-11-08
// added to function CalcWetnessIndex()
//	if (WetnessIndex[i] > 1.0) WetnessIndex[i] = 1.0;
//---------------------------------------------------------
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
//						CONSTRUCTORS
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
// DEFAULT CONSTRUCTOR
//---------------------------------------------------------------------
Cihacres_eq::Cihacres_eq()
{};

//---------------------------------------------------------------------
// two storages
// using vector<double> as input
// if no temperature data are available
Cihacres_eq::Cihacres_eq(date_array date_in,
						 vector_d streamflow,
						 vector_d pcp,
						 double TwConst, double f, double c,
						 double l, double p,
						 double aq, double as, double bq, double bs)
{
	sizeAll			= (int)streamflow.size();
	date			= date_in;
	streamflow_obs	= streamflow;
	precipitation	= pcp;
	this->TwConst	= TwConst;
	this->f			= f;
	this->c			= c;
	this->l			= l;
	this->p			= p;
	this->aq		= aq;
	this->as		= as;
	this->bq		= bq;
	this->bs		= bs;
	// Initialize Vectors
	_InitVectorsStart((int)streamflow_obs.size());
}
//---------------------------------------------------------------------
// two storages
// using vector<double> as input
// if temperature data are available
Cihacres_eq::Cihacres_eq(date_array date_in,
						 vector_d streamflow,
						 vector_d pcp,
						 vector_d tmp,
						 double TwConst, double f, double c,
						 double l, double p,
						 double aq, double as, double bq, double bs,
						 double area, bool TMP_data_exist,
						 int IHAC_vers,
						 int storconf,
						 bool bSnowModule,
						 CSnowModule *SnowMod,
						 //double T_Rain, double T_Melt, double DD_FAC,
						 int delay)
{
	// Initialize Parameters and Vectors
	sizeAll			= (int)streamflow.size();
	date			= date_in;
	streamflow_obs	= streamflow;
	precipitation	= pcp;
	temperature		= tmp;
	this->TwConst	= TwConst;
	this->f			= f;
	this->c			= c;
	this->l			= l;
	this->p			= p;
	this->aq		= aq;
	this->as		= as;
	this->bq		= bq;
	this->bs		= bs;
	this->delay		= delay;
	this->area		= area;
	IHAC_version	= IHAC_vers;
	this->bSnowModule	= bSnowModule;
	m_pSnowMod = SnowMod;

	// Initialize Vectors containing calculated values
	_InitVectorsStart(sizeAll);
	
	// Convert Streamflow vector from m3/s*day-1 to mm/day
	streamflowMM_obs = model_tools::m3s_to_mmday(streamflow_obs, streamflowMM_obs, area);

	// perform simulation
		if (bSnowModule)
		{
			RunNonLinearModule(TMP_data_exist, bSnowModule, m_pSnowMod->Get_T_Rain());
		} else {
			RunNonLinearModule(TMP_data_exist, bSnowModule, 0.0);
		}
	//switch (IHAC_version)
	//{
	//case 0: // Jakeman & Hornberger (1993)
	//	if (bSnowModule)
	//	{
	//		RunNonLinearModule(TMP_data_exist, bSnowModule, m_pSnowMod->Get_T_Rain());
	//	} else {
	//		RunNonLinearModule(TMP_data_exist, bSnowModule, 0.0);
	//	}
	//	break;
	//case 1: // Croke et al. (2005) Redesign
	//	//RunNonLinearModule5Parms();
	//	break;
	//}

	switch(storconf)
	{
	case 0: // single storage
		this->a = aq;
		this->b = bq;
		SimStreamflowSingle(excessRain, streamflowMM_obs[0], streamflow_sim, delay, a, b);
		break;
	case 1: // two storages in parallel
		SimStreamflow2Parallel(excessRain, streamflow_sim,
			streamflowMM_obs[0],
			aq, as, bq, bs,
			vq, vs,
			IHAC_vers,
			delay);
		break;
	} // end switch(storconf)

	NSE = model_tools::CalcEfficiency(streamflowMM_obs, streamflow_sim);
}
//---------------------------------------------------------------------
// two storages
// using double arrays as input
// if no temperature data are available
Cihacres_eq::Cihacres_eq(int size, // array size
						 date_array date_in,
						 double *streamflow,
						 double *pcp,
						 double TwConst, double f, double c,
						 double aq, double as, double bq, double bs)
{
	// assign values to global parameters
	sizeAll = size;
	date = date_in;
	streamflow_obs.resize(sizeAll);
	precipitation.resize(sizeAll);
	for (int i = 0; i < sizeAll; i++)
	{
		streamflow_obs[i] = streamflow[i];
		precipitation[i]  = pcp[i];
	}
	this->TwConst	= TwConst;
	this->f			= f;
	this->c			= c;
	this->aq		= aq;
	this->as		= as;
	this->bq		= bq;
	this->bs		= bs;
	// Initialize Vectors
	_InitVectorsStart(sizeAll);
}
//---------------------------------------------------------------------
// two storages
// using double arrays as input
// if temperature data are available
Cihacres_eq::Cihacres_eq(int size, // array size
						 date_array date_in,
						 double *streamflow,
						 double *pcp,
						 double *tmp,
						 double TwConst, double f, double c,
						 double aq, double as, double bq, double bs)
{
	sizeAll = size;
	date = date_in;
	streamflow_obs.resize(size);
	precipitation.resize(size);
	temperature.resize(size);
	for (int i = 0; i < size; i++)
	{
		streamflow_obs[i] = streamflow[i];
		precipitation[i]  = pcp[i];
		temperature[i]    = tmp[i];
	}
	this->TwConst	= TwConst;
	this->f			= f;
	this->c			= c;
	this->aq		= aq;
	this->as		= as;
	this->bq		= bq;
	this->bs		= bs;
	// Initialize Vectors
	_InitVectorsStart((int)streamflow_obs.size());
}
//---------------------------------------------------------------------
// end constructors ///////////////////////////////////////////////////

// destructor
Cihacres_eq::~Cihacres_eq(void)
{
	_ZeroAllVectors();
}

///////////////////////////////////////////////////////////////////////
//
//                         PUBLIC MEMBER FUNCTIONS
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//						Run Non-Linear Module
//---------------------------------------------------------------------
void Cihacres_eq::RunNonLinearModule(bool TMP_data_exist, bool bSnowModule, double T_Rain)
{
	double WI_init = 0.5;
	double eR_init = 0.0;

	switch (IHAC_version)
	{
	case 0: // Jakeman & Hornberger (1993)
		// if temperature data are available (TMP_data_exist = true), then adjust the rate
		// at which the catchment wetness declines in the absence of rainfall
		// to daily temperature
		if (TMP_data_exist){
			CalcWetnessTimeConst(temperature, Tw, TwConst, f);
		}
		if (bSnowModule)
			{
			// calculate the catchment wetness index
			CalcWetnessIndex(Tw, precipitation, temperature,
				WetnessIndex, WI_init, c,
				bSnowModule, m_pSnowMod->Get_T_Rain());
			// calculate effective rainfall
			sum_eRainMM = CalcExcessRain(precipitation, temperature, WetnessIndex, excessRain, eR_init,
				sum_eRainGTpcp, bSnowModule, m_pSnowMod);
		} else {
			CalcWetnessIndex(Tw, precipitation, temperature, WetnessIndex, WI_init, c,
				bSnowModule, 0.0);
			sum_eRainMM = CalcExcessRain(precipitation, temperature, WetnessIndex, excessRain, eR_init,
				sum_eRainGTpcp, bSnowModule, m_pSnowMod);
		}
		break;

	case 1: // Croke et al. (2005)
		if (TMP_data_exist) {
			CalcWetnessTimeConst_Redesign(temperature, Tw, TwConst, f);
		}
		if (bSnowModule)
		{
			// calculate the catchment wetness index
			CalcWetnessIndex_Redesign(Tw, precipitation, WetnessIndex, bSnowModule, m_pSnowMod->Get_T_Rain());
			// calculate effective rainfall
			sum_eRainMM = CalcExcessRain_Redesign(precipitation, temperature,
				WetnessIndex, excessRain, eR_init, sum_eRainGTpcp,
				c, l, p, bSnowModule, m_pSnowMod);
		} else {
			// calculate the catchment wetness index
			CalcWetnessIndex_Redesign(Tw, precipitation, WetnessIndex, bSnowModule, 0.0);
			// calculate effective rainfall
			sum_eRainMM = CalcExcessRain_Redesign(precipitation, temperature,
				WetnessIndex, excessRain, eR_init, sum_eRainGTpcp,
				c, l, p, bSnowModule, m_pSnowMod);
		}
		break;
	}
}

//---------------------------------------------------------------------
//				   Simulate Streamflow (single storage)
//---------------------------------------------------------------------
void Cihacres_eq::SimStreamflowSingle(vector_d &excessRain, double initVal, 
									  vector_d &streamflow_sim, int delay,
									  double a, double b)
{
	int i;
	int size = (int)streamflow_sim.size();
	// using the first observed streamflow value as initial simulation value
	for (i = 0; i < delay; i++)
		streamflow_sim[i] = initVal;
	// start calculation with second value
	for (i = delay; i < size; i++)
	{
		streamflow_sim[i] = -a * streamflow_sim[i-1] + b * excessRain[i-delay];
	}
}
//---------------------------------------------------------------------
//				   Simulate Streamflow (single storage)
//---------------------------------------------------------------------
void Cihacres_eq::SimStreamflowSingle(double *excessRain, double initVal,
									  double *streamflow_sim, int delay,
									  double a, double b, int size)
{
	int i;
	// using the first observed streamflow value as initial simulation value
	for (i = 0; i < delay; i++)
		streamflow_sim[i] = initVal;
	// start calculation with second value
	for (i = delay; i < size; i++)
	{
		streamflow_sim[i] = -a * streamflow_sim[i-1] + b * excessRain[i-delay];
	}
}

//---------------------------------------------------------------------
//			   Simulate Streamflow (2 parallel storages)
//---------------------------------------------------------------------
void Cihacres_eq::SimStreamflow2Parallel(vector_d &excessRain, vector_d &streamflow_sim,
										 double initVal,
										 double aq, double as, double bq, double bs,
										 double &vq, double &vs,
										 int IHAC_vers, int delay)
{
	int i;
	int size = (int)streamflow_sim.size();
	double *sf_q = new double[size]; // quick streamflow component
	double *sf_s = new double[size]; // slow streamflow component

	// calculate the dependent b-value
	// after Jakeman etc.
	vq = bq / (1 + aq);
	vs = 1 - vq;

	// using the first observed streamflow value as initial simulation value
	for (i = 0; i < delay; i++)
	{
		streamflow_sim[i]	= initVal;
		sf_q[i]				= initVal * vq;
		sf_s[i]				= initVal * vs;
	}

	// using the first observed streamflow value as initial simulation value
	//for (i = 0; i < delay; i++)
	//{
	//	streamflow_sim[i]	= initVal;
	//	sf_q[i]				= initVal / 2;
	//	sf_s[i]				= initVal / 2;
	//}

	//// calculate the dependent b-value
	//if (IHAC_vers == 1)		// after Kokkonen
	//{
	//	vq = bq / (bq + bs);	
	//	vs = 1 - vq;
	//} else {				// after Jakeman etc.
	//	vq = bq / (1 + aq);
	//	vs = 1 - vq;
	//}

	// calculate streamflow
	for (i = delay; i < size; i++)
	{	
		sf_q[i] = -aq * sf_q[i-1] + bq * excessRain[i-delay];
		sf_s[i] = -as * sf_s[i-1] + bs * excessRain[i-delay];
		streamflow_sim[i] = sf_q[i] + sf_s[i];
	}
/*
	switch(IHAC_vers)
	{
	case 0 : // after Jakeman & Hornberger (1993)
		vq = bq / (1 + aq);
		vs = 1 - vq;
		if (!b_freebee)
			bs = vs * (1 + as);

		// calculate quick and slow components
		for (i = delay; i < size; i++)
		{
			sf_q[i] = -aq * sf_q[i-1] + bq * excessRain[i-delay];
			sf_s[i] = -as * sf_s[i-1] + bs * excessRain[i-delay];
			streamflow_sim[i] = sf_q[i] + sf_s[i];
		}
		break;
	case 1 : // after Kokkonen et al. (2003)
		vq = bq / (bq + bs);	
		vs = 1 - vq;
		if (!b_freebee)
			bs = vs * (1 + as);
		// calculate quick and slow components
		for (i = delay; i < size; i++)
		{
			sf_q[i] = -aq * sf_q[i-1] + bq * excessRain[i-delay];
			sf_s[i] = -as * sf_s[i-1] + bs * excessRain[i-delay];
			streamflow_sim[i] = sf_q[i] + sf_s[i];
		}
		break;
	case 2: // Ahandere Mahn
		vq = bq / (1 + aq);
		vs = 1 - vq;
		if (!b_freebee)
			bs = vs * (1 + as);

		// calculate quick and slow components
		for (i = delay; i < size; i++)
		{	
			sf_q[i] = -aq * sf_q[i-1] + bq * excessRain[i-delay];
			sf_s[i] = -as * sf_s[i-1] + bs * excessRain[i-delay];
			streamflow_sim[i] = sf_q[i] + sf_s[i];
		}
		break;
	case 3 : // after Croke et al. (2005)
		vq = bq / (1 + aq);
		vs = 1 - vq;
		if (!b_freebee)
			bs = vs * (1 + as);

		// calculate quick and slow components
		for (i = delay; i < size; i++)
		{	
			sf_q[i] = -aq * sf_q[i-1] + bq * excessRain[i-delay];
			sf_s[i] = -as * sf_s[i-1] + bs * excessRain[i-delay];
			streamflow_sim[i] = sf_q[i] + sf_s[i];
		}
		break;
	}
*/
	delete[] sf_q;
	delete[] sf_s;
}
//---------------------------------------------------------------------
//			   Simulate Streamflow (2 parallel storages)
//---------------------------------------------------------------------
void Cihacres_eq::SimStreamflow2Parallel(double *excessRain, double *streamflow_sim,
										 double initVal,
										 double aq, double as, double bq, double bs,
										 double &vq, double &vs,
										 int IHAC_vers, int size, int delay)
{
	int i;
	double *sf_q = new double[size]; // quick streamflow component
	double *sf_s = new double[size]; // slow streamflow component

	// calculate the dependent b-value
	// after Jakeman etc.
	vq = bq / (1 + aq);
	vs = 1 - vq;

	// using the first observed streamflow value as initial simulation value
	for (i = 0; i < delay; i++)
	{
		streamflow_sim[i]	= initVal;
		sf_q[i]				= initVal * vq;
		sf_s[i]				= initVal * vs;
	}

	//// using the first observed streamflow value as initial simulation value
	//for (i = 0; i < delay; i++)
	//{
	//	streamflow_sim[i]	= initVal;
	//	sf_q[i]				= initVal / 2;
	//	sf_s[i]				= initVal / 2;
	//}

	//// calculate the dependent b-value
	//if (IHAC_vers == 1)		// after Kokkonen
	//{
	//	vq = bq / (bq + bs);	
	//	vs = 1 - vq;
	//} else {				// after Jakeman etc.
	//	vq = bq / (1 + aq);
	//	vs = 1 - vq;
	//}

	// calculate streamflow
	for (i = delay; i < size; i++)
	{	
		sf_q[i] = -aq * sf_q[i-1] + bq * excessRain[i-delay];
		sf_s[i] = -as * sf_s[i-1] + bs * excessRain[i-delay];
		streamflow_sim[i] = sf_q[i] + sf_s[i];
	}
/*
	switch(IHAC_vers)
	{
	case 0 : // after Jakeman & Hornberger (1993)
		vq = bq / (1 + aq);
		vs = 1 - vq;
		if (!b_freebee)
			bs = vs * (1 + as);

		// calculate quick and slow components
		for (i = delay; i < size; i++)
		{	
			sf_q[i] = -aq * sf_q[i-1] + bq * excessRain[i-delay];
			sf_s[i] = -as * sf_s[i-1] + bs * excessRain[i-delay];
			streamflow_sim[i] = sf_q[i] + sf_s[i];
		}
		break;
	case 1 : // after Kokkonen et al. (2003)
		vq = bq / (bq + bs);	
		vs = 1 - vq;
		if (!b_freebee)
			bs = vs * (1 + as);
		// calculate quick and slow components
		for (i = delay; i < size; i++)
		{	
			sf_q[i] = -aq * sf_q[i-1] + bq * excessRain[i-delay];
			sf_s[i] = -as * sf_s[i-1] + bs * excessRain[i-delay];
			streamflow_sim[i] = sf_q[i] + sf_s[i];
		}
		break;
	case 2: // Jakeman & SnowModule
		vq = bq / (1 + aq);
		vs = 1 - vq;
		if (!b_freebee)
			bs = vs * (1 + as);

		// calculate quick and slow components
		for (i = delay; i < size; i++)
		{	
			sf_q[i] = -aq * sf_q[i-1] + bq * excessRain[i-delay];
			sf_s[i] = -as * sf_s[i-1] + bs * excessRain[i-delay];
			streamflow_sim[i] = sf_q[i] + sf_s[i];
		}
		break;
	case 3 : // after Croke et al. (2005)
		vq = bq / (1 + aq);
		vs = 1 - vq;
		if (!b_freebee)
			bs = vs * (1 + as);

		// calculate quick and slow components
		for (i = delay; i < size; i++)
		{	
			sf_q[i] = -aq * sf_q[i-1] + bq * excessRain[i-delay];
			sf_s[i] = -as * sf_s[i-1] + bs * excessRain[i-delay];
			streamflow_sim[i] = sf_q[i] + sf_s[i];
		}
		break;
	}
*/
	delete[] sf_q;
	delete[] sf_s;
}

//---------------------------------------------------------------------
//			   Simulate Streamflow (2 parallel storages)
//---------------------------------------------------------------------
void Cihacres_eq::SimStreamflow2Parallel(double *excessRain, double *streamflow_sim,
										 double initVal, // first observed streamflow value 
										 C_IHAC_LinearParms* linparms, int index,
										 double &vq, double &vs, int size, int delay)
{
	int i;
	double *sf_q = new double[size]; // quick streamflow component
	double *sf_s = new double[size]; // slow streamflow component

	// calculate the dependent b-value
	// after Jakeman etc.
	vq = linparms->bq[index] / (1 + linparms->aq[index]);
	vs = 1 - vq;

	// using the first observed streamflow value as initial simulation value
	for (i = 0; i < delay; i++)
	{
		streamflow_sim[i]	= initVal;
		sf_q[i]				= initVal * vq;
		sf_s[i]				= initVal * vs;
	}

	// calculate streamflow
	for (i = delay; i < size; i++)
	{	
		sf_q[i] = -linparms->aq[index] * sf_q[i-1] + linparms->bq[index] * excessRain[i-delay];
		sf_s[i] = -linparms->as[index] * sf_s[i-1] + linparms->bs[index] * excessRain[i-delay];
		streamflow_sim[i] = sf_q[i] + sf_s[i];
	}
	delete[] sf_q;
	delete[] sf_s;
}


//---------------------------------------------------------------------
//						Calculate Parameter b(q)
//---------------------------------------------------------------------
double Cihacres_eq::Calc_Parm_BS(double aq, double as, double bq)
{
	return( (1 - (bq / (1 + aq))) * (1 + as) );
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//						Calculate time of decay (quick or slow)
//---------------------------------------------------------------------
double Cihacres_eq::Calc_TimeOfDecay(double a)
{
	return( -1 / log(-a) );
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//						Calculate Wetness Time Constant
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessTimeConst(vector_d &temperature,
									   vector_d &Tw,
									   double TwConst,
									   double f)
{
	for (unsigned int i = 0; i < Tw.size(); i++)
	{
		Tw[i] = TwConst * exp((20.0 - temperature[i]) * f);
	}
}
//---------------------------------------------------------------------
//						Calculate Wetness Time Constant
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessTimeConst(double *temperature,
									   double *Tw,
									   double TwConst,
									   double f,
									   int size)
{
	for (int i = 0; i < size; i++)
	{
		Tw[i] = TwConst * exp((20.0 - temperature[i]) * f);
	}
}

//---------------------------------------------------------------------
//						Calculate Wetness Time Constant
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessTimeConst(double *temperature, double *Tw,
									   C_IHAC_NonLinearParms* nonlinparms, int index,
									   int size)
{
	for (int i = 0; i < size; i++)
	{
		Tw[i] = nonlinparms->mp_tw[index] * exp((20.0 - temperature[i]) * nonlinparms->mp_f[index]);
	}
}

//---------------------------------------------------------------------
//						Calculate Wetness Time Constant
//		For ihacres_climate_scen
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessTimeConst_scen(double *temperature, double *Tw,
									   C_IHAC_NonLinearParms* nonlinparms, int index,
									   int size)
{
	Tw[0] = 0.0;
	for (int i = 1; i < size; i++)
	{
		Tw[i] = nonlinparms->mp_tw[index] * exp((20.0 - temperature[i]) * nonlinparms->mp_f[index]);
	}
}

//---------------------------------------------------------------------
//			Calculate Wetness Time Constant (Croke et al. 2005)
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessTimeConst_Redesign(vector_d &temperature,
									   vector_d &Tw,
									   double TwConst,
									   double f)
{
	double Tr = 20.0; // reference temperature
	for (unsigned int i = 0; i < Tw.size(); i++)
	{
		Tw[i] = TwConst * exp(0.062 * f * (Tr - temperature[i]));
	}
}

//---------------------------------------------------------------------
//			Calculate Wetness Time Constant (Croke et al. 2005)
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessTimeConst_Redesign(double *temperature,
									   double *Tw,
									   double TwConst,
									   double f,
									   int size)
{
	double Tr = 20.0; // reference temperature
	for (int i = 0; i < size; i++)
	{
		Tw[i] = TwConst * exp(0.062 * f * (Tr - temperature[i]));
	}
}
//---------------------------------------------------------------------
//			Calculate Wetness Time Constant (Croke et al. 2005)
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessTimeConst_Redesign(double *temperature, double *Tw,
									   C_IHAC_NonLinearParms* nonlinparms, int index,
									   int size)
{
	double Tr = 20.0; // reference temperature
	for (int i = 0; i < size; i++)
	{
		Tw[i] = nonlinparms->mp_tw[index] * exp(0.062 * nonlinparms->mp_f[index] * (Tr - temperature[i]));
	}
}

//---------------------------------------------------------------------
//						Calculate Wetness Index
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessIndex(vector_d &Tw,
								   vector_d &precipitation, vector_d &temperature,
								   vector_d &WetnessIndex, double WI_init,
								   double c, bool bSnowModule, double T_Rain)
{
	WetnessIndex[0] = WI_init;
	// starting at the second value (i=1)
	for (unsigned int i = 1; i < WetnessIndex.size(); i++)
	{
		if (bSnowModule && temperature[i] < T_Rain)
		{
			WetnessIndex[i] = (1 - (1 / Tw[i])) * WetnessIndex[i-1];
		} else {
			WetnessIndex[i] = c * precipitation[i] + (1 - (1 / Tw[i])) * WetnessIndex[i-1];
		}
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// if (WetnessIndex[i] > 1.0) WetnessIndex[i] = 1.0;
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
}
//---------------------------------------------------------------------
//						Calculate Wetness Index
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessIndex(double *Tw,
								   double *precipitation, double *temperature,
								   double *WetnessIndex, double WI_init,
								   double c, bool bSnowModule, double T_Rain,
								   int size)
{
	//WetnessIndex[0] = 0.5;
	WetnessIndex[0] = WI_init;
	// starting at the second value (i=1)
	for (int i = 1; i < size; i++)
	{
		if (bSnowModule && temperature[i] < T_Rain)
		{
			WetnessIndex[i] = (1 - (1 / Tw[i])) * WetnessIndex[i-1];
		} else {
			WetnessIndex[i] = c * precipitation[i] + (1 - (1 / Tw[i])) * WetnessIndex[i-1];
		}
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// if (WetnessIndex[i] > 1.0) WetnessIndex[i] = 1.0;
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
}

//---------------------------------------------------------------------
//			Calculate Wetness Index after Croke et al. (2005)
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessIndex_Redesign(vector_d &Tw,
								   vector_d &precipitation,
								   vector_d &WetnessIndex, bool bSnowModule,double T_Rain)
{
	WetnessIndex[0] = 0.5;
	// starting at the second value (i=1)
	for (unsigned int i = 1; i < WetnessIndex.size(); i++)
	{
		WetnessIndex[i] = precipitation[i] + (1 - (1 / Tw[i])) * WetnessIndex[i-1];
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// if (WetnessIndex[i] > 1.0) WetnessIndex[i] = 1.0;
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
}
//---------------------------------------------------------------------
//			Calculate Wetness Index after Croke et al. (2005)
//---------------------------------------------------------------------
void Cihacres_eq::CalcWetnessIndex_Redesign(double *Tw,
								   double *precipitation,double *WetnessIndex, double WI_init,
								   bool bSnowModule,double T_Rain,
								   int size)
{
	//WetnessIndex[0] = 0.5;
	WetnessIndex[0] = WI_init;
	// starting at the second value (i=1)
	for (int i = 1; i < size; i++)
	{
		WetnessIndex[i] = precipitation[i] + (1 - (1 / Tw[i])) * WetnessIndex[i-1];
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// if (WetnessIndex[i] > 1.0) WetnessIndex[i] = 1.0;
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
}
//---------------------------------------------------------------------
//				Calculate Effective or Excess Rainfall
//---------------------------------------------------------------------
double Cihacres_eq::CalcExcessRain(vector_d &precipitation, vector_d &temperature, vector_d &WetnessIndex,
								   vector_d &excessRain, double eR_init, double &sum_eRainGTpcp,
								   bool bSnowModule, CSnowModule* pSnowModule)
								   //double T_Rain, double T_Melt, double* MeltRate)
{
	double sum = 0.0;		// sum of total ExcessRain of the period
	sum_eRainGTpcp = 0.0;

	excessRain[0] = eR_init;
	if (precipitation[0] > 0.0) excessRain[0] = precipitation[0] / 2;
	// starting at the second value (i=1)
	for (unsigned int i = 1; i < excessRain.size(); i++)
	{
		// "excess" rainfall after Jakeman & Hornberger (1993)
		// ExcessRain[i] = pcp[i] * WetnessIndex[i];

		// "excess" rainfall after Croke et al. (2004)
		excessRain[i] = precipitation[i] * ((WetnessIndex[i] + WetnessIndex[i-1]) / 2);

		if (excessRain[i] > precipitation[i])
		{
			// if calculated excess rain volume is greater than observed precipitation,
			// then summarize volume differences and set to current pcp value
			sum_eRainGTpcp += excessRain[i] - precipitation[i];
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// if excess rainfall is greater than observed precip, then
			// it is reduced to precip!
			//excessRain[i] = precipitation[i];
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}

		if (excessRain[i] < 0.0) excessRain[i] = 0.0;

		// snow module ********************************************
		if (bSnowModule)
		{
			if (temperature[i] < pSnowModule->Get_T_Rain()) excessRain[i] = 0.0;
			if (temperature[i] > pSnowModule->Get_T_Melt()) excessRain[i] += pSnowModule->Get_MeltRate(i);
			if ((temperature[i] < pSnowModule->Get_T_Melt()) && (temperature[i] > pSnowModule->Get_T_Rain()))
				excessRain[i] += pSnowModule->Get_MeltRate(i);
		}
		// end snow module ****************************************

		sum += excessRain[i];
	}
	sum += excessRain[0]; // add the initial value
	return sum;
}
//---------------------------------------------------------------------
//				Calculate Effective or Excess Rainfall
//---------------------------------------------------------------------
double Cihacres_eq::CalcExcessRain(double *precipitation, double* temperature, double *WetnessIndex,
								   double *excessRain, double eR_init,
								   double &sum_eRainGTpcp, int size,
								   bool bSnowModule, double T_Rain, double T_Melt, double* MeltRate)
{
	double sum = 0.0;		// sum of total ExcessRain of the period
	sum_eRainGTpcp = 0.0;

	//excessRain[0] = 0.0;
	excessRain[0] = eR_init;
	//if (precipitation[0] > 0.0) excessRain[0] = precipitation[0] / 2;
	// starting at the second value (i=1)
	for (int i = 1; i < size; i++)
	{
		// "excess" rainfall after Jakeman & Hornberger (1993)
		// ExcessRain[i] = pcp[i] * WetnessIndex[i];

		// "excess" rainfall after Croke et al. (2004)
		excessRain[i] = precipitation[i] * ((WetnessIndex[i] + WetnessIndex[i-1]) / 2);

		if (excessRain[i] > precipitation[i])
		{
			// if calculated excess rain volume is greater than observed precipitation,
			// then summarize volume differences and set to current pcp value
			sum_eRainGTpcp += excessRain[i] - precipitation[i];
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// if excess rainfall is greater than observed precip, then
			// it is reduced to precip!
			//excessRain[i] = precipitation[i];
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		if (excessRain[i] < 0.0) excessRain[i] = 0.0;

		// snow module ********************************************
		if (bSnowModule)
		{
			if (temperature[i] < T_Rain) excessRain[i] = 0.0;
			if (temperature[i] > T_Melt) excessRain[i] += MeltRate[i];
			if ((temperature[i] < T_Melt) && (temperature[i] > T_Rain))
				excessRain[i] += MeltRate[i];
		}
		// end snow module ****************************************

		sum += excessRain[i];
	}
	sum += excessRain[0]; // add the initial value
	return sum;
}

//---------------------------------------------------------------------
//	Calculate Effective or Excess Rainfall after Croke et al. (2005)
//---------------------------------------------------------------------
double Cihacres_eq::CalcExcessRain_Redesign(vector_d &precipitation, vector_d& temperature, vector_d &WetnessIndex,
								   vector_d &excessRain, double eR_init, double &sum_eRainGTpcp,
								   double c, double l, double p, bool bSnowModule, CSnowModule* pSnowMod)
{
	double sum = 0.0;		// sum of total ExcessRain of the period
	sum_eRainGTpcp = 0.0;

	excessRain[0] = eR_init;
	if (precipitation[0] > 0.0) excessRain[0] = precipitation[0] / 2;
	// starting at the second value (i=1)
	for (unsigned int i = 1; i < excessRain.size(); i++)
	{
		// "excess" rainfall after Jakeman & Hornberger (1993)
		// ExcessRain[i] = pcp[i] * WetnessIndex[i];

		// "excess" rainfall after Croke et al. (2004)
		// excessRain[i] = precipitation[i] * ((WetnessIndex[i] + WetnessIndex[i-1]) / 2);
		
		// excess rainfall after Croke et al. (2005)
		// The first equation is as described in the paper,
		// but Barry Croke told me that parameter "c" must be outside the brackets !!!
		// excessRain[i] = pow((c *(WetnessIndex[i] - l)),p) * precipitation[i];
		if ( WetnessIndex[i] - l < 0.0 ) {
			excessRain[i] = 0.0;
		} else {
			excessRain[i] = c * pow((WetnessIndex[i] - l),p) * precipitation[i];
		}

		if (excessRain[i] > precipitation[i])
		{
			// if calculated excess rain volume is greater than observed precipitation,
			// then summarize volume differences and set to current pcp value
			sum_eRainGTpcp += excessRain[i] - precipitation[i];
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// if excess rainfall is greater than observed precip, then
			// it is reduced to precip!
			//excessRain[i] = precipitation[i];
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		if (excessRain[i] < 0.0) excessRain[i] = 0.0;

		// snow module ********************************************
		if (bSnowModule)
		{
			if (temperature[i] < pSnowMod->Get_T_Rain()) excessRain[i] = 0.0;
			if (temperature[i] > pSnowMod->Get_T_Melt()) excessRain[i] += pSnowMod->Get_MeltRate(i);
			if ((temperature[i] < pSnowMod->Get_T_Melt()) && (temperature[i] > pSnowMod->Get_T_Rain()))
				excessRain[i] += pSnowMod->Get_MeltRate(i);
		}
		// end snow module ****************************************

		sum += excessRain[i];
	}
	sum += excessRain[0]; // add the initial value
	return sum;
}
//---------------------------------------------------------------------
//	Calculate Effective or Excess Rainfall after Croke et al. (2005)
//---------------------------------------------------------------------
double Cihacres_eq::CalcExcessRain_Redesign(double *precipitation, double* temperature, double *WetnessIndex,
								   double *excessRain, double eR_init,
								   double &sum_eRainGTpcp,
								   int size, double c, double l, double p,
								   bool bSnowModule, double T_Rain, double T_Melt, double* MeltRate)
{
	double sum = 0.0;		// sum of total ExcessRain of the period
	sum_eRainGTpcp = 0.0;

	//excessRain[0] = 0.0;
	excessRain[0] = eR_init;
	//if (precipitation[0] > 0.0) excessRain[0] = precipitation[0] / 2;
	// starting at the second value (i=1)
	for (int i = 1; i < size; i++)
	{
		// excess rainfall after Croke et al. (2005)
		// The first equation is as described in the paper,
		// but Barry Croke told me that parameter "c" must be outside the brackets !!!
		// excessRain[i] = pow((c *(WetnessIndex[i] - l)),p) * precipitation[i];
		excessRain[i] = c * pow((WetnessIndex[i] - l),p) * precipitation[i];

		if (excessRain[i] > precipitation[i])
		{
			// if calculated excess rain volume is greater than observed precipitation,
			// then summarize volume differences and set to current pcp value
			sum_eRainGTpcp += excessRain[i] - precipitation[i];
			excessRain[i] = precipitation[i];
		}
		if (excessRain[i] < 0.0) excessRain[i] = 0.0;

		// snow module ********************************************
		if (bSnowModule)
		{
			if (temperature[i] < T_Rain) excessRain[i] = 0.0;
			if (temperature[i] > T_Melt) excessRain[i] += MeltRate[i];
			if ((temperature[i] < T_Melt) && (temperature[i] > T_Rain))
				excessRain[i] += MeltRate[i];
		}
		// end snow module ****************************************

		sum += excessRain[i];
	}
	sum += excessRain[0]; // add the initial value
	return sum;
}



//---------------------------------------------------------------------
//			    Assign first and last record (user selection)
//---------------------------------------------------------------------
void Cihacres_eq::AssignFirstLastRec(CSG_Table &pTable, int &first, int &last,
									 CSG_String date1, CSG_String date2, int dateField)
{
	int		j;
	///////////////////////////////////////////////////////////////
	// searching the first and the last record of the time range
	///////////////////////////////////////////////////////////////

	for (j = 0; j < pTable.Get_Record_Count(); j++)
	{
		if (!date1.Cmp(pTable.Get_Record(j)->asString(dateField)))
		{
			first = j;
		}
		else if (!date2.Cmp(pTable.Get_Record(j)->asString(dateField)))
		{
			last = j;
		}
	}
}

//---------------------------------------------------------------------

int Cihacres_eq::Assign_nStorages(int storconf)
{
	switch(storconf)
	{
		case 0: return(1); break;
		case 1: return(2); break;
		case 2: return(2); break;
		default : return(1);
	}
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
//                 Summarize all Values in a Vector
//---------------------------------------------------------------------
double Cihacres_eq::SumVector(vector_d &input)
{
	double sum = 0;
	for (unsigned int j = 0; j < input.size(); j++)
	{
		sum += input[j];
	}
	return sum;
}

//---------------------------------------------------------------------
//                 Summarize all Values in an Array
//---------------------------------------------------------------------
double Cihacres_eq::SumVector(double *input, int size)
{
	double sum = 0;
	for (int j = 0; j < size; j++)
	{
		sum += input[j];
	}
	return sum;
}


//---------------------------------------------------------------------
// Assign temporary Nash-Sutcliffe efficiency according to
// objective function (NSE, NSE_highflow, NSE_lowflow)
//---------------------------------------------------------------------
double Cihacres_eq::_Assign_NSE_temp(int obj_func, double NSE, double NSE_highflow, double NSE_lowflow)
{
	switch(obj_func)
	{
		case 0: // NSE
			return(NSE);
		case 1: // NSE high flow
			return(NSE_highflow);
		case 2: // NSE low flow (not yet implemented!)
			return(NSE_lowflow);
			break;
		default: return(NSE);
	} // end switch(m_obj_func)
}
//---------------------------------------------------------------------



///////////////////////////////////////////////////////////////////////
//
//                         GET FUNCTIONS
//
///////////////////////////////////////////////////////////////////////
double Cihacres_eq::get_vq()
{
	// return (bq / (bq + bs));
	return(vq);
}
//---------------------------------------------------------------------
double Cihacres_eq::get_vs()
{
	// return (1 - get_vq());
	return(vs);
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
double Cihacres_eq::get_sum_streamflowMM_obs(int size)
{
	double sum_streamflowMM_obs = 0.0;
	// calculate sum of observed streamflow in [mm]
	for (int j = 0; j < size; j++)
	{
		sum_streamflowMM_obs += streamflowMM_obs[j];
	}
	return(sum_streamflowMM_obs);
}
//---------------------------------------------------------------------
double Cihacres_eq::get_sum_precipitation(int size)
{
	double sum_pcp = 0.0;
	for (int i = 0; i < size; i++)
	{
		sum_pcp += precipitation[i];
	}
	return(sum_pcp);
}
//---------------------------------------------------------------------
double Cihacres_eq::get_NSE()
{
	return(NSE);
}
//---------------------------------------------------------------------
vector_d Cihacres_eq::get_streamflow_sim()
{
	return(streamflow_sim);
}
//---------------------------------------------------------------------
vector_d Cihacres_eq::get_excessRain()
{
	return(excessRain);
}
//---------------------------------------------------------------------
vector_d Cihacres_eq::get_WetnessIndex()
{
	return(WetnessIndex);
}
//---------------------------------------------------------------------
vector_d Cihacres_eq::get_Tw()
{
	return(Tw);
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
//
//                         PRIVATE MEMBER FUNCTIONS
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//				    Initialize (Resize) global Vectors
//---------------------------------------------------------------------
void Cihacres_eq::_InitVectorsStart(int size)
{
	streamflow_sim.resize(size);
	excessRain.resize(size);
	WetnessIndex.resize(size);
	Tw.resize(size);
	streamflowMM_obs.resize(size);
	//if (IHAC_version == 2)
	//{
	//	m_pSnowMod->SnowStorage = new double[size];
	//	m_pSnowMod->MeltRate = new double[size];
	//	for (int i = 0; i < size; i++)
	//	{
	//		m_pSnowMod->SnowStorage[i] = 0.0;
	//		m_pSnowMod->MeltRate[i] = 0.0;
	//	}
	//	//SnowModule(m_pSnowMod->SnowStorage,m_pSnowMod->MeltRate,temperature,precipitation, m_pSnowMod->T_Rain,m_pSnowMod->T_Melt,m_pSnowMod->DD_FAC, size);
	//}
}

//---------------------------------------------------------------------
//							Zero all Vectors
//---------------------------------------------------------------------
void Cihacres_eq::_ZeroAllVectors()
{
	// Resize all Vectors to Zero
	streamflow_sim.resize(0);
	excessRain.resize(0);
	WetnessIndex.resize(0);
	Tw.resize(0);
	date.resize(0);
	streamflow_obs.resize(0);
	precipitation.resize(0);
	temperature.resize(0);
}
