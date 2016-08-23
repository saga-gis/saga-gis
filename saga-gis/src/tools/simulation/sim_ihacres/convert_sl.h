/**********************************************************
 * Version $Id: convert_sl.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
#ifndef __convert_sl_h
#define __convert_sl_h

#include <iostream>
#include <string>
#include <sstream>			// function StringToDouble
#ifdef _SAGA_LINUX
	#include <cstdio>
#endif

using std::string;
using std::istringstream;	// function StringToDouble
using std::stringstream;	// used by function Double2String(), Int2String()


namespace convert_sl
{

	// if a string is a char* then:
	//		sprintf(myString, "%d", myInt);
	// or:
	//		_itoa(myInt, myString, 10);

	//---------------------------------------------------------------------
	// Converting a string to an integer value
	int			StringToInt(string str);

	//---------------------------------------------------------------------
	// Converting a string to a double value
	double		StringToDouble (string s);

	//---------------------------------------------------------------------
	// Any Number to std::string
	//---------------------------------------------------------------------
	string		Number2String(double d);
	string		Number2String(float f);
	string		Number2String(int i);
	string		Number2String(long l);
	//---------------------------------------------------------------------

	//---------------------------------------------------------------------
	// Converting a double to a string value
	string		Double2String(double d);

	//---------------------------------------------------------------------
	// Converting a float to a string value
	string		Float2String(float f);

	//---------------------------------------------------------------------
	// Converting an integer to a string value
	string		Int2String(int i);

	//---------------------------------------------------------------------
	// Converting a string value to bool
	bool		StringToBool(string str);


	//---------------------------------------------------------------------
	// Converting a date string 'YYYYMMDD' to three separate integer values
	void		Get_Date_Int(string date, int &year, int &month, int &day);

};

#endif /* __convert_sl_h */
