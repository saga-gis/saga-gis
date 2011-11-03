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
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
//               SAGA User Group Associaton              //
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
//    contact:    SAGA User Group Associaton             //
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
		return( _TL("Grid - Calculus") );

	case MLB_INFO_Author:
		return( SG_T("O. Conrad, A. Ringeler, V. Olaya (c) 2001-4") );

	case MLB_INFO_Description:
		return( _TL("Grid based or related calculations.") );

	case MLB_INFO_Version:
		return( _TL("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Grid|Calculus" ));
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Grid_Normalise.h"
#include "Grid_Calculator.h"
#include "Grid_Volume.h"
#include "grid_difference.h"
#include "Grid_Plotter.h"
#include "Grid_Geometric_Figures.h"
#include "Grid_Random_Terrain.h"
#include "Grid_Random_Field.h"

#include "Fuzzify.h"
#include "FuzzyAND.h"
#include "FuzzyOR.h"

#include "grid_metric_conversion.h"
#include "gradient_cartes_polar.h"

#include "grids_trend_polynom.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  1:		return( new CGrid_Calculator );
	case  2:		return( new CGrid_Volume );

	case  4:		return( new CGrid_Plotter );
	case  5:		return( new CGrid_Geometric_Figures );
	case  6:		return( new CGrid_Random_Terrain );
	case  7:		return( new CGrid_Random_Field );

	case  0:		return( new CGrid_Normalise );
	case 10:		return( new CGrid_Standardise );

	case  3:		return( new CGrid_Difference );
	case 18:		return( new CGrid_Division );
	case  8:		return( new CGrids_Sum );
	case  9:		return( new CGrids_Product );

	case 11:		return( new CFuzzify );
	case 12:		return( new CFuzzyAND );
	case 13:		return( new CFuzzyOR );

	case 14:		return( new CGrid_Metric_Conversion );

	case 15:		return( new CGradient_Cartes_To_Polar );
	case 16:		return( new CGradient_Polar_To_Cartes );

	case 17:		return( new CGrids_Trend );

	//-----------------------------------------------------
	case 19:		return( NULL );
	default:		return( MLB_INTERFACE_SKIP_MODULE );
	}
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
