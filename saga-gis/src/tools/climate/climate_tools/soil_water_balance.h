
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    climate_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 soil_water_balance.h                  //
//                                                       //
//                 Copyrights (C) 2019                   //
//                     Olaf Conrad                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__soil_water_balance_H
#define HEADER_INCLUDED__soil_water_balance_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSoil_Water_Balance : public CSG_Tool_Grid
{
public:
	CSoil_Water_Balance(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Soils") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	double						m_Lat_const, m_SWC, m_SWC_0, m_SWT_Resist[2];

	CSG_Grid					m_Lat_Grid, *m_pLat_Grid, *m_pSWC, *m_pSnow, *m_pSW[2];

	CSG_Parameter_Grid_List		*m_pTavg, *m_pTmin, *m_pTmax, *m_pPsum;


	bool						Initialize				(void);
	bool						Finalize				(void);

	bool						Get_SW_Capacity			(int x, int y, double SWC[2]);
	double						Get_Snow_Storage		(int x, int y, double T, double P);
	bool						Get_Weather				(int x, int y, int Day, const CSG_DateTime &Date, double &T, double &P, double &ETpot);
	bool						Set_Day					(int x, int y, int Day, const CSG_DateTime &Date);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__soil_water_balance_H
