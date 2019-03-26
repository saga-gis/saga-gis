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
//                    api_string.cpp                     //
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
#include <math.h>

#include <wx/string.h>
#include <wx/datetime.h>
#include <wx/tokenzr.h>
#include <wx/wxcrt.h>
#include <wx/wxcrtvararg.h>
#include <wx/convauto.h>

#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String::CSG_String(void)
{
	m_pString	= new wxString;
}

CSG_String::CSG_String(const CSG_String &String)
{
	m_pString	= new wxString(*String.m_pString);
}

CSG_String::CSG_String(const char *String)
{
	m_pString	= new wxString(String);
}

CSG_String::CSG_String(const wchar_t *String)
{
	m_pString	= new wxString(String);
}

CSG_String::CSG_String(char Character, size_t nRepeat)
{
	m_pString	= new wxString(Character, nRepeat);
}

CSG_String::CSG_String(wchar_t Character, size_t nRepeat)
{
	m_pString	= new wxString(Character, nRepeat);
}

//---------------------------------------------------------
CSG_String::CSG_String(const class wxString *pString)
{
	if( pString )
		m_pString	= new wxString(*pString);
	else
		m_pString	= new wxString;
}

bool CSG_String::Create(const class wxString *pString)
{
	if( pString )
		*m_pString	= *pString;
	else
		m_pString->Clear();

	return( true );
}

