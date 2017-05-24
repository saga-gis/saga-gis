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
CTable_Enumerate::CTable_Enumerate(void)
{
	CSG_Parameter	*pNode;

	Set_Name(_TL("Enumerate Table Field"));

	Set_Description(_TW(
		"Enumerate attribute of a table, i.e. assign to identical values of chosen attribute field unique identifiers."
	));

	pNode	= Parameters.Add_Table(
		NULL	, "INPUT"	, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"	, _TL("Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Table(
		NULL	, "OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}

//---------------------------------------------------------
CTable_Enumerate::~CTable_Enumerate(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Enumerate::On_Execute(void)
{
	int			iField, iField_ID;
	CSG_Table	*pTable, *pOutput;

	//-----------------------------------------------------
	pTable	= Parameters("INPUT")	->asTable();
	pOutput	= Parameters("OUTPUT")	->asTable();
	iField	= Parameters("FIELD")	->asInt();

	//-----------------------------------------------------
	if( pTable->Get_Record_Count() <= 0 )
	{
		Error_Set(_TL("no records in data set"));

		return( false );
	}

	//-----------------------------------------------------
	if( pOutput != NULL && pOutput != pTable )
	{
		pOutput->Create		(*pTable);
		pOutput->Set_Name	( pTable->Get_Name());
		pTable	= pOutput;
	}

	pTable->Add_Field(_TL("ENUM_ID"), SG_DATATYPE_Int);
	iField_ID	= pTable->Get_Field_Count() - 1;

	//-----------------------------------------------------
	if( iField >= 0 && iField < pTable->Get_Field_Count() )
	{
		TSG_Table_Index_Order	old_Order	= pTable->Get_Index_Order(0);
		int						old_Field	= pTable->Get_Index_Field(0);

		pTable->Set_Index(iField, TABLE_INDEX_Descending);

		CSG_String	Value	= pTable->Get_Record_byIndex(0)->asString(iField);

		for(int iRecord=0, iID=1; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record_byIndex(iRecord);

			if( Value.Cmp(pRecord->asString(iField)) )
			{
				Value	= pRecord->asString(iField);

				iID++;
			}

			pRecord->Set_Value(iField_ID, iID);
		}

		pTable->Set_Index(old_Field, old_Order);
	}
	else
	{
		for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

			pRecord->Set_Value(iField_ID, iRecord + 1);
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
