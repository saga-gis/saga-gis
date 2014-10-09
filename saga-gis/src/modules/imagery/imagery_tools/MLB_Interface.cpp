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
//                    imagery_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                 SAGA User Group Assoc.                //
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
//    e-mail:     author@email.de                        //
//                                                       //
//    contact:    Author                                 //
//                Sesame Street. 7                       //
//                12345 Metropolis                       //
//                Nirwana                                //
//                                                       //
///////////////////////////////////////////////////////////


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
		return( _TL("Tools") );

	case MLB_INFO_Category:
		return( _TL("Imagery") );

	case MLB_INFO_Author:
		return( SG_T("SAGA User Group Assoc. (c) 2009") );

	case MLB_INFO_Description:
		return( _TL("Image processing tools.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Imagery|Tools") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Image_VI_Distance.h"
#include "Image_VI_Slope.h"
#include "evi.h"
#include "tasseled_cap.h"

#include "pansharpening.h"

#include "landsat_toar.h"
#include "landsat_acca.h"
#include "landsat_import.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CImage_VI_Distance );
	case  1:	return( new CImage_VI_Slope );
	case  2:	return( new CEnhanced_VI );
	case  3:	return( new CTasseled_Cap );

	case  4:	return( new CPanSharp_IHS );
	case  5:	return( new CPanSharp_Brovey );
	case  6:	return( new CPanSharp_CN );
	case  7:	return( new CPanSharp_PCA );

	case  8:	return( new CLandsat_TOAR );
	case  9:	return( new CLandsat_ACCA );
	case 10:	return( new CLandsat_Import );

	//-----------------------------------------------------
	case 11:	return( NULL );
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
