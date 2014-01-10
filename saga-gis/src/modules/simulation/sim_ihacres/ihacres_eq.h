/**********************************************************
 * Version $Id: ihacres_eq.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
///////////////////////////////////////////////////////////
//                    ihacres_eq.h                       //
//                                                       //
//                 Copyright (C) 2006 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//	e-mail:       stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com               //
//                     2006-08-27                        //
//	modified:		   2008-01-28						 //
//-------------------------------------------------------//
//
// References:											 
// Jakeman, A.J. / Hornberger, G.M (1993).				 
//   How Much Complexity Is Warranted in a				 
//	 Rainfall-Runoff Model?								 
//	 Water Resources Research, (29), NO. 8 (2637-2649)   
// Kokkonen, T. S. et al. (2003).
//   Predicting daily flows in ungauged catchments:
//   model regionalization from catchment descriptors
//   at the Coweeta Hydrologic Laboratory, North Carolina
//   Hydrological Processes (17), 2219-2238
// Croke, B. F. W., W. S. Merritt, et al. (2004).
//   A dynamic model for predicting hydrologic response
//   to land cover changes in gauged and
//   ungauged catchments.
//   Journal Of Hydrology 291(1-2): 115-131.
// Croke, B. F. W., Andrews, F., Jakeman, A. J., Cuddy, S., Luddy, A.:
//	(2005). Redesign of the IHACRES rainfall-runoff model
///////////////////////////////////////////////////////////

//*******************************************************//
//                     NOTES							 //
//*******************************************************//
// This class provides functions of the rainfall-runoff
// model IHACRES. On the one hand the class can be used as
// a provider of functions using the standard constructor
// or the class can be instantiated and then performs the 
// calculations automatically.
// 
// Every function working with fields can be provided with
// arrays (pointer) or vector. You find for both solutions
// the appropriate function.
//*******************************************************//

//*******************************************************//
//                        ToDo                           //
//-------------------------------------------------------//
// - Difference eRain-streamflow_obs
// - Tau(q) and Tau(s)
// - Tau-values for different storages
// - if TMP_data_exist = false ???
// - hourly data (not only daily)
//
// - SNOW MODULE
//		- snow module on/off
//		- snow module integration in CalcExcessRain (add bool variable)
// - implementation of an instanteneous store
// - implement more efficiency criterions
//*******************************************************//

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__ihacres_eq_H
#define HEADER_INCLUDED__ihacres_eq_H
//---------------------------------------------------------

#include <saga_api/saga_api.h>	// CSG_Table

#include <vector>   // used for storing date string values in array
					// and double arrays (streamflow, pcp, tmp ...)
#include <string>

using namespace std;

#include "snow_module.h"
#include "model_tools.h"

typedef std::vector<std::string> date_array;
typedef vector<double> vector_d;
//---------------------------------------------------------

//---------------------------------------------------------
//  A CLASS TO DEAL WITH PARAMETERS OF THE
//	LINEAR STORAGE MODULE
//---------------------------------------------------------
class C_IHAC_LinearParms
{
public:
	C_IHAC_LinearParms() {
		_ZeroPointers();
	}
	C_IHAC_LinearParms(int size, int nStorages) {
		_ZeroPointers();
		this->nStorages = nStorages;
		if (nStorages == 1)
		{
			a = new double[size];
			b = new double[size];
		}
		if (nStorages == 2)
		{
			aq = new double[size];
			as = new double[size];
			bq = new double[size];
			bs = new double[size];
		}
	}
	~C_IHAC_LinearParms(void) {
		if (nStorages == 1)
		{
			if (a) delete[] a;
			if (b) delete[] b;
		}
		if (nStorages == 2)
		{			
			if (aq) delete[] aq;
			if (as) delete[] as;
			if (bq) delete[] bq;
			if (bs) delete[] bs;
		}
	}
	
	// linear module parameters
	int			nStorages;
	double*		a;
	double*		b;
	double*		aq;
	double*		as;
	double*		bq;
	double*		bs;

private:
	void _ZeroPointers() {
		a = NULL;
		b = NULL;
		aq = NULL;
		as = NULL;
		bq = NULL;
		bs = NULL;
	}
};
//---------------------------------------------------------

