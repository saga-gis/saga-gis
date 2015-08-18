/**********************************************************
 * Version $Id: datetime.cpp 2291 2014-10-16 13:46:55Z oconrad $
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
//                     datetime.cpp                      //
//                                                       //
//          Copyright (C) 2015 by Olaf Conrad            //
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
//                University of Hamburg                  //
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
#include "datetime.h"

#include <wx/datetime.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_TimeSpan::Format(const CSG_String &Format)	const
{
	wxTimeSpan	ts((wxLongLong(m_span)));

	wxString	s	= !Format.is_Empty() ? ts.Format(Format.c_str()) : ts.Format(wxDefaultTimeSpanFormat);

	return( CSG_String(&s) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_DateTime::CSG_DateTime(void)
{
	m_pDateTime	= new wxDateTime();
}

//---------------------------------------------------------
CSG_DateTime::CSG_DateTime(const CSG_DateTime &DateTime)
{
	m_pDateTime	= new wxDateTime(*DateTime.m_pDateTime);
}

//---------------------------------------------------------
CSG_DateTime::CSG_DateTime(double JDN)
{
	m_pDateTime	= new wxDateTime(JDN);
}

//---------------------------------------------------------
CSG_DateTime::CSG_DateTime(TSG_DateTime Hour, TSG_DateTime Minute, TSG_DateTime Second, TSG_DateTime Millisec)
{
	m_pDateTime	= new wxDateTime(Hour, Minute, Second, Millisec);
}

//---------------------------------------------------------
CSG_DateTime::CSG_DateTime(TSG_DateTime Day, Month Month, int Year, TSG_DateTime Hour, TSG_DateTime Minute, TSG_DateTime Second, TSG_DateTime Millisec)
{
	m_pDateTime	= new wxDateTime(Day, (wxDateTime::Month)Month, Year, Hour, Minute, Second, Millisec);
}

//---------------------------------------------------------
CSG_DateTime::~CSG_DateTime(void)
{
	delete(m_pDateTime);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set(const CSG_DateTime &DateTime)
{
	m_pDateTime->Set(DateTime.m_pDateTime->GetTm());

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set(double JDN)
{
	m_pDateTime->Set(JDN);

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set(TSG_DateTime Hour, TSG_DateTime Minute, TSG_DateTime Second, TSG_DateTime Millisec)
{
	m_pDateTime->Set(Hour, Minute, Second, Millisec);

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set(TSG_DateTime Day, Month Month, int Year, TSG_DateTime Hour, TSG_DateTime Minute, TSG_DateTime Second, TSG_DateTime Millisec)
{
	m_pDateTime->Set(Day, (wxDateTime::Month)Month, Year, Hour, Minute, Second, Millisec);

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set_Millisecond(unsigned short Value)
{
	m_pDateTime->SetMillisecond(Value);

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set_Second(unsigned short Value)
{
	m_pDateTime->SetSecond(Value);

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set_Minute(unsigned short Value)
{
	m_pDateTime->SetMinute(Value);

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set_Hour(unsigned short Value)
{
	m_pDateTime->SetHour(Value);

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set_Day(unsigned short Value)
{
	m_pDateTime->SetDay(Value);

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set_Month(Month Value)
{
	m_pDateTime->SetMonth((wxDateTime::Month)Value);

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set_Year(int Value)
{
	m_pDateTime->SetYear(Value);

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Set_To_Current(void)
{
	m_pDateTime->SetToCurrent();

	return( *this );
}

//---------------------------------------------------------
CSG_DateTime & CSG_DateTime::Reset_Time(void)
{
	m_pDateTime->ResetTime();

	return( *this );
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
sLong CSG_DateTime::Get_Value(void)	const
{
	return( m_pDateTime->GetValue().GetValue() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_DateTime CSG_DateTime::Add(const CSG_TimeSpan &TimeSpan)	const
{
	CSG_DateTime	dt(*this);

	return( dt.Add(TimeSpan) );
}

CSG_DateTime & CSG_DateTime::Add(const CSG_TimeSpan &TimeSpan)
{
	m_pDateTime->Add(wxTimeSpan((wxLongLong)TimeSpan.m_span));

	return( *this);
}

//---------------------------------------------------------
CSG_DateTime CSG_DateTime::Subtract(const CSG_TimeSpan &TimeSpan)	const
{
	CSG_DateTime	dt(*this);

	return( dt.Subtract(TimeSpan) );
}

CSG_DateTime & CSG_DateTime::Subtract(const CSG_TimeSpan &TimeSpan)
{
	m_pDateTime->Subtract(wxTimeSpan((wxLongLong)TimeSpan.m_span));

	return( *this);
}

//---------------------------------------------------------
CSG_TimeSpan CSG_DateTime::Subtract(const CSG_DateTime &DateTime)	const
{
	CSG_TimeSpan	Span((sLong)m_pDateTime->Subtract(*DateTime.m_pDateTime).GetValue().GetValue());

	return( Span );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_DateTime::is_Valid(void)	const
{
	return( m_pDateTime->IsValid() );
}

//---------------------------------------------------------
unsigned short CSG_DateTime::Get_Millisecond(void)	const	{	return( m_pDateTime->GetMillisecond() );	}
unsigned short CSG_DateTime::Get_Second     (void)	const	{	return( m_pDateTime->GetSecond     () );	}
unsigned short CSG_DateTime::Get_Minute     (void)	const	{	return( m_pDateTime->GetMinute     () );	}
unsigned short CSG_DateTime::Get_Hour       (void)	const	{	return( m_pDateTime->GetHour       () );	}
unsigned short CSG_DateTime::Get_Day        (void)	const	{	return( m_pDateTime->GetDay        () );	}

CSG_DateTime::Month CSG_DateTime::Get_Month(void)	const
{
	return( (Month)m_pDateTime->GetMonth() );
}

int CSG_DateTime::Get_Year(void)	const
{
	return( m_pDateTime->GetYear() );
}

//---------------------------------------------------------
unsigned short CSG_DateTime::Get_DayOfYear(void)	const
{
	return( m_pDateTime->GetDayOfYear() );
}

CSG_DateTime::WeekDay CSG_DateTime::Get_WeekDay(void)	const
{
	return( (WeekDay)m_pDateTime->GetWeekDay() );
}

CSG_DateTime::TSG_DateTime CSG_DateTime::Get_WeekOfMonth(void)	const
{
	return( (TSG_DateTime)m_pDateTime->GetWeekOfMonth() );
}

CSG_DateTime::TSG_DateTime CSG_DateTime::Get_WeekOfYear(void)	const
{
	return( (TSG_DateTime)m_pDateTime->GetWeekOfYear() );
}

//---------------------------------------------------------
double CSG_DateTime::Get_JDN(void)	const
{
	return( m_pDateTime->GetJDN() );
}

double CSG_DateTime::Get_MJD(void)	const
{
	return( m_pDateTime->GetMJD() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_DateTime::is_EarlierThan    (const CSG_DateTime &DateTime)	const	{	return( m_pDateTime->IsEarlierThan(*DateTime.m_pDateTime) );	}
bool CSG_DateTime::is_EqualTo        (const CSG_DateTime &DateTime)	const	{	return( m_pDateTime->IsEqualTo    (*DateTime.m_pDateTime) );	}
bool CSG_DateTime::is_LaterThan      (const CSG_DateTime &DateTime)	const	{	return( m_pDateTime->IsLaterThan  (*DateTime.m_pDateTime) );	}

//---------------------------------------------------------
bool CSG_DateTime::is_SameDate       (const CSG_DateTime &DateTime)	const	{	return( m_pDateTime->IsSameDate   (*DateTime.m_pDateTime) );	}
bool CSG_DateTime::is_SameTime       (const CSG_DateTime &DateTime)	const	{	return( m_pDateTime->IsSameTime   (*DateTime.m_pDateTime) );	}

//---------------------------------------------------------
bool CSG_DateTime::is_Between        (const CSG_DateTime &t1, const CSG_DateTime &t2)	const	{	return( m_pDateTime->IsBetween        (*t1.m_pDateTime, *t2.m_pDateTime) );	}
bool CSG_DateTime::is_StrictlyBetween(const CSG_DateTime &t1, const CSG_DateTime &t2)	const	{	return( m_pDateTime->IsStrictlyBetween(*t1.m_pDateTime, *t2.m_pDateTime) );	}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_DateTime::Format(const CSG_String &Format)	const
{
	wxString	s(m_pDateTime->Format(Format.c_str()));	return( &s ); // CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Format_Date(void)	const
{
	wxString	s(m_pDateTime->FormatDate());			return( &s ); // CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Format_Time(void)	const
{
	wxString	s(m_pDateTime->FormatTime());			return( &s ); // CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Format_ISODate(void)	const
{
	wxString	s(m_pDateTime->FormatISODate());		return( &s ); // CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Format_ISOTime(void)	const
{
	wxString	s(m_pDateTime->FormatISOTime());		return( &s ); // CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Format_ISOCombined(char sep)	const
{
	wxString	s(m_pDateTime->FormatISOCombined(sep));	return( &s ); // CSG_String _s(&s); return( _s );
}

//---------------------------------------------------------
bool CSG_DateTime::Parse_Date(const CSG_String &date)
{
	wxString::const_iterator	end;	return( m_pDateTime->ParseDate(date.c_str(), &end) );
}

bool CSG_DateTime::Parse_DateTime(const CSG_String &datetime)
{
	wxString::const_iterator	end;	return( m_pDateTime->ParseDateTime(datetime.c_str(), &end) );
}

bool CSG_DateTime::Parse_Format(const CSG_String &date, const CSG_String &format, const CSG_DateTime &dateDef)
{
	wxString::const_iterator	end;	return( m_pDateTime->ParseFormat(date.c_str(), format.c_str(), *dateDef.m_pDateTime, &end) );
}

bool CSG_DateTime::Parse_Format(const CSG_String &date, const CSG_String &format)
{
	wxString::const_iterator	end;	return( m_pDateTime->ParseFormat(date.c_str(), format.c_str(), &end) );
}

bool CSG_DateTime::Parse_Format(const CSG_String &date)
{
	wxString::const_iterator	end;	return( m_pDateTime->ParseFormat(date.c_str(), &end) );
}

//---------------------------------------------------------
bool CSG_DateTime::Parse_ISOCombined(const CSG_String &date, char sep)
{
	return( m_pDateTime->ParseISOCombined(date.c_str(), sep) );
}

bool CSG_DateTime::Parse_ISODate(const CSG_String &date)
{
	return( m_pDateTime->ParseISODate(date.c_str()) );
}

bool CSG_DateTime::Parse_ISOTime(const CSG_String &date)
{
	return( m_pDateTime->ParseISOTime(date.c_str()) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_DateTime::Month CSG_DateTime::Get_Current_Month(void)	{	return( (Month)wxDateTime::GetCurrentMonth() );	}
int                 CSG_DateTime::Get_Current_Year (void)	{	return(        wxDateTime::GetCurrentYear () );	}

//---------------------------------------------------------
CSG_String CSG_DateTime::Get_MonthName(Month month, NameFlags flags)
{
	wxString	s(wxDateTime::GetMonthName         ((wxDateTime::Month)month    , (wxDateTime::NameFlags)flags));	return( &s ); // CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Get_EnglishMonthName(Month month, NameFlags flags)
{
	wxString	s(wxDateTime::GetEnglishMonthName  ((wxDateTime::Month)month    , (wxDateTime::NameFlags)flags));	return( &s ); // CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Get_WeekDayName(WeekDay weekday, NameFlags flags)
{
	wxString	s(wxDateTime::GetWeekDayName       ((wxDateTime::WeekDay)weekday, (wxDateTime::NameFlags)flags));	return( &s ); // CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Get_EnglishWeekDayName(WeekDay weekday, NameFlags flags)
{
	wxString	s(wxDateTime::GetEnglishWeekDayName((wxDateTime::WeekDay)weekday, (wxDateTime::NameFlags)flags));	return( &s ); // CSG_String _s(&s); return( _s );
}

//---------------------------------------------------------
CSG_DateTime::TSG_DateTime CSG_DateTime::Get_NumberOfDays(int year)
{
	return( (TSG_DateTime)wxDateTime::GetNumberOfDays(year) );
}

CSG_DateTime::TSG_DateTime CSG_DateTime::Get_NumberOfDays(Month month, int year)
{
	return( (TSG_DateTime)wxDateTime::GetNumberOfDays((wxDateTime::Month)month, year) );
}

//---------------------------------------------------------
bool CSG_DateTime::is_LeapYear(int year)
{
	return( wxDateTime::IsLeapYear(year) );
}

//---------------------------------------------------------
CSG_DateTime CSG_DateTime::Now(void)
{
	CSG_DateTime	DateTime;	*DateTime.m_pDateTime	= wxDateTime::Now();

	return( DateTime );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
