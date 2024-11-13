
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2023 by                 //
//                      Olaf Conrad                      //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include <saga_api/saga_api.h>


//---------------------------------------------------------
// 2. Place general tool library information here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Filter") );

	case TLB_INFO_Category:
		return( _TL("Grid") );

	case TLB_INFO_Author:
		return( "SAGA User Group Associaton, HfT Stuttgart (c) 2002-23" );

	case TLB_INFO_Description:
		return( _TL("Various filter and convolution tools for raster data.") );

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Grid|Filter") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

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
#include "Filter_in_Polygon.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0: return( new CFilter );
	case  1: return( new CFilter_Gauss );
	case  2: return( new CFilter_LoG );
	case  3: return( new CFilter_Multi_Dir_Lee );
	case  4: return( new CFilter_3x3 );
	case  5: return( new CFilterClumps );
	case  6: return( new CFilter_Majority );
	case  7: return( new CFilter_Terrain_SlopeBased );
	case  8: return( new CFilter_Morphology );
	case  9: return( new CFilter_Rank );
	case 10: return( new CMesh_Denoise_Grid );
	case 11: return( new CFilter_Resample );
	case 12: return( new CGeomrec );
	case 13: return( new Cbin_erosion_reconst );
	case 14: return( new Cconnectivity_analysis );
	case 15: return( new CFilter_Sieve );
	case 16: return( new CWombling );
	case 17: return( new CWombling_MultiFeature );
	case 18: return( new CFilter_in_Polygon );
	case 19: return( new CFilter_LoG_Sharpening );

	//-----------------------------------------------------
	case 20: return( NULL );
	default: return( TLB_INTERFACE_SKIP_TOOL );
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