//---------------------------------------------------------
//  A CLASS TO DEAL WITH PARAMETERS OF THE
//	NON-LINEAR RAINFALL-LOSSES MODULE
//---------------------------------------------------------
class C_IHAC_NonLinearParms
{
public:
	C_IHAC_NonLinearParms() {
		mp_tw	= NULL;
		mp_f	= NULL;
		mp_c	= NULL;
		mp_l	= NULL;
		mp_p	= NULL;
		mp_eR_flow_dif	= NULL;
	};
	C_IHAC_NonLinearParms(int size) {
		mp_tw = new double[size];
		mp_f = new double[size];
		mp_c = new double[size];
		mp_l = new double[size];
		mp_p = new double[size];
		mp_eR_flow_dif = new double[size];
	};
	~C_IHAC_NonLinearParms(void) {
		if (mp_tw) delete[] mp_tw;
		if (mp_f) delete[] mp_f;
		if (mp_c) delete[] mp_c;
		if (mp_l) delete[] mp_l;
		if (mp_p) delete[] mp_p;
		if (mp_eR_flow_dif) delete[] mp_eR_flow_dif;
	};
	// non-linear module parms
	double*		mp_tw;
	double*		mp_f;
	double*		mp_c;
	double*		mp_l;
	double*		mp_p;
	double*		mp_eR_flow_dif;
};
//---------------------------------------------------------


///////////////////////////////////////////////////////////////////////
//
//		CLASS Cihacres_eq
//
///////////////////////////////////////////////////////////////////////
class Cihacres_eq
{
public:

	///////////////////////////////////////////////////////////////////
	//
	//							CONSTRUCTORS
	//
	///////////////////////////////////////////////////////////////////

	// default
	Cihacres_eq();

	// two storages
	// using vector<double> as input
	// if no temperature data are available
	Cihacres_eq		(date_array date,
					 vector_d streamflow_obs,
					 vector_d precipitation,
					 double Tw, double f, double c,
					 double l, double p,
					 double aq, double as, double bq, double bs);
	// two storages
	// using vector<double> as input
	// if temperature data are available
	Cihacres_eq		(date_array date,
					 vector_d streamflow_obs,
					 vector_d precipitation,
					 vector_d temperature,
					 double Tw, double f, double c,
					 double l, double p,
					 double aq, double as, double bq, double bs,
					 double area, bool TMP_data_exist,
					 int IHAC_vers,
					 int storconf,
					 bool bSnowModule,
					 CSnowModule *SnowMod,
					 //double T_Rain,
					 //double T_Melt,
					 //double DD_FAC,
					 int delay);
	
	// two storages
	// using double arrays as input
	// if no temperature data are available
	Cihacres_eq		(int size, // array size
					 date_array date,
					 double *streamflow_obs,
					 double *precipitation,
					 double Tw, double f, double c,
					 double aq, double as, double bq, double bs);
	// two storages
	// if temperature data are available
	// using double arrays as input
	Cihacres_eq		(int size, // array size
					 date_array date,
					 double *streamflow_obs,
					 double *precipitation,
					 double *temperature,
					 double Tw, double f, double c,
					 double aq, double as, double bq, double bs);

	// end constructors ///////////////////////////////////////////////

	// destructor
	~Cihacres_eq(void);

	///////////////////////////////////////////////////////////////////
	//
	// PUBLIC MEMEBER VARIABLES
	//
	///////////////////////////////////////////////////////////////////

	// Variables
	int				sizeAll;		// incoming data array size (number of records in input data)

	CSnowModule		*m_pSnowMod;	// class Snow Module

	double*			m_pMeltRate;

	///////////////////////////////////////////////////////////////////
	//
	// PUBLIC MEMBER FUNCTIONS
	//
	///////////////////////////////////////////////////////////////////

	//--------------------------------------------------------

	void			RunNonLinearModule		(bool TMP_data_exist, bool bSnowModule, double T_Rain);

	// simulate streamflow (single storage)
	void			SimStreamflowSingle		(vector_d &excessRain, double initVal,
											 vector_d &streamflow_sim, int delay,
											 double a, double b);
	
	void			SimStreamflowSingle		(double *excessRain, double initVal,
											 double *streamflow_sim, int delay,
											 double a, double b, int size);

