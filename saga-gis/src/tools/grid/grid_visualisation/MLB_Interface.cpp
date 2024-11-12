
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
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
// 1. Include the appropriate SAGA-API header...

#include <saga_api/saga_api.h>


//---------------------------------------------------------
// 2. Place general tool library information here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Grids") );

	case TLB_INFO_Category:
		return( _TL("Visualization") );

	case TLB_INFO_Author:
		return( "O. Conrad, V. Wichmann (c) 2002-2024" );

	case TLB_INFO_Description:
		return( _TL("Visualization tools for grids.") );

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Visualization|Grid") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Grid_Color_Rotate.h"
#include "Grid_Color_Blend.h"
#include "Grid_Colors_Fit.h"
#include "Grid_RGB_Composite.h"
#include "Grid_3D_Image.h"
#include "Grid_Color_Triangle.h"
#include "Grid_Histogram_Surface.h"
#include "Grid_Aspect_Slope_Map.h"
#include "Grid_Terrain_Map.h"
#include "Grid_LUT_Assign.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0: return( new CGrid_Color_Rotate );
	case  1: return( new CGrid_Color_Blend );
	case  2: return( new CGrid_Colors_Fit );
	case  3: return( new CGrid_RGB_Composite );
	case  4: return( new CGrid_3D_Image );
	case  5: return( new CGrid_Color_Triangle );
	case  6: return( new CGrid_Histogram_Surface );
	case  7: return( new CGrid_Aspect_Slope_Map );
	case  8: return( new CGrid_Terrain_Map );
	case  9: return( new CGrid_RGB_Split );
	case 10: return( new CGrid_LUT_Assign );
	case 11: return( new CLUT_Create );

	case 12: return( NULL );
	}

	return( TLB_INTERFACE_SKIP_TOOL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

	TLB_INTERFACE

//}}AFX_SAGA
