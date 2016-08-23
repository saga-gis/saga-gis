/**********************************************************
 * Version $Id: ihacres_elev_cal.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
///////////////////////////////////////////////////////////
//                    ihacres_elev_cal.h                 //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com               //
//                     2008-02-13                        //
//-------------------------------------------------------//

//*******************************************************//
//                        ToDo                           //
//-------------------------------------------------------//
// -
//*******************************************************//

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__ihacres_elev_cal_H
#define HEADER_INCLUDED__ihacres_elev_cal_H
//---------------------------------------------------------
#include "MLB_Interface.h"
#include "ihacres_eq.h"
#include "ihacres_elev_bands.h"
#include "snow_module.h"

#include <vector>   // used for storing date string values in array
//using namespace std;

typedef std::vector<std::string> date_array;


class Cihacres_elev_cal : public CSG_Tool
{
public:

	//--------------------------------------------------------
	// CONSTRUCTORS
	//--------------------------------------------------------

	// default
	Cihacres_elev_cal();

	// destructor
	virtual ~Cihacres_elev_cal(void);

	virtual bool			needs_GUI		(void)	{	return( true );	}


protected:

	// execute module
	virtual bool	On_Execute(void);

private:

	//--------------------------------------------------------
	// PRIVATE MEMBER VARIABLES
	//--------------------------------------------------------

	int				m_counter;

	//----------------------------------
	// parameters of first module dialog
	//----------------------------------
	int				m_nElevBands;		// number of used elevation bands
	int				m_nsim;				// number of simulations
	double			m_Area_tot;			// total catchment area [km2]
	int				m_IHAC_version;		// Different versions of IHACRES exist, corresponding
										// to the version...
	int				m_StorConf;			// Storage configuration
										// 0 = single, 1 = two in parallel, 2 = two in series
	bool			m_bSnowModule;		// true if snow module is active
	
	int				m_nStorages;		// number of storages

	int				m_obj_func;
	
	double			m_NSEmin;

	//----------------------------------
	// parameters of second module dialog
	//----------------------------------
	// time series variables
	CSG_Table*		m_p_InputTable;		// IHACRES input table
	int				m_nValues;			// number of selected records
	date_array		m_vec_date;
	double*			m_p_Q_obs_m3s;		// pointer containing observed streamflow in [m3/s]
	double*			m_p_Q_obs_mmday;	// pointer containing observed streamflow in [mm]
	double*			m_p_Q_sim_mmday;
	Cihacres_elev_bands*	m_p_elevbands;	// Class Cihacres_elev_cal_bands

	// Field numbers
	int				m_dateField;		// table field numbers
	int				m_streamflowField;
	// elevation band parameters
	int*			m_p_pcpField;
	int*			m_p_tmpField;
	//double*			m_p_Area;
	//double*			m_p_mean_elev;

	//----------------------------------
	// parameters of third module dialog
	//----------------------------------
	CSG_String		m_date1;			// first day of time series YYYYMMDD
	CSG_String		m_date2;			// last day

	int				m_first;
	int				m_last;

	//----------------------------------
	// Model parameter (in: ihacres_eq.h)
	//----------------------------------
	C_IHAC_LinearParms*		m_p_lin_lb;
	C_IHAC_LinearParms*		m_p_lin_ub;
	C_IHAC_LinearParms*		m_p_linparms;		// parameters of the linear storage module
	C_IHAC_NonLinearParms*	m_p_nl_lb;
	C_IHAC_NonLinearParms*	m_p_nl_ub;
	C_IHAC_NonLinearParms*	m_p_nonlinparms;	// parameters of the non-linear rainfall loss module
	CSnowParms*		m_pSnowparms_lb;
	CSnowParms*		m_pSnowparms_ub;
	CSnowParms*		m_pSnowparms;				// 3 snow module paramters
	CSnowModule*	m_p_SnowModule;
	int				m_delay;
	double*			m_vq;
	double*			m_vs;

	double			m_NSE;
	double			m_NSE_highflow;
	double			m_NSE_lowflow;
	double			m_PBIAS;

	CSG_Table*		m_pTable;
	CSG_Table*		m_pTable_parms;

	//--------------------------------------------------------
	// PRIVATE MEMBER FUNCTIONS
	//--------------------------------------------------------

	void			_Init_ElevBands(int n); // n = m_nElevBands
	void			_Init_Pointers(int nvals);
	void			_CreateDialog1();
	bool			_CreateDialog2();
	bool			_CreateDialog3();
	void			_ReadInputFile();
	void			_CalcSnowModule(int elevband);
	void			_Simulate_NonLinearModule();
	void			_Simulate_Streamflow();
	void			_Sum_Streamflow();
	void			_CalcEfficiency();
	void			_CreateTableSim();
	void			_CreateTableParms();
	void			_WriteTableParms();

	// Class ihacres_eq
	Cihacres_eq		ihacres;

};

#endif /* HEADER_INCLUDED__ihacres_elev_cal_H */
