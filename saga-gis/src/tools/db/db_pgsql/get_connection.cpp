
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       db_pgsql                        //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Lists all PostgreSQL sources."
	));

	Parameters.Add_Table("",
		"CONNECTIONS"	, _TL("Connections"),
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
		pRecord->Set_Value(1, pConnection->Get_Host      ());
		pRecord->Set_Value(2, pConnection->Get_Port      ());
		pRecord->Set_Value(3, pConnection->Get_DBName    ());
		pRecord->Set_Value(4, pConnection->Get_Version   ());
		pRecord->Set_Value(5, pConnection->Get_PostGIS   ());
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

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Connect to PostgreSQL data source."
	));

	Parameters.Add_String("",
		"PG_HOST"	, _TL("Host"),
		_TL(""),
		"localhost"
	);

	Parameters.Add_Int("",
		"PG_PORT"	, _TL("Port"),
		_TL(""),
		5432, 0, true
	);

	Parameters.Add_String("",
		"PG_USER"	, _TL("User"),
		_TL(""),
		"postgres"
	);

	Parameters.Add_String("",
		"PG_PWD"	, _TL("Password"),
		_TL(""),
		"postgres", false, true
	);

	Parameters.Add_String("",
		"PG_NAME"	, _TL("Database"),
		_TL(""),
		""
	);

	Parameters.Add_Choice("",
		"PG_LIST"	, _TL("Database"),
		_TL(""),
		""
	)->Set_UseInCMD(false);

	Parameters("PG_LIST")->Set_Enabled(false);
}

