/**********************************************************
 * Version $Id: TLB_Interface.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       db_pgsql                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    TLB_Interface.h                    //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__db_pgsql_TLB_Interface_H
#define HEADER_INCLUDED__db_pgsql_TLB_Interface_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>

//---------------------------------------------------------
#ifdef db_pgsql_EXPORTS
	#define	db_pgsql_EXPORT	_SAGA_DLL_EXPORT
#else
	#define	db_pgsql_EXPORT	_SAGA_DLL_IMPORT
#endif

//---------------------------------------------------------
#include "db_pgsql.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__db_pgsql_TLB_Interface_H
