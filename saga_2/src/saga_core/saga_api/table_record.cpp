
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
CSG_Table_Record::CSG_Table_Record(CSG_Table *pTable, int Index)
{
	m_pTable	= pTable;
	m_Index		= Index;
	m_bSelected	= false;

	if( m_pTable && m_pTable->Get_Field_Count() > 0 )
	{
		m_Values	= (CSG_Table_Value **)SG_Malloc(m_pTable->Get_Field_Count() * sizeof(CSG_Table_Value *));

		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			m_Values[iField]	= _Create_Value(m_pTable->Get_Field_Type(iField));
		}
	}
	else
	{
		m_Values	= NULL;
	}
}

//---------------------------------------------------------
CSG_Table_Record::~CSG_Table_Record(void)
{
	if( m_bSelected )
	{
		m_pTable->Select(m_Index, true);
	}

	if( m_pTable->Get_Field_Count() > 0 )
	{
		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			delete(m_Values[iField]);
		}

		SG_Free(m_Values);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table_Value * CSG_Table_Record::_Create_Value(TSG_Table_Field_Type Type)
{
	switch( Type )
	{
	default:
	case TABLE_FIELDTYPE_String:	return( new CSG_Table_Value_String() );

	case TABLE_FIELDTYPE_Date:		return( new CSG_Table_Value_Date() );

	case TABLE_FIELDTYPE_Color:
	case TABLE_FIELDTYPE_Char:
	case TABLE_FIELDTYPE_Short:
	case TABLE_FIELDTYPE_Int:
	case TABLE_FIELDTYPE_Long:		return( new CSG_Table_Value_Int() );

	case TABLE_FIELDTYPE_Float:
	case TABLE_FIELDTYPE_Double:	return( new CSG_Table_Value_Double() );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_Record::_Add_Field(int add_Field)
{
	if( add_Field < 0 )
	{
		add_Field	= 0;
	}
	else if( add_Field >= m_pTable->Get_Field_Count() )
	{
		add_Field	= m_pTable->Get_Field_Count() - 1;
	}

	m_Values	= (CSG_Table_Value **)SG_Realloc(m_Values, m_pTable->Get_Field_Count() * sizeof(CSG_Table_Value *));

	for(int iField=m_pTable->Get_Field_Count()-1; iField>add_Field; iField--)
	{
		m_Values[iField]	= m_Values[iField - 1];
	}

	m_Values[add_Field]	= _Create_Value(m_pTable->Get_Field_Type(add_Field));

	return( true );
}

//---------------------------------------------------------
bool CSG_Table_Record::_Del_Field(int del_Field)
{
	delete(m_Values[del_Field]);

	for(int iField=del_Field; iField<m_pTable->Get_Field_Count(); iField++)
	{
		m_Values[iField]	= m_Values[iField + 1];
	}

	m_Values	= (CSG_Table_Value **)SG_Realloc(m_Values, m_pTable->Get_Field_Count() * sizeof(CSG_Table_Value *));

	return( true );
}

//---------------------------------------------------------
int CSG_Table_Record::_Get_Field(const SG_Char *Field) const
{
	if( Field && *Field )
	{
		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if( !SG_STR_CMP(Field, m_pTable->Get_Field_Name(iField)) )
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
bool CSG_Table_Record::Set_Value(int iField, const SG_Char *Value)
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		if( m_Values[iField]->Set_Value(Value) )
		{
			m_pTable->Set_Modified();
			m_pTable->Set_Update_Flag();
			m_pTable->_Stats_Invalidate(iField);

			return( true );
		}
	}

	return( false );
}

bool CSG_Table_Record::Set_Value(const SG_Char *Field, const SG_Char *Value)
{
	return( Set_Value(_Get_Field(Field), Value) );
}

//---------------------------------------------------------
bool CSG_Table_Record::Set_Value(int iField, double Value)
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		if( m_Values[iField]->Set_Value(Value) )
		{
			m_pTable->Set_Modified();
			m_pTable->Set_Update_Flag();
			m_pTable->_Stats_Invalidate(iField);

			return( true );
		}
	}

	return( false );
}

bool CSG_Table_Record::Set_Value(const SG_Char *Field, double Value)
{
	return( Set_Value(_Get_Field(Field), Value) );
}

//---------------------------------------------------------
bool CSG_Table_Record::Add_Value(int iField, double Value)
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		return( Set_Value(iField, asDouble(iField) + Value) );
	}

	return( false );
}

bool CSG_Table_Record::Add_Value(const SG_Char *Field, double Value)
{
	return( Add_Value(_Get_Field(Field), Value) );
}

//---------------------------------------------------------
bool CSG_Table_Record::Mul_Value(int iField, double Value)
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		return( Set_Value(iField, asDouble(iField) * Value) );
	}

	return( false );
}

bool CSG_Table_Record::Mul_Value(const SG_Char *Field, double Value)
{
	return( Mul_Value(_Get_Field(Field), Value) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_Record::Set_NoData(int iField)
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		if( m_Values[iField]->Set_NoData() )
		{
			m_pTable->Set_Modified();
			m_pTable->Set_Update_Flag();
			m_pTable->_Stats_Invalidate(iField);

			return( true );
		}
	}

	return( false );
}

bool CSG_Table_Record::Set_NoData(const SG_Char *Field)
{
	return( Set_NoData(_Get_Field(Field)) );
}

//---------------------------------------------------------
bool CSG_Table_Record::is_NoData(int iField) const
{
	return( iField >= 0 && iField < m_pTable->Get_Field_Count() ? m_Values[iField]->is_NoData() : true );
}

bool CSG_Table_Record::is_NoData(const SG_Char *Field) const
{
	return( is_NoData(_Get_Field(Field)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_Table_Record::asString(int iField, int Decimals) const
{
	return( iField >= 0 && iField < m_pTable->Get_Field_Count() ? m_Values[iField]->asString(Decimals) : NULL );
}

const SG_Char * CSG_Table_Record::asString(const SG_Char *Field, int Decimals) const
{
	return( asString(_Get_Field(Field), Decimals) );
}

//---------------------------------------------------------
int CSG_Table_Record::asInt(int iField) const
{
	return( iField >= 0 && iField < m_pTable->Get_Field_Count() ? m_Values[iField]->asInt() : 0 );
}

int CSG_Table_Record::asInt(const SG_Char *Field) const
{
	return( asInt(_Get_Field(Field)) );
}

//---------------------------------------------------------
double CSG_Table_Record::asDouble(int iField) const
{
	return( iField >= 0 && iField < m_pTable->Get_Field_Count() ? m_Values[iField]->asDouble() : 0.0 );
}

double CSG_Table_Record::asDouble(const SG_Char *Field) const
{
	return( asDouble(_Get_Field(Field)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_Record::Assign(CSG_Table_Record *pRecord)
{
	if( pRecord )
	{
		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			Set_Value(iField, pRecord->asString(iField));
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
