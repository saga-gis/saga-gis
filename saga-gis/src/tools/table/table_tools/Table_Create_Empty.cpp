
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
#include "Table_Create_Empty.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_ID_NAME(i)	CSG_String::Format("NAME%d", i)
#define GET_ID_TYPE(i)	CSG_String::Format("TYPE%d", i)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Create_Empty::CTable_Create_Empty(void)
{
	Set_Name		(_TL("Create New Table"));

	Set_Author		("O.Conrad (c) 2005");

	CSG_String Types;

	for(int i=0; i<SG_DATATYPE_Undefined; i++)
	{
		if( (SG_DATATYPES_Table & SG_Data_Type_Get_Flag((TSG_Data_Type)i)) != 0 )
		{
			Types += "<li>" + SG_Data_Type_Get_Name((TSG_Data_Type)i) + "</li>";
		}
	}

	Set_Description	(CSG_String::Format(_TW(
		"Creates a new empty table.\n\nPossible field types are:\n<ul>%s</ul>"), Types.c_str()
	));

	//-----------------------------------------------------
	Parameters.Add_Table_Output("",
		"TABLE"  , _TL("Table"),
		_TL("")
	);

	Parameters.Add_String("",
		"NAME"   , _TL("Name"),
		_TL(""),
		_TL("New Table")
	);

	//-----------------------------------------------------
	Parameters.Add_Int("",
		"NFIELDS", _TL("Number of Attributes"),
		_TL(""),
		2, 1, true
	);

	CSG_Parameters *pFields = Parameters.Add_Parameters("",
		"FIELDS" , _TL("Attributes"),
		_TL("")
	)->asParameters();

	Set_Field_Count(pFields, Parameters("NFIELDS")->asInt());

	(*pFields)(GET_ID_NAME(0))->Set_Value("ID");
	(*pFields)(GET_ID_TYPE(0))->Set_Value( 3  );

	(*pFields)(GET_ID_NAME(1))->Set_Value("Name");
	(*pFields)(GET_ID_TYPE(1))->Set_Value( 0  );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTable_Create_Empty::Set_Field_Count(CSG_Parameters *pFields, int nFields)
{
	if( pFields && nFields > 0 )
	{
		int nCurrent = pFields->Get_Count() / 2;

		if( nCurrent < nFields )
		{
			for(int i=nCurrent; i<nFields; i++)
			{
				CSG_String Name(CSG_String::Format("%s %d", _TL("Field"), i + 1));

				pFields->Add_String   (""            , GET_ID_NAME(i),      Name  , _TL("Name"), Name);
				pFields->Add_Data_Type(GET_ID_NAME(i), GET_ID_TYPE(i), _TL("Type"), _TL("Type"), SG_DATATYPES_Table);
			}
		}
		else if( nCurrent > nFields )
		{
			for(int i=nCurrent, j=2*nCurrent; i>nFields; i--)
			{
				pFields->Del_Parameter(--j);
				pFields->Del_Parameter(--j);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Create_Empty::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("NFIELDS") )
	{
		Set_Field_Count((*pParameters)("FIELDS")->asParameters(), pParameter->asInt());
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Create_Empty::On_Execute(void)
{
	CSG_Table *pTable = Parameters("TABLE")->asTable();

	if( !pTable )
	{
		Parameters("TABLE")->Set_Value(pTable = SG_Create_Table());
	}

	pTable->Set_Name(Parameters("NAME")->asString());

	CSG_Parameters &Fields = *Parameters("FIELDS")->asParameters();

	for(int i=0; i<Parameters("NFIELDS")->asInt(); i++)
	{
		pTable->Add_Field(
			Fields(GET_ID_NAME(i))->asString(),
			Fields(GET_ID_TYPE(i))->asDataType()->Get_Data_Type()
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Create_Copy::CTable_Create_Copy(void)
{
	Set_Name		(_TL("Copy Table"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"Creates a copy of a table."
	));

	Parameters.Add_Table("", "TABLE", _TL("Table"), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Table("", "COPY" , _TL("Copy" ), _TL(""), PARAMETER_OUTPUT);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Create_Copy::On_Execute(void)
{
	CSG_Table *pCopy = Parameters("COPY")->asTable();

	return( pCopy->Create(*Parameters("TABLE")->asTable()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
