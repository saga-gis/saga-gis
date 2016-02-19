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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#ifdef _OPENMP

#include <omp.h>

//---------------------------------------------------------
int		g_SG_OMP_Max_Num_Threads	= omp_get_num_procs();

//---------------------------------------------------------
void	SG_OMP_Set_Max_Num_Threads		(int iCores)
{
	if( iCores > omp_get_num_procs() )
	{
		iCores	= omp_get_num_procs();
	}

	omp_set_num_threads(g_SG_OMP_Max_Num_Threads = iCores);
}

//---------------------------------------------------------
int		SG_OMP_Get_Max_Num_Threads		(void)
{
	return( g_SG_OMP_Max_Num_Threads );
}

//---------------------------------------------------------
int		SG_OMP_Get_Max_Num_Procs		(void)
{
	return( omp_get_num_procs() );
}

//---------------------------------------------------------
#else
void	SG_OMP_Set_Max_Num_Threads	(int iCores)	{}
int		SG_OMP_Get_Max_Num_Threads	(void)	{	return( 1 );	}
int		SG_OMP_Get_Max_Num_Procs	(void)	{	return( 1 );	}
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
#include <time.h>

//---------------------------------------------------------
CSG_Time_Converter::CSG_Time_Converter(void)
{
	m_sec		= 0;
	m_min		= 0;
	m_hour		= 0;
	m_day		= 0;
	m_mon		= 0;
	m_year		= 0;

	m_StrFmt	= SG_TIME_STRFMT_YMD_hms;
}

//---------------------------------------------------------
CSG_Time_Converter::CSG_Time_Converter(int Time, TSG_Time_Format Format)
{
	Set_Time(Time, Format);
}

//---------------------------------------------------------
bool CSG_Time_Converter::Set_Time(int Time, TSG_Time_Format Format)
{
	switch( Format )
	{
	default:
		{
			return( false );
		}

	//-----------------------------------------------------
	case SG_TIME_FMT_Seconds_Unix:
		{
			time_t		tUnix	= Time;
			struct tm	t;

			#ifdef _SAGA_LINUX
				t	= *gmtime(&tUnix);
			#else
				gmtime_s(&t, &tUnix);
			#endif
			
			m_sec	= t.tm_sec;
			m_min	= t.tm_min;
			m_hour	= t.tm_hour;
			m_day	= t.tm_mday;
			m_mon	= t.tm_mon;
			m_year	= t.tm_year + 1900;
		}
		break;

	//-----------------------------------------------------
	case SG_TIME_FMT_Hours_AD:
		{
			long	d, n;
			double	h;

			h		= 1721424.0 + (Time - 12.0) / 24.0;
			d		= (long)h;		// Truncate to integral day
			h		= h - d + 0.5;	// Fractional part of calendar day
			if( h >= 1.0 )			// Is it really the next calendar day?
			{
				h--;
				d++;
			}

			h		= 24.0 * (h);
			m_hour	= (int)h;
			h		= 60.0 * (h - m_hour);
			m_min	= (int)h;
			h		= 60.0 * (h - m_min);
			m_sec	= (int)h;

			d		= d + 68569;
			n		= 4 * d / 146097;
			d		= d - (146097 * n + 3) / 4;
			m_year	= 4000 * (d + 1) / 1461001;
			d		= d - 1461 * m_year / 4 + 31;	// 1461 = 365.25 * 4
			m_mon	= 80 * d / 2447;
			m_day	= d - 2447 * m_mon / 80;
			d		= m_mon / 11;
			m_mon	= m_mon + 2 - 12 * d;
			m_year	= 100 * (n - 49) + m_year + d;
		}
		break;
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
CSG_String CSG_Time_Converter::Get_String(void) const
{
	CSG_String	s;

	s.Printf(SG_T("%04d.%02d.%02d %02d:%02d:%02d"), m_year, m_mon, m_day, m_hour, m_min, m_sec);

	return( s );
}

//---------------------------------------------------------
CSG_String CSG_Time_Converter::Get_String(int Time, TSG_Time_Format Format)
{
	CSG_Time_Converter	tc(Time, Format);

	return( tc.Get_String() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
