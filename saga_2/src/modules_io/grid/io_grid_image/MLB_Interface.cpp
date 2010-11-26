
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       image_io                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
//    contact:    SAGA User Group Association            //
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

const SG_Char *	Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Import/Export - Images") );

	case MLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2005") );

	case MLB_INFO_Description:
		return( _TL("Image Import/Export.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("File|Grid") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "grid_export.h"
#include "grid_import.h"

#include <wx/image.h>


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case 0:	return( new CGrid_Export );
	case 1:	return( new CGrid_Import );
	}

	//------------------------------------------------------
	wxInitAllImageHandlers();

//	wxImage::AddHandler(new wxBMPHandler);	// For loading and saving.
//	wxImage::AddHandler(new wxICOHandler);	// For loading and saving.
//	wxImage::AddHandler(new wxJPEGHandler);	// For loading and saving.
//	wxImage::AddHandler(new wxPCXHandler);	// For loading and saving.
//	wxImage::AddHandler(new wxPNGHandler);	// For loading (including alpha support) and saving.
//	wxImage::AddHandler(new wxPNMHandler);	// For loading and saving.
//	wxImage::AddHandler(new wxTIFFHandler);	// For loading and saving.
//	wxImage::AddHandler(new wxXPMHandler);	// For loading and saving.
//	wxImage::AddHandler(new wxGIFHandler);	// Only for loading, due to legal issues.

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
