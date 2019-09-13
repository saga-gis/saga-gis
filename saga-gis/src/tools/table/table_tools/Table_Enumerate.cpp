
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Table_Tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Table_Enumerate.cpp                  //
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
#include "Table_Enumerate.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Enumerate::CTable_Enumerate(bool bShapes)
{
	Set_Author("O.Conrad (c) 2017");

	Set_Description(_TW(
		"Enumeration of a table attribute, i.e. a unique identifier "
		"is assigned to identical values of the chosen attribute field. "
		"If no attribute is chosen, a simple enumeration is done for "
		"all records, and this with respect to the sorting order "
		"if the dataset has been indexed.\n"
	));

	if( bShapes )
	{
		Set_Name(_TL("Table Field Enumeration (Shapes)"));

		Parameters.Add_Shapes("", "INPUT" , _TL("Input" ), _TL(""), PARAMETER_INPUT);
		Parameters.Add_Shapes("", "OUTPUT", _TL("Output"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}
	else
	{
		Set_Name(_TL("Table Field Enumeration"));

		Parameters.Add_Table ("", "INPUT" , _TL("Input" ), _TL(""), PARAMETER_INPUT);
		Parameters.Add_Table ("", "OUTPUT", _TL("Output"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}

	Parameters.Add_Table_Field("INPUT",
		"FIELD"	, _TL("Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field("INPUT",
		"ENUM"	, _TL("Enumeration"),
		_TL(""),
		true
	);

	Parameters.Add_String("ENUM",
		"NAME"	, _TL("Enumeration Field Name"),
		_TL(""),
		"ENUM"
	);

	Parameters.Add_Choice("",
		"ORDER"	, _TL("Order"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("ascending"),
			_TL("descending")
		), 0
	);
}

//---------------------------------------------------------
CSG_String CTable_Enumerate::Get_MenuPath(void)
{
	if( Parameters("INPUT")->Get_Type() == PARAMETER_TYPE_Shapes )
	{
		return( _TL("A:Shapes|Table") );
	}

	return( CSG_Tool::Get_MenuPath() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Enumerate::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("ENUM") )
	{
		CSG_Table	*pTable	= (*pParameters)("INPUT")->asTable();

		pParameters->Set_Enabled("NAME", pTable && pParameter->asInt() >= pTable->Get_Field_Count());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Enumerate::On_Execute(void)
{
	CSG_Table	*pTable	= Parameters("INPUT")->asTable();

	if( pTable->Get_Record_Count() <= 0 )
	{
		Error_Set(_TL("no records in data set"));

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("OUTPUT")->asTable() && Parameters("OUTPUT")->asTable() != pTable )
	{
		CSG_Table	*pInput	= pTable;	pTable	= Parameters("OUTPUT")->asTable();

		if( pTable->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
		{
			((CSG_Shapes *)pTable)->Create(*((CSG_Shapes *)pInput));	// copy constructor
		}
		else
		{
			pTable->Create(*pInput);	// copy constructor
		}

		pTable->Fmt_Name("%s [%s]", pTable->Get_Name(), _TL("Enumerated"));
	}

	//-----------------------------------------------------
	bool	bAscending	= Parameters("ORDER")->asInt() != 1;

	int	Field	= Parameters("FIELD")->asInt();
	int	Enum	= Parameters("ENUM" )->asInt();

	if( Enum < 0 )
	{
		Enum	= pTable->Get_Field_Count();

		CSG_String	Name	= Parameters("NAME")->asString();

		if( Name.is_Empty() )
		{
			Name	= "NR";
		}

		if( Field >= 0 )
		{
			Name	+= CSG_String("_") + pTable->Get_Field_Name(Field);
		}

		pTable->Add_Field(Name, SG_DATATYPE_Int);
	}

	//-----------------------------------------------------
	if( Field < 0 )
	{
		for(int i=0; i<pTable->Get_Count(); i++)
		{
			pTable->Get_Record_byIndex(i)->Set_Value(Enum, bAscending ? 1 + i : pTable->Get_Count() - i);
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Index	Index;

		if( !pTable->Set_Index(Index, Field, bAscending) )
		{
			Error_Fmt("%s (%s)", _TL("failed to create index on field"), pTable->Get_Field_Name(Field));

			return( false );
		}

		CSG_String	Value(pTable->Get_Record(Index[0])->asString(Field));

		for(int i=0, n=1; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record(Index[i]);

			if( Value.Cmp(pRecord->asString(Field)) )
			{
				Value	= pRecord->asString(Field);

				n++;
			}

			pRecord->Set_Value(Enum, n);
		}
	}

	//-----------------------------------------------------
	if( pTable == Parameters("INPUT")->asTable() )
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
