
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
#define NODATA_STRING	SG_T("__NO_DATA__")
#define NODATA_INT		-2147483647
#define NODATA_DOUBLE	2.2204460492503131e-016


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value
{
public:
	CSG_Table_Value(void)	{}
	virtual ~CSG_Table_Value(void)	{}

	virtual bool				Set_Value		(const SG_Char *Value)	= 0;
	virtual bool				Set_Value		(double      Value)	= 0;

	virtual bool				Set_NoData		(void)				= 0;
	virtual bool				is_NoData		(void)				= 0;

	virtual const SG_Char *		asString		(int Decimals)		= 0;
	virtual int					asInt			(void)				= 0;
	virtual double				asDouble		(void)				= 0;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value_String : public CSG_Table_Value
{
public:
	CSG_Table_Value_String(void) {}
	virtual ~CSG_Table_Value_String(void) {}

	//-----------------------------------------------------
	virtual bool				Set_Value		(const SG_Char *Value)
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
		CSG_String	s;

		s.Printf(SG_T("%f"), Value);

		return( Set_Value(s.c_str()) );
	}

	//-----------------------------------------------------
	virtual bool				Set_NoData		(void)			{	return( Set_Value(NODATA_STRING) );			}
	virtual bool				is_NoData		(void)			{	return( m_Value.Cmp(NODATA_STRING) == 0 );	}

	//-----------------------------------------------------
	virtual const SG_Char *		asString		(int Decimals)	{	return( m_Value );	}
	virtual int					asInt			(void)			{	return( m_Value.asInt() );	}
	virtual double				asDouble		(void)			{	return( m_Value.asDouble() );	}


private:

	CSG_String					m_Value;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value_Date : public CSG_Table_Value
{
public:
	CSG_Table_Value_Date(void) {	Set_Value(SG_T("0"));	}
	virtual ~CSG_Table_Value_Date(void) {}

	//-----------------------------------------------------
	virtual bool				Set_Value		(const SG_Char *Value)
	{
		return( Set_Value(SG_Date_To_Double(Value)) );
	}

	//-----------------------------------------------------
	virtual bool				Set_Value		(double      Value)
	{
		if( m_Value != (int)Value )
		{
			m_Date	= SG_Double_To_Date(Value);
			m_Value	= (int)Value;

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool				Set_NoData		(void)			{	return( Set_Value(NODATA_INT) );	}
	virtual bool				is_NoData		(void)			{	return( m_Value == NODATA_INT );	}

	//-----------------------------------------------------
	virtual const SG_Char *		asString		(int Decimals)	{	return( m_Date );	}
	virtual int					asInt			(void)			{	return( m_Value );	}
	virtual double				asDouble		(void)			{	return( m_Value );	}


private:

	int							m_Value;

	CSG_String					m_Date;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Value_Int : public CSG_Table_Value
{
public:
	CSG_Table_Value_Int(void)	{	m_Value	= 0;	}
	virtual ~CSG_Table_Value_Int(void) {}

	//-----------------------------------------------------
	virtual bool				Set_Value		(const SG_Char *Value)
	{
		double		d;
		CSG_String	s(Value);

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
	virtual const SG_Char *		asString		(int Decimals)
	{
		static CSG_String	s;

		s.Printf(SG_T("%d"), m_Value);

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
class CSG_Table_Value_Double : public CSG_Table_Value
{
public:
	CSG_Table_Value_Double(void) {	m_Value	= 0.0;	}
	virtual ~CSG_Table_Value_Double(void) {}

	//-----------------------------------------------------
	virtual bool				Set_Value		(const SG_Char *Value)
	{
		double		d;
		CSG_String	s(Value);

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
	virtual const SG_Char *		asString		(int Decimals)
	{
		static CSG_String	s;

		if( Decimals > 0 )
		{
			s.Printf(SG_T("%.*f"), Decimals, m_Value);
		}
		else
		{
			s.Printf(SG_T("%f"), m_Value);
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
