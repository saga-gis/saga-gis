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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
class CGrid_Polygon_Clip : public CSG_Module_Grid
{
public:
	// constructor
	CGrid_Polygon_Clip(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Spatial Extent") );	}


protected:
	// execute module
	virtual bool			On_Execute		(void);


private:

	bool					m_bNoData;


	// estimates the Extent of the new grid
	bool					Get_Extent		(int &xMin, int &xCount,
											 int &yMin, int &yCount, CSG_Grid *pMask, CSG_Parameter_Grid_List *pGrids);

	bool					is_InGrid		(int x, int y, CSG_Grid *pMask, CSG_Parameter_Grid_List *pGrids);

	// This function has been copied from Module: 'Grid_Statistics_AddTo_Polygon'
	// Function: Get_ShapeIDs
	// copyright by Olaf Conrad
	bool					Get_Mask		(CSG_Shapes *pShapes, CSG_Grid *pMask);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Polygon_Clip_H
