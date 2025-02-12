
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
#include "table.h"
#include "shapes.h"
#include "tool_library.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
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
	case SG_DATAOBJECT_TYPE_Table     :
		return( new CSG_Table(Table) );

	case SG_DATAOBJECT_TYPE_Shapes    :
	case SG_DATAOBJECT_TYPE_PointCloud:
		return( SG_Create_Shapes(*((CSG_Shapes *)&Table)) );

	default:
		return( NULL );
	}
}

//---------------------------------------------------------
CSG_Table * SG_Create_Table(CSG_Table *pTemplate)
{
	if( pTemplate )
	{
		switch( pTemplate->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Table     :
			return( new CSG_Table(pTemplate) );

		case SG_DATAOBJECT_TYPE_Shapes    :
		case SG_DATAOBJECT_TYPE_PointCloud:
			return( SG_Create_Shapes((CSG_Shapes *)pTemplate) );

		default:
			break;
		}
	}

	return( new CSG_Table() );
}

//---------------------------------------------------------
CSG_Table * SG_Create_Table(const char       *File, TSG_Table_File_Type Format, int Encoding) { return( SG_Create_Table(CSG_String(File), Format, Encoding) ); }
CSG_Table * SG_Create_Table(const wchar_t    *File, TSG_Table_File_Type Format, int Encoding) { return( SG_Create_Table(CSG_String(File), Format, Encoding) ); }
CSG_Table * SG_Create_Table(const CSG_String &File, TSG_Table_File_Type Format, int Encoding)
{
	CSG_Table *pTable = new CSG_Table();

	if( pTable->Create(File, Format, Encoding) )
	{
		return( pTable );
	}

	delete(pTable); return( NULL );
}

