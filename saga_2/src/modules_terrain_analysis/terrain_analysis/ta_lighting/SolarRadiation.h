
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   SolarRadiation.h                    //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SolarRadiation_H
#define HEADER_INCLUDED__SolarRadiation_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class ta_lighting_EXPORT CSolarRadiation : public CModule_Grid
{
public:
	CSolarRadiation(void);
	virtual ~CSolarRadiation(void);


protected:

	virtual bool		On_Execute				(void);


private:

	int					m_Method;

	double				m_SolarConstant, m_Transmittance, m_Pressure, m_Water, m_Dust;

	CGrid				*m_pDTM, *m_pSum, *m_pRadiation, *m_pDuration;


	void				Execute_DailySum		(double Latitude_RAD, double Hour_Step, double Hour_Start, double Hour_Stop, int Day);
	void				Execute_SumOfDays		(double Latitude_RAD, double Hour_Step, double Hour_Start, double Hour_Stop, int Day_Step, int Day_Start, int Day_Stop);

	void				Get_DailySum			(double Latitude_RAD, double Hour_Step, double Hour_Start, double Hour_Stop, int Day, bool bProgressBar = true);

	void				Get_SolarCorrection		(double ZenithAngle, double Elevation, double &RDIRN, double &RDIFN);

	bool				Get_SolarPosition		(int Day, double Time, double LAT, double LON, double &Azimuth, double &Declination, bool bDegree = true);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SolarRadiation_H
