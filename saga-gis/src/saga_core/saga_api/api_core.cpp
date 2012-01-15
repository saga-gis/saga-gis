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

#ifdef _OPENMP
#include <omp.h>

//---------------------------------------------------------
int		g_SG_Max_Num_Threads_Omp = omp_get_num_procs();

//---------------------------------------------------------
void	SG_Set_Max_Num_Threads_Omp		(int iCores)
{
	if( iCores > omp_get_num_procs() )
		iCores = omp_get_num_procs();

	g_SG_Max_Num_Threads_Omp	= iCores;
	omp_set_num_threads(g_SG_Max_Num_Threads_Omp);
}

//---------------------------------------------------------
int		SG_Get_Max_Num_Threads_Omp		(void)
{
	return( g_SG_Max_Num_Threads_Omp );
}

//---------------------------------------------------------
int		SG_Get_Max_Num_Procs_Omp		(void)
{
	return( omp_get_num_procs() );
}
#endif

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String	SG_Data_Type_Get_Name	(TSG_Data_Type Type)
{
	switch( Type )
	{
	default:					return( _TL("undefined") );
	case SG_DATATYPE_Bit:		return( _TL("bit") );
	case SG_DATATYPE_Byte:		return( _TL("unsigned 1 byte integer") );
	case SG_DATATYPE_Char:		return( _TL("signed 1 byte integer") );
	case SG_DATATYPE_Word:		return( _TL("unsigned 2 byte integer") );
	case SG_DATATYPE_Short:		return( _TL("signed 2 byte integer") );
	case SG_DATATYPE_DWord:		return( _TL("unsigned 4 byte integer") );
	case SG_DATATYPE_Int:		return( _TL("signed 4 byte integer") );
	case SG_DATATYPE_ULong:		return( _TL("unsigned 8 byte integer") );
	case SG_DATATYPE_Long:		return( _TL("signed 8 byte integer") );
	case SG_DATATYPE_Float:		return( _TL("4 byte floating point number") );
	case SG_DATATYPE_Double:	return( _TL("8 byte floating point number") );
	case SG_DATATYPE_String:	return( _TL("string") );
	case SG_DATATYPE_Date:		return( _TL("date") );
	case SG_DATATYPE_Color:		return( _TL("color") );
	case SG_DATATYPE_Binary:	return( _TL("binary") );
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

//---------------------------------------------------------
bool SG_DataType_Range_Check(TSG_Data_Type Type, double &Value)
{
	double	min, max;

	switch( Type )
	{
		default:
		case SG_DATATYPE_Double:	Value	= (double)Value;	return( true );
		case SG_DATATYPE_Float:		Value	= (float )Value;	return( true );

		case SG_DATATYPE_Bit:		min	=           0.0;	max =          1.0;	break;
		case SG_DATATYPE_Byte:		min	=           0.0;	max =        255.0; break;
		case SG_DATATYPE_Char:		min	=        -128.0;	max =        127.0;	break;
		case SG_DATATYPE_Word:		min	=           0.0;	max =      65535.0;	break;
		case SG_DATATYPE_Short:		min	=      -32768.0;	max =      32767.0;	break;
		case SG_DATATYPE_DWord:		min	=           0.0;	max = 4294967295.0;	break;
		case SG_DATATYPE_Int:		min	= -2147483648.0;	max = 2147483647.0;	break;
	}

	if( Value < min )
	{
		Value	= min;
	}
	else if( Value > max )
	{
		Value	= max;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
