
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	Set_Name		(_TL("Split Shapes Layer by Attribute"));

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL(""),
		false
	);

	Parameters.Add_Shapes_List(
		NULL	, "CUTS"		, _TL("Cuts"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}

//---------------------------------------------------------
CShapes_Split_by_Attribute::~CShapes_Split_by_Attribute(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Split_by_Attribute::On_Execute(void)
{
	int			iField;
	CSG_Shapes	*pShapes;

	//-----------------------------------------------------
	pShapes	= Parameters("SHAPES")	->asShapes();
	iField	= Parameters("FIELD")	->asInt();

	Parameters("CUTS")->asShapesList()->Del_Items();

	//-----------------------------------------------------
	if( pShapes->is_Valid() && pShapes->Set_Index(iField, TABLE_INDEX_Ascending) )
	{
		CSG_String	sValue;
		CSG_Shapes	*pCut	= NULL;

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			if( !pCut || sValue.Cmp(pShape->asString(iField)) )
			{
				pCut	= SG_Create_Shapes(
					pShapes->Get_Type(),
					CSG_String::Format(SG_T("%s [%s = %s]"),
						pShapes->Get_Name(),
						pShapes->Get_Table().Get_Field_Name(iField),
						pShape->asString(iField)
					),
					&pShapes->Get_Table()
				);

				Parameters("CUTS")->asShapesList()->Add_Item(pCut);

				sValue	= pShape->asString(iField);
			}

			pCut->Add_Shape(pShape, true);
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
