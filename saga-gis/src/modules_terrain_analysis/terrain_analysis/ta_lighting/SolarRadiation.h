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
class ta_lighting_EXPORT CSolarRadiation : public CSG_Module_Grid
{
public:
	CSolarRadiation(void);


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	bool					m_bLocalSVF, m_bBending;

	int						m_Time, m_Method, m_Day_A, m_Day_B, m_dDays, m_bUpdate;

	double					m_Solar_Const, m_Hour_A, m_Hour_B, m_dHour, m_Latitude, m_Atmosphere, m_Vapour, m_Transmittance, m_Pressure, m_Water, m_Dust;

	CSG_Grid				*m_pDEM, *m_pVapour, *m_pSVF, *m_pDirect, *m_pDiffus, *m_pTotal, *m_pRatio, *m_pDuration, *m_pSunrise, *m_pSunset,
							m_Slope, m_Aspect, m_Shade, m_Lat, m_Lon, m_Sol_Height, m_Sol_Azimuth;


	bool					Finalise				(void);

	bool					Get_Insolation			(void);
	bool					Get_Insolation			(int Day);
	bool					Get_Insolation			(int Day, double Hour);
	bool					Get_Insolation			(double Sol_Height, double Sol_Azimuth, double Hour);

	double					Get_Air_Mass			(double Sol_Height);

	bool					Get_Irradiance			(int x, int y, double Sol_Height, double Sol_Azimuth, double &Direct, double &Diffus);

	bool					Get_Shade				(double Sol_Height, double Sol_Azimuth);
	void					Set_Shade				(int x, int y, double dx, double dy, double dz);
	void					Set_Shade_Bended		(int x, int y, char iLock);
	bool					Get_Shade_Complete		(int x, int y);
	void					Get_Shade_Params		(double Sol_Height, double Sol_Azimuth, double &dx, double &dy, double &dz);

	int						Get_Day_of_Year			(int Month);
	bool					Get_Solar_Position		(int Day, double Hour, double Lat, double Lon, double &Sol_Height, double &Sol_Azimuth);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SolarRadiation_H
