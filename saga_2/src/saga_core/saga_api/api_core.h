
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
//                      api_core.h                       //
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
#ifndef HEADER_INCLUDED__SAGA_API__api_core_H
#define HEADER_INCLUDED__SAGA_API__api_core_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _SAGA_MSW
	#define	_SAGA_DLL_EXPORT		__declspec( dllexport )
	#define	_SAGA_DLL_IMPORT		__declspec( dllimport )
#else
	#define	_SAGA_DLL_EXPORT
	#define	_SAGA_DLL_IMPORT
#endif

//---------------------------------------------------------
#ifdef _SAGA_API_EXPORTS
	#define	SAGA_API_DLL_EXPORT		_SAGA_DLL_EXPORT
#else
	#define	SAGA_API_DLL_EXPORT		_SAGA_DLL_IMPORT
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef SWIG

#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#endif	// #ifdef SWIG


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// this is defined by configure, but will not be on a normal
// application build
//
#ifndef SIZEOF_LONG
	#if defined(__alpha) || defined(__sparcv9) || defined(__LP64__) || (defined(__HOS_AIX__) && defined(_LP64))
		#define SIZEOF_LONG        8
	#else
		#define SIZEOF_LONG        4
	#endif
#endif

//---------------------------------------------------------
#ifdef _TYPEDEF_BOOL
	typedef unsigned int	bool;
	#define true			((bool)1)
	#define false			((bool)0)
#endif	// _TYPEDEF_BOOL

//---------------------------------------------------------
#ifdef _TYPEDEF_BYTE
	typedef unsigned char	BYTE;
#endif	// _TYPEDEF_BYTE

//---------------------------------------------------------
#ifdef _TYPEDEF_WORD
	typedef unsigned short	WORD;
	#if (SIZEOF_LONG == 4)
		typedef unsigned long	DWORD;
	#else
		typedef unsigned int	DWORD;
	#endif
#endif	// _TYPEDEF_WORD


///////////////////////////////////////////////////////////
//														 //
//						Memory							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_GET_LONG(b0, b1, b2, b3)	((long) (((BYTE)(b0) | ((WORD)(b1) << 8)) | (((DWORD)(BYTE)(b2)) << 16) | (((DWORD)(BYTE)(b3)) << 24)))

#define SG_GET_BYTE_0(vLong)		((BYTE) ((vLong)      ))
#define SG_GET_BYTE_1(vLong)		((BYTE) ((vLong) >>  8))
#define SG_GET_BYTE_2(vLong)		((BYTE) ((vLong) >> 16))
#define SG_GET_BYTE_3(vLong)		((BYTE) ((vLong) >> 24))

//---------------------------------------------------------
SAGA_API_DLL_EXPORT void *			SG_Malloc			(size_t size);
SAGA_API_DLL_EXPORT void *			SG_Calloc			(size_t num, size_t size);
SAGA_API_DLL_EXPORT void *			SG_Realloc			(void *memblock, size_t size);
SAGA_API_DLL_EXPORT void			SG_Free				(void *memblock);

SAGA_API_DLL_EXPORT void			SG_Swap_Bytes		(void *Buffer, int nBytes);

SAGA_API_DLL_EXPORT int				SG_Mem_Get_Int		(const char *Buffer			, bool bSwapBytes);
SAGA_API_DLL_EXPORT void			SG_Mem_Set_Int		(char *Buffer, int Value	, bool bSwapBytes);
SAGA_API_DLL_EXPORT double			SG_Mem_Get_Double	(const char *Buffer			, bool bSwapBytes);
SAGA_API_DLL_EXPORT void			SG_Mem_Set_Double	(char *Buffer, double Value	, bool bSwapBytes);


///////////////////////////////////////////////////////////
//														 //
//						String							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _SAGA_UNICODE
	#define SG_Char			char
	#define SG_T(s)			s
	#define SG_PRINTF			printf
	#define SG_FPRINTF			fprintf
	#define SG_SSCANF			sscanf
	#define SG_STR_CMP			strcmp
	#define SG_STR_CPY			strcpy
	#define SG_STR_LEN			strlen
	#define SG_STR_TOD			strtod
	#define SG_STR_SGTOMB(s)	s
	#define SG_STR_MBTOSG(s)	s
#else
	#define SG_Char				wchar_t
	#define SG_T(s)				L ## s
	#define SG_PRINTF			SG_Printf
	#define SG_FPRINTF			SG_FPrintf
	#define SG_SSCANF			swscanf
	#define SG_STR_CMP			wcscmp
	#define SG_STR_CPY			wcscpy
	#define SG_STR_LEN			wcslen
	#define SG_STR_TOD			wcstod
	#define SG_STR_SGTOMB(s)	CSG_String(s).b_str()
	#define SG_STR_MBTOSG(s)	CSG_String(s).c_str()
