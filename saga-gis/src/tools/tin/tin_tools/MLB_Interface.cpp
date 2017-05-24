/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       TIN_Tools                       //
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
		return( _TL("Tools") );

	case TLB_INFO_Category:
		return( _TL("TIN") );

	case TLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2004") );

	case TLB_INFO_Description:
		return( _TL("Tools for Triangulated Irregular Network (TIN) processing.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("TIN") );
	}
}


//---------------------------------------------------------
#include "TIN_From_Grid.h"
#include "TIN_From_Grid_Specific_Points.h"
#include "TIN_From_Shapes.h"
#include "TIN_To_Shapes.h"
#include "TIN_Gradient.h"
#include "TIN_Flow_Trace.h"
#include "TIN_Flow_Parallel.h"


//---------------------------------------------------------
CSG_Tool *		Create_Tool(int i)
{
	CSG_Tool	*pTool;

	switch( i )
	{
	case 0:
		pTool	= new CTIN_From_Grid;
		break;

	case 1:
		pTool	= new CTIN_From_Grid_Specific_Points;
		break;

	case 2:
		pTool	= new CTIN_From_Shapes;
		break;

	case 3:
		pTool	= new CTIN_To_Shapes;
		break;

	case 4:
		pTool	= new CTIN_Gradient;
		break;

	case 5:
		pTool	= new CTIN_Flow_Trace;
		break;

	case 6:
		pTool	= new CTIN_Flow_Parallel;
		break;

	default:
		pTool	= NULL;
		break;
	}

	return( pTool );
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
