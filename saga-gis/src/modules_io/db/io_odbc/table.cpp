/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        io_odbc                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       Table.cpp                       //
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
#include "get_connection.h"

#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Info::CTable_Info(void)
{
	Set_Name		(_TL("Table Field Description"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Loads table information from ODBC data source."
	));

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Field Description"),
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

//---------------------------------------------------------
bool CTable_Info::On_Before_Execution(void)
{
	if( !CSG_ODBC_Module::On_Before_Execution() )
	{
		return( false );
	}

	CSG_String	Table(Parameters("TABLES")->asString());

	Parameters("TABLES")->asChoice()->Set_Items(Get_Connection()->Get_Tables());
	Parameters("TABLES")->Set_Value(Table);

	return( true );
}

//---------------------------------------------------------
bool CTable_Info::On_Execute(void)
{
	if( !Get_Connection() )
	{
		return( false );
	}

	CSG_Parameter_Choice	*pTables	= Parameters("TABLES")	->asChoice();
	CSG_Table				*pTable		= Parameters("TABLE")	->asTable();

	pTable->Assign(&Get_Connection()->Get_Field_Desc(pTables->asString()));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Load::CTable_Load(void)
{
	Set_Name		(_TL("Table Import"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Imports a table from a database via ODBC."
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

//---------------------------------------------------------
bool CTable_Load::On_Before_Execution(void)
{
	if( !CSG_ODBC_Module::On_Before_Execution() )
	{
		return( false );
	}

	CSG_String	Tables(Get_Connection()->Get_Tables());

	Parameters("TABLES")->asChoice()->Set_Items(Tables);
	Parameters("TABLES")->Set_Value(Parameters("TABLES")->asString());

	return( Tables.Length() > 0 );
}

//---------------------------------------------------------
bool CTable_Load::On_Execute(void)
{
	if( !Get_Connection() )
	{
		return( false );
	}

	CSG_Parameter_Choice	*pTables	= Parameters("TABLES")	->asChoice();
	CSG_Table				*pTable		= Parameters("TABLE")	->asTable();

	return( Get_Connection()->Table_Load(*pTable, pTables->asString()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Save::CTable_Save(void)
{
	Set_Name		(_TL("Table Export"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Exports a table to a database via ODBC."
	));

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_String(
		NULL	, "NAME"		, _TL("Table Name"),
		_TL(""),
		SG_T("")
	);

	Parameters.Add_Parameters(
		NULL	, "FLAGS"		, _TL("Constraints"),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "EXISTS"		, _TL("If table exists..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("abort export"),
			_TL("replace existing table"),
			_TL("append records, if table structure allows")
		), 0
	);
}

//---------------------------------------------------------
int CTable_Save::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("TABLE")) )
	{
		pParameters->Get_Parameter("NAME")->Set_Value(pParameter->asTable() ? pParameter->asTable()->Get_Name() : SG_T(""));

		Set_Constraints(pParameters->Get_Parameter("FLAGS")->asParameters(), pParameter->asTable());
	}

	return( 0 );
}

//---------------------------------------------------------
bool CTable_Save::On_Execute(void)
{
	if( !Get_Connection() )
	{
		return( false );
	}

	CSG_Table	*pTable	= Parameters("TABLE")	->asTable();
	CSG_String	Name	= Parameters("NAME")	->asString();	if( Name.Length() == 0 )	Name	= pTable->Get_Name();

	//-----------------------------------------------------
	if( Get_Connection()->Table_Exists(Name) )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("table already exists"), Name.c_str()));

		switch( Parameters("EXISTS")->asInt() )
		{
		case 0:	// abort export
			return( false );

		case 1:	// replace existing table
			Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("dropping table"), Name.c_str()));

			if( !Get_Connection()->Table_Drop(Name, false) )
			{
				Message_Add(CSG_String::Format(SG_T(" ...%s!"), _TL("failed")));

				return( false );
			}

			return( Get_Connection()->Table_Save(Name, *pTable, Get_Constraints(Parameters("FLAGS")->asParameters(), pTable)) );

		case 2:	// append records, if table structure allows
			Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("appending to existing table"), Name.c_str()));

			if( !Get_Connection()->Table_Insert(Name, *pTable) )
			{
				Message_Add(CSG_String::Format(SG_T(" ...%s!"), _TL("failed")));

				return( false );
			}

			return( true );
		}
	}
	else
	{
		return( Get_Connection()->Table_Save(Name, *pTable, Get_Constraints(Parameters("FLAGS")->asParameters(), pTable)) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Drop::CTable_Drop(void)
{
	Set_Name		(_TL("Table Deletion"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Deletes a table from a database via ODBC."
	));

	Parameters.Add_Choice(
		NULL	, "TABLES"		, _TL("Tables"),
		_TL(""),
		CSG_String::Format(SG_T("%s|"),
			_TL("--- no table available ---")
		)
	);
}

//---------------------------------------------------------
bool CTable_Drop::On_Before_Execution(void)
{
	if( !CSG_ODBC_Module::On_Before_Execution() )
	{
		return( false );
	}

	CSG_String	Table(Parameters("TABLES")->asString());

	Parameters("TABLES")->asChoice()->Set_Items(Get_Connection()->Get_Tables());

	if( Parameters("TABLES")->asChoice()->Get_Count() == 0 )
	{
		Message_Dlg(_TL("No tables available in database!"));

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CTable_Drop::On_Execute(void)
{
	if( !Get_Connection() )
	{
		return( false );
	}

	return( Get_Connection()->Table_Drop(Parameters("TABLES")->asChoice()->asString()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Query::CTable_Query(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Table from Query"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Import a SQL table from a database via ODBC."
	));

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table from SQL Query"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_String(
		NULL	, "TABLES"		, _TL("Tables"),
		_TL(""),
		SG_T("")
	);

	Parameters.Add_String(
		NULL	, "FIELDS"		, _TL("Fields"),
		_TL(""),
		SG_T("*")
	);

	Parameters.Add_String(
		NULL	, "WHERE"		, _TL("Where"),
		_TL(""),
		SG_T("")
	);

	pNode	= Parameters.Add_String(
		NULL	, "GROUP"		, _TL("Group by"),
		_TL(""),
		SG_T("")
	);

	Parameters.Add_String(
		pNode	, "HAVING"		, _TL("Having"),
		_TL(""),
		SG_T("")
	);

	Parameters.Add_String(
		NULL	, "ORDER"		, _TL("Order by"),
		_TL(""),
		SG_T("")
	);

	Parameters.Add_Value(
		NULL	, "DISTINCT"	, _TL("Distinct"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}

//---------------------------------------------------------
bool CTable_Query::On_Execute(void)
{
	if( !Get_Connection() )
	{
		return( false );
	}

	CSG_Table	*pTable		= Parameters("TABLE")	->asTable ();
	CSG_String	Tables		= Parameters("TABLES")	->asString();
	CSG_String	Fields		= Parameters("FIELDS")	->asString();
	CSG_String	Where		= Parameters("WHERE")	->asString();
	CSG_String	Group		= Parameters("GROUP")	->asString();
	CSG_String	Having		= Parameters("HAVING")	->asString();
	CSG_String	Order		= Parameters("ORDER")	->asString();
	bool		bDistinct	= Parameters("DISTINCT")->asBool  ();

	return( Get_Connection()->Table_Load(*pTable, Tables, Fields, Where, Group, Having, Order, bDistinct) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