//---------------------------------------------------------
CSG_String::~CSG_String(void)
{
	delete(m_pString);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String & CSG_String::operator = (const CSG_String &String)
{
	*m_pString	= *String.m_pString;

	return( *this );
}

CSG_String & CSG_String::operator = (const char *String)
{
	*m_pString	= String;

	return( *this );
}

CSG_String & CSG_String::operator = (const wchar_t *String)
{
	*m_pString	= String;

	return( *this );
}

CSG_String & CSG_String::operator = (char Character)
{
	*m_pString	= Character;

	return( *this );
}

CSG_String & CSG_String::operator = (wchar_t Character)
{
	*m_pString	= Character;

	return( *this );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_String::Length(void)	const
{
	return( m_pString->Length() );
}

//---------------------------------------------------------
bool CSG_String::is_Empty(void)	const
{
	return( m_pString->IsEmpty() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SG_Char CSG_String::operator [] (int i) const
{
	if( i >= 0 && i < (int)Length() )
	{
		return( m_pString->GetChar(i) );
	}

	return( SG_T('\0') );
}

SG_Char CSG_String::operator [] (size_t i) const
{
	if( i < Length() )
	{
		return( m_pString->GetChar(i) );
	}

	return( SG_T('\0') );
}

SG_Char CSG_String::Get_Char(size_t i)	const
{
	if( i < Length() )
	{
		return( m_pString->GetChar(i) );
	}

	return( SG_T('\0') );
}

//---------------------------------------------------------
void CSG_String::Set_Char(size_t i, char Character)
{
	m_pString->SetChar(i, Character);
}

void CSG_String::Set_Char(size_t i, wchar_t Character)
{
	m_pString->SetChar(i, Character);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_String::c_str(void) const
{
	return( m_pString->c_str() );
}

//---------------------------------------------------------
const char * CSG_String::b_str(void) const
{
	return( *m_pString );
}

//---------------------------------------------------------
const wchar_t * CSG_String::w_str(void) const
{
	return( m_pString->wc_str() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_String::Clear(void)
{
	m_pString->Clear();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_String::Format(const char *Format, ...)
{
	CSG_String	s;

#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format);
	s.m_pString->PrintfV(_Format, argptr);
#else
	va_list	argptr; va_start(argptr, Format);
	s.m_pString->PrintfV(Format, argptr);
#endif

	va_end(argptr);

	return( s );
}

//---------------------------------------------------------
CSG_String CSG_String::Format(const wchar_t *Format, ...)
{
	CSG_String	s;

#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format);
	s.m_pString->PrintfV(_Format, argptr);
#else
	va_list	argptr; va_start(argptr, Format);
	s.m_pString->PrintfV(Format, argptr);
#endif

	va_end(argptr);

	return( s );
}

//---------------------------------------------------------
int CSG_String::Printf(const char *Format, ...)
{
#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format);
	int Result	= m_pString->PrintfV(_Format, argptr);
#else
	va_list	argptr; va_start(argptr, Format);
	int Result	= m_pString->PrintfV(Format, argptr);
#endif

	va_end(argptr);

	return( Result );
}

//---------------------------------------------------------
int CSG_String::Printf(const wchar_t *Format, ...)
{
#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format);
	int Result	= m_pString->PrintfV(_Format, argptr);
#else
	va_list	argptr; va_start(argptr, Format);
	int Result	= m_pString->PrintfV(Format, argptr);
#endif

	va_end(argptr);

	return( Result );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String & CSG_String::Prepend(const CSG_String &String)
{
	m_pString->Prepend(*String.m_pString);

	return( *this );
}

//---------------------------------------------------------
CSG_String & CSG_String::Append(const CSG_String &String)
{
	m_pString->Append(*String.m_pString);

	return( *this );
}

//---------------------------------------------------------
CSG_String & CSG_String::Append(const char *String)
{
	m_pString->Append(String);

	return( *this );
}

//---------------------------------------------------------
CSG_String & CSG_String::Append(const wchar_t *String)
{
	m_pString->Append(String);

	return( *this );
}

//---------------------------------------------------------
CSG_String & CSG_String::Append(char Character, size_t nRepeat)
{
	m_pString->Append(Character, nRepeat);

	return( *this );
}

//---------------------------------------------------------
CSG_String & CSG_String::Append(wchar_t Character, size_t nRepeat)
{
	m_pString->Append(Character, nRepeat);

	return( *this );
}

//---------------------------------------------------------
void CSG_String::operator += (const CSG_String &String)
{
	*m_pString	+= *String.m_pString;
}

void CSG_String::operator += (const char *String)
{
	*m_pString	+= String;
}

void CSG_String::operator += (const wchar_t *String)
{
	*m_pString	+= String;
}

void CSG_String::operator += (char Character)
{
	*m_pString	+= Character;
}

void CSG_String::operator += (wchar_t Character)
{
	*m_pString	+= Character;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_String::operator + (const CSG_String &String) const
{
	CSG_String	s(*this);

	s	+= String;

	return( s );
}

CSG_String CSG_String::operator + (const char *String) const
{
	CSG_String	s(*this);

	s	+= String;

	return( s );
}

CSG_String CSG_String::operator + (const wchar_t *String) const
{
	CSG_String	s(*this);

	s	+= String;

	return( s );
}

//---------------------------------------------------------
CSG_String CSG_String::operator + (char Character) const
{
	CSG_String	s(*this);

	s	+= Character;

	return( s );
}

CSG_String CSG_String::operator + (wchar_t Character) const
{
	CSG_String	s(*this);

	s	+= Character;

	return( s );
}

//---------------------------------------------------------
CSG_String		operator + (const char *A, const CSG_String &B)
{
	CSG_String	s(A);

	s	+= B;

	return( s );
}

CSG_String		operator + (const wchar_t *A, const CSG_String &B)
{
	CSG_String	s(A);

	s	+= B;

	return( s );
}

CSG_String		operator + (char A, const CSG_String &B)
{
	CSG_String	s(A);

	s	+= B;

	return( s );
}

CSG_String		operator + (wchar_t A, const CSG_String &B)
{
	CSG_String	s(A);

	s	+= B;

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_String::Cmp(const CSG_String &String)	const
{
	return( m_pString->Cmp(String.c_str()) );
}

//---------------------------------------------------------
int CSG_String::CmpNoCase(const CSG_String &String) const
{
	return( m_pString->CmpNoCase(String.c_str()) );
}

//---------------------------------------------------------
bool CSG_String::is_Same_As(const CSG_String &String, bool bCase) const
{
	return( m_pString->IsSameAs(*String.m_pString, bCase) );
}

bool CSG_String::is_Same_As(char Character, bool bCase) const
{
	return( m_pString->IsSameAs(Character, bCase) );
}

bool CSG_String::is_Same_As(wchar_t Character, bool bCase) const
{
	return( m_pString->IsSameAs(Character, bCase) );
}

//---------------------------------------------------------
CSG_String & CSG_String::Make_Lower(void)
{
	m_pString->MakeLower();

	return( *this );
}

//---------------------------------------------------------
CSG_String & CSG_String::Make_Upper(void)
{
	m_pString->MakeUpper();

	return( *this );
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_String::Replace(const CSG_String &sOld, const CSG_String &sNew, bool bReplaceAll)
{
	return( m_pString->Replace(*sOld.m_pString, *sNew.m_pString, bReplaceAll) );
}

//---------------------------------------------------------
CSG_String & CSG_String::Remove(size_t pos)
{
	m_pString->Remove(pos);

	return( *this );
}

//---------------------------------------------------------
CSG_String & CSG_String::Remove(size_t pos, size_t len)
{
	m_pString->Remove(pos, len);

	return( *this );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_String::Trim(bool fromRight)
{
	size_t	n	= m_pString->Length();

	m_pString->Trim(fromRight);

	return( (int)(n - m_pString->Length()) );
}

//---------------------------------------------------------
int CSG_String::Trim_Both(void)
{
	size_t	n	= m_pString->Length();

	m_pString->Trim( true);
	m_pString->Trim(false);

	return( (int)(n - m_pString->Length()) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_String::Find(char Character, bool fromEnd) const
{
	return( m_pString->Find(Character, fromEnd) );
}

int CSG_String::Find(wchar_t Character, bool fromEnd) const
{
	return( m_pString->Find(Character, fromEnd) );
}

//---------------------------------------------------------
int CSG_String::Find(const CSG_String &String) const
{
	return( m_pString->Find(*String.m_pString) );
}

//---------------------------------------------------------
bool CSG_String::Contains(const CSG_String &String) const
{
	return( m_pString->Contains(*String.m_pString) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_String::AfterFirst(char Character) const
{
	return( CSG_String(m_pString->AfterFirst(Character).c_str().AsWChar()) );
}

CSG_String CSG_String::AfterFirst(wchar_t Character) const
{
	return( CSG_String(m_pString->AfterFirst(Character).c_str().AsWChar()) );
}

//---------------------------------------------------------
CSG_String CSG_String::AfterLast(char Character) const
{
	return( CSG_String(m_pString->AfterLast(Character).c_str().AsWChar()) );
}

CSG_String CSG_String::AfterLast(wchar_t Character) const
{
	return( CSG_String(m_pString->AfterLast(Character).c_str().AsWChar()) );
}

//---------------------------------------------------------
CSG_String CSG_String::BeforeFirst(char Character) const
{
	return( CSG_String(m_pString->BeforeFirst(Character).c_str().AsWChar()) );
}

CSG_String CSG_String::BeforeFirst(wchar_t Character) const
{
	return( CSG_String(m_pString->BeforeFirst(Character).c_str().AsWChar()) );
}

//---------------------------------------------------------
CSG_String CSG_String::BeforeLast(char Character) const
{
	return( CSG_String(m_pString->BeforeLast(Character).c_str().AsWChar()) );
}

CSG_String CSG_String::BeforeLast(wchar_t Character) const
{
	return( CSG_String(m_pString->BeforeLast(Character).c_str().AsWChar()) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_String::Right(size_t count) const
{
	return( CSG_String(m_pString->Right(count).c_str().AsWChar()) );
}

//---------------------------------------------------------
CSG_String CSG_String::Mid(size_t first, size_t count) const
{
	return( CSG_String(m_pString->Mid(first, count <= 0 ? Length() : count).c_str().AsWChar()) );
}

//---------------------------------------------------------
CSG_String CSG_String::Left(size_t count) const
{
	return( CSG_String(m_pString->Left(count).c_str().AsWChar()) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_String::is_Number(void) const
{
	return( m_pString->IsNumber() );
}

//---------------------------------------------------------
int CSG_String::asInt(void) const
{
	int		Value	= 0;

	asInt(Value);

	return( Value );
}

bool CSG_String::asInt(int &Value) const
{
	const wxChar	*start = m_pString->c_str();
	wxChar			*end;

	Value	= wxStrtol(start, &end, 10);

	return( end > start );
}

//---------------------------------------------------------
double CSG_String::asDouble(void) const
{
	double	Value	= 0.0;

	asDouble(Value);

	return( Value );
}

bool CSG_String::asDouble(double &Value) const
{
	const wxChar	*start = m_pString->c_str();
	wxChar			*end;

	Value	= wxStrtod(start, &end);

	return( end > start );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_String::from_UTF8(const char *String, size_t Length)
{
	CSG_String	s;

	if( String )
	{
		if( !Length )
		{
			Length	= strlen(String);
		}

		*s.m_pString	= wxString::FromUTF8(String, Length);
	}

	return( s );
}

//---------------------------------------------------------
/**
* This function creates a single char array and fills it
* with the UTF-8 representation of this string object.
* If successful pString will point to the address of the
* allocated memory. Freeing memory is in response of the
* calling function.
*/
//---------------------------------------------------------
size_t CSG_String::to_UTF8(char **pString) const
{
	if( !is_Empty() )
	{
		const wxScopedCharBuffer	Buffer	= m_pString->utf8_str();

		if( (*pString = (char *)SG_Malloc(Buffer.length())) != NULL )
		{
			memcpy(*pString, Buffer.data(), Buffer.length());

			return( Buffer.length() );
		}
	}

	*pString	= NULL;

	return( 0 );
}

//---------------------------------------------------------
CSG_Buffer CSG_String::to_UTF8(void) const
{
	CSG_Buffer	String;

	if( !is_Empty() )
	{
		const wxScopedCharBuffer	Buffer	= m_pString->utf8_str();

		String.Set_Data(Buffer.data(), Buffer.length());
	}

	return( String );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* This function creates a single char array and fills it
* with the multibyte representation of this string object.
* If successful pString will point to the address of the
* allocated memory. Freeing memory is in response of the
* calling function.
*/
//---------------------------------------------------------
size_t CSG_String::to_MBChar(char **pString, int Encoding) const
{
	CSG_Buffer	String(to_MBChar(Encoding));

	if( String.Get_Size() && (*pString = (char *)SG_Malloc(String.Get_Size())) != NULL )
	{
		memcpy(*pString, String.Get_Data(), String.Get_Size());

		return( String.Get_Size() );
	}

	*pString	= NULL;

	return( 0 );
}

//---------------------------------------------------------
CSG_Buffer CSG_String::to_MBChar(int Encoding) const
{
	CSG_Buffer	String;

	if( !is_Empty() )
	{
		wxScopedCharBuffer	Buffer;

		switch( Encoding )	// selecting the appropriate wxMBConv class
		{
		case SG_FILE_ENCODING_ANSI   : Buffer = m_pString->mb_str(wxConvLibc       ); break;
		case SG_FILE_ENCODING_UTF7   : Buffer = m_pString->mb_str(wxConvUTF7       ); break;
		case SG_FILE_ENCODING_UTF8   : Buffer = m_pString->mb_str(wxConvUTF8       ); break;
		case SG_FILE_ENCODING_UTF16LE: Buffer = m_pString->mb_str(wxMBConvUTF16LE()); break;
		case SG_FILE_ENCODING_UTF16BE: Buffer = m_pString->mb_str(wxMBConvUTF16BE()); break;
		case SG_FILE_ENCODING_UTF32LE: Buffer = m_pString->mb_str(wxMBConvUTF32LE()); break;
		case SG_FILE_ENCODING_UTF32BE: Buffer = m_pString->mb_str(wxMBConvUTF32BE()); break;
		default                      : Buffer = m_pString->mb_str(wxConvAuto     ()); break;
		}

		String.Set_Data(Buffer.data(), Buffer.length());
	}

	return( String );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * This function creates a single char array and fills it
  * with the ASCII representation of this string object.
  * If successful pString will point to the address of the
  * allocated memory. Freeing memory is in response of the
  * calling function.
*/
//---------------------------------------------------------
bool CSG_String::to_ASCII(char **pString, char Replace)	const
{
	if( !is_Empty() )
	{
		#if wxVERSION_NUMBER < 3100
			const wxScopedCharBuffer	Buffer	= m_pString->ToAscii();
		#else
			const wxScopedCharBuffer	Buffer	= m_pString->ToAscii(Replace);
		#endif

		if( (*pString = (char *)SG_Malloc(Buffer.length())) != NULL )
		{
			memcpy(*pString, Buffer.data(), Buffer.length());

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
CSG_Buffer CSG_String::to_ASCII(char Replace) const
{
	CSG_Buffer	String;

	if( !is_Empty() )
	{
		#if wxVERSION_NUMBER < 3100
			const wxScopedCharBuffer	Buffer	= m_pString->ToAscii();
		#else
			const wxScopedCharBuffer	Buffer	= m_pString->ToAscii(Replace);
		#endif

		String.Set_Data(Buffer.data(), Buffer.length());
	}

	return( String );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Strings::CSG_Strings(void)
{
	m_nStrings	= 0;
	m_Strings	= NULL;
}

//---------------------------------------------------------
CSG_Strings::CSG_Strings(const CSG_Strings &Strings)
{
	m_nStrings	= 0;
	m_Strings	= NULL;

	Assign(Strings);
}

//---------------------------------------------------------
CSG_Strings::CSG_Strings(int nStrings, const SG_Char **Strings)
{
	m_nStrings	= 0;
	m_Strings	= NULL;

	for(int i=0; i<nStrings; i++)
	{
		Add(Strings[i]);
	}
}

//---------------------------------------------------------
CSG_Strings::~CSG_Strings(void)
{
	Clear();
}

//---------------------------------------------------------
void CSG_Strings::Clear(void)
{
	if( m_Strings )
	{
		for(int i=0; i<m_nStrings; i++)
		{
			delete(m_Strings[i]);
		}

		SG_Free(m_Strings);

		m_nStrings	= 0;
		m_Strings	= NULL;
	}
}

//---------------------------------------------------------
bool CSG_Strings::Assign(const CSG_Strings &Strings)
{
	Clear();

	for(int i=0; i<Strings.m_nStrings; i++)
	{
		Add(Strings[i]);
	}

	return( true );
}

//---------------------------------------------------------
CSG_Strings & CSG_Strings::operator  = (const CSG_Strings &Strings)
{
	Assign(Strings);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Strings::Add(const CSG_String &String)
{
	m_Strings	= (CSG_String **)SG_Realloc(m_Strings, (m_nStrings + 1) * sizeof(CSG_String *));
	m_Strings[m_nStrings++]	= new CSG_String(String);

	return( true );
}

//---------------------------------------------------------
bool CSG_Strings::Del(int Index)
{
	if( Index >= 0 && Index < m_nStrings )
	{
		delete(m_Strings[Index]);

		m_nStrings--;

		for(int i=Index; i<m_nStrings; i++)
		{
			m_Strings[i]	= m_Strings[i + 1];
		}

		m_Strings	= (CSG_String **)SG_Realloc(m_Strings, m_nStrings * sizeof(CSG_String *));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Strings & CSG_Strings::operator  += (const CSG_String &String)
{
	Add(String);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Strings::Set_Count(int nStrings)
{
	Clear();

	for(int i=0; i<nStrings; i++)
	{
		Add(SG_T(""));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool SG_is_Character_Numeric(int Character)
{
	switch( Character )
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
	case '+':
	case '.':
	case ',':
	case 'e':
	case 'E':
		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int				SG_Printf(const CSG_String &Format)
{
	return( SG_Printf(Format.c_str()) );
}

int				SG_Printf(const char *Format, ...)
{
#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format); int ret = wxVprintf(_Format, argptr); va_end(argptr); return( ret );
#else
	va_list	argptr; va_start(argptr,  Format); int ret = wxVprintf( Format, argptr); va_end(argptr); return( ret );
#endif
}

int				SG_Printf(const wchar_t *Format, ...)
{
#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format); int ret = wxVprintf(_Format, argptr); va_end(argptr); return( ret );
#else
	va_list	argptr; va_start(argptr,  Format); int ret = wxVprintf( Format, argptr); va_end(argptr); return( ret );
#endif
}

//---------------------------------------------------------
int				SG_FPrintf(FILE *Stream, const CSG_String &String)
{
	return( SG_FPrintf(Stream, String.c_str()) );
}

int				SG_FPrintf(FILE *Stream, const char *Format, ...)
{
#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format); int ret = wxVfprintf(Stream, _Format, argptr); va_end(argptr); return( ret );
#else
	va_list	argptr; va_start(argptr,  Format); int ret = wxVfprintf(Stream,  Format, argptr); va_end(argptr); return( ret );
#endif
}

int				SG_FPrintf(FILE *Stream, const wchar_t *Format, ...)
{
#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_list	argptr; va_start(argptr, _Format); int ret = wxVfprintf(Stream, _Format, argptr); va_end(argptr); return( ret );
#else
	va_list	argptr; va_start(argptr,  Format); int ret = wxVfprintf(Stream,  Format, argptr); va_end(argptr); return( ret );
#endif
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String		SG_Get_CurrentTimeStr(bool bWithDate)
{
	CSG_String	s;
	wxDateTime	t;

	t.SetToCurrent();

	if( bWithDate )
	{
		s.Append(t.FormatISODate().wc_str());
		s.Append(SG_T("/"));
	}

	s.Append(t.FormatISOTime().wc_str());

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String		SG_Double_To_Degree(double Value)
{
	SG_Char		c;
	int			d, h;
	double		s;
	CSG_String	String;

	if( Value < 0.0 )
	{
		Value	= -Value;
		c		= SG_T('-');
	}
	else
	{
		c		= SG_T('+');
	}

	Value	= fmod(Value, 360.0);
	d		= (int)Value;
	Value	= 60.0 * (Value - d);
	h		= (int)Value;
	Value	= 60.0 * (Value - h);
	s		= Value;

	String.Printf(SG_T("%c%03d\xb0%02d'%02.*f''"), c, d, h, SG_Get_Significant_Decimals(s), s);

	return( String );
}

//---------------------------------------------------------
double			SG_Degree_To_Double(const CSG_String &String)
{
	double		d, h, s, sig;

	sig	= 1.0;
	d	= h	= s	= 0.0;

	if( String.BeforeFirst('\xb0').asDouble(d) )
	{
		if( d < 0.0 )
		{
			sig	= -1.0;
			d	= -d;
		}

		String.AfterFirst('\xb0').asDouble(h);
		String.AfterFirst('\''  ).asDouble(s);
	}
	else
	{
		String.asDouble(d);
	}

	return( sig * (d + h / 60.0 + s / (60.0 * 60.0)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int				SG_Get_Significant_Decimals(double Value, int maxDecimals)
{
	int		Decimals;
	double	Reminder;

	Value	= fabs(Value);

	for(Decimals=0; Decimals<maxDecimals; Decimals++)
	{
		Reminder	= Value - floor(Value);

		if( Reminder == 0.0 )
		{
			return( Decimals );
		}

		Value	= 10.0 * Value;
	}

	return( maxDecimals );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void			SG_Flip_Decimal_Separators(CSG_String &String)
{
	for(size_t i=0; i<String.Length(); i++)
	{
		switch( String[i] )
		{
		case SG_T('.'):	String.Set_Char(i, ',');	break;
		case SG_T(','):	String.Set_Char(i, '.');	break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Returns floating point number 'Value' as formatted string.
  * The format depends on the 'Precision' value. 
  * If Precision is -99 (the default) it will simply use %f format.
  * If 'Precision' is -98 the string will always be formatted
  * with the %e format specification (i.e. scientific: d.dddd e dd).
  * If Precision is zero (= 0) no decimals will be printed
  * If Precision is positive (> 0) it specifies the fix number
  * of decimals ("%.*f", Precision, Value),
  * if negative only significant decimals will be plotted up to a
  * maximum of the absolute value of 'Precision' digits.
*/
//---------------------------------------------------------
CSG_String		SG_Get_String(double Value, int Precision)
{
	CSG_String	s;

	if     ( Precision == -99 )
	{
		s.Printf("%f", Value);
	}
	else if( Precision == -98 )
	{
		s.Printf("%e", Value);
	}
	else if( Precision ==   0 )
	{
		s.Printf("%d", (int)Value);
	}
	else if( Precision  >   0 )
	{
		s.Printf("%.*f", Precision, Value);
	}
	else // if( Precision < 0 )
	{
		Precision	= SG_Get_Significant_Decimals(Value, abs(Precision));

		if( Precision == 0 )
		{
			s.Printf("%.0f", Value);
		}
		else // if( Precision > 0 )
		{
			s.Printf("%.*f", Precision, Value);

			while( s.Length() > 1 && s[s.Length() - 1] == '0' )
			{
				s	= s.Left(s.Length() - 1);
			}

			if( s.Length() > 1 && (s[s.Length() - 1] == '.' || s[s.Length() - 1] == ',') )
			{
				s	= s.Left(s.Length() - 1);
			}
		}
	}

	s.Replace(",", ".");

	return( s );
}

//---------------------------------------------------------
CSG_String		SG_Get_String(int Value, int Precision)
{
	if( Precision > 0 )
	{
		return( SG_Get_String((double)Value, Precision) );
	}

	if( Precision < 0 )
	{
		return( CSG_String::Format("%0*d", Precision, Value) );
	}

	return( CSG_String::Format("%d", Value) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String_Tokenizer::CSG_String_Tokenizer(void)
{
	m_pTokenizer	= new wxStringTokenizer();
}

//---------------------------------------------------------
CSG_String_Tokenizer::CSG_String_Tokenizer(const CSG_String &String, const CSG_String &Delimiters, TSG_String_Tokenizer_Mode Mode)
{
	m_pTokenizer	= new wxStringTokenizer();

	Set_String(String, Delimiters, Mode);
}

//---------------------------------------------------------
CSG_String_Tokenizer::~CSG_String_Tokenizer(void)
{
	delete(m_pTokenizer);
}

//---------------------------------------------------------
size_t CSG_String_Tokenizer::Get_Tokens_Count(void)	const
{
	return( m_pTokenizer->CountTokens() );
}

//---------------------------------------------------------
SG_Char CSG_String_Tokenizer::Get_Last_Delimiter(void)	const
{
	return( m_pTokenizer->GetLastDelimiter() );
}

//---------------------------------------------------------
CSG_String CSG_String_Tokenizer::Get_Next_Token(void)
{
	wxString	s(m_pTokenizer->GetNextToken());

	return( &s );
}

//---------------------------------------------------------
size_t CSG_String_Tokenizer::Get_Position(void)	const
{
	return( m_pTokenizer->GetPosition() );
}

//---------------------------------------------------------
CSG_String CSG_String_Tokenizer::Get_String(void)	const
{
	wxString	s(m_pTokenizer->GetString());

	return( &s );
}

//---------------------------------------------------------
bool CSG_String_Tokenizer::Has_More_Tokens(void)	const
{
	return( m_pTokenizer->HasMoreTokens() );
}

//---------------------------------------------------------
void CSG_String_Tokenizer::Set_String(const CSG_String &String, const CSG_String &Delimiters, TSG_String_Tokenizer_Mode Mode)
{
	wxStringTokenizerMode	_Mode;

	switch( Mode )
	{
	default:                     _Mode	= wxTOKEN_DEFAULT      ; break;
	case SG_TOKEN_INVALID:       _Mode	= wxTOKEN_INVALID      ; break;
	case SG_TOKEN_RET_EMPTY:     _Mode	= wxTOKEN_RET_EMPTY    ; break;
	case SG_TOKEN_RET_EMPTY_ALL: _Mode	= wxTOKEN_RET_EMPTY_ALL; break;
	case SG_TOKEN_RET_DELIMS:    _Mode	= wxTOKEN_RET_DELIMS   ; break;
	case SG_TOKEN_STRTOK:        _Mode	= wxTOKEN_STRTOK       ; break;
	}

	m_pTokenizer->SetString(String.c_str(), Delimiters.c_str(), _Mode);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Strings SG_String_Tokenize(const CSG_String &String, const CSG_String &Delimiters, TSG_String_Tokenizer_Mode Mode)
{
	CSG_Strings	Strings;

	CSG_String_Tokenizer	Tokenizer(String, Delimiters, Mode);

	while( Tokenizer.Has_More_Tokens() )
	{
		Strings	+= Tokenizer.Get_Next_Token();
	}

	return( Strings );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
