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
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
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
		return( _TL("Grids") );

	case TLB_INFO_Category:
		return( _TL("Import/Export") );

	case TLB_INFO_Author:
		return( _TL("SAGA User Group Associaton (c) 2002") );

	case TLB_INFO_Description:
		return( _TL("Tools for the import and export of gridded data.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("File|Grid") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

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
#include "wrf.h"
#include "import_clip_resample.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case 0 :	return( new CESRI_ArcInfo_Export );
	case 1 :	return( new CESRI_ArcInfo_Import );
	case 2 :	return( new CSurfer_Export );
	case 3 :	return( new CSurfer_Import );
	case 4 :	return( new CRaw_Import );
	case 5 :	return( new CXYZ_Export );
	case 6 :	return( new CXYZ_Import );
	case 7 :	return( new CUSGS_SRTM_Import );
	case 8 :	return( new CMOLA_Import );
	case 9 :	return( new CSRTM30_Import );
	case 10:	return( new CBMP_Export );
	case 11:	return( new CErdas_LAN_Import );
	case 12:	return( new CGrid_Table_Import );
	case 13:	return( new CWRF_Import );
	case 14:	return( new CWRF_Export );
	case 16:	return( new CImport_Clip_Resample );
	case 17:	return( new CCRU_Table_Import );

	//-----------------------------------------------------
	case 18:	return( NULL );
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
