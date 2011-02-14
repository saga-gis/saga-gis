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
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


//---------------------------------------------------------
const SG_Char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Shapes - Point Clouds") );

	case MLB_INFO_Author:
		return( SG_T("O.Conrad, Volker Wichmann (c) 2009-10") );

	case MLB_INFO_Description:
		return( _TL("Tools for point clouds.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
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
#include "pc_reclass_extract.h"
#include "pc_thinning_simple.h"
#include "pc_to_grid.h"
#include "pc_to_shapes.h"
#include "pc_transform.h"


//---------------------------------------------------------
CSG_Module *		Create_Module(int i)
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
