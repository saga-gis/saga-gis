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

const SG_Char *	Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Import/Export - Tables using MySQL native driver") );

	case MLB_INFO_Author:
		return( _TL("Ferhat Bingöl (c) 2009") );

	case MLB_INFO_Description:
		return( _TL("Database access through the libmysql.lib interface.") );

	case MLB_INFO_Version:
		return( _TL("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("File|Table") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Get_Table.h"

//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CGet_Table;
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
