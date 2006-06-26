
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
#include "Table_Rotate.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Rotate::CTable_Rotate(void)
{
	Set_Name(_TL("Rotate Table"));

	Set_Description(
		_TL("Rotate a table, i.e. swap rows and columns.\n")
	);

	Parameters.Add_Table(
		NULL	, "INPUT"	, _TL("Input"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "OUTPUT"	, _TL("Output"),
		"",
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
CTable_Rotate::~CTable_Rotate(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Rotate::On_Execute(void)
{
	bool				bCopy;
	int					x, y;
	TTable_FieldType	FieldType;
	CTable				*pInput, *pOutput;
	CTable_Record		*pRec_In, *pRec_Out;

	//-----------------------------------------------------
	pInput	= Parameters("INPUT")	->asTable();
	pOutput	= Parameters("OUTPUT")	->asTable();

	//-----------------------------------------------------
	if( pInput->Get_Field_Count() > 0 && pInput->Get_Record_Count() > 0 )
	{
		if( pInput != pOutput )
		{
			bCopy	= true;
		}
		else
		{
			bCopy	= false;
			pOutput	= API_Create_Table();
		}

		pOutput->Destroy();
		pOutput->Set_Name(CAPI_String::Format(_TL("%s [rotated]"), pInput->Get_Name()));

		FieldType	= TABLE_FIELDTYPE_String;

		//-------------------------------------------------
		pOutput->Add_Field(pInput->Get_Field_Name(0), TABLE_FIELDTYPE_String);

		for(y=0; y<pInput->Get_Record_Count(); y++)
		{
			pOutput->Add_Field(pInput->Get_Record(y)->asString(0), FieldType);
		}

		for(y=1; y<pInput->Get_Field_Count(); y++)
		{
			pRec_Out	= pOutput->Add_Record();
			pRec_Out->Set_Value(0, pInput->Get_Field_Name(y));

			for(x=0; x<pInput->Get_Record_Count(); x++)
			{
				pRec_In		= pInput->Get_Record(x);

				if( FieldType == TABLE_FIELDTYPE_String )
				{
					pRec_Out->Set_Value(x + 1, pRec_In->asString(y));
				}
				else
				{
					pRec_Out->Set_Value(x + 1, pRec_In->asDouble(y));
				}
			}
		}

		//-------------------------------------------------
		if( !bCopy )
		{
			pInput->Assign(pOutput);
			delete(pOutput);
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
