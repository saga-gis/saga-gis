
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
//                    table_value.h                      //
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
#ifndef HEADER_INCLUDED__SAGA_API__table_value_H
#define HEADER_INCLUDED__SAGA_API__table_value_H


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
#define NODATA_STRING	"__NO_DATA__"
#define NODATA_INT		-2147483647
#define NODATA_DOUBLE	2.2204460492503131e-016


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Value
{
public:
	CTable_Value(void)	{}
	virtual ~CTable_Value(void)	{}

	virtual bool				Set_Value		(const char *Value)	= 0;
	virtual bool				Set_Value		(double      Value)	= 0;

	virtual bool				Set_NoData		(void)				= 0;
	virtual bool				is_NoData		(void)				= 0;

	virtual const char *		asString		(int Decimals)		= 0;
	virtual int					asInt			(void)				= 0;
	virtual double				asDouble		(void)				= 0;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Value_String : public CTable_Value
{
public:
	CTable_Value_String(void) {}
	virtual ~CTable_Value_String(void) {}

	//-----------------------------------------------------
	virtual bool				Set_Value		(const char *Value)
	{
		if( Value && m_Value.Cmp(Value) )
		{
			m_Value.Printf(Value);

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool				Set_Value		(double      Value)
	{
		CAPI_String	s;

		s.Printf("%f", Value);

		return( Set_Value(s.c_str()) );
	}

	//-----------------------------------------------------
	virtual bool				Set_NoData		(void)			{	return( Set_Value(NODATA_STRING) );			}
	virtual bool				is_NoData		(void)			{	return( m_Value.Cmp(NODATA_STRING) == 0 );	}

	//-----------------------------------------------------
	virtual const char *		asString		(int Decimals)	{	return( m_Value );	}
	virtual int					asInt			(void)			{	return( m_Value.asInt() );	}
	virtual double				asDouble		(void)			{	return( m_Value.asDouble() );	}


private:

	CAPI_String					m_Value;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Value_Date : public CTable_Value
{
public:
	CTable_Value_Date(void) {	Set_Value("0");	}
	virtual ~CTable_Value_Date(void) {}

	//-----------------------------------------------------
	virtual bool				Set_Value		(const char *Value)
	{
		return( Set_Value(API_DateStr2Double(Value)) );
	}

	//-----------------------------------------------------
	virtual bool				Set_Value		(double      Value)
	{
		if( m_Value != (int)Value )
		{
			m_Date	= API_Double2DateStr(Value);
			m_Value	= (int)Value;

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool				Set_NoData		(void)			{	return( Set_Value(NODATA_INT) );	}
	virtual bool				is_NoData		(void)			{	return( m_Value == NODATA_INT );	}

	//-----------------------------------------------------
	virtual const char *		asString		(int Decimals)	{	return( m_Date );	}
	virtual int					asInt			(void)			{	return( m_Value );	}
	virtual double				asDouble		(void)			{	return( m_Value );	}


private:

	int							m_Value;

	CAPI_String					m_Date;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Value_Int : public CTable_Value
{
public:
	CTable_Value_Int(void)	{	m_Value	= 0;	}
	virtual ~CTable_Value_Int(void) {}

	//-----------------------------------------------------
	virtual bool				Set_Value		(const char *Value)
	{
		double		d;
		CAPI_String	s(Value);

		return( s.asDouble(d) ? Set_Value(d) : false );
	}

	//-----------------------------------------------------
	virtual bool				Set_Value		(double      Value)
	{
		if( m_Value != Value )
		{
			m_Value	= (int)Value;

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool				Set_NoData		(void)			{	return( Set_Value(NODATA_INT) );	}
	virtual bool				is_NoData		(void)			{	return( m_Value == NODATA_INT );	}

	//-----------------------------------------------------
	virtual const char *		asString		(int Decimals)
	{
		static CAPI_String	s;

		s.Printf("%d", m_Value);

		return( s.c_str() );
	}

	//-----------------------------------------------------
	virtual int					asInt			(void)			{	return( m_Value );	}
	virtual double				asDouble		(void)			{	return( m_Value );	}


private:

	int							m_Value;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTable_Value_Double : public CTable_Value
{
public:
	CTable_Value_Double(void) {	m_Value	= 0.0;	}
	virtual ~CTable_Value_Double(void) {}

	//-----------------------------------------------------
	virtual bool				Set_Value		(const char *Value)
	{
		double		d;
		CAPI_String	s(Value);

		return( s.asDouble(d) ? Set_Value(d) : false );
	}

	//-----------------------------------------------------
	virtual bool				Set_Value		(double      Value)
	{
		if( m_Value != Value )
		{
			m_Value	= Value;

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool				Set_NoData		(void)			{	return( Set_Value(NODATA_DOUBLE) );	}
	virtual bool				is_NoData		(void)			{	return( m_Value == NODATA_DOUBLE );	}

	//-----------------------------------------------------
	virtual const char *		asString		(int Decimals)
	{
		static CAPI_String	s;

		if( Decimals > 0 )
		{
			s.Printf("%.*f", Decimals, m_Value);
		}
		else
		{
			s.Printf("%f", m_Value);
		}

		return( s.c_str() );
	}

	//-----------------------------------------------------
	virtual int					asInt			(void)			{	return( (int)m_Value );	}
	virtual double				asDouble		(void)			{	return( m_Value );		}


private:

	double						m_Value;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__table_value_H
