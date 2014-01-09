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
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Grid_Gaps_Spline_Fill.h                //
//                                                       //
//                 Copyright (C) 2010 by                 //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Gaps_Spline_Fill_H
#define HEADER_INCLUDED__Grid_Gaps_Spline_Fill_H

///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Gaps_Spline_Fill : public CSG_Module_Grid  
{
public:
	CGrid_Gaps_Spline_Fill(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("A:Grid|Gaps") );	}


protected:

	virtual bool			On_Execute		(void);


private:

	bool					m_bExtended;

	int						m_nGaps, m_nGapCells, m_iStack, m_nPoints_Max, m_nPoints_Local, m_nGapCells_Max, m_Neighbours;

	double					m_Relaxation, m_Radius;

	CSG_Points_Int			m_GapCells, m_Stack;

	CSG_Thin_Plate_Spline	m_Spline;

	CSG_Grid				*m_pGrid, *m_pMask, m_Gaps;


	bool					is_Gap			(int  x, int  y);

	void					Push			(int  x, int  y);
	void					Pop				(int &x, int &y);

	void					Set_Gap_Cell	(int  x, int  y);

	void					Close_Gap		(int  x, int  y);

	void					Close_Gap		(void);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Gaps_Spline_Fill_H
