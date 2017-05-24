/**********************************************************
 * Version $Id: table.cpp 1508 2012-11-01 16:13:43Z oconrad $
 *********************************************************/

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
//                       Table.cpp                       //
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
CTable_List::CTable_List(void)
{
	Set_Name		(_TL("List Tables"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Lists all tables of an PostgreSQL data source."
	));

	Parameters.Add_Table("",
		"TABLES"	, _TL("Tables"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
bool CTable_List::On_Execute(void)
{
	CSG_Table	*pTables	= Parameters("TABLES")->asTable();

	pTables->Destroy();
	pTables->Set_Name(Get_Connection()->Get_Connection() + " [" + _TL("Tables") + "]");

	pTables->Add_Field(_TL("Table"), SG_DATATYPE_String);
	pTables->Add_Field(_TL("Type" ), SG_DATATYPE_String);

	CSG_Strings	Tables;

	if( Get_Connection()->Get_Tables(Tables) )
	{
		CSG_Table	t;

		for(int i=0; i<Tables.Get_Count(); i++)
		{
			CSG_Table_Record	*pTable	= pTables->Add_Record();

			pTable->Set_Value(0, Tables[i]);

			if( Get_Connection()->Table_Load(t, "geometry_columns", "type", CSG_String::Format("f_table_name='%s'", Tables[i].c_str())) && t.Get_Count() == 1 )
			{
				pTable->Set_Value(1, t[0][0].asString());
			}
			else if( Get_Connection()->Table_Load(t, "raster_columns", "*", CSG_String::Format("r_table_name='%s'", Tables[i].c_str())) && t.Get_Count() == 1 )
			{
				pTable->Set_Value(1, "RASTER");
			}
			else
			{
				pTable->Set_Value(1, "TABLE");
			}
		}
	}

	return( pTables->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Info::CTable_Info(void)
{
	Set_Name		(_TL("List Table Fields"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Loads table information from PostgreSQL data source."
	));

	Parameters.Add_Table("",
		"TABLE"		, _TL("Field Description"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"TABLES"	, _TL("Table"),
		_TL(""),
		""
	);

	Parameters.Add_Bool("",
		"VERBOSE"	, _TL("Verbose"),
		_TL(""),
		false
	);
}

//---------------------------------------------------------
void CTable_Info::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_Parameter	*pParameter	= pParameters->Get_Parameter("TABLES");

	pParameter->asChoice()->Set_Items(Get_Connection()->Get_Tables());
	pParameter->Set_Value(pParameter->asString());
}

//---------------------------------------------------------
bool CTable_Info::On_Execute(void)
{
	CSG_String	Table	= Parameters("TABLES")->asString();
	CSG_Table	*pTable	= Parameters("TABLE" )->asTable();

	pTable->Create(Get_Connection()->Get_Field_Desc(Table, Parameters("VERBOSE")->asBool()));
	pTable->Set_Name(Table + " [" + _TL("Field Description") + "]");

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
	Set_Name		(_TL("Import Table"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Imports a table from a PostgreSQL database."
	));

	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"TABLES"	, _TL("Tables"),
		_TL(""),
		""
	);
}

//---------------------------------------------------------
void CTable_Load::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_Parameter	*pParameter	= pParameters->Get_Parameter("TABLES");

	pParameter->asChoice()->Set_Items(Get_Connection()->Get_Tables());
	pParameter->Set_Value(pParameter->asString());
}

//---------------------------------------------------------
bool CTable_Load::On_Execute(void)
{
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	return( Get_Connection()->Table_Load(*pTable, Parameters("TABLES")->asString()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Save::CTable_Save(void)
{
	Set_Name		(_TL("Export Table"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Exports a table to a PostgreSQL database."
	));

	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Set_Constraints(&Parameters, "TABLE");

	Parameters.Add_String("",
		"NAME"		, _TL("Table Name"),
		_TL(""),
		""
	);

	Parameters.Add_Choice("",
		"EXISTS"	, _TL("If table exists..."),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("abort export"),
			_TL("replace existing table"),
			_TL("append records, if table structure allows")
		), 0
	);
}

//---------------------------------------------------------
int CTable_Save::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TABLE") )
	{
		pParameters->Get_Parameter("NAME")->Set_Value(pParameter->asTable() ? pParameter->asTable()->Get_Name() : SG_T(""));
	}

	return( CSG_PG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CTable_Save::On_Execute(void)
{
	bool		bResult	= false;
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();
	CSG_String	Name	= Parameters("NAME" )->asString();	if( Name.Length() == 0 )	Name	= pTable->Get_Name();

	//-----------------------------------------------------
	CSG_String	SavePoint;

	Get_Connection()->Begin(SavePoint = Get_Connection()->is_Transaction() ? "SHAPES_SAVE" : "");

	//-----------------------------------------------------
	if( Get_Connection()->Table_Exists(Name) )
	{
		Message_Add(CSG_String::Format("%s: %s", _TL("table already exists"), Name.c_str()));

		switch( Parameters("EXISTS")->asInt() )
		{
		case 0:	// abort export
			break;

		case 1:	// replace existing table
			Message_Add(CSG_String::Format("%s: %s", _TL("dropping table"), Name.c_str()));

			if( !Get_Connection()->Table_Drop(Name, false) )
			{
				Message_Add(CSG_String::Format(" ...%s!", _TL("failed")));
			}
			else
			{
				bResult	= Get_Connection()->Table_Save(Name, *pTable, Get_Constraints(&Parameters, "TABLE"));
			}
			break;

		case 2:	// append records, if table structure allows
			Message_Add(CSG_String::Format("%s: %s", _TL("appending to existing table"), Name.c_str()));

			if( !(bResult = Get_Connection()->Table_Insert(Name, *pTable)) )
			{
				Message_Add(CSG_String::Format(" ...%s!", _TL("failed")));
			}
			break;
		}
	}
	else
	{
		bResult	= Get_Connection()->Table_Save(Name, *pTable, Get_Constraints(&Parameters, "TABLE"));
	}

	//-----------------------------------------------------
	if( !bResult )
	{
		Get_Connection()->Rollback(SavePoint);
	}
	else
	{
		Get_Connection()->Commit(SavePoint);

		Get_Connection()->GUI_Update();
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Drop::CTable_Drop(void)
{
	Set_Name		(_TL("Drop Table"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Deletes a table from a PostgreSQL database."
	));

	Parameters.Add_Choice("",
		"TABLES"	, _TL("Tables"),
		_TL(""),
		""
	);
}

//---------------------------------------------------------
void CTable_Drop::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_Parameter	*pParameter	= pParameters->Get_Parameter("TABLES");

	pParameter->asChoice()->Set_Items(Get_Connection()->Get_Tables());
	pParameter->Set_Value(pParameter->asString());
}

//---------------------------------------------------------
bool CTable_Drop::On_Execute(void)
{
	if( Get_Connection()->Table_Drop(Parameters("TABLES")->asChoice()->asString()) )
	{
		Get_Connection()->GUI_Update();

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
CTable_Query::CTable_Query(void)
{
	Set_Name		(_TL("Import Table from SQL Query"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Import a SQL table from a PostgreSQL database."
	));

	Parameters.Add_Table("",
		"TABLE"		, _TL("Query Result"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_String("", "TABLES"  , _TL("Tables"  ), _TL(""), "");
	Parameters.Add_String("", "FIELDS"  , _TL("Fields"  ), _TL(""), "");
	Parameters.Add_String("", "WHERE"   , _TL("Where"   ), _TL(""), "");
	Parameters.Add_String("", "GROUP"   , _TL("Group by"), _TL(""), "");
	Parameters.Add_String("", "HAVING"  , _TL("Having"  ), _TL(""), "");
	Parameters.Add_String("", "ORDER"   , _TL("Order by"), _TL(""), "");
	Parameters.Add_Bool  ("", "DISTINCT", _TL("Distinct"), _TL("")    );
}

//---------------------------------------------------------
bool CTable_Query::On_Execute(void)
{
	return( Get_Connection()->Table_Load(
		*Parameters("TABLE"   )->asTable (),
		 Parameters("TABLES"  )->asString(),
		 Parameters("FIELDS"  )->asString(),
		 Parameters("WHERE"   )->asString(),
		 Parameters("GROUP"   )->asString(),
		 Parameters("HAVING"  )->asString(),
		 Parameters("ORDER"   )->asString(),
		 Parameters("DISTINCT")->asBool  ()
	));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Query_GUI::CTable_Query_GUI(void)
{
	Set_Name		(_TL("Import Table from SQL Query (GUI)"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"Import a SQL table from a PostgreSQL database."
	));

	Parameters.Add_Table("",
		"TABLE"		, _TL("Query Result"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Parameters("", "TABLES"  , _TL("Tables"  ), _TL("")    );
	Parameters.Add_Parameters("", "FIELDS"  , _TL("Fields"  ), _TL("")    );
	Parameters.Add_String    ("", "WHERE"   , _TL("Where"   ), _TL(""), "");
	Parameters.Add_Parameters("", "GROUP"   , _TL("Group by"), _TL("")    );
	Parameters.Add_String    ("", "HAVING"  , _TL("Having"  ), _TL(""), "");
	Parameters.Add_String    ("", "ORDER"   , _TL("Order by"), _TL(""), "");
	Parameters.Add_Bool      ("", "DISTINCT", _TL("Distinct"), _TL("")    );
}

//---------------------------------------------------------
void CTable_Query_GUI::On_Connection_Changed(CSG_Parameters *pParameters)
{
	CSG_Parameters	*pTables	= pParameters->Get_Parameter("TABLES")->asParameters();

	pTables->Del_Parameters();

	CSG_Strings	Tables;

	if( Get_Connection()->Get_Tables(Tables) > 0 )
	{
		for(int i=0; i<Tables.Get_Count(); i++)
		{
			pTables->Add_Bool("", Tables[i], Tables[i], "");
		}
	}

	pParameters->Get_Parameter("FIELDS")->asParameters()->Del_Parameters();
	pParameters->Get_Parameter("GROUP" )->asParameters()->Del_Parameters();

	On_Parameter_Changed(pParameters, pParameters->Get_Parameter("TABLES"));
}

//---------------------------------------------------------
int CTable_Query_GUI::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TABLES") )
	{
		CSG_Parameters	&Tables	= *pParameters->Get_Parameter("TABLES")->asParameters();
		CSG_Parameters	&Fields	= *pParameters->Get_Parameter("FIELDS")->asParameters();
		CSG_Parameters	&Group	= *pParameters->Get_Parameter("GROUP" )->asParameters();

		for(int i=0; i<Tables.Get_Count(); i++)
		{
			CSG_String	Table	= Tables[i].Get_Identifier();

			if( Tables[i].asBool() && !Fields(Table) )
			{
				CSG_Table	Desc	= Get_Connection()->Get_Field_Desc(Table);

				CSG_Parameter	*pFields	= Fields.Add_Node("", Table, Table, "");
				CSG_Parameter	*pGroup 	= Group .Add_Node("", Table, Table, "");

				for(int j=0; j<Desc.Get_Count(); j++)
				{
					CSG_String	ID	= Table + "." + Desc[j].asString(0);

					Fields.Add_Bool(pFields, ID, Desc[j].asString(0), "");
					Group .Add_Bool(pGroup , ID, Desc[j].asString(0), "");
				}
			}
			else if( !Tables[i].asBool() && Fields(Table) )
			{
				Fields.Del_Parameter(Table);
				Group .Del_Parameter(Table);
			}
		}
	}

	return( CSG_PG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CTable_Query_GUI::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{

	return( CSG_PG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CTable_Query_GUI::On_Execute(void)
{
	return( Get_Connection()->Table_Load(
		*Parameters   ("TABLE"   )->asTable(),
		 Get_Selection("TABLES"  ),
		 Get_Selection("FIELDS"  ),
		 Parameters   ("WHERE"   )->asString(),
		 Get_Selection("GROUP"   ),
		 Parameters   ("HAVING"  )->asString(),
		 Parameters   ("ORDER"   )->asString(),
		 Parameters   ("DISTINCT")->asBool  ()
	));
}

//---------------------------------------------------------
CSG_String CTable_Query_GUI::Get_Selection(const CSG_String &Parameter)
{
	CSG_String	s;

	if( Parameters(Parameter) && Parameters(Parameter)->asParameters() )
	{
		CSG_Parameters	&P	= *Parameters(Parameter)->asParameters();

		for(int i=0; i<P.Get_Count(); i++)
		{
			if( P[i].Get_Type() == PARAMETER_TYPE_Bool && P[i].asBool() )
			{
				if( !s.is_Empty() )
				{
					s	+= ",";
				}

				s	+= P[i].Get_Identifier();
			}
		}
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
