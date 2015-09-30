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
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
//               SAGA User Group Associaton              //
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
//    contact:    SAGA User Group Associaton             //
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
		return( _TL("Filter") );

	case MLB_INFO_Category:
		return( _TL("Grid") );

	case MLB_INFO_Author:
		return( SG_T("SAGA User Group Associaton, HfT Stuttgart (c) 2002-14") );

	case MLB_INFO_Description:
		return( _TL("Tools for the manipulation of gridded data.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Grid|Filter") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Filter.h"
#include "Filter_Gauss.h"
#include "Filter_LoG.h"
#include "Filter_Multi_Dir_Lee.h"
#include "Filter_3x3.h"
#include "FilterClumps.h"
#include "Filter_Majority.h"
#include "Filter_Terrain_SlopeBased.h"
#include "Filter_Morphology.h"
#include "Filter_Rank.h"
#include "mesh_denoise.h"
#include "Filter_Resample.h"
#include "geomrec.h"
#include "bin_erosion_reconst.h"
#include "connect_analysis.h"
#include "Filter_Sieve.h"
#include "wombling.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CFilter );
	case  1:	return( new CFilter_Gauss );
	case  2:	return( new CFilter_LoG );
	case  3:	return( new CFilter_Multi_Dir_Lee );
	case  4:	return( new CFilter_3x3 );
	case  5:	return( new CFilterClumps );
	case  6:	return( new CFilter_Majority );
	case  7:	return( new CFilter_Terrain_SlopeBased );
	case  8:	return( new CFilter_Morphology );
	case  9:	return( new CFilter_Rank );
	case 10:	return( new CMesh_Denoise_Grid );
	case 11:	return( new CFilter_Resample );
	case 12:	return( new CGeomrec );
	case 13:	return( new Cbin_erosion_reconst );
	case 14:	return( new Cconnectivity_analysis );
	case 15:	return( new CFilter_Sieve );
	case 16:	return( new CWombling );
	case 17:	return( new CWombling_MultiFeature );

	case 18:	return( NULL );
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
