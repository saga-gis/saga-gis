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
//                       TIN_Tools                       //
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
		return( _TL("TIN - Tools") );

	case MLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2004") );

	case MLB_INFO_Description:
		return( _TL("Tools for Triangulated Irregular Network (TIN) processing.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
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
CSG_Module *		Create_Module(int i)
{
	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CTIN_From_Grid;
		break;

	case 1:
		pModule	= new CTIN_From_Grid_Specific_Points;
		break;

	case 2:
		pModule	= new CTIN_From_Shapes;
		break;

	case 3:
		pModule	= new CTIN_To_Shapes;
		break;

	case 4:
		pModule	= new CTIN_Gradient;
		break;

	case 5:
		pModule	= new CTIN_Flow_Trace;
		break;

	case 6:
		pModule	= new CTIN_Flow_Parallel;
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