//---------------------------------------------------------
int CGet_Connection::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( SG_UI_Get_Window_Main() )
	{
		if( pParameter->Cmp_Identifier("PG_HOST")
		||  pParameter->Cmp_Identifier("PG_PORT")
		||  pParameter->Cmp_Identifier("PG_USER")
		||  pParameter->Cmp_Identifier("PG_PWD" ) )
		{
			CSG_Table	DBs;

			CSG_PG_Connection	Connection(
				pParameters->Get_Parameter("PG_HOST")->asString(),
				pParameters->Get_Parameter("PG_PORT")->asInt   (),
				"",
				pParameters->Get_Parameter("PG_USER")->asString(),
				pParameters->Get_Parameter("PG_PWD" )->asString()
			);

			if( Connection.is_Connected() && Connection.Execute("SELECT datname FROM pg_database", &DBs) )
			{
				CSG_String	List;	DBs.Set_Index(0, TABLE_INDEX_Ascending);

				for(int i=0; i<DBs.Get_Count(); i++)
				{
					List	+= DBs[i].asString(0) + CSG_String("|");
				}

				pParameters->Get_Parameter("PG_LIST")->asChoice()->Set_Items(List);
				pParameters->Get_Parameter("PG_LIST")->Set_Value(pParameters->Get_Parameter("PG_NAME")->asString());
				pParameters->Set_Enabled  ("PG_LIST",  true);
				pParameters->Set_Enabled  ("PG_NAME", false);
				pParameters->Get_Parameter("PG_NAME")->Set_Value(pParameters->Get_Parameter("PG_LIST")->asString());
			}
			else
			{
				pParameters->Set_Enabled  ("PG_LIST", false);
				pParameters->Set_Enabled  ("PG_NAME",  true);
			}
		}

		if( pParameter->Cmp_Identifier("PG_LIST") )
		{
			pParameters->Get_Parameter("PG_NAME")->Set_Value(pParameter->asString());
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CGet_Connection::On_Execute(void)
{
	CSG_String	Connection	= CSG_String::Format("%s [%s:%d]",
		Parameters("PG_NAME")->asString(),
		Parameters("PG_HOST")->asString(),
		Parameters("PG_PORT")->asInt   ()
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
		Parameters("PG_PORT")->asInt   ()
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

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Disconnect PostgreSQL data source."
	));

	Parameters.Add_Choice("",
		"TRANSACT"	, _TL("Transactions"),
		_TL(""),
		CSG_String::Format("%s|%s",
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

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Disconnects all PostgreSQL connections."
	));

	Parameters.Add_Choice("",
		"TRANSACT"	, _TL("Transactions"),
		_TL(""),
		CSG_String::Format("%s|%s",
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
CTransaction_Start::CTransaction_Start(void)
{
	Set_Name		(_TL("Begin Transaction"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Begins a transaction, which will be finished later with a commit or rollback. "
		"Tries to add a save point, if already in transaction mode. "
	));

	Parameters.Add_String("",
		"SAVEPOINT"	, _TL("Save Point"),
		_TL(""),
		"SAVEPOINT_01"
	);
}

//---------------------------------------------------------
void CTransaction_Start::On_Connection_Changed(CSG_Parameters *pParameters)
{
	pParameters->Get_Parameter("SAVEPOINT")->Set_Enabled(Get_Connection()->is_Transaction());
}

//---------------------------------------------------------
bool CTransaction_Start::On_Execute(void)
{
	if( !Get_Connection()->is_Transaction() )
	{
		if( Get_Connection()->Begin() )
		{
			Message_Add(Get_Connection()->Get_Connection() + ": " + _TL("transaction started"));

			return( true );
		}

		Message_Add(Get_Connection()->Get_Connection() + ": " + _TL("could not start transaction"));
	}
	else
	{
		if( Get_Connection()->Begin(Parameters("SAVEPOINT")->asString()) )
		{
			Message_Add(Get_Connection()->Get_Connection() + ": " + _TL("save point added"));

			return( true );
		}

		Message_Add(Get_Connection()->Get_Connection() + ": " + _TL("could not add save point"));
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTransaction_Stop::CTransaction_Stop(void)
{
	Set_Name		(_TL("Commit/Rollback Transaction"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Execute a commit or rollback on open transactions with PostgreSQL source."
	));

	Parameters.Add_Choice("",
		"TRANSACT"	, _TL("Transactions"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("rollback"),
			_TL("commit")
		), 1
	);

	Parameters.Add_String("",
		"SAVEPOINT"	, _TL("Save Point"),
		_TL(""),
		""
	);
}

//---------------------------------------------------------
bool CTransaction_Stop::On_Execute(void)
{
	if( !Get_Connection()->is_Transaction() )
	{
		Message_Add(Get_Connection()->Get_Connection() + ": " + _TL("not in transaction"));

		return( false );
	}

	if( Parameters("TRANSACT")->asInt() == 1 )
	{
		if( Get_Connection()->Commit(Parameters("SAVEPOINT")->asString()) )
		{
			Message_Add(Get_Connection()->Get_Connection() + ": " + _TL("open transactions committed"));

			SG_UI_ODBC_Update(Get_Connection()->Get_Connection());

			return( true );
		}
	}
	else
	{
		if( Get_Connection()->Rollback(Parameters("SAVEPOINT")->asString()) )
		{
			Message_Add(Get_Connection()->Get_Connection() + ": " + _TL("open transactions rolled back"));

			SG_UI_ODBC_Update(Get_Connection()->Get_Connection());

			return( true );
		}
	}

	Message_Add(Get_Connection()->Get_Connection() + ": " + _TL("could not commit/rollback transactions."));

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

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Execute SQL commands on a connected PostgreSQL source. "
		"Separate different commands with a semicolon (\';\'). "
	));

	Parameters.Add_String("",
		"SQL"	, _TL("SQL Statement"),
		_TL(""),
		"CREATE TABLE myTable1 (Col1 VARCHAR(255) PRIMARY KEY, Col2 INTEGER);\n"
		"INSERT INTO myTable1 (Col1, Col2) VALUES(\'First Value\', 1);\n"
		"DROP TABLE myTable1;\n",
		true
	);

	Parameters.Add_Table_List("",
		"TABLES", _TL("Tables"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"OUTPUT", _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("none"),
			_TL("message window"),
			_TL("tables")
		), 2
	);

	Parameters.Add_Bool("",
		"STOP"	, _TL("Stop on Error"),
		_TL(""),
		false
	);
}

//---------------------------------------------------------
bool CExecute_SQL::On_Execute(void)
{
	CSG_String	SQL	= Parameters("SQL")->asString();

	if( SQL.is_Empty() )
	{
		Error_Set(_TL("empty string"));

		return( false );
	}

	//-----------------------------------------------------
	int		nErrors = 0;

	int		Output	= Parameters("OUTPUT")->asInt();
	bool	bStop	= Parameters("STOP"  )->asBool();

	CSG_Parameter_Table_List	*pTables	= Parameters("TABLES")->asTableList();

	//-----------------------------------------------------
	CSG_String_Tokenizer	Commands(SQL, ";");

	while( Commands.Has_More_Tokens() && (!nErrors || !bStop) )
	{
		CSG_String	Command	= Commands.Get_Next_Token(); Command.Trim(true); Command.Trim(false);

		if( !Command.is_Empty() )
		{
			CSG_Table	*pTable	= Output ? SG_Create_Table() : NULL;

			if( !Get_Connection()->Execute(Command, pTable) )
			{
				nErrors++;

				Error_Set(CSG_String::Format("%s: %s", _TL("Error"), Command.c_str()));
			}
			else
			{
				Message_Fmt("\n%s: %s", _TL("Success"), Command.c_str());

				if( pTable && pTable->Get_Count() > 0 )
				{
					if( Output == 2 )	// tables
					{
						pTables->Add_Item(pTable);

						pTable	= NULL;
					}
					else	// message window
					{
						int			iField, iRecord;
						CSG_String	s	= "\n";

						for(iField=0; iField<pTable->Get_Field_Count(); iField++)
						{
							s	+= iField > 0 ? "\t" : "\n";
							s	+= pTable->Get_Field_Name(iField);
						}

						s	= "\n___";

						for(iRecord=0; iRecord<pTable->Get_Count(); iRecord++)
						{
							CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

							for(iField=0; iField<pTable->Get_Field_Count(); iField++)
							{
								s	+= iField > 0 ? "\t" : "\n";
								s	+= pRecord->asString(iField);
							}

							Message_Add(s, false);	s.Clear();
						}

						Message_Add("\n", false);
					}
				}
			}

			//---------------------------------------------
			if( pTable )
			{
				delete(pTable);
			}
		}
	}

	//-----------------------------------------------------
	return( nErrors == 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDatabase_Create::CDatabase_Create(void)
{
	Set_Name		(_TL("Create Database"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Creates a new PostgreSQL Database."
	));

	Parameters.Add_String("",
		"PG_HOST"	, _TL("Host"),
		_TL(""),
		"localhost"
	);

	Parameters.Add_Int("",
		"PG_PORT"	, _TL("Port"),
		_TL(""),
		5432, 0, true
	);

	Parameters.Add_String("",
		"PG_NAME"	, _TL("Database"),
		_TL(""),
		"geo_test"
	);

	Parameters.Add_String("",
		"PG_USER"	, _TL("User"),
		_TL(""),
		"postgres"
	);

	Parameters.Add_String("",
		"PG_PWD"	, _TL("Password"),
		_TL(""),
		"postgres", false, true
	);
}

//---------------------------------------------------------
bool CDatabase_Create::On_Execute(void)
{
	const SG_Char	*Host, *Name, *User, *Password;

	Host		= Parameters("PG_HOST")->asString();
	int	Port	= Parameters("PG_PORT")->asInt();
	Name		= Parameters("PG_NAME")->asString(),
	User		= Parameters("PG_USER")->asString();
	Password	= Parameters("PG_PWD" )->asString();

	if( SG_PG_Get_Connection_Manager().Get_Connection(CSG_String::Format("%s [%s:%d]", Name, Host, Port)) )
	{
		Message_Fmt("\n%s [%s:%d]: %s", Name, Host, Port, _TL("PostgreSQL database is already connected"));

		return( false );
	}

	CSG_PG_Connection	Connection;

	if( Connection.Create(Host, Port, "", User, Password) && Connection.Execute(CSG_String::Format("CREATE DATABASE %s", Name)) )
	{
		CSG_PG_Connection	*pConnection	= SG_PG_Get_Connection_Manager().Add_Connection(Name, User, Password, Host, Port);

		if( pConnection )
		{
			if( pConnection->Execute("CREATE EXTENSION postgis") )
			{
				Message_Fmt("\n%s [%s:%d]: %s", Name, Host, Port, _TL("PostGIS extension added"));
			}

			pConnection->GUI_Update();

			return( true );
		}
	}

	Message_Fmt("\n%s [%s:%d]: %s", Name, Host, Port, _TL("could not create new PostgreSQL database"));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDatabase_Destroy::CDatabase_Destroy(void)
{
	Set_Name		(_TL("Drop Database"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Deletes a PostgreSQL Database."
	));

	Parameters.Add_String("",
		"PG_HOST"	, _TL("Host"),
		_TL(""),
		"localhost"
	);

	Parameters.Add_Int("",
		"PG_PORT"	, _TL("Port"),
		_TL(""),
		5432, 0, true
	);

	Parameters.Add_String("",
		"PG_NAME"	, _TL("Database"),
		_TL(""),
		"geo_test"
	);

	Parameters.Add_String("",
		"PG_USER"	, _TL("User"),
		_TL(""),
		"postgres"
	);

	Parameters.Add_String("",
		"PG_PWD"	, _TL("Password"),
		_TL(""),
		"postgres", false, true
	);
}

//---------------------------------------------------------
bool CDatabase_Destroy::On_Execute(void)
{
	const SG_Char	*Host, *Name, *User, *Password;

	Host		= Parameters("PG_HOST")->asString();
	int	Port	= Parameters("PG_PORT")->asInt   ();
	Name		= Parameters("PG_NAME")->asString();
	User		= Parameters("PG_USER")->asString();
	Password	= Parameters("PG_PWD" )->asString();

	if( SG_PG_Get_Connection_Manager().Get_Connection(CSG_String::Format("%s [%s:%d]", Name, Host, Port)) )
	{
		if( !SG_PG_Get_Connection_Manager().Del_Connection(CSG_String::Format("%s [%s:%d]", Name, Host, Port), false) )
		{
			Message_Fmt("\n%s [%s:%d]: %s", Name, Host, Port, _TL("could not disconnect and drop PostgreSQL database"));

			return( false );
		}
	}

	CSG_PG_Connection	Connection;

	if( Connection.Create(Host, Port, "", User, Password) && Connection.Execute(CSG_String::Format("DROP DATABASE IF EXISTS %s", Name)) )
	{
		SG_UI_ODBC_Update("");

		return( true );
	}

	Message_Fmt("\n%s [%s:%d]: %s", Name, Host, Port, _TL("could not drop PostgreSQL database"));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
