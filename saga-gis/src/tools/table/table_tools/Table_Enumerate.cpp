/**********************************************************
 * Version $Id$
 *********************************************************/

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
bool CTable_Enumerate::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("INPUT")->asTable();

	if( pTable->Get_Record_Count() <= 0 )
	{
		Error_Set(_TL("no records in data set"));

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("OUTPUT")->asTable() && Parameters("OUTPUT")->asTable() != pTable )
	{
		CSG_Table	*pCopy	= pTable;	pTable	= Parameters("OUTPUT")->asTable();

		if( pCopy->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
		{
			((CSG_Shapes *)pTable)->Create(*((CSG_Shapes *)pTable));
		}
		else
		{
			pTable->Create(pCopy);
		}

		pTable->Set_Name(CSG_String::Format("%s [%s]", pTable->Get_Name(), _TL("Enumerated")));
	}

	//-----------------------------------------------------
	int	nFields	= Parameters("ENUM")->asInt();
	

	int	Field	= Parameters("FIELD")->asInt();

	//-----------------------------------------------------
	if( Field < 0 )
	{
		if( nFields < 0 )
		{
			nFields	= pTable->Get_Field_Count();

			pTable->Add_Field("NR", SG_DATATYPE_Int);
		}

		for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
		{
			pTable->Get_Record_byIndex(i)->Set_Value(nFields, i + 1);
		}
	}

	//-----------------------------------------------------
	else
	{
		if( nFields < 0 )
		{
			nFields	= pTable->Get_Field_Count();

			pTable->Add_Field(CSG_String::Format("NR-%s", pTable->Get_Field_Name(Field)), SG_DATATYPE_Int);
		}

		TSG_Table_Index_Order	old_Order	= pTable->Get_Index_Order(0);
		int						old_Field	= pTable->Get_Index_Field(0);

		pTable->Set_Index(Field, TABLE_INDEX_Descending);

		CSG_String	Value	= pTable->Get_Record_byIndex(0)->asString(Field);

		for(int i=0, ID=1; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record_byIndex(i);

			if( Value.Cmp(pRecord->asString(Field)) )
			{
				Value	= pRecord->asString(Field);

				ID++;
			}

			pRecord->Set_Value(nFields, ID);
		}

		pTable->Set_Index(old_Field, old_Order);
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
