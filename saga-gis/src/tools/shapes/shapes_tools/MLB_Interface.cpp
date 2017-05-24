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
//                     Shapes_Tools                      //
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
		return( _TL("Shapes") );

	case TLB_INFO_Author:
		return( SG_T("O. Conrad, V. Olaya, V. Wichmann (c) 2002-2016") );

	case TLB_INFO_Description:
		return( _TL("Tools for the manipulation of vector data.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Shapes|Tools") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Shapes_Create_Empty.h"
#include "Shapes_Merge.h"
#include "Shapes_Support_Tool_Chains.h"

#include "QueryBuilder.h"
#include "SearchInTable.h"
#include "SelectByTheme.h"
#include "NewLayerFromSelectedShapes.h"

#include "SeparateShapes.h"
#include "TransformShapes.h"
#include "CreateChartLayer.h"
#include "GraticuleBuilder.h"

#include "shapes_cut.h"
#include "shapes_cut_interactive.h"
#include "shapes_split.h"
#include "shapes_split_randomly.h"
#include "shapes_split_by_attribute.h"
#include "shapes_buffer.h"
#include "shapes_extents.h"

#include "quadtree_structure.h"

#include "shapes_polar_to_cartes.h"
#include "shapes_generate.h"
#include "shapes_convert_vertex_type.h"

#include "LandUse_Scenario.h"

#include "shapes_clean.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new CShapes_Create_Empty );
	case  2:	return( new CShapes_Merge );
	case 24:	return( new CTables_Merge );

	case  3:	return( new CSelect_Numeric );
	case  4:	return( new CSelect_String );
	case  5:	return( new CSelect_Location );

	case  6:	return( new CSelection_Copy );
	case  7:	return( new CSelection_Delete );
	case  8:	return( new CSelection_Invert );

	case  9:	return( new CSeparateShapes );
	case 10:	return( new CTransformShapes );
	case 11:	return( new CCreateChartLayer );
	case 12:	return( new CGraticuleBuilder );
	case 13:	return( new CShapes_Cut );
	case 14:	return( new CShapes_Cut_Interactive );
	case 15:	return( new CShapes_Split );
	case 16:	return( new CShapes_Split_Randomly );
	case 17:	return( new CShapes_Split_by_Attribute );
	case 18:	return( new CShapes_Buffer );
	case 19:	return( new CShapes_Extents );
	case 20:	return( new CQuadTree_Structure );

	case 21:	return( new CShapes_Polar_to_Cartes );
	case 22:	return( new CShapes_Generate );
	case 23:	return( new CShapes_Convert_Vertex_Type );

	case 25:	return( new CLandUse_Scenario );

	case 26:	return( new CSelect_Shapes_From_List );

	case 27:	return( new CShapes_Clean );

	case 30:	return( NULL );
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
