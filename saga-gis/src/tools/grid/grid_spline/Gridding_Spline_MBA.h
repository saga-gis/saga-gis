/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_spline                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Gridding_Spline_MBA.h                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Gridding_Spline_MBA_H
#define HEADER_INCLUDED__Gridding_Spline_MBA_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Gridding_Spline_Base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class grid_spline_EXPORT CGridding_Spline_MBA : public CGridding_Spline_Base
{
public:
	CGridding_Spline_MBA(void);
	virtual ~CGridding_Spline_MBA(void);


protected:

	virtual bool			On_Execute			(void);


private:

	bool					m_bUpdate;

	int						m_Level_Max;

	double					m_Epsilon, m_Resolution;

	CSG_Points_Z			m_Points;


	bool					_Set_MBA			(double dCell);
	bool					_Set_MBA_Refinement	(double dCell);
	bool					_Set_MBA_Refinement	(CSG_Grid *Psi, CSG_Grid *Phi);

	bool					_Get_Phi			(CSG_Grid &Phi, double dCell, int nCells);
	bool					_Get_Difference		(CSG_Grid &Phi);

	void					BA_Set_Grid			(CSG_Grid &Phi, bool bAdd = false);
	double					BA_Get_Value		(double x, double y, CSG_Grid &Phi);
	bool					BA_Get_Phi			(CSG_Grid &Phi);
	double					BA_Get_B			(int i, double d);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Gridding_Spline_MBA_H
