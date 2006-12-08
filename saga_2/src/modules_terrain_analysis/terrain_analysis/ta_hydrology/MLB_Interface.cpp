
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
		return( _TL("Olaf Conrad, Victor Olaya (c) 2001-4") );

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


//---------------------------------------------------------
CSG_Module *		Create_Module(int i)
{
	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CFlow_Parallel;
		break;

	case 1:
		pModule	= new CFlow_RecursiveUp;
		break;

	case 2:
		pModule	= new CFlow_RecursiveDown;
		break;

	case 3:
		pModule	= new CFlow_AreaUpslope_Interactive;
		break;

	case 4:
		pModule	= new CFlow_AreaUpslope_Area;
		break;

	case 5:
		pModule	= new CFlow_AreaDownslope;
		break;

	case 6:
		pModule	= new CFlow_Distance;
		break;

	case 7:
		pModule = new CSlopeLength;
		break;

	case 8:
		pModule	= new CIsochronesConst;
		break;

	case 9:
		pModule	= new CIsochronesVar;
		break;

	case 10:
		pModule	= new CCellBalance;
		break;

	case 11:
		pModule	= new CSinuosity;
		break;

	case 12:
		pModule	= new CFlowDepth;
		break;

	case 13:
		pModule	= new CEdgeContamination;
		break;

	case 14:
		pModule	= new CTopographicIndices;
		break;

	case 15:
		pModule	= new CSAGA_Wetness_Index;
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
