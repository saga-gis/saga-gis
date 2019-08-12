
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2004 by                 //
//                     Victor Olaya                      //
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
//    e-mail:     volaya@ya.com                          //
//                                                       //
//    contact:    Victor Olaya Ferrero                   //
//                Madrid                                 //
//                Spain                                  //
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
// 2. Place general tool library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Analysis") );

	case TLB_INFO_Category:
		return( _TL("Grid") );

	case TLB_INFO_Author:
		return( _TL("Various authors.") );

	case TLB_INFO_Description:
		return( _TL("Some Grid Analysis Tools.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Grid|Analysis") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Cost_Isotropic.h"
#include "LeastCostPathProfile.h"
#include "LeastCostPathProfile_Points.h"

#include "Grid_CVA.h"
#include "CoveredDistance.h"
#include "Grid_Pattern.h"
#include "Grid_LayerOfMaximumValue.h"
#include "Grid_AHP.h"
#include "owa.h"
#include "Grid_AggregationIndex.h"
#include "CrossClassification.h"

#include "Soil_Texture.h"

#include "fragmentation_standard.h"
#include "fragmentation_resampling.h"
#include "fragmentation_classify.h"

#include "Grid_Accumulation_Functions.h"
#include "Grid_IMCORR.h"

#include "diversity_analysis.h"
#include "diversity_shannon.h"
#include "diversity_simpson.h"
#include "diversity_raos_q.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new CCost_Accumulated );
	case  4:	return( new CLeastCostPathProfile );
	case  5:	return( new CLeastCostPathProfile_Points );

	case  6:	return( new CGrid_CVA );
	case  7:	return( new CCoveredDistance );
	case  8:	return( new CGrid_Pattern );
	case  9:	return( new CLayerOfMaximumValue );
	case 10:	return( new CAHP );
	case 11:	return( new COWA );
	case 12:	return( new CAggregationIndex );
	case 13:	return( new CCrossClassification );

	case 14:	return( new CSoil_Texture );
	case 20:	return( new CSoil_Texture_Table );

	case 15:	return( new CFragmentation_Standard );
	case 16:	return( new CFragmentation_Resampling );
	case 17:	return( new CFragmentation_Classify );

	case 18:	return( new CGrid_Accumulation_Functions );

	case 19:	return( new CGrid_IMCORR );

	case 21:	return( new CDiversity_Analysis );
	case 22:	return( new CDiversity_Shannon );
	case 23:	return( new CDiversity_Simpson );
	case 24:	return( new CDiversity_Raos_Q_Classic );
	case 25:	return( new CDiversity_Raos_Q );

	case 26:	return( NULL );
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
