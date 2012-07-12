/**********************************************************
 * Version $Id: topographic_openness.h 911 2011-02-14 16:38:15Z reklov_w $
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
//                 topographic_openness.h                //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
#ifndef HEADER_INCLUDED__topographic_openness_H
#define HEADER_INCLUDED__topographic_openness_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTopographic_Openness : public CSG_Module_Grid
{
public:
	CTopographic_Openness(void);


protected:

	virtual bool			On_Execute		(void);


private:

	int						m_Method, m_nLevels;

	double					m_Radius;

	CSG_Points_Z			m_Direction;

	CSG_Vector				m_MinAngle, m_MaxAngle;

	CSG_Grid_Pyramid		m_Pyramid;

	CSG_Grid				*m_pDEM;


	bool					Initialise				(int nDirections);

	bool					Get_Angles_Multi_Scale	(int x, int y);
	bool					Get_Angles_Sectoral		(int x, int y);
	bool					Get_Angle_Sectoral		(int x, int y, int i, double &Max, double &Min);

	bool					Get_Openness			(int x, int y, double &Pos, double &Neg);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__topographic_openness_H
