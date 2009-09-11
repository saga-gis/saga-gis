
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     saga_api_db                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       odbc.cpp                        //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "odbc.h"

#include <wx/dbtable.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_ODBC_Connection::CSG_ODBC_Connection(void)
{
	m_pDBCInf	= NULL;
	m_pDB		= NULL;

	Create();
}

//---------------------------------------------------------
CSG_ODBC_Connection::~CSG_ODBC_Connection(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Create(void)
{
	Destroy();

	if( (m_pDBCInf = new wxDbConnectInf(NULL, SG_T(""), SG_T(""), SG_T(""), SG_T(""))) != NULL )
	{
		wxChar	DSName[1 + SQL_MAX_DSN_LENGTH], DSDesc[256];

		while( wxDbGetDataSource(m_pDBCInf->GetHenv(), DSName, 1 + SQL_MAX_DSN_LENGTH, DSDesc, 255) )
		{
			m_Servers	+= CSG_String::Format(SG_T("%s|"), DSName);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Destroy(void)
{
	if( m_pDBCInf )
	{
		Disconnect();

		m_pDBCInf->FreeHenv();

		delete(m_pDBCInf);

		m_pDBCInf	= NULL;

		m_Servers.Clear();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Connect(const CSG_String &Server, const CSG_String &User, const CSG_String &Password, const CSG_String &Directory)
{
	if( m_pDBCInf )
	{
		Disconnect();

		m_pDBCInf->SetDsn		(Server		.c_str());
		m_pDBCInf->SetUserID	(User		.c_str());	
		m_pDBCInf->SetPassword	(Password	.c_str());
		m_pDBCInf->SetDefaultDir(Directory	.c_str());

		if( (m_pDB = wxDbGetConnection(m_pDBCInf)) != NULL )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Disconnect(void)
{
	if( m_pDBCInf && m_pDB )
	{
		wxDbFreeConnection(m_pDB);

		m_pDB		= NULL;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_String CSG_ODBC_Connection::Get_Server(void)
{
	CSG_String	Server;

	if( m_pDBCInf && m_pDB )
	{
		Server	= m_pDBCInf->GetDsn();
	}

	return( Server );
}

//---------------------------------------------------------
//	bool CSG_ODBC_Connection::is_Oracle			(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsORACLE		);	}
//	bool CSG_ODBC_Connection::is_Sybase_ASA		(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsSYBASE_ASA	);	}	// Adaptive Server Anywhere
//	bool CSG_ODBC_Connection::is_Sybase_ASE		(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsSYBASE_ASE	);	}	// Adaptive Server Enterprise
//	bool CSG_ODBC_Connection::is_MS_SQL_Server	(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsMS_SQL_SERVER );	}
//	bool CSG_ODBC_Connection::is_My_SQL			(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsMY_SQL		);	}
bool CSG_ODBC_Connection::is_Postgres		(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsPOSTGRES		);	}
//	bool CSG_ODBC_Connection::is_Access			(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsACCESS		);	}
//	bool CSG_ODBC_Connection::is_DBase			(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsDBASE			);	}
//	bool CSG_ODBC_Connection::is_Informix		(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsINFORMIX		);	}
//	bool CSG_ODBC_Connection::is_Virtuoso		(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsVIRTUOSO		);	}
//	bool CSG_ODBC_Connection::is_DB2			(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsDB2			);	}
//	bool CSG_ODBC_Connection::is_Interbase		(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsINTERBASE		);	}
//	bool CSG_ODBC_Connection::is_Pervasive_SQL	(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsPERVASIVE_SQL );	}
//	bool CSG_ODBC_Connection::is_XBase_Sequiter	(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsXBASE_SEQUITER);	}
//	bool CSG_ODBC_Connection::is_Firebird		(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsFIREBIRD		);	}
//	bool CSG_ODBC_Connection::is_MaxDB			(void)	{	return( m_pDB && m_pDB->Dbms() == dbmsMAXDB			);	}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_ODBC_Connection::_Error_Message(const SG_Char *Message, const SG_Char *Additional)
{
	SG_UI_Msg_Add(Message, true, SG_UI_MSG_STYLE_FAILURE);

	if( m_pDB )
	{
		CSG_String	s(Message);

		s	+= SG_T(":\n");

	//	s	+= CSG_String::Format(SG_T("%s\t>> %s\n"), _TL("user id") , m_pDB->GetUsername());
	//	s	+= CSG_String::Format(SG_T("%s\t>> %s\n"), _TL("password"), m_pDB->GetPassword());

		if( Additional )
		{
			s	+= Additional;
			s	+= SG_T("\n");
		}

		SG_UI_Msg_Add_Error(wxDbLogExtendedErrorMsg(s.c_str(), m_pDB, 0, 0));
	}
	else
	{
		SG_UI_Msg_Add_Error(Message);
	}
}

//---------------------------------------------------------
int CSG_ODBC_Connection::_Get_Type_To_SQL(TSG_Table_Field_Type Type)
{
	switch( Type )
	{
	case TABLE_FIELDTYPE_String:	return( DB_DATA_TYPE_VARCHAR );	// strings

	case TABLE_FIELDTYPE_Color:
	case TABLE_FIELDTYPE_Char:
	case TABLE_FIELDTYPE_Short:
	case TABLE_FIELDTYPE_Int:
	case TABLE_FIELDTYPE_Long:		return( DB_DATA_TYPE_INTEGER );	// non-floating point numbers

	case TABLE_FIELDTYPE_Float:
	case TABLE_FIELDTYPE_Double:	return( DB_DATA_TYPE_FLOAT   );	// floating point numbers

	case TABLE_FIELDTYPE_Date:		return( DB_DATA_TYPE_DATE    );	// dates
	}

	return( DB_DATA_TYPE_VARCHAR );
}

//---------------------------------------------------------
TSG_Table_Field_Type CSG_ODBC_Connection::_Get_Type_From_SQL(int Type)
{
	switch( Type )
	{
	case DB_DATA_TYPE_BLOB:			return( TABLE_FIELDTYPE_String );
	case DB_DATA_TYPE_MEMO:			return( TABLE_FIELDTYPE_String );
	case DB_DATA_TYPE_DATE:			return( TABLE_FIELDTYPE_Date   );
	case DB_DATA_TYPE_VARCHAR:		return( TABLE_FIELDTYPE_String );
	case DB_DATA_TYPE_INTEGER:		return( TABLE_FIELDTYPE_Int    );
	case DB_DATA_TYPE_FLOAT:		return( TABLE_FIELDTYPE_Double );
	}

	return( TABLE_FIELDTYPE_String );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_ODBC_Connection::Get_Tables(void)
{
	CSG_String	Tables;
	wxDbInf		*pCatalog;

	if( m_pDB && (pCatalog = m_pDB->GetCatalog(SG_T(""))) != NULL )
	{
		for(int i=0; i<pCatalog->numTables; i++)
		{
			Tables	+= CSG_String::Format(SG_T("%s|"), pCatalog->pTableInf[i].tableName);
		}
	}

	return( Tables );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Execute(const CSG_String &SQL, bool bCommit)
{
	if( !m_pDB )
	{
		_Error_Message(_TL("no database connection"));

		return( false );
	}

	if( !m_pDB->ExecSql(SQL.c_str()) )
	{
		_Error_Message(_TL("sql excution error"), SQL);

		return( false );
	}

	if( bCommit )
	{
		return( Commit() );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Commit(void)
{
	if( !m_pDB )
	{
		_Error_Message(_TL("no database connection"));

		return( false );
	}

	if( !m_pDB->CommitTrans() )
	{
		_Error_Message(_TL("could not commit transactions"));

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Rollback(void)
{
	if( !m_pDB )
	{
		_Error_Message(_TL("no database connection"));

		return( false );
	}

	return( m_pDB->RollbackTrans() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Exists(const CSG_String &Table_Name)
{
	return( m_pDB && m_pDB->TableExists(Table_Name.c_str(), SG_T("")) );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Create(const CSG_String &Table_Name, const CSG_Table &Table, bool bCommit)
{
	if( !m_pDB )
	{
		_Error_Message(_TL("no database connection"));

		return( false );
	}

	if( Table.Get_Field_Count() <= 0 )
	{
		_Error_Message(_TL("no attributes in table"));

		return( false );
	}

	//-----------------------------------------------------
	wxDbTable	DbTable(m_pDB, Table_Name.c_str(), Table.Get_Field_Count(), wxT(""), !wxDB_QUERY_ONLY, wxT(""));

	for(int iField=0; iField<Table.Get_Field_Count(); iField++)
	{
		DbTable.SetColDefs(iField, Table.Get_Field_Name(iField), _Get_Type_To_SQL(Table.Get_Field_Type(iField)), NULL, SQL_C_WXCHAR, 255, true, true);
	}

	//-----------------------------------------------------
	if( !DbTable.CreateTable(false) )
	{
		_Error_Message(_TL("failed to create database table"));

		return( false );
	}

	if( bCommit )
	{
		return( Commit() );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Drop(const CSG_String &Table_Name, bool bCommit)
{
	if( !m_pDB )
	{
		_Error_Message(_TL("no database connection"));

		return( false );
	}

	if( !Table_Exists(Table_Name) )
	{
		_Error_Message(_TL("database table does not exist"));

		return( false );
	}

	wxDbTable	DBTable(m_pDB, Table_Name.c_str(), m_pDB->GetColumnCount(Table_Name.c_str(), SG_T("")));

	if( !DBTable.DropTable() )
	{
		_Error_Message(_TL("could not drop database table"));

		return( false );
	}

	if( bCommit )
	{
		return( Commit() );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Load(CSG_Table &Table, const CSG_String &Table_Name)
{
	bool		bResult	= false;
	int			iField;
	UWORD		nFields;
	wxDbColInf	*Fields;
	SG_Char		**Values;

	//-----------------------------------------------------
	if( !m_pDB )
	{
		_Error_Message(_TL("no database connection"));

		return( false );
	}

	if( (Fields = m_pDB->GetColumns(Table_Name.c_str(), &nFields)) == NULL || nFields <= 0 )
	{
		_Error_Message(_TL("could not retrieve column information"));

		return( false );
	}

	//-----------------------------------------------------
	wxDbTable	DBTable(m_pDB, Table_Name.c_str(), nFields);

	Values		= (SG_Char **)SG_Malloc(nFields * sizeof(SG_Char *));
	Values[0]	= (SG_Char  *)SG_Calloc(nFields,  sizeof(SG_Char) * 256);

	Table.Destroy();
	Table.Set_Name(Table_Name);

	for(iField=0; iField<nFields; iField++)
	{
		Values[iField]	= Values[0] + iField * 256 * sizeof(SG_Char);

		DBTable.SetColDefs(iField, Fields[iField].colName, Fields[iField].dbDataType, Values[iField], SQL_C_WXCHAR, 255, true, true);

		Table.Add_Field(Fields[iField].colName, _Get_Type_From_SQL(Fields[iField].dbDataType));
	}

	//-----------------------------------------------------
	DBTable.SetWhereClause  (SG_T(""));
	DBTable.SetOrderByClause(SG_T(""));
	DBTable.SetFromClause   (SG_T(""));

	if( !DBTable.Open() )
	{
		_Error_Message(_TL("could not open database table"));
	}
	else if( !DBTable.Query() )
	{
		_Error_Message(_TL("could not execute query"));
	}
	else
	{
		while( DBTable.GetNext() && SG_UI_Process_Set_Progress(Table.Get_Record_Count(), DBTable.Count()) )
		{
			CSG_Table_Record	*pRecord	= Table.Add_Record();

			for(iField=0; iField<nFields; iField++)
			{
				if( !Values[iField] || !Values[iField][0] )
				{
					pRecord->Set_NoData(iField);
				}
				else
				{
					pRecord->Set_Value(iField, Values[iField]);
				}
			}
		}

		if( !(bResult = Table.Get_Record_Count() > 0) )
		{
			_Error_Message(_TL("no records in database table"));
		}
	}

	//-----------------------------------------------------
	SG_Free(Values[0]);
	SG_Free(Values);

	return( bResult );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Save(const CSG_String &Table_Name, const CSG_Table &Table, bool bCommit)
{
	bool		bResult	= false;
	int			iField, iRecord;
	SG_Char		**Values;

	//-----------------------------------------------------
	if( !m_pDB )
	{
		_Error_Message(_TL("no database connection"));

		return( false );
	}

	if( Table.Get_Field_Count() <= 0 )
	{
		_Error_Message(_TL("no database connection"));

		return( false );
	}

	//-----------------------------------------------------
	wxDbTable	DbTable(m_pDB, Table_Name.c_str(), Table.Get_Field_Count(), wxT(""), !wxDB_QUERY_ONLY, wxT(""));

	Values		= (SG_Char **)SG_Calloc(Table.Get_Field_Count(), sizeof(SG_Char *));

	for(iField=0; iField<Table.Get_Field_Count(); iField++)
	{
		int		Type, SQL_C, Precision;

		switch( Table.Get_Field_Type(iField) )
		{
		case TABLE_FIELDTYPE_String: default:
		case TABLE_FIELDTYPE_Date:
			Type			= DB_DATA_TYPE_VARCHAR;	// strings
			SQL_C			= SQL_C_WXCHAR;
			Precision		= 255;
			Values[iField]	= (SG_Char  *)SG_Calloc(Precision + 1, sizeof(SG_Char));
			break;

		case TABLE_FIELDTYPE_Color:
		case TABLE_FIELDTYPE_Char:
		case TABLE_FIELDTYPE_Short:
		case TABLE_FIELDTYPE_Int:
		case TABLE_FIELDTYPE_Long:
			Type			= DB_DATA_TYPE_INTEGER;	// non-floating point numbers
			SQL_C			= SQL_C_LONG;
			Precision		= sizeof(long);
			Values[iField]	= (SG_Char  *)SG_Calloc(1, Precision);
			break;

		case TABLE_FIELDTYPE_Float:
		case TABLE_FIELDTYPE_Double:
			Type			= DB_DATA_TYPE_FLOAT;	// floating point numbers
			SQL_C			= SQL_C_DOUBLE;
			Precision		= sizeof(double);
			Values[iField]	= (SG_Char  *)SG_Calloc(1, Precision);
			break;
		}

		DbTable.SetColDefs(iField, Table.Get_Field_Name(iField), Type, Values[iField], SQL_C, Precision, true, true);
	}

	//-----------------------------------------------------
	if( !DbTable.CreateTable(false) )
	{
		_Error_Message(_TL("failed to create database table"));
	}
	else if( !DbTable.Open() )
	{
		_Error_Message(_TL("failed to open database table"));
	}
	else
	{
		for(iRecord=0, bResult=true; iRecord<Table.Get_Count() && SG_UI_Process_Set_Progress(iRecord, Table.Get_Count()) && bResult; iRecord++)
		{
			for(iField=0; iField<Table.Get_Field_Count(); iField++)
			{
				switch( Table.Get_Field_Type(iField) )
				{
				case TABLE_FIELDTYPE_String: default:
				case TABLE_FIELDTYPE_Date:
					wxStrcpy(Values[iField], Table[iRecord].asString(iField));
					break;

				case TABLE_FIELDTYPE_Color:
				case TABLE_FIELDTYPE_Char:
				case TABLE_FIELDTYPE_Short:
				case TABLE_FIELDTYPE_Int:
				case TABLE_FIELDTYPE_Long:
					*((long   *)(Values[iField]))	= Table[iRecord].asInt   (iField);
					break;

				case TABLE_FIELDTYPE_Float:
				case TABLE_FIELDTYPE_Double:
					*((double *)(Values[iField]))	= Table[iRecord].asDouble(iField);
					break;
				}
			}

			if( !DbTable.Insert() )
			{
				_Error_Message(_TL("insertion error"));

				bResult	= false;
			}
		}

		if( bResult && bCommit && !Commit() )
		{
			bResult	= false;
		}
	}

	//-----------------------------------------------------
	for(iField=0; iField<Table.Get_Field_Count(); iField++)
	{
		SG_Free(Values[iField]);
	}

	SG_Free(Values);

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_From_Query(const CSG_String &FieldNames, const CSG_String &Tables, const CSG_String &Where, const CSG_String &Order, CSG_Table &Table)
{
	if( m_pDB != NULL && Tables.Length() )
	{
		short		iField, nFields;
		wxDbColInf	*Fields;
		CSG_String	SQL;

		SQL.Printf(SG_T("SELECT %s FROM %s"), FieldNames.c_str(), Tables.c_str());

		if( Where.Length() )
		{
			SQL	+= SG_T(" WHERE ") + Where;
		}

		if( Order.Length() )
		{
			SQL	+= SG_T(" ORDER BY ") + Order;
		}

		SG_UI_Msg_Add_Execution(SQL, true);

		//-------------------------------------------------
		if( m_pDB->ExecSql(SQL.c_str(), &Fields, nFields) )
		{
			Table.Destroy();
			Table.Set_Name(SQL);

			for(iField=0; iField<nFields; iField++)
			{
				switch( Fields[iField].dbDataType )
				{
				default:
				case DB_DATA_TYPE_BLOB:
				case DB_DATA_TYPE_MEMO:
				case DB_DATA_TYPE_DATE:
				case DB_DATA_TYPE_VARCHAR:	Table.Add_Field(Fields[iField].colName, TABLE_FIELDTYPE_String);	break;
				case DB_DATA_TYPE_INTEGER:	Table.Add_Field(Fields[iField].colName, TABLE_FIELDTYPE_Int);		break;
				case DB_DATA_TYPE_FLOAT:	Table.Add_Field(Fields[iField].colName, TABLE_FIELDTYPE_Double);	break;
				}
			}

			delete[](Fields);

			//---------------------------------------------
			SG_Char	Value[256];
			SDWORD	cb;

			while( m_pDB->GetNext() && SG_UI_Process_Get_Okay(false) )
			{
				CSG_Table_Record	*pRecord	= Table.Add_Record();

				for(iField=0; iField<nFields; iField++)
				{
					if( m_pDB->GetData(iField + 1, SQL_C_WXCHAR, Value, 255, &cb) && cb != SQL_NULL_DATA )
					{
						pRecord->Set_Value(iField, Value);
					}
				}
			}

			return( Table.Get_Record_Count() > 0 );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
