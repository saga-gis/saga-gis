
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        db_odbc                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Get_Connection.cpp                  //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
CGet_Servers::CGet_Servers(void)
{
	Set_Name		(_TL("List Data Sources"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Lists all ODBC sources."
	));

	Parameters.Add_Table(
		"", "SOURCES"	, _TL("Data Sources"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool(
		"", "CONNECTED"	, _TL("Only List Connected Sources"),
		_TL(""),
		false
	);
}

//---------------------------------------------------------
bool CGet_Servers::On_Execute(void)
{
	bool     bConnected = Parameters("CONNECTED")->asBool ();
	CSG_Table *pSources = Parameters("SOURCES"  )->asTable();

	pSources->Destroy();
	pSources->Set_Name(_TL("ODBC Data Sources"));

	pSources->Add_Field(_TL("Data Source Name"), SG_DATATYPE_String);
	pSources->Add_Field(_TL("Connected"       ), SG_DATATYPE_Int   );

	CSG_Strings Sources;

	if( SG_ODBC_Get_Connection_Manager().Get_Servers(Sources) > 0 )
	{
		for(int i=0; i<Sources.Get_Count(); i++)
		{
			if( !bConnected || SG_ODBC_Get_Connection_Manager().Get_Connection(Sources[i]) )
			{
				CSG_Table_Record *pSource = pSources->Add_Record();

				pSource->Set_Value(0, Sources[i]);
				pSource->Set_Value(1, SG_ODBC_Get_Connection_Manager().Get_Connection(Sources[i]) ? 1 : 0);
			}
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
CGet_Connection::CGet_Connection(void)
{
	Set_Name		(_TL("Connect to ODBC Source"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Connects to an ODBC source."
	));

	Parameters.Add_Choice(
		"", "DSN"     , _TL("Data Source"),
		_TL(""),
		"The ODBC Data Source Name."
	);

	Parameters.Add_String(
		"", "USER"    , _TL("User"),
		_TL(""),
		""
	);

	Parameters.Add_String(
		"", "PASSWORD", _TL("Password"),
		_TL(""),
		"", false, true
	);
}

//---------------------------------------------------------
bool CGet_Connection::On_Before_Execution(void)
{
	CSG_String Sources;

	if( SG_ODBC_Get_Connection_Manager().Get_Servers(Sources) > 0 )
	{
		Parameters("DSN")->asChoice()->Set_Items(Sources);

		return( true );
	}

	Message_Dlg(
		_TW("No ODBC source available!\n"
			"Set up an ODBC source first."),
		_TL("ODBC Database Connection Error")
	);

	return( false );
}

//---------------------------------------------------------
bool CGet_Connection::On_Execute(void)
{
	CSG_String   Source = Parameters("DSN"     )->asString();
	CSG_String     User = Parameters("USER"    )->asString();
	CSG_String Password = Parameters("PASSWORD")->asString();

	if( SG_ODBC_Get_Connection_Manager().Add_Connection(Source, User, Password) )
	{
		Message_Fmt("\n%s: %s", Source.c_str(), _TL("ODBC source connected"));

		SG_UI_ODBC_Update(Source);

		return( true );
	}

	Message_Fmt("\n%s: %s", Source.c_str(), _TL("could not connect ODBC source"));

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
	Set_Name		(_TL("Disconnect from ODBC Source"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Disconnects an ODBC source connection."
	));

	Parameters.Add_Choice(
		"", "TRANSACT"	, _TL("Transactions"),
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
	CSG_String Source = Get_Connection()->Get_Server();

	if( SG_ODBC_Get_Connection_Manager().Del_Connection(Get_Connection(), Parameters("TRANSACT")->asInt() == 1) )
	{
		Message_Add(Source + ": " + _TL("ODBC source disconnected"));

		SG_UI_ODBC_Update(Source);

		return( true );
	}

	Message_Add(Source + ": " + _TL("could not disconnect ODBC source"));

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
		"Disconnects all connected ODBC sources."
	));

	Parameters.Add_Choice(
		"", "TRANSACT", _TL("Transactions"),
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
	CSG_String Sources;

	if( SG_ODBC_Get_Connection_Manager().Get_Connections(Sources) > 0 )
	{
		return( true );
	}

	Message_Dlg(
		_TL("No ODBC connection available!"),
		_TL("ODBC Database Connection Error")
	);

	return( false );
}

//---------------------------------------------------------
bool CDel_Connections::On_Execute(void)
{
	bool bCommit = Parameters("TRANSACT")->asInt() == 1;

	CSG_ODBC_Connections &Connections = SG_ODBC_Get_Connection_Manager();

	for(int i=Connections.Get_Count()-1; i>=0; i--)
	{
		Connections.Del_Connection(i, bCommit);
	}

	return( Connections.Get_Count() == 0 );
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

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Execute a commit or rollback on open transactions with ODBC source."
	));

	Parameters.Add_Choice(
		"", "SOURCE"  , _TL("Source"),
		_TL(""),
		""
	);

	Parameters.Add_Choice(
		"", "TRANSACT", _TL("Transactions"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("rollback"),
			_TL("commit")
		), 1
	);
}

//---------------------------------------------------------
bool CTransaction::On_Before_Execution(void)
{
	CSG_String Sources;

	if( SG_ODBC_Get_Connection_Manager().Get_Connections(Sources) > 0 )
	{
		Parameters("SOURCE")->asChoice()->Set_Items(Sources);

		return( true );
	}

	Message_Dlg(
		_TL("No ODBC connection available!"),
		_TL("ODBC Database Connection Error")
	);

	return( false );
}

//---------------------------------------------------------
bool CTransaction::On_Execute(void)
{
	CSG_String Source = Parameters("SOURCE")->asString();

	CSG_ODBC_Connection *pConnection = SG_ODBC_Get_Connection_Manager().Get_Connection(Source);

	if( !pConnection )
	{
		return( false );
	}

	if( Parameters("TRANSACT")->asInt() == 1 )
	{
		if( pConnection->Commit() )
		{
			Message_Fmt("\n%s: %s", Source.c_str(), _TL("open transactions committed"));

			SG_UI_ODBC_Update(Source);

			return( true );
		}
	}
	else
	{
		if( pConnection->Rollback() )
		{
			Message_Fmt("\n%s: %s", Source.c_str(), _TL("open transactions rollbacked"));

			SG_UI_ODBC_Update(Source);

			return( true );
		}
	}

	Message_Fmt("\n%s: %s", Source.c_str(), _TL("could not commit/rollback transactions."));

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

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Execute SQL commands on a connected ODBC source. "
		"Separate different commands with a semicolon (\';\'). "
	));

	Parameters.Add_String(
		"", "SQL"	, _TL("SQL Statement"),
		_TL(""),
		"CREATE TABLE myTable1 (Col1 VARCHAR(255) PRIMARY KEY, Col2 INTEGER);\n"
		"INSERT INTO myTable1 (Col1, Col2) VALUES(\'First Value\', 1);\n"
		"DROP TABLE myTable1;\n",
		true
	);

	Parameters.Add_Bool(
		"", "COMMIT", _TL("Commit"),
		_TL(""),
		true
	);

	Parameters.Add_Bool(
		"", "STOP"	, _TL("Stop on Error"),
		_TL(""),
		false
	);
}

//---------------------------------------------------------
bool CExecute_SQL::On_Execute(void)
{
	if( !Get_Connection() )
	{
		return( false );
	}

	//-----------------------------------------------------
	bool   bCommit = Parameters("COMMIT")->asBool  ();
	bool     bStop = Parameters("STOP"  )->asBool  ();
	CSG_String SQL = Parameters("SQL"   )->asString();

	//-----------------------------------------------------
	if( SQL.Find(';') < 0 )
	{
		return( Get_Connection()->Execute(SQL, bCommit) );
	}

	//-----------------------------------------------------
	int nSuccess = 0, nErrors = 0;

	SQL += ';';

	do
	{
		CSG_String s = SQL.BeforeFirst(';');

		s.Trim();

		if( s.Length() > 0 )
		{
			Message_Add(s);

			if( Get_Connection()->Execute(s, bCommit) )
			{
				nSuccess++;

				Message_Fmt("...%s!", _TL("okay"));
			}
			else
			{
				nErrors++;

				Message_Fmt("...%s!", _TL("failed"));

				if( bStop )
				{
					return( false );
				}
			}
		}

		SQL = SQL.AfterFirst(';');
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
