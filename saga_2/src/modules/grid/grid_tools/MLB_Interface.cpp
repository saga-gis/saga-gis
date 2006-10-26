
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Tools                       //
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
		return( _TL("Grid - Tools") );

	case MLB_INFO_Author:
		return( _TL("Olaf Conrad, Victor Olaya, Volker Wichmann (c) 2002-5") );

	case MLB_INFO_Description:
		return( _TL("Tools for the manipulation of gridded data.") );

	case MLB_INFO_Version:
		return( "1.0" );

	case MLB_INFO_Menu_Path:
		return( _TL("Grid|Tools") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Grid_Orientation.h"

#include "Grid_Resample.h"
#include "Grid_Aggregate.h"
#include "Grid_Cut.h"
#include "Grid_Merge.h"
#include "Grid_Completion.h"
#include "Grid_Gaps_OneCell.h"
#include "Grid_Gaps.h"
#include "Grid_CombineGrids.h"
#include "Grid_ConstantGrid.h"
#include "Grid_CropToData.h"
#include "Grid_InvertNoData.h"
#include "GridsFromTableAndGrid.h"

#include "Grid_Value_Type.h"
#include "Grid_Value_Replace.h"
#include "Grid_Value_Replace_Interactive.h"
#include "Grid_Value_Request.h"
#include "Grid_Value_Reclassify.h"

#include "Grid_Buffer.h"
#include "Grid_ThresholdBuffer.h"

#include "Grid_SortRaster.h"

#include "Grid_Fill.h"
//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	// Don't forget to continuously enumerate the case switches
	// when adding new modules! Also bear in mind that the
	// enumeration always has to start with [case 0:] and
	// that [default:] must return NULL!...

	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CGrid_Resample;
		break;

	case 1:
		pModule	= new CGrid_Aggregate;
		break;

	case 2:
		pModule	= new CGrid_Cut;
		break;

	case 3:
		pModule	= new CGrid_Merge;
		break;

	case 4: 
		pModule = new CConstantGrid;
		break;

	case 5:
		pModule	= new CGrid_Completion;
		break;

	case 6:
		pModule	= new CGrid_Gaps_OneCell;
		break;

	case 7:
		pModule	= new CGrid_Gaps;
		break;

	case 8:
		pModule	= new CGrid_Buffer;
		break;

	case 9: 
		pModule = new CThresholdBuffer;
		break;

	case 10:
		pModule	= new CGrid_Value_Type;
		break;

	case 11:
		pModule	= new CGrid_Value_Replace;
		break;

	case 12:
		pModule	= new CGrid_Value_Replace_Interactive;
		break;

	case 13:
		pModule	= new CGrid_Value_Request;
		break;

	case 14:
		pModule	= new CGrid_Value_Reclassify;
		break;

	case 15:
		pModule	= new CGrid_Fill;
		break;

	case 16: 
		pModule = new CCropToData;
		break;

	case 17: 
		pModule = new CInvertNoData;
		break;

	case 18:
		pModule	= new CGrid_Orientation;
		break;
		
	case 19: 
		pModule = new CCombineGrids;
		break;

	case 20: 
		pModule = new CSortRaster;
		break;

	case 21: 
		pModule = new CGridsFromTableAndGrid;
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
