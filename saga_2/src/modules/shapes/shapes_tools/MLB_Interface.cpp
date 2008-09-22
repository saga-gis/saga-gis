
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Shapes_Tools                      //
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

const SG_Char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Shapes - Tools") );

	case MLB_INFO_Author:
		return( _TL("Olaf Conrad, Victor Olaya (c) 2002-2006") );

	case MLB_INFO_Description:
		return( _TL("Tools for the manipulation of vector data.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Shapes|Tools") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Shapes_Create_Empty.h"
#include "Shapes_Assign_Table.h"
#include "Shapes_Merge.h"
#include "Shapes_Report.h"
#include "CreateWebContent.h"

#include "QueryBuilder.h"
#include "SearchInTable.h"
#include "SelectByTheme.h"
#include "SeparateShapes.h"
#include "TransformShapes.h"
#include "CreateChartLayer.h"
#include "NewLayerFromSelectedShapes.h"
#include "GraticuleBuilder.h"

#include "Summarize.h"

#include "shapes_cut.h"
#include "shapes_cut_interactive.h"
#include "shapes_split.h"
#include "shapes_split_randomly.h"
#include "shapes_split_by_attribute.h"
#include "shapes_buffer.h"
#include "shapes_extents.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CShapes_Create_Empty );
	case  1:	return( new CShapes_Assign_Table );
	case  2:	return( new CShapes_Merge );
	case  3:	return( new CNewLayerFromSelectedShapes );
	case  4:	return( new CQueryBuilder );
	case  5:	return( new CSearchInTable );
	case  6:	return( new CSelectByTheme );
	case  7:	return( new CSeparateShapes );
	case  8:	return( new CTransformShapes );
	case  9:	return( new CCreateChartLayer );
	case 10:	return( new CGraticuleBuilder );
	case 11:	return( new CShapes_Report );
	case 12:	return( new CSummarize );
	case 13:	return( new CCreateWebContent );
	case 14:	return( new CShapes_Cut );
	case 15:	return( new CShapes_Cut_Interactive );
	case 16:	return( new CShapes_Split );
	case 17:	return( new CShapes_Split_Randomly );
	case 18:	return( new CShapes_Split_by_Attribute );
	case 19:	return( new CShapes_Buffer );
	case 20:	return( new CShapes_Extents );
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
