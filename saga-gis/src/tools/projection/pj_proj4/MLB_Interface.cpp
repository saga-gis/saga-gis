
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   Projection_Proj4                    //
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
// 1. Include the appropriate SAGA-API header...

#include "crs_transform.h"


//---------------------------------------------------------
// 2. Place general tool library information here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("PROJ") );

	case TLB_INFO_Category:
		return( _TL("Projection") );

	case TLB_INFO_Author:
		return( "O. Conrad (c) 2004-24" );

	case TLB_INFO_Description:
		return( CSG_CRSProjector::Get_Description() );

	case TLB_INFO_Version:
		return( "2.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Projection") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "crs_assign.h"

#include "crs_transform_shapes.h"
#include "crs_transform_grid.h"
#include "crs_transform_point.h"
#include "crs_transform_utm.h"
#include "crs_transform_coords.h"

#include "gcs_lon_range.h"
#include "gcs_graticule.h"
#include "gcs_rotated_grid.h"

#include "crs_indicatrix.h"
#include "crs_grid_geogcoords.h"
#include "crs_distance.h"

#include "crs_definition.h"

#include "globe_gores.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0: return( new CCRS_Assign() );

	case 29: return( new CCRS_Transform_Point() );

	case  1: return( new CCRS_Transform_Shapes    (true ) );
	case  2: return( new CCRS_Transform_Shapes    (false) );
	case  3: return( new CCRS_Transform_Grid      (true ) );
	case  4: return( new CCRS_Transform_Grid      (false) );

	case 23: return( new CCRS_Transform_UTM_Grids (true ) );
	case 24: return( new CCRS_Transform_UTM_Grids (false) );
	case 25: return( new CCRS_Transform_UTM_Shapes(true ) );
	case 26: return( new CCRS_Transform_UTM_Shapes(false) );

	#if PROJ_VERSION_MAJOR >= 6
	case 19: return( new CCRS_Definition() );
	#endif

	case 15: return( new CCRS_Picker() );

	case 13: return( new CGCS_Grid_Longitude_Range() );
	case 14: return( new CGCS_Graticule() );
	case 18: return( new CGCS_Rotated_Grid() );

	case 16: return( new CCRS_Indicatrix() );
	case 17: return( new CCRS_Grid_GeogCoords() );
	case 20: return( new CCRS_Distance_Lines() );
	case 21: return( new CCRS_Distance_Points() );
	case 22: return( new CCRS_Distance_Interactive() );

	case 30: return( new CCRS_Transform_Coords_Grid () );
	case 31: return( new CCRS_Transform_Coords_Table() );

	case 32: return( new CGlobe_Gores() );

	case 33: return( NULL );
	default: return( TLB_INTERFACE_SKIP_TOOL );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TLB_INTERFACE_ESTABLISH
TLB_INTERFACE_FINALIZE

//---------------------------------------------------------
#include <proj.h>

//---------------------------------------------------------
extern "C" _SAGA_DLL_EXPORT bool TLB_Initialize	(const SG_Char *TLB_Path)
{
	#if _SAGA_MSW && PROJ_VERSION_MAJOR >= 6
	CSG_String Path;
	if( SG_Get_Environment("PROJ_LIB", &Path) )
	{
		char *Paths[1] = { NULL };
		Path.to_UTF8(&Paths[0]);
		proj_context_set_search_paths(NULL, 1, Paths);
		SG_FREE_SAFE(Paths[0]);
	}
	#endif

	return( TLB_Interface.Create(SAGA_VERSION, TLB_Path, Get_Info, Create_Tool) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
