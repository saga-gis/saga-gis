/**********************************************************
 * Version $Id: points_filter.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    shapes_points                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     convex_hull.h                     //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__convex_hull_H
#define HEADER_INCLUDED__convex_hull_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CConvex_Hull : public CSG_Module
{
public:
	CConvex_Hull(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("A:Shapes|Tools") );	}


protected:

	virtual bool			On_Execute			(void);


private:

	static CSG_Shapes		*m_pPoints;

	static int				Compare				(const int iElement_1, const int iElement_2);

	double					is_Left				(const TSG_Point &Line_A, const TSG_Point &Line_B, const TSG_Point &Point);

	bool					Get_Chain_Hull		(CSG_Shapes *pPoints, CSG_Shapes *pHulls);
	int						Get_Chain_Hull		(CSG_Points &P, CSG_Points &H);

	bool					Get_Bounding_Box	(CSG_Shape *pHull, CSG_Shape *pBox);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__convex_hull_H
