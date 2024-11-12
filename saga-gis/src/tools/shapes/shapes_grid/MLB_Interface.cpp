
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Shapes                      //
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
		return( _TL("Shapes-Grid Tools") );

	case TLB_INFO_Category:
		return( _TL("Shapes") );

	case TLB_INFO_Author:
		return( "O. Conrad, V.Wichmann (c) 2002-23" );

	case TLB_INFO_Description:
		return( _TL("Tools related to gridded and vector data (conversions, combinations, etc.).") );

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Shapes|Shapes-Grid Tools") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Grid_Values_AddTo_Points.h"
#include "Grid_Values_AddTo_Shapes.h"
#include "Grid_Statistics_AddTo_Polygon.h"
#include "Grid_Statistics_For_Points.h"
#include "Grid_To_Points.h"
#include "Grid_To_Points_Random.h"
#include "Grid_To_Contour.h"
#include "Grid_Classes_To_Shapes.h"
#include "Grid_Polygon_Clip.h"
#include "Grid_To_Gradient.h"
#include "grid_local_extremes_to_points.h"
#include "grid_extent.h"
#include "grid_rectangle_clip.h"
#include "Grid_Class_Statistics_For_Polygons.h"
#include "boundary_cells_to_polygons.h"
#include "grid_line_direction.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0: return( new CGrid_Values_AddTo_Points );
	case  1: return( new CGrid_Values_AddTo_Shapes );
	case  2: return( new CGrid_Statistics_AddTo_Polygon );
	case  8: return( new CGrid_Statistics_For_Points );
	case  3: return( new CGrid_To_Points );
	case  4: return( new CGrid_To_Points_Random );
	case  5: return( new CGrid_To_Contour );
	case  6: return( new CGrid_Classes_To_Shapes );
	case  7: return( new CGrid_Polygon_Clip );

	case  9: return( new CGrid_Local_Extremes_to_Points );
	case 10: return( new CGrid_Extent );
	case 11: return( new CGrid_Rectangle_Clip );

	case 15: return( new CGrid_To_Gradient(0) );
	case 16: return( new CGrid_To_Gradient(1) );
	case 17: return( new CGrid_To_Gradient(2) );

	case 18: return( new CGrid_Class_Statistics_For_Polygons );

	case 19: return( new CBoundary_Cells_to_Polygons );
	case 20: return( new CGridLineDirection );

	//-----------------------------------------------------
	case 21: return( NULL );
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
