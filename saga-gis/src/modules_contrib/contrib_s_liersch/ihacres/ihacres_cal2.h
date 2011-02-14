/**********************************************************
 * Version $Id$
 *********************************************************/
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        IHACRES                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    ihacres_cal2.h                     //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com                   //
//                                                       //
//                     2008-01-15                        //
//                                                       //
///////////////////////////////////////////////////////////


//*******************************************************//
//                        ToDo                           //
//-------------------------------------------------------//
// - 
//*******************************************************//

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__ihacres_cal2_H
#define HEADER_INCLUDED__ihacres_cal2_H
//---------------------------------------------------------

#include "MLB_Interface.h"
#include "ihacres_eq.h"

#include <vector>   // used for storing date string values in array
//using namespace std;
typedef std::vector<std::string>	date_array;
//typedef std::vector<double>			vector_d;
//typedef std::vector<int>			vector_i;
//typedef std::vector<bool>			vector_b;
//---------------------------------------------------------

///////////////////////////////////////////////////////////////////////
//
//		CLASS Cihacres_cal2
//
///////////////////////////////////////////////////////////////////////

class Cihacres_cal2 : public CSG_Module
{
public:

	// constructor
	Cihacres_cal2(void);

	// destructor
	virtual ~Cihacres_cal2(void);

protected:

	///////////////////////////////////////////////////////////////////
	//
	//		PROTECTED FUNCTIONS
	//
	///////////////////////////////////////////////////////////////////

	// execute module
	virtual bool	On_Execute		(void);

private:

	///////////////////////////////////////////////////////////////////
	//
	//		PRIVATE PARAMETERS
	//
	///////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------
	//		EXTERNAL CLASSES
	//-----------------------------------------------------------------
	Cihacres_eq		ihacres;		// all IHACRES equations are defined here
	
	CSnowModule*	m_pSnowModule;	// (snow_module.h/.cpp)
	CSnowParms		m_SnowParms;
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	//		TIME SERIES PARAMETERS
	//-----------------------------------------------------------------
	int				m_nValues;		// number of values / time steps in simulation
	date_array		m_vec_date;		// Vector containing date values
	double*			m_p_Q_Inflow_m3s;
	double*			m_p_Q_dif_m3s;
	double*			m_p_Q_obs_m3s;	// Array containing observed discharge values in [m3/s]
	double*			m_p_Q_obs_mmday;// Array containing observed discharge values in [mm]
	double*			m_p_Q_sim_mmday; // simulated streamflow [mm/day]
	double*			m_pPCP;			// Array containing observed precipitation values		
	double*			m_pTMP;			// Array containing temperature values

	// non-linear module time series
	double*			m_pExcessRain;	// "excess" or effective rainfall [mm]
	double*			m_pTw;			// Tw is approximately the time constant, or inversely,
									// the rate at which the catchment wetness declines
									// in the absence of rainfall.
	double*			m_pWI;			// catchment wetness index

	double*			m_pMeltRate;	// if snow module is active

	double			m_sum_obsDisMM;
	double			m_sum_eRainGTpcp;
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	//		MODEL PARAMETERS / Cihacres_eq
	//-----------------------------------------------------------------
	// NON-LINEAR MODULE
	double			m_Tw;
	double			m_f;
	double			m_c;
	double			m_l;
	double			m_p;

	// LINEAR MODULE
	double			m_a;
	double			m_b;
	double			m_aq;
	double			m_as;
	double			m_bq;
	double			m_bs;

	double			m_vq;
	double			m_vs;
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	//		PARAMETERS DIALOG 1
	//-----------------------------------------------------------------
	CSG_Table*		m_pTable;
	int				m_dateField;
	int				m_dischargeField;
	int				m_pcpField;
	int				m_tmpField;
	
	int				m_inflowField;
	bool			m_bUpstream; // true = upstream, false = downstream

	bool			m_bTMP;

	int				m_nsim;			// number of simulations
	double			m_area;			// area of the watershed in [km2]
	//double			m_dev_eRainDis;
	//double			m_dev_eRainPCP;
	int				m_storconf;
	int				m_IHAC_version;
	bool			m_bSnowModule;
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	//		PARAMETERS DIALOG 2
	//-----------------------------------------------------------------
	CSG_String		m_date1, m_date2;
	//-----------------------------------------------------------------
	//		CALIBRATION PARAMETERS
	//-----------------------------------------------------------------
	// non linear module parameters
	double			m_TwConst_ub;	// Tw is approximately the time constant, or inversely,
	double			m_TwConst_lb;	// the rate at which the catchment wetness declines
									// in the absence of rainfall.
	double			m_f_lb;			// temperature modulation factor which
	double			m_f_ub;			// determines how Tw changes with temperature
	double			m_c_lb;			// Parameter c is chosen so that the volume
	double			m_c_ub;			// of excess rainfall is equal to the total
									// streamflow over the calibration period
	// Croke et al. (2005) Redesign of non-linear module
	double			m_l_lb;			// soil moisture index threshold
	double			m_l_ub;
	double			m_p_lb;			// power on soil moisture
	double			m_p_ub;

	// linear module parameters (single storage)
	double			m_a_lb;
	double			m_a_ub;
	double			m_b_lb;
	double			m_b_ub;
	// linear module parameters (2 storages)
	double			m_aq_lb;
	double			m_aq_ub;
	double			m_as_lb;
	double			m_as_ub;
	double			m_bq_lb;
	double			m_bq_ub;
	// SnowModule calibration parameters
	double			m_T_Rain_lb;
	double			m_T_Rain_ub;
	double			m_T_Melt_lb;
	double			m_T_Melt_ub;
	double			m_DD_FAC_lb;
	double			m_DD_FAC_ub;

	int				m_delay;
	int				m_obj_func;
	double			m_NSEmin;
	double			m_NSE;
	double			m_NSE_highflow;
	double			m_NSE_lowflow;
	double			m_PBIAS;		// percent bias

	int				m_counter;	// used by writing output table
	//-----------------------------------------------------------------



	///////////////////////////////////////////////////////////////////
	//
	//		PRIVATE FUNCTIONS
	//
	///////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------
	void			_InitPointers();
	void			_DeletePointers();
	void			_ReadInputTable(int first, int last);
	void			_Calc_ObsMinInflow();
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	//		Simulation
	//-----------------------------------------------------------------
	void			_CalcNonLinearModule();
	void			_CalcLinearModule();
	void			_CalcEfficiency();
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	//		Output
	//-----------------------------------------------------------------
	void			_CreateOutputTable();
	//void			_CreateTable2Storages();
	void			_WriteOutputTable();
	//void			_WriteTable2Storages();
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	//		Dialogs
	//-----------------------------------------------------------------
	void			_CreateDialog1();
	bool			_CreateDialog2();
	//-----------------------------------------------------------------
};

#endif /* HEADER_INCLUDED__ihacres_cal2_H */
