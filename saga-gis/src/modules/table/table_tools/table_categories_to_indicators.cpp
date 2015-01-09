/**********************************************************
 * Version $Id: table_categories_to_indicators.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     table_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//           table_categories_to_indicators.cpp          //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
//                University of Hamburg                  //
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
#include "table_categories_to_indicators.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Categories_to_Indicators::CTable_Categories_to_Indicators(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("Add Indicator Fields for Categories"));

	Set_Author	("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Adds for each unique value found in the category field "
		"an indicator field that will show a value of one (1) "
		"for all records with this category value and zero (0) "
		"for all others. This might be used e.g. for subsequent "
		"indicator kriging. "
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL("Input table or shapefile"),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Categories"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "OUT_TABLE"	, _TL("Output table with field(s) deleted"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "OUT_SHAPES"	, _TL("Output shapes with field(s) deleted"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Categories_to_Indicators::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TABLE") )
	{
		CSG_Data_Object	*pObject	= pParameter->asDataObject();

		pParameters->Get_Parameter("OUT_TABLE" )->Set_Enabled(pObject &&
			pObject->Get_ObjectType() == DATAOBJECT_TYPE_Table
		);

		pParameters->Get_Parameter("OUT_SHAPES")->Set_Enabled(pObject &&
			pObject->Get_ObjectType() == DATAOBJECT_TYPE_Shapes
		);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Categories_to_Indicators::On_Execute(void)
{
	//-----------------------------------------------------
	int		iCategory	= Parameters("FIELD")->asInt();

	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	//-----------------------------------------------------
	if( pTable->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
	{
		if( Parameters("OUT_SHAPES")->asShapes() != NULL && Parameters("OUT_SHAPES")->asShapes() != pTable )
		{
			CSG_Shapes	*pOutput	= Parameters("OUT_SHAPES")->asShapes();

			pOutput->Create(((CSG_Shapes *)pTable)->Get_Type(), NULL, (CSG_Table *)0, ((CSG_Shapes *)pTable)->Get_Vertex_Type());
			pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pTable->Get_Name(), pTable->Get_Field_Name(iCategory)));
			pOutput->Add_Field(pTable->Get_Field_Name(iCategory), pTable->Get_Field_Type(iCategory));

			for(int i=0; i<pTable->Get_Count(); i++)
			{
				CSG_Table_Record	*pRecord	= pOutput->Add_Shape((CSG_Shape *)pTable->Get_Record(i), SHAPE_COPY_GEOM);

				*(pRecord->Get_Value(0))	= *(pTable->Get_Record(i)->Get_Value(iCategory));
			}

			pTable		= pOutput;
			iCategory	= 0;
		}
	}

	//-----------------------------------------------------
	else // if( pTable->Get_ObjectType() == DATAOBJECT_TYPE_Table )
	{
		if( Parameters("OUT_TABLE"  )->asTable() != NULL && Parameters("OUT_TABLE"  )->asTable() != pTable )
		{
			CSG_Table	*pOutput	= Parameters("OUT_SHAPES")->asTable();

			pOutput->Destroy();
			pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pTable->Get_Name(), pTable->Get_Field_Name(iCategory)));
			pOutput->Add_Field(pTable->Get_Field_Name(iCategory), pTable->Get_Field_Type(iCategory));

			for(int i=0; i<pTable->Get_Count(); i++)
			{
				CSG_Table_Record	*pRecord	= pOutput->Add_Record();

				*(pRecord->Get_Value(0))	= *(pTable->Get_Record(i)->Get_Value(iCategory));
			}

			pTable		= pOutput;
			iCategory	= 0;
		}
	}

	//-----------------------------------------------------
	TSG_Table_Index_Order	old_Order	= pTable->Get_Index_Order(0);
	int						old_Field	= pTable->Get_Index_Field(0);

	pTable->Set_Index(iCategory, TABLE_INDEX_Descending);

	int		nCategories	= 0;

	CSG_String	Value;

	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record_byIndex(iRecord);

		if( iRecord == 0 || Value.Cmp(pRecord->asString(iCategory)) )
		{
			Value	= pRecord->asString(iCategory);

			pTable->Add_Field(Value, SG_DATATYPE_Int);

			nCategories++;
		}

		pRecord->Set_Value(pTable->Get_Field_Count() - 1, 1.0);
	}

	pTable->Set_Index(old_Field, old_Order);

	Message_Add(CSG_String::Format("\n%s: %d\n", _TL("number of categories"), nCategories), false);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