#endif

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_String
{
public:
	CSG_String(void);
	CSG_String(const CSG_String &String);
	CSG_String(const SG_Char *String);
#ifdef _SAGA_UNICODE
	CSG_String(const char *String);
#endif
	CSG_String(SG_Char Character);

	virtual ~CSG_String(void);

	const SG_Char *					c_str				(void)	const;
	operator const SG_Char *							(void)	const	{	return( c_str() );	}

#ifndef _SAGA_UNICODE
	const char *					b_str				(void)			{	return( c_str() );	}
#else
	const char *					b_str				(void);
#endif

	size_t							Length				(void)	const;

	void							Clear				(void);
	static CSG_String				Format				(const SG_Char *Format, ...);
	int								Printf				(const SG_Char *Format, ...);
	int								Scanf				(const SG_Char *Format, ...);

	CSG_String &					Append				(const SG_Char *String);
	CSG_String &					Append				(SG_Char Character);

	CSG_String &					operator =			(const CSG_String &String);
	CSG_String &					operator =			(const SG_Char *String);
	CSG_String &					operator =			(SG_Char Character);

	CSG_String						operator +			(const CSG_String &String)		const;
	CSG_String						operator +			(const SG_Char *String)			const;
	CSG_String						operator +			(SG_Char Character)				const;

	void							operator +=			(const CSG_String &String);
	void							operator +=			(const SG_Char *String);
	void							operator +=			(SG_Char Character);

	SG_Char &						operator []			(int i);

	int								Cmp					(const SG_Char *String)			const;
	int								CmpNoCase			(const SG_Char *String)			const;

	CSG_String &					Make_Lower			(void);
	CSG_String &					Make_Upper			(void);

	size_t							Replace				(const SG_Char *sOld, const SG_Char *sNew, bool replaceAll = true);

	CSG_String &					Remove				(size_t pos);
	CSG_String &					Remove				(size_t pos, size_t len);

	int								Trim				(bool fromRight = false);

	int								Find				(SG_Char Character, bool fromEnd = false);
	int								Find				(const SG_Char *String);
	bool							Contains			(const SG_Char *String);

	CSG_String						AfterFirst			(SG_Char Character)				const;
	CSG_String						AfterLast			(SG_Char Character)				const;
	CSG_String						BeforeFirst			(SG_Char Character)				const;
	CSG_String						BeforeLast			(SG_Char Character)				const;

	CSG_String						Right				(size_t count)					const;
	CSG_String						Mid					(size_t first, size_t count = 0)const;
	CSG_String						Left				(size_t count) const;

	int								asInt				(void)							const;
	bool							asInt				(int &Value)					const;
	double							asDouble			(void)							const;
	bool							asDouble			(double &Value)					const;


protected:

	class wxString					*m_pString;

#ifdef _SAGA_UNICODE
	char							*m_bString;
#endif

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_String		operator +			(const SG_Char *A, const CSG_String &B);
SAGA_API_DLL_EXPORT CSG_String		operator +			(SG_Char        A, const CSG_String &B);

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Strings
{
public:
	CSG_Strings(void);
	CSG_Strings(const CSG_Strings &Strings);
	CSG_Strings(int nStrings, const SG_Char **Strings);

	virtual ~CSG_Strings(void);

	void							Clear				(void);

	CSG_Strings &					operator  =			(const CSG_Strings &Strings);
	bool							Assign				(const CSG_Strings &Strings);

	bool							Add					(const CSG_String &String);
	CSG_Strings &					operator +=			(const CSG_String &String);

	bool							Set_Count			(int nStrings);
	int								Get_Count			(void)		const	{	return( m_nStrings );	}

	CSG_String &					operator []			(int Index) const	{	return( *m_Strings[Index]   );	}
	CSG_String &					Get_String			(int Index) const	{	return( *m_Strings[Index]   );	}


protected:

	int								m_nStrings;

	CSG_String						**m_Strings;

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT int				SG_Printf						(const SG_Char *Format, ...);
SAGA_API_DLL_EXPORT int				SG_FPrintf						(FILE* stream,const SG_Char *Format, ...);
SAGA_API_DLL_EXPORT int				SG_Sscanf						(const SG_Char *Buffer, const SG_Char *Format, ...);

SAGA_API_DLL_EXPORT CSG_String		SG_Get_CurrentTimeStr			(bool bWithDate = true);

SAGA_API_DLL_EXPORT CSG_String		SG_UTF8_To_String				(const SG_Char *String);
SAGA_API_DLL_EXPORT CSG_String		SG_String_To_UTF8				(const SG_Char *String);

SAGA_API_DLL_EXPORT double			SG_Degree_To_Double				(const SG_Char *String);
SAGA_API_DLL_EXPORT CSG_String		SG_Double_To_Degree				(double Value);

SAGA_API_DLL_EXPORT int				SG_Date_To_Number				(const SG_Char *Value);
SAGA_API_DLL_EXPORT CSG_String		SG_Number_To_Date				(int            Value);
SAGA_API_DLL_EXPORT CSG_String		SG_Number_To_Date				(double         Value);

SAGA_API_DLL_EXPORT int				SG_Get_Significant_Decimals		(double Value, int maxDecimals = 6);

SAGA_API_DLL_EXPORT void			SG_Flip_Decimal_Separators		(CSG_String &String);

SAGA_API_DLL_EXPORT CSG_String		SG_Get_String					(double Value, int Precision = 2, bool bScientific = false);


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Buffer
{
public:

						CSG_Buffer		(void);
	bool				Create			(void);

						CSG_Buffer		(const CSG_Buffer &Buffer);
	bool				Create			(const CSG_Buffer &Buffer);

						CSG_Buffer		(size_t Size);
	bool				Create			(size_t Size);

	virtual				~CSG_Buffer		(void);
	void				Destroy			(void);

	bool				Set_Size		(size_t Size, bool bShrink = true);
	bool				Inc_Size		(size_t Size)				{	return( Set_Size(m_Size + Size) );	}
	size_t				Get_Size		(void)				const	{	return( m_Size );	}

	bool				Set_Data		(const char *Buffer, size_t Size, bool bShrink = true);
	char *				Get_Data		(int Offset = 0)	const	{	return( m_pData + Offset );		}
	char				operator []		(int Position)		const	{	return( m_pData[Position] );	}

	void				Add_Value		(char   Value, bool bBigEndian = false)	{	if( Inc_Size(sizeof(Value)) ) Set_Value(m_Size - sizeof(Value), Value, bBigEndian);	}
	void				Add_Value		(short  Value, bool bBigEndian = false)	{	if( Inc_Size(sizeof(Value)) ) Set_Value(m_Size - sizeof(Value), Value, bBigEndian);	}
	void				Add_Value		(int    Value, bool bBigEndian = false)	{	if( Inc_Size(sizeof(Value)) ) Set_Value(m_Size - sizeof(Value), Value, bBigEndian);	}
	void				Add_Value		(float  Value, bool bBigEndian = false)	{	if( Inc_Size(sizeof(Value)) ) Set_Value(m_Size - sizeof(Value), Value, bBigEndian);	}
	void				Add_Value		(double Value, bool bBigEndian = false)	{	if( Inc_Size(sizeof(Value)) ) Set_Value(m_Size - sizeof(Value), Value, bBigEndian);	}

	CSG_Buffer &		operator +=		(char   Value)				{	Add_Value(Value);	return( *this );	}
	CSG_Buffer &		operator +=		(short  Value)				{	Add_Value(Value);	return( *this );	}
	CSG_Buffer &		operator +=		(int    Value)				{	Add_Value(Value);	return( *this );	}
	CSG_Buffer &		operator +=		(float  Value)				{	Add_Value(Value);	return( *this );	}
	CSG_Buffer &		operator +=		(double Value)				{	Add_Value(Value);	return( *this );	}

	void				Set_Value		(int Offset, char   Value, bool bBigEndian = false)	{	m_pData[Offset]	= Value;	}
	void				Set_Value		(int Offset, short  Value, bool bBigEndian = false)	{	if( bBigEndian ) SG_Swap_Bytes(&Value, sizeof(Value)); *(short  *)(m_pData + Offset) = Value;	}
	void				Set_Value		(int Offset, int    Value, bool bBigEndian = false)	{	if( bBigEndian ) SG_Swap_Bytes(&Value, sizeof(Value)); *(int    *)(m_pData + Offset) = Value;	}
	void				Set_Value		(int Offset, float  Value, bool bBigEndian = false)	{	if( bBigEndian ) SG_Swap_Bytes(&Value, sizeof(Value)); *(float  *)(m_pData + Offset) = Value;	}
	void				Set_Value		(int Offset, double Value, bool bBigEndian = false)	{	if( bBigEndian ) SG_Swap_Bytes(&Value, sizeof(Value)); *(double *)(m_pData + Offset) = Value;	}

	short				asShort			(int Offset, bool bBigEndian = false) const	{	short  Value = *(short  *)(m_pData + Offset); if( bBigEndian ) SG_Swap_Bytes(&Value, sizeof(Value)); return( Value );	}
	int					asInt			(int Offset, bool bBigEndian = false) const	{	int    Value = *(int    *)(m_pData + Offset); if( bBigEndian ) SG_Swap_Bytes(&Value, sizeof(Value)); return( Value );	}
	float				asFloat			(int Offset, bool bBigEndian = false) const	{	float  Value = *(float  *)(m_pData + Offset); if( bBigEndian ) SG_Swap_Bytes(&Value, sizeof(Value)); return( Value );	}
	double				asDouble		(int Offset, bool bBigEndian = false) const	{	double Value = *(double *)(m_pData + Offset); if( bBigEndian ) SG_Swap_Bytes(&Value, sizeof(Value)); return( Value );	}


private:

	char				*m_pData;

	size_t				m_Size;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Bytes
{
public:
							CSG_Bytes		(void);
	bool					Create			(void);

							CSG_Bytes		(const CSG_Bytes &Bytes);
	bool					Create			(const CSG_Bytes &Bytes);

							CSG_Bytes		(const BYTE *Bytes, int nBytes);
	bool					Create			(const BYTE *Bytes, int nBytes);

							CSG_Bytes		(const SG_Char *Bytes);
	bool					Create			(const SG_Char *Bytes);

	virtual ~CSG_Bytes(void);

	bool					Destroy			(void);
	bool					Clear			(void);

	void					Rewind			(void)				{	m_Cursor	= 0;	}
	bool					is_EOF			(void)				{	return( m_Cursor >= m_nBytes );	}

	int						Get_Count		(void)		const	{	return( m_nBytes );	}
	BYTE *					Get_Bytes		(void)		const	{	return( m_Bytes );	}

	CSG_Bytes				Get_Bytes		(int i)		const	{	if( i < 0 || i >= m_nBytes ) i = 0; return( CSG_Bytes(m_Bytes + i, m_nBytes - i) );	}
	CSG_Bytes				operator +		(int i)		const	{	return( Get_Bytes(i) );	}

	BYTE					Get_Byte		(int i)		const	{	return( i >= 0 && i < m_nBytes ? m_Bytes[i]  : 0 );		}
	BYTE					operator []		(int i)		const	{	return( Get_Byte(i) );	}

	bool					Assign			(const CSG_Bytes &Bytes);
	CSG_Bytes &				operator =		(const CSG_Bytes &Bytes)	{	Assign(Bytes);	return( *this );	}

	bool					Add				(const CSG_Bytes &Bytes);
	CSG_Bytes &				operator +=		(const CSG_Bytes &Bytes)	{	Add(Bytes);		return( *this );	}

	bool					Add				(void *Bytes, int nBytes, bool bSwapBytes);

	bool					Add				(BYTE   Value)							{	return( Add(&Value, sizeof(Value), false) );	}
	CSG_Bytes &				operator +=		(BYTE   Value)							{	Add(Value);		return( *this );	}
	bool					Add				(char   Value)							{	return( Add(&Value, sizeof(Value), false) );	}
	CSG_Bytes &				operator +=		(char   Value)							{	Add(Value);		return( *this );	}
	bool					Add				(short  Value, bool bSwapBytes = false)	{	return( Add(&Value, sizeof(Value), bSwapBytes) );	}
	CSG_Bytes &				operator +=		(short  Value)							{	Add(Value);		return( *this );	}
	bool					Add				(WORD   Value, bool bSwapBytes = false)	{	return( Add(&Value, sizeof(Value), bSwapBytes) );	}
	CSG_Bytes &				operator +=		(WORD   Value)							{	Add(Value);		return( *this );	}
	bool					Add				(int    Value, bool bSwapBytes = false)	{	return( Add(&Value, sizeof(Value), bSwapBytes) );	}
	CSG_Bytes &				operator +=		(int    Value)							{	Add(Value);		return( *this );	}
	bool					Add				(DWORD  Value, bool bSwapBytes = false)	{	return( Add(&Value, sizeof(Value), bSwapBytes) );	}
	CSG_Bytes &				operator +=		(DWORD  Value)							{	Add(Value);		return( *this );	}
	bool					Add				(float  Value, bool bSwapBytes = false)	{	return( Add(&Value, sizeof(Value), bSwapBytes) );	}
	CSG_Bytes &				operator +=		(float  Value)							{	Add(Value);		return( *this );	}
	bool					Add				(double Value, bool bSwapBytes = false)	{	return( Add(&Value, sizeof(Value), bSwapBytes) );	}
	CSG_Bytes &				operator +=		(double Value)							{	Add(Value);		return( *this );	}

	BYTE					asByte			(int i)								const	{	return(       Get_Byte (i) );	}
	char					asChar			(int i)								const	{	return( (char)Get_Byte (i) );	}
	short					asShort			(int i, bool bSwapBytes = false)	const	{	short	v = *((short  *)(m_Bytes + i)); if( bSwapBytes ) SG_Swap_Bytes(&v, sizeof(v)); return( v );	}
	WORD					asWord			(int i, bool bSwapBytes = false)	const	{	WORD	v = *((WORD   *)(m_Bytes + i)); if( bSwapBytes ) SG_Swap_Bytes(&v, sizeof(v)); return( v );	}
	int						asInt			(int i, bool bSwapBytes = false)	const	{	int		v = *((int    *)(m_Bytes + i)); if( bSwapBytes ) SG_Swap_Bytes(&v, sizeof(v)); return( v );	}
	DWORD					asDWord			(int i, bool bSwapBytes = false)	const	{	DWORD	v = *((DWORD  *)(m_Bytes + i)); if( bSwapBytes ) SG_Swap_Bytes(&v, sizeof(v)); return( v );	}
	float					asFloat			(int i, bool bSwapBytes = false)	const	{	float	v = *((float  *)(m_Bytes + i)); if( bSwapBytes ) SG_Swap_Bytes(&v, sizeof(v)); return( v );	}
	double					asDouble		(int i, bool bSwapBytes = false)	const	{	double	v = *((double *)(m_Bytes + i)); if( bSwapBytes ) SG_Swap_Bytes(&v, sizeof(v)); return( v );	}

	BYTE					Read_Byte		(void)						{	BYTE	v = asByte  (m_Cursor);             m_Cursor += sizeof(v); return( v ); }
	char					Read_Char		(void)						{	char	v = asByte  (m_Cursor);             m_Cursor += sizeof(v); return( v ); }
	short					Read_Short		(bool bSwapBytes = false)	{	short	v = asShort (m_Cursor, bSwapBytes); m_Cursor += sizeof(v); return( v ); }
	WORD					Read_Word		(bool bSwapBytes = false)	{	WORD	v = asWord  (m_Cursor, bSwapBytes); m_Cursor += sizeof(v); return( v ); }
	int						Read_Int		(bool bSwapBytes = false)	{	int		v = asInt   (m_Cursor, bSwapBytes); m_Cursor += sizeof(v); return( v ); }
	DWORD					Read_DWord		(bool bSwapBytes = false)	{	DWORD	v = asDWord (m_Cursor, bSwapBytes); m_Cursor += sizeof(v); return( v ); }
	float					Read_Float		(bool bSwapBytes = false)	{	float	v = asFloat (m_Cursor, bSwapBytes); m_Cursor += sizeof(v); return( v ); }
	double					Read_Double		(bool bSwapBytes = false)	{	double	v = asDouble(m_Cursor, bSwapBytes); m_Cursor += sizeof(v); return( v ); }

	CSG_String				toHexString		(void)	const;
	bool					fromHexString	(const CSG_String &HexString);


private:

	int						m_nBytes, m_nBuffer, m_Cursor;

	BYTE					*m_Bytes;


	bool					_Inc_Array		(int nBytes);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Bytes_Array
{
public:
	CSG_Bytes_Array(void);
	virtual ~CSG_Bytes_Array(void);

	bool					Destroy			(void);

	int						Get_Count		(void)	const	{	return( m_nBytes );		}
	CSG_Bytes *				Get_Bytes		(int i)			{	return( i >= 0 && i < m_nBytes ? m_pBytes[i] : NULL );	}
	CSG_Bytes &				operator []		(int i)			{	return( *Get_Bytes(i) );	}

	CSG_Bytes *				Add				(void);


private:

	int						m_nBytes, m_nBuffer;

	CSG_Bytes				**m_pBytes;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Stack
{
public:
	CSG_Stack(size_t RecordSize) : m_Size(0), m_Buffer(0), m_RecordSize(RecordSize), m_Stack(NULL)	{}
	virtual ~CSG_Stack(void)						{	Destroy();				}

	size_t					Get_RecordSize	(void)	{	return( m_RecordSize );	}
	size_t					Get_Size		(void)	{	return( m_Size );		}
	void					Clear			(void)	{	m_Size	= 0;			}

	void					Destroy			(void)
	{
		if( m_Stack )
		{
			SG_Free(m_Stack);
		}

		m_Size		= 0;
		m_Buffer	= 0;
		m_Stack		= NULL;
	}


protected:

	void *					Get_Record_Push	(void)
	{
		if( m_Size < m_Buffer || _Grow() )
		{
			m_Size++;

			return( (void *)(((char *)m_Stack) + m_RecordSize * (m_Size - 1)) );
		}

		return( NULL );
	}

	void *					Get_Record_Pop	(void)
	{
		if( m_Size > 0 )
		{
			m_Size--;

			return( (void *)(((char *)m_Stack) + m_RecordSize * (m_Size)) );
		}

		return( NULL );
	}


private:

	size_t					m_Size, m_Buffer, m_RecordSize;

	void					*m_Stack;


	virtual bool			_Grow			(void)
	{
		void	*Stack	= SG_Realloc(m_Stack, (m_Buffer + 256) * m_RecordSize);

		if( Stack )
		{
			m_Stack		= Stack;
			m_Buffer	+= 256;

			return( true );
		}

		return( false );
	}

};


///////////////////////////////////////////////////////////
//														 //
//						Data Types						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Data_Type
{
	SG_DATATYPE_Bit			= 0,
	SG_DATATYPE_Byte,
	SG_DATATYPE_Char,
	SG_DATATYPE_Word,
	SG_DATATYPE_Short,
	SG_DATATYPE_DWord,
	SG_DATATYPE_Int,
	SG_DATATYPE_ULong,
	SG_DATATYPE_Long,
	SG_DATATYPE_Float,
	SG_DATATYPE_Double,
	SG_DATATYPE_String,
	SG_DATATYPE_Date,
	SG_DATATYPE_Color,
	SG_DATATYPE_Binary,
	SG_DATATYPE_Undefined
}
TSG_Data_Type;

//---------------------------------------------------------
const SG_Char	gSG_Data_Type_Identifier[][32]	=
{
	SG_T("BIT"),
	SG_T("BYTE_UNSIGNED"),
	SG_T("BYTE"),
	SG_T("SHORTINT_UNSIGNED"),
	SG_T("SHORTINT"),
	SG_T("INTEGER_UNSIGNED"),
	SG_T("INTEGER"),
	SG_T("LONGINT_UNSIGNED"),
	SG_T("LONGINT"),
	SG_T("FLOAT"),
	SG_T("DOUBLE"),
	SG_T("STRING"),
	SG_T("DATE"),
	SG_T("COLOR"),
	SG_T("BINARY"),
	SG_T("UNDEFINED")
};

//---------------------------------------------------------
inline size_t	SG_Data_Type_Get_Size	(TSG_Data_Type Type)
{
	switch( Type )
	{
	default:					return( 0 );
	case SG_DATATYPE_Bit:		return( 0 );
	case SG_DATATYPE_Byte:		return( sizeof(unsigned char) );
	case SG_DATATYPE_Char:		return( sizeof(char) );
	case SG_DATATYPE_Word:		return( sizeof(unsigned short int) );
	case SG_DATATYPE_Short:		return( sizeof(short int) );
	case SG_DATATYPE_DWord:		return( sizeof(unsigned int) );
	case SG_DATATYPE_Int:		return( sizeof(int) );
	case SG_DATATYPE_ULong:		return( sizeof(unsigned long) );
	case SG_DATATYPE_Long:		return( sizeof(long) );
	case SG_DATATYPE_Float:		return( sizeof(float) );
	case SG_DATATYPE_Double:	return( sizeof(double) );
	case SG_DATATYPE_String:	return( 0 );
	case SG_DATATYPE_Date:		return( 0 );
	case SG_DATATYPE_Color:		return( sizeof(unsigned int) );
	case SG_DATATYPE_Binary:	return( 0 );
	}
}

//---------------------------------------------------------
SAGA_API_DLL_EXPORT const SG_Char *	SG_Data_Type_Get_Name	(TSG_Data_Type Type);
SAGA_API_DLL_EXPORT bool			SG_Data_Type_is_Numeric	(TSG_Data_Type Type);
SAGA_API_DLL_EXPORT bool			SG_DataType_Range_Check	(TSG_Data_Type Type, double &Value);


///////////////////////////////////////////////////////////
//														 //
//						File							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _SAGA_UNICODE
	#define SG_FILE_OPEN		fopen
	#define SG_FILE_PRINTF		fprintf
	#define SG_FILE_SCANF		fscanf
	#define SG_FILE_GETC		fgetc
#else
	#ifndef _SAGA_LINUX
		#define SG_FILE_OPEN	_wfopen
	#else
		#define SG_FILE_OPEN	fopen
	#endif
	#define SG_FILE_PRINTF		fwprintf
	#define SG_FILE_SCANF		fwscanf
	#define SG_FILE_GETC		fgetwc
#endif

//---------------------------------------------------------
enum
{
	SG_FILE_R,
	SG_FILE_W,
	SG_FILE_RW,
	SG_FILE_WA,
	SG_FILE_RWA
};

//---------------------------------------------------------
enum
{
	SG_FILE_START,
	SG_FILE_CURRENT,
	SG_FILE_END
};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_File
{
public:

	CSG_File(void);
	CSG_File(const CSG_String &File_Name, int Mode = SG_FILE_R, bool bBinary = true, bool bUnicode = false);

	virtual ~CSG_File(void);

	bool							Attach				(FILE *Stream);
	bool							Detach				(void);
	FILE *							Get_Stream			(void)	const	{	return( m_pStream );	}
	bool							Get_UnicodeFlag		(void)	const	{	return( m_bUnicode );	}
	bool							Set_UnicodeFlag		(bool bOn);

	bool							Open				(const CSG_String &FileName, int Mode = SG_FILE_R, bool bBinary = true, bool bUnicode = false);
	bool							Close				(void);

	bool							is_Open				(void)	const	{	return( m_pStream != NULL );	}
	bool							is_EOF				(void)	const;

	int								Length				(void)	const;

	bool							Seek				(int Offset, int Origin = SG_FILE_START)	const;
	bool							Seek_Start			(void)	const;
	bool							Seek_End			(void)	const;

	int								Tell				(void)	const;

	bool							Flush				(void)	const;

	int								Printf				(const SG_Char *Format, ...);
	int								Scanf				(const SG_Char *Format, ...)	const;

	int								Get_Character		(void)	const;

	size_t							Read				(void       *Buffer, size_t Size, size_t Count = 1)	const;
	size_t							Write				(void       *Buffer, size_t Size, size_t Count = 1)	const;
	size_t							Read				(CSG_String &Buffer, size_t Size)	const;
	size_t							Write				(CSG_String &Buffer)				const;

	bool							Read_Line			(CSG_String &sLine)	const;

	int								Read_Int			(				bool bBigEndian = false)	const;
	bool							Write_Int			(int    Value,	bool bBigEndian = false);
	double							Read_Double			(				bool bBigEndian = false)	const;
	bool							Write_Double		(double Value,	bool bBigEndian = false);

	bool							Scan				(int        &Value)	const;
	bool							Scan				(double     &Value)	const;
	bool							Scan				(CSG_String &Value, SG_Char Separator)	const;

	int								Scan_Int			(void)				const;
	double							Scan_Double			(void)				const;
	CSG_String						Scan_String			(SG_Char Separator)	const;


private:

	bool							m_bUnicode;

	FILE							*m_pStream;

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool			SG_Dir_Exists			(const SG_Char *Directory);
SAGA_API_DLL_EXPORT bool			SG_Dir_Create			(const SG_Char *Directory);
SAGA_API_DLL_EXPORT CSG_String		SG_Dir_Get_Current		(void);

SAGA_API_DLL_EXPORT bool			SG_File_Exists			(const SG_Char *FileName);
SAGA_API_DLL_EXPORT bool			SG_File_Delete			(const SG_Char *FileName);
SAGA_API_DLL_EXPORT CSG_String		SG_File_Get_TmpName		(const SG_Char *Prefix, const SG_Char *Directory);
SAGA_API_DLL_EXPORT CSG_String		SG_File_Get_Name		(const SG_Char *full_Path, bool bExtension);
SAGA_API_DLL_EXPORT CSG_String		SG_File_Get_Path		(const SG_Char *full_Path);
SAGA_API_DLL_EXPORT CSG_String		SG_File_Make_Path		(const SG_Char *Directory, const SG_Char *Name, const SG_Char *Extension = NULL);
SAGA_API_DLL_EXPORT bool			SG_File_Cmp_Extension	(const SG_Char *File_Name, const SG_Char *Extension);
SAGA_API_DLL_EXPORT bool			SG_File_Set_Extension	(const SG_Char *File_Name, const SG_Char *Extension);
SAGA_API_DLL_EXPORT CSG_String		SG_File_Get_Extension	(const SG_Char *File_Name);

SAGA_API_DLL_EXPORT bool			SG_Read_Line			(FILE *Stream, CSG_String &Line);


///////////////////////////////////////////////////////////
//														 //
//						Colors							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_GET_RGB(r, g, b)		((DWORD) (((BYTE)(r) | ((WORD)(g) << 8)) | (((DWORD)(BYTE)(b)) << 16)))
#define SG_GET_RGBA(r, g, b, a)	((DWORD) (((BYTE)(r) | ((WORD)(g) << 8)) | (((DWORD)(BYTE)(b)) << 16) | (((DWORD)(BYTE)(a)) << 24)))

#define SG_GET_R(rgb)				((BYTE) ((rgb)      ))
#define SG_GET_G(rgb)				((BYTE) ((rgb) >>  8))
#define SG_GET_B(rgb)				((BYTE) ((rgb) >> 16))
#define SG_GET_A(rgb)				((BYTE) ((rgb) >> 24))

//---------------------------------------------------------
#define SG_COLOR_BLACK				SG_GET_RGB(  0,   0,   0)
#define SG_COLOR_GREY				SG_GET_RGB(128, 128, 128)
#define SG_COLOR_GREY_LIGHT		SG_GET_RGB(192, 192, 192)
#define SG_COLOR_WHITE				SG_GET_RGB(255, 255, 255)
#define SG_COLOR_RED				SG_GET_RGB(255,   0,   0)
#define SG_COLOR_RED_DARK			SG_GET_RGB(128,   0,   0)
#define SG_COLOR_YELLOW			SG_GET_RGB(255, 255,   0)
#define SG_COLOR_YELLOW_DARK		SG_GET_RGB(128, 128,   0)
#define SG_COLOR_GREEN				SG_GET_RGB(  0, 255,   0)
#define SG_COLOR_GREEN_DARK		SG_GET_RGB(  0, 128,   0)
#define SG_COLOR_GREEN_LIGHT		SG_GET_RGB(  0, 255,   0)
#define SG_COLOR_BLUE				SG_GET_RGB(  0,   0, 255)
#define SG_COLOR_BLUE_DARK			SG_GET_RGB(  0,   0, 128)
#define SG_COLOR_BLUE_LIGHT		SG_GET_RGB(  0, 255, 255)
#define SG_COLOR_BLUE_GREEN		SG_GET_RGB(  0, 128, 128)
#define SG_COLOR_PURPLE			SG_GET_RGB(128,   0, 128)
#define SG_COLOR_PINK				SG_GET_RGB(255,   0, 255)
#define SG_COLOR_NONE				-1
#define SG_COLOR_RANDOM			-2

//---------------------------------------------------------
enum ESG_Colors
{
	SG_COLORS_DEFAULT			= 0,
	SG_COLORS_DEFAULT_BRIGHT,
	SG_COLORS_BLACK_WHITE,
	SG_COLORS_BLACK_RED,
	SG_COLORS_BLACK_GREEN,
	SG_COLORS_BLACK_BLUE,
	SG_COLORS_WHITE_RED,
	SG_COLORS_WHITE_GREEN,
	SG_COLORS_WHITE_BLUE,
	SG_COLORS_YELLOW_RED,
	SG_COLORS_YELLOW_GREEN,
	SG_COLORS_YELLOW_BLUE,
	SG_COLORS_RED_GREEN,
	SG_COLORS_RED_BLUE,
	SG_COLORS_GREEN_BLUE,
	SG_COLORS_RED_GREY_BLUE,
	SG_COLORS_RED_GREY_GREEN,
	SG_COLORS_GREEN_GREY_BLUE,
	SG_COLORS_RED_GREEN_BLUE,
	SG_COLORS_RED_BLUE_GREEN,
	SG_COLORS_GREEN_RED_BLUE,
	SG_COLORS_RAINBOW,
	SG_COLORS_NEON,
	SG_COLORS_COUNT
};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Colors
{
public:
	CSG_Colors(void);
	CSG_Colors(const CSG_Colors &Colors);
	CSG_Colors(int nColors, int Palette = SG_COLORS_DEFAULT, bool bRevert = false);
	virtual ~CSG_Colors(void);

	void							Destroy				(void);

	bool							Set_Count			(int nColors);
	int								Get_Count			(void) const		{	return( m_nColors );	}

	CSG_Colors &					operator  =			(const CSG_Colors &Colors);
	long &							operator []			(int Index)			{	return( m_Colors[Index] );	}

	bool							Set_Color			(int Index, long Color);
	bool							Set_Color			(int Index, int Red, int Green, int Blue);
	bool							Set_Red				(int Index, int Value);
	bool							Set_Green			(int Index, int Value);
	bool							Set_Blue			(int Index, int Value);
	bool							Set_Brightness		(int Index, int Value);

	long							Get_Color			(int Index) const	{	return( Index >= 0 && Index < m_nColors ? m_Colors[Index] : 0 );	}
	long							Get_Red				(int Index) const	{	return( SG_GET_R(Get_Color(Index)) );	}
	long							Get_Green			(int Index) const	{	return( SG_GET_G(Get_Color(Index)) );	}
	long							Get_Blue			(int Index) const	{	return( SG_GET_B(Get_Color(Index)) );	}
	long							Get_Brightness		(int Index) const	{	return( (Get_Red(Index) + Get_Green(Index) + Get_Blue(Index)) / 3 );	}

	bool							Set_Default			(int nColors = 100);
	bool							Set_Palette			(int Index, bool bRevert = false, int nColors = 100);
	bool							Set_Ramp			(long Color_A, long Color_B);
	bool							Set_Ramp			(long Color_A, long Color_B, int iColor_A, int iColor_B);
	bool							Set_Ramp_Brighness	(int Brightness_A, int Brightness_B);
	bool							Set_Ramp_Brighness	(int Brightness_A, int Brightness_B, int iColor_A, int iColor_B);

	bool							Random				(void);
	bool							Invert				(void);
	bool							Revert				(void);

	bool							Assign				(const CSG_Colors &Colors);
	bool							Assign				(CSG_Colors *pSource);

	bool							Load				(const CSG_String &File_Name);
	bool							Save				(const CSG_String &File_Name, bool bBinary);

	bool							Serialize			(CSG_File &Stream, bool bSave, bool bBinary);


private:

	int								m_nColors;

	long							*m_Colors;


	void							_Set_Brightness		(double &a, double &b, double &c, int Pass = 0);

};


///////////////////////////////////////////////////////////
//														 //
//						Translator						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Translator
{
public:
	CSG_Translator(void);
	CSG_Translator(const CSG_String &File_Name, bool bSetExtension = true);

	virtual ~CSG_Translator(void);

	bool							Create					(const CSG_String &File_Name, bool bSetExtension = true);
	void							Destroy					(void);

	int								Get_Count				(void)	{	return( m_nTranslations );	}

	const SG_Char *					Get_Text				(int Index);
	const SG_Char *					Get_Translation			(int Index);

	const SG_Char *					Get_Translation			(const SG_Char *Text);


private:

	class CSG_Translation
	{
	public:
		CSG_Translation(const SG_Char *Text, const SG_Char *Translation)
		{
			m_Text					= Text;
			m_Translation			= Translation;
		}

		CSG_String					m_Text, m_Translation;
	};


private:

	int								m_nTranslations;

	CSG_Translation					**m_Translations;


	int								_Get_Index				(const SG_Char *Text);

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_Translator &	SG_Get_Translator	(void);

SAGA_API_DLL_EXPORT const SG_Char *		SG_Translate		(const SG_Char *Text);

#ifndef _SAGA_UNICODE
	#define LNG(s)	SG_Translate(s)
#else
	#define LNG(s)	SG_Translate(SG_T(s))
#endif


///////////////////////////////////////////////////////////
//														 //
//						UI Callback						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_UI_MSG_STYLE
{
	SG_UI_MSG_STYLE_NORMAL	= 0,
	SG_UI_MSG_STYLE_BOLD,
	SG_UI_MSG_STYLE_ITALIC,
	SG_UI_MSG_STYLE_SUCCESS,
	SG_UI_MSG_STYLE_FAILURE,
	SG_UI_MSG_STYLE_BIG,
	SG_UI_MSG_STYLE_SMALL,
	SG_UI_MSG_STYLE_01,
	SG_UI_MSG_STYLE_02,
	SG_UI_MSG_STYLE_03
}
TSG_UI_MSG_STYLE;

//---------------------------------------------------------
typedef enum ESG_UI_DataObject_Update
{
	SG_UI_DATAOBJECT_UPDATE_ONLY	= 0,
	SG_UI_DATAOBJECT_SHOW,
	SG_UI_DATAOBJECT_SHOW_NEW_MAP,
	SG_UI_DATAOBJECT_SHOW_LAST_MAP
}
TSG_UI_DataObject_Update;

//---------------------------------------------------------
typedef enum ESG_UI_Callback_ID
{
	CALLBACK_PROCESS_GET_OKAY,
	CALLBACK_PROCESS_SET_OKAY,
	CALLBACK_PROCESS_SET_PROGRESS,
	CALLBACK_PROCESS_SET_READY,
	CALLBACK_PROCESS_SET_TEXT,

	CALLBACK_STOP_EXECUTION,

	CALLBACK_DLG_MESSAGE,
	CALLBACK_DLG_CONTINUE,
	CALLBACK_DLG_ERROR,

	CALLBACK_DLG_PARAMETERS,

	CALLBACK_MESSAGE_ADD,
	CALLBACK_MESSAGE_ADD_ERROR,
	CALLBACK_MESSAGE_ADD_EXECUTION,

	CALLBACK_DATAOBJECT_FIND_BY_FILE,
	CALLBACK_DATAOBJECT_CHECK,
	CALLBACK_DATAOBJECT_ADD,
	CALLBACK_DATAOBJECT_UPDATE,
	CALLBACK_DATAOBJECT_SHOW,
	CALLBACK_DATAOBJECT_ASIMAGE,

	CALLBACK_DATAOBJECT_COLORS_GET,
	CALLBACK_DATAOBJECT_COLORS_SET,
	CALLBACK_DATAOBJECT_PARAMS_GET,
	CALLBACK_DATAOBJECT_PARAMS_SET,

	CALLBACK_DATAOBJECT_GET_ALL,

	CALLBACK_GUI_GET_WINDOW,
	CALLBACK_GET_APP_PATH
}
TSG_UI_Callback_ID;

//---------------------------------------------------------
typedef int (* TSG_PFNC_UI_Callback) (TSG_UI_Callback_ID ID, long pParam_1, long pParam_2);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool					SG_Set_UI_Callback			(TSG_PFNC_UI_Callback Function);
SAGA_API_DLL_EXPORT TSG_PFNC_UI_Callback	SG_Get_UI_Callback			(void);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT int						SG_UI_Progress_Lock			(bool bOn);
SAGA_API_DLL_EXPORT bool					SG_UI_Process_Get_Okay		(bool bBlink = false);
SAGA_API_DLL_EXPORT bool					SG_UI_Process_Set_Okay		(bool bOkay = true);
SAGA_API_DLL_EXPORT bool					SG_UI_Process_Set_Progress	(double Position, double Range);
SAGA_API_DLL_EXPORT bool					SG_UI_Process_Set_Ready		(void);
SAGA_API_DLL_EXPORT void					SG_UI_Process_Set_Text		(const SG_Char *Text);

SAGA_API_DLL_EXPORT bool					SG_UI_Stop_Execution		(void);

SAGA_API_DLL_EXPORT void					SG_UI_Dlg_Message			(const SG_Char *Message, const SG_Char *Caption);
SAGA_API_DLL_EXPORT bool					SG_UI_Dlg_Continue			(const SG_Char *Message, const SG_Char *Caption);
SAGA_API_DLL_EXPORT int						SG_UI_Dlg_Error				(const SG_Char *Message, const SG_Char *Caption);
SAGA_API_DLL_EXPORT bool					SG_UI_Dlg_Parameters		(class CSG_Parameters *pParameters, const SG_Char *Caption);

SAGA_API_DLL_EXPORT int						SG_UI_Msg_Lock				(bool bOn);
SAGA_API_DLL_EXPORT void					SG_UI_Msg_Add				(const SG_Char *Message, bool bNewLine, TSG_UI_MSG_STYLE Style = SG_UI_MSG_STYLE_NORMAL);
SAGA_API_DLL_EXPORT void					SG_UI_Msg_Add_Error			(const SG_Char *Message);
SAGA_API_DLL_EXPORT void					SG_UI_Msg_Add_Execution		(const SG_Char *Message, bool bNewLine, TSG_UI_MSG_STYLE Style = SG_UI_MSG_STYLE_NORMAL);

SAGA_API_DLL_EXPORT class CSG_Data_Object *	SG_UI_DataObject_Find		(const SG_Char *File_Name          , int Object_Type);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Check		(class CSG_Data_Object *pDataObject, int Object_Type);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Add		(class CSG_Data_Object *pDataObject, int Show);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Update		(class CSG_Data_Object *pDataObject, int Show, class CSG_Parameters *pParameters);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Show		(class CSG_Data_Object *pDataObject, int Show);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_asImage	(class CSG_Data_Object *pDataObject, class CSG_Grid *pGrid);

SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Colors_Get	(class CSG_Data_Object *pDataObject, class CSG_Colors *pColors);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Colors_Set	(class CSG_Data_Object *pDataObject, class CSG_Colors *pColors);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Params_Get	(class CSG_Data_Object *pDataObject, class CSG_Parameters *pParameters);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Params_Set	(class CSG_Data_Object *pDataObject, class CSG_Parameters *pParameters);

SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Get_All	(class CSG_Parameters *pParameters);

SAGA_API_DLL_EXPORT void *					SG_UI_Get_Window_Main		(void);
SAGA_API_DLL_EXPORT CSG_String				SG_UI_Get_Application_Path	(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__api_core_H
