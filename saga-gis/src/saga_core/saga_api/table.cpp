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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
#include "shapes.h"
#include "tool_library.h"


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
	switch( Table.Get_ObjectType() )
	{
	case SG_DATAOBJECT_TYPE_Table:
		return( new CSG_Table(Table) );

	case SG_DATAOBJECT_TYPE_Shapes:
	case SG_DATAOBJECT_TYPE_PointCloud:
		return( SG_Create_Shapes(*((CSG_Shapes *)&Table)) );

	default:
		return( NULL );
	}
}

//---------------------------------------------------------
CSG_Table * SG_Create_Table(const CSG_String &File_Name)
{
	return( new CSG_Table(File_Name) );
}

//---------------------------------------------------------
CSG_Table * SG_Create_Table(CSG_Table *pTemplate)
{
	if( pTemplate )
	{
		switch( pTemplate->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Table:
			return( new CSG_Table(pTemplate) );

		case SG_DATAOBJECT_TYPE_Shapes:
		case SG_DATAOBJECT_TYPE_PointCloud:
			return( SG_Create_Shapes((CSG_Shapes *)pTemplate) );

		default:
			break;
		}
	}

	return( new CSG_Table() );
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

bool CSG_Table::Create(const CSG_Table &Table)
{
	return( Assign((CSG_Data_Object *)&Table) );
}

//---------------------------------------------------------
CSG_Table::CSG_Table(const CSG_String &File_Name, TSG_Table_File_Type Format, int Encoding)
{
	_On_Construction();

	Create(File_Name, Format, Encoding);
}

bool CSG_Table::Create(const CSG_String &File_Name, TSG_Table_File_Type Format, int Encoding)
{
	Destroy();

	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Load table"), File_Name.c_str()), true);

	//-----------------------------------------------------
	bool	bResult	= File_Name.BeforeFirst(':').Cmp("PGSQL") && SG_File_Exists(File_Name) && Load(File_Name, (int)Format, '\0', Encoding);

	if( bResult )
	{
		Set_File_Name(File_Name, true);
	}

	//-----------------------------------------------------
	else if( File_Name.BeforeFirst(':').Cmp("PGSQL") == 0 )	// database source
	{
		CSG_String	s(File_Name);

		s	= s.AfterFirst(':');	CSG_String	Host  (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	Port  (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	DBName(s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	Table (s.BeforeFirst(':'));

		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("db_pgsql", 0);	// CGet_Connections

		if(	pTool != NULL )
		{
			SG_UI_ProgressAndMsg_Lock(true);

			//---------------------------------------------
			CSG_Table	Connections;
			CSG_String	Connection	= DBName + " [" + Host + ":" + Port + "]";

			pTool->Set_Manager(NULL);
			pTool->On_Before_Execution();

			if( SG_TOOL_PARAMETER_SET("CONNECTIONS", &Connections) && pTool->Execute() )	// CGet_Connections
			{
				for(int i=0; !bResult && i<Connections.Get_Count(); i++)
				{
					if( !Connection.Cmp(Connections[i].asString(0)) )
					{
						bResult	= true;
					}
				}
			}

			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

			//---------------------------------------------
			if( bResult && (bResult = (pTool = SG_Get_Tool_Library_Manager().Create_Tool("db_pgsql", 12)) != NULL) == true )	// CPGIS_Table_Load
			{
				pTool->Set_Manager(NULL);
				pTool->On_Before_Execution();

				bResult	=  SG_TOOL_PARAMETER_SET("CONNECTION", Connection)
						&& SG_TOOL_PARAMETER_SET("TABLES"    , Table)
						&& SG_TOOL_PARAMETER_SET("TABLE"     , this)
						&& pTool->Execute();

				SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
			}

			SG_UI_ProgressAndMsg_Lock(false);
		}
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Set_Modified(false);
		Set_Update_Flag();

		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	Destroy();

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}

//---------------------------------------------------------
CSG_Table::CSG_Table(const CSG_String &File_Name, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(File_Name, Format, Separator, Encoding);
}

bool CSG_Table::Create(const CSG_String &File_Name, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding)
{
	return( Load(File_Name, (int)Format, Separator, Encoding) );
}

//---------------------------------------------------------
CSG_Table::CSG_Table(const CSG_Table *pTemplate)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(pTemplate);
}

bool CSG_Table::Create(const CSG_Table *pTemplate)
{
	if( !pTemplate || pTemplate->Get_Field_Count() < 1 )
	{
		return( false );
	}

	Destroy();

	Set_Name              (pTemplate->Get_Name       ());
	Set_Description       (pTemplate->Get_Description());
	Set_NoData_Value_Range(pTemplate->Get_NoData_Value(), pTemplate->Get_NoData_hiValue());

	m_Encoding	= pTemplate->m_Encoding;

	for(int i=0; i<pTemplate->Get_Field_Count(); i++)
	{
		Add_Field(pTemplate->Get_Field_Name(i), pTemplate->Get_Field_Type(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Table::_On_Construction(void)
{
	m_nFields		= 0;
	m_Field_Name	= NULL;
	m_Field_Type	= NULL;
	m_Field_Stats	= NULL;

	m_Records		= NULL;
	m_nRecords		= 0;
	m_nBuffer		= 0;

	m_Encoding		= SG_FILE_ENCODING_UTF8;

	m_Index			= NULL;

	m_Selection.Create(sizeof(size_t), 0, SG_ARRAY_GROWTH_3);

	Set_Update_Flag();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table::~CSG_Table(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Table::Destroy(void)
{
	_Destroy_Selection();

	Del_Records();

	if( m_nFields > 0 )
	{
		for(int i=0; i<m_nFields; i++)
		{
			delete(m_Field_Name [i]);
			delete(m_Field_Stats[i]);
		}

		m_nFields		= 0;

		SG_Free(m_Field_Name);
		SG_Free(m_Field_Type);
		SG_Free(m_Field_Stats);

		m_Field_Name	= NULL;
		m_Field_Type	= NULL;
		m_Field_Stats	= NULL;
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
CSG_Table & CSG_Table::operator = (const CSG_Table &Table)
{
	Create(Table);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Table::Assign(CSG_Data_Object *pObject)
{
	if( !pObject || !pObject->is_Valid()
	||	(	pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_Table
		&&	pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_Shapes
		&&	pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_PointCloud	) )
	{
		return( false );
	}

	CSG_Table	*pTable	= (CSG_Table *)pObject;

	if( !Create(pTable) )
	{
		return( false );
	}

	for(int i=0; i<pTable->Get_Count(); i++)
	{
		Add_Record(pTable->Get_Record(i));
	}

	Get_History()	= pTable->Get_History();

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::Assign_Values(CSG_Table *pTable)
{
	if( !is_Compatible(pTable) || !Set_Record_Count(pTable->Get_Count()) )
	{
		return( false );
	}

	for(int i=0; i<pTable->Get_Record_Count(); i++)
	{
		Get_Record(i)->Assign(pTable->Get_Record(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Checks							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::is_Compatible(CSG_Table *pTable, bool bExactMatch) const
{
	if( pTable && Get_Field_Count() == pTable->Get_Field_Count() )
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
			case SG_DATATYPE_String:
//				if( pTable->Get_Field_Type(i) != SG_DATATYPE_String )
//				{
//					return( false );
//				}
				break;

			default:
				if( pTable->Get_Field_Type(i) == SG_DATATYPE_String )
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
bool CSG_Table::Add_Field(const CSG_String &Name, TSG_Data_Type Type, int add_Field)
{
	int		iField, iRecord;

	//-----------------------------------------------------
	if( add_Field < 0 || add_Field > m_nFields )
	{
		add_Field	= m_nFields;
	}

	//-----------------------------------------------------
	m_nFields++;

	m_Field_Name	= (CSG_String            **)SG_Realloc(m_Field_Name , m_nFields * sizeof(CSG_String *));
	m_Field_Type	= (TSG_Data_Type          *)SG_Realloc(m_Field_Type , m_nFields * sizeof(TSG_Data_Type));
	m_Field_Stats	= (CSG_Simple_Statistics **)SG_Realloc(m_Field_Stats, m_nFields * sizeof(CSG_Simple_Statistics *));

	//-----------------------------------------------------
	for(iField=m_nFields-1; iField>add_Field; iField--)
	{
		m_Field_Name [iField]	= m_Field_Name [iField - 1];
		m_Field_Type [iField]	= m_Field_Type [iField - 1];
		m_Field_Stats[iField]	= m_Field_Stats[iField - 1];
	}

	//-----------------------------------------------------
	m_Field_Name [add_Field]	= new CSG_String(!Name.is_Empty() ? Name : CSG_String::Format("FIELD_%d", m_nFields));
	m_Field_Type [add_Field]	= Type;
	m_Field_Stats[add_Field]	= new CSG_Simple_Statistics();

	//-----------------------------------------------------
	for(iRecord=0; iRecord<m_nRecords; iRecord++)
	{
		m_Records[iRecord]->_Add_Field(add_Field);
	}

	Set_Modified();

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::Del_Field(int del_Field)
{
	int		iRecord, iField;

	if( del_Field >= 0 && del_Field < m_nFields )
	{
		m_nFields--;

		//-------------------------------------------------
		delete(m_Field_Name [del_Field]);
		delete(m_Field_Stats[del_Field]);

		//-------------------------------------------------
		for(iField=del_Field; iField<m_nFields; iField++)
		{
			m_Field_Name [iField]	= m_Field_Name [iField + 1];
			m_Field_Type [iField]	= m_Field_Type [iField + 1];
			m_Field_Stats[iField]	= m_Field_Stats[iField + 1];
		}

		//-------------------------------------------------
		m_Field_Name	= (CSG_String            **)SG_Realloc(m_Field_Name , m_nFields * sizeof(CSG_String *));
		m_Field_Type	= (TSG_Data_Type          *)SG_Realloc(m_Field_Type , m_nFields * sizeof(TSG_Data_Type));
		m_Field_Stats	= (CSG_Simple_Statistics **)SG_Realloc(m_Field_Stats, m_nFields * sizeof(CSG_Simple_Statistics *));

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

//---------------------------------------------------------
bool CSG_Table::Set_Field_Name(int iField, const SG_Char *Name)
{
	if( iField >= 0 && iField < m_nFields && Name && *Name )
	{
		*(m_Field_Name[iField])	= Name;

		Set_Modified();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Set_Field_Type(int iField, TSG_Data_Type Type)
{
	if( iField >= 0 && iField < m_nFields )
	{
		if( m_Field_Type[iField] != Type )
		{
			m_Field_Type[iField]	= Type;

			for(int i=0; i<m_nRecords; i++)
			{
				CSG_Table_Record	*pRecord	= m_Records[i];

				CSG_Table_Value	*pNew	= CSG_Table_Record::_Create_Value(Type);

				(*pNew)	= *pRecord->m_Values[iField];

				delete(pRecord->m_Values[iField]);

				pRecord->m_Values[iField]	= pNew;

				pRecord->Set_Modified();
			}

			Set_Modified();
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CSG_Table::Get_Field_Length(int iField, int Encoding)	const
{
	size_t	Length	= 0;

	if( iField >= 0 && iField < m_nFields && m_Field_Type[iField] == SG_DATATYPE_String )
	{
		for(int i=0; i<m_nRecords; i++)
		{
			CSG_String	s(m_Records[i]->asString(iField));

			size_t	n;

			switch( Encoding )
			{
			default                      :
			case SG_FILE_ENCODING_UTF7   : n = s.Length()            ; break;
			case SG_FILE_ENCODING_UTF8   : n = s.to_UTF8().Get_Size(); break;
			case SG_FILE_ENCODING_UTF16LE:
			case SG_FILE_ENCODING_UTF16BE: n = s.Length() * 2        ; break;
			case SG_FILE_ENCODING_UTF32LE:
			case SG_FILE_ENCODING_UTF32BE: n = s.Length() * 4        ; break;
			}

			if( Length < n )
			{
				Length	= n;
			}
		}
	}

	return( (int)Length );
}

//---------------------------------------------------------
int CSG_Table::Get_Field(const CSG_String &Name) const
{
	for(int iField=0; iField<Get_Field_Count(); iField++)
	{
		if( !Name.Cmp(Get_Field_Name(iField)) )
		{
			return( iField );
		}
	}

	return( -1 );
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

			if( is_Indexed() )
			{
				int		*Index	= (int *)SG_Realloc(m_Index, m_nBuffer * sizeof(int));

				if( Index )
				{
					m_Index	= Index;
				}
				else
				{
					_Index_Destroy();
				}
			}
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

			if( is_Indexed() )
			{
				int		*Index	= (int *)SG_Realloc(m_Index, m_nBuffer * sizeof(int));

				if( Index )
				{
					m_Index	= Index;
				}
				else
				{
					_Index_Destroy();
				}
			}
		}
		else
		{
			return( false );
		}
	}

	return( true );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::_Get_New_Record(int Index)
{
	return( new CSG_Table_Record(this, Index) );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::Add_Record(CSG_Table_Record *pCopy)
{
	CSG_Table_Record	*pRecord;

	if( _Inc_Array() && (pRecord = _Get_New_Record(m_nRecords)) != NULL )
	{
		if( pCopy )
		{
			if( Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes && pCopy->Get_Table()->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
			{
				((CSG_Shape *)pRecord)->Assign((CSG_Shape *)pCopy, true);
			}
			else
			{
				pRecord->Assign(pCopy);
			}
		}

		if( is_Indexed() )
		{
			m_Index[m_nRecords]	= m_nRecords;
		}

		m_Records[m_nRecords]	= pRecord;
		m_nRecords++;

		Set_Modified();

		Set_Update_Flag();

		_Stats_Invalidate();

		return( pRecord );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::Ins_Record(int iRecord, CSG_Table_Record *pCopy)
{
	if( iRecord >= m_nRecords )
	{
		return( Add_Record(pCopy) );
	}
	else if( iRecord < 0 )
	{
		iRecord	= 0;
	}

	//-----------------------------------------------------
	CSG_Table_Record	*pRecord;

	if( _Inc_Array() && (pRecord = _Get_New_Record(m_nRecords)) != NULL )
	{
		if( pCopy )
		{
			pRecord->Assign(pCopy);
		}

		for(int i=m_nRecords; i>iRecord; i--)
		{
			if( is_Indexed() )
			{
				m_Index[i]	= m_Index[i - 1];
			}

			m_Records[i]			= m_Records[i - 1];
			m_Records[i]->m_Index	= i;
		}

		if( is_Indexed() )
		{
			m_Index[iRecord]	= iRecord;
		}

		pRecord->m_Index	= iRecord;
		m_Records[iRecord]	= pRecord;
		m_nRecords++;

		Set_Modified();

		Set_Update_Flag();

		_Stats_Invalidate();

		return( pRecord );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Table::Set_Record(int iRecord, CSG_Table_Record *pCopy)
{
	if( iRecord >= 0 && iRecord < m_nRecords && pCopy )
	{
		return( m_Records[iRecord]->Assign(pCopy) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Del_Record(int iRecord)
{
	if( iRecord >= 0 && iRecord < m_nRecords )
	{
		int		i;

		delete(m_Records[iRecord]);

		m_nRecords--;

		for(i=iRecord; i<m_nRecords; i++)
		{
			m_Records[i]			= m_Records[i + 1];
			m_Records[i]->m_Index	= i;
		}

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

			for(i=0; i<m_nRecords; i++)
			{
				if( m_Index[i] > iRecord )
				{
					m_Index[i]--;
				}
			}
		}

		_Dec_Array();

		Set_Modified();

		Set_Update_Flag();

		_Stats_Invalidate();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Del_Records(void)
{
	_Index_Destroy();

	for(int iRecord=0; iRecord<m_nRecords; iRecord++)
	{
		delete(m_Records[iRecord]);
	}

	SG_FREE_SAFE(m_Records);

	m_nRecords	= 0;
	m_nBuffer	= 0;

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::Set_Record_Count(int nRecords)
{
	if( m_nRecords < nRecords )
	{
		while( m_nRecords < nRecords && Add_Record() != NULL )	{}
	}
	else if( nRecords >= 0 && m_nRecords > nRecords )
	{
		while( m_nRecords > nRecords && Del_Record(m_nRecords - 1) )	{}
	}

	return( m_nRecords == nRecords );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Find_Record(int &iRecord, int iField, const CSG_String &Value, bool bCreateIndex)
{
	if( iField < 0 || iField >= m_nFields || m_nRecords < 1 )
	{
		return( false );
	}

	if( m_nRecords == 1 )
	{
		return( Value.Cmp(m_Records[iRecord = 0]->asString(iField)) == 0 );
	}

	if( bCreateIndex && iField != Get_Index_Field(0) )
	{
		Set_Index(iField, TABLE_INDEX_Ascending);
	}

	//-----------------------------------------------------
	if( iField != Get_Index_Field(0) )
	{
		for(iRecord=0; iRecord<m_nRecords; iRecord++)
		{
			if( Value.Cmp(m_Records[iRecord]->asString(iField)) == 0 )
			{
				return( true );
			}
		}
	}

	//-----------------------------------------------------
	else	// indexed search
	{
		#define GET_RECORD(i)	Get_Record_byIndex(bAscending ? (iRecord = i) : m_nRecords - 1 - (iRecord = i))

		bool	bAscending	= Get_Index_Order(0) == TABLE_INDEX_Ascending;

		double	d;

		if( (d = Value.Cmp(GET_RECORD(0             )->asString(iField))) < 0 ) { return( false ); } else if( d == 0 ) { return( true ); }
		if( (d = Value.Cmp(GET_RECORD(m_nRecords - 1)->asString(iField))) > 0 ) { return( false ); } else if( d == 0 ) { return( true ); }

		for(int a=0, b=m_nRecords-1; b-a > 1; )
		{
			d	= Value.Cmp(GET_RECORD(a + (b - a) / 2)->asString(iField));

			if( d > 0.0 )
			{
				a	= iRecord;
			}
			else if( d < 0.0 )
			{
				b	= iRecord;
			}
			else
			{
				iRecord	= Get_Record_byIndex(bAscending ? iRecord : m_nRecords - 1 - iRecord)->Get_Index();

				return( true );
			}
		}

		iRecord	= Get_Record_byIndex(bAscending ? iRecord : m_nRecords - 1 - iRecord)->Get_Index();
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::Find_Record(int iField, const CSG_String &Value, bool bCreateIndex)
{
	int	iRecord;

	if( Find_Record(iRecord, iField, Value, bCreateIndex) )
	{
		return( Get_Record(iRecord) );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Table::Find_Record(int &iRecord, int iField, double Value, bool bCreateIndex)
{
	if( iField < 0 || iField >= m_nFields || m_nRecords < 1 )
	{
		return( false );
	}

	if( m_nRecords == 1 )
	{
		return( Value == m_Records[iRecord = 0]->asDouble(iField) );
	}

	if( bCreateIndex && iField != Get_Index_Field(0) )
	{
		Set_Index(iField, TABLE_INDEX_Ascending);
	}

	//-----------------------------------------------------
	if( iField != Get_Index_Field(0) )
	{
		for(iRecord=0; iRecord<m_nRecords; iRecord++)
		{
			if( Value == m_Records[iRecord]->asDouble(iField) )
			{
				return( true );
			}
		}
	}

	//-----------------------------------------------------
	else	// indexed search
	{
		#define GET_RECORD(i)	Get_Record_byIndex(bAscending ? (iRecord = i) : m_nRecords - 1 - (iRecord = i))

		bool	bAscending	= Get_Index_Order(0) == TABLE_INDEX_Ascending;

		double	d;

		if( (d = Value - GET_RECORD(0             )->asDouble(iField)) < 0.0 ) { return( false ); } else if( d == 0.0 ) { return( true ); }
		if( (d = Value - GET_RECORD(m_nRecords - 1)->asDouble(iField)) > 0.0 ) { return( false ); } else if( d == 0.0 ) { return( true ); }

		for(int a=0, b=m_nRecords-1; b-a > 1; )
		{
			d	= Value - GET_RECORD(a + (b - a) / 2)->asDouble(iField);

			if( d > 0.0 )
			{
				a	= iRecord;
			}
			else if( d < 0.0 )
			{
				b	= iRecord;
			}
			else
			{
				iRecord	= Get_Record_byIndex(bAscending ? iRecord : m_nRecords - 1 - iRecord)->Get_Index();

				return( true );
			}
		}

		iRecord	= Get_Record_byIndex(bAscending ? iRecord : m_nRecords - 1 - iRecord)->Get_Index();
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::Find_Record(int iField, double Value, bool bCreateIndex)
{
	int	iRecord;

	if( Find_Record(iRecord, iField, Value, bCreateIndex) )
	{
		return( Get_Record(iRecord) );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//						Value Access					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Table::Set_Modified(bool bModified)
{
	if( bModified != is_Modified() )
	{
		CSG_Data_Object::Set_Modified(bModified);

		if( bModified == false )
		{
			#pragma omp parallel for
			for(int iRecord=0; iRecord<m_nRecords; iRecord++)
			{
				m_Records[iRecord]->Set_Modified(false);
			}
		}
	}
}

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
bool CSG_Table::_Stats_Invalidate(void) const
{
	for(int iField=0; iField<m_nFields; iField++)
	{
		m_Field_Stats[iField]->Invalidate();
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::_Stats_Invalidate(int iField) const
{
	if( iField >= 0 && iField < m_nFields )
	{
		m_Field_Stats[iField]->Invalidate();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::_Stats_Update(int iField) const
{
	if( iField >= 0 && iField < m_nFields && m_nRecords > 0 )
	{
		CSG_Simple_Statistics	*pStatistics	= m_Field_Stats[iField];

		if( !pStatistics->is_Evaluated() )
		{
			for(int iRecord=0; iRecord<m_nRecords; iRecord++)
			{
				CSG_Table_Record	*pRecord	= m_Records[iRecord];

				if( !pRecord->is_NoData(iField) )
				{
					pStatistics->Add_Value(pRecord->asDouble(iField));
				}
			}

			pStatistics->Evaluate();	// evaluate! prevent values to be added more than once!
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
bool CSG_Table::On_NoData_Changed(void)
{
	_Stats_Invalidate();

	return( CSG_Data_Object::On_NoData_Changed() );
}


///////////////////////////////////////////////////////////
//														 //
//						Index							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Set_Index(int Field_1, TSG_Table_Index_Order Order_1, int Field_2, TSG_Table_Index_Order Order_2, int Field_3, TSG_Table_Index_Order Order_3)
{
	m_Index_Field[0]	= m_Index_Field[1]	= m_Index_Field[2]	= -1;
	m_Index_Order[0]	= m_Index_Order[1]	= m_Index_Order[2]	= TABLE_INDEX_None;

	if( Field_1 >= 0 && Field_1 < m_nFields && Order_1 != TABLE_INDEX_None )
	{
		m_Index_Field[0]	= Field_1;
		m_Index_Order[0]	= Order_1;

		if( Field_2 >= 0 && Field_2 < m_nFields && Order_2 != TABLE_INDEX_None )
		{
			m_Index_Field[1]	= Field_2;
			m_Index_Order[1]	= Order_2;

			if( Field_3 >= 0 && Field_3 < m_nFields && Order_3 != TABLE_INDEX_None )
			{
				m_Index_Field[2]	= Field_3;
				m_Index_Order[2]	= Order_3;
			}
		}

		_Index_Create();
	}
	else
	{
		_Index_Destroy();
	}

	//-----------------------------------------------------
	if( Get_Selection_Count() > 0 )
	{
		size_t	n	= 0;

		for(int i=0; i<m_nRecords && n<Get_Selection_Count(); i++)
		{
			CSG_Table_Record	*pRecord	= Get_Record_byIndex(i);

			if( pRecord && pRecord->is_Selected() )
			{
				_Set_Selection(pRecord->Get_Index(), n++);
			}
		}
	}

	return( is_Indexed() );
}

//---------------------------------------------------------
bool CSG_Table::Del_Index(void)
{
	_Index_Destroy();

	return( is_Indexed() );
}

//---------------------------------------------------------
bool CSG_Table::Toggle_Index(int iField)
{
	if( iField >= 0 && iField < m_nFields )
	{
		if( iField != m_Index_Field[0] )
		{
			return( Set_Index(iField, TABLE_INDEX_Ascending) );
		}
		else if( m_Index_Order[0] == TABLE_INDEX_Ascending )
		{
			return( Set_Index(iField, TABLE_INDEX_Descending) );
		}
		else
		{
			return( Set_Index(iField, TABLE_INDEX_None) );
		}
	}

	return( false );
}

//---------------------------------------------------------
#define SORT_SWAP(a,b)	{itemp=(a);(a)=(b);(b)=itemp;}

void CSG_Table::_Index_Create(void)
{
	const int	M	= 7;

	int		indxt, itemp, *istack,
			i, j, k, a,
			l		= 0,
			ir		= m_nRecords - 1,
			nstack	= 64,
			jstack	= 0;

	//-----------------------------------------------------
	if( m_Index == NULL || m_nBuffer < m_nRecords )
	{
		m_Index	= (int *)SG_Realloc(m_Index, (m_nBuffer < m_nRecords ? m_nRecords : m_nBuffer) * sizeof(int));
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
	m_Index_Field[0]	= -1;

	if( m_Index )
	{
		SG_Free(m_Index);

		m_Index	= NULL;
	}
}

//---------------------------------------------------------
inline int CSG_Table::_Index_Compare(int a, int b)
{
	int		Result	= _Index_Compare(a, b, 0);

	if( Result == 0 && m_Index_Field[1] >= 0 )
	{
		Result	= _Index_Compare(a, b, 1);

		if( Result == 0 && m_Index_Field[2] >= 0 )
		{
			Result	= _Index_Compare(a, b, 2);
		}
	}

	return( Result );
}

//---------------------------------------------------------
inline int CSG_Table::_Index_Compare(int a, int b, int Field)
{
	double	Result;

	switch( m_Field_Type[m_Index_Field[Field]] )
	{
	case SG_DATATYPE_String:
	case SG_DATATYPE_Date:
		Result	= SG_STR_CMP(
				  Get_Record(a)->asString(m_Index_Field[Field]),
				  Get_Record(b)->asString(m_Index_Field[Field])
				);
		break;

	default:
		Result	= Get_Record(a)->asDouble(m_Index_Field[Field])
				- Get_Record(b)->asDouble(m_Index_Field[Field]);
		break;
	}

	return( m_Index_Order[Field] == TABLE_INDEX_Ascending
		? (Result < 0.0 ? -1 : (Result > 0.0 ? 1 : 0))
		: (Result > 0.0 ? -1 : (Result < 0.0 ? 1 : 0))
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
