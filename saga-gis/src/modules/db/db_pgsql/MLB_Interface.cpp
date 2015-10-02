/**********************************************************
 * Version $Id: MLB_Interface.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       db_pgsql                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
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
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general module library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("PostgreSQL") );

	case MLB_INFO_Category:
		return( _TL("Import/Export") );

	case MLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2013") );

	case MLB_INFO_Description:
		return( _TL("Accessing PostgreSQL databases.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Database|PostgreSQL") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "get_connection.h"
#include "table.h"
#include "pgis_shapes.h"
#include "pgis_raster.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CGet_Connections );
	case  1:	return( new CGet_Connection );
	case  2:	return( new CDel_Connection );
	case  3:	return( new CDel_Connections );
	case  4:	return( new CTransaction_Start );
	case  5:	return( new CTransaction_Stop );
	case  6:	return( new CExecute_SQL );

	case 10:	return( new CTable_List );
	case 11:	return( new CTable_Info );
	case 12:	return( new CTable_Load );
	case 13:	return( new CTable_Save );
	case 14:	return( new CTable_Drop );
	case 15:	return( new CTable_Query );

	case 20:	return( new CShapes_Load );
	case 21:	return( new CShapes_Save );
	case 22:	return( new CShapes_SRID_Update );

	case 30:	return( new CRaster_Load );
	case 33:	return( new CRaster_Load_Band );
	case 31:	return( new CRaster_Save );
	case 32:	return( new CRaster_SRID_Update );

	case 40:	return( NULL );
	default:	return( MLB_INTERFACE_SKIP_MODULE );
	}
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
