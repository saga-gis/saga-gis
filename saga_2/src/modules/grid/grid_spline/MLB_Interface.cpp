
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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

const char *	Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( "Grid - Spline Interpolation" );

	case MLB_INFO_Author:
		return( "(c) 2006 by Olaf Conrad" );

	case MLB_INFO_Description:
		return(
			"Several spline interpolation/approximation methods for the gridding of scattered data. "
			"In most cases the 'Multilevel B-spline Interpolation' will be the optimal choice. "
		);

	case MLB_INFO_Version:
		return( "1.0" );

	case MLB_INFO_Menu_Path:
		return( "Grid|Gridding" );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Gridding_Spline_TPS_Global.h"
#include "Gridding_Spline_TPS_Local.h"
#include "Gridding_Spline_TPS_TIN.h"
#include "Gridding_Spline_BA.h"
#include "Gridding_Spline_MBA.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CGridding_Spline_TPS_Global;
		break;

	case 1:
		pModule	= new CGridding_Spline_TPS_Local;
		break;

	case 2:
		pModule	= new CGridding_Spline_TPS_TIN;
		break;

	case 3:
		pModule	= new CGridding_Spline_BA;
		break;

	case 4:
		pModule	= new CGridding_Spline_MBA;
		break;

	default:
		pModule	= NULL;
		break;
	}

	return( pModule );
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
