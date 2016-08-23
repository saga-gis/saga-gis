/**********************************************************
 * Version $Id: TLB_Interface.cpp 1925 2014-01-09 12:15:18Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Table_ODBC                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    TLB_Interface.h                    //
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


///////////////////////////////////////////////////////////
//														 //
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general tool library informations here...

const SG_Char *	Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Tables using MySQL native driver") );

	case TLB_INFO_Category:
		return( _TL("Import/Export") );

	case TLB_INFO_Author:
		return( _TL("Ferhat Bingöl (c) 2009") );

	case TLB_INFO_Description:
		return( _TL("Database access through the libmysql.lib interface.") );

	case TLB_INFO_Version:
		return( _TL("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("File|Table") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Get_Table.h"

//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	CSG_Tool	*pTool;

	switch( i )
	{
	case 0:
		pTool	= new CGet_Table;
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
