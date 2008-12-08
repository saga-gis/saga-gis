
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Table_ODBC                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Get_Table.cpp                     //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
#include "Get_Table.h"

#include <wx/dbtable.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGet_Table::CGet_Table(void)
{
	Set_Name	(_TL("Import Table via ODBC"));

	Set_Author		(SG_T("(c) 2005 by O.Conrad"));

	Set_Description(
		_TL("Get table from ODBC source.\n")
	);

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "DB_SOURCE"	, _TL("ODBC Source"),
		_TL(""),

		CSG_String::Format(SG_T("%s|"),
			_TL("NONE")
		)
	);

	Parameters.Add_String(
		NULL	, "USERNAME"	, _TL("User Name"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_String(
		NULL	, "PASSWORD"	, _TL("Password"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "DIRPATH"		, _TL("Directory Path"),
		_TL(""),
		NULL, NULL, false, true
	);

	//-----------------------------------------------------
	CSG_Parameters	*pTables	= Add_Parameters("DB_TABLE", _TL("Select a Table"), _TL(""));

	pTables->Add_Choice(
		NULL	, "TABLES"		, _TL("Tables"),
		_TL(""),

		CSG_String::Format(SG_T("%s|"),
			_TL("NONE")
		)
	);

	//-----------------------------------------------------
	m_DSN	= (SG_Char *)SG_Malloc((1 + SQL_MAX_DSN_LENGTH) * sizeof(SG_Char));

	_Initialize();
}

//---------------------------------------------------------
CGet_Table::~CGet_Table(void)
{
	wxDbCloseConnections();

	wxDELETE(m_DBC_Inf);

	SG_Free(m_DSN);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGet_Table::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameters->Get_Identifier(), Parameters.Get_Identifier()) )
	{
		if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("DB_SOURCE")) )
		{
		}
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGet_Table::On_Execute(void)
{
	bool		bResult	= false;
	wxDb		*pDB;
	wxDbTable	*pDBTable;

	//-----------------------------------------------------
	if( (pDB = _Connect()) != NULL && (pDBTable = _Get_Table(pDB)) != NULL )
	{
		bResult	= _Get_Data(Parameters("TABLE")->asTable(), pDBTable);

		delete(pDBTable);
	}

	//-----------------------------------------------------
	if( pDB )
	{
		wxDbFreeConnection(pDB);
	}

	wxDbCloseConnections();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGet_Table::_Initialize(void)
{
	wxChar		DSN[1 + SQL_MAX_DSN_LENGTH], DSDesc[255];
	wxString	s;

	//-----------------------------------------------------
	DSN[0]		= '\0';
    m_DBC_Inf	= new wxDbConnectInf(NULL, DSN, Parameters("USERNAME")->asString(), Parameters("PASSWORD")->asString(), Parameters("DIRPATH")->asString());

	if( !m_DBC_Inf || !m_DBC_Inf->GetHenv() )
	{
		wxDELETE(m_DBC_Inf);
		m_DBC_Inf	= NULL;

		return( false );
	}

	//-----------------------------------------------------
	while( wxDbGetDataSource(m_DBC_Inf->GetHenv(), DSN, 1 + SQL_MAX_DSN_LENGTH, DSDesc, 255) )
	{
		s.Append(wxString::Format(SG_T("%s|"), DSN).c_str());
	}

	((CSG_Parameter_Choice *)Parameters("DB_SOURCE")->Get_Data())->Set_Items(s.c_str());

	return( true );
}

//---------------------------------------------------------
wxDb * CGet_Table::_Connect(void)
{
	wxString			DSN;
	CSG_Parameter_Choice	*pDSNs	= (CSG_Parameter_Choice *)Parameters("DB_SOURCE")->Get_Data();

	//-----------------------------------------------------
	if( m_DBC_Inf && pDSNs && pDSNs->Get_Count() > 0 )
	{
		m_DBC_Inf->SetDsn		(pDSNs->Get_Item(pDSNs->asInt()));
		m_DBC_Inf->SetUserID	(Parameters("USERNAME")->asString());
		m_DBC_Inf->SetPassword	(Parameters("PASSWORD")->asString());
		m_DBC_Inf->SetDefaultDir(Parameters("DIRPATH" )->asString());

		return( wxDbGetConnection(m_DBC_Inf) );
	}

	return( NULL );
}

//---------------------------------------------------------
wxDbTable * CGet_Table::_Get_Table(wxDb *pDB)
{
	int			i;
	wxString	s;
	wxDbInf		*pInf;
	CSG_Parameter_Choice	*pTables;

	//-----------------------------------------------------
	if( pDB && (pInf = pDB->GetCatalog(NULL)) != NULL && pInf->numTables > 0 )
	{
		for(i=0; i<pInf->numTables; i++)
		{
			s.Append(wxString::Format(SG_T("%s\n"), pInf->pTableInf[i].tableName).c_str());
		}

		pTables	= (CSG_Parameter_Choice *)Get_Parameters("DB_TABLE")->Get_Parameter("TABLES")->Get_Data();
		pTables->Set_Items(s.c_str());

		if( Dlg_Parameters("DB_TABLE") )
		{
			return(
				new wxDbTable(
					pDB,
					pTables->Get_Item(pTables->asInt()),
					pInf->pTableInf[pTables->asInt()].numCols,
					_TL(""), !wxDB_QUERY_ONLY,
					_TL("")
				)
			);
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CGet_Table::_Get_Data(CSG_Table *pTable, wxDbTable *pDBTable)
{
	SG_Char				**Values;
	int					iField, nFields;
	UWORD				numCols;
	CSG_Table_Record	*pRecord;
	wxDbColInf			*ColDefs;

	//-----------------------------------------------------
	if( (nFields = pDBTable->GetNumberOfColumns()) > 0 )
	{
		Values		= (SG_Char **)SG_Malloc(nFields * sizeof(SG_Char *));
		Values[0]	= (SG_Char  *)SG_Malloc(nFields * 256 * sizeof(SG_Char));
		pTable->Destroy();

		ColDefs		= pDBTable->GetDb()->GetColumns(pDBTable->GetTableName(), &numCols);

		for(iField=0; iField<nFields; iField++)
		{
			switch( ColDefs[iField].dbDataType )
			{
			default:
			case DB_DATA_TYPE_VARCHAR:
			case DB_DATA_TYPE_DATE:
				pTable->Add_Field(ColDefs[iField].colName, TABLE_FIELDTYPE_String);
				break;

			case DB_DATA_TYPE_INTEGER:
				pTable->Add_Field(ColDefs[iField].colName, TABLE_FIELDTYPE_Int);
				break;

			case DB_DATA_TYPE_FLOAT:
				pTable->Add_Field(ColDefs[iField].colName, TABLE_FIELDTYPE_Double);
				break;
			}

			Values[iField]	= Values[0] + iField * 256 * sizeof(SG_Char);
			pDBTable->SetColDefs(iField, ColDefs[iField].colName, DB_DATA_TYPE_VARCHAR, Values[iField], SQL_C_CHAR, 255, TRUE, TRUE);
		}

		//-----------------------------------------------------
		pDBTable->Open();

		pDBTable->SetWhereClause	(_TL(""));
		pDBTable->SetOrderByClause	(_TL(""));
		pDBTable->SetFromClause		(_TL(""));

		if( pDBTable->Query() )
		{
			while( pDBTable->GetNext() )
			{
				pRecord	= pTable->Add_Record();

				for(iField=0; iField<nFields; iField++)
				{
					pRecord->Set_Value(iField, Values[iField]);
				}
			}
		}

		//-----------------------------------------------------
		SG_Free(Values[0]);
		SG_Free(Values);

		return( pTable->Get_Record_Count() > 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** /
wxDbConnectInf  *DbConnectInf = NULL;

wxDb        *db    = NULL;       // The database connection
wxDbTable   *table = NULL;       // The data table to access

wxChar       FirstName[50+1];    // buffer for data from column "FIRST_NAME"
wxChar       LastName[50+1];     // buffer for data from column "LAST_NAME"

bool         errorOccured = FALSE;

const wxChar tableName[]          = "CONTACTS";
const UWORD  numTableColumns      = 2;           // Number of bound columns

FirstName[0] = 0;
LastName[0]  = 0;

DbConnectInf = new wxDbConnectInf(NULL,"MyDSN","MyUserName", "MyPassword");

if (!DbConnectInf || !DbConnectInf->GetHenv())
{
  wxMessageBox("Unable to allocate an ODBC environment handle",
            "DB CONNECTION ERROR", wxOK | wxICON_EXCLAMATION);
  return;
} 

// Get a database connection from the cached connections
db = wxDbGetConnection(DbConnectInf);

// Create the table connection
table = new wxDbTable(db, tableName, numTableColumns, "", 
                      !wxDB_QUERY_ONLY, "");

//
// Bind the columns that you wish to retrieve. Note that there must be
// 'numTableColumns' calls to SetColDefs(), to match the wxDbTable definition
//
// Not all columns need to be bound, only columns whose values are to be 
// returned back to the client.
//
table->SetColDefs(0, "FIRST_NAME", DB_DATA_TYPE_VARCHAR, FirstName,
                  SQL_C_CHAR, sizeof(name), TRUE, TRUE);
table->SetColDefs(1, "LAST_NAME", DB_DATA_TYPE_VARCHAR, LastName,
                  SQL_C_CHAR, sizeof(LastName), TRUE, TRUE);

// Open the table for access
table->Open();

// Set the WHERE clause to limit the result set to only
// return all rows that have a value of 'GEORGE' in the
// FIRST_NAME column of the table.
table->SetWhereClause("FIRST_NAME = 'GEORGE'");

// Result set will be sorted in ascending alphabetical 
// order on the data in the 'LAST_NAME' column of each row
table->SetOrderByClause("LAST_NAME");

// No other tables (joins) are used for this query
table->SetFromClause("");

// Instruct the datasource to perform a query based on the 
// criteria specified above in the where/orderBy/from clauses.
if (!table->Query())
{
    wxMessageBox("Error on Query()","ERROR!",
                  wxOK | wxICON_EXCLAMATION);
    errorOccured = TRUE;
}

wxString msg;

// Start and continue reading every record in the table
// displaying info about each record read.
while (table->GetNext())
{
    msg.Printf("Row #%lu -- First Name : %s  Last Name is %s",
               table->GetRowNum(), FirstName, LastName);
    wxMessageBox(msg, "Data", wxOK | wxICON_INFORMATION, NULL);
}

// If the wxDbTable instance was successfully created
// then delete it as I am done with it now.
if (table)
{
    delete table;
    table = NULL;
}

// If we have a valid wxDb instance, then free the connection
// (meaning release it back in to the cache of datasource
// connections) for the next time a call to wxDbGetConnection()
// is made.
if (db)
{
    wxDbFreeConnection(db);
    db = NULL;
}

// The program is now ending, so we need to close
// any cached connections that are still being 
// maintained.
wxDbCloseConnections();

// Release the environment handle that was created
// for use with the ODBC datasource connections
delete DbConnectInf;
/**/
