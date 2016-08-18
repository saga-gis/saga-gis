/**********************************************************
 * Version $Id: ihacres_elev.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
///////////////////////////////////////////////////////////
//                    ihacres_elev.h                     //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com                   //
//                     2008-01-08                        //
//-------------------------------------------------------//

//*******************************************************//
//                        ToDo                           //
//-------------------------------------------------------//
// - m_delay is not yet implemented for each elevation band
//	as it is suggested by the user dialog. The delay value
//	of the last elevation band is used instead for all bands.
//*******************************************************//

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__ihacres_elev_H
#define HEADER_INCLUDED__ihacres_elev_H
//---------------------------------------------------------
#include "MLB_Interface.h"
#include "ihacres_eq.h"
#include "ihacres_elev_bands.h"
#include "snow_module.h"

#include <vector>   // used for storing date string values in array
//using namespace std;

typedef std::vector<std::string> date_array;


class Cihacres_elev : public CSG_Tool
{
public:

	//--------------------------------------------------------
	// CONSTRUCTORS
	//--------------------------------------------------------

	// default
	Cihacres_elev();

	// destructor
	virtual ~Cihacres_elev(void);

	virtual bool			needs_GUI		(void)	{	return( true );	}


protected:

	// execute module
	virtual bool	On_Execute(void);

private:

	//--------------------------------------------------------
	// PRIVATE MEMBER VARIABLES
	//--------------------------------------------------------

	//----------------------------------
	// parameters of first module dialog
	//----------------------------------
	int				m_nElevBands;		// number of used elevation bands
	double			m_Area_tot;			// total catchment area [km2]
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
	Cihacres_elev_bands*	m_p_elevbands;	// Class Cihacres_elev_bands

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
	C_IHAC_LinearParms*		m_p_linparms;		// parameters of the linear storage module
	C_IHAC_NonLinearParms*	m_p_nonlinparms;	// parameters of the non-linear rainfall loss module
	CSnowParms*		m_pSnowparms;				// 3 snow module parameters
	CSnowModule*	m_p_SnowModule;
	int				m_delay;
	double			m_vq;
	double			m_vs;

	CSG_Table*		m_pTable;

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
	void			_Simulate_NonLinearModule(int elevband);
	void			_Simulate_Streamflow(int elevband);
	void			_CreateTableSim();

	// Class ihacres_eq
	Cihacres_eq		ihacres;

};

#endif /* HEADER_INCLUDED__ihacres_elev_H */
