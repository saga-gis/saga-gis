/**********************************************************
 * Version $Id: TLB_Interface.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
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
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


//---------------------------------------------------------
CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Hydrology") );

	case TLB_INFO_Category:
		return( _TL("Terrain Analysis") );

	case TLB_INFO_Author:
		return( SG_T("O. Conrad, V. Olaya (c) 2001-4") );

	case TLB_INFO_Description:
		return( _TL("Tools for digital terrain analysis.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
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
#include "Flow_Length.h"
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

#include "melton_ruggedness.h"

#include "Erosion_LS_Fields.h"

#include "flow_by_slope.h"

#include "Flow_Fields.h"

//---------------------------------------------------------
CSG_Tool *		Create_Tool(int i)
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
	case 15:	return( new CSAGA_Wetness_Index );
	case 16:	return( new CLakeFlood );
	case 17:	return( new CLakeFloodInteractive );
	case 18:	return( new CFlow_MassFlux );
	case 19:	return( new CFlow_Width );
	case 20:	return( new CTWI );
	case 21:	return( new CStream_Power );
	case 22:	return( new CLS_Factor );
	case 23:	return( new CMelton_Ruggedness );
	case 24:	return( new CTCI_Low );
	case 25:	return( new CErosion_LS_Fields );
	case 26:	return( new CFlow_by_Slope );
	case 27:	return( new CFlow_Length );
	case 28:	return( new CFlow_Fields );
	case 29: return(NULL);
	
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
