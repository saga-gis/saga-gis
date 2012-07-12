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
//                     View_Shed.h                       //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__View_Shed_H
#define HEADER_INCLUDED__View_Shed_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CView_Shed : public CSG_Module_Grid
{
public:
	CView_Shed(void);


protected:

	virtual bool			On_Execute		(void);


private:

	int						m_Method, m_nLevels;

	double					m_Radius;

	CSG_Points_Z			m_Direction;

	CSG_Vector				m_Angles, m_Distances;

	CSG_Grid				*m_pDEM;

	CSG_Grid_Pyramid		m_Pyramid;


	bool					Initialise				(int nDirections);

	bool					Get_Angles_Multi_Scale	(int x, int y);
	bool					Get_Angles_Sectoral		(int x, int y);
	void					Get_Angle_Sectoral		(int x, int y, double dx, double dy, double &Angle, double &Distance);

	bool					Get_View_Shed			(int x, int y, double &Sky_Visible, double &Sky_Factor, double &Sky_Simple, double &Sky_Terrain, double &Distance);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__View_Shed_H
