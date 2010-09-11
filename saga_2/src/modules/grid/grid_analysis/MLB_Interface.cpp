
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      grid analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
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
//    e-mail:     volaya@ya.com                          //
//                                                       //
//    contact:    Victor Olaya Ferrero                   //
//                Madrid                                 //
//                Spain                                  //
//                                                       //
///////////////////////////////////////////////////////////


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
		return( _TL("Grid - Analysis") );

	case MLB_INFO_Author:
		return( _TL("Various authors.") );

	case MLB_INFO_Description:
		return( _TL("Some Grid Analysis Tools.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Grid|Analysis") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Grid_CVA.h"
#include "CoveredDistance.h"
#include "Grid_Pattern.h"
#include "CrossClassification.h"
#include "Grid_LayerOfMaximumValue.h"
#include "Grid_AggregationIndex.h"
#include "Grid_AHP.h"
#include "owa.h"

#include "Cost_Isotropic.h"
#include "Cost_Anisotropic.h"
#include "Cost_PolarToRect.h"
#include "Cost_RectToPolar.h"
#include "LeastCostPathProfile.h"
#include "LeastCostPathProfile_Points.h"

#include "Image_VI_Distance.h"
#include "Image_VI_Slope.h"

#include "FuzzyAND.h"
#include "FuzzyOR.h"
#include "Fuzzify.h"

#include "Soil_Texture.h"

#include "fragmentation_standard.h"
#include "fragmentation_resampling.h"
#include "fragmentation_classify.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case 0:		return( new CCost_Isotropic );
	case 1:		return( new CCost_Anisotropic );
	case 2:		return( new CCost_PolarToRect );
	case 3:		return( new CCost_RectToPolar );
	case 4:		return( new CLeastCostPathProfile );
	case 5:		return( new CImage_VI_Distance );
	case 6:		return( new CImage_VI_Slope );
	case 7:		return( new CFuzzyAND );
	case 8:		return( new CFuzzyOR );
	case 9:		return( new CFuzzify );
	case 10:	return( new CGrid_CVA );
	case 11:	return( new CCoveredDistance );
	case 12:	return( new CGrid_Pattern );
	case 13:	return( new CLayerOfMaximumValue );
	case 14:	return( new CAHP );
	case 15:	return( new COWA );
	case 16:	return( new CAggregationIndex );
	case 17:	return( new CCrossClassification );
	case 18:	return( new CSoil_Texture );
	case 19:	return( new CFragmentation_Standard );
	case 20:	return( new CFragmentation_Resampling );
	case 21:	return( new CFragmentation_Classify );
	case 22:	return( new CLeastCostPathProfile_Points );
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