	// simulate streamflow with two parallel storages
	void			SimStreamflow2Parallel	(vector_d &excessRain,
											 vector_d &streamflow_sim,
											 double initVal, // first observed streamflow value 
											 double aq, double as, double bq, double bs,
											 double &vq, double &vs,
											 int IHAC_vers,
											 int delay);
	
	void			SimStreamflow2Parallel	(double *excessRain,
											 double *streamflow_sim,
											 double initVal, // first observed streamflow value 
											 double aq, double as, double bq, double bs,
											 double &vq, double &vs,
											 int IHAC_vers, int size,
											 int delay);

	void			SimStreamflow2Parallel	(double *excessRain,
											 double *streamflow_sim,
											 double initVal, // first observed streamflow value 
											 C_IHAC_LinearParms* linparms, int index,
											 double &vq, double &vs, int size, int delay);

	double			Calc_Parm_BS			(double aq, double as, double bq);

	// calculate time of decay for quick or slow component (aq or bq)
	double			Calc_TimeOfDecay		(double a);

	//--------------------------------------------------------

	// calculating the time constant, or inversely the rate at which
	// the catchment wetness declines in the absence of rainfall
	void			CalcWetnessTimeConst	(vector_d &temperature,
											 vector_d &Tw,
											 double TwConst,
											 double f);
	
	void			CalcWetnessTimeConst	(double *temperature,
											 double *Tw,
											 double TwConst,
											 double f,
											 int size);

	void			CalcWetnessTimeConst	(double* temperature, double* Tw,
											 C_IHAC_NonLinearParms* nonlinparms, int index,
											 int size);

	// For ihacres_climate_scen
	void			CalcWetnessTimeConst_scen(double* temperature, double* Tw,
											 C_IHAC_NonLinearParms* nonlinparms, int index,
											 int size);

	// modified version after Croke et al. (2005)
	void			CalcWetnessTimeConst_Redesign(vector_d &temperature,
											 vector_d &Tw,
											 double TwConst,
											 double f);

	// modified version after Croke et al. (2005)
	void			CalcWetnessTimeConst_Redesign(double *temperature,
											 double *Tw,
											 double TwConst,
											 double f,
											 int size);

	void			CalcWetnessTimeConst_Redesign(double *temperature, double *Tw,
											 C_IHAC_NonLinearParms* nonlinparms, int index,
											 int size);

	// calculating the catchment wetness index,
	// or antecedent precipitation index
	void			CalcWetnessIndex		(vector_d &Tw,
											 vector_d &precipitation, vector_d &temperature,
											 vector_d &WetnessIndex, double WI_init,
											 double c, bool bSnowModule, double T_Rain);

	void			CalcWetnessIndex		(double *Tw,
											 double *precipitation, double *temperature,
											 double *WetnessIndex, double WI_init,
											 double c, bool bSnowModule, double T_Rain,
											 int size);

	// modified version after Croke et al. (2005)
	void			CalcWetnessIndex_Redesign(vector_d &Tw,
											 vector_d &precipitation,
											 vector_d &WetnessIndex,
											 bool bSnowModule,double T_Rain);

	// modified version after Croke et al. (2005)
	void			CalcWetnessIndex_Redesign(double *Tw,
											 double *precipitation,
											 double *WetnessIndex, double WI_init,
											 bool bSnowModule,double T_Rain,
											 int size);

	// calculate the effecive or excess rainfall and
	// calculate total effective rainfall over the period in [mm]
	double			CalcExcessRain			(vector_d &precipitation,
											 vector_d &temperature,
											 vector_d &WetnessIndex,
											 vector_d &excessRain, double eR_init,
											 double   &sum_eRainGTpcp,
											 bool bSnowModule,
											 CSnowModule* pSnowModule);

	double			CalcExcessRain			(double *precipitation,
											 double *temperature,
											 double *WetnessIndex,
											 double *excessRain, double eR_init,
											 double &sum_eRainGTpcp,
											 int size,
											 bool bSnowModule, double T_Rain, double T_Melt, double* MeltRate);

	// modified version after Croke et al. (2005)
	double			CalcExcessRain_Redesign	(vector_d &precipitation,
											 vector_d &temperature,
											 vector_d &WetnessIndex,
											 vector_d &excessRain, double eR_init,
											 double &sum_eRainGTpcp,
											 double	c,	// mass balance parameter
											 double l,	// soil moisture index threshold
											 double p,	// power on soil moisture
											 bool bSnowModule, CSnowModule* m_pSnowMod);

