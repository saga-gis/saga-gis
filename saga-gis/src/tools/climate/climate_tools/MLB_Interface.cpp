/**********************************************************
 * Version $Id: TLB_Interface.cpp 1383 2012-04-26 15:44:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
//                University of Hamburg                  //
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
		return( _TL("Tools") );

	case TLB_INFO_Category:
		return( _TL("Climate") );

	case TLB_INFO_Author:
		return( "O.Conrad (c) 2012" );

	case TLB_INFO_Description:
		return( _TL("Tools for weather and climate data.") );

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Climate") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "grid_levels_interpolation.h"
#include "milankovic.h"
#include "etp_hargreave.h"
#include "daily_sun.h"
#include "bioclimatic_vars.h"
#include "treeline.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0: 	return( new CGrid_Levels_to_Surface );
	case  1: 	return( new CGrid_Levels_to_Points );

	case  2:	return( new CMilankovic );
	case  3:	return( new CMilankovic_SR_Location );
	case  4:	return( new CMilankovic_SR_Day_Location );
	case  5:	return( new CMilankovic_SR_Monthly_Global );

	case  8:	return( new CPET_Hargreave_Grid );
	case  6:	return( new CPET_Hargreave_Table );
	case  7:	return( new CPET_Day_To_Hour );

	case  9:	return( new CDaily_Sun );

	case 10:	return( new CBioclimatic_Vars );
	case 11:	return( new CTree_Growth );
	case 12:	return( new CWater_Balance_Interactive );

	//-----------------------------------------------------
	case 13:	return( NULL );
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
