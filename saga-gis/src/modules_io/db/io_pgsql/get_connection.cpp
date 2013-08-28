/**********************************************************
 * Version $Id: get_connection.cpp 1509 2012-11-01 16:39:43Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       io_pgsql                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Get_Connection.cpp                  //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
//                University of Hamburg                  //
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
#include "get_connection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGet_Connections::CGet_Connections(void)
{
	Set_Name		(_TL("List PostgreSQL Connections"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Lists all PostgreSQL sources."
	));

	Parameters.Add_Table(
		NULL	, "CONNECTIONS"		, _TL("Connections"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
bool CGet_Connections::On_Execute(void)
{
	CSG_Table	*pConnections	= Parameters("CONNECTIONS")->asTable();

	pConnections->Destroy();
	pConnections->Set_Name(_TL("PostgreSQL Connections"));

	pConnections->Add_Field(_TL("Name"    ), SG_DATATYPE_String);
	pConnections->Add_Field(_TL("Host"    ), SG_DATATYPE_String);
	pConnections->Add_Field(_TL("Port"    ), SG_DATATYPE_Int);
	pConnections->Add_Field(_TL("Database"), SG_DATATYPE_String);
	pConnections->Add_Field(_TL("Version" ), SG_DATATYPE_String);
	pConnections->Add_Field(_TL("PostGIS" ), SG_DATATYPE_String);

	for(int i=0; i<SG_PG_Get_Connection_Manager().Get_Count(); i++)
	{
		CSG_PG_Connection	*pConnection = SG_PG_Get_Connection_Manager().Get_Connection(i);
		CSG_Table_Record	*pRecord     = pConnections->Add_Record();

		pRecord->Set_Value(0, pConnection->Get_Connection());
		pRecord->Set_Value(1, pConnection->Get_Host());
		pRecord->Set_Value(2, pConnection->Get_Port());
		pRecord->Set_Value(3, pConnection->Get_DBName());
		pRecord->Set_Value(4, pConnection->Get_Version());
		pRecord->Set_Value(5, pConnection->Get_PostGIS());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGet_Connection::CGet_Connection(void)
{
	Set_Name		(_TL("Connect to PostgreSQL"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Connect to PostgreSQL data source."
	));

	Parameters.Add_String(
		NULL	, "PG_HOST"		, _TL("Host"),
		_TL("Password"),
		"localhost"
	);

	Parameters.Add_Value(
		NULL	, "PG_PORT"		, _TL("Port"),
		_TL(""),
		PARAMETER_TYPE_Int, 5432, 0, true
	);

	Parameters.Add_String(
		NULL	, "PG_NAME"		, _TL("Database"),
		_TL("Database Name"),
		"geo_test"
	);

	Parameters.Add_String(
		NULL	, "PG_USER"		, _TL("User"),
		_TL("User Name"),
		"postgres"
	);

	Parameters.Add_String(
		NULL	, "PG_PWD"		, _TL("Password"),
		_TL("Password"),
		"postgres", false, true
	);
}

//---------------------------------------------------------
bool CGet_Connection::On_Execute(void)
{
	CSG_String	Connection	= CSG_String::Format(SG_T("%s [%s:%d]"),
		Parameters("PG_NAME")->asString(),
		Parameters("PG_HOST")->asString(),
		Parameters("PG_PORT")->asInt()
	);

	if( SG_PG_Get_Connection_Manager().Get_Connection(Connection) )
	{
		Message_Add(Connection + ": " + _TL("PostgreSQL source is already connected"));

		return( false );
	}

	CSG_PG_Connection	*pConnection	= SG_PG_Get_Connection_Manager().Add_Connection(
		Parameters("PG_NAME")->asString(),
		Parameters("PG_USER")->asString(),
		Parameters("PG_PWD" )->asString(),
		Parameters("PG_HOST")->asString(),
		Parameters("PG_PORT")->asInt()
	);

	if( pConnection )
	{
		Message_Add(Connection + ": " + _TL("PostgreSQL source connected"));

		pConnection->GUI_Update();

		return( true );
	}

	Message_Add(Connection + ": " + _TL("could not connect to PostgreSQL source"));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDel_Connection::CDel_Connection(void)
{
	Set_Name		(_TL("Disconnect from PostgreSQL"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Disconnect PostgreSQL data source."
	));

	Parameters.Add_Choice(
		NULL	, "TRANSACT"	, _TL("Transactions"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("rollback"),
			_TL("commit")
		), 1
	);
}

//---------------------------------------------------------
bool CDel_Connection::On_Execute(void)
{
	CSG_String	Connection	= Get_Connection()->Get_Connection();

	if( SG_PG_Get_Connection_Manager().Del_Connection(Get_Connection(), Parameters("TRANSACT")->asInt() == 1) )
	{
		Message_Add(Connection + ": " + _TL("PostgreSQL source disconnected"));

		SG_UI_ODBC_Update(Connection);

		return( true );
	}

	Message_Add(Connection + ": " + _TL("could not disconnect PostgreSQL source"));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDel_Connections::CDel_Connections(void)
{
	Set_Name		(_TL("Disconnect All"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Disconnects all PostgreSQL connections."
	));

	Parameters.Add_Choice(
		NULL	, "TRANSACT"	, _TL("Transactions"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("rollback"),
			_TL("commit")
		), 1
	);
}

//---------------------------------------------------------
bool CDel_Connections::On_Before_Execution(void)
{
	if( SG_PG_Get_Connection_Manager().Get_Count() > 0 )
	{
		return( true );
	}

	Message_Dlg(
		_TL("No PostgreSQL connection available!"),
		_TL("PostgreSQL Database Connection Error")
	);

	return( false );
}

//---------------------------------------------------------
bool CDel_Connections::On_Execute(void)
{
	bool	bCommit	= Parameters("TRANSACT")->asInt() == 1;

	CSG_PG_Connections	&Manager	= SG_PG_Get_Connection_Manager();

	for(int i=Manager.Get_Count()-1; i>=0; i--)
	{
		Manager.Del_Connection(i, bCommit);
	}

	SG_UI_ODBC_Update("");

	return( Manager.Get_Count() == 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTransaction::CTransaction(void)
{
	Set_Name		(_TL("Commit/Rollback Transaction"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Execute a commit or rollback on open transactions with PostgreSQL source."
	));

	Parameters.Add_Choice(
		NULL	, "DATABASE"	, _TL("Database"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "TRANSACT"	, _TL("Transactions"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("rollback"),
			_TL("commit")
		), 1
	);
}

//---------------------------------------------------------
bool CTransaction::On_Before_Execution(void)
{
	CSG_String	Connections;

	if( SG_PG_Get_Connection_Manager().Get_Connections(Connections) > 0 )
	{
		Parameters("DATABASE")->asChoice()->Set_Items(Connections);

		return( true );
	}

	Message_Dlg(
		_TL("No PostgreSQL connection available!"),
		_TL("PostgreSQL Database Connection Error")
	);

	return( false );
}

//---------------------------------------------------------
bool CTransaction::On_Execute(void)
{
	CSG_String	Connection	= Parameters("DATABASE")->asString();

	CSG_PG_Connection	*pConnection	= SG_PG_Get_Connection_Manager().Get_Connection(Connection);

	if( !pConnection )
	{
		return( false );
	}

	if( Parameters("TRANSACT")->asInt() == 1 )
	{
		if( pConnection->Commit() )
		{
			Message_Add(CSG_String::Format(SG_T("%s: %s"), Connection.c_str(), _TL("open transactions committed")));

			SG_UI_ODBC_Update(Connection);

			return( true );
		}
	}
	else
	{
		if( pConnection->Rollback() )
		{
			Message_Add(CSG_String::Format(SG_T("%s: %s"), Connection.c_str(), _TL("open transactions rolled back")));

			SG_UI_ODBC_Update(Connection);

			return( true );
		}
	}

	Message_Add(CSG_String::Format(SG_T("%s: %s"), Connection.c_str(), _TL("could not commit/rollback transactions.")));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExecute_SQL::CExecute_SQL(void)
{
	Set_Name		(_TL("Execute SQL"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Execute SQL commands on a connected PostgreSQL source. "
		"Separate different commands with a semicolon (\';\'). "
	));

	Parameters.Add_String(
		NULL	, "SQL"			, _TL("SQL Statment"),
		_TL(""),
		SG_T("CREATE TABLE myTable1 (Col1 VARCHAR(255) PRIMARY KEY, Col2 INTEGER);\n")
		SG_T("INSERT INTO myTable1 (Col1, Col2) VALUES(\'First Value\', 1);\n")
		SG_T("DROP TABLE myTable1;\n"),
		true
	);

	Parameters.Add_Value(
		NULL	, "COMMIT"		, _TL("Commit"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "STOP"		, _TL("Stop on Error"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}

//---------------------------------------------------------
bool CExecute_SQL::On_Execute(void)
{
	//-----------------------------------------------------
	bool		bCommit	= Parameters("COMMIT")	->asBool  ();
	bool		bStop	= Parameters("STOP")	->asBool  ();
	CSG_String	SQL		= Parameters("SQL")		->asString();

	//-----------------------------------------------------
	if( SQL.Find(SG_T(';')) < 0 )
	{
		return( Get_Connection()->Execute(SQL) );
	}

	//-----------------------------------------------------
	int		nSuccess = 0, nErrors = 0;

	SQL	+= SG_T(';');

	do
	{
		CSG_String	s	= SQL.BeforeFirst(SG_T(';'));

		s.Trim();

		if( s.Length() > 0 )
		{
			Message_Add(s);

			if( Get_Connection()->Execute(s) )
			{
				nSuccess++;

				Message_Add(CSG_String::Format(SG_T("...%s!"), _TL("okay")), false);
			}
			else
			{
				nErrors++;

				Message_Add(CSG_String::Format(SG_T("...%s!"), _TL("failed")));

				if( bStop )
				{
					return( false );
				}
			}
		}

		SQL	= SQL.AfterFirst(SG_T(';'));
	}
	while( SQL.Length() > 0 );

	return( nErrors == 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
