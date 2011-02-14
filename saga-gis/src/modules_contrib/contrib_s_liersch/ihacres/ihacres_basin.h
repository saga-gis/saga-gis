/**********************************************************
 * Version $Id$
 *********************************************************/
///////////////////////////////////////////////////////////
//                   ihacres_basin.h                     //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com               //
//                     2008-01-30                        //
//-------------------------------------------------------//

//*******************************************************//
//                        ToDo                           //
//-------------------------------------------------------//
// -
//*******************************************************//

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__ihacres_basin_H
#define HEADER_INCLUDED__ihacres_basin_H
//---------------------------------------------------------

#include "MLB_Interface.h"
#include "ihacres_eq.h"

//---------------------------------------------------------

///////////////////////////////////////////////////////////////////////
//
//		CLASS Cihacres_sub_basin
//
///////////////////////////////////////////////////////////////////////

class Cihacres_subbasin
{
public:
	///////////////////////////////////////////////////////////////////
	//
	//							CONSTRUCTORS
	//
	///////////////////////////////////////////////////////////////////

	// default
	Cihacres_subbasin(void) {
		m_pPCP			= NULL;
		m_pTMP			= NULL;
		m_pER			= NULL;
		m_p_Q_sim_mmday	= NULL;
		m_pTw			= NULL;
		m_pWI			= NULL;
		m_pMeltRate		= NULL;
		m_pSnowStorage	= NULL;
	}

	Cihacres_subbasin(int nvals) {
		Initialize(nvals);
	}

	// destructor
	~Cihacres_subbasin(void) {
		if (m_pPCP) delete[] m_pPCP;
		if (m_pTMP) delete[] m_pTMP;
		if (m_pER) delete[] m_pER;
		if (m_p_Q_sim_mmday) delete[] m_p_Q_sim_mmday;
		if (m_pTw) delete[] m_pTw;
		if (m_pWI) delete[] m_pWI;
		if (m_pMeltRate) delete[] m_pMeltRate;
		if (m_pSnowStorage) delete[] m_pSnowStorage;

	}

	void			Initialize(int nvals) {
		m_nValues		= nvals;
		m_pPCP			= new double[nvals];
		m_pTMP			= new double[nvals];
		m_pER			= new double[nvals];
		m_p_Q_sim_mmday	= new double[nvals];
		m_pTw			= new double[nvals];
		m_pWI			= new double[nvals];
		m_pMeltRate		= new double[nvals];
		m_pSnowStorage	= new double[nvals];		
	}

	///////////////////////////////////////////////////////////////////
	//
	//							PARAMETERS
	//
	///////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------
	// TIME SERIES
	//-----------------------------------------------------------------
	int				m_nValues;
	double*			m_pPCP;				// precipitation time series
	double*			m_pTMP;				// temperature time series
	double*			m_pER;				// excess rainfall time series
	double*			m_p_Q_sim_mmday;	// simulated streamflow time series [mm] !!!
	double*			m_pTw;				// 
	double*			m_pWI;				// Wetness Index
	double*			m_pMeltRate;
	double*			m_pSnowStorage;
	//-----------------------------------------------------------------
	//
	//-----------------------------------------------------------------
	// lag coefficient
	// this parameter is to account for the time lag between the streamflow signal
	// at the subbasin outlet and the time of this signal at
	// the catchment/bain outlet
	int				m_lag;
	int				m_delay;
	double			m_area;				// sub-catchment area [km2]
	double			m_sum_eRainGTpcp;


private:

};
//-------------------------------------------------------------------







///////////////////////////////////////////////////////////////////////
//
//		CLASS Cihacres_basin
//
///////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------

class Cihacres_basin : public CSG_Module
{
public:
	///////////////////////////////////////////////////////////////////
	//
	//							CONSTRUCTORS
	//
	///////////////////////////////////////////////////////////////////

	// default
	Cihacres_basin(void);

	// destructor
	~Cihacres_basin(void);

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

	//----------------------------------
	// parameters of first module dialog
	//----------------------------------
	int				m_nSubbasins;		// number of sub-basins
	//double			m_Area_tot;			// total catchment area [km2]
	int				m_IHAC_version;		// Different versions of IHACRES exist, corresponding
										// to the version...
	int				m_StorConf;			// Storage configuration
										// 0 = single, 1 = two in parallel, 2 = two in series
	bool			m_bSnowModule;		// true if snow module is active
	
	int				m_nStorages;		// number of storages

	//----------------------------------
	// parameters of second module dialog
	//----------------------------------
	// time series variables
	CSG_Table*		m_p_InputTable;		// IHACRES input table
	int				m_nValues;			// number of selected records
	date_array		m_vec_date;
	double*			m_p_Q_obs_m3s;		// pointer containing observed streamflow in [m3/s]
	double*			m_p_Q_obs_mmday;	// pointer containing observed streamflow in [mm]
	Cihacres_subbasin*	m_pSubbasin;	// Class Cihacres_elev_bands

	// Field numbers
	int				m_dateField;		// table field numbers
	int				m_streamflowField;
	// subbasin band parameters
	int*			m_p_pcpField;
	int*			m_p_tmpField;
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
	C_IHAC_LinearParms*		m_p_linparms;		// parameters of the linear storage module
	C_IHAC_NonLinearParms*	m_p_nonlinparms;	// parameters of the non-linear rainfall loss module
	CSnowParms*		m_pSnowparms;				// 3 snow module paramters
	CSnowModule*	m_p_SnowModule;
	//int				m_delay;
	double			m_vq;
	double			m_vs;

	CSG_Table*		m_pTable;


	//--------------------------------------------------------
	// PRIVATE MEMBER FUNCTIONS
	//--------------------------------------------------------

	void			_Init_Subbasins(int n); // n = m_nElevBands
	void			_Init_Pointers(int nvals);
	void			_CreateDialog1();
	bool			_CreateDialog2();
	bool			_CreateDialog3();
	void			_ReadInputFile();
	void			_CalcSnowModule(int nSubbasins);
	void			_Simulate_NonLinearModule(int nSubbasins);
	void			_Simulate_Streamflow(int nSubbasins, double Q_init);
	void			_CreateTableSim();

	// Class ihacres_eq
	Cihacres_eq		ihacres;
};

#endif /* HEADER_INCLUDED__ihacres_basin_H */
