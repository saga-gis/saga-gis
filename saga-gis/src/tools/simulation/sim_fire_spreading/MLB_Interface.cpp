/**********************************************************
 * Version $Id: TLB_Interface.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//    System for an Automated Geo-Scientific Analysis    //
//                                                       //
//                     Tool Library                      //
//                      Fire_Spreading                   //
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
// This file is part of 'SAGA - System for an Automated  //
// Geo-Scientific Analysis'. SAGA is free software; you  //
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
//    e-mail:     volaya@saga-gis.org                    //
//                                                       //
//    contact:    Victor Olaya                           //
//                Spain                                  //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

#include "MLB_Interface.h"

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Fire Spreading Analysis") );

	case TLB_INFO_Category:
		return( _TL("Simulation") );

	case TLB_INFO_Author:
		return( SG_T("Victor Olaya (c) 2004") );

	case TLB_INFO_Description:
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

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Simulation|Fire Spreading") );
	}
}

#include "Simulate.h"
#include "Forecasting.h"

CSG_Tool *		Create_Tool(int i)
{

	CSG_Tool	*pTool;

	switch( i )
	{
	case 0:
		pTool	= new CForecasting;
		break;

	case 1:
		pTool	= new CSimulate;
		break;

	default:
		pTool	= NULL;
		break;
	}

	return( pTool );
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
