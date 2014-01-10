/**********************************************************
 * Version $Id: flow_massflux.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    flow_massflux.h                    //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__flow_massflux_H
#define HEADER_INCLUDED__flow_massflux_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>

//---------------------------------------------------------
class CFlow_MassFlux : public CSG_Module_Grid
{
public: ////// public members and functions: //////////////

	CFlow_MassFlux(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("R:Catchment Area" ));	}


protected: /// protected members and functions: ///////////

	virtual bool			On_Execute		(void);


private: ///// private members and functions: /////////////

	int						m_Method;

	CSG_Grid				*m_pDEM, *m_pSlope, *m_pAspect, m_Area, m_dir, m_dif;

	CSG_Shapes				*m_pFlow;


	bool					Set_Flow		(int x, int y, int Direction);

	double					Get_Flow		(int x, int y, int Direction);
	double					Get_Area		(int x, int y);

	bool					Check_Flow		(int x, int y);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__flow_massflux_H
