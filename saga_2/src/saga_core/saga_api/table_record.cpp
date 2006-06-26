
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
//                   table_record.cpp                    //
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
#include <string.h>

#include "table.h"
#include "table_value.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Record::CTable_Record(CTable *pOwner, int Index)
{
	m_pOwner	= pOwner;
	m_Index		= Index;
	m_bSelected	= false;

	if( m_pOwner && m_pOwner->Get_Field_Count() > 0 )
	{
		m_Values	= (CTable_Value **)API_Malloc(m_pOwner->Get_Field_Count() * sizeof(CTable_Value *));

		for(int iField=0; iField<m_pOwner->Get_Field_Count(); iField++)
		{
			m_Values[iField]	= _Create_Value(m_pOwner->Get_Field_Type(iField));
		}
	}
	else
	{
		m_Values	= NULL;
	}
}

//---------------------------------------------------------
CTable_Record::~CTable_Record(void)
{
	if( m_pOwner->Get_Field_Count() > 0 )
	{
		for(int iField=0; iField<m_pOwner->Get_Field_Count(); iField++)
		{
			delete(m_Values[iField]);
		}

		API_Free(m_Values);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Value * CTable_Record::_Create_Value(TTable_FieldType Type)
{
	switch( Type )
	{
	default:
	case TABLE_FIELDTYPE_String:	return( new CTable_Value_String() );

	case TABLE_FIELDTYPE_Date:		return( new CTable_Value_Date() );

	case TABLE_FIELDTYPE_Color:
	case TABLE_FIELDTYPE_Char:
	case TABLE_FIELDTYPE_Short:
	case TABLE_FIELDTYPE_Int:
	case TABLE_FIELDTYPE_Long:		return( new CTable_Value_Int() );

	case TABLE_FIELDTYPE_Float:
	case TABLE_FIELDTYPE_Double:	return( new CTable_Value_Double() );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Record::_Add_Field(int add_Field)
{
	if( add_Field < 0 )
	{
		add_Field	= 0;
	}
	else if( add_Field >= m_pOwner->Get_Field_Count() )
	{
		add_Field	= m_pOwner->Get_Field_Count() - 1;
	}

	m_Values	= (CTable_Value **)API_Realloc(m_Values, m_pOwner->Get_Field_Count() * sizeof(CTable_Value *));

	for(int iField=m_pOwner->Get_Field_Count()-1; iField>add_Field; iField--)
	{
		m_Values[iField]	= m_Values[iField - 1];
	}

	m_Values[add_Field]	= _Create_Value(m_pOwner->Get_Field_Type(add_Field));

	return( true );
}

//---------------------------------------------------------
bool CTable_Record::_Del_Field(int del_Field)
{
	delete(m_Values[del_Field]);

	for(int iField=del_Field; iField<m_pOwner->Get_Field_Count(); iField++)
	{
		m_Values[iField]	= m_Values[iField + 1];
	}

	m_Values	= (CTable_Value **)API_Realloc(m_Values, m_pOwner->Get_Field_Count() * sizeof(CTable_Value *));

	return( true );
}

//---------------------------------------------------------
int CTable_Record::_Get_Field(const char *Field)
{
	if( Field && strlen(Field) > 0 )
	{
		for(int iField=0; iField<m_pOwner->Get_Field_Count(); iField++)
		{
			if( !strcmp(Field, m_pOwner->Get_Field_Name(iField)) )
			{
				return( iField );
			}
		}
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Record::Set_Value(int iField, const char *Value)
{
	if( iField >= 0 && iField < m_pOwner->Get_Field_Count() )
	{
		if( m_Values[iField]->Set_Value(Value) )
		{
			m_pOwner->Set_Modified();
			m_pOwner->_Range_Invalidate(iField);

			return( true );
		}
	}

	return( false );
}

bool CTable_Record::Set_Value(const char *Field, const char *Value)
{
	return( Set_Value(_Get_Field(Field), Value) );
}

//---------------------------------------------------------
bool CTable_Record::Set_Value(int iField, double Value)
{
	if( iField >= 0 && iField < m_pOwner->Get_Field_Count() )
	{
		if( m_Values[iField]->Set_Value(Value) )
		{
			m_pOwner->Set_Modified();
			m_pOwner->_Range_Invalidate(iField);

			return( true );
		}
	}

	return( false );
}

bool CTable_Record::Set_Value(const char *Field, double Value)
{
	return( Set_Value(_Get_Field(Field), Value) );
}

//---------------------------------------------------------
bool CTable_Record::Add_Value(int iField, double Value)
{
	if( iField >= 0 && iField < m_pOwner->Get_Field_Count() )
	{
		return( Set_Value(iField, asDouble(iField) + Value) );
	}

	return( false );
}

bool CTable_Record::Add_Value(const char *Field, double Value)
{
	return( Add_Value(_Get_Field(Field), Value) );
}

//---------------------------------------------------------
bool CTable_Record::Mul_Value(int iField, double Value)
{
	if( iField >= 0 && iField < m_pOwner->Get_Field_Count() )
	{
		return( Set_Value(iField, asDouble(iField) * Value) );
	}

	return( false );
}

bool CTable_Record::Mul_Value(const char *Field, double Value)
{
	return( Mul_Value(_Get_Field(Field), Value) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Record::Set_NoData(int iField)
{
	if( iField >= 0 && iField < m_pOwner->Get_Field_Count() )
	{
		if( m_Values[iField]->Set_NoData() )
		{
			m_pOwner->Set_Modified();
			m_pOwner->_Range_Invalidate(iField);

			return( true );
		}
	}

	return( false );
}

bool CTable_Record::Set_NoData(const char *Field)
{
	return( Set_NoData(_Get_Field(Field)) );
}

//---------------------------------------------------------
bool CTable_Record::is_NoData(int iField)
{
	return( iField >= 0 && iField < m_pOwner->Get_Field_Count() ? m_Values[iField]->is_NoData() : true );
}

bool CTable_Record::is_NoData(const char *Field)
{
	return( is_NoData(_Get_Field(Field)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char * CTable_Record::asString(int iField, int Decimals)
{
	return( iField >= 0 && iField < m_pOwner->Get_Field_Count() ? m_Values[iField]->asString(Decimals) : NULL );
}

const char * CTable_Record::asString(const char *Field, int Decimals)
{
	return( asString(_Get_Field(Field), Decimals) );
}

//---------------------------------------------------------
int CTable_Record::asInt(int iField)
{
	return( iField >= 0 && iField < m_pOwner->Get_Field_Count() ? m_Values[iField]->asInt() : 0 );
}

int CTable_Record::asInt(const char *Field)
{
	return( asInt(_Get_Field(Field)) );
}

//---------------------------------------------------------
double CTable_Record::asDouble(int iField)
{
	return( iField >= 0 && iField < m_pOwner->Get_Field_Count() ? m_Values[iField]->asDouble() : 0.0 );
}

double CTable_Record::asDouble(const char *Field)
{
	return( asDouble(_Get_Field(Field)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTable_Record::Assign(CTable_Record *pSource)
{
	for(int iField=0; iField<m_pOwner->Get_Field_Count(); iField++)
	{
		Set_Value(iField, pSource->asString(iField) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
