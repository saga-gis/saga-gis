
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

#include "Get_Table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGet_Table::CGet_Table(void)
{
	Set_Name		(_TL("ODBC: Import a Table"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"Imports a table from a data base via ODBC."
	));

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "TABLES"		, _TL("Tables"),
		_TL(""),
		CSG_String::Format(SG_T("%s|"),
			_TL("--- no table available ---")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGet_Table::On_Before_Execution(void)
{
	if( !g_Connection.is_Connected() )
	{
		Message_Dlg(_TW(
			"No database connection available!\n"
			"Connect to an ODBC source using module \"Connect ODBC Source\" first."
		));

		return( false );
	}

	CSG_String	Table(Parameters("TABLES")->asString());

	Parameters("TABLES")->asChoice()->Set_Items(g_Connection.Get_Tables());
	Parameters("TABLES")->Set_Value(Table);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGet_Table::On_Execute(void)
{
	if( g_Connection.is_Connected() )
	{
		CSG_Parameter_Choice	*pTables	= Parameters("TABLES")	->asChoice();
		CSG_Table				*pTable		= Parameters("TABLE")	->asTable();

		return( g_Connection.Get_Table(pTables->asInt(), pTables->asString(), *pTable) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
