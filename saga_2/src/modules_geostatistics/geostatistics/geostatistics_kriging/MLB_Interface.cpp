
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 Geostatistics_Kriging                 //
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
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general module library informations here...

const SG_Char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Geostatistics - Kriging") );

	case MLB_INFO_Author:
		return( SG_T("(c) 2003-08 by O.Conrad") );

	case MLB_INFO_Description:
		return( _TL("Kriging - geostatistical procedures for the gridding of irregular distributed point data." ));

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Geostatistics" ));
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "kriging_ordinary.h"
#include "kriging_ordinary_global.h"
#include "kriging_universal.h"
#include "kriging_universal_global.h"

#include "semivariogram.h"

#include "_kriging_ordinary.h"
#include "_kriging_ordinary_global.h"
#include "_kriging_universal.h"
#include "_kriging_universal_global.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case 0:		return( new CKriging_Ordinary );
	case 1:		return( new CKriging_Ordinary_Global );
	case 2:		return( new CKriging_Universal );
	case 3:		return( new CKriging_Universal_Global );

	case 4:		return( new CSemiVariogram );

	case 5:		return( new C_Kriging_Ordinary );
	case 6:		return( new C_Kriging_Ordinary_Global );
	case 7:		return( new C_Kriging_Universal );
	case 8:		return( new C_Kriging_Universal_Global );
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
