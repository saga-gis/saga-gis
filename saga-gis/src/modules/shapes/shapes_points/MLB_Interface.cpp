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
//                     shapes_points                     //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     volaya@ya.com                          //
//                                                       //
//    contact:    Victor Olaya Ferrero                   //
//                Madrid                                 //
//                Spain                                  //
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
		return( _TL("Shapes - Points") );

	case MLB_INFO_Author:
		return( _TL("Various Authors") );

	case MLB_INFO_Description:
		return( _TL("Tools for the manipulation of point vector data.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Shapes|Points") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Points_From_Table.h"
#include "Points_From_Lines.h"
#include "CountPoints.h"
#include "CreatePointGrid.h"
#include "DistanceMatrix.h"
#include "FitNPointsToShape.h"
#include "AddCoordinates.h"
#include "remove_duplicates.h"
#include "Clip_Points.h"
#include "separate_by_direction.h"
#include "add_polygon_attributes.h"
#include "points_filter.h"
#include "convex_hull.h"
#include "select_points.h"
#include "points_thinning.h"
#include "thiessen_polygons.h"
#include "gps_track_aggregation.h"
#include "snap_points_to_features.h"
#include "snap_points_to_grid.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CPoints_From_Table );
	case  1:	return( new CCountPoints );
	case  2:	return( new CCreatePointGrid );
	case  3:	return( new CDistanceMatrix );
	case  4:	return( new CFitNPointsToShape );
	case  5:	return( new CPoints_From_Lines );
	case  6:	return( new CAddCoordinates );
	case  7:	return( new CRemove_Duplicates );
	case  8:	return( new CClip_Points );
	case  9:	return( new CSeparate_by_Direction );
	case 10:	return( new CAdd_Polygon_Attributes );
	case 11:	return( new CPoints_Filter );
	case 12:	return( new CConvex_Hull );
	case 13:	return( new CSelect_Points );
	case 14:	return( new CPoints_Thinning );
	case 15:	return( new CPoints_From_MultiPoints );
	case 16:	return( new CThiessen_Polygons );
	case 17:	return( new CGPS_Track_Aggregation );
	case 18:	return( new CSnap_Points_to_Features(SHAPE_TYPE_Point) );
	case 19:	return( new CSnap_Points_to_Features(SHAPE_TYPE_Line) );
	case 20:	return( new CSnap_Points_to_Grid() );
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
