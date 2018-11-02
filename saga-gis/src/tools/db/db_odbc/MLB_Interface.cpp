/**********************************************************
 * Version $Id: TLB_Interface.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        db_odbc                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2004 by                 //
//                     Olaf Conrad                       //
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
//														 //
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general tool library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("ODBC/OTL") );

	case TLB_INFO_Category:
		return( _TL("Import/Export") );

	case TLB_INFO_Author:
		return( SG_T("O.Conrad (c) 2010") );

	case TLB_INFO_Description:
		return( _TW(
			"Database access via Open Data Base Connection (ODBC) interface. "
			"Based on the OTL (Oracle, Odbc and DB2-CLI Template Library), Version 4.0: "
			"<a target=\"_blank\" href=\"http://otl.sourceforge.net/\">http://otl.sourceforge.net/</a>"
		));

	case TLB_INFO_Version:
		return( _TL("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Database|ODBC") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "get_connection.h"
#include "table.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  9:	return( new CGet_Servers );
	case  0:	return( new CGet_Connection );
	case  1:	return( new CDel_Connection );
	case 11:	return( new CDel_Connections );
	case  2:	return( new CTransaction );
	case  3:	return( new CExecute_SQL );

	case 10:	return( new CTable_List );
	case  4:	return( new CTable_Info );
	case  5:	return( new CTable_Load );
	case  6:	return( new CTable_Save );
	case  7:	return( new CTable_Drop );
	case  8:	return( new CTable_Query );

	case 12:	return( NULL );
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

	TLB_INTERFACE_CORE TLB_INTERFACE_INITIALIZE

//}}AFX_SAGA

//---------------------------------------------------------
extern "C" _SAGA_DLL_EXPORT bool TLB_Finalize	(void)
{
	TLB_Interface.Destroy();

	return( SG_ODBC_Get_Connection_Manager().Destroy() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
