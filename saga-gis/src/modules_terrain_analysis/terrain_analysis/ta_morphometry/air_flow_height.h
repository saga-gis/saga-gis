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
//                   Air_Flow_Height.h                   //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//    e-mail:     conrad@geowiss.uni-hamburg.de          //
//                                                       //
//    contact:    Olaf Conrad                            //
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
#ifndef HEADER_INCLUDED__Air_Flow_Height_H
#define HEADER_INCLUDED__Air_Flow_Height_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CAir_Flow_Height : public CSG_Module_Grid
{
public:
	CAir_Flow_Height(void);

	virtual const SG_Char *	Get_MenuPath	(void)	{	return( _TL("R:Wind" ));	}


protected:

	virtual bool			On_Execute		(void);


private:

	bool					m_bTrace;

	double					m_dLee, m_dLuv, m_maxDistance, m_Acceleration, m_dx, m_dy;

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
#endif // #ifndef HEADER_INCLUDED__Air_Flow_Height_H
