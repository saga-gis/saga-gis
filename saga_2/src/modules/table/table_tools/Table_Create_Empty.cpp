
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Table_Tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Table_Create_Empty.cpp                //
//                                                       //
//                 Copyright (C) 2005 by                 //
//              SAGA User Group Associaton               //
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
//    e-mail:     oconrad@gwdg.de                        //
//                                                       //
//    contact:    SAGA User Group Associaton             //
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
#include "Table_Create_Empty.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Create_Empty::CTable_Create_Empty(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Create Empty Table"));

	Set_Author		(_TL("Copyrights (c) 2005 by Olaf Conrad"));

	Set_Description	(_TL(
		"Creates a new empty table."
	));


	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TABLE"	, _TL("Table"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_String(
		NULL	, "NAME"	, _TL("Name"),
		"",
		_TL("New table")
	);

	Parameters.Add_Value(
		NULL	, "FIELDS"	, _TL("Number of Fields"),
		"",
		PARAMETER_TYPE_Int, 1, 1, true
	);
}

//---------------------------------------------------------
CTable_Create_Empty::~CTable_Create_Empty(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Create_Empty::On_Execute(void)
{
	int					iField, nFields, iType;
	TTable_FieldType	Type;
	CSG_String			Name, Types, s;
	CTable				*pTable;
	CParameters			P;
	CParameter			*pNode;

	//-----------------------------------------------------
	nFields	= Parameters("FIELDS")	->asInt();
	Name	= Parameters("NAME")	->asString();
	pTable	= Parameters("TABLE")	->asTable();

	Types.Printf("%s|%s|%s|%s|%s|%s|%s|",
		LNG("character string"),
		LNG("1 byte integer"),
		LNG("2 byte integer"),
		LNG("4 byte integer"),
		LNG("4 byte floating point"),
		LNG("8 byte floating point"),
		LNG("color (rgb)")
	);

	P.Set_Name(_TL("Field Properties"));

	for(iField=1; iField<=nFields; iField++)
	{
		s.Printf("NODE_%03d", iField);
		pNode	= P.Add_Node(NULL, s, CSG_String::Format("%d. %s", iField, _TL("Field")), "");

		s.Printf("FIELD_%03d", iField);
		P.Add_String(pNode, s, _TL("Name"), _TL(""), s);

		s.Printf("TYPE_%03d" , iField);
		P.Add_Choice(pNode, s, _TL("Type"), _TL(""), Types);
	}

	//-----------------------------------------------------
	if( Dlg_Parameters(&P, _TL("Field Properties")) )
	{
		pTable->Create(Name);

		for(iField=0; iField<nFields; iField++)
		{
			Name	= P(CSG_String::Format("FIELD_%03d", iField + 1).c_str())->asString();
			iType	= P(CSG_String::Format("TYPE_%03d" , iField + 1).c_str())->asInt();

			switch( iType )
			{
			default:
			case 0:	Type	= TABLE_FIELDTYPE_String;	break;
			case 1:	Type	= TABLE_FIELDTYPE_Char;		break;
			case 2:	Type	= TABLE_FIELDTYPE_Short;	break;
			case 3:	Type	= TABLE_FIELDTYPE_Int;		break;
			case 4:	Type	= TABLE_FIELDTYPE_Float;	break;
			case 5:	Type	= TABLE_FIELDTYPE_Double;	break;
			case 6:	Type	= TABLE_FIELDTYPE_Color;	break;
			}

			pTable->Add_Field(Name, Type);
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
