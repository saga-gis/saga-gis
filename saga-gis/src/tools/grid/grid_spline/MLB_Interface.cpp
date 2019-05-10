
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
//                   MLP_Interface.cpp                   //
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
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include <saga_api/saga_api.h>


//---------------------------------------------------------
// 2. Place general tool library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Spline Interpolation") );

	case TLB_INFO_Category:
		return( _TL("Grid") );

	case TLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2006-10") );

	case TLB_INFO_Description:
		return( _TW(
			"Several spline interpolation/approximation methods for the gridding of scattered data. "
			"In most cases the 'Multilevel B-spline Interpolation' might be the optimal choice. "
		));

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Grid|Gridding") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Gridding_Spline_TPS_Local.h"
#include "Gridding_Spline_TPS_TIN.h"
#include "Gridding_Spline_BA.h"
#include "Gridding_Spline_MBA.h"
#include "Gridding_Spline_MBA_Grid.h"
#include "Gridding_Spline_MBA_3D.h"
#include "Gridding_Spline_CSA.h"

#include "MBASpline_for_Categories.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case 1:		return( new CGridding_Spline_TPS_Local );
	case 2:		return( new CGridding_Spline_TPS_TIN );
	case 3:		return( new CGridding_Spline_BA );
	case 4:		return( new CGridding_Spline_MBA );
	case 5:		return( new CGridding_Spline_MBA_Grid );
	case 8:		return( new CGridding_Spline_MBA_3D );
	case 6:		return( new CGridding_Spline_CSA );

	case 7:		return( new CMBASpline_for_Categories );

	case 10:	return( NULL );
	default:	return( TLB_INTERFACE_SKIP_TOOL );
	}
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