//---------------------------------------------------------
CSG_Table * SG_Create_Table(const char       *File, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding) { return( SG_Create_Table(CSG_String(File), Format, Separator, Encoding) ); }
CSG_Table * SG_Create_Table(const wchar_t    *File, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding) { return( SG_Create_Table(CSG_String(File), Format, Separator, Encoding) ); }
CSG_Table * SG_Create_Table(const CSG_String &File, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding)
{
	CSG_Table *pTable = new CSG_Table();

	if( pTable->Create(File, Format, Separator, Encoding) )
	{
		return( pTable );
	}

	delete(pTable); return( NULL );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table::CSG_Table(void)
	: CSG_Data_Object()
{
	_On_Construction();
}

bool CSG_Table::Create(void)
{
	Destroy(); return( true );
}

//---------------------------------------------------------
CSG_Table::CSG_Table(const CSG_Table &Table)
	: CSG_Data_Object()
{
	_On_Construction(); Create(Table);
}

bool CSG_Table::Create(const CSG_Table &Table)
{
	return( Assign((CSG_Data_Object *)&Table) );
}

//---------------------------------------------------------
CSG_Table::CSG_Table(const char       *File, TSG_Table_File_Type Format, int Encoding) : CSG_Table(CSG_String(File), Format, Encoding) {}
CSG_Table::CSG_Table(const wchar_t    *File, TSG_Table_File_Type Format, int Encoding) : CSG_Table(CSG_String(File), Format, Encoding) {}
CSG_Table::CSG_Table(const CSG_String &File, TSG_Table_File_Type Format, int Encoding)
	: CSG_Data_Object()
{
	_On_Construction(); Create(File, Format, Encoding);
}

bool CSG_Table::Create(const char       *File, TSG_Table_File_Type Format, int Encoding) { return( Create(CSG_String(File), Format, Encoding) ); }
bool CSG_Table::Create(const wchar_t    *File, TSG_Table_File_Type Format, int Encoding) { return( Create(CSG_String(File), Format, Encoding) ); }
bool CSG_Table::Create(const CSG_String &File, TSG_Table_File_Type Format, int Encoding)
{
	Destroy();

	SG_UI_Msg_Add(CSG_String::Format("%s %s: %s...", _TL("Loading"), _TL("table"), File.c_str()), true);

	//-----------------------------------------------------
	bool bResult = File.BeforeFirst(':').Cmp("PGSQL") && SG_File_Exists(File) && Load(File, (int)Format, '\0', Encoding);

	if( bResult )
	{
		Set_File_Name(File, true);
	}

	//-----------------------------------------------------
	else if( File.BeforeFirst(':').Cmp("PGSQL") == 0 )	// database source
	{
		CSG_String s(File);

		s = s.AfterFirst(':'); CSG_String Host (s.BeforeFirst(':'));
		s = s.AfterFirst(':'); CSG_String Port (s.BeforeFirst(':'));
		s = s.AfterFirst(':'); CSG_String DBase(s.BeforeFirst(':'));
		s = s.AfterFirst(':'); CSG_String Table(s.BeforeFirst(':'));

		CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("db_pgsql", 12, true); // CPGIS_Table_Load

		if( pTool )
		{
			SG_UI_ProgressAndMsg_Lock(true);

			CSG_String Connection(DBase + " [" + Host + ":" + Port + "]");

			bResult = pTool->Set_Manager(NULL) && pTool->On_Before_Execution()
			       && pTool->Set_Parameter("CONNECTION", Connection)
			       && pTool->Set_Parameter("DB_TABLE"  , Table     )
			       && pTool->Set_Parameter("TABLE"     , this      )
			       && pTool->Execute();

			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

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
CSG_Table::CSG_Table(const char       *File, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding) : CSG_Table(CSG_String(File), Format, Separator, Encoding) {}
CSG_Table::CSG_Table(const wchar_t    *File, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding) : CSG_Table(CSG_String(File), Format, Separator, Encoding) {}
CSG_Table::CSG_Table(const CSG_String &File, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding)
	: CSG_Data_Object()
{
	_On_Construction(); Create(File, Format, Separator, Encoding);
}

bool CSG_Table::Create(const char       *File, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding) { return( Create(CSG_String(File), Format, Separator, Encoding) ); }
bool CSG_Table::Create(const wchar_t    *File, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding) { return( Create(CSG_String(File), Format, Separator, Encoding) ); }
bool CSG_Table::Create(const CSG_String &File, TSG_Table_File_Type Format, const SG_Char Separator, int Encoding)
{
	return( Load(File, (int)Format, Separator, Encoding) );
}

//---------------------------------------------------------
CSG_Table::CSG_Table(const CSG_Table *pTemplate)
	: CSG_Data_Object()
{
	_On_Construction(); Create(pTemplate);
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
	Set_NoData_Value_Range(pTemplate->Get_NoData_Value(), pTemplate->Get_NoData_Value(true));

	m_Encoding = pTemplate->m_Encoding;

	for(int i=0; i<pTemplate->Get_Field_Count(); i++)
	{
		Add_Field(pTemplate->Get_Field_Name(i), pTemplate->Get_Field_Type(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Table::_On_Construction(void)
{
	m_nFields     = 0;
	m_Field_Name  = NULL;
	m_Field_Type  = NULL;
	m_Field_Stats = NULL;

	m_Records     = NULL;
	m_nRecords    = 0;
	m_nBuffer     = 0;

	m_Encoding    = SG_FILE_ENCODING_UTF8;

	m_Selection.Create(sizeof(sLong), 0, TSG_Array_Growth::SG_ARRAY_GROWTH_3);

	Set_Update_Flag();
}


///////////////////////////////////////////////////////////
//                                                       //
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

		SG_Free(m_Field_Name );
		SG_Free(m_Field_Stats);
		SG_Free(m_Field_Type );

		m_nFields     = 0;
		m_Field_Name  = NULL;
		m_Field_Type  = NULL;
		m_Field_Stats = NULL;
	}

	CSG_Data_Object::Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Assign							 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table & CSG_Table::operator = (const CSG_Table &Table)
{
	Create(Table);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Table::Assign(CSG_Data_Object *pObject, bool bProgress)
{
	if( pObject && pObject->asTable(true) && CSG_Data_Object::Assign(pObject) )
	{
		CSG_Table *pTable = pObject->asTable(true);

		m_Encoding = pTable->m_Encoding;

		for(int i=0; i<pTable->Get_Field_Count(); i++)
		{
			Add_Field(pTable->Get_Field_Name(i), pTable->Get_Field_Type(i));
		}

		for(sLong i=0; i<pTable->Get_Count() && (!bProgress || SG_UI_Process_Set_Progress(i, pTable->Get_Count())); i++)
		{
			Add_Record(pTable->Get_Record(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Assign_Values(const CSG_Table &Table)
{
	if( is_Compatible(Table) && Set_Count(Table.Get_Count()) )
	{
		for(sLong i=0; i<Table.Get_Count(); i++)
		{
			Get_Record(i)->Assign(Table.Get_Record(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Assign_Values(CSG_Table *pTable)
{
	return( pTable && Assign_Values(*pTable) );
}

//---------------------------------------------------------
bool CSG_Table::Assign_Values(const SG_Char *FileName)
{
	CSG_Table Table; return( Table.Create(FileName) && Assign_Values(&Table) );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Checks							 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::is_Compatible(const CSG_Table &Table, bool bExactMatch) const
{
	if( Get_Field_Count() == Table.Get_Field_Count() )
	{
		if( bExactMatch )
		{
			for(int i=0; i<Get_Field_Count(); i++)
			{
				if( Get_Field_Type(i) != Table.Get_Field_Type(i) )
				{
					return( false );
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::is_Compatible(CSG_Table *pTable, bool bExactMatch) const
{
	return( pTable && is_Compatible(*pTable, bExactMatch) );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Fields							 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Add_Field(const CSG_String &Name, TSG_Data_Type Type, int Position)
{
	if( Position < 0 || Position > m_nFields )
	{
		Position	= m_nFields;
	}

	//-----------------------------------------------------
	m_nFields++;

	m_Field_Name	= (CSG_String            **)SG_Realloc(m_Field_Name , m_nFields * sizeof(CSG_String            *));
	m_Field_Type	= (TSG_Data_Type          *)SG_Realloc(m_Field_Type , m_nFields * sizeof(TSG_Data_Type          ));
	m_Field_Stats	= (CSG_Simple_Statistics **)SG_Realloc(m_Field_Stats, m_nFields * sizeof(CSG_Simple_Statistics *));

	//-----------------------------------------------------
	for(int i=m_nFields-1; i>Position; i--)
	{
		m_Field_Name [i]	= m_Field_Name [i - 1];
		m_Field_Type [i]	= m_Field_Type [i - 1];
		m_Field_Stats[i]	= m_Field_Stats[i - 1];
	}

	//-----------------------------------------------------
	m_Field_Name [Position]	= new CSG_String(!Name.is_Empty() ? Name : CSG_String::Format("FIELD_%d", m_nFields));
	m_Field_Type [Position]	= Type;
	m_Field_Stats[Position]	= new CSG_Simple_Statistics();

	//-----------------------------------------------------
	for(sLong i=0; i<m_nRecords; i++)
	{
		m_Records[i]->_Add_Field(Position);
	}

	Set_Modified();

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::Del_Field(int del_Field)
{
	if( del_Field < 0 || del_Field >= m_nFields )
	{
		return( false );
	}

	//-----------------------------------------------------
	delete(m_Field_Name [del_Field]);
	delete(m_Field_Stats[del_Field]);

	//-------------------------------------------------
	m_nFields--;

	for(int i=del_Field; i<m_nFields; i++)
	{
		m_Field_Name [i]	= m_Field_Name [i + 1];
		m_Field_Type [i]	= m_Field_Type [i + 1];
		m_Field_Stats[i]	= m_Field_Stats[i + 1];
	}

	//-------------------------------------------------
	m_Field_Name	= (CSG_String            **)SG_Realloc(m_Field_Name , m_nFields * sizeof(CSG_String            *));
	m_Field_Type	= (TSG_Data_Type          *)SG_Realloc(m_Field_Type , m_nFields * sizeof(TSG_Data_Type          ));
	m_Field_Stats	= (CSG_Simple_Statistics **)SG_Realloc(m_Field_Stats, m_nFields * sizeof(CSG_Simple_Statistics *));

	//-------------------------------------------------
	for(sLong i=0; i<m_nRecords; i++)
	{
		m_Records[i]->_Del_Field(del_Field);
	}

	Set_Modified();

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::Mov_Field(int iField, int Position)
{
	if( Position < 0 )
	{
		Position	= 0;
	}
	else if( Position >= m_nFields - 1 )
	{
		Position	= m_nFields - 1;
	}

	if( iField < 0 || iField >= m_nFields || iField == Position )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Position > iField )
	{
		Position++;
	}

	if( !Add_Field(Get_Field_Name(iField), Get_Field_Type(iField), Position) )
	{
		return( false );
	}

	if( Position < iField )
	{
		iField++;
	}

	#pragma omp parallel for
	for(sLong i=0; i<m_nRecords; i++)
	{
		*m_Records[i]->Get_Value(Position) = *m_Records[i]->Get_Value(iField);
	}

	if( !Del_Field(iField) )
	{
		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

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
bool CSG_Table::Set_Field_Type(int Field, TSG_Data_Type Type)
{
	if( Field < 0 || Field >= m_nFields )
	{
		return( false );
	}

	if( m_Field_Type[Field] == Type )
	{
		return( true );
	}

	#pragma omp parallel for
	for(sLong i=0; i<m_nRecords; i++)
	{
		CSG_Table_Record *pRecord = m_Records[i];

		bool bNoData = pRecord->is_NoData(Field);

		CSG_Table_Value *pValue = pRecord->m_Values[Field];

		pRecord->m_Values[Field] = CSG_Table_Record::_Create_Value(Type);

		if( bNoData )
		{
			if( Type != SG_DATATYPE_String && Type != SG_DATATYPE_Binary )
			{
				pRecord->m_Values[Field]->Set_Value(Get_NoData_Value());
			}
		}
		else if( pValue->Get_Type() == SG_TABLE_VALUE_TYPE_String && SG_Data_Type_is_Numeric(Type) )
		{
			if( !pRecord->m_Values[Field]->Set_Value(pValue->asString()) )
			{
				pRecord->m_Values[Field]->Set_Value(Get_NoData_Value());
			}
		}
		else
		{
			*(pRecord->m_Values[Field]) = *pValue;
		}

		delete(pValue);

		pRecord->m_Flags |= SG_TABLE_REC_FLAG_Modified;
	}

	m_Field_Type[Field] = Type;

	Set_Update_Flag(); _Stats_Invalidate(Field); Set_Modified();

	return( true );
}

//---------------------------------------------------------
/**
* Returns the maximum number of characters for data type string
* or the number of bytes used for all other data types.
*/
//---------------------------------------------------------
int CSG_Table::Get_Field_Length(int Field, int Encoding) const
{
	size_t Length = 0;

	if( Field >= 0 && Field < m_nFields )
	{
		switch( m_Field_Type[Field] )
		{
		default:
			Length = SG_Data_Type_Get_Size(m_Field_Type[Field]);
			break;

		case SG_DATATYPE_Date:
			Length = 10; // => YYYY-MM-DD
			break;

		case SG_DATATYPE_String:
			for(sLong i=0; i<m_nRecords; i++)
			{
				CSG_String s(m_Records[i]->asString(Field));

				size_t nBytes;

				switch( Encoding )
				{
				default                      :
				case SG_FILE_ENCODING_UTF7   : nBytes = s.Length()            ; break;
				case SG_FILE_ENCODING_UTF8   : nBytes = s.to_UTF8().Get_Size(); break;
				case SG_FILE_ENCODING_UTF16LE:
				case SG_FILE_ENCODING_UTF16BE: nBytes = s.Length() * 2        ; break;
				case SG_FILE_ENCODING_UTF32LE:
				case SG_FILE_ENCODING_UTF32BE: nBytes = s.Length() * 4        ; break;
				}

				if( Length < nBytes )
				{
					Length = nBytes;
				}
			}
			break;
		}
	}

	return( (int)Length );
}

//---------------------------------------------------------
int CSG_Table::Get_Field(const char       *Name) const { return( Get_Field(CSG_String(Name))); }
int CSG_Table::Get_Field(const wchar_t    *Name) const { return( Get_Field(CSG_String(Name))); }
int CSG_Table::Get_Field(const CSG_String &Name) const
{
	for(int Field=0; Field<Get_Field_Count(); Field++)
	{
		if( !Name.Cmp(Get_Field_Name(Field)) )
		{
			return( Field );
		}
	}

	return( -1 );
}

//---------------------------------------------------------
/**
* Returns the index of the field with the requested name or
* -1 if it does not exists.
*/
int CSG_Table::Find_Field(const CSG_String &Name) const
{
	for(int i=0; i<Get_Field_Count(); i++)
	{
		if( !Name.Cmp(Get_Field_Name(i)) )
		{
			return( i );
		}
	}

	return( -1 );
}

//---------------------------------------------------------
/**
* Returns true if a field with the requested name exists and
* sets the Index parameter to its position.
*/
bool CSG_Table::Find_Field(const CSG_String &Name, int &Index) const
{
	return( (Index = Find_Field(Name)) >= 0 );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Records							 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_GROW_SIZE(n)	(n < 256 ? 1 : (n < 8192 ? 128 : 1024))

//---------------------------------------------------------
bool CSG_Table::_Inc_Array(void)
{
	if( m_nRecords < m_nBuffer )
	{
		return( true );
	}

	CSG_Table_Record **pRecords = (CSG_Table_Record **)SG_Realloc(m_Records, (m_nBuffer + GET_GROW_SIZE(m_nBuffer)) * sizeof(CSG_Table_Record *));

	if( pRecords == NULL )
	{
		return( false );
	}

	m_Records  = pRecords;
	m_nBuffer += GET_GROW_SIZE(m_nBuffer);

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::_Dec_Array(void)
{
	if( m_nRecords < 0 || m_nRecords >= m_nBuffer - GET_GROW_SIZE(m_nBuffer) )
	{
		return( true );
	}

	CSG_Table_Record	**pRecords	= (CSG_Table_Record **)SG_Realloc(m_Records, (m_nBuffer - GET_GROW_SIZE(m_nBuffer)) * sizeof(CSG_Table_Record *));

	if( pRecords == NULL )
	{
		return( false );
	}

	m_Records	= pRecords;
	m_nBuffer	-= GET_GROW_SIZE(m_nBuffer);

	return( true );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::_Get_New_Record(sLong Index)
{
	return( new CSG_Table_Record(this, Index) );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::Add_Record(CSG_Table_Record *pCopy)
{
	return( Ins_Record(m_nRecords, pCopy) );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::Ins_Record(sLong iRecord, CSG_Table_Record *pCopy)
{
	if( iRecord < 0 ) { iRecord = 0; } else if( iRecord > m_nRecords ) { iRecord = m_nRecords; }

	CSG_Table_Record *pRecord = _Inc_Array() ? _Get_New_Record(m_nRecords) : NULL;

	if( pRecord )
	{
		if( pCopy )
		{
			pRecord->Assign(pCopy);
		}

		//-------------------------------------------------
		if( iRecord < m_nRecords )
		{
			if( Get_Selection_Count() > 0 )	// update selection index
			{
				sLong *Selection = (sLong *)m_Selection.Get_Array();

				for(sLong i=0; i<m_Selection.Get_Size(); i++)
				{
					if( Selection[i] > iRecord )
					{
						Selection[i]++;
					}
				}
			}

			for(sLong i=m_nRecords; i>iRecord; i--)
			{
				m_Records[i] = m_Records[i - 1]; m_Records[i]->m_Index = i;
			}

			pRecord->m_Index = iRecord;
		}

		m_Records[iRecord] = pRecord;
		m_nRecords++;

		//-------------------------------------------------
		if( m_Index.is_Okay() )
		{
			m_Index.Add_Entry(iRecord);
		}

		Set_Modified();

		Set_Update_Flag();

		_Stats_Invalidate();
	}

	return( pRecord );
}

//---------------------------------------------------------
bool CSG_Table::Set_Record(sLong iRecord, CSG_Table_Record *pCopy)
{
	if( iRecord >= 0 && iRecord < m_nRecords && pCopy )
	{
		return( m_Records[iRecord]->Assign(pCopy) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Del_Record(sLong iRecord)
{
	if( iRecord >= 0 && iRecord < m_nRecords )
	{
		if( m_Records[iRecord]->is_Selected() )
		{
			_Del_Selection(iRecord);
		}

		delete(m_Records[iRecord]);

		m_nRecords--;

		for(sLong i=iRecord; i<m_nRecords; i++)
		{
			m_Records[i] = m_Records[i + 1]; m_Records[i]->m_Index = i;
		}

		_Dec_Array();

		//-------------------------------------------------
		if( m_Index.is_Okay() )
		{
			m_Index.Del_Entry(iRecord);
		}

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
	Del_Index();

	for(sLong iRecord=0; iRecord<m_nRecords; iRecord++)
	{
		delete(m_Records[iRecord]);
	}

	SG_FREE_SAFE(m_Records);

	m_nRecords = 0;
	m_nBuffer  = 0;

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::Set_Count(sLong nRecords)
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Find_Record(sLong &iRecord, int iField, const CSG_String &Value, bool bCreateIndex)
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
		#define GET_RECORD(i) Get_Record_byIndex(bAscending ? (iRecord = i) : m_nRecords - 1 - (iRecord = i))

		double d; bool bAscending = Get_Index_Order(0) == TABLE_INDEX_Ascending;

		if( (d = Value.Cmp(GET_RECORD(0             )->asString(iField))) < 0 ) { return( false ); } else if( d == 0 ) { return( true ); }
		if( (d = Value.Cmp(GET_RECORD(m_nRecords - 1)->asString(iField))) > 0 ) { return( false ); } else if( d == 0 ) { return( true ); }

		for(sLong a=0, b=m_nRecords-1; b-a > 1; )
		{
			d = Value.Cmp(GET_RECORD(a + (b - a) / 2)->asString(iField));

			if( d > 0. )
			{
				a = iRecord;
			}
			else if( d < 0. )
			{
				b = iRecord;
			}
			else
			{
				iRecord = Get_Record_byIndex(bAscending ? iRecord : m_nRecords - 1 - iRecord)->Get_Index();

				return( true );
			}
		}

		iRecord = Get_Record_byIndex(bAscending ? iRecord : m_nRecords - 1 - iRecord)->Get_Index();
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::Find_Record(int iField, const CSG_String &Value, bool bCreateIndex)
{
	sLong iRecord;

	if( Find_Record(iRecord, iField, Value, bCreateIndex) )
	{
		return( Get_Record(iRecord) );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Table::Find_Record(sLong &iRecord, int iField, double Value, bool bCreateIndex)
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

		double d; bool bAscending = Get_Index_Order(0) == TABLE_INDEX_Ascending;

		if( (d = Value - GET_RECORD(0             )->asDouble(iField)) < 0. ) { return( false ); } else if( d == 0. ) { return( true ); }
		if( (d = Value - GET_RECORD(m_nRecords - 1)->asDouble(iField)) > 0. ) { return( false ); } else if( d == 0. ) { return( true ); }

		for(sLong a=0, b=m_nRecords-1; b-a > 1; )
		{
			d = Value - GET_RECORD(a + (b - a) / 2)->asDouble(iField);

			if( d > 0. )
			{
				a = iRecord;
			}
			else if( d < 0. )
			{
				b = iRecord;
			}
			else
			{
				iRecord = Get_Record_byIndex(bAscending ? iRecord : m_nRecords - 1 - iRecord)->Get_Index();

				return( true );
			}
		}

		iRecord = Get_Record_byIndex(bAscending ? iRecord : m_nRecords - 1 - iRecord)->Get_Index();
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
CSG_Table_Record * CSG_Table::Find_Record(int iField, double Value, bool bCreateIndex)
{
	sLong iRecord;

	if( Find_Record(iRecord, iField, Value, bCreateIndex) )
	{
		return( Get_Record(iRecord) );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Value Access					 //
//                                                       //
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
			for(sLong iRecord=0; iRecord<m_nRecords; iRecord++)
			{
				m_Records[iRecord]->Set_Modified(false);
			}
		}
	}
}

//---------------------------------------------------------
bool CSG_Table::Set_Value(sLong iRecord, int iField, const SG_Char  *Value)
{
	if( iField >= 0 && iField < m_nFields )
	{
		CSG_Table_Record *pRecord = Get_Record(iRecord);

		return( pRecord && pRecord->Set_Value(iField, Value) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Set_Value(sLong iRecord, int iField, double       Value)
{
	if( iField >= 0 && iField < m_nFields )
	{
		CSG_Table_Record *pRecord = Get_Record(iRecord);

		return( pRecord && pRecord->Set_Value(iField, Value) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Get_Value(sLong iRecord, int iField, CSG_String &Value) const
{
	if( iField >= 0 && iField < m_nFields )
	{
		CSG_Table_Record *pRecord = Get_Record(iRecord);

		if( pRecord )
		{
			Value = pRecord->asString(iField);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Get_Value(sLong iRecord, int iField, double      &Value) const
{
	if( iField >= 0 && iField < m_nFields )
	{
		CSG_Table_Record *pRecord = Get_Record(iRecord);

		if( pRecord )
		{
			Value = pRecord->asDouble(iField);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Statistics						 //
//                                                       //
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
	if( iField < 0 || iField >= m_nFields || Get_Count() < 1 )
	{
		return( false );
	}

	CSG_Simple_Statistics &Statistics = *m_Field_Stats[iField];

	if( Statistics.is_Evaluated() )
	{
		return( true );
	}

	if( Get_Max_Samples() > 0 && Get_Max_Samples() < Get_Count() )
	{
		double d = (double)Get_Count() / (double)Get_Max_Samples();

		for(double i=0; i<(double)Get_Count(); i+=d)
		{
			CSG_Table_Record *pRecord = m_Records[(sLong)i];

			if( !pRecord->is_NoData(iField) )
			{
				Statistics += pRecord->asDouble(iField);
			}
		}

		Statistics.Set_Count(Statistics.Get_Count() >= Get_Max_Samples() ? Get_Count()	// any no-data cells ?
			: (sLong)(Get_Count() * (double)Statistics.Get_Count() / (double)Get_Max_Samples())
		);
	}
	else
	{
		for(sLong i=0; i<Get_Count(); i++)
		{
			CSG_Table_Record *pRecord = m_Records[i];

			if( !pRecord->is_NoData(iField) )
			{
				Statistics += pRecord->asDouble(iField);
			}
		}
	}

	return( Statistics.Evaluate() );	// evaluate! prevent values to be added more than once!
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::On_NoData_Changed(void)
{
	_Stats_Invalidate();

	return( CSG_Data_Object::On_NoData_Changed() );
}

//---------------------------------------------------------
bool CSG_Table::On_Update(void)
{
	_Stats_Invalidate();

	if( m_Index.is_Okay() )
	{
		_Index_Update();
	}

	return( CSG_Data_Object::On_Update() );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Sort							 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Sort(const char       *Field, bool bAscending) { return( Sort(Get_Field(Field), bAscending) ); }
bool CSG_Table::Sort(const wchar_t    *Field, bool bAscending) { return( Sort(Get_Field(Field), bAscending) ); }
bool CSG_Table::Sort(const CSG_String &Field, bool bAscending) { return( Sort(Get_Field(Field), bAscending) ); }
bool CSG_Table::Sort(int               Field, bool bAscending)
{
	CSG_Index Index; return( Set_Index(Index, Field, bAscending) && Sort(Index) );
}

//---------------------------------------------------------
bool CSG_Table::Sort(const CSG_Index &Index)
{
	if( Get_Count() > 0 && Get_Count() == Index.Get_Count() )
	{
		CSG_Table_Record **Records = m_Records; m_Records = (CSG_Table_Record **)SG_Malloc(m_nBuffer * sizeof(CSG_Table_Record *));

		for(sLong i=0; i<Get_Count(); i++)
		{
			m_Records[i] = Records[Index[i]];
		}

		SG_Free(Records);

		Del_Index();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//						Index							 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Set_Index(int Field_1, TSG_Table_Index_Order Order_1, int Field_2, TSG_Table_Index_Order Order_2, int Field_3, TSG_Table_Index_Order Order_3)
{
	m_Index_Fields.Destroy();

	if( Field_1 >= 0 && Field_1 < m_nFields && Order_1 != TABLE_INDEX_None )
	{
		Field_1++; m_Index_Fields += Order_1 == TABLE_INDEX_Ascending ? Field_1 : -Field_1;

		if( Field_2 >= 0 && Field_2 < m_nFields && Order_2 != TABLE_INDEX_None )
		{
			Field_2++; m_Index_Fields += Order_2 == TABLE_INDEX_Ascending ? Field_2 : -Field_2;

			if( Field_3 >= 0 && Field_3 < m_nFields && Order_3 != TABLE_INDEX_None )
			{
				Field_3++; m_Index_Fields += Order_3 == TABLE_INDEX_Ascending ? Field_3 : -Field_3;
			}
		}

		_Index_Update();
	}
	else
	{
		Del_Index();
	}

	return( is_Indexed() );
}

//---------------------------------------------------------
bool CSG_Table::Del_Index(void)
{
	m_Index.Destroy();
	
	m_Index_Fields.Destroy();

	return( true );
}

//---------------------------------------------------------
bool CSG_Table::Toggle_Index(int iField)
{
	if( iField < 0 || iField >= m_nFields )
	{
		return( false );
	}

	if( iField != Get_Index_Field(0) )
	{
		return( Set_Index(iField, TABLE_INDEX_Ascending) );
	}

	if( Get_Index_Order(0) == TABLE_INDEX_Ascending )
	{
		return( Set_Index(iField, TABLE_INDEX_Descending) );
	}

	return( Del_Index() );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Record_Compare_Field : public CSG_Index::CSG_Index_Compare
{
public:
	CSG_Table_Record_Compare_Field(const CSG_Table *pTable, int Field, bool Ascending)
	{
		m_pTable	= pTable;
		m_Field		= Field;
		m_Ascending	= Ascending;

		if( !m_pTable || m_Field < 0 || m_Field >= m_pTable->Get_Field_Count() )
		{
			m_pTable	= NULL;
		}
	}

	bool				is_Okay		(void)	const	{	return( m_pTable != NULL );	}

	virtual int			Compare		(const sLong _a, const sLong _b)
	{
		sLong a = m_Ascending ? _a : _b;
		sLong b = m_Ascending ? _b : _a;

		switch( m_pTable->Get_Field_Type(m_Field) )
		{
		default: {
			double d =
				m_pTable->Get_Record(a)->asDouble(m_Field) -
				m_pTable->Get_Record(b)->asDouble(m_Field);

			return( d < 0. ? -1 : d > 0. ? 1 : 0 );
		}

		case SG_DATATYPE_String:
		case SG_DATATYPE_Date  :
			return( SG_STR_CMP(
				m_pTable->Get_Record(a)->asString(m_Field),
				m_pTable->Get_Record(b)->asString(m_Field))
			);
		}
	}


private:

	bool				m_Ascending;

	int					m_Field;

	const CSG_Table		*m_pTable;

};

//---------------------------------------------------------
bool CSG_Table::Set_Index(CSG_Index &Index, int Field, bool bAscending) const
{
	CSG_Table_Record_Compare_Field Compare(this, Field, bAscending);

	return( Compare.is_Okay() && Index.Create(Get_Count(), &Compare) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_Record_Compare_Fields : public CSG_Index::CSG_Index_Compare
{
public:
	CSG_Table_Record_Compare_Fields(const CSG_Table *pTable, int Fields[], int nFields, bool Ascending)
		: m_Fields(Fields), m_nFields(nFields), m_pTable(pTable)
	{
		m_Ascending.Create(nFields);

		for(int i=0; m_pTable && i<m_nFields; i++)
		{
			if( m_Fields[i] >= m_pTable->Get_Field_Count() )
			{
				m_pTable = NULL;
			}

			m_Ascending[i] = Ascending ? 1 : 0;
		}
	}

	CSG_Table_Record_Compare_Fields(const CSG_Table *pTable, int Fields[], int nFields, int Ascending[])
		: m_Fields(Fields), m_nFields(nFields), m_pTable(pTable)
	{
		m_Ascending.Create(nFields);

		for(int i=0; m_pTable && i<m_nFields; i++)
		{
			if( m_Fields[i] >= m_pTable->Get_Field_Count() )
			{
				m_pTable = NULL;
			}

			m_Ascending[i] = Ascending[i] > 0 ? 1 : 0;
		}
	}

	bool				is_Okay		(void)	const	{	return( m_pTable != NULL );	}

	virtual int			Compare		(const sLong _a, const sLong _b)
	{
		int	Difference	= 0;

		for(int i=0; !Difference && i<m_nFields; i++)
		{
			int	Field = m_Fields[i];

			sLong a = m_Ascending[i] ? _a : _b;
			sLong b = m_Ascending[i] ? _b : _a;

			switch( m_pTable->Get_Field_Type(Field) )
			{
			default: {
				double d = m_pTable->Get_Record(a)->asDouble(Field);
				d       -= m_pTable->Get_Record(b)->asDouble(Field);
				Difference = d < 0. ? -1 : d > 0. ? 1 : 0;
			}	break;

			case SG_DATATYPE_String:
			case SG_DATATYPE_Date  :
				CSG_String   s    (m_pTable->Get_Record(a)->asString(Field));
				Difference = s.Cmp(m_pTable->Get_Record(b)->asString(Field));
				break;
			}
		}

		return( Difference );
	}


private:

	int					*m_Fields, m_nFields;

	CSG_Array_Int		m_Ascending;

	const CSG_Table		*m_pTable;

};

//---------------------------------------------------------
bool CSG_Table::Set_Index(CSG_Index &Index, int Fields[], int nFields, bool bAscending) const
{
	CSG_Table_Record_Compare_Fields	Compare(this, Fields, nFields, bAscending);

	return( Compare.is_Okay() && Index.Create(Get_Count(), &Compare) );
}

//---------------------------------------------------------
bool CSG_Table::Set_Index(CSG_Index &Index, const CSG_Array_Int &Fields, bool bAscending) const
{
	return( Set_Index(Index, Fields.Get_Array(), (int)Fields.Get_Size(), bAscending) );
}

//---------------------------------------------------------
void CSG_Table::_Index_Update(void)
{
	if( m_Index_Fields.Get_Size() < 1 )
	{
		Del_Index();
	}

	CSG_Array_Int Fields, Ascending;

	for(sLong i=0; i<m_Index_Fields.Get_Size(); i++)
	{
		Fields    += abs(m_Index_Fields[i]) - 1;
		Ascending +=     m_Index_Fields[i] > 0 ? 1 : 0;
	}

	CSG_Table_Record_Compare_Fields	Compare(this, Fields.Get_Array(), (int)Fields.Get_Size(), Ascending.Get_Array());

	if( !Compare.is_Okay() || !m_Index.Create(Get_Count(), &Compare) )
	{
		Del_Index();
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
