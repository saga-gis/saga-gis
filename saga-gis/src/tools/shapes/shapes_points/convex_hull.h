
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#ifndef HEADER_INCLUDED__convex_hull_H
#define HEADER_INCLUDED__convex_hull_H


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
class CConvex_Hull : public CSG_Tool
{
public:
	CConvex_Hull(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("A:Shapes|Tools") );	}


protected:

	virtual int				On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute			(void);


private:

	double					is_Left				(const TSG_Point &Line_A, const TSG_Point &Line_B, const TSG_Point &Point);

	bool					Get_Chain_Hull		(CSG_Shapes *pPoints, CSG_Shapes *pHulls, CSG_Shape *pAttributes = NULL, bool bConvexity = false);
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
