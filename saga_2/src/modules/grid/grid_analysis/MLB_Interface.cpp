
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

const char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Grid - Analysis") );

	case MLB_INFO_Author:
		return( _TL("Victor Olaya (c) 2004") );

	case MLB_INFO_Description:
		return( _TL("Some Grid Analysis Tools.") );

	case MLB_INFO_Version:
		return( "1.0" );

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

#include "Image_VI_Distance.h"
#include "Image_VI_Slope.h"

#include "FuzzyAND.h"
#include "FuzzyOR.h"
#include "Fuzzify.h"

//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	// Don't forget to continuously enumerate the case switches
	// when adding new modules! Also bear in mind that the
	// enumeration always has to start with [case 0:] and
	// that [default:] must return NULL!...

	CSG_Module	*pModule;

	switch( i )
	{
		
	case 0:
		pModule	= new CCost_Isotropic;
		break;		

	case 1:
		pModule	= new CCost_Anisotropic;
		break;		

	case 2:
		pModule	= new CCost_PolarToRect;
		break;		

	case 3:
		pModule	= new CCost_RectToPolar;
		break;	
		
	case 4:
		pModule = new CLeastCostPathProfile;
		break;

	case 5:
		pModule	= new CImage_VI_Distance;
		break;		

	case 6:
		pModule	= new CImage_VI_Slope;
		break;	

	case 7:
		pModule	= new CFuzzyAND;
		break;

	case 8:
		pModule = new CFuzzyOR;
		break;

	case 9:
		pModule = new CFuzzify;
		break;

	case 10:
		pModule	= new CGrid_CVA;
		break;
	
	case 11:
		pModule = new CCoveredDistance;
		break;

	case 12:
		pModule	= new CGrid_Pattern;
		break;		

	case 13:
		pModule	= new CLayerOfMaximumValue;
		break;	

	case 14:
		pModule	= new CAHP;
		break;	

	case 15:
		pModule	= new COWA;
		break;	

	case 16:
		pModule	= new CAggregationIndex;
		break;	

	case 17:
		pModule	= new CCrossClassification;
		break;
	
	default:
		pModule	= NULL;
		break;
	}

	return( pModule );
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
