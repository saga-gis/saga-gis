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
		return( SG_T("Various Authors") );

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
#include "mass_balance_index.h"
#include "air_flow_height.h"
#include "anisotropic_heating.h"
#include "land_surface_temperature.h"
#include "relative_heights.h"
#include "wind_effect.h"
#include "ruggedness.h"


//---------------------------------------------------------
CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case 0:		return( new CMorphometry );
	case 1:		return( new CConvergence );
	case 2:		return( new CConvergence_Radius );
	case 3:		return( new CSurfaceSpecificPoints );
	case 4:		return( new CCurvature_Classification );
	case 5:		return( new CHypsometry );
	case 6:		return( new CRealArea );
	case 7:		return( new CProtectionIndex );
	case 8:		return( new CMRVBF );
	case 9:		return( new CDistance_Gradient );
	case 10:	return( new CMass_Balance_Index );
	case 11:	return( new CAir_Flow_Height );
	case 12:	return( new CAnisotropic_Heating );
	case 13:	return( new CLand_Surface_Temperature );
	case 14:	return( new CRelative_Heights );
	case 15:	return( new CWind_Effect );
	case 16:	return( new CRuggedness_TRI );
	case 17:	return( new CRuggedness_VRM );
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
