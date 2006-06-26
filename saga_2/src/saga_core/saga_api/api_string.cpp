
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
CAPI_String::CAPI_String(void)
{
	m_pString	= new wxString;
}

CAPI_String::CAPI_String(const CAPI_String &String)
{
	m_pString	= new wxString(*String.m_pString);
}

CAPI_String::CAPI_String(const char *String)
{
	m_pString	= new wxString(String);
}

CAPI_String::CAPI_String(char Character)
{
	m_pString	= new wxString(Character);
}

//---------------------------------------------------------
CAPI_String::~CAPI_String(void)
{
	delete(m_pString);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char * CAPI_String::c_str(void)
{
	return( m_pString->c_str() );
}

//---------------------------------------------------------
size_t CAPI_String::Length(void)
{
	return( m_pString->Length() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CAPI_String::Clear(void)
{
	m_pString->Clear();
}

//---------------------------------------------------------
int CAPI_String::Printf(const char *Format, ...)
{
	va_list	argptr;

	va_start(argptr, Format);

	m_pString->PrintfV(Format, argptr);

	va_end(argptr);

	return( Length() );
}

//---------------------------------------------------------
CAPI_String CAPI_String::Format(const char *Format, ...)
{
	va_list		argptr;
	CAPI_String	s;

	va_start(argptr, Format);

	s.m_pString->PrintfV(Format, argptr);

	va_end(argptr);

	return( s );
}

//---------------------------------------------------------
CAPI_String & CAPI_String::Append(const char *String)
{
	m_pString->Append(String);

	return( *this );
}

CAPI_String & CAPI_String::Append(char Character)
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
CAPI_String & CAPI_String::operator = (const CAPI_String &String)
{
	*m_pString	= *String.m_pString;

	return( *this );
}

CAPI_String & CAPI_String::operator = (const char *String)
{
	*m_pString	= String;

	return( *this );
}

CAPI_String & CAPI_String::operator = (char Character)
{
	*m_pString	= Character;

	return( *this );
}

//---------------------------------------------------------
void CAPI_String::operator += (const CAPI_String &String)
{
	*m_pString	+= *String.m_pString;
}

void CAPI_String::operator += (const char *String)
{
	*m_pString	+= String;
}

void CAPI_String::operator += (char Character)
{
	*m_pString	+= Character;
}

//---------------------------------------------------------
char & CAPI_String::operator [] (int i)
{
	return( m_pString->GetWritableChar(i) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CAPI_String::Cmp(const char *String)
{
	return( m_pString->Cmp(String) );
}

//---------------------------------------------------------
int CAPI_String::CmpNoCase(const char *String)
{
	return( m_pString->CmpNoCase(String) );
}

//---------------------------------------------------------
CAPI_String & CAPI_String::Make_Lower(void)
{
	m_pString->MakeLower();

	return( *this );
}

//---------------------------------------------------------
CAPI_String & CAPI_String::Make_Upper(void)
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
size_t CAPI_String::Replace(const char *sOld, const char *sNew, bool replaceAll)
{
	return( m_pString->Replace(sOld, sNew, replaceAll) );
}

//---------------------------------------------------------
CAPI_String & CAPI_String::Remove(size_t pos)
{
	m_pString->Remove(pos);

	return( *this );
}

//---------------------------------------------------------
CAPI_String & CAPI_String::Remove(size_t pos, size_t len)
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
int CAPI_String::Remove_WhiteChars(bool fromEnd)
{
	int		n, c;

	if( fromEnd )
	{
		for(n=Length()-1; n>=0; n--)
		{
			c	= m_pString->GetChar(n);

			if( c == ' ' || c == '\t' )
			{
				break;
			}
		}

		if( n < (int)Length() - 1 )
		{
			Remove(n);
		}

		n	= Length() - n;
	}
	else
	{
		for(n=0; n<(int)Length(); n++)
		{
			c	= m_pString->GetChar(n);

			if( c == ' ' || c == '\t' )
			{
				break;
			}
		}

		if( n > 0 )
		{
			Remove(0, n);
		}
	}

	return( n );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CAPI_String::Find(char Character, bool fromEnd)
{
	return( m_pString->Find(Character, fromEnd) );
}

//---------------------------------------------------------
int CAPI_String::Find(const char *String)
{
	return( m_pString->Find(String) );
}

//---------------------------------------------------------
bool CAPI_String::Contains(const char *String)
{
	return( m_pString->Contains(String) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_String CAPI_String::AfterFirst(char Character) const
{
	return( CAPI_String(m_pString->AfterFirst(Character).c_str()) );
}

//---------------------------------------------------------
CAPI_String CAPI_String::AfterLast(char Character) const
{
	return( CAPI_String(m_pString->AfterLast(Character).c_str()) );
}

//---------------------------------------------------------
CAPI_String CAPI_String::BeforeFirst(char Character) const
{
	return( CAPI_String(m_pString->BeforeFirst(Character).c_str()) );
}

//---------------------------------------------------------
CAPI_String CAPI_String::BeforeLast(char Character) const
{
	return( CAPI_String(m_pString->BeforeLast(Character).c_str()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_String CAPI_String::Right(size_t count) const
{
	return( CAPI_String(m_pString->Right(count).c_str()) );
}

//---------------------------------------------------------
CAPI_String CAPI_String::Mid(size_t first, size_t count) const
{
	return( CAPI_String(m_pString->Mid(first, count).c_str()) );
}

//---------------------------------------------------------
CAPI_String CAPI_String::Left(size_t count) const
{
	return( CAPI_String(m_pString->Left(count).c_str()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CAPI_String::asInt(void)
{
	int		Value	= 0;

	asInt(Value);

	return( Value );

//	return( asInt(Value) ? Value : 0 );
}

bool CAPI_String::asInt(int &Value)
{
	long	lValue;

	if( m_pString->ToLong(&lValue) )
	{
		Value	= (int)lValue;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
double CAPI_String::asDouble(void)
{
	double	Value	= 0.0;

	asDouble(Value);

	return( Value );

//	return( asDouble(Value) ? Value : 0.0 );
}

bool CAPI_String::asDouble(double &Value)
{
	return( m_pString->ToDouble(&Value) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_String		operator + (const CAPI_String &x, const CAPI_String &y)
{
	CAPI_String	s(x), sy(y);

	s	+= sy;

	return( s );
}

CAPI_String		operator + (const CAPI_String &x, const char *y)
{
	CAPI_String	s(x);

	s	+= y;

	return( s );
}

CAPI_String		operator + (const CAPI_String &x, char y)
{
	CAPI_String	s(x);

	s	+= y;

	return( s );
}

CAPI_String		operator + (const char *x, const CAPI_String &y)
{
	CAPI_String	s(x), sy(y);

	s	+= sy;

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_Strings::CAPI_Strings(void)
{
	m_nStrings	= 0;
	m_Strings	= NULL;
}

//---------------------------------------------------------
CAPI_Strings::CAPI_Strings(const CAPI_Strings &Strings)
{
	m_nStrings	= 0;
	m_Strings	= NULL;

	Assign(Strings);
}

//---------------------------------------------------------
CAPI_Strings::CAPI_Strings(int nStrings, const char **Strings)
{
	m_nStrings	= 0;
	m_Strings	= NULL;

	for(int i=0; i<m_nStrings; i++)
	{
		Add(Strings[i]);
	}
}

//---------------------------------------------------------
CAPI_Strings::~CAPI_Strings(void)
{
	Clear();
}

//---------------------------------------------------------
void CAPI_Strings::Clear(void)
{
	for(int i=0; i<m_nStrings; i++)
	{
		delete(m_Strings[i]);
	}

	if( m_Strings )
	{
		API_Free(m_Strings);
	}

	m_nStrings	= 0;
	m_Strings	= NULL;
}

//---------------------------------------------------------
bool CAPI_Strings::Assign(const CAPI_Strings &Strings)
{
	Clear();

	for(int i=0; i<m_nStrings; i++)
	{
		Add(Strings[i]);
	}

	return( true );
}

//---------------------------------------------------------
CAPI_Strings & CAPI_Strings::operator  = (const CAPI_Strings &Strings)
{
	Assign(Strings);

	return( *this );
}

//---------------------------------------------------------
bool CAPI_Strings::Add(const CAPI_String &String)
{
	m_Strings	= (CAPI_String **)API_Realloc(m_Strings, (m_nStrings + 1) * sizeof(CAPI_String *));
	m_Strings[m_nStrings]	= new CAPI_String(String);
	m_nStrings++;

	return( true );
}

//---------------------------------------------------------
bool CAPI_Strings::Set_Count(int nStrings)
{
	Clear();

	for(int i=0; i<m_nStrings; i++)
	{
		Add("");
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_String		API_Get_CurrentTimeStr(bool bWithDate)
{
	CAPI_String	s;
	wxDateTime	t;

	t.SetToCurrent();

	if( bWithDate )
	{
		s.Append(t.FormatISODate());
		s.Append("/");
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
CAPI_String		API_Double2DegreeStr(double Value)
{
	char		c;
	int			d, h;
	double		s;
	CAPI_String	String;

	if( Value < 0.0 )
	{
		Value	= -Value;
		c		= '-';
	}
	else
	{
		c		= '+';
	}

	Value	= fmod(Value, 360.0);
	d		= (int)Value;
	Value	= 60.0 * (Value - d);
	h		= (int)Value;
	Value	= 60.0 * (Value - h);
	s		= Value;

	String.Printf("%c%03d°%02d'%02f''", c, d, h, s);

	return( String );
}

//---------------------------------------------------------
double			API_DegreeStr2Double(const char *String)
{
	double		d, h, s, sig;
	CAPI_String	sVal(String);

	sig	= 1.0;
	d	= h	= s	= 0.0;

	if( sVal.BeforeFirst('°').asDouble(d) )
	{
		if( d < 0.0 )
		{
			sig	= -1.0;
			d	= -d;
		}

		sVal.AfterFirst('°' ).asDouble(h);
		sVal.AfterFirst('\'').asDouble(s);
	}
	else
	{
		sVal.asDouble(d);
	}

	return( sig * (d + h / 60.0 + s / (60.0 * 60.0)) );
}

//---------------------------------------------------------
CAPI_String		API_Double2DateStr(double Value)
{
	int		y, m, d;

	y	= (int)(Value / 10000);	Value	-= y * 10000;
	m	= (int)(Value / 100);	Value	-= m * 100;
	d	= (int)(Value / 1);

	return( CAPI_String::Format("%02d.%02d.%04d", d, m, y) );
}

//---------------------------------------------------------
double			API_DateStr2Double(const char *String)
{
	int			d, m, y;

	if( String )
	{
		CAPI_String	s(String), sValue;

		sValue	= s.AfterLast	('.');
		y		= sValue.asInt();
		sValue	= s.BeforeLast	('.');	s		= sValue;

		sValue	= s.AfterLast	('.');
		m		= sValue.asInt();
		sValue	= s.BeforeLast	('.');	s		= sValue;
		d		= sValue.asInt();

		if( d < 1 )	d	= 1;	else if( d > 31 )	d	= 31;
		if( m < 1 )	m	= 1;	else if( m > 12 )	d	= 12;

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
int				API_Get_Significant_Decimals(double Value, int maxDecimals)
{
	int		Decimals;
	double	Reminder;

	Value	= fabs(Value);

	for(Decimals=0; Decimals<maxDecimals; Decimals++)
	{
		Reminder	= Value - (int)(Value);

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
CAPI_String		API_Get_String(double Value, int Precision, bool bScientific)
{
	CAPI_String	s;

	s.Printf("%.*f", Precision, Value);

	s.Replace(",", ".");

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
