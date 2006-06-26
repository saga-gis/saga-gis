
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Polygon_Clip.h                  //
//                                                       //
//                 Copyright (C) 2006 by                 //
//                     Stefan Liersch                    //
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
//    e-mail:     stefan.liersch@ufz.de                  //
//                stliersch@freenet.de                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Polygon_Clip_H
#define HEADER_INCLUDED__Grid_Polygon_Clip_H

//---------------------------------------------------------

#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Polygon_Clip : public CModule_Grid
{
public:
	// constructor
	CGrid_Polygon_Clip(void);
	// destructor
	virtual ~CGrid_Polygon_Clip(void);

protected:
	// execute module
	virtual bool		On_Execute		(void);

private:

	// estimates the Extent of the new grid
	bool				Get_Extent		(int &xMin, int &xCount,
										 int &yMin, int &yCount, CGrid *pMask);

	// This function has been copied from Module: 'Grid_Statistics_AddTo_Polygon'
	// Function: Get_ShapeIDs
	// copyright by Olaf Conrad
	bool				Get_Mask		(CShapes *pShapes, CGrid *pMask);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Polygon_Clip_H
