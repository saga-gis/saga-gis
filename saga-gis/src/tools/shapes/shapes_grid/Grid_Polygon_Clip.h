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
class CGrid_Polygon_Clip : public CSG_Tool_Grid
{
public:
	CGrid_Polygon_Clip(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Spatial Extent") );	}


protected:

	virtual bool			On_Execute		(void);


private:

	bool					Get_Output		(const CSG_Grid &Mask, CSG_Parameter_Grid_List *pOutput, CSG_Grid_System &System);

	bool					Has_Data		(int x, int y, CSG_Parameter_Grid_List *pInput);

	bool					Get_Mask		(CSG_Grid &Mask);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Polygon_Clip_H
