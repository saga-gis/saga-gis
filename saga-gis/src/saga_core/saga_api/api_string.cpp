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
#include <math.h>

#include <wx/string.h>
#include <wx/datetime.h>

#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define WXCONV			wxConvUTF8


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String::CSG_String(void)
{
	m_pString	= new wxString;
#ifdef _SAGA_UNICODE
	m_bString	= NULL;
#endif
}

CSG_String::CSG_String(const CSG_String &String)
{
	m_pString	= new wxString(*String.m_pString);
#ifdef _SAGA_UNICODE
	m_bString	= NULL;
#endif
}

CSG_String::CSG_String(const SG_Char *String)
{
	m_pString	= new wxString(String);
#ifdef _SAGA_UNICODE
	m_bString	= NULL;
#endif
}

#ifdef _SAGA_UNICODE
CSG_String::CSG_String(const char *String)
{
	m_pString	= new wxString(WXCONV.cMB2WC(String));
	m_bString	= NULL;
}
#endif

CSG_String::CSG_String(SG_Char Character)
{
	m_pString	= new wxString(Character);
#ifdef _SAGA_UNICODE
	m_bString	= NULL;
#endif
}

//---------------------------------------------------------
CSG_String::~CSG_String(void)
{
	delete(m_pString);

#ifdef _SAGA_UNICODE
	if( m_bString )
	{
		SG_Free(m_bString);
	}
#endif
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_String::c_str(void) const
{
	return( m_pString->c_str() );
}

//---------------------------------------------------------
#ifdef _SAGA_UNICODE
const char * CSG_String::b_str(void)
{
	m_bString	= (char *)SG_Realloc(m_bString, (1 + strlen(m_pString->mb_str(WXCONV))) * sizeof(char));

	strcpy(m_bString, m_pString->mb_str(WXCONV));

	return( m_bString );
}
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_String::Length(void)	const
{
	return( m_pString->Length() );
}

//---------------------------------------------------------
void CSG_String::Clear(void)
{
	m_pString->Clear();
}

//---------------------------------------------------------
CSG_String CSG_String::Format(const SG_Char *Format, ...)
{
	va_list		argptr;
	CSG_String	s;

	va_start(argptr, Format);

	s.m_pString->PrintfV(Format, argptr);

	va_end(argptr);

	return( s );
}

//---------------------------------------------------------
int CSG_String::Printf(const SG_Char *Format, ...)
{
	va_list	argptr;

	va_start(argptr, Format);

	m_pString->PrintfV(Format, argptr);

	va_end(argptr);

	return( (int)Length() );
}

//---------------------------------------------------------
int CSG_String::Scanf(const SG_Char *Format, ...)
{
	va_list	argptr;

	va_start(argptr, Format);

	int	ret	= SG_Sscanf(m_pString->c_str(), Format, argptr);

	va_end(argptr);

	return( ret );
}

//---------------------------------------------------------
CSG_String & CSG_String::Append(const SG_Char *String)
{
	m_pString->Append(String);

	return( *this );
}

CSG_String & CSG_String::Append(SG_Char Character)
{
	m_pString->Append(Character);

	return( *this );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String & CSG_String::operator = (const CSG_String &String)
{
	*m_pString	= *String.m_pString;

	return( *this );
}

CSG_String & CSG_String::operator = (const SG_Char *String)
{
	*m_pString	= String;

	return( *this );
}

CSG_String & CSG_String::operator = (SG_Char Character)
{
	*m_pString	= Character;

	return( *this );
}

//---------------------------------------------------------
CSG_String CSG_String::operator + (const CSG_String &String) const
{
	CSG_String	s(*this);

	s	+= String;

	return( s );
}

CSG_String CSG_String::operator + (const SG_Char *String) const
{
	CSG_String	s(*this);

	s	+= String;

	return( s );
}

CSG_String CSG_String::operator + (SG_Char Character) const
{
	CSG_String	s(*this);

	s	+= Character;

	return( s );
}

CSG_String		operator + (const SG_Char *A, const CSG_String &B)
{
	CSG_String	s(A);

	s	+= B;

	return( s );
}

CSG_String		operator + (SG_Char A, const CSG_String &B)
{
	CSG_String	s(A);

	s	+= B;

	return( s );
}

//---------------------------------------------------------
void CSG_String::operator += (const CSG_String &String)
{
	*m_pString	+= *String.m_pString;
}

void CSG_String::operator += (const SG_Char *String)
{
	*m_pString	+= String;
}

void CSG_String::operator += (SG_Char Character)
{
	*m_pString	+= Character;
}

//---------------------------------------------------------
SG_Char & CSG_String::operator [] (int i)
{
	return( m_pString->GetWritableChar(i) );
}

SG_Char CSG_String::operator [] (int i) const
{
	return( m_pString->GetChar(i) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_String::Replace(const SG_Char *sOld, const SG_Char *sNew, bool replaceAll)
{
	return( m_pString->Replace(sOld, sNew, replaceAll) );
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_String::Trim(bool fromRight)
{
	size_t	n	= m_pString->Length();

	m_pString->Trim(fromRight);

	return( (int)(n - m_pString->Length()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_String::Find(SG_Char Character, bool fromEnd) const
{
	return( m_pString->Find(Character, fromEnd) );
}

//---------------------------------------------------------
int CSG_String::Find(const SG_Char *String) const
{
	return( m_pString->Find(String) );
}

//---------------------------------------------------------
bool CSG_String::Contains(const SG_Char *String) const
{
	return( m_pString->Contains(String) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_String::AfterFirst(SG_Char Character) const
{
	return( CSG_String(m_pString->AfterFirst(Character).c_str()) );
}

//---------------------------------------------------------
CSG_String CSG_String::AfterLast(SG_Char Character) const
{
	return( CSG_String(m_pString->AfterLast(Character).c_str()) );
}

//---------------------------------------------------------
CSG_String CSG_String::BeforeFirst(SG_Char Character) const
{
	return( CSG_String(m_pString->BeforeFirst(Character).c_str()) );
}

//---------------------------------------------------------
CSG_String CSG_String::BeforeLast(SG_Char Character) const
{
	return( CSG_String(m_pString->BeforeLast(Character).c_str()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_String::Right(size_t count) const
{
	return( CSG_String(m_pString->Right(count).c_str()) );
}

//---------------------------------------------------------
CSG_String CSG_String::Mid(size_t first, size_t count) const
{
	return( CSG_String(m_pString->Mid(first, count <= 0 ? wxSTRING_MAXLEN : count).c_str()) );
}

//---------------------------------------------------------
CSG_String CSG_String::Left(size_t count) const
{
	return( CSG_String(m_pString->Left(count).c_str()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_String::asInt(void) const
{
	int		Value	= 0;

	asInt(Value);

	return( Value );

//	return( asInt(Value) ? Value : 0 );
}

bool CSG_String::asInt(int &Value) const
{
	long	lValue	= 0;

	if( m_pString->ToLong(&lValue) || lValue != 0 )
	{
		Value	= (int)lValue;

		return( true );
	}

	return( false );
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
int				SG_Printf(const SG_Char *Format, ...)
{
	va_list	argptr;

	va_start(argptr, Format);

	int		ret	= wxVprintf(Format, argptr);

	va_end(argptr);

	return( ret );
}

//---------------------------------------------------------
int				SG_FPrintf(FILE* stream,const SG_Char *Format, ...)
{
	va_list	argptr;

	va_start(argptr, Format);

	int		ret	= wxVfprintf(stream, Format, argptr);

	va_end(argptr);

	return( ret );
}

//---------------------------------------------------------
int				SG_Sscanf(const SG_Char *Buffer, const SG_Char *Format, ...)
{
	va_list	argptr;

	va_start(argptr, Format);

	int		ret	= 0;	// wxVsscanf(Buffer, Format, argptr);

	va_end(argptr);

	return( ret );
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
		s.Append(t.FormatISODate());
		s.Append(SG_T("/"));
	}

	s.Append(t.FormatISOTime());

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String		SG_UTF8_To_String(const SG_Char *String)
{
#ifdef _SAGA_UNICODE
	return( !String && !String[0] ? SG_T("") : String );
#else
	return( !String && !String[0] ? SG_T("") : wxString::FromUTF8(String).c_str() );
#endif
}

//---------------------------------------------------------
CSG_String		SG_String_To_UTF8(const SG_Char *String)
{
#ifdef _SAGA_UNICODE
	return( !String && !String[0] ? SG_T("") : wxString(String, wxConvUTF8).c_str() );
#else
	return( !String && !String[0] ? SG_T("") : wxString(wxString(String).ToUTF8()).c_str() );
#endif
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

	String.Printf(SG_T("%c%03d\xb0%02d'%02f''"), c, d, h, s);

	return( String );
}

//---------------------------------------------------------
double			SG_Degree_To_Double(const SG_Char *String)
{
	double		d, h, s, sig;
	CSG_String	sVal(String);

	sig	= 1.0;
	d	= h	= s	= 0.0;

	if( sVal.BeforeFirst('\xb0').asDouble(d) )
	{
		if( d < 0.0 )
		{
			sig	= -1.0;
			d	= -d;
		}

		sVal.AfterFirst('\xb0' ).asDouble(h);
		sVal.AfterFirst('\'').asDouble(s);
	}
	else
	{
		sVal.asDouble(d);
	}

	return( sig * (d + h / 60.0 + s / (60.0 * 60.0)) );
}

//---------------------------------------------------------
CSG_String		SG_Number_To_Date(int Value)
{
	return( SG_Number_To_Date((double)Value) );
}

CSG_String		SG_Number_To_Date(double Value)
{
	int		y, m, d;

	y	= (int)(Value / 10000);	Value	-= y * 10000;
	m	= (int)(Value / 100);	Value	-= m * 100;
	d	= (int)(Value / 1);

	return( CSG_String::Format(SG_T("%02d.%02d.%04d"), d, m, y) );
}

//---------------------------------------------------------
int				SG_Date_To_Number(const SG_Char *String)
{
	if( String && String[0] )
	{
		CSG_String	s(String), sValue;

		sValue	= s.AfterLast	('.');
		int	y	= sValue.asInt();
		sValue	= s.BeforeLast	('.');	s		= sValue;

		sValue	= s.AfterLast	('.');
		int	m	= sValue.asInt();
		sValue	= s.BeforeLast	('.');	s		= sValue;
		int	d	= sValue.asInt();

		if( d < 1 )	d	= 1;	else if( d > 31 )	d	= 31;
		if( m < 1 )	m	= 1;	else if( m > 12 )	m	= 12;

		return( 10000 * y + 100 * m + 1 * d );
	}

	return( 0.0 );
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
	for(int i=0; i<(int)String.Length(); i++)
	{
		switch( String[i] )
		{
		case '.':	String[i]	= ',';	break;
		case ',':	String[i]	= '.';	break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String		SG_Get_String(double Value, int Precision, bool bScientific)
{
	CSG_String	s;

	if( Precision >= 0 )
	{
		s.Printf(SG_T("%.*f"), Precision, Value);
	}
	else if( Precision == -1 )
	{
		s.Printf(SG_T("%f"), Value);
	}
	else // if( Precision == -2 )
	{
		s.Printf(SG_T("%.*f"), SG_Get_Significant_Decimals(Value, abs(Precision)), Value);
	}

	s.Replace(SG_T(","), SG_T("."));

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
