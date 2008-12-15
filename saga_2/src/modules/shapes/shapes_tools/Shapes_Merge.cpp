/*******************************************************************************
    Shapes_Merge.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 

#include "Shapes_Merge.h"
#include <string>
#include <vector>

CShapes_Merge::CShapes_Merge(void){

	Set_Name(_TL("Merge Shapes Layers"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Merge Shapes Layers"));

	Parameters.Add_Shapes(NULL, 
						"OUT", 
						_TL("Merged Layer"), 
						_TL(""), 
						PARAMETER_OUTPUT);

	Parameters.Add_Shapes(NULL, 
						"MAIN", 
						_TL("Main Layer"),
						_TL("Main Layer. Output layer will have the same field in the attributes table as this layer"),
						PARAMETER_INPUT);

	Parameters.Add_Shapes_List(NULL,
						"LAYERS", 
						_TL("Additional Layers"),
						_TL("Layers to merge with main layer"),
						PARAMETER_INPUT_OPTIONAL);

}//constructor


CShapes_Merge::~CShapes_Merge(void)
{}

bool CShapes_Merge::On_Execute(void){

	CSG_Table *pTable;
	CSG_Shapes *pShapes, *pOutput;
	CSG_Shape *pShape, *pShape2;
	CSG_Parameter_Shapes_List	*pShapesList;
	CSG_Shapes* pMainShapes;
	int i,j;
	int iLayer, iField, nFields;
	int iFieldMain, iFieldAdditional;
	std::vector<int> FieldsMain, FieldsAdditional;
	CSG_String sName1,sName2;
	
	pOutput = Parameters("OUT")->asShapes();
	pMainShapes = Parameters("MAIN")->asShapes();
	TSG_Shape_Type MainType = pMainShapes->Get_Type();

	pOutput->Create(MainType, _TL("Shapes_Merge"));

	pTable	= pMainShapes;
	nFields	= pTable->Get_Field_Count();

	for (i=0; i<nFields; i++)
	{
		pOutput->Add_Field(pTable->Get_Field_Name(i), pTable->Get_Field_Type(i));
	}//for

	pOutput->Add_Field(SG_T("LAYER_ID")	, TABLE_FIELDTYPE_Int);
	pOutput->Add_Field(SG_T("LAYER")	, TABLE_FIELDTYPE_String);
	
	//copy main layer into destination
	for(i=0; i<pMainShapes->Get_Count(); i++)
	{
		pShape	= pMainShapes->Get_Shape(i);
		pShape2	= pOutput->Add_Shape(pShape);
		pShape2	->Set_Value(nFields + 0, 1);
		pShape2	->Set_Value(nFields + 1, pMainShapes->Get_Name());

		for	(iField = 0; iField<nFields; iField++)
		{
			if (pTable->Get_Field_Type(iField) == TABLE_FIELDTYPE_String)
			{
				pShape2->Set_Value(iField, pShape->asString(iField));
			}//if
			else
			{
				pShape2->Set_Value(iField, pShape->asDouble(iField));
			}//else
		}//for
	}//for

	//now copy the additional layers	
	if( (pShapesList = Parameters("LAYERS")->asShapesList()) != NULL && pShapesList->Get_Count() > 0 )
	{
		for (iLayer=0; iLayer<pShapesList->Get_Count(); iLayer++)
		{
			if( (pShapes = pShapesList->asShapes(iLayer)) != NULL )
			{
				if (pShapes->Get_Type() == MainType)
				{
					FieldsMain.clear();
					FieldsAdditional.clear();

					//see which fields are in both attributes tables
					for (i=0; i<nFields; i++)
					{
						for (j=0; j<pShapes->Get_Field_Count(); j++)
						{
							sName1 = pShapes->Get_Field_Name(j);
							sName2 = pTable->Get_Field_Name(i);

							if (!sName1.CmpNoCase(sName2))
							{
								FieldsMain.push_back(i);
								FieldsAdditional.push_back(j);
								continue;
							}//if
						}//for
					}//for

					//copy shapes and attributes
					for(i=0; i<pShapes->Get_Count(); i++)
					{
						pShape	= pShapes->Get_Shape(i);					
						pShape2 = pOutput->Add_Shape(pShape);
						pShape2	->Set_Value(nFields + 0, 2 + iLayer);
						pShape2	->Set_Value(nFields + 1, pShapes->Get_Name());

						for (j=0; j<FieldsMain.size(); j++)
						{
							iFieldMain			= FieldsMain.at(j);
							iFieldAdditional	= FieldsAdditional.at(j);

							if (pTable->Get_Field_Type(iFieldMain) == TABLE_FIELDTYPE_String)
							{
								pShape2->Set_Value(iFieldMain, pShape->asString(iFieldAdditional));
							}//if
							else
							{
								pShape2->Set_Value(iFieldMain, pShape->asDouble(iFieldAdditional));
							}//else
						}//for
					}//for
				}//if
			}//if
		}//for
	}//if

	return true;

}//method
