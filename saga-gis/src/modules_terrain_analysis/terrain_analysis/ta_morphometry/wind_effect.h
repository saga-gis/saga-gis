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
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     wind_effect.h                     //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                     Andre Ringeler                    //
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
//    e-mail:     aringeler@saga-gis.org                 //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#ifndef HEADER_INCLUDED__LeeLuv_B_H
#define HEADER_INCLUDED__LeeLuv_B_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWind_Effect : public CSG_Module_Grid
{
public:
	CWind_Effect(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("A:Terrain Analysis|Climate and Weather" ));	}


protected:

	virtual bool			On_Execute		(void);


private:

	bool					m_bTrace;

	double					m_maxDistance, m_Acceleration, m_dx, m_dy;

	CSG_Grid				*m_pDEM, m_DX, m_DY;

	CSG_Grid_Pyramid		m_DEM;


	bool					Get_Next		(TSG_Point &Position, double Distance, bool bReverse);
	bool					Get_Z			(const TSG_Point &Position, double Distance, double &z);

	void					Get_Luv			(int x, int y, double &Sum_A);
	void					Get_Lee			(int x, int y, double &Sum_A, double &Sum_B);

	void					Get_Luv			(int x, int y, double dx, double dy, double &Sum_A);
	void					Get_Lee			(int x, int y, double dx, double dy, double &Sum_A, double &Sum_B);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__LeeLuv_B_H