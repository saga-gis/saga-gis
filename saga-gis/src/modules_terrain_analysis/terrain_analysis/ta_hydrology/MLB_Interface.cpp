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
//                     ta_hydrology                      //
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
		return( _TL("Terrain Analysis - Hydrology") );

	case MLB_INFO_Author:
		return( SG_T("O. Conrad, V. Olaya (c) 2001-4") );

	case MLB_INFO_Description:
		return( _TL("Tools for digital terrain analysis.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Terrain Analysis|Hydrology") );
	}
}


//---------------------------------------------------------
#include "Flow_Parallel.h"
#include "Flow_RecursiveUp.h"
#include "Flow_RecursiveDown.h"
#include "Flow_AreaUpslope.h"
#include "Flow_AreaDownslope.h"

#include "Flow_Distance.h"
#include "SlopeLength.h"

#include "EdgeContamination.h"

#include "IsochronesConst.h"
#include "IsochronesVar.h"

#include "CellBalance.h"
#include "Sinuosity.h"

#include "FlowDepth.h"

#include "TopographicIndices.h"
#include "SAGA_Wetness_Index.h"

#include "LakeFlood.h"

#include "flow_massflux.h"
#include "flow_width.h"


//---------------------------------------------------------
CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CFlow_Parallel );
	case  1:	return( new CFlow_RecursiveUp );
	case  2:	return( new CFlow_RecursiveDown );
	case  3:	return( new CFlow_AreaUpslope_Interactive );
	case  4:	return( new CFlow_AreaUpslope_Area );
	case  5:	return( new CFlow_AreaDownslope );
	case  6:	return( new CFlow_Distance );
	case  7:	return( new CSlopeLength );
	case  8:	return( new CIsochronesConst );
	case  9:	return( new CIsochronesVar );
	case 10:	return( new CCellBalance );
	case 11:	return( new CSinuosity );
	case 12:	return( new CFlowDepth );
	case 13:	return( new CEdgeContamination );
	case 14:	return( new CTopographicIndices );
	case 15:	return( new CSAGA_Wetness_Index );
	case 16:	return( new CLakeFlood );
	case 17:	return( new CLakeFloodInteractive );
	case 18:	return( new CFlow_MassFlux );
	case 19:	return( new CFlow_Width );
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
