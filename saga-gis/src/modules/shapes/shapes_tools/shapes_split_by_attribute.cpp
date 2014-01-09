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
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              shapes_split_by_attribute.cpp            //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes_split_by_attribute.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Split_by_Attribute::CShapes_Split_by_Attribute(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Split Table/Shapes by Attribute"));

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table / Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL(""),
		false
	);

	Parameters.Add_Table_List(
		NULL	, "CUTS"		, _TL("Cuts"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Split_by_Attribute::On_Execute(void)
{
	int			iField;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable	= Parameters("TABLE")	->asTable();
	iField	= Parameters("FIELD")	->asInt();

	Parameters("CUTS")->asTableList()->Del_Items();

	//-----------------------------------------------------
	if( pTable->is_Valid() && pTable->Set_Index(iField, TABLE_INDEX_Ascending) )
	{
		CSG_String	sValue;
		CSG_Table	*pCut	= NULL;

		for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record_byIndex(iRecord);

			if( !pCut || sValue.Cmp(pRecord->asString(iField)) )
			{
				pCut	= pTable->Get_ObjectType() == DATAOBJECT_TYPE_Shapes
						? SG_Create_Shapes(((CSG_Shapes *)pTable)->Get_Type(), SG_T(""), pTable)
						: SG_Create_Table(pTable);

				pCut->Set_Name(CSG_String::Format(SG_T("%s [%s = %s]"),
					pTable->Get_Name(),
					pTable->Get_Field_Name(iField),
					pRecord->asString(iField)
				));

				Parameters("CUTS")->asTableList()->Add_Item(pCut);

				sValue	= pRecord->asString(iField);
			}

			pCut->Add_Record(pRecord);
		}

		return( pCut != NULL );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
