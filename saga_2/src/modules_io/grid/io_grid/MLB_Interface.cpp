
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        Grid_IO                        //
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
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general module library informations here...

const char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Import/Export - Grids") );

	case MLB_INFO_Author:
		return( _TL("SAGA User Group Associaton (c) 2002") );

	case MLB_INFO_Description:
		return( _TL("Tools for the import and export of gridded data.") );

	case MLB_INFO_Version:
		return( "1.0" );

	case MLB_INFO_Menu_Path:
		return( _TL("File|Grid") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "esri_arcinfo.h"
#include "surfer.h"
#include "raw.h"
#include "xyz.h"
#include "usgs_srtm.h"
#include "mola.h"
#include "srtm30.h"
#include "bmp_export.h"
#include "erdas_lan.h"
#include "grid_table.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CModule *		Create_Module(int i)
{
	// Don't forget to continuously enumerate the case switches
	// when adding new modules! Also bear in mind that the
	// enumeration always has to start with [case 0:] and
	// that [default:] must return NULL!...

	CModule	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CESRI_ArcInfo_Export;
		break;

	case 1:
		pModule	= new CESRI_ArcInfo_Import;
		break;

	case 2:
		pModule	= new CSurfer_Export;
		break;

	case 3:
		pModule	= new CSurfer_Import;
		break;

	case 4:
		pModule	= new CRaw_Import;
		break;

	case 5:
		pModule	= new CXYZ_Export;
		break;

	case 6:
		pModule	= new CUSGS_SRTM_Import;
		break;

	case 7:
		pModule	= new CMOLA_Import;
		break;

	case 8:
		pModule	= new CSRTM30_Import;
		break;

	case 9:
		pModule	= new CBMP_Export;
		break;

	case 10:
		pModule	= new CErdas_LAN_Import;
		break;

	case 11:
		pModule	= new CGrid_Table_Import;
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
