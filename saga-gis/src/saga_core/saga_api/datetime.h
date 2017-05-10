/**********************************************************
 * Version $Id: datetime.h 2266 2014-10-01 15:00:52Z oconrad $
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
//                      datetime.h                       //
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
#ifndef HEADER_INCLUDED__SAGA_API__datetime_H
#define HEADER_INCLUDED__SAGA_API__datetime_H


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
// simplified wrappers for wxDateTime and related classes.


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_TimeSpan
{
	friend class CSG_DateTime;

public:

	CSG_TimeSpan(void)						{	m_span	= 0l       ;	}
	CSG_TimeSpan(const CSG_TimeSpan &ts)	{	m_span	= ts.m_span;	}
	CSG_TimeSpan(const sLong &diff)			{	m_span	= diff     ;	}
	CSG_TimeSpan(double hours)				{	m_span	= (sLong)(1000*60*60 * hours);	}
	CSG_TimeSpan(long hours, long min = 0, sLong sec = 0, sLong msec = 0)
	{
		m_span	= msec + 1000 * (sec + 60 * (min + 60 * hours));
	}

	//-----------------------------------------------------
	static CSG_TimeSpan			Milliseconds			(sLong ms   )						{	return( CSG_TimeSpan(0, 0, 0, ms) );	}
	static CSG_TimeSpan			Seconds					(sLong sec  )						{	return( CSG_TimeSpan(0, 0, sec  ) );	}
	static CSG_TimeSpan			Minutes					(long  min  )						{	return( CSG_TimeSpan(0, min, 0  ) );	}
	static CSG_TimeSpan			Hours					(long  hours)						{	return( CSG_TimeSpan(hours, 0, 0) );	}
	static CSG_TimeSpan			Days					(long  days )						{	return( Hours(24 * days) );	}
	static CSG_TimeSpan			Weeks					(long  days )						{	return( Days ( 7 * days) );	}

	static CSG_TimeSpan			Millisecond				(void)								{	return( Milliseconds(1) );	}
	static CSG_TimeSpan			Second					(void)								{	return( Seconds     (1) );	}
	static CSG_TimeSpan			Minute					(void)								{	return( Minutes     (1) );	}
	static CSG_TimeSpan			Hour					(void)								{	return( Hours       (1) );	}
	static CSG_TimeSpan			Day						(void)								{	return( Days        (1) );	}
	static CSG_TimeSpan			Week					(void)								{	return( Weeks       (1) );	}

	//-----------------------------------------------------
	CSG_TimeSpan &				Add						(const CSG_TimeSpan &ts)			{	m_span	+= ts.m_span;	return( *this );	}
	CSG_TimeSpan &				Subtract				(const CSG_TimeSpan &ts)			{	m_span	-= ts.m_span;	return( *this );	}
	CSG_TimeSpan &				Multiply				(int                  n)			{	m_span	*=         n;	return( *this );	}

	CSG_TimeSpan &				operator +=				(const CSG_TimeSpan &ts)			{	return( Add     (ts) );	}
	CSG_TimeSpan &				operator -=				(const CSG_TimeSpan &ts)			{	return( Subtract(ts) );	}
	CSG_TimeSpan &				operator *=				(int                  n)			{	return( Multiply( n) );	}

	CSG_TimeSpan				Add						(const CSG_TimeSpan &ts)	const	{	return( CSG_TimeSpan(m_span + ts.m_span) );	}
	CSG_TimeSpan				Subtract				(const CSG_TimeSpan &ts)	const	{	return( CSG_TimeSpan(m_span - ts.m_span) );	}
	CSG_TimeSpan				Multiply				(int                  n)	const	{	return( CSG_TimeSpan(m_span *         n) );	}

	CSG_TimeSpan				operator +				(const CSG_TimeSpan &ts)	const	{	return( CSG_TimeSpan(m_span + ts.m_span) );	}
	CSG_TimeSpan				operator -				(const CSG_TimeSpan &ts)	const	{	return( CSG_TimeSpan(m_span - ts.m_span) );	}
	CSG_TimeSpan				operator *				(int                  n)	const	{	return( CSG_TimeSpan(m_span *         n) );	}

	CSG_TimeSpan				Abs						(void)	const						{	return( CSG_TimeSpan(m_span >= 0l ? m_span : -m_span) );	}
	CSG_TimeSpan				Negate					(void)	const						{	return( CSG_TimeSpan(-m_span) );	}
	CSG_TimeSpan &				Neg						(void)								{	m_span	= -m_span;	return( *this );	}
	CSG_TimeSpan &				operator -				(void)								{	return( Neg() );	}

	bool						is_Null					(void)	const						{	return( m_span == 0l );	}
	bool						is_Positive				(void)	const						{	return( m_span >  0l );	}
	bool						is_Negative				(void)	const						{	return( m_span <  0l ); }

	bool						operator !				(void)	const						{	return( !is_Null() );	}

	bool						is_EqualTo				(const CSG_TimeSpan &ts)	const	{	return( m_span == ts.m_span );	}
	bool						is_LongerThan			(const CSG_TimeSpan &ts)	const	{	return( m_span >  ts.m_span );	}
	bool						is_ShorterThan			(const CSG_TimeSpan &ts)	const	{	return( m_span <  ts.m_span );	}

	bool						operator <				(const CSG_TimeSpan &ts)	const	{	return( m_span <  ts.m_span );	}
	bool						operator <=				(const CSG_TimeSpan &ts)	const	{	return( m_span <= ts.m_span );	}
	bool						operator ==				(const CSG_TimeSpan &ts)	const	{	return( m_span == ts.m_span );	}
	bool						operator !=				(const CSG_TimeSpan &ts)	const	{	return( m_span != ts.m_span );	}
	bool						operator >=				(const CSG_TimeSpan &ts)	const	{	return( m_span >= ts.m_span );	}
	bool						operator >				(const CSG_TimeSpan &ts)	const	{	return( m_span >  ts.m_span );	}

	int							Get_Weeks				(void)	const						{	return(       Get_Days   () /  7  );	}
	int							Get_Days				(void)	const						{	return(       Get_Hours  () / 24  );	}
	int							Get_Hours				(void)	const						{	return(       Get_Minutes() / 60  );	}
	int							Get_Minutes				(void)	const						{	return( (int)(Get_Seconds() / 60) );	}
	sLong						Get_Seconds				(void)	const						{	return( m_span / 1000l );	}
	sLong						Get_Milliseconds		(void)	const						{	return( m_span );	}
	sLong						Get_Value				(void)	const						{	return( m_span );	}

	CSG_String					Format					(const CSG_String &format = "")	const;


private:

	sLong						m_span;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_DateTime
{
public:

	//-----------------------------------------------------
	// types

	typedef unsigned short	TSG_DateTime;


	//-----------------------------------------------------
	// constants

	enum Month		// symbolic names for the months
	{
		Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec, Inv_Month
	};

	enum WeekDay	// symbolic names for the weekdays
	{
		Sun, Mon, Tue, Wed, Thu, Fri, Sat, Inv_WeekDay
	};

	enum Year		// invalid value for the year
	{
		Inv_Year	= -32768	// should hold in wxDateTime_t
	};

	enum NameFlags	// flags for GetWeekDayName and GetMonthName
	{
		Name_Full	= 0x01,	// return full name
		Name_Abbr	= 0x02	// return abbreviated name
	};


public:
								
	//-----------------------------------------------------
	CSG_DateTime(void);
	CSG_DateTime(const CSG_DateTime &DateTime);
	CSG_DateTime(double JDN);
	CSG_DateTime(                                                    TSG_DateTime Hour    , TSG_DateTime Minute = 0, TSG_DateTime Second = 0, TSG_DateTime Millisec = 0);
	CSG_DateTime(TSG_DateTime Day, Month Month, int Year = Inv_Year, TSG_DateTime Hour = 0, TSG_DateTime Minute = 0, TSG_DateTime Second = 0, TSG_DateTime Millisec = 0);

	virtual ~CSG_DateTime(void);


	//-----------------------------------------------------
	CSG_DateTime &				Set						(const CSG_DateTime &DateTime);
	CSG_DateTime &				Set						(double JDN);
	CSG_DateTime &				Set						(                                                    TSG_DateTime Hour    , TSG_DateTime Minute = 0, TSG_DateTime Second = 0, TSG_DateTime Millisec = 0);
	CSG_DateTime &				Set						(TSG_DateTime Day, Month Month, int Year = Inv_Year, TSG_DateTime Hour = 0, TSG_DateTime Minute = 0, TSG_DateTime Second = 0, TSG_DateTime Millisec = 0);

	CSG_DateTime &				Set_Millisecond			(unsigned short Value);
	CSG_DateTime &				Set_Second				(unsigned short Value);
	CSG_DateTime &				Set_Minute				(unsigned short Value);
	CSG_DateTime &				Set_Hour				(unsigned short Value);
	CSG_DateTime &				Set_Hour				(double         Value);
	CSG_DateTime &				Set_Day					(unsigned short Value);
	CSG_DateTime &				Set_Month				(Month          Value);
	CSG_DateTime &				Set_Year				(int            Value);

	CSG_DateTime &				Set_To_Current			(void);

	CSG_DateTime &				Reset_Time				(void);


	//-----------------------------------------------------
	CSG_DateTime &				Make_UTC				(bool noDST = false);

	bool						is_DST					(void)	const;


	//-----------------------------------------------------
	CSG_DateTime				Add						(const CSG_TimeSpan &TimeSpan)	const;
	CSG_DateTime &				Add						(const CSG_TimeSpan &TimeSpan);

	CSG_DateTime				Subtract				(const CSG_TimeSpan &TimeSpan)	const;
	CSG_DateTime &				Subtract				(const CSG_TimeSpan &TimeSpan);

	CSG_TimeSpan				Subtract				(const CSG_DateTime &DateTime)	const;


	//-----------------------------------------------------
	bool						is_Valid				(void)	const;

	sLong						Get_Value				(void)	const;

	unsigned short				Get_Millisecond			(void)	const;
	unsigned short				Get_Second				(void)	const;
	unsigned short				Get_Minute				(void)	const;
	unsigned short				Get_Hour				(void)	const;

	unsigned short				Get_Day					(void)	const;
	Month						Get_Month				(void)	const;
	int							Get_Year				(void)	const;

	unsigned short				Get_DayOfYear			(void)	const;
	WeekDay						Get_WeekDay				(void)	const;
	TSG_DateTime				Get_WeekOfMonth			(void)	const;
	TSG_DateTime				Get_WeekOfYear			(void)	const;

	double						Get_JDN					(void)	const;
	double						Get_MJD					(void)	const;


	//-----------------------------------------------------
	bool						is_EarlierThan			(const CSG_DateTime &DateTime)	const;
	bool						is_EqualTo				(const CSG_DateTime &DateTime)	const;
	bool						is_LaterThan			(const CSG_DateTime &DateTime)	const;

	bool						is_SameDate				(const CSG_DateTime &DateTime)	const;
	bool						is_SameTime				(const CSG_DateTime &DateTime)	const;

	bool						is_Between				(const CSG_DateTime &t1, const CSG_DateTime &t2)	const;
	bool						is_StrictlyBetween		(const CSG_DateTime &t1, const CSG_DateTime &t2)	const;


	//-----------------------------------------------------
	CSG_DateTime &				operator  =				(const CSG_DateTime &DateTime)			{	return( Set     (DateTime) );	}
	CSG_DateTime &				operator  =				(double JDN)							{	return( Set     (     JDN) );	}

	CSG_DateTime &				operator +=				(const CSG_TimeSpan &TimeSpan)			{	return( Add     (TimeSpan) );	}
	CSG_DateTime &				operator -=				(const CSG_TimeSpan &TimeSpan)			{	return( Subtract(TimeSpan) );	}

	CSG_DateTime				operator +				(const CSG_TimeSpan &TimeSpan)	const	{	return( Add     (TimeSpan) );	}
	CSG_DateTime				operator -				(const CSG_TimeSpan &TimeSpan)	const	{	return( Subtract(TimeSpan) );	}

	CSG_TimeSpan				operator -				(const CSG_DateTime &DateTime)	const	{	return( Subtract(DateTime) );	}

	bool						operator <				(const CSG_DateTime &DateTime)	const	{	return( Get_Value() <  DateTime.Get_Value() );	}
	bool						operator <=				(const CSG_DateTime &DateTime)	const	{	return( Get_Value() <= DateTime.Get_Value() );	}
	bool						operator ==				(const CSG_DateTime &DateTime)	const	{	return( Get_Value() == DateTime.Get_Value() );	}
	bool						operator >=				(const CSG_DateTime &DateTime)	const	{	return( Get_Value() >= DateTime.Get_Value() );	}
	bool						operator >				(const CSG_DateTime &DateTime)	const	{	return( Get_Value() >  DateTime.Get_Value() );	}


	//-----------------------------------------------------
	CSG_String					Format					(const CSG_String &Format = "")	const;
	CSG_String					Format_Date				(void)	const;
	CSG_String					Format_Time				(void)	const;
	CSG_String					Format_ISODate			(void)	const;
	CSG_String					Format_ISOTime			(void)	const;
	CSG_String					Format_ISOCombined		(char sep = 'T')	const;

	bool						Parse_Date				(const CSG_String &date);
	bool						Parse_DateTime			(const CSG_String &datetime);
	bool						Parse_Format			(const CSG_String &date, const CSG_String &format, const CSG_DateTime &dateDef);
	bool						Parse_Format			(const CSG_String &date, const CSG_String &format);
	bool						Parse_Format			(const CSG_String &date);
	bool						Parse_ISOCombined		(const CSG_String &date, char sep= 'T');
	bool						Parse_ISODate			(const CSG_String &date);
	bool						Parse_ISOTime			(const CSG_String &date);


	//-----------------------------------------------------
	static TSG_DateTime			Get_Current_Day			(void);
	static Month				Get_Current_Month		(void);
	static int					Get_Current_Year		(void);

	static CSG_String			Get_MonthName			(Month     month, NameFlags flags = Name_Full);
	static CSG_String			Get_EnglishMonthName	(Month     month, NameFlags flags = Name_Full);
	static CSG_String			Get_WeekDayName			(WeekDay weekday, NameFlags flags = Name_Full);
	static CSG_String			Get_EnglishWeekDayName	(WeekDay weekday, NameFlags flags = Name_Full);

	static TSG_DateTime			Get_NumberOfDays		(             int year);
	static TSG_DateTime			Get_NumberOfDays		(Month month, int year = Inv_Year);

	static bool					is_LeapYear				(int year = Inv_Year);

	static CSG_DateTime			Now						(void);

	static CSG_String			Get_Month_Choices		(void);


private:

	class wxDateTime			*m_pDateTime;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool		SG_Get_Sun_Position		(double   JulianDayNumber, double &RA, double &Dec);
SAGA_API_DLL_EXPORT bool		SG_Get_Sun_Position		(const CSG_DateTime &Time, double &RA, double &Dec);

SAGA_API_DLL_EXPORT bool		SG_Get_Sun_Position		(double   JulianDayNumber, double Longitude, double Latitude, double &Height, double &Azimuth);
SAGA_API_DLL_EXPORT bool		SG_Get_Sun_Position		(const CSG_DateTime &Time, double Longitude, double Latitude, double &Height, double &Azimuth);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Time_Format
{
	SG_TIME_FMT_Seconds_Unix	= 0,
	SG_TIME_FMT_Hours_AD
}
TSG_Time_Format;

//---------------------------------------------------------
typedef enum ESG_Time_String_Format
{
	SG_TIME_STRFMT_YMD_hms		= 0,
	SG_TIME_STRFMT_DMY_hms
}
TSG_Time_String_Format;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Time_Converter
{
public:
	CSG_Time_Converter(void);
	CSG_Time_Converter(int Time, TSG_Time_Format Format);

	bool						Set_Time		(int Time, TSG_Time_Format Format);

	CSG_String					Get_String		(void)	const;

	static CSG_String			Get_String		(int Time, TSG_Time_Format Format);


public:

	TSG_Time_String_Format		m_StrFmt;

	int							m_sec, m_min, m_hour, m_day, m_mon, m_year;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__datetime_H
