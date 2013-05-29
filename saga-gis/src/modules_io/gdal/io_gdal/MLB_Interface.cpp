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
//                     Grid_IO_GDAL                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                        Author                         //
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
//    e-mail:     author@email.de                        //
//                                                       //
//    contact:    Author                                 //
//                Sesame Street 7                        //
//                12345 Metropolis                       //
//                Nirwana                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "gdal_driver.h"


//---------------------------------------------------------
// 2. Place general module library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Import/Export - GDAL/OGR") );

	case MLB_INFO_Author:
		return( _TL("SAGA User Group Associaton (c) 2008" ));

	case MLB_INFO_Description:
		return( CSG_String::Format(SG_T("%s\n%s %s\n%s: %s"),
			_TL("Interface to Frank Warmerdam's Geospatial Data Abstraction Library (GDAL)."),
			_TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str(),
			_TL("Homepage"), SG_T("<a target=\"_blank\" href=\"http://www.gdal.org/\">www.gdal.org</a>\n")
		));

	case MLB_INFO_Version:
		return( SG_T("2.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("File|GDAL/OGR") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "gdal_import.h"
#include "gdal_export.h"
#include "gdal_export_geotiff.h"
#include "gdal_import_netcdf.h"

#include "ogr_import.h"
#include "ogr_export.h"
#include "ogr_export_kml.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CGDAL_Import );
	case  1:	return( new CGDAL_Export );
	case  2:	return( new CGDAL_Export_GeoTIFF );

	case  3:	return( new COGR_Import );
	case  4:	return( new COGR_Export );

	case  5:	return( new COGR_Export_KML );

	case  6:	return( SG_Get_GDAL_Drivers().Get_Driver("netCDF") ? new CGDAL_Import_NetCDF : NULL );

	//-----------------------------------------------------
	case  9:	return( NULL );
	default:	return( MLB_INTERFACE_SKIP_MODULE );
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
