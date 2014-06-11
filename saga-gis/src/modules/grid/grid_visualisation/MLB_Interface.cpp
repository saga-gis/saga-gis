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
//                  Grid_Visualisation                   //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Grid - Visualisation") );

	case MLB_INFO_Author:
		return( SG_T("O. Conrad, V. Wichmann (c) 2002-2014") );

	case MLB_INFO_Description:
		return( _TL("Special visualisation functions for grids.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Grid|Visualisation") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Grid_Color_Rotate.h"
#include "Grid_Color_Blend.h"
#include "Grid_Colors_Fit.h"
#include "Grid_RGB_Composite.h"
#include "Grid_3D_Image.h"
#include "Grid_Color_Triangle.h"
#include "Grid_Histogram_Surface.h"
#include "Grid_Aspect_Slope_Map.h"
#include "Grid_Terrain_Map.h"

//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CGrid_Color_Rotate );
	case  1:	return( new CGrid_Color_Blend );
	case  2:	return( new CGrid_Colors_Fit );
	case  3:	return( new CGrid_RGB_Composite );
	case  4:	return( new CGrid_3D_Image );
	case  5:	return( new CGrid_Color_Triangle );
	case  6:	return( new CGrid_Histogram_Surface );
	case  7:	return( new CGrid_Aspect_Slope_Map );
	case  8:	return( new CGrid_Terrain_Map );
	case  9:	return( new CGrid_RGB_Split );
	}

	return( NULL );
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
