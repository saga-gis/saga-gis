
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
//                   Get_SQLTable.cpp                    //
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

#include "Get_SQLTable.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGet_SQLTable::CGet_SQLTable(void)
{
	Set_Name		(_TL("ODBC: Import a Table using SQL Query"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"Import a table from Open Data Base Connection (ODBC) source using SQL (Structured Query Language)."
	));

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_String(
		NULL	, "FIELDS"		, _TL("Fields"),
		_TL(""),
		SG_T("*")
	);

	Parameters.Add_String(
		NULL	, "TABLES"		, _TL("Tables"),
		_TL(""),
		SG_T("")
	);

	Parameters.Add_String(
		NULL	, "WHERE"		, _TL("Where Clause"),
		_TL(""),
		SG_T("")
	);

	Parameters.Add_String(
		NULL	, "ORDER"		, _TL("Order by"),
		_TL(""),
		SG_T("")
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGet_SQLTable::On_Before_Execution(void)
{
	if( !g_Connection.is_Connected() )
	{
		Message_Dlg(_TW(
			"No database connection available!\n"
			"Connect to an ODBC source using module \"Connect ODBC Source\" first."
		));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGet_SQLTable::On_Execute(void)
{
	if( g_Connection.is_Connected() )
	{
		CSG_Table	*pTable	= Parameters("TABLE")	->asTable();
		CSG_String	Fields	= Parameters("FIELDS")	->asString();
		CSG_String	Tables	= Parameters("TABLES")	->asString();
		CSG_String	Where	= Parameters("WHERE")	->asString();
		CSG_String	Order	= Parameters("ORDER")	->asString();

		return( g_Connection.Get_Query(Fields, Tables, Where, Order, *pTable) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
