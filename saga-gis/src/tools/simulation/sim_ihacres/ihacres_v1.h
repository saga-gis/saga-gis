/**********************************************************
 * Version $Id: ihacres_v1.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        IHACRES                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    ihacres_v1.h                       //
//                                                       //
//                 Copyright (C) 2006 by                 //
//                     Stefan Liersch                    //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     stefan.liersch@ufz.de                  //
//                stefan.liersch@gmail.com                   //
//                                                       //
//                     2006-08-31                        //
//                                                       //
///////////////////////////////////////////////////////////


//*******************************************************//
//                        ToDo                           //
//-------------------------------------------------------//
// - stürzt ab, wenn im zweiten Dialog auf Cancel gedrückt wird
//
// - add NSE_highflow to output table
// - wenn SingleStore, dann wird IHACRES_Sim output table nicht korrekt geschrieben
//   (Spalte Streamflow sim)
// - implement 2 or n storages in series
// - number of simulations for linear module
// - choosing the number of Top simulations from non-linear module calibration
// - Gültigkeitsbereiche auf Sinn prüfen
// - WriteTable1Storage
// - Streamflow simulation function using n, m values
// - if (bTMP) = false ???
//*******************************************************//

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__ihacres_v1_H
#define HEADER_INCLUDED__ihacres_v1_H

//---------------------------------------------------------
#include "MLB_Interface.h"
#include "ihacres_eq.h"

#include <vector>   // used for storing date string values in array
using namespace std;

typedef std::vector<std::string> date_array;
typedef vector<double> vector_d;
//---------------------------------------------------------

class Cihacres_v1 : public CSG_Tool
{
public:

	// constructor
	Cihacres_v1(void);

	// destructor
	virtual ~Cihacres_v1(void);

	virtual bool			needs_GUI		(void)	{	return( true );	}


protected:

	// execute module
	virtual bool	On_Execute			(void);

private:

	//--------------------------------------------------------
	// Private Member Variables
	//--------------------------------------------------------
	int				sizeAll;	// number of selected records
	date_array		date;
	vector_d		m_Q_obs_m3s;
	vector_d		m_Q_obs_mmday;
	vector_d		precipitation;
	vector_d		temperature;

	// Field numbers
	int				dateField;	// table field numbers
	int				dischargeField;
	int				pcpField;
	int				tmpField;

	int				first;		// number of the record (index) of first date
	int				last;		// number of the record (index) of last date

	CSG_String		date1, date2; // first and last date of selected period
	double			m_area;		// area of the watershed in [km2]

	// Non-Linear Tool Parameters
	double			TwConst;	// Tw is approximately the time constant, or inversely,
								// the rate at which the catchment wetness declines
								// in the absence of rainfall.
	double			f;			// temperature modulation factor which
	double			c;			// Parameter c is chosen so that the volume
								// of excess rainfall is equal to the total
								// streamflow over the calibration period
	double			l;			// Croke et al. (2005)
	double			p;			// Croke et al. (2005)

	// Linear Tool Parameters
	double			a;			// linear module (single storage)
	double			b;
	double			aq;			// linear module (two storages)
	double			as;
	double			bq;
	double			bs;

	int				delay;		// The delay after the start of rainfall,
								// before the discharge starts to rise.

	int				IHAC_version; // Different versions of IHACRES exist, corresponding
								  // to the version...

	bool			bSnowModule;

	// snow module parameters
	CSnowModule		*m_pSnowModule;
	double			*m_pMeltRate;
	double			T_Rain;
	double			T_Melt;
	double			DD_FAC;

	//vector_d		SnowStorage;
	//vector_d		MeltRate;
	//CSnowModule_alt		*m_pSnowModule;


	//--------------------------------------------------------
	// Private Member Functions
	//--------------------------------------------------------

	void			CreateTableSim		(CSG_Table *pTable, date_array date, vector_d strfl_obs, vector_d strfl_sim);
	void			CreateTableParms	(CSG_Table *pTable, date_array date, vector_d strfl_obs, vector_d precipitation, vector_d temperature,
										 vector_d strfl_sim, vector_d excessRain, vector_d wi, vector_d Tw);
	void			CreateTableSettings	(CSG_Table *pTable, double Tw, double c, double f,
										 double aq, double as, double bq, double bs,
										 double vq, double vs,
										 bool bSnowModule, double T_Rain, double T_Melt, double DD_FAC,
										 int delay,
										 double RRCoef, double NSE,
										 double area);
	
	void			CreateDialog1		();
	bool			CreateDialog2		(bool bTMP, int storconf, int IHAC_version,
										 CSG_Table *pTable,
										 int dateField, int dischargeField);

	// Class ihacres_eq
	Cihacres_eq		*ihacres;

};
#endif /* #ifndef HEADER_INCLUDED__ihacres_v1_H */