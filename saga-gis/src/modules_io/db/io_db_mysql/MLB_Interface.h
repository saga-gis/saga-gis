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
//                      Table_ODBC                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    MLB_Interface.h                    //
//                                                       //
//                 Copyright (C) 2004 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

///////////////////////////////////////////////////////////
//                                                       //
//    This code is something I did for SAGA GIS to       //
//    import data from MySQL Database.					 //
//														 //
//	  INPUT : Connection information and SQL statement	 //
//	  OUTPUT: Result data as Table						 //
//														 //
//		RELEASE : v0.1b (Lot more to do)				 //
//														 //
//	  The SAGA structure code is grapped from 	         //
//    Olaf Conrad's ODBC example and database			 //
//	  communication part is replaced with MySQL			 //
//	  native library (libmysql.dll for win32)			 //
//														 //
//	  The code is compiled under						 //
//	      Visual C++ 2008 Express Edition 	             //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     sagamysql@ferhatbingol.com             //
//                                                       //
//    contact:    Ferhat Bingöl                          //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//				Include the SAGA-API here				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__io_table_mysql_H
#define HEADER_INCLUDED__io_table_mysql_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>

//---------------------------------------------------------
#ifdef io_table_mysql_EXPORTS
	#define	io_table_mysql_EXPORT	_SAGA_DLL_EXPORT
#else
	#define	io_table_mysql_EXPORT	_SAGA_DLL_IMPORT
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__io_table_mysql_H
