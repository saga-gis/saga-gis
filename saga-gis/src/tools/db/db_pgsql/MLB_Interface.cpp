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
//                   TLB_Interface.cpp                   //
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
		return( _TL("PostgreSQL") );

	case TLB_INFO_Category:
		return( _TL("Import/Export") );

	case TLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2013") );

	case TLB_INFO_Description:
		return( _TL("Accessing PostgreSQL databases.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Database|PostgreSQL") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "get_connection.h"
#include "table.h"
#include "pgis_shapes.h"
#include "pgis_raster.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
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
	case 16:	return( new CTable_Query_GUI );

	case 20:	return( new CShapes_Load );
	case 21:	return( new CShapes_Save );
	case 22:	return( new CShapes_SRID_Update );
	case 23:	return( new CShapes_Join );

	case 30:	return( new CRaster_Load );
	case 33:	return( new CRaster_Load_Band );
	case 31:	return( new CRaster_Save );
	case 34:	return( new CRaster_Collection_Save );
	case 32:	return( new CRaster_SRID_Update );

	case 35:	return( new CDatabase_Create );
	case 36:	return( new CDatabase_Destroy );

	case 40:	return( NULL );
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
