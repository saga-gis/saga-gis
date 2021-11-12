
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
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
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "gdal_driver.h"


//---------------------------------------------------------
// 2. Place general tool library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("GDAL/OGR") );

	case TLB_INFO_Category:
		return( _TL("Import/Export") );

	case TLB_INFO_Author:
		return( _TL("SAGA User Group Associaton (c) 2008-21" ));

	case TLB_INFO_Description:
		return( CSG_String::Format(SG_T("%s\n%s %s\n%s: %s"),
			_TL("Interface to Frank Warmerdam's Geospatial Data Abstraction Library (GDAL)."),
			_TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str(),
			_TL("Homepage"), SG_T("<a target=\"_blank\" href=\"http://www.gdal.org/\">www.gdal.org</a>\n")
		));

	case TLB_INFO_Version:
		return( "2.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("File") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "gdal_import.h"
#include "gdal_export.h"
#include "gdal_export_geotiff.h"
#include "gdal_import_netcdf.h"
#include "gdal_import_wms.h"
#include "gdal_import_aster.h"
#include "gdal_catalogue.h"

#ifdef GDAL_V2_1_OR_NEWER
#include "gdal_buildvrt.h"
#endif
#include "gdal_import_vrt.h"

#include "ogr_import.h"
#include "ogr_export.h"
#include "ogr_export_kml.h"

#include "gdal_formats.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	default:	return( TLB_INTERFACE_SKIP_TOOL );

	case  0:	return( new CGDAL_Import );
	case  1:	return( new CGDAL_Export );
	case  2:	return( new CGDAL_Export_GeoTIFF );

	case  7:	return( new CGDAL_Catalogue );
	case  8:	return( new CGDAL_Catalogues );

	case  3:	return( new COGR_Import );
	case  4:	return( new COGR_Export );

	case  5:	return( new COGR_Export_KML );

	case  6:	return( SG_Get_GDAL_Drivers().Get_Driver("netCDF") ? new CGDAL_Import_NetCDF : TLB_INTERFACE_SKIP_TOOL );

	case  9:	return( new CGDAL_Import_WMS );
	case 11:	return( new CGDAL_Import_ASTER );

#ifdef GDAL_V2_1_OR_NEWER
	case 12:	return( new CGDAL_BuildVRT );
#endif
	case 13:	return( new CGDAL_Import_VRT );

	case 10:	return( new CGDAL_Formats );


	//-----------------------------------------------------
	case 14:	// initializations

		CPLSetErrorHandler(CPLQuietErrorHandler);

		return( NULL );
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
