
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      table_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               table_change_field_type.cpp             //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table_change_field_type.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Change_Field_Type::CTable_Change_Field_Type(void)
{
	Set_Name		(_TL("Change Field Type"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"With this tool you can change the data type of a table's attribute field."
	));

	Parameters.Add_Table("",
		"TABLE"	, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD" , _TL("Field"),
		_TL("")
	);

	Parameters.Add_Table("",
		"OUTPUT", _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Data_Type("",
		"TYPE"  , _TL("Data Type"),
		_TL(""),
		SG_DATATYPES_Table
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Change_Field_Type::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("TABLE")
	||  pParameter->Cmp_Identifier("FIELD") )
	{
		CSG_Table *pTable = (*pParameters)("TABLE")->asTable();

		if( pTable )
		{
			(*pParameters)("TYPE")->asDataType()->Set_Data_Type(
				pTable->Get_Field_Type((*pParameters)("FIELD")->asInt())
			);
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Change_Field_Type::On_Execute(void)
{
	CSG_Table *pTable = Parameters("OUTPUT")->asTable();

	if( pTable && pTable != Parameters("TABLE")->asTable() )
	{
		pTable->Assign  (Parameters("TABLE")->asTable());
		pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());
	}
	else
	{
		pTable = Parameters("TABLE")->asTable();
	}

	//-----------------------------------------------------
	TSG_Data_Type Type = Parameters("TYPE")->asDataType()->Get_Data_Type();

	int Field = Parameters("FIELD")->asInt();

	if( Type == pTable->Get_Field_Type(Field) )
	{
		Message_Add(_TL("nothing to do: original and desired field types are identical"));

		return( true );
	}

	//-----------------------------------------------------
	pTable->Set_Field_Type(Field, Type);

	if( pTable == Parameters("TABLE")->asTable() )
	{
		DataObject_Update(pTable);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Change_Field_Name::CTable_Change_Field_Name(void)
{
	Set_Name		(_TL("Change Field Name"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"With this tool you can change the name of a table's attribute field."
	));

	Parameters.Add_Table("",
		"TABLE"	, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD" , _TL("Field"),
		_TL("")
	);

	Parameters.Add_Table("",
		"OUTPUT", _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_String("",
		"NAME"  , _TL("Name"),
		_TL(""),
		""
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Change_Field_Name::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("TABLE")
	||  pParameter->Cmp_Identifier("FIELD") )
	{
		CSG_Table *pTable = (*pParameters)("TABLE")->asTable();

		if( pTable )
		{
			pParameters->Set_Parameter("NAME", pTable->Get_Field_Name((*pParameters)("FIELD")->asInt()));
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Change_Field_Name::On_Execute(void)
{
	CSG_Table *pTable = Parameters("OUTPUT")->asTable();

	if( pTable && pTable != Parameters("TABLE")->asTable() )
	{
		pTable->Assign  (Parameters("TABLE")->asTable());
		pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());
	}
	else
	{
		pTable = Parameters("TABLE")->asTable();
	}

	//-----------------------------------------------------
	CSG_String Name(Parameters("NAME")->asString());

	int Field = Parameters("FIELD")->asInt();

	if( !Name.Cmp(pTable->Get_Field_Name(Field)) )
	{
		Message_Add(_TL("nothing to do: original and new field name are identical"));

		return( true );
	}

	//-----------------------------------------------------
	pTable->Set_Field_Name(Field, Name);

	if( pTable == Parameters("TABLE")->asTable() )
	{
		DataObject_Update(pTable);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
