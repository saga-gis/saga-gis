
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
CTable * API_Create_Table(void)
{
	return( new CTable );
}

//---------------------------------------------------------
CTable * API_Create_Table(const CTable &Table)
{
	return( new CTable(Table) );
}

//---------------------------------------------------------
CTable * API_Create_Table(const char *FileName)
{
	return( new CTable(FileName) );
}

//---------------------------------------------------------
CTable * API_Create_Table(CTable *pStructure)
{
	return( new CTable(pStructure) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable::CTable(void)
	: CDataObject()
{
	_On_Construction();
}

//---------------------------------------------------------
CTable::CTable(const CTable &Table)
	: CDataObject()
{
	_On_Construction();

	Create(Table);
}

//---------------------------------------------------------
CTable::CTable(const char *File_Name, char Separator)
	: CDataObject()
{
	_On_Construction();

	Create(File_Name, Separator);
}

//---------------------------------------------------------
CTable::CTable(CTable *pStructure)
	: CDataObject()
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
void CTable::_On_Construction(void)
{
	m_nFields		= 0;
	m_Field_Name	= NULL;
	m_Field_Type	= NULL;
	m_Field_Val_Min	= NULL;
	m_Field_Val_Max	= NULL;

	m_nRecords		= 0;
	m_Records		= NULL;

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
bool CTable::Create(const CTable &Table)
{
	return( is_Private() ? false : _Create(Table) );
}

bool CTable::_Create(const CTable &Table)
{
	return( Assign((CDataObject *)&Table) );
}

//---------------------------------------------------------
bool CTable::Create(const char *File_Name, char Separator)
{
	return( is_Private() ? false : _Create(File_Name, Separator) );
}

bool CTable::_Create(const char *File_Name, char Separator)
{
	return( _Load(File_Name, TABLE_FILETYPE_Undefined, Separator) );
}

//---------------------------------------------------------
bool CTable::Create(CTable *pStructure)
{
	return( is_Private() ? false : _Create(pStructure) );
}

bool CTable::_Create(CTable *pStructure)
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
CTable::~CTable(void)
{
	_Destroy();
}

//---------------------------------------------------------
bool CTable::Destroy(void)
{
	return( is_Private() ? false : _Destroy() );
}

bool CTable::_Destroy(void)
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

		API_Free(m_Field_Name);
		API_Free(m_Field_Type);
		API_Free(m_Field_Val_Min);
		API_Free(m_Field_Val_Max);

		m_Field_Name	= NULL;
		m_Field_Type	= NULL;
		m_Field_Val_Min	= NULL;
		m_Field_Val_Max	= NULL;
	}

	CDataObject::Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Assign							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable::Assign(CDataObject *pObject)
{
	return( is_Private() ? false : _Assign(pObject) );
}

bool CTable::_Assign(CDataObject *pObject)
{
	int		i;
	CTable	*pTable;

	if( pObject && pObject->is_Valid() && pObject->Get_ObjectType() == Get_ObjectType() )
	{
		_Destroy();

		pTable	= (CTable *)pObject;

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
bool CTable::Assign_Values(CTable *pTable)
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
bool CTable::is_Compatible(CTable *pTable, bool bExactMatch)
{
	int		i;

	if( Get_Field_Count() == pTable->Get_Field_Count() )
	{
		for(i=0; i<Get_Field_Count(); i++)
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
void CTable::Add_Field(const char *Name, TTable_FieldType Type, int add_Field)
{
	int		iField, iRecord;

	//-----------------------------------------------------
	if( add_Field < 0 || add_Field > m_nFields )
	{
		add_Field	= m_nFields;
	}

	//-----------------------------------------------------
	m_nFields++;

	m_Field_Name	= (CAPI_String     **)API_Realloc(m_Field_Name		, m_nFields * sizeof(CAPI_String *));
	m_Field_Type	= (TTable_FieldType *)API_Realloc(m_Field_Type		, m_nFields * sizeof(TTable_FieldType));
	m_Field_Val_Min	= (double           *)API_Realloc(m_Field_Val_Min	, m_nFields * sizeof(double));
	m_Field_Val_Max	= (double           *)API_Realloc(m_Field_Val_Max	, m_nFields * sizeof(double));

	//-----------------------------------------------------
	for(iField=m_nFields-1; iField>add_Field; iField--)
	{
		m_Field_Name   [iField]	= m_Field_Name   [iField - 1];
		m_Field_Type   [iField]	= m_Field_Type   [iField - 1];
		m_Field_Val_Min[iField]	= m_Field_Val_Min[iField - 1];
		m_Field_Val_Max[iField]	= m_Field_Val_Max[iField - 1];
	}

	//-----------------------------------------------------
	m_Field_Name   [add_Field]	= new CAPI_String(Name);
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
bool CTable::Del_Field(int del_Field)
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
		m_Field_Name	= (CAPI_String     **)API_Realloc(m_Field_Name		, m_nFields * sizeof(CAPI_String *));
		m_Field_Type	= (TTable_FieldType *)API_Realloc(m_Field_Type		, m_nFields * sizeof(TTable_FieldType));
		m_Field_Val_Min	= (double           *)API_Realloc(m_Field_Val_Min	, m_nFields * sizeof(double));
		m_Field_Val_Max	= (double           *)API_Realloc(m_Field_Val_Max	, m_nFields * sizeof(double));

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
CTable_Record * CTable::Add_Record(CTable_Record *pValues)
{
	return( is_Private() ? NULL : _Add_Record(pValues) );
}

CTable_Record * CTable::_Add_Record(CTable_Record *pValues)
{
	CTable_Record	*pRecord;

	//-----------------------------------------------------
	if( is_Indexed() )
	{
		m_Index				= (int *)API_Realloc(m_Index, (m_nRecords + 1) * sizeof(int));
		m_Index[m_nRecords]	= m_nRecords;
	}

	//-----------------------------------------------------
	m_Records				= (CTable_Record **)API_Realloc(m_Records, (m_nRecords + 1) * sizeof(CTable_Record *));
	m_Records[m_nRecords]	= pRecord	= new CTable_Record(this, m_nRecords);
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
CTable_Record * CTable::Ins_Record(int iRecord, CTable_Record *pValues)
{
	return( is_Private() ? NULL : _Ins_Record(iRecord, pValues) );
}

CTable_Record * CTable::_Ins_Record(int iRecord, CTable_Record *pValues)
{
	int				i;
	CTable_Record	*pRecord;

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
	if( is_Indexed() )
	{
		m_Index				= (int *)API_Realloc(m_Index, (m_nRecords + 1) * sizeof(int));

		for(i=m_nRecords; i>iRecord; i--)
		{
			m_Index[i]		= m_Index[i - 1];
		}

		m_Index[iRecord]	= iRecord;
	}

	//-----------------------------------------------------
	m_Records				= (CTable_Record **)API_Realloc(m_Records, (m_nRecords + 1) * sizeof(CTable_Record *));

	for(i=m_nRecords; i>iRecord; i--)
	{
		m_Records[i]			= m_Records[i - 1];
		m_Records[i]->m_Index	= i;
	}

	m_Records[iRecord]		= pRecord	= new CTable_Record(this, iRecord);
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
bool CTable::Del_Record(int iRecord)
{
	return( is_Private() ? false : _Del_Record(iRecord) );
}

bool CTable::_Del_Record(int iRecord)
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

		m_Records	= (CTable_Record **)API_Realloc(m_Records, m_nRecords * sizeof(CTable_Record *));

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

			m_Index	= (int *)API_Realloc(m_Index, m_nRecords * sizeof(int));

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
bool CTable::Del_Records(void)
{
	return( is_Private() ? false : _Del_Records() );
}

bool CTable::_Del_Records(void)
{
	if( m_nRecords > 0 )
	{
		_Index_Destroy();

		for(int iRecord=0; iRecord<m_nRecords; iRecord++)
		{
			delete(m_Records[iRecord]);
		}

		API_Free(m_Records);
		m_Records	= NULL;
		m_nRecords	= 0;

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
bool CTable::Set_Value(int iRecord, int iField, const char  *Value)
{
	CTable_Record	*pRecord;

	if( iField >= 0 && iField < m_nFields && (pRecord = Get_Record(iRecord)) != NULL )
	{
		return( pRecord->Set_Value(iField, Value) );
	}

	return( false );
}

//---------------------------------------------------------
bool CTable::Set_Value(int iRecord, int iField, double       Value)
{
	CTable_Record	*pRecord;

	if( iField >= 0 && iField < m_nFields && (pRecord = Get_Record(iRecord)) != NULL )
	{
		return( pRecord->Set_Value(iField, Value) );
	}

	return( false );
}

//---------------------------------------------------------
bool CTable::Get_Value(int iRecord, int iField, CAPI_String &Value)
{
	CTable_Record	*pRecord;

	if( iField >= 0 && iField < m_nFields && (pRecord = Get_Record(iRecord)) != NULL )
	{
		Value	= pRecord->asString(iField);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CTable::Get_Value(int iRecord, int iField, double      &Value)
{
	CTable_Record	*pRecord;

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
bool CTable::_Range_Invalidate(void)
{
	int		iField;

	for(iField=0; iField<m_nFields; iField++)
	{
		_Range_Invalidate(iField);
	}

	return( true );
}

//---------------------------------------------------------
bool CTable::_Range_Invalidate(int iField)
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
bool CTable::_Range_Update(int iField)
{
	int				iRecord;
	double			Value;
	CTable_Record	**ppRecord;

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
bool CTable::Set_Index(int iField, TTable_Index_Order Order)
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
bool CTable::Toggle_Index(int iField)
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

void CTable::_Index_Create(int iField)
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
		m_Index	= (int *)API_Malloc(m_nRecords * sizeof(int));
	}

	for(j=0; j<m_nRecords; j++)
	{
		m_Index[j]	= j;
	}

	istack	= (int *)API_Malloc(nstack * sizeof(int));

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
				istack	= (int *)API_Realloc(istack, nstack * sizeof(int));
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

	API_Free(istack);
}

#undef SORT_SWAP

//---------------------------------------------------------
void CTable::_Index_Destroy(void)
{
	m_Index_Field	= -1;
	m_Index_Order	= TABLE_INDEX_None;

	if( m_Index )
	{
		API_Free(m_Index);

		m_Index	= NULL;
	}
}

//---------------------------------------------------------
inline int CTable::_Index_Compare(int a, int b)
{
	switch( m_Field_Type[m_Index_Field] )
	{
	case TABLE_FIELDTYPE_String:
		return( strcmp(
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
