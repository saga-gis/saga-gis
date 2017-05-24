///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                   sim_geomorphology                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2016 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@alps-gmbh.com                 //
//                wichmann@laserdata.at                  //
//                                                       //
//    contact:    alpS GmbH                              //
//                Grabenweg 68                           //
//                A-6020 Innsbruck                       //
//                www.alps-gmbh.com                      //
//                                                       //
//                LASERDATA GmbH                         //
//                Technikerstr. 21a                      //
//                A-6020 Innsbruck                       //
//                www.laserdata.at                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


//---------------------------------------------------------
CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Geomorphology") );

	case TLB_INFO_Category:
		return( _TL("Simulation") );

	case TLB_INFO_Author:
		return( SG_T("V. Wichmann (c) 2016") );

	case TLB_INFO_Description:
		return( _TL("This library contains tools for the simulation of geomorphological processes.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Simulation|Geomorphology") );
	}
}


//---------------------------------------------------------
#include "GPP_Model.h"


//---------------------------------------------------------
CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0: 	return( new CGPP_Model );

	//-----------------------------------------------------
	case  1:	return( NULL );
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