	// modified version after Croke et al. (2005)
	double			CalcExcessRain_Redesign	(double *precipitation,
											 double *temperature,
											 double *WetnessIndex,
											 double *excessRain, double eR_init,
											 double &sum_eRainGTpcp,
											 int size,
											 double c,	// mass balance parameter
											 double l,	// soil moisture index threshold
											 double p,	// power on soil moisture
											 bool bSnowModule, double T_Rain, double T_Melt, double* MeltRate);
	//--------------------------------------------------------


	//--------------------------------------------------------
	void			AssignFirstLastRec		(CSG_Table &pTable,
											 int &first, int &last,
											 CSG_String date1,CSG_String date2,
											 int dateField);

	int				Assign_nStorages		(int storconf);

	double			SumVector				(vector_d &input);
	double			SumVector				(double *input, int size);

	double			_Assign_NSE_temp		(int obj_func,
											 double NSE,
											 double NSE_highflow,
											 double NSE_lowflow);

	//--------------------------------------------------------
	// Get Functions
	//--------------------------------------------------------
	double			get_vq();		// fraction of quick flow
	double			get_vs();		// fraction of slow flow
	double			get_sum_streamflowMM_obs(int size); // sum of observed streamflow in [mm]
	double			get_sum_precipitation(int size);
	double			get_NSE();

	vector_d		get_streamflow_sim();
	vector_d		get_excessRain();
	vector_d		get_WetnessIndex();
	vector_d		get_Tw();

private:

	///////////////////////////////////////////////////////////////////
	// Private Member Variables
	///////////////////////////////////////////////////////////////////

	// incoming arrays / vectors
	date_array		date;			// Vector containing date values
	vector_d		streamflow_obs;	// Vector containing observed streamflow data
	vector_d		precipitation;	// Vector containing measured precipitation
	vector_d		temperature;	// Vector containing measured temperature
	vector_d		streamflowMM_obs; // Streamflow time series in [mm]

	//
	double			sum_eRainGTpcp;	// sum of excess rainfall that is greater
									// than precipitation in one simulation

	// Vectors (arrays)
	vector_d		streamflow_sim;	// Vector containing simulated streamflow data
	vector_d		excessRain;		// Vector containing estimated excess or effective rainfall
	vector_d		WetnessIndex;	// catchment wetness index
	vector_d		Tw;

	// parameters
	bool			TMP_data_exist; // if temperature data are available (TMP_data_exist = true)
	double			RainRunoffCoef;	// Rainfall-Runoff coefficient
	double			NSE;			// Nash-Sutcliffe Efficiency

	double			sum_streamflow_obs;
	double			sum_streamflow_sim;
	double			sum_eRainMM;	// sum of estimated effective rainfall [mm]

	// non-linear IHACRES module parameters
	double			c;
	double			f;
	double			TwConst;		// Tw is approximately the time constant, or inversely,
									// the rate at which the catchment wetness declines
									// in the absence of rainfall.
	// Additional parameters for Croke et al. (2005) Redesign version
	double			l;				// soil moisture index threshold
	double			p;				// power on soil moisture

	// linear IHACRES module parameters
	double			a;
	double			b;

	double			aq;
	double			as;
	double			bq;
	double			bs;

	double			vq;				// fraction of quick flow
	double			vs;				// fraction of slow flow

	int				delay;			// The delay after the start of rainfall,
									// before the discharge starts to rise.

	double			area;			// area of the watershed in [km2]

	int				IHAC_version;	// Different versions of IHACRES exist, corresponding
									// to the version...

	bool			bSnowModule;	// if true, snow module is active

	/*
	// snow module parameters
	vector_d		SnowStorage;
	vector_d		MeltRate;
	double			T_Rain;
	double			T_Melt;
	double			DD_FAC;
	*/
	///////////////////////////////////////////////////////////////////
	// Private Member Functions
	///////////////////////////////////////////////////////////////////

	// initialize Vectors
	void			_InitVectorsStart		(int size);

	// Resize all vectors *.resize(0)
	void			_ZeroAllVectors			();

}; // end class ihacres_eq

//---------------------------------------------------------
#endif /* #ifndef HEADER_INCLUDED__ihacres_eq_H */
