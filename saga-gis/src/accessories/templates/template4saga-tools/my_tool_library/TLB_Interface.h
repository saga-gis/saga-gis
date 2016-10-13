
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                    TLB_Interfaces                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    TLB_Interface.h                    //
//                                                       //
//                 Copyright (C) 2016 by                 //
//                      Hein Bloed                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version >=2 of the License. //
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
//    e-mail:     hbloed@saga-gis.org                    //
//                                                       //
//    contact:    Hein Bloed                             //
//                Sesamestreet 42                        //
//                Metropolis                             //
//                Middle Earth                           //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__TLB_Interface_H
#define HEADER_INCLUDED__TLB_Interface_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if SAGA_MAJOR_VERSION <= 2 && SAGA_MINOR_VERSION < 4
	#define TLB_INFO_Name			MLB_INFO_Name
	#define TLB_INFO_Category		MLB_INFO_Category
	#define TLB_INFO_Author			MLB_INFO_Author
	#define TLB_INFO_Description	MLB_INFO_Description
	#define TLB_INFO_Version		MLB_INFO_Version
	#define TLB_INFO_Menu_Path		MLB_INFO_Menu_Path
	#define TLB_INTERFACE_SKIP_TOOL	MLB_INTERFACE_SKIP_MODULE
	#define TLB_INTERFACE			MLB_INTERFACE
	#define Create_Tool				Create_Module
	#define CSG_Tool				CSG_Module
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__TLB_Interface_H
