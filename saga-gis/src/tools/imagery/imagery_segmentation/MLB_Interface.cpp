
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 imagery_segmentation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                 SAGA User Group Assoc.                //
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
//    e-mail:     author@email.de                        //
//                                                       //
//    contact:    Author                                 //
//                Sesame Street. 7                       //
//                12345 Metropolis                       //
//                Nirwana                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//           The Tool Link Library Interface             //
//														 //
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
		return( _TL("Segmentation") );

	case TLB_INFO_Category:
		return( _TL("Imagery") );

	case TLB_INFO_Author:
		return( SG_T("SAGA User Group Assoc. (c) 2009-2021") );

	case TLB_INFO_Description:
		return( _TL("Image segmentation algorithms.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Imagery|Segmentation") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "watershed_segmentation.h"
#include "skeletonization.h"
#include "grid_seeds.h"
#include "rga_basic.h"
#include "slic.h"
#include "connected_components.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new CWatershed_Segmentation );
	case  1:	return( new CSkeletonization );
	case  2:	return( new CGrid_Seeds );
	case  3:	return( new CRGA_Basic );
	case  4:	return( new CSLIC );
    case  5:    return( new CConnected_Components );

	//-----------------------------------------------------
	case  6:	return( NULL );
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
