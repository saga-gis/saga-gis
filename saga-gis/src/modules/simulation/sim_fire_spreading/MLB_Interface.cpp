/**********************************************************
 * Version $Id: MLB_Interface.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//    System for an Automated Geo-Scientific Analysis    //
//                                                       //
//                    Module Library:                    //
//                      Fire_Spreading                   //
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
// This file is part of 'SAGA - System for an Automated  //
// Geo-Scientific Analysis'. SAGA is free software; you  //
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
//    e-mail:     volaya@saga-gis.org                    //
//                                                       //
//    contact:    Victor Olaya                           //
//                Spain                                  //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

#include "MLB_Interface.h"

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Fire Spreading Analysis") );

	case MLB_INFO_Category:
		return( _TL("Simulation") );

	case MLB_INFO_Author:
		return( SG_T("Victor Olaya (c) 2004") );

	case MLB_INFO_Description:
		return( _TW(
			"Fire spreading analyses based on the BEHAVE fire modeling system "
			"supported by the U.S. Forest Service, Fire and Aviation Management. "
			"Find more information on BEHAVE at the <i>Public Domain Software for the Wildland Fire Community</i> at "
			"<a target=\"_blank\" href=\"http://fire.org\">http://fire.org</a>\n"
			"\n"
			"Reference:\n"
			"Andrews, P.L. (1986): BEHAVE: Fire Behavior Prediction and Fuel Modeling System - "
			"Burn Subsystem, Part 1. U.S. Department of Agriculture, Forest Service General, Technical Report INT-194. "
		));

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Simulation|Fire Spreading") );
	}
}

#include "Simulate.h"
#include "Forecasting.h"

CSG_Module *		Create_Module(int i)
{

	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CForecasting;
		break;

	case 1:
		pModule	= new CSimulate;
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
