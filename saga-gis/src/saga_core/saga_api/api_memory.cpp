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
//                    api_memory.cpp                     //
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

#ifndef _WINDOWS_

//---------------------------------------------------------
void *		SG_Malloc(size_t size)
{
	return( malloc(size) );
}

//---------------------------------------------------------
void *		SG_Calloc(size_t num, size_t size)
{
	return( calloc(num, size) );
}

//---------------------------------------------------------
void *		SG_Realloc(void *memblock, size_t new_size)
{
	return( realloc(memblock, new_size) );
}

//---------------------------------------------------------
void		SG_Free(void *memblock)
{
	if( memblock )
	{
		free(memblock);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Due to a bad 'feature' in the realloc routine of MS's
// MSVCRT (C-Runtime-Library), we recommend to use our own
// memory allocation routines...

#else	// ifndef _WINDOWS_

void *		SG_Malloc(size_t size)
{
	return( HeapAlloc(GetProcessHeap(), 0, size) );
}

void *		SG_Calloc(size_t num, size_t size)
{
	return( HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, num * size) );
}

void *		SG_Realloc(void *memblock, size_t new_size)
{
	if( new_size > 0 )
	{
		if( memblock )
		{
			return( HeapReAlloc(GetProcessHeap(), 0, memblock, new_size) );
		}
		else
		{
			return( HeapAlloc(GetProcessHeap(), 0, new_size) );
		}
	}
	else
	{
		SG_Free(memblock);

		return( NULL );
	}
}

void		SG_Free(void *memblock)
{
	if( memblock )
	{
		HeapFree(GetProcessHeap(), 0, memblock);
	}
}

#endif	// ifndef _WINDOWS_


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void			SG_Swap_Bytes(void *Buffer, int nBytes)
{
	char	Byte, *pA, *pB;

	pA	= (char *)Buffer;
	pB	= pA + nBytes - 1;

	while( pA < pB )
	{
		Byte	= *pA;
		*(pA++)	= *pB;
		*(pB--)	= Byte;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int				SG_Mem_Get_Int(const char *Buffer, bool bSwapBytes)
{
	int		Value	= *(int *)Buffer;

	if( bSwapBytes )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Value );
}

void			SG_Mem_Set_Int(char *Buffer, int Value, bool bSwapBytes)
{
	if( bSwapBytes )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	*((int *)Buffer)	= Value;
}

//---------------------------------------------------------
double			SG_Mem_Get_Double(const char *Buffer, bool bSwapBytes)
{
	double	Value	= *(double *)Buffer;

	if( bSwapBytes )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	return( Value );
}

void			SG_Mem_Set_Double(char *Buffer, double Value, bool bSwapBytes)
{
	if( bSwapBytes )
	{
		SG_Swap_Bytes(&Value, sizeof(Value));
	}

	*(double *)Buffer	= Value;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Array::CSG_Array(void)
{
	m_Value_Size	= sizeof(char);
	m_Growth		= SG_ARRAY_GROWTH_0;

	m_nBuffer		= 0;
	m_nValues		= 0;
	m_Values		= NULL;
}

//---------------------------------------------------------
CSG_Array::CSG_Array(size_t Value_Size, size_t nValues, TSG_Array_Growth Growth)
{
	Create(Value_Size, nValues, Growth);
}

void * CSG_Array::Create(size_t Value_Size, size_t nValues, TSG_Array_Growth Growth)
{
	m_Value_Size	= Value_Size;
	m_Growth		= Growth;

	m_nBuffer		= 0;
	m_nValues		= 0;
	m_Values		= NULL;

	return( Get_Array(nValues) );
}

//---------------------------------------------------------
CSG_Array::~CSG_Array(void)
{
	Destroy();
}

void CSG_Array::Destroy(void)
{
	if( m_Values )
	{
		SG_Free(m_Values);
	}

	m_nBuffer		= 0;
	m_nValues		= 0;
	m_Values		= NULL;
}

//---------------------------------------------------------
bool CSG_Array::Set_Growth(TSG_Array_Growth Growth)
{
	m_Growth		= Growth;

	return( true );
}

//---------------------------------------------------------
bool CSG_Array::Set_Array(size_t nValues, bool bShrink)
{
	if( nValues >= m_nValues && nValues <= m_nBuffer )
	{
		m_nValues	= nValues;

		return( true );
	}

	if( nValues < m_nValues && !bShrink )
	{
		m_nValues	= nValues;

		return( true );
	}

	if( nValues == 0 )
	{
		Destroy();

		return( true );
	}

	//-----------------------------------------------------
	size_t	nBuffer;

	switch( m_Growth )
	{
	default:
	case SG_ARRAY_GROWTH_0:
		nBuffer	= nValues;
		break;

	case SG_ARRAY_GROWTH_1:
		nBuffer	= nValues <    100 ?      nValues
				: nValues <   1000 ? (1 + nValues /    10) *    10
				: nValues <  10000 ? (1 + nValues /   100) *   100
				: nValues < 100000 ? (1 + nValues /  1000) *  1000
				:                    (1 + nValues / 10000) * 10000;
		break;

	case SG_ARRAY_GROWTH_2:
		nBuffer	= nValues <     10 ?      nValues
				: nValues <    100 ? (1 + nValues /    10) *    10
				: nValues <   1000 ? (1 + nValues /   100) *   100
				: nValues <  10000 ? (1 + nValues /  1000) *  1000
				:                    (1 + nValues / 10000) * 10000;
		break;

	case SG_ARRAY_GROWTH_3:
		nBuffer	= nValues <    1000 ? (1 + nValues /    1000) *    1000
				: nValues <   10000 ? (1 + nValues /   10000) *   10000
				: nValues <  100000 ? (1 + nValues /  100000) *  100000
				:                     (1 + nValues / 1000000) * 1000000;
		break;
	}

	//-----------------------------------------------------
	if( nBuffer == m_nBuffer )
	{
		m_nValues	= nValues;

		return( true );
	}

	//-----------------------------------------------------
	void	*Values	= SG_Realloc(m_Values, nBuffer * m_Value_Size);

	if( Values )
	{
		m_nBuffer	= nBuffer;
		m_nValues	= nValues;
		m_Values	= Values;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Array::Set_Array(size_t nValues, void **pArray, bool bShrink)
{
	if( Set_Array(nValues, bShrink) )
	{
		*pArray	= m_Values;

		return( true );
	}

	*pArray	= m_Values;

	return( false );
}

//---------------------------------------------------------
bool CSG_Array::Inc_Array		(void)
{
	return( Set_Array(m_nValues + 1) );
}

bool CSG_Array::Inc_Array		(void **pArray)
{
	return( Set_Array(m_nValues + 1, pArray) );
}

//---------------------------------------------------------
bool CSG_Array::Dec_Array		(bool bShrink)
{
	return( m_Values > 0 ? Set_Array(m_nValues - 1, bShrink) : false );
}

bool CSG_Array::Dec_Array		(void **pArray, bool bShrink)
{
	return( m_Values > 0 ? Set_Array(m_nValues - 1, pArray, bShrink) : false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Buffer::CSG_Buffer(void)
{
	m_pData	= NULL;
	m_Size	= 0;
}

bool CSG_Buffer::Create(void)
{
	Destroy();

	return( true );
}

//---------------------------------------------------------
CSG_Buffer::CSG_Buffer(const CSG_Buffer &Buffer)
{
	m_pData	= NULL;
	m_Size	= 0;

	Create(Buffer);
}

bool CSG_Buffer::Create(const CSG_Buffer &Buffer)
{
	return( Set_Data(Buffer.m_pData, Buffer.m_Size) );
}

//---------------------------------------------------------
CSG_Buffer::CSG_Buffer(size_t Size)
{
	m_pData	= NULL;
	m_Size	= 0;

	Create(Size);
}

bool CSG_Buffer::Create(size_t Size)
{
	return( Set_Size(Size) );
}

//---------------------------------------------------------
CSG_Buffer::~CSG_Buffer(void)
{
	Destroy();
}

void CSG_Buffer::Destroy(void)
{
	if( m_pData )
	{
		SG_Free(m_pData);
	}

	m_pData	= NULL;
	m_Size	= 0;
}

//---------------------------------------------------------
bool CSG_Buffer::Set_Size(size_t Size, bool bShrink)
{
	if( Size > m_Size || (Size < m_Size && bShrink) )
	{
		char	*pData	= (char *)SG_Realloc(m_pData, Size * sizeof(char));

		if( !pData )
		{
			return( false );
		}

		m_pData	= pData;
		m_Size	= Size;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Buffer::Set_Data(const char *Buffer, size_t Size, bool bShrink)
{
	if( !Buffer || !Size || !Set_Size(Size, bShrink) )
	{
		return( false );
	}

	memcpy(m_pData, Buffer, m_Size);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Bytes::CSG_Bytes(void)
{
	m_Bytes		= NULL;
	m_nBytes	= 0;
	m_nBuffer	= 0;
	m_Cursor	= 0;
}

//---------------------------------------------------------
bool CSG_Bytes::Create(void)
{
	return( Destroy() );
}

//---------------------------------------------------------
CSG_Bytes::CSG_Bytes(const CSG_Bytes &Bytes)
{
	m_Bytes		= NULL;
	m_nBytes	= 0;
	m_nBuffer	= 0;
	m_Cursor	= 0;

	Create(Bytes);
}

bool CSG_Bytes::Create(const CSG_Bytes &Bytes)
{
	return( Assign(Bytes) );
}

//---------------------------------------------------------
CSG_Bytes::CSG_Bytes(const BYTE *Bytes, int nBytes)
{
	m_Bytes		= NULL;
	m_nBytes	= 0;
	m_nBuffer	= 0;
	m_Cursor	= 0;

	Create(Bytes, nBytes);
}

bool CSG_Bytes::Create(const BYTE *Bytes, int nBytes)
{
	Destroy();

	return( Add((void *)Bytes, nBytes, false) );
}

//---------------------------------------------------------
CSG_Bytes::CSG_Bytes(const SG_Char *Bytes)
{
	m_Bytes		= NULL;
	m_nBytes	= 0;
	m_nBuffer	= 0;
	m_Cursor	= 0;

	Create(Bytes);
}

bool CSG_Bytes::Create(const SG_Char *Bytes)
{
	Destroy();

	if( Bytes == NULL )	// Reset
	{
		return( true );
	}

#ifndef _SAGA_UNICODE
	int	nBytes	= SG_STR_LEN(Bytes);
#else
	int	nBytes	= SG_STR_LEN(Bytes) * 2;
#endif

	return( Add((void *)Bytes, nBytes, false) );
}

//---------------------------------------------------------
CSG_Bytes::~CSG_Bytes(void)
{
	Destroy();
}

bool CSG_Bytes::Destroy(void)
{
	if( m_Bytes )
	{
		SG_Free(m_Bytes);
	}

	m_Bytes		= NULL;
	m_nBytes	= 0;
	m_nBuffer	= 0;
	m_Cursor	= 0;

	return( true );
}

bool CSG_Bytes::Clear(void)
{
	m_nBytes	= 0;
	m_Cursor	= 0;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Bytes::_Inc_Array(int nBytes)
{
	if( m_nBuffer < m_nBytes + nBytes )
	{
		int		nBuffer	= m_nBuffer + nBytes + 1024;
		BYTE	*Bytes	= (BYTE *)SG_Realloc(m_Bytes, nBuffer * sizeof(BYTE));

		if( !Bytes )
		{
			return( false );
		}

		m_Bytes		= Bytes;
		m_nBuffer	= nBuffer;
	}

	m_nBytes	+= nBytes;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Bytes::Assign(const CSG_Bytes &Bytes)
{
	Destroy();

	if( _Inc_Array(Bytes.m_nBytes) )
	{
		memcpy(m_Bytes, Bytes.m_Bytes, m_nBytes);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Bytes::Add(const CSG_Bytes &Bytes)
{
	return( Add(Bytes.m_Bytes, Bytes.m_nBytes, false) );
}

//---------------------------------------------------------
bool CSG_Bytes::Add(void *Bytes, int nBytes, bool bSwapBytes)
{
	int		Offset	= m_nBytes;

	if( _Inc_Array(nBytes) )
	{
		memcpy(m_Bytes + Offset, Bytes, nBytes);

		if( bSwapBytes )
		{
			SG_Swap_Bytes(m_Bytes + Offset, nBytes);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BYTE	SG_Hex_to_Byte	(const SG_Char Hex)
{
	switch( Hex )
	{
	case '1':				return(  1 );
	case '2':				return(  2 );
	case '3':				return(  3 );
	case '4':				return(  4 );
	case '5':				return(  5 );
	case '6':				return(  6 );
	case '7':				return(  7 );
	case '8':				return(  8 );
	case '9':				return(  9 );
	case 'a':	case 'A':	return( 10 );
	case 'b':	case 'B':	return( 11 );
	case 'c':	case 'C':	return( 12 );
	case 'd':	case 'D':	return( 13 );
	case 'e':	case 'E':	return( 14 );
	case 'f':	case 'F':	return( 15 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Bytes::toHexString(void) const
{
	CSG_String	HexString;

	for(int i=0; i<m_nBytes; i++)
	{
		HexString	+= CSG_String::Format(SG_T("%02X"), m_Bytes[i]);
	}

	return( HexString );
}

//---------------------------------------------------------
bool CSG_Bytes::fromHexString(const CSG_String &HexString)
{
	Destroy();

	const SG_Char	*s	= HexString.c_str();

	for(size_t i=0; i<HexString.Length(); i+=2, s+=2)
	{
		Add(SG_Hex_to_Byte(s[1]) + 16 * SG_Hex_to_Byte(s[0]));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Bytes_Array::CSG_Bytes_Array(void)
{
	m_pBytes	= NULL;
	m_nBytes	= 0;
	m_nBuffer	= 0;
}

//---------------------------------------------------------
CSG_Bytes_Array::~CSG_Bytes_Array(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Bytes_Array::Destroy(void)
{
	if( m_pBytes )
	{
		for(int i=0; i<m_nBytes; i++)
		{
			delete(m_pBytes[i]);
		}

		SG_Free(m_pBytes);
	}

	m_pBytes	= NULL;
	m_nBytes	= 0;
	m_nBuffer	= 0;

	return( true );
}

//---------------------------------------------------------
CSG_Bytes * CSG_Bytes_Array::Add(void)
{
	if( m_nBytes >= m_nBuffer )
	{
		CSG_Bytes	**pBytes	= (CSG_Bytes **)SG_Realloc(m_pBytes, (m_nBuffer + 256) * sizeof(CSG_Bytes *));

		if( !pBytes )
		{
			return( NULL );
		}

		m_pBytes	 = pBytes;
		m_nBuffer	+= 256;
	}

	return( m_pBytes[m_nBytes++] = new CSG_Bytes );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
