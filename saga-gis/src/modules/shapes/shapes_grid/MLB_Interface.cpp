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
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general module library informations here...

const SG_Char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Shapes - Grid") );

	case MLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2002") );

	case MLB_INFO_Description:
		return( _TL("Tools related to gridded and vector data (conversions, combinations, etc.).") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Shapes|Grid") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Grid_Values_AddTo_Points.h"
#include "Grid_Values_AddTo_Shapes.h"
#include "Grid_Statistics_AddTo_Polygon.h"
#include "Grid_To_Points.h"
#include "Grid_To_Points_Random.h"
#include "Grid_To_Contour.h"
#include "Grid_Classes_To_Shapes.h"
#include "Grid_Polygon_Clip.h"
#include "Grid_To_Gradient.h"
#include "grid_local_extremes_to_points.h"
#include "grid_extent.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CGrid_Values_AddTo_Points );
	case  1:	return( new CGrid_Values_AddTo_Shapes );
	case  2:	return( new CGrid_Statistics_AddTo_Polygon );
	case  3:	return( new CGrid_To_Points );
	case  4:	return( new CGrid_To_Points_Random );
	case  5:	return( new CGrid_To_Contour );
	case  6:	return( new CGrid_Classes_To_Shapes );
	case  7:	return( new CGrid_Polygon_Clip );

	case  9:	return( new CGrid_Local_Extremes_to_Points );
	case 10:	return( new CGrid_Extent );

	case 15:	return( new CGrid_To_Gradient(0) );
	case 16:	return( new CGrid_To_Gradient(1) );
	case 17:	return( new CGrid_To_Gradient(2) );

	case 20:	return( NULL );
	}

	return( MLB_INTERFACE_SKIP_MODULE );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

	MLB_INTERFACE

//}}AFX_SAGA
