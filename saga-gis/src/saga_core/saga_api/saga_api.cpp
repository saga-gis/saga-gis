/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    saga_api.cpp                       //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "saga_api.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char *	SAGA_API_Get_Version(void)
{
	return( SG_T("SAGA Application Programming Interface - Version: ") SAGA_VERSION );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Compares two versions numbers.
  * Returns -1 if it is less, 1 if it is greater or 0 if it is equal.
  * Version string is assumed to have the form 'Major.Minor.Release'.
*/
//---------------------------------------------------------
int	SG_Compare_Version	(const CSG_String &Version, int Major, int Minor, int Release)
{
	int		v;

	if( !Version                .asInt(v) )	return( -1 );
	if( v < Major   )	return( -1 );
	if( v > Major   )	return(  1 );

	if( !Version.AfterFirst('.').asInt(v) )	return( -1 );
	if( v < Minor   )	return( -1 );
	if( v > Minor   )	return(  1 );

	if( !Version.AfterLast ('.').asInt(v) )	return( -1 );
	if( v < Release )	return( -1 );
	if( v > Release )	return(  1 );

	return( 0 );
}

//---------------------------------------------------------
/**
  * Compares two versions numbers.
  * Returns -1 if it is less, 1 if it is greater or 0 if it is equal.
  * Version strings are assumed to have the form 'Major.Minor.Release'.
*/
//---------------------------------------------------------
int	SG_Compare_Version	(const CSG_String &Version, const CSG_String &Release)
{
	int	v[3];

	if( !Release                .asInt(v[0]) )	return( -1 );
	if( !Release.AfterFirst('.').asInt(v[1]) )	return( -1 );
	if( !Release.AfterLast ('.').asInt(v[2]) )	return( -1 );

	return( SG_Compare_Version(Version, v[0], v[1], v[2]) );
}

//---------------------------------------------------------
/**
  * Compares to current SAGA version and returns -1 if it is
  * less, 1 if it is greater or 0 if it is equal.
*/
//---------------------------------------------------------
int SG_Compare_SAGA_Version(int Major, int Minor, int Release)
{
	if( Major   < SAGA_MAJOR_VERSION  )	return( -1 );
	if( Major   > SAGA_MAJOR_VERSION  )	return(  1 );

	if( Minor   < SAGA_MINOR_VERSION  )	return( -1 );
	if( Minor   > SAGA_MINOR_VERSION  )	return(  1 );

	if( Release < SAGA_RELEASE_NUMBER )	return( -1 );
	if( Release > SAGA_RELEASE_NUMBER )	return(  1 );

	return( 0 );
}

//---------------------------------------------------------
/**
  * Compares to current SAGA version and returns -1 if it is
  * less, 1 if it is greater or 0 if it is equal.
  * Version string is assumed to have the form 'Major.Minor.Release'.
*/
//---------------------------------------------------------
int SG_Compare_SAGA_Version(const CSG_String &Version)
{
	int	v[3];

	if( !Version                .asInt(v[0]) )	return( -1 );
	if( !Version.AfterFirst('.').asInt(v[1]) )	return( -1 );
	if( !Version.AfterLast ('.').asInt(v[2]) )	return( -1 );

	return( SG_Compare_SAGA_Version(v[0], v[1], v[2]) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
