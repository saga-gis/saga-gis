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
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//                University of Hamburg                  //
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
#include "MLB_Interface.h"


//---------------------------------------------------------
CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Point Clouds") );

	case TLB_INFO_Category:
		return( _TL("Shapes") );

	case TLB_INFO_Author:
		return( SG_T("O.Conrad, V.Wichmann, M.Bremer (c) 2009-15") );

	case TLB_INFO_Description:
		return( _TL("Tools for point clouds.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Shapes|Point Clouds") );
	}
}


//---------------------------------------------------------
#include "pc_attribute_calculator.h"
#include "pc_cluster_analysis.h"
#include "pc_cut.h"
#include "pc_drop_attribute.h"
#include "pc_from_grid.h"
#include "pc_from_shapes.h"
#include "pc_from_table.h"
#include "pc_merge.h"
#include "pc_reclass_extract.h"
#include "pc_thinning_simple.h"
#include "pc_to_grid.h"
#include "pc_to_shapes.h"
#include "pc_transform.h"


//---------------------------------------------------------
CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case 0:		return( new CPC_Cut );
	case 1:		return( new CPC_Cut_Interactive );
	case 2:		return( new CPC_From_Grid );
	case 3:		return( new CPC_From_Shapes );
	case 4:		return( new CPC_To_Grid );
	case 5:		return( new CPC_To_Shapes );
	case 6:		return( new CPC_Reclass_Extract );
	case 7:		return( new CPC_Drop_Attribute );
	case 8:		return( new CPC_Transform );
	case 9:		return( new CPC_Thinning_Simple );
	case 10:	return( new CPC_Attribute_Calculator );
	case 11:	return( new CPC_Cluster_Analysis );
	case 12:	return( new CPC_Merge );
	case 13:	return( new CPC_From_Table );
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

	TLB_INTERFACE

//}}AFX_SAGA
