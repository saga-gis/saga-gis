/**********************************************************
 * Version $Id: MLB_Interface.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                imagery_classification                 //
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
		return( _TL("Classification") );

	case MLB_INFO_Category:
		return( _TL("Imagery") );

	case MLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2002-11") );

	case MLB_INFO_Description:
		return( _TL("Classification tools for grids.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Imagery|Classification") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "classify_supervised.h"
#include "classify_cluster_analysis.h"
#include "change_detection.h"
#include "decision_tree.h"
#include "classify_supervised_polygons.h"
#include "classification_quality.h"
#include "classify_isocluster.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CGrid_Classify_Supervised );
	case  1:	return( new CGrid_Cluster_Analysis );
	case  2:	return( new CChange_Detection );
	case  3:	return( new CDecision_Tree );
	case  4:	return( new CPolygon_Classify_Supervised( true) );
	case  5:	return( new CPolygon_Classify_Supervised(false) );
	case  6:	return( new CClassification_Quality );
	case  7:	return( new CGrid_Cluster_ISODATA );

	case  8:	return( NULL );
	default:	return( MLB_INTERFACE_SKIP_MODULE );
	}
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
