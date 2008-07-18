
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
	wxChar		DSName[1 + SQL_MAX_DSN_LENGTH], DSDesc[255];

	//-----------------------------------------------------
	m_Servers.Clear();

	DSName[0]	= '\0';
    m_pDBCInf	= new wxDbConnectInf(NULL, DSName, SG_T(""), SG_T(""), SG_T(""));

	//-----------------------------------------------------
	if( m_pDBCInf && m_pDBCInf->GetHenv() )
	{
		while( wxDbGetDataSource(m_pDBCInf->GetHenv(), DSName, 1 + SQL_MAX_DSN_LENGTH, DSDesc, 255) )
		{
			m_Servers	+= CSG_String::Format(SG_T("%s|"), DSName);
		}

		return( true );
	}

	//-----------------------------------------------------
	Destroy();

	return( false );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Destroy(void)
{
	Disconnect();

	if( m_pDBCInf )
	{
		wxDELETE(m_pDBCInf);

		m_pDBCInf	= NULL;
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

	//	m_pDB->Close();
	//	delete(m_pDB);

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_ODBC_Connection::Get_Tables(void)
{
	CSG_String	Tables;
	wxDbInf		*pInf;

	//-----------------------------------------------------
	if( m_pDB != NULL && (pInf = m_pDB->GetCatalog(SG_T(""))) != NULL && pInf->numTables > 0 )
	{
		for(int i=0; i<pInf->numTables; i++)
		{
			Tables	+= CSG_String::Format(SG_T("%s|"), pInf->pTableInf[i].tableName);
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
bool CSG_ODBC_Connection::Get_Table(int iTable, const CSG_String &Table_Name, CSG_Table &Table)
{
	if( m_pDB != NULL && iTable >= 0 && iTable < m_pDB->GetCatalog(NULL)->numTables )
	{
		wxDbTable	DBTable(m_pDB, Table_Name.c_str(), m_pDB->GetCatalog(NULL)->pTableInf[iTable].numCols);

		if( DBTable.GetNumberOfColumns() > 0 )
		{
			SG_Char		**Values;
			int			iField;
			UWORD		nFields;
			wxDbColInf	*Fields;

			//---------------------------------------------
			Table.Destroy();
			Table.Set_Name(Table_Name);

			Fields		= DBTable.GetDb()->GetColumns(DBTable.GetTableName(), &nFields);

			Values		= (SG_Char **)SG_Malloc(nFields * sizeof(SG_Char *));
			Values[0]	= (SG_Char  *)SG_Malloc(nFields * 256 * sizeof(SG_Char));

			for(iField=0; iField<nFields; iField++)
			{
				Values[iField]	= Values[0] + iField * 256 * sizeof(SG_Char);

				DBTable.SetColDefs(iField, Fields[iField].colName, DB_DATA_TYPE_VARCHAR, Values[iField], SQL_C_WXCHAR, 255, true, true);

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

			//---------------------------------------------
			DBTable.Open();

			DBTable.SetWhereClause  (_TL(""));
			DBTable.SetOrderByClause(_TL(""));
			DBTable.SetFromClause   (_TL(""));

			if( DBTable.Query() )
			{
				while( DBTable.GetNext() && SG_UI_Process_Set_Progress(Table.Get_Record_Count(), DBTable.Count()) )
				{
					CSG_Table_Record	*pRecord	= Table.Add_Record();

					for(iField=0; iField<nFields; iField++)
					{
						pRecord->Set_Value(iField, Values[iField]);
					}
				}
			}

			//---------------------------------------------
			SG_Free(Values[0]);
			SG_Free(Values);

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
bool CSG_ODBC_Connection::Get_Query(const CSG_String &FieldNames, const CSG_String &Tables, const CSG_String &Where, const CSG_String &Order, CSG_Table &Table)
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
