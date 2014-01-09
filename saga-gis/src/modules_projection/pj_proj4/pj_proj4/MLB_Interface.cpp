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
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
		return( _TL("Projection - Proj.4") );

	case MLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2004-8") );

	case MLB_INFO_Description:
		return( _TW(
			"Coordinate transformation based on the "
			"<a target=\"_blank\" href=\"http://trac.osgeo.org/proj/\">Proj.4</a> library."
		));

	case MLB_INFO_Version:
		return( _TL("2.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Projection") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "crs_assign.h"
#include "crs_transform_shapes.h"
#include "crs_transform_grid.h"

#include "PROJ4_Shapes.h"
#include "PROJ4_Grid.h"

#include "gcs_lon_range.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CCRS_Assign() );
	case  1:	return( new CCRS_Transform_Shapes(true ) );
	case  2:	return( new CCRS_Transform_Shapes(false) );
	case  3:	return( new CCRS_Transform_Grid  (true ) );
	case  4:	return( new CCRS_Transform_Grid  (false) );

	case  5:	return( new CPROJ4_Shapes	(PROJ4_INTERFACE_SIMPLE, false) );
	case  6:	return( new CPROJ4_Shapes	(PROJ4_INTERFACE_DIALOG, false) );
	case  7:	return( new CPROJ4_Grid		(PROJ4_INTERFACE_SIMPLE, false) );
	case  8:	return( new CPROJ4_Grid		(PROJ4_INTERFACE_DIALOG, false) );
	case  9:	return( new CPROJ4_Shapes	(PROJ4_INTERFACE_SIMPLE, true) );
	case 10:	return( new CPROJ4_Shapes	(PROJ4_INTERFACE_DIALOG, true) );
	case 11:	return( new CPROJ4_Grid		(PROJ4_INTERFACE_SIMPLE, true) );
	case 12:	return( new CPROJ4_Grid		(PROJ4_INTERFACE_DIALOG, true) );

	case 13:	return( new CGCS_Grid_Longitude_Range() );
	}

	return( NULL );
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
