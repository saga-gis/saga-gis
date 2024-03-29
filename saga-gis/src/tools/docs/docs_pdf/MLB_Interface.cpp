
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       docs_pdf                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include <hpdf_version.h>


//---------------------------------------------------------
CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name: default:
		return( _TL("PDF") );

	case TLB_INFO_Category:
		return( _TL("Reports") );

	case TLB_INFO_Author:
		return( "SAGA User Group (c) 2010" );

	case TLB_INFO_Description:
		return( _TW(
			"Create reports in the Portable Document Format (PDF). "
			"The export tools are using the free open source library "
			"<a target=\"_blank\" href=\"https://github.com/libharu/libharu\">libHaru</a> "
			"version ") + CSG_String(HPDF_VERSION_TEXT)
		);

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("File|Reports") );
	}
}


//---------------------------------------------------------
#include "Shapes_Report.h"
#include "Shapes_Summary.h"
#include "Profile_Cross_Sections.h"


//---------------------------------------------------------
CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0: return( new CShapes_Report );
	case  1: return( new CShapes_Summary );
	case  2: return( new CProfile_Cross_Sections );

	case  3: return( NULL );
	default: return( TLB_INTERFACE_SKIP_TOOL );
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

	TLB_INTERFACE

//}}AFX_SAGA
