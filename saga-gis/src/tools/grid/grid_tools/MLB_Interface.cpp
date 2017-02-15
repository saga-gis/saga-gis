/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Tools                       //
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
		return( _TL("Tools") );

	case TLB_INFO_Category:
		return( _TL("Grid") );

	case TLB_INFO_Author:
		return( SG_T("O. Conrad, V. Olaya, V. Wichmann (c) 2002-11") );

	case TLB_INFO_Description:
		return( _TL("Tools for the manipulation of gridded data.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Grid|Tools") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Grid_Orientation.h"

#include "Grid_Resample.h"
#include "Grid_Aggregate.h"
#include "Grid_Cut.h"
#include "Grid_Merge.h"
#include "Grid_Completion.h"
#include "Grid_Gaps.h"
#include "Grid_Gaps_OneCell.h"
#include "Grid_Gaps_Resampling.h"
#include "Grid_Gaps_Spline_Fill.h"
#include "Grid_CombineGrids.h"
#include "Grid_ConstantGrid.h"
#include "Grid_CropToData.h"
#include "Grid_InvertNoData.h"
#include "GridsFromTableAndGrid.h"

#include "Grid_Value_NoData.h"
#include "Grid_Value_Type.h"
#include "Grid_Value_Replace.h"
#include "Grid_Value_Replace_Interactive.h"
#include "Grid_Value_Request.h"
#include "Grid_Value_Reclassify.h"

#include "Grid_Buffer.h"
#include "Grid_ThresholdBuffer.h"
#include "Grid_Buffer_Proximity.h"

#include "Grid_SortRaster.h"

#include "Grid_Fill.h"

#include "CreateGridSystem.h"

#include "Grid_Mask.h"
#include "Grid_Proximity.h"
#include "Grid_Tiling.h"
#include "Grid_Shrink_Expand.h"

#include "Grid_Transpose.h"

#include "Grid_Support_Tool_Chains.h"
#include "Grid_Combine_Classes.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new CGrid_Resample );
	case  1:	return( new CGrid_Aggregate );

	case  2:	return( new CGrid_Clip_Interactive );
	case 31: 	return( new CGrid_Clip );

	case  3:	return( new CGrid_Merge );
	case  4: 	return( new CConstantGrid );

	case  5:	return( new CGrid_Completion );
	case  6:	return( new CGrid_Gaps_OneCell );
	case  7:	return( new CGrid_Gaps );
	case 25: 	return( new CGrid_Gaps_Spline_Fill );
	case 29: 	return( new CGrid_Gaps_Resampling );

	case  8:	return( new CGrid_Buffer );
	case  9: 	return( new CThresholdBuffer );
	case 10:	return( new CGrid_Proximity_Buffer );

	case 36:	return( new CGrid_Value_NoData );
	case 11:	return( new CGrid_Value_Type );
	case 12:	return( new CGrid_Value_Replace );
	case 13:	return( new CGrid_Value_Replace_Interactive );
	case 14:	return( new CGrid_Value_Request );
	case 15:	return( new CGrid_Value_Reclassify );

	case 16:	return( new CGrid_Fill );

	case 17: 	return( new CCropToData );

	case 18: 	return( new CInvertNoData );

	case 20: 	return( new CCombineGrids );
	case 21: 	return( new CSortRaster );
	case 22: 	return( new CGridsFromTableAndGrid );
	case 23: 	return( new CCreateGridSystem );
	case 24: 	return( new CGrid_Mask );

	case 26: 	return( new CGrid_Proximity );
	case 27: 	return( new CGrid_Tiling );
	case 28: 	return( new CGrid_Shrink_Expand );
	case 30: 	return( new CGrid_Transpose );
	case 32: 	return( new CSelect_Grid_From_List );

	case 33:	return( new CGrid_Copy );
	case 34:	return( new CGrid_Invert );
	case 35:	return( new CGrid_Mirror );

	case 37:	return( new CGrid_Combine_Classes );

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
