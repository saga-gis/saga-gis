
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_lighting                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 SADO_SolarRadiation.h                 //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#ifndef HEADER_INCLUDED__SADO_SolarRadiation_H
#define HEADER_INCLUDED__SADO_SolarRadiation_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class ta_lighting_EXPORT CSADO_SolarRadiation : public CModule_Grid
{
public:
	CSADO_SolarRadiation(void);
	virtual ~CSADO_SolarRadiation(void);

//	virtual const char *	Get_MenuPath			(void)	{	return( _TL("R:Lighting") );	}


protected:

	virtual bool			On_Execute				(void);


private:

	bool					m_bMoment, m_bHorizon, m_bBending,
							m_bUpdateDirect, m_bUpdateDiffus, m_bUpdateTotal;

	int						m_Day_A, m_Day_B, m_dDays;

	double					m_Solar_Const, m_Atmosphere, m_VP, m_Latitude, m_Hour, m_dHour;

	CGrid					*m_pDEM, *m_pVP, *m_pSumDirect, *m_pSumDiffus, *m_pSumTotal,
							m_TmpDirect, m_TmpDiffus, m_TmpTotal,
							m_Slope, m_Aspect, m_Shade, m_Lat, m_Lon, m_Decline, m_Azimuth;


	bool					Initialise				(void);
	bool					Finalise				(double SumFactor = 1.0);

	bool					Get_Insolation			(void);
	bool					Get_Insolation			(int Day, double Hour);

	double					Get_Vapour_Exponent		(double VapourPressure);
	double					Get_Vapour_A			(double VapourPressure);
	bool					Set_Insolation			(double Decline, double Azimuth);

	double					Get_Solar_Reduction		(double Elevation, double Decline, double Reduction);
	double					Get_Solar_Direct		(int x, int y, double Decline, double Azimuth, double Exponent);
	double					Get_Solar_Diffus		(int x, int y, double Decline, double A      , double Exponent);

	bool					Get_Shade				(double Azimuth, double Decline);
	void					Set_Shade				(int x, int y, double dx, double dy, double dz);
	void					Set_Shade_Bended		(int x, int y, char iLock);
	bool					Get_Shade_Complete		(int x, int y);
	void					Get_Shade_Params		(double Decline, double Azimuth, double &dx, double &dy, double &dz);

	bool					Get_Solar_Position		(int Day, double Hour, double Lat, double Lon, double &Dec, double &Azi);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SADO_SolarRadiation_H
