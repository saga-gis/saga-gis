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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
typedef enum ESG_Table_Value_Type
{
	SG_TABLE_VALUE_TYPE_Binary,
	SG_TABLE_VALUE_TYPE_String,
	SG_TABLE_VALUE_TYPE_Date,
	SG_TABLE_VALUE_TYPE_Int,
	SG_TABLE_VALUE_TYPE_Long,
	SG_TABLE_VALUE_TYPE_Double
}
TSG_Table_Value_Type;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value
{
public:
	CSG_Table_Value(void)			{}
	virtual ~CSG_Table_Value(void)	{}

	virtual TSG_Table_Value_Type	Get_Type		(void)				const	= 0;

	//-----------------------------------------------------
	virtual bool					Set_Value		(const CSG_Bytes &Value)	= 0;
	virtual bool					Set_Value		(const SG_Char   *Value)	= 0;
	virtual bool					Set_Value		(int              Value)	= 0;
	virtual bool					Set_Value		(sLong            Value)	= 0;
	virtual bool					Set_Value		(double           Value)	= 0;

	//-----------------------------------------------------
	virtual CSG_Bytes				asBinary		(void)				const
	{
		const SG_Char *s	= asString();

		return( CSG_Bytes((BYTE *)s, (int)(s && *s ? SG_STR_LEN(s) : 0) * sizeof(SG_Char)) );
	}

	virtual const SG_Char *			asString		(int Decimals = 0)	const	= 0;
	virtual int						asInt			(void)				const	= 0;
	virtual sLong					asLong			(void)				const	= 0;
	virtual double					asDouble		(void)				const	= 0;

	//-----------------------------------------------------
	virtual bool					is_Equal		(const CSG_Table_Value &Value)	const	= 0;
	bool							operator ==		(const CSG_Table_Value &Value)	const	{	return( is_Equal(Value) );	}

	//-----------------------------------------------------
	operator const SG_Char *						(void)				const	{	return( asString() );	}
	operator double									(void)				const	{	return( asDouble() );	}

	//-----------------------------------------------------
	virtual CSG_Table_Value &		operator = (const SG_Char         *Value)	{	Set_Value(Value);	return( *this );	}
	virtual CSG_Table_Value &		operator = (double                 Value)	{	Set_Value(Value);	return( *this );	}
	virtual CSG_Table_Value &		operator = (const CSG_Table_Value &Value)	= 0;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value_Binary : public CSG_Table_Value
{
public:
	CSG_Table_Value_Binary(void) {}
	virtual ~CSG_Table_Value_Binary(void) {}

	virtual TSG_Table_Value_Type	Get_Type		(void)				const	{	return( SG_TABLE_VALUE_TYPE_Binary );	}

	//-----------------------------------------------------
	virtual bool					Set_Value		(const CSG_Bytes &Value)
	{
		return( m_Value.Create(Value) );
	}

	virtual bool					Set_Value		(const SG_Char   *Value)
	{
		return( m_Value.Create((BYTE *)Value, (int)((Value && *Value ? SG_STR_LEN(Value) : 0 * sizeof(SG_Char)))) );
	}

	virtual bool					Set_Value		(int              Value)
	{
		return( m_Value.Create((BYTE *)&Value, sizeof(Value)) );
	}

	virtual bool					Set_Value		(sLong            Value)
	{
		return( m_Value.Create((BYTE *)&Value, sizeof(Value)) );
	}

	virtual bool					Set_Value		(double           Value)
	{
		return( m_Value.Create((BYTE *)&Value, sizeof(Value)) );
	}

	//-----------------------------------------------------
	virtual bool					Set_NoData		(void)						{	return( m_Value.Destroy() );		}
	virtual bool					is_NoData		(void)				const	{	return( m_Value.Get_Count() <= 0 );	}

	//-----------------------------------------------------
	virtual CSG_Bytes				asBinary		(void)				const	{	return( m_Value );					}
	virtual const SG_Char *			asString		(int Decimals = 0)	const	{	return( (const SG_Char *)m_Value.Get_Bytes() );	}
	virtual int						asInt			(void)				const	{	return( m_Value.Get_Count() );		}
	virtual sLong					asLong			(void)				const	{	return( m_Value.Get_Count() );		}
	virtual double					asDouble		(void)				const	{	return( 0.0 );	}

	//-----------------------------------------------------
	virtual bool					is_Equal		(const CSG_Table_Value &Value)	const
	{
		return( !SG_STR_CMP(asString(), Value.asString()) );
	}

	//-----------------------------------------------------
	virtual CSG_Table_Value &		operator = (const CSG_Table_Value &Value)	{	Set_Value(Value.asBinary());	return( *this );	}

	//-----------------------------------------------------
	CSG_Bytes &						Get_Binary		(void)						{	return( m_Value );	}


private:

	CSG_Bytes						m_Value;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value_String : public CSG_Table_Value
{
public:
	CSG_Table_Value_String(void) {}
	virtual ~CSG_Table_Value_String(void) {}

	virtual TSG_Table_Value_Type	Get_Type		(void)				const	{	return( SG_TABLE_VALUE_TYPE_String );	}

	//-----------------------------------------------------
	virtual bool					Set_Value		(const CSG_Bytes &Value)
	{
		return( Set_Value((SG_Char *)Value.Get_Bytes()) );
	}

	virtual bool					Set_Value		(const SG_Char   *Value)
	{
		if( Value && m_Value.Cmp(Value) )
		{
			m_Value	= Value;

			return( true );
		}

		return( false );
	}

	virtual bool					Set_Value		(int              Value)
	{
		return( Set_Value(CSG_String::Format(SG_T("%d"), Value).c_str()) );
	}

	virtual bool					Set_Value		(sLong            Value)
	{
		return( Set_Value(CSG_String::Format(SG_T("%ld"), Value).c_str()) );
	}

	virtual bool					Set_Value		(double           Value)
	{
		return( Set_Value(CSG_String::Format(SG_T("%f"), Value).c_str()) );
	}

	//-----------------------------------------------------
	virtual const SG_Char *			asString		(int Decimals = 0)	const	{	return( m_Value );							}
	virtual int						asInt			(void)				const	{	return( m_Value.asInt() );					}
	virtual sLong					asLong			(void)				const	{	return( m_Value.asInt() );					}
	virtual double					asDouble		(void)				const	{	return( m_Value.asDouble() );				}

	//-----------------------------------------------------
	virtual bool					is_Equal		(const CSG_Table_Value &Value)	const
	{
		return( !m_Value.Cmp(Value.asString()) );
	}

	//-----------------------------------------------------
	virtual CSG_Table_Value &		operator = (const CSG_Table_Value &Value)	{	Set_Value(Value.asString());	return( *this );	}


private:

	CSG_String						m_Value;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value_Date : public CSG_Table_Value
{
public:
	CSG_Table_Value_Date(void) {	Set_Value(SG_T("0"));	}
	virtual ~CSG_Table_Value_Date(void) {}

	virtual TSG_Table_Value_Type	Get_Type		(void)				const	{	return( SG_TABLE_VALUE_TYPE_Date );	}

	//-----------------------------------------------------
	virtual bool					Set_Value		(const CSG_Bytes &Value)
	{
		return( Set_Value((SG_Char *)Value.Get_Bytes()) );
	}

	virtual bool					Set_Value		(const SG_Char   *Value)
	{
		return( Set_Value(SG_Date_To_Number(Value)) );
	}

	virtual bool					Set_Value		(int              Value)
	{
		if( m_Value != Value )
		{
			m_Date	= SG_Number_To_Date(Value);
			m_Value	= Value;

			return( true );
		}

		return( false );
	}

	virtual bool					Set_Value		(sLong            Value)
	{
		return( Set_Value((int)Value) );
	}

	virtual bool					Set_Value		(double           Value)
	{
		return( Set_Value((int)Value) );
	}

	//-----------------------------------------------------
	virtual const SG_Char *			asString		(int Decimals = 0)	const	{	return( m_Date );	}
	virtual int						asInt			(void)				const	{	return( m_Value );	}
	virtual sLong					asLong			(void)				const	{	return( m_Value );	}
	virtual double					asDouble		(void)				const	{	return( m_Value );	}

	//-----------------------------------------------------
	virtual bool					is_Equal		(const CSG_Table_Value &Value)	const
	{
		return( m_Value	== Value.asInt() );
	}

	//-----------------------------------------------------
	virtual CSG_Table_Value &		operator = (const CSG_Table_Value &Value)	{	Set_Value(Value.asString());	return( *this );	}


private:

	int								m_Value;

	CSG_String						m_Date;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value_Int : public CSG_Table_Value
{
public:
	CSG_Table_Value_Int(void)	{	m_Value	= 0;	}
	virtual ~CSG_Table_Value_Int(void) {}

	virtual TSG_Table_Value_Type	Get_Type		(void)				const	{	return( SG_TABLE_VALUE_TYPE_Int );	}

	//-----------------------------------------------------
	virtual bool					Set_Value		(const CSG_Bytes &Value)
	{
		return( Set_Value((SG_Char *)Value.Get_Bytes()) );
	}

	virtual bool					Set_Value		(const SG_Char   *Value)
	{
		int			i;
		CSG_String	s(Value);

		return( s.asInt(i) ? Set_Value(i) : false );
	}

	virtual bool					Set_Value		(int              Value)
	{
		if( m_Value != Value )
		{
			m_Value	= Value;

			return( true );
		}

		return( false );
	}

	virtual bool					Set_Value		(sLong            Value)
	{
		return( Set_Value((int)Value) );
	}

	virtual bool					Set_Value		(double           Value)
	{
		return( Set_Value((int)Value) );
	}

	//-----------------------------------------------------
	virtual const SG_Char *			asString		(int Decimals = 0)	const
	{
		static CSG_String	s;

		s.Printf(SG_T("%d"), m_Value);

		return( s.c_str() );
	}

	virtual int						asInt			(void)				const	{	return( m_Value );	}
	virtual sLong					asLong			(void)				const	{	return( m_Value );	}
	virtual double					asDouble		(void)				const	{	return( m_Value );	}

	//-----------------------------------------------------
	virtual bool					is_Equal		(const CSG_Table_Value &Value)	const
	{
		return( m_Value	== Value.asInt() );
	}

	//-----------------------------------------------------
	virtual CSG_Table_Value &		operator = (const CSG_Table_Value &Value)	{	Set_Value(Value.asInt());	return( *this );	}


private:

	int								m_Value;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value_Long : public CSG_Table_Value
{
public:
	CSG_Table_Value_Long(void)	{	m_Value	= 0;	}
	virtual ~CSG_Table_Value_Long(void) {}

	virtual TSG_Table_Value_Type	Get_Type		(void)				const	{	return( SG_TABLE_VALUE_TYPE_Long );	}

	//-----------------------------------------------------
	virtual bool					Set_Value		(const CSG_Bytes &Value)
	{
		return( Set_Value((SG_Char *)Value.Get_Bytes()) );
	}

	virtual bool					Set_Value		(const SG_Char   *Value)
	{
		int			i;
		CSG_String	s(Value);

		return( s.asInt(i) ? Set_Value(i) : false );
	}

	virtual bool					Set_Value		(int              Value)
	{
		return( Set_Value((sLong)Value) );
	}

	virtual bool					Set_Value		(sLong            Value)
	{
		if( m_Value != Value )
		{
			m_Value	= Value;

			return( true );
		}

		return( false );
	}

	virtual bool					Set_Value		(double           Value)
	{
		return( Set_Value((sLong)Value) );
	}

	//-----------------------------------------------------
	virtual const SG_Char *			asString		(int Decimals = 0)	const
	{
		static CSG_String	s;

		s.Printf(SG_T("%ld"), m_Value);

		return( s.c_str() );
	}

	virtual int						asInt			(void)				const	{	return(    (int)m_Value );	}
	virtual sLong					asLong			(void)				const	{	return(         m_Value );	}
	virtual double					asDouble		(void)				const	{	return( (double)m_Value );	}

	//-----------------------------------------------------
	virtual bool					is_Equal		(const CSG_Table_Value &Value)	const
	{
		return( m_Value	== Value.asLong() );
	}

	//-----------------------------------------------------
	virtual CSG_Table_Value &		operator = (const CSG_Table_Value &Value)	{	Set_Value(Value.asLong());	return( *this );	}


private:

	sLong							m_Value;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value_Double : public CSG_Table_Value
{
public:
	CSG_Table_Value_Double(void) {	m_Value	= 0.0;	}
	virtual ~CSG_Table_Value_Double(void) {}

	virtual TSG_Table_Value_Type	Get_Type		(void)				const	{	return( SG_TABLE_VALUE_TYPE_Double );	}

	//-----------------------------------------------------
	virtual bool					Set_Value		(const CSG_Bytes &Value)
	{
		return( Set_Value((SG_Char *)Value.Get_Bytes()) );
	}

	virtual bool					Set_Value		(const SG_Char   *Value)
	{
		double		d;
		CSG_String	s(Value);

		return( s.asDouble(d) ? Set_Value(d) : false );
	}

	virtual bool					Set_Value		(int              Value)
	{
		return( Set_Value((double)Value) );
	}

	virtual bool					Set_Value		(sLong            Value)
	{
		return( Set_Value((double)Value) );
	}

	virtual bool					Set_Value		(double           Value)
	{
		if( m_Value != Value )
		{
			m_Value	= Value;

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual const SG_Char *			asString		(int Decimals = 0)	const
	{
		static CSG_String	s;

		s	= SG_Get_String(m_Value, Decimals, false);

		return( s.c_str() );
	}

	virtual int						asInt			(void)				const	{	return( (int  )m_Value );	}
	virtual sLong					asLong			(void)				const	{	return( (sLong)m_Value );	}
	virtual double					asDouble		(void)				const	{	return(        m_Value );	}

	//-----------------------------------------------------
	virtual bool					is_Equal		(const CSG_Table_Value &Value)	const
	{
		return( m_Value	== Value.asDouble() );
	}

	//-----------------------------------------------------
	virtual CSG_Table_Value &		operator = (const CSG_Table_Value &Value)	{	Set_Value(Value.asDouble());	return( *this );	}


private:

	double							m_Value;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__table_value_H
