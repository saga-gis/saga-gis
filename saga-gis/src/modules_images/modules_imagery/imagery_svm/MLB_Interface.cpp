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
//                         SVM                           //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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

#include "svm.h"

//---------------------------------------------------------
CSG_String Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Imagery - SVM") );

	case MLB_INFO_Author:
		return( SG_T("O.Conrad (c) 2012") );

	case MLB_INFO_Description:
		return( _TW(
			"Interface to LIBSVM - A Library for Support Vector Machines.\n"
			"Reference:\n"
			"Chang, C.-C. / Lin, C.-J. (2011): A library for support vector machines. "
			"ACM Transactions on Intelligent Systems and Technology, vol.2/3, p.1-27. "
			"<a target=\"_blank\" href=\"http://www.csie.ntu.edu.tw/~cjlin/libsvm\">LIBSVM Homepage</a>.\n"
		));

	case MLB_INFO_Version:
		return( CSG_String::Format(SG_T("LIBSVM %d.%d"), libsvm_version / 100, libsvm_version - 100 * (libsvm_version / 100)) );

	case MLB_INFO_Menu_Path:
		return( _TL("SVM") );
	}
}

//---------------------------------------------------------
#include "svm_grids.h"

//---------------------------------------------------------
CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CSVM_Grids );

	case 10:	return( NULL );
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
