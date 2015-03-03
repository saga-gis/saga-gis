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
	m_Flags		= 0;

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
	if( is_Selected() )
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
CSG_Table_Value * CSG_Table_Record::_Create_Value(TSG_Data_Type Type)
{
	switch( Type )
	{
	default:
	case SG_DATATYPE_String:	return( new CSG_Table_Value_String() );

	case SG_DATATYPE_Date  :	return( new CSG_Table_Value_Date  () );

	case SG_DATATYPE_Color :
	case SG_DATATYPE_Byte  :
	case SG_DATATYPE_Char  :
	case SG_DATATYPE_Word  :
	case SG_DATATYPE_Short :
	case SG_DATATYPE_DWord :
	case SG_DATATYPE_Int   :	return( new CSG_Table_Value_Int   () );

	case SG_DATATYPE_ULong :
	case SG_DATATYPE_Long  :	return( new CSG_Table_Value_Long  () );

	case SG_DATATYPE_Float :
	case SG_DATATYPE_Double:	return( new CSG_Table_Value_Double() );

	case SG_DATATYPE_Binary:	return( new CSG_Table_Value_Binary() );
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
int CSG_Table_Record::_Get_Field(const CSG_String &Field) const
{
	if( Field.Length() )
	{
		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if( !Field.Cmp(m_pTable->Get_Field_Name(iField)) )
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
void CSG_Table_Record::Set_Selected(bool bOn)
{
	if( bOn != is_Selected() )
	{
		if( bOn )
		{
			m_Flags	|=  SG_TABLE_REC_FLAG_Selected;
		}
		else
		{
			m_Flags	&= ~SG_TABLE_REC_FLAG_Selected;
		}
	}
}

//---------------------------------------------------------
void CSG_Table_Record::Set_Modified(bool bOn)
{
	if( bOn != is_Modified() )
	{
		if( bOn )
		{
			m_Flags	|=  SG_TABLE_REC_FLAG_Modified;
		}
		else
		{
			m_Flags	&= ~SG_TABLE_REC_FLAG_Modified;
		}
	}

	if( bOn )
	{
		m_pTable->Set_Modified();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_Record::Set_Value		(int           iField, const CSG_Bytes &Value)
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		if( m_Values[iField]->Set_Value(Value) )
		{
			Set_Modified(true);

			m_pTable->Set_Update_Flag();
			m_pTable->_Stats_Invalidate(iField);

			return( true );
		}
	}

	return( false );
}

bool CSG_Table_Record::Set_Value(const CSG_String &Field, const CSG_Bytes &Value)
{
	return( Set_Value(_Get_Field(Field), Value) );
}

//---------------------------------------------------------
bool CSG_Table_Record::Set_Value(int iField, const CSG_String &Value)
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		if( m_Values[iField]->Set_Value(Value) )
		{
			Set_Modified(true);

			m_pTable->Set_Update_Flag();
			m_pTable->_Stats_Invalidate(iField);

			return( true );
		}
	}

	return( false );
}

bool CSG_Table_Record::Set_Value(const CSG_String &Field, const CSG_String &Value)
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
			Set_Modified(true);

			m_pTable->Set_Update_Flag();
			m_pTable->_Stats_Invalidate(iField);

			return( true );
		}
	}

	return( false );
}

bool CSG_Table_Record::Set_Value(const CSG_String &Field, double Value)
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

bool CSG_Table_Record::Add_Value(const CSG_String &Field, double Value)
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

bool CSG_Table_Record::Mul_Value(const CSG_String &Field, double Value)
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
		switch( m_pTable->Get_Field_Type(iField) )
		{
		default:
		case SG_DATATYPE_String:
			if( !m_Values[iField]->Set_Value(SG_T("")) )
				return( false );
			break;

		case SG_DATATYPE_Date:
		case SG_DATATYPE_Color:
		case SG_DATATYPE_Byte:
		case SG_DATATYPE_Char:
		case SG_DATATYPE_Word:
		case SG_DATATYPE_Short:
		case SG_DATATYPE_DWord:
		case SG_DATATYPE_Int:
		case SG_DATATYPE_ULong:
		case SG_DATATYPE_Long:
		case SG_DATATYPE_Float:
		case SG_DATATYPE_Double:
			if( !m_Values[iField]->Set_Value(m_pTable->Get_NoData_Value()) )
				return( false );
			break;

		case SG_DATATYPE_Binary:
			m_Values[iField]->asBinary().Destroy();
			break;
		}

		Set_Modified(true);

		m_pTable->Set_Update_Flag();
		m_pTable->_Stats_Invalidate(iField);

		return( true );
	}

	return( false );
}

bool CSG_Table_Record::Set_NoData(const CSG_String &Field)
{
	return( Set_NoData(_Get_Field(Field)) );
}

//---------------------------------------------------------
bool CSG_Table_Record::is_NoData(int iField) const
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		switch( m_pTable->Get_Field_Type(iField) )
		{
		default:
		case SG_DATATYPE_String:
			return( m_Values[iField]->asString() == NULL );

		case SG_DATATYPE_Date:
		case SG_DATATYPE_Color:
		case SG_DATATYPE_Byte:
		case SG_DATATYPE_Char:
		case SG_DATATYPE_Word:
		case SG_DATATYPE_Short:
		case SG_DATATYPE_DWord:
		case SG_DATATYPE_Int:
		case SG_DATATYPE_ULong:
		case SG_DATATYPE_Long:
			return( m_pTable->is_NoData_Value(m_Values[iField]->asInt()) );

		case SG_DATATYPE_Float:
		case SG_DATATYPE_Double:
			return( m_pTable->is_NoData_Value(m_Values[iField]->asDouble()) );

		case SG_DATATYPE_Binary:
			return( m_Values[iField]->asBinary().Get_Count() == 0 );
		}
	}

	return( true );
}

bool CSG_Table_Record::is_NoData(const CSG_String &Field) const
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

const SG_Char * CSG_Table_Record::asString(const CSG_String &Field, int Decimals) const
{
	return( asString(_Get_Field(Field), Decimals) );
}

//---------------------------------------------------------
int CSG_Table_Record::asInt(int iField) const
{
	return( iField >= 0 && iField < m_pTable->Get_Field_Count() ? m_Values[iField]->asInt() : 0 );
}

int CSG_Table_Record::asInt(const CSG_String &Field) const
{
	return( asInt(_Get_Field(Field)) );
}

//---------------------------------------------------------
sLong CSG_Table_Record::asLong(int iField) const
{
	return( iField >= 0 && iField < m_pTable->Get_Field_Count() ? m_Values[iField]->asLong() : 0 );
}

sLong CSG_Table_Record::asLong(const CSG_String &Field) const
{
	return( asLong(_Get_Field(Field)) );
}

//---------------------------------------------------------
double CSG_Table_Record::asDouble(int iField) const
{
	return( iField >= 0 && iField < m_pTable->Get_Field_Count() ? m_Values[iField]->asDouble() : 0.0 );
}

double CSG_Table_Record::asDouble(const CSG_String &Field) const
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
		int		nFields	= m_pTable->Get_Field_Count() < pRecord->m_pTable->Get_Field_Count()
						? m_pTable->Get_Field_Count() : pRecord->m_pTable->Get_Field_Count();

		for(int iField=0; iField<nFields; iField++)
		{
			*(m_Values[iField])	= *(pRecord->m_Values[iField]);
		}

		Set_Modified();

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
