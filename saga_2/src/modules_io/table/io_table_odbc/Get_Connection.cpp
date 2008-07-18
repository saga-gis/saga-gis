
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
#include "Get_Connection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_ODBC_Connection	g_Connection;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define STR_DISCONNTECT	_TL("--- DISCONNECT ---")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGet_Connection::CGet_Connection(void)
{
	Set_Name		(_TL("ODBC: Connect/Disconnect"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"Connect/disconnect ODBC source."
	));

	Parameters.Add_Choice(
		NULL	, "SERVERS"		, _TL("Server"),
		_TL(""),
		g_Connection.Get_Servers()
	);

	Parameters.Add_String(
		NULL	, "USERNAME"	, _TL("User"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_String(
		NULL	, "PASSWORD"	, _TL("Password"),
		_TL(""),
		SG_T(""), false, true
	);

	Parameters.Add_FilePath(
		NULL	, "DIRPATH"		, _TL("Directory"),
		_TL(""),
		NULL, NULL, false, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGet_Connection::On_Before_Execution(void)
{
	CSG_String	Servers(g_Connection.Get_Servers());

	if( g_Connection.is_Connected() )
	{
		Servers	+= CSG_String::Format(SG_T("%s|"), STR_DISCONNTECT);
	}

	Parameters("SERVERS")->asChoice()->Set_Items(Servers);
	Parameters("SERVERS")->Set_Value(g_Connection.Get_Server());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGet_Connection::On_Execute(void)
{
	CSG_String	Server, User, Password, Directory;

	//-----------------------------------------------------
	if( g_Connection.is_Connected() )
	{
		Server	= g_Connection.Get_Server();

		g_Connection.Disconnect();

		Message_Add(CSG_String::Format(SG_T("%s: %s"), Server.c_str(), _TL("ODBC source disconnected")));
	}

	//-----------------------------------------------------
	Server		= Parameters("SERVERS")		->asString();
	User		= Parameters("USERNAME")	->asString();
	Password	= Parameters("PASSWORD")	->asString();
	Directory	= Parameters("DIRPATH")		->asString();

	if( Server.Cmp(STR_DISCONNTECT) == 0 )
	{
		return( true );
	}

	if( g_Connection.Connect(Server, User, Password, Directory) )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), Server.c_str(), _TL("ODBC source connected")));

		return( true );
	}
	else
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), Server.c_str(), _TL("could not connect ODBC source")));

		return( false );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
