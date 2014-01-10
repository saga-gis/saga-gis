/**********************************************************
 * Version $Id: Get_Table.h 1925 2014-01-09 12:15:18Z oconrad $
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

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Get_Table_H
#define HEADER_INCLUDED__Get_Table_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGet_Table : public CSG_Module
{
public:
	CGet_Table(void);
	virtual ~CGet_Table(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("R:Import") );	}


protected:

	virtual bool				On_Execute				(void);

	


private:



};

#endif // #ifndef HEADER_INCLUDED__Get_Table_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
