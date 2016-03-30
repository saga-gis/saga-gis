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

CSG_DateTime & CSG_DateTime::Set_Hour(double Value)
{
	m_pDateTime->ResetTime();

	if( Value > 0.0 && Value < 24.0 )
	{
		m_pDateTime->SetHour       ((wxDateTime::wxDateTime_t)Value);	Value	= (Value - (wxDateTime::wxDateTime_t)Value) *   60.0;
		m_pDateTime->SetMinute     ((wxDateTime::wxDateTime_t)Value);	Value	= (Value - (wxDateTime::wxDateTime_t)Value) *   60.0;
		m_pDateTime->SetSecond     ((wxDateTime::wxDateTime_t)Value);	Value	= (Value - (wxDateTime::wxDateTime_t)Value) * 1000.0;
		m_pDateTime->SetMillisecond((wxDateTime::wxDateTime_t)Value);
	}

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
CSG_DateTime & CSG_DateTime::Make_UTC(bool noDST)
{
	m_pDateTime->MakeUTC(noDST);

	return( *this );
}

//---------------------------------------------------------
bool CSG_DateTime::is_DST(void)	const
{
	return( m_pDateTime->IsDST() != 0 );
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
	wxString	s(m_pDateTime->Format(Format.c_str()));	CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Format_Date(void)	const
{
	wxString	s(m_pDateTime->FormatDate());			CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Format_Time(void)	const
{
	wxString	s(m_pDateTime->FormatTime());			CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Format_ISODate(void)	const
{
	wxString	s(m_pDateTime->FormatISODate());		CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Format_ISOTime(void)	const
{
	wxString	s(m_pDateTime->FormatISOTime());		CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Format_ISOCombined(char sep)	const
{
	wxString	s(m_pDateTime->FormatISOCombined(sep));	CSG_String _s(&s); return( _s );
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
CSG_DateTime::TSG_DateTime CSG_DateTime::Get_Current_Day(void)
{
	CSG_DateTime Now;
	
	Now.Set_To_Current();
	
	return( Now.Get_Day() );
}

CSG_DateTime::Month CSG_DateTime::Get_Current_Month(void)
{
	return( (Month)wxDateTime::GetCurrentMonth() );
}

int CSG_DateTime::Get_Current_Year(void)
{
	return( wxDateTime::GetCurrentYear() );
}

//---------------------------------------------------------
CSG_String CSG_DateTime::Get_MonthName(Month month, NameFlags flags)
{
	wxString	s(wxDateTime::GetMonthName         ((wxDateTime::Month)month    , (wxDateTime::NameFlags)flags));	CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Get_EnglishMonthName(Month month, NameFlags flags)
{
	wxString	s(wxDateTime::GetEnglishMonthName  ((wxDateTime::Month)month    , (wxDateTime::NameFlags)flags));	CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Get_WeekDayName(WeekDay weekday, NameFlags flags)
{
	wxString	s(wxDateTime::GetWeekDayName       ((wxDateTime::WeekDay)weekday, (wxDateTime::NameFlags)flags));	CSG_String _s(&s); return( _s );
}

CSG_String CSG_DateTime::Get_EnglishWeekDayName(WeekDay weekday, NameFlags flags)
{
	wxString	s(wxDateTime::GetEnglishWeekDayName((wxDateTime::WeekDay)weekday, (wxDateTime::NameFlags)flags));	CSG_String _s(&s); return( _s );
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
CSG_String CSG_DateTime::Get_Month_Choices(void)
{
	CSG_String	Choices;

	Choices.Printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
		_TL("January"), _TL("February"), _TL("March"    ), _TL("April"  ), _TL("May"     ), _TL("June"    ),
		_TL("July"   ), _TL("August"  ), _TL("September"), _TL("October"), _TL("November"), _TL("December")
	);

	return( Choices );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "mat_tools.h"

//---------------------------------------------------------
/**
  * Calculates the Sun's position for given date and time
  * as declination and right ascension angles (radians).
  * Solar coordinates according to
  * Jean Meeus: Astronomical Algorithms, accuracy of 0.01 degree
*/
//---------------------------------------------------------
bool SG_Get_Sun_Position(double JulianDayNumber, double &RA, double &Dec)
{
	//-----------------------------------------------------
	double	T	= (JulianDayNumber - 2451545.0 ) / 36525.0;	// Number of Julian centuries since 2000/01/01 at 12 UT (JDN = 2451545.0)

	double	M	= M_DEG_TO_RAD *  (357.52910 + 35999.05030 * T - 0.0001559 * T*T - 0.00000048 * T*T*T);	// mean anomaly
	double	L	= M_DEG_TO_RAD * ((280.46645 + 36000.76983 * T + 0.0003032 * T*T)						// mean longitude
							   + ((1.914600 - 0.004817 * T - 0.000014  * T*T) * sin(M)
							   +  (0.019993 - 0.000101 * T) * sin(2.0 * M) + 0.000290 * sin(3.0 * M)));	// true longitude

	//-----------------------------------------------------
	// convert ecliptic longitude to right ascension RA and declination delta

	static const double	Ecliptic_Obliquity	= M_DEG_TO_RAD * 23.43929111;

	double	X	= cos(L);
	double	Y	= cos(Ecliptic_Obliquity) * sin(L);
	double	Z	= sin(Ecliptic_Obliquity) * sin(L);
	double	R	= sqrt(1.0 - Z*Z); 

	Dec	= atan2(Z, R);
	RA	= 2.0 * atan2(Y, (X + R));

	return( true );
}

//---------------------------------------------------------
bool SG_Get_Sun_Position(const CSG_DateTime &Time, double &RA, double &Dec)
{
	return( SG_Get_Sun_Position(Time.Get_JDN(), RA, Dec) );
}

//---------------------------------------------------------
/**
  * Calculates the Sun's position for given date and time
  * and position on Earth as given by longitude and latitude (radians).
  * Height and Azimuth are calculated as radians.
  * Returns true if Sun is above horizon.
*/
//---------------------------------------------------------
bool SG_Get_Sun_Position(double JulianDayNumber, double Longitude, double Latitude, double &Height, double &Azimuth)
{
	//-----------------------------------------------------
	// 1. Get right ascension RA and declination delta

	double	RA, Delta;	SG_Get_Sun_Position(JulianDayNumber, RA, Delta);

	//-----------------------------------------------------
	// 2. compute sidereal time (radians) at Greenwich local sidereal time at longitude (radians)

	double	T		= (JulianDayNumber - 2451545.0 ) / 36525.0;

	double	Theta	= Longitude + M_DEG_TO_RAD * (280.46061837 + 360.98564736629 * (JulianDayNumber - 2451545.0) + T*T * (0.000387933 - T / 38710000.0));

	double	Tau		= Theta - RA;	// compute local hour angle (radians)

	//-----------------------------------------------------
	// 3. convert (tau, delta) to horizon coordinates (h, az) of the observer

	Height	= asin ( sin(Latitude) * sin(Delta) + cos(Latitude) * cos(Delta) * cos(Tau));
	Azimuth	= atan2(-sin(Tau) * cos(Delta), cos(Latitude) * sin(Delta) - sin(Latitude) * cos(Delta) * cos(Tau));
//	Azimuth	= atan2(-sin(Tau), cos(Latitude) * tan(Delta) - sin(Latitude) * cos(Tau));	// previous formula gives same result but is better because of division by zero effects...

	return( Height > 0.0 );
}

//---------------------------------------------------------
bool SG_Get_Sun_Position(const CSG_DateTime &Time, double Longitude, double Latitude, double &Height, double &Azimuth)
{
	return( SG_Get_Sun_Position(Time.Get_JDN(), Longitude, Latitude, Height, Azimuth) );
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
