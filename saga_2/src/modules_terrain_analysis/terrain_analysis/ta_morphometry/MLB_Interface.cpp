
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_morphometry                     //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "MLB_Interface.h"


//---------------------------------------------------------
const SG_Char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Terrain Analysis - Morphometry") );

	case MLB_INFO_Author:
		return( _TL("Olaf Conrad, Goettingen (c) 2001") );

	case MLB_INFO_Description:
		return( _TL("Tools for (grid based) digital terrain analysis.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Terrain Analysis|Morphometry") );
	}
}


//---------------------------------------------------------
#include "Morphometry.h"
#include "Convergence.h"
#include "Convergence_Radius.h"
#include "SurfaceSpecificPoints.h"
#include "Curvature_Classification.h"
#include "Hypsometry.h"
#include "RealArea.h"
#include "ProtectionIndex.h"
#include "mrvbf.h"
#include "distance_gradient.h"

//---------------------------------------------------------
CSG_Module *		Create_Module(int i)
{
	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CMorphometry;
		break;

	case 1:
		pModule	= new CConvergence;
		break;

	case 2:
		pModule	= new CConvergence_Radius;
		break;

	case 3:
		pModule	= new CSurfaceSpecificPoints;
		break;

	case 4:
		pModule	= new CCurvature_Classification;
		break;

	case 5:
		pModule	= new CHypsometry;
		break;

	case 6:
		pModule	= new CRealArea;
		break;

	case 7:
		pModule	= new CProtectionIndex;
		break;

	case 8:
		pModule	= new CMRVBF;
		break;

	case 9:
		pModule	= new CDistance_Gradient;
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
