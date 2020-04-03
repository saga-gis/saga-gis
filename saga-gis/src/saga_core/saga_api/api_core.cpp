
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
#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _OPENMP

#include <omp.h>

//---------------------------------------------------------
int		g_SG_OMP_Max_Num_Threads	= omp_get_num_procs();

//---------------------------------------------------------
void	SG_OMP_Set_Max_Num_Threads	(int iCores)
{
	if( iCores < 1 )
	{
		iCores	= 1;
	}
	else if( iCores > omp_get_num_procs() )
	{
		iCores	= omp_get_num_procs();
	}

	omp_set_num_threads(g_SG_OMP_Max_Num_Threads = iCores);
}

//---------------------------------------------------------
int		SG_OMP_Get_Max_Num_Threads	(void)
{
	return( g_SG_OMP_Max_Num_Threads );
}

//---------------------------------------------------------
int		SG_OMP_Get_Max_Num_Procs	(void)
{
	return( omp_get_num_procs() );
}

//---------------------------------------------------------
int		SG_OMP_Get_Thread_Num		(void)
{
	return( omp_get_thread_num() );
}

//---------------------------------------------------------
#else
void	SG_OMP_Set_Max_Num_Threads	(int iCores)	{}
int		SG_OMP_Get_Max_Num_Threads	(void)	{	return( 1 );	}
int		SG_OMP_Get_Max_Num_Procs	(void)	{	return( 1 );	}
int		SG_OMP_Get_Thread_Num		(void)	{	return( 0 );	}
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
	default                : return( _TL("undefined") );
	case SG_DATATYPE_Bit   : return( _TL("bit") );
	case SG_DATATYPE_Byte  : return( _TL("unsigned 1 byte integer") );
	case SG_DATATYPE_Char  : return( _TL("signed 1 byte integer") );
	case SG_DATATYPE_Word  : return( _TL("unsigned 2 byte integer") );
	case SG_DATATYPE_Short : return( _TL("signed 2 byte integer") );
	case SG_DATATYPE_DWord : return( _TL("unsigned 4 byte integer") );
	case SG_DATATYPE_Int   : return( _TL("signed 4 byte integer") );
	case SG_DATATYPE_ULong : return( _TL("unsigned 8 byte integer") );
	case SG_DATATYPE_Long  : return( _TL("signed 8 byte integer") );
	case SG_DATATYPE_Float : return( _TL("4 byte floating point number") );
	case SG_DATATYPE_Double: return( _TL("8 byte floating point number") );
	case SG_DATATYPE_String: return( _TL("string") );
	case SG_DATATYPE_Date  : return( _TL("date") );
	case SG_DATATYPE_Color : return( _TL("color") );
	case SG_DATATYPE_Binary: return( _TL("binary") );
	}
};

//---------------------------------------------------------
CSG_String	SG_Data_Type_Get_Identifier	(TSG_Data_Type Type)
{
	return( gSG_Data_Type_Identifier[Type] );
}

//---------------------------------------------------------
TSG_Data_Type SG_Data_Type_Get_Type(const CSG_String &Identifier)
{
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Bit   ]) )	return( SG_DATATYPE_Bit    );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Byte  ]) )	return( SG_DATATYPE_Byte   );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Char  ]) )	return( SG_DATATYPE_Char   );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Word  ]) )	return( SG_DATATYPE_Word   );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Short ]) )	return( SG_DATATYPE_Short  );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_DWord ]) )	return( SG_DATATYPE_DWord  );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Int   ]) )	return( SG_DATATYPE_Int    );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_ULong ]) )	return( SG_DATATYPE_ULong  );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Long  ]) )	return( SG_DATATYPE_Long   );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Float ]) )	return( SG_DATATYPE_Float  );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Double]) )	return( SG_DATATYPE_Double );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_String]) )	return( SG_DATATYPE_String );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Date  ]) )	return( SG_DATATYPE_Date   );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Color ]) )	return( SG_DATATYPE_Color  );

	return( SG_DATATYPE_Undefined );
}

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
bool SG_Data_Type_Range_Check(TSG_Data_Type Type, double &Value)
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
