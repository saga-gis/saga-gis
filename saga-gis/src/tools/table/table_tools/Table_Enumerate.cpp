
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
CTable_Enumerate::CTable_Enumerate(void)
{
	Set_Name		(_TL("Field Enumeration"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"Enumeration of a table attribute, i.e. a unique identifier "
		"is assigned to identical values of the chosen attribute field. "
		"If no attribute is chosen, a simple enumeration is done for "
		"all records, and this with respect to the sorting order "
		"if the dataset has been indexed.\n"
		"The tool can be used with tables, shapes or point clouds."
	));

	Parameters.Add_Table("",
		"INPUT"	, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("INPUT",
		"FIELD"	, _TL("Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field("INPUT",
		"ENUM"	, _TL("Enumeration"),
		_TL("Field to which the enumeration is written. If not set, a new attribute field is created."),
		true
	);

	Parameters.Add_String("",
		"NAME"	, _TL("Enumeration Field Name"),
		_TL("The field name of the created attribute. If an attribute is chosen, the name is used as prefix."),
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

	Parameters.Add_Table     ("", "RESULT_TABLE" , _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Shapes    ("", "RESULT_SHAPES", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_PointCloud("", "RESULT_PC"    , _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Enumerate::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("INPUT") )
	{
		CSG_Data_Object *pTable = pParameter->asDataObject();

		pParameters->Set_Enabled("RESULT_TABLE" , pTable && pTable->asTable     ());
		pParameters->Set_Enabled("RESULT_SHAPES", pTable && pTable->asShapes    ());
		pParameters->Set_Enabled("RESULT_PC"	, pTable && pTable->asPointCloud());
	}

	if( pParameter->Cmp_Identifier("ENUM") )
	{
		pParameters->Set_Enabled("NAME", pParameter->asInt() < 0);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Enumerate::On_Execute(void)
{
	CSG_Table *pTable = Parameters("INPUT")->asTable();

	if( pTable->Get_Count() <= 0 )
	{
		Error_Set(_TL("no records in data set"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	TableName = pTable->Get_Name();
	bool		bSetName  = false;

	switch( pTable->Get_ObjectType() )
	{
	case SG_DATAOBJECT_TYPE_Table     : {
		CSG_Table *pResult = Parameters("RESULT_TABLE")->asTable();

		if( pResult && pResult != pTable )
		{
			pResult->Create(*pTable->asTable()); pTable = pResult;
			bSetName = true;
		}
		break; }

	case SG_DATAOBJECT_TYPE_Shapes    : {
		CSG_Shapes *pResult = Parameters("RESULT_SHAPES")->asShapes();

		if( pResult && pResult != pTable )
		{
			pResult->Create(*pTable->asShapes()); pTable = pResult;
			bSetName = true;
		}
		break; }

	case SG_DATAOBJECT_TYPE_PointCloud: {
		CSG_PointCloud *pResult = Parameters("RESULT_PC")->asPointCloud();

		if( pResult && pResult != pTable )
		{
			pResult->Create(*pTable->asPointCloud()); pTable = pResult;
			bSetName = true;
		}
		break; }
	}

	if( bSetName )
	{
		pTable->Fmt_Name("%s [%s]", TableName.c_str(), _TL("Enumerated"));
	}

	//-----------------------------------------------------
	bool bAscending = Parameters("ORDER")->asInt() != 1;

	int Field = Parameters("FIELD")->asInt();
	int Enum  = Parameters("ENUM" )->asInt();

	if( Enum < 0 )
	{
		Enum = pTable->Get_Field_Count();

		CSG_String Name = Parameters("NAME")->asString();

		if( Name.is_Empty() )
		{
			Name = "NR";
		}

		if( Field >= 0 )
		{
			Name += CSG_String("_") + pTable->Get_Field_Name(Field);
		}

		pTable->Add_Field(Name, SG_DATATYPE_Long);
	}

	//-----------------------------------------------------
	if( Field < 0 )
	{
		for(sLong i=0; i<pTable->Get_Count(); i++)
		{
			pTable->Get_Record_byIndex(i)->Set_Value(Enum, bAscending ? 1 + i : pTable->Get_Count() - i);
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Index Index;

		if( !pTable->Set_Index(Index, Field, bAscending) )
		{
			Error_Fmt("%s (%s)", _TL("failed to create index on field"), pTable->Get_Field_Name(Field));

			return( false );
		}

		CSG_String Value(pTable->Get_Record(Index[0])->asString(Field));

		for(sLong i=0, n=1; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
		{
			CSG_Table_Record *pRecord = pTable->Get_Record(Index[i]);

			if( Value.Cmp(pRecord->asString(Field)) )
			{
				Value = pRecord->asString(Field);

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
