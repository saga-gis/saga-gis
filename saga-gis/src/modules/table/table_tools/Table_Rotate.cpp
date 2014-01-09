/**********************************************************
 * Version $Id$
 *********************************************************/

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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	Set_Name		(_TL("Transpose Table"));

	Set_Description	(_TW(
		"Transposes a table, i.e. to swap rows and columns."
	));

	Parameters.Add_Table(
		NULL	, "INPUT"	, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "OUTPUT"	, _TL("Output"),
		_TL(""),
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
	TSG_Data_Type		FieldType;
	CSG_Table			*pInput, *pOutput;
	CSG_Table_Record	*pRec_In, *pRec_Out;

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
			pOutput	= SG_Create_Table();
		}

		pOutput->Destroy();
		pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), _TL("rotated")));

		FieldType	= SG_DATATYPE_String;

		//-------------------------------------------------
		pOutput->Add_Field(pInput->Get_Field_Name(0), SG_DATATYPE_String);

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

				if( FieldType == SG_DATATYPE_String )
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
