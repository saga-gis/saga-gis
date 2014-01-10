/**********************************************************
 * Version $Id: convert_sl.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
#include "convert_sl.h"

//---------------------------------------------------------------------
//
//								DATA TYPE
//                         CONVERTING FUNCTIONS
//
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Converting a string to an integer value
int convert_sl::StringToInt(string str)
{
	int i;

	sscanf(str.c_str(),"%d",&i);
	return i;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
// Converting a string to a double value

double convert_sl::StringToDouble (string s)
{
	// function adopted from:
	// http://episteme.arstechnica.com/groupee/forums/a/tpc/f/6330927813/m/736000556631/p/2
	double d_return_value;
	istringstream iss(s);

	// Extract a double from the input string stream
	iss >> d_return_value;
	if (iss.fail())
	{
		return -9999.0;
	}

	return d_return_value;
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Any number to string
//---------------------------------------------------------------------

string convert_sl::Number2String(double d)
{
	std::ostringstream s;
	s << d;
	return(s.str());
}
//---------------------------------------------------------------------

string convert_sl::Number2String(float f)
{
	std::ostringstream s;
	s << f;
	return(s.str());
}
//---------------------------------------------------------------------

string convert_sl::Number2String(int i)
{
	std::ostringstream s;
	s << i;
	return(s.str());
}
//---------------------------------------------------------------------

string convert_sl::Number2String(long l)
{
	std::ostringstream s;
	s << l;
	return(s.str());
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------

string convert_sl::Double2String(double d)
{
	std::ostringstream s;
	s << d;
	return(s.str());
}
//---------------------------------------------------------------------

string convert_sl::Float2String(float f)
{
	std::ostringstream s;
	s << f;
	return(s.str());
}
//---------------------------------------------------------------------

string convert_sl::Int2String(int i)
{
	// Bjarne Stroustrup's C++ Style and Technique FAQ
	// http://www.research.att.com/~bs/bs_faq2.html#int-to-string
	std::ostringstream s;
	s << i;
	return(s.str());
}
//---------------------------------------------------------------------

bool convert_sl::StringToBool(string str)
{
	if ( (str.substr(0,1) == "0") ||
		 (str == "false") ||
		 (str == "FALSE") ||
		 (str == "False") ||
		 (str == "f") ||
		 (str == "F") )
	{
		 return(false);
	} else {
		return(true);
	}
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Converting a date string 'YYYYMMDD' to three separate integer values

void convert_sl::Get_Date_Int(string date, int &year, int &month, int &day)
{
	year	= StringToInt(date.substr(0,4));
	month	= StringToInt(date.substr(4,2));
	day		= StringToInt(date.substr(6,2));
	// divide date string to year, month, and day string
	//string y(date.c_str(),0,4);
	//string m(date.c_str(),4,2);
	//string d(date.c_str(),6,2);

	//// convert to integer
	//sscanf(y.c_str(),"%d",&year);
	//sscanf(m.c_str(),"%d",&month);
	//sscanf(d.c_str(),"%d",&day);
}
//---------------------------------------------------------------------
