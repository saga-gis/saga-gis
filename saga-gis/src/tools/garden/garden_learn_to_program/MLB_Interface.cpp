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
//                       Lectures                        //
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
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general tool library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Introducing Tool Programming" ));

	case TLB_INFO_Category:
		return( _TL("Garden") );

	case TLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2003" ));

	case TLB_INFO_Description:
		return( _TL("A set of basic SAGA tool programming examples." ));

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Garden|Introducing Tool Programming" ));
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Exercise_01.h"
#include "Exercise_02.h"
#include "Exercise_03.h"
#include "Exercise_04.h"
#include "Exercise_05.h"
#include "Exercise_06.h"
#include "Exercise_07.h"
#include "Exercise_08.h"
#include "Exercise_09.h"
#include "Exercise_10.h"
#include "Exercise_11.h"
#include "Exercise_12.h"
#include "Exercise_13.h"
#include "Exercise_14.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	// Don't forget to continuously enumerate the case switches
	// when adding new tools! Also bear in mind that the
	// enumeration always has to start with [case 0:] and
	// that [default:] must return NULL!...

	switch( i )
	{
	case  0:	return( new CExercise_01 );
	case  1:	return( new CExercise_02 );
	case  2:	return( new CExercise_03 );
	case  3:	return( new CExercise_04 );
	case  4:	return( new CExercise_05 );
	case  5:	return( new CExercise_06 );
	case  6:	return( new CExercise_07 );
	case  7:	return( new CExercise_08 );
	case  8:	return( new CExercise_09 );
	case  9:	return( new CExercise_10 );
	case 10:	return( new CExercise_11 );
	case 11:	return( new CExercise_12 );
	case 12:	return( new CExercise_13 );
	case 13:	return( new CExercise_14 );

	default:	return( NULL );
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
