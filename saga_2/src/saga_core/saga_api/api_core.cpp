
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
//                     api_core.cpp                      //
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
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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
#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char *	SG_Data_Type_Get_Name	(TSG_Data_Type Type)
{
	switch( Type )
	{
	default:					return( LNG("undefined") );
	case SG_DATATYPE_Bit:		return( LNG("bit") );
	case SG_DATATYPE_Byte:		return( LNG("unsigned 1 byte integer") );
	case SG_DATATYPE_Char:		return( LNG("signed 1 byte integer") );
	case SG_DATATYPE_Word:		return( LNG("unsigned 2 byte integer") );
	case SG_DATATYPE_Short:		return( LNG("signed 2 byte integer") );
	case SG_DATATYPE_DWord:		return( LNG("unsigned 4 byte integer") );
	case SG_DATATYPE_Int:		return( LNG("signed 4 byte integer") );
	case SG_DATATYPE_ULong:		return( LNG("unsigned 8 byte integer") );
	case SG_DATATYPE_Long:		return( LNG("signed 8 byte integer") );
	case SG_DATATYPE_Float:		return( LNG("4 byte floating point number") );
	case SG_DATATYPE_Double:	return( LNG("8 byte floating point number") );
	case SG_DATATYPE_String:	return( LNG("string") );
	case SG_DATATYPE_Date:		return( LNG("date") );
	case SG_DATATYPE_Color:		return( LNG("color") );
	case SG_DATATYPE_Binary:	return( LNG("binary") );
	}
};


//---------------------------------------------------------
bool SG_Data_Type_is_Numeric(TSG_Data_Type Type)
{
	switch( Type )
	{
	case SG_DATATYPE_Bit:
	case SG_DATATYPE_Byte:
	case SG_DATATYPE_Char:
	case SG_DATATYPE_Word:
	case SG_DATATYPE_Short:
	case SG_DATATYPE_DWord:
	case SG_DATATYPE_Int:
	case SG_DATATYPE_ULong:
	case SG_DATATYPE_Long:
	case SG_DATATYPE_Float:
	case SG_DATATYPE_Double:
		return( true );

	default:
	case SG_DATATYPE_String:
	case SG_DATATYPE_Date:
	case SG_DATATYPE_Color:
	case SG_DATATYPE_Binary:
		return( false );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
