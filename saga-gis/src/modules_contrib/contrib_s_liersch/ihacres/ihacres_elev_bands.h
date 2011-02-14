/**********************************************************
 * Version $Id$
 *********************************************************/
///////////////////////////////////////////////////////////
//                 ihacres_elev_bands.h                  //
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
// -
//*******************************************************//

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__ihacres_elev_bands_H
#define HEADER_INCLUDED__ihacres_elev_bands_H
//---------------------------------------------------------


class Cihacres_elev_bands
{
public:

	///////////////////////////////////////////////////////////////////
	//
	//							CONSTRUCTORS
	//
	///////////////////////////////////////////////////////////////////

	// default
	Cihacres_elev_bands();

	// destructor
	~Cihacres_elev_bands();

	///////////////////////////////////////////////////////////////////
	//
	//							PARAMETERS
	//
	///////////////////////////////////////////////////////////////////

	//bool			m_b_SnowModule;			// true, if snow module is active
	//int				m_IHAC_version;			// IHACRES version (Jakeman & Hornberger, 1993 / Croke, B. 2005 /...)

	// time series variables
	//double			m_nValues;				// number of pointer elements (time steps / days / ...)
	double*			m_p_pcp;				// precipitation time series
	double*			m_p_tmp;				// temperature time series
	double*			m_p_ER;					// excess rainfall time series
	double*			m_p_streamflow_sim;		// simulated streamflow time series [mm] !!!
	double*			m_p_Tw;					// 
	double*			m_p_WI;					// Wetness Index

	double*			m_p_MeltRate;
	double*			m_p_SnowStorage;

	double			m_sum_eRainGTpcp;

	double			m_mean_elev;			// mean elevation of elevation band [m.a.s.l.]
	double			m_area;		
	///////////////////////////////////////////////////////////////////
	//
	//							PUBLIC FUNCTIONS
	//
	///////////////////////////////////////////////////////////////////

	//double			Get_Elevation()	{ return(m_mean_elev); }
	//double			Get_Area()		{ return(m_area); }

private:
			// elevation band area [km2]

};


#endif /* HEADER_INCLUDED__ihacres_elev_bands_H */