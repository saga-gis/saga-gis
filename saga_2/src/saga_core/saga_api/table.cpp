
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
//                       table.cpp                       //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table * SG_Create_Table(void)
{
	return( new CSG_Table );
}

//---------------------------------------------------------
CSG_Table * SG_Create_Table(const CSG_Table &Table)
{
	return( new CSG_Table(Table) );
}

//---------------------------------------------------------
CSG_Table * SG_Create_Table(const SG_Char *FileName)
{
	return( new CSG_Table(FileName) );
}

//---------------------------------------------------------
CSG_Table * SG_Create_Table(CSG_Table *pStructure)
{
	return( new CSG_Table(pStructure) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table::CSG_Table(void)
	: CSG_Data_Object()
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_Table::CSG_Table(const CSG_Table &Table)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(Table);
}

//---------------------------------------------------------
CSG_Table::CSG_Table(const SG_Char *File_Name, SG_Char Separator)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(File_Name, Separator);
}

//---------------------------------------------------------
CSG_Table::CSG_Table(CSG_Table *pStructure)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(pStructure);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Table::_On_Construction(void)
{
	m_nFields		= 0;
	m_Field_Name	= NULL;
	m_Field_Type	= NULL;
	m_Field_Val_Min	= NULL;
	m_Field_Val_Max	= NULL;

	m_Records		= NULL;
	m_nRecords		= 0;
	m_nBuffer		= 0;

	m_nSelected		= 0;
	m_Selected		= NULL;

	m_Index			= NULL;
	m_Index_Field	= -1;
	m_Index_Order	= TABLE_INDEX_None;

	m_pOwner		= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Create(const CSG_Table &Table)
{
	return( is_Private() ? false : _Create(Table) );
}

bool CSG_Table::_Create(const CSG_Table &Table)
{
	return( Assign((CSG_Data_Object *)&Table) );
}

//---------------------------------------------------------
bool CSG_Table::Create(const SG_Char *File_Name, SG_Char Separator)
{
	return( is_Private() ? false : _Create(File_Name, Separator) );
}

bool CSG_Table::_Create(const SG_Char *File_Name, SG_Char Separator)
{
	return( _Load(File_Name, TABLE_FILETYPE_Undefined, Separator) );
}

//---------------------------------------------------------
bool CSG_Table::Create(CSG_Table *pStructure)
{
	return( is_Private() ? false : _Create(pStructure) );
}

bool CSG_Table::_Create(CSG_Table *pStructure)
{
	_Destroy();

	if( pStructure && pStructure->Get_Field_Count() > 0 )
	{
		for(int i=0; i<pStructure->Get_Field_Count(); i++)
		{
			Add_Field(pStructure->Get_Field_Name(i), pStructure->Get_Field_Type(i));
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
CSG_Table::~CSG_Table(void)
{
	_Destroy();
}

//---------------------------------------------------------
bool CSG_Table::Destroy(void)
{
	return( is_Private() ? false : _Destroy() );
}

bool CSG_Table::_Destroy(void)
{
	_Destroy_Selection();

	_Del_Records();

	if( m_nFields > 0 )
	{
		for(int i=0; i<m_nFields; i++)
		{
			delete(m_Field_Name[i]);
		}

		m_nFields		= 0;

		SG_Free(m_Field_Name);
		SG_Free(m_Field_Type);
		SG_Free(m_Field_Val_Min);
		SG_Free(m_Field_Val_Max);

		m_Field_Name	= NULL;
		m_Field_Type	= NULL;
		m_Field_Val_Min	= NULL;
		m_Field_Val_Max	= NULL;
	}

	CSG_Data_Object::Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Assign							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Assign(CSG_Data_Object *pObject)
{
	return( is_Private() ? false : _Assign(pObject) );
}

bool CSG_Table::_Assign(CSG_Data_Object *pObject)
{
	int			i;
	CSG_Table	*pTable;

	if( pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType() )
	{
		_Destroy();

		pTable	= (CSG_Table *)pObject;

		for(i=0; i<pTable->m_nFields; i++)
		{
			Add_Field(pTable->m_Field_Name[i]->c_str(), pTable->m_Field_Type[i]);
		}

		for(i=0; i<pTable->m_nRecords; i++)
		{
			_Add_Record(pTable->m_Records[i]);
		}

		Get_History().Assign(pTable->Get_History());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Assign_Values(CSG_Table *pTable)
{
	int		i;

	if( is_Compatible(pTable) )
	{
		if( is_Private() )
		{
			if( Get_Record_Count() == pTable->Get_Record_Count() )
			{
				_Index_Destroy();

				for(i=0; i<pTable->Get_Record_Count(); i++)
				{
					Get_Record(i)->Assign(pTable->Get_Record(i));
				}

				return( true );
			}
		}
		else
		{
			Del_Records();

			for(i=0; i<pTable->Get_Record_Count(); i++)
			{
				Add_Record(pTable->Get_Record(i));
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Checks							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::is_Compatible(CSG_Table *pTable, bool bExactMatch) const
{
	if( Get_Field_Count() == pTable->Get_Field_Count() )
	{
		for(int i=0; i<Get_Field_Count(); i++)
		{
			if( bExactMatch )
			{
				if( Get_Field_Type(i) != pTable->Get_Field_Type(i) )
				{
					return( false );
				}
			}
			else switch( Get_Field_Type(i) )
			{
			case TABLE_FIELDTYPE_String:
//				if( pTable->Get_Field_Type(i) != TABLE_FIELDTYPE_String )
//				{
//					return( false );
//				}
				break;

			default:
				if( pTable->Get_Field_Type(i) == TABLE_FIELDTYPE_String )
				{
					return( false );
				}
				break;
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Fields							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Table::Add_Field(const SG_Char *Name, TSG_Table_Field_Type Type, int add_Field)
{
	int		iField, iRecord;

	//-----------------------------------------------------
	if( add_Field < 0 || add_Field > m_nFields )
	{
		add_Field	= m_nFields;
	}

	//-----------------------------------------------------
	m_nFields++;

	m_Field_Name	= (CSG_String          **)SG_Realloc(m_Field_Name		, m_nFields * sizeof(CSG_String *));
	m_Field_Type	= (TSG_Table_Field_Type *)SG_Realloc(m_Field_Type		, m_nFields * sizeof(TSG_Table_Field_Type));
	m_Field_Val_Min	= (double               *)SG_Realloc(m_Field_Val_Min	, m_nFields * sizeof(double));
	m_Field_Val_Max	= (double               *)SG_Realloc(m_Field_Val_Max	, m_nFields * sizeof(double));

	//-----------------------------------------------------
	for(iField=m_nFields-1; iField>add_Field; iField--)
	{
		m_Field_Name   [iField]	= m_Field_Name   [iField - 1];
		m_Field_Type   [iField]	= m_Field_Type   [iField - 1];
		m_Field_Val_Min[iField]	= m_Field_Val_Min[iField - 1];
		m_Field_Val_Max[iField]	= m_Field_Val_Max[iField - 1];
	}

	//-----------------------------------------------------
	m_Field_Name   [add_Field]	= new CSG_String(Name);
	m_Field_Type   [add_Field]	= Type;
	m_Field_Val_Min[add_Field]	= 0.0;
	m_Field_Val_Max[add_Field]	= 0.0;

	//-----------------------------------------------------
	for(iRecord=0; iRecord<m_nRecords; iRecord++)
	{
		m_Records[iRecord]->_Add_Field(add_Field);
	}

	Set_Modified();
}

//---------------------------------------------------------
#ifdef _SAGA_UNICODE
void CSG_Table::Add_Field(const char *Name, TSG_Table_Field_Type Type, int iField)
{	Add_Field(CSG_String(Name), Type, iField);	}
#endif

//---------------------------------------------------------
bool CSG_Table::Del_Field(int del_Field)
{
	int		iRecord, iField;

	if( del_Field >= 0 && del_Field < m_nFields )
	{
		m_nFields--;

		//-------------------------------------------------
		delete(m_Field_Name[del_Field]);

		//-------------------------------------------------
		for(iField=del_Field; iField<m_nFields; iField++)
		{
			m_Field_Name   [iField]	= m_Field_Name   [iField + 1];
			m_Field_Type   [iField]	= m_Field_Type   [iField + 1];
			m_Field_Val_Min[iField]	= m_Field_Val_Min[iField + 1];
			m_Field_Val_Max[iField]	= m_Field_Val_Max[iField + 1];
		}

		//-------------------------------------------------
		m_Field_Name	= (CSG_String          **)SG_Realloc(m_Field_Name		, m_nFields * sizeof(CSG_String *));
		m_Field_Type	= (TSG_Table_Field_Type *)SG_Realloc(m_Field_Type		, m_nFields * sizeof(TSG_Table_Field_Type));
		m_Field_Val_Min	= (double               *)SG_Realloc(m_Field_Val_Min	, m_nFields * sizeof(double));
		m_Field_Val_Max	= (double               *)SG_Realloc(m_Field_Val_Max	, m_nFields * sizeof(double));

		//-------------------------------------------------
		for(iRecord=0; iRecord<m_nRecords; iRecord++)
		{
			m_Records[iRecord]->_Del_Field(del_Field);
		}

		Set_Modified();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Records							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_GROW_SIZE(n)	(n < 256 ? 1 : (n < 8192 ? 128 : 1024))

//---------------------------------------------------------
bool CSG_Table::_Inc_Array(void)
{
	if( m_nRecords >= m_nBuffer )
	{
		CSG_Table_Record	**pRecords	= (CSG_Table_Record **)SG_Realloc(m_Records, (m_nBuffer + GET_GROW_SIZE(m_nBuffer)) * sizeof(CSG_Table_Record *));

		if( pRecords )
		{
			m_Records	= pRecords;
			m_nBuffer	+= GET_GROW_SIZE(m_nBuffer);
		}
		else
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::_Dec_Array(void)
{
	if( m_nRecords >= 0 && m_nRecords < m_nBuffer - GET_GROW_SIZE(m_nBuffer) )
	{
		CSG_Table_Record	**pRecords	= (CSG_Table_Record **)SG_Realloc(m_Records, (m_nBuffer - GET_GROW_SIZE(m_nBuffer)) * sizeof(CSG_Table_Record *));

		if( pRecords )
		{
			m_Records	= pRecords;
			m_nBuffer	-= GET_GROW_SIZE(m_nBuffer);
		}
		else
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::Add_Record(CSG_Table_Record *pValues)
{
	return( is_Private() ? NULL : _Add_Record(pValues) );
}

CSG_Table_Record * CSG_Table::_Add_Record(CSG_Table_Record *pValues)
{
	if( !_Inc_Array() )
	{
		return( NULL );
	}

	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	if( is_Indexed() )
	{
		m_Index				= (int *)SG_Realloc(m_Index, (m_nRecords + 1) * sizeof(int));
		m_Index[m_nRecords]	= m_nRecords;
	}

	//-----------------------------------------------------
	m_Records[m_nRecords]	= pRecord	= new CSG_Table_Record(this, m_nRecords);
	m_nRecords++;

	//-----------------------------------------------------
	if( pValues )
	{
		pRecord->Assign(pValues);
	}

	Set_Modified();
	_Range_Invalidate();

	return( pRecord );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::Ins_Record(int iRecord, CSG_Table_Record *pValues)
{
	return( is_Private() ? NULL : _Ins_Record(iRecord, pValues) );
}

CSG_Table_Record * CSG_Table::_Ins_Record(int iRecord, CSG_Table_Record *pValues)
{
	//-----------------------------------------------------
	if( iRecord >= m_nRecords )
	{
		return( _Add_Record(pValues) );
	}
	else if( iRecord < 0 )
	{
		iRecord	= 0;
	}

	//-----------------------------------------------------
	if( !_Inc_Array() )
	{
		return( NULL );
	}

	//-----------------------------------------------------
	int					i;
	CSG_Table_Record	*pRecord;

	if( is_Indexed() )
	{
		m_Index				= (int *)SG_Realloc(m_Index, (m_nRecords + 1) * sizeof(int));

		for(i=m_nRecords; i>iRecord; i--)
		{
			m_Index[i]		= m_Index[i - 1];
		}

		m_Index[iRecord]	= iRecord;
	}

	//-----------------------------------------------------
	for(i=m_nRecords; i>iRecord; i--)
	{
		m_Records[i]			= m_Records[i - 1];
		m_Records[i]->m_Index	= i;
	}

	m_Records[iRecord]		= pRecord	= new CSG_Table_Record(this, iRecord);
	m_nRecords++;

	//-----------------------------------------------------
	if( pValues )
	{
		pRecord->Assign(pValues);
	}

	Set_Modified();
	_Range_Invalidate();

	return( pRecord );
}

//---------------------------------------------------------
bool CSG_Table::Del_Record(int iRecord)
{
	return( is_Private() ? false : _Del_Record(iRecord) );
}

bool CSG_Table::_Del_Record(int iRecord)
{
	int		i, j;

	if( iRecord >= 0 && iRecord < m_nRecords )
	{
		delete(m_Records[iRecord]);

		m_nRecords--;

		for(i=iRecord, j=iRecord+1; i<m_nRecords; i++, j++)
		{
			m_Records[i]			= m_Records[i + 1];
			m_Records[i]->m_Index	= i;
		}

		_Dec_Array();

		if( is_Indexed() )
		{
			for(i=0; i<m_nRecords; i++)
			{
				if( m_Index[i] == iRecord )
				{
					for(; i<m_nRecords; i++)
					{
						m_Index[i]	= m_Index[i + 1];
					}
				}
			}

			m_Index	= (int *)SG_Realloc(m_Index, m_nRecords * sizeof(int));

			for(i=0; i<m_nRecords; i++)
			{
				if( m_Index[i] > iRecord )
				{
					m_Index[i]--;
				}
			}
		}

		Set_Modified();
		_Range_Invalidate();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Del_Records(void)
{
	return( is_Private() ? false : _Del_Records() );
}

bool CSG_Table::_Del_Records(void)
{
	if( m_Records > 0 )
	{
		_Index_Destroy();

		for(int iRecord=0; iRecord<m_nRecords; iRecord++)
		{
			delete(m_Records[iRecord]);
		}

		SG_Free(m_Records);
		m_Records	= NULL;
		m_nRecords	= 0;
		m_nBuffer	= 0;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Value Access					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Set_Value(int iRecord, int iField, const SG_Char  *Value)
{
	CSG_Table_Record	*pRecord;

	if( iField >= 0 && iField < m_nFields && (pRecord = Get_Record(iRecord)) != NULL )
	{
		return( pRecord->Set_Value(iField, Value) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Set_Value(int iRecord, int iField, double       Value)
{
	CSG_Table_Record	*pRecord;

	if( iField >= 0 && iField < m_nFields && (pRecord = Get_Record(iRecord)) != NULL )
	{
		return( pRecord->Set_Value(iField, Value) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Get_Value(int iRecord, int iField, CSG_String &Value) const
{
	CSG_Table_Record	*pRecord;

	if( iField >= 0 && iField < m_nFields && (pRecord = Get_Record(iRecord)) != NULL )
	{
		Value	= pRecord->asString(iField);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Get_Value(int iRecord, int iField, double      &Value) const
{
	CSG_Table_Record	*pRecord;

	if( iField >= 0 && iField < m_nFields && (pRecord = Get_Record(iRecord)) != NULL )
	{
		Value	= pRecord->asDouble(iField);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Statistics						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::_Range_Invalidate(void) const
{
	int		iField;

	for(iField=0; iField<m_nFields; iField++)
	{
		_Range_Invalidate(iField);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::_Range_Invalidate(int iField) const
{
	if( iField >= 0 && iField < m_nFields )
	{
		m_Field_Val_Min[iField]	=  0.0;
		m_Field_Val_Max[iField]	= -1.0;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::_Range_Update(int iField) const
{
	int				iRecord;
	double			Value;
	CSG_Table_Record	**ppRecord;

	if( iField >= 0 && iField < m_nFields && m_nRecords > 0 )
	{
		if( m_Field_Val_Min[iField] > m_Field_Val_Max[iField] )
		{
			ppRecord	= m_Records;
			m_Field_Val_Min[iField]	= m_Field_Val_Max[iField]	= (*ppRecord)->asDouble(iField);

			for(iRecord=1, ppRecord++; iRecord<m_nRecords; iRecord++, ppRecord++)
			{
				Value	= (*ppRecord)->asDouble(iField);

				if( m_Field_Val_Min[iField] > Value )
				{
					m_Field_Val_Min[iField]	= Value;
				}
				else if( m_Field_Val_Max[iField] < Value )
				{
					m_Field_Val_Max[iField]	= Value;
				}
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Index							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Set_Index(int iField, TSG_Table_Index_Order Order)
{
	if( iField >= 0 && iField < m_nFields )
	{
		m_Index_Order	= Order;

		switch( m_Index_Order )
		{
		case TABLE_INDEX_None: default:
			_Index_Destroy();
			break;

		case TABLE_INDEX_Up:
		case TABLE_INDEX_Down:
			_Index_Create(iField);
			break;
		}
	}

	return( is_Indexed() );
}

//---------------------------------------------------------
bool CSG_Table::Toggle_Index(int iField)
{
	if( iField >= 0 && iField < m_nFields )
	{
		if( iField != m_Index_Field )
		{
			return( Set_Index(iField, TABLE_INDEX_Up) );
		}
		else switch( m_Index_Order )
		{
		case TABLE_INDEX_None:
			return( Set_Index(iField, TABLE_INDEX_Up) );

		case TABLE_INDEX_Up:
			return( Set_Index(iField, TABLE_INDEX_Down) );

		case TABLE_INDEX_Down:
			return( Set_Index(iField, TABLE_INDEX_None) );
		}
	}

	return( false );
}

//---------------------------------------------------------
#define SORT_SWAP(a,b)	{itemp=(a);(a)=(b);(b)=itemp;}

void CSG_Table::_Index_Create(int iField)
{
	const int	M	= 7;

	int		indxt, itemp, *istack,
			i, j, k, a,
			l		= 0,
			ir		= m_nRecords - 1,
			nstack	= 64,
			jstack	= 0;

	//-----------------------------------------------------
	m_Index_Field	= iField;

	if( m_Index == NULL )
	{
		m_Index	= (int *)SG_Malloc(m_nRecords * sizeof(int));
	}

	for(j=0; j<m_nRecords; j++)
	{
		m_Index[j]	= j;
	}

	istack	= (int *)SG_Malloc(nstack * sizeof(int));

	//-----------------------------------------------------
	for(;;)
	{
		if( ir - l < M )
		{
			for(j=l+1; j<=ir; j++)
			{
				a		= indxt	= m_Index[j];

				for(i=j-1; i>=0; i--)
				{
					if( _Index_Compare(m_Index[i], a) <= 0 )
					{
						break;
					}

					m_Index[i + 1]	= m_Index[i];
				}

				m_Index[i + 1]	= indxt;
			}

			if( jstack == 0 )
			{
				break;
			}

			ir		= istack[jstack--];
			l		= istack[jstack--];
		}
		else
		{
			k		= (l + ir) >> 1;
			SORT_SWAP(m_Index[k], m_Index[l + 1]);

			if( _Index_Compare(m_Index[l + 1], m_Index[ir]) > 0 )
				SORT_SWAP(     m_Index[l + 1], m_Index[ir]);

			if( _Index_Compare(m_Index[l    ], m_Index[ir]) > 0 )
				SORT_SWAP(     m_Index[l    ], m_Index[ir]);

			if( _Index_Compare(m_Index[l + 1], m_Index[l ]) > 0 )
				SORT_SWAP(     m_Index[l + 1], m_Index[l ]);

			i		= l + 1;
			j		= ir;
			a		= indxt	= m_Index[l];

			for(;;)
			{
				do	i++;	while( _Index_Compare(m_Index[i], a) < 0 );
				do	j--;	while( _Index_Compare(m_Index[j], a) > 0 );

				if( j < i )
				{
					break;
				}

				SORT_SWAP(m_Index[i], m_Index[j]);
			}

			m_Index[l]	= m_Index[j];
			m_Index[j]	= indxt;
			jstack		+= 2;

			if( jstack >= nstack )
			{
				nstack	+= 64;
				istack	= (int *)SG_Realloc(istack, nstack * sizeof(int));
			}

			if( ir - i + 1 >= j - l )
			{
				istack[jstack]		= ir;
				istack[jstack - 1]	= i;
				ir					= j - 1;
			}
			else
			{
				istack[jstack]		= j - 1;
				istack[jstack - 1]	= l;
				l					= i;
			}
		}
	}

	SG_Free(istack);
}

#undef SORT_SWAP

//---------------------------------------------------------
void CSG_Table::_Index_Destroy(void)
{
	m_Index_Field	= -1;
	m_Index_Order	= TABLE_INDEX_None;

	if( m_Index )
	{
		SG_Free(m_Index);

		m_Index	= NULL;
	}
}

//---------------------------------------------------------
inline int CSG_Table::_Index_Compare(int a, int b)
{
	switch( m_Field_Type[m_Index_Field] )
	{
	case TABLE_FIELDTYPE_String:
		return( SG_STR_CMP(
			m_Records[a]->asString(m_Index_Field),
			m_Records[b]->asString(m_Index_Field))
		);

	default:
		double	d	= m_Records[a]->asDouble(m_Index_Field)
					- m_Records[b]->asDouble(m_Index_Field);

		return( d < 0.0 ? -1 : (d > 0.0 ? 1 : 0) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
