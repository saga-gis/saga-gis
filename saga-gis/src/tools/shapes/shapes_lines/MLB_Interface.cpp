
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
		return( _TL("Lines") );

	case TLB_INFO_Category:
		return( _TL("Shapes") );

	case TLB_INFO_Author:
		return( "O. Conrad, V. Wichmann (c) 2005-2024" );

	case TLB_INFO_Description:
		return( _TL("Tools for lines.") );

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Shapes|Lines") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Lines_From_Polygons.h"
#include "Lines_From_Points.h"
#include "line_properties.h"
#include "line_polygon_intersection.h"
#include "line_simplification.h"
#include "line_dissolve.h"
#include "line_split_with_lines.h"
#include "line_smoothing.h"
#include "line_crossings.h"
#include "extract_closed_lines.h"
#include "line_split.h"
#include "line_parts_to_separate_lines.h"
#include "line_flip_direction.h"
#include "merge_line_parts.h"
#include "line_density.h"
#include "topologize.h"
#include "upstream_edges.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0: return( new CLines_From_Polygons );
	case  1: return( new CLines_From_Points );
	case  2: return( new CLine_Properties );
	case  3: return( new CLine_Polygon_Intersection );
	case  4: return( new CLine_Simplification );
	case  5: return( new CLine_Dissolve );
	case  6: return( new CLine_Split_with_Lines );
	case  8: return( new CLine_Split_at_Points );
	case  7: return( new CLine_Smoothing );
	case  9: return( new CLine_Crossings );
    case 10: return( new CExtract_Closed_Lines );
	case 11: return( new CCut_Lines );
	case 12: return( new CLine_Parts_to_Separate_Lines );
	case 13: return( new CLine_Flip_Direction );
	case 14: return( new CMerge_Line_Parts );
	case 15: return( new CLine_Density );
	case 16: return( new CLine_Topology );
	case 17: return( new CLine_UpstreamEdges );

	case 18: return( NULL );
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
