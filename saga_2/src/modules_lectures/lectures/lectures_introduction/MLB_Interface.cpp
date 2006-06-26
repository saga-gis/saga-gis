
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Lectures                        //
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

const char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Lectures - Introducing Module Programming" ));

	case MLB_INFO_Author:
		return( _TL("Olaf Conrad (c) 2003" ));

	case MLB_INFO_Description:
		return( _TL("A set of basic SAGA module programming lectures." ));

	case MLB_INFO_Version:
		return( "1.0" );

	case MLB_INFO_Menu_Path:
		return( _TL("Lectures|Introducing Module Programming" ));
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

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
// 4. Allow your modules to be created here...

CModule *		Create_Module(int i)
{
	// Don't forget to continuously enumerate the case switches
	// when adding new modules! Also bear in mind that the
	// enumeration always has to start with [case 0:] and
	// that [default:] must return NULL!...

	CModule	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CExercise_01;
		break;

	case 1:
		pModule	= new CExercise_02;
		break;

	case 2:
		pModule	= new CExercise_03;
		break;

	case 3:
		pModule	= new CExercise_04;
		break;

	case 4:
		pModule	= new CExercise_05;
		break;

	case 5:
		pModule	= new CExercise_06;
		break;

	case 6:
		pModule	= new CExercise_07;
		break;

	case 7:
		pModule	= new CExercise_08;
		break;

	case 8:
		pModule	= new CExercise_09;
		break;

	case 9:
		pModule	= new CExercise_10;
		break;

	case 10:
		pModule	= new CExercise_11;
		break;

	case 11:
		pModule	= new CExercise_12;
		break;

	case 12:
		pModule	= new CExercise_13;
		break;

	case 13:
		pModule	= new CExercise_14;
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
