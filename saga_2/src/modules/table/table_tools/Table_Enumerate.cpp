
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

	Set_Name(_TL("Enumerate a Table Attribute"));

	Set_Description(
		_TL("Enumerate attribute of a table, i.e. assign to identical values of choosen attribute field unique identifiers.\n")
	);

	pNode	= Parameters.Add_Table(
		NULL	, "INPUT"	, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"	, _TL("Attribute"),
		_TL("")
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
	int					iField, iField_ID, iID, iRecord, old_Field;
	TSG_Table_Index_Order	old_Order;
	CSG_String			Value;
	CSG_Table				*pTable, *pOutput;
	CSG_Table_Record		*pRecord;

	//-----------------------------------------------------
	pTable	= Parameters("INPUT")	->asTable();
	pOutput	= Parameters("OUTPUT")	->asTable();
	iField	= Parameters("FIELD")	->asInt();

	//-----------------------------------------------------
	if( iField >= 0 && iField < pTable->Get_Field_Count() && pTable->Get_Record_Count() > 0 )
	{
		if( pOutput != NULL && pOutput != pTable )
		{
			pOutput->Create		(*pTable);
			pOutput->Set_Name	( pTable->Get_Name());
			pTable	= pOutput;
		}

		//-------------------------------------------------
		pTable->Add_Field(_TL("ENUM_ID"), TABLE_FIELDTYPE_Int);
		iField_ID	= pTable->Get_Field_Count() - 1;

		old_Order	= pTable->Get_Index_Order(0);
		old_Field	= pTable->Get_Index_Field(0);

		pTable->Set_Index(iField, TABLE_INDEX_Down);
		Value		= pTable->Get_Record_byIndex(0)->asString(iField);

		for(iRecord=0, iID=1; iRecord<pTable->Get_Record_Count(); iRecord++)
		{
			pRecord	= pTable->Get_Record_byIndex(iRecord);

			if( Value.Cmp(pRecord->asString(iField)) )
			{
				Value	= pRecord->asString(iField);
				iID++;
			}

			pRecord->Set_Value(iField_ID, iID);
		}

		pTable->Set_Index(old_Field, old_Order);

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
