
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
//                    Table_Rotate.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#include "Table_Rotate.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Rotate::CTable_Rotate(void)
{
	Set_Name		(_TL("Transpose Table"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Transposes a table, i.e. swap the rows and columns."
	));

	Parameters.Add_Table("", "INPUT" , _TL("Input" ), _TL(""), PARAMETER_INPUT          );
	Parameters.Add_Table("", "OUTPUT", _TL("Output"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Data_Type("",
		"TYPE", _TL("Data Type"),
		_TL(""),
		SG_DATATYPES_String|SG_DATATYPES_Numeric, SG_DATATYPE_String
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Rotate::On_Execute(void)
{
	CSG_Table *pTable = Parameters("INPUT")->asTable();

	if( pTable->Get_Field_Count() < 1 || pTable->Get_Count() < 1 )
	{
		Error_Set(_TL("Empty or invalid input table."));

		return( false );
	}

	if( pTable->Get_ObjectType() != SG_DATAOBJECT_TYPE_Table )
	{
		Error_Set(_TL("Attribute tables of vector data cannot be transposed directly."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table *pOutput = Parameters("OUTPUT")->asTable(), Table;

	if( pOutput == NULL || pOutput == pTable )
	{
		pOutput = pTable; Table.Create(*pTable); pTable = &Table;
	}

	pOutput->Destroy();

	if( pOutput != Parameters("INPUT")->asTable() )
	{
		pOutput->Fmt_Name("%s [%s]", pTable->Get_Name(), _TL("rotated"));
	}

	TSG_Data_Type Type = Parameters("TYPE")->asDataType()->Get_Data_Type();

	pOutput->Add_Field(pTable->Get_Field_Name(0), SG_DATATYPE_String);

	for(int row=0; row<pTable->Get_Count(); row++)
	{
		pOutput->Add_Field(pTable->Get_Record(row)->asString(0), Type);
	}

	//-----------------------------------------------------
	for(int col=1; col<pTable->Get_Field_Count(); col++)
	{
		CSG_Table_Record &Output = *pOutput->Add_Record();

		Output.Set_Value(0, pTable->Get_Field_Name(col));

		for(int row=0; row<pTable->Get_Count(); row++)
		{
			if( Type == SG_DATATYPE_String )
			{
				Output.Set_Value(row + 1, pTable->Get_Record(row)->asString(col));
			}
			else
			{
				Output.Set_Value(row + 1, pTable->Get_Record(row)->asDouble(col));
			}
		}
	}

	//-----------------------------------------------------
	if( pOutput == Parameters("INPUT")->asTable() )
	{
		DataObject_Update(pOutput);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
