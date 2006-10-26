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
	Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Merge Shapes Layers"));

	Parameters.Add_Shapes(NULL, 
						"OUT", 
						_TL("Merged Layer"), 
						"", 
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
	TSG_Point	Point;
	CSG_Shape *pShape, *pShape2;
	CSG_Parameter_Shapes_List	*pShapesList;
	CSG_Shapes* pMainShapes;
	int i,j,k;
	int iLayer, iField;
	int iFieldMain, iFieldAdditional;
	std::vector<int> FieldsMain, FieldsAdditional;
	CSG_String sName1,sName2;
	
	pOutput = Parameters("OUT")->asShapes();
	pMainShapes = Parameters("MAIN")->asShapes();
	TSG_Shape_Type MainType = pMainShapes->Get_Type();

	pOutput->Create(MainType, _TL("Shapes_Merge"));

	pTable=&pMainShapes->Get_Table();
	for (i=0; i<pTable->Get_Field_Count(); i++){
		pOutput->Get_Table().Add_Field(pTable->Get_Field_Name(i), pTable->Get_Field_Type(i));	
	}//for

	
	//copy main layer into destination
	for(i=0; i<pMainShapes->Get_Count(); i++){			
		pShape = pMainShapes->Get_Shape(i);					
		pShape2 = pOutput->Add_Shape();	
		for(int j=0; j<pShape->Get_Part_Count(); j++){	
			for(int k=0; k<pShape->Get_Point_Count(j)-1; k++){
				Point = pShape->Get_Point(k,j);	
				pShape2->Add_Point(Point.x,Point.y,j);
				for	(iField = 0; iField<pTable->Get_Field_Count(); iField++){
					if (pTable->Get_Field_Type(iField) == TABLE_FIELDTYPE_String){
						const char *cValue = pShape->Get_Record()->asString(iField);
						pShape2->Get_Record()->Set_Value(iField, cValue);
					}//if
					else{
						double dValue = pShape->Get_Record()->asDouble(iField);
						pShape2->Get_Record()->Set_Value(iField, dValue);
					}//else
				}//for
			}//for
		}//for
	}//for

	//now copy the additional layers	
	if( (pShapesList = Parameters("LAYERS")->asShapesList()) != 
			NULL && pShapesList->Get_Count() > 0 ){
		for (iLayer=0; iLayer<pShapesList->Get_Count(); iLayer++){
			if( (pShapes = pShapesList->asShapes(iLayer)) != NULL ){
				if (pShapes->Get_Type() == MainType){
					FieldsMain.clear();
					FieldsAdditional.clear();
					//see which fields are in both attributes tables
					for (i=0; i<pTable->Get_Field_Count(); i++){
						for (j=0; j<pShapes->Get_Table().Get_Field_Count(); j++){
							sName1 = pShapes->Get_Table().Get_Field_Name(j);
							sName2 = pTable->Get_Field_Name(i);
							if (!sName1.CmpNoCase(sName2)){
								FieldsMain.push_back(i);
								FieldsAdditional.push_back(j);
								continue;
							}//if
						}//for
					}//for
					//copy shapes and attributes
					for(i=0; i<pShapes->Get_Count(); i++){			
						pShape = pShapes->Get_Shape(i);					
						pShape2 = pOutput->Add_Shape();
						for (j=0; j<FieldsMain.size(); j++){
							iFieldMain = FieldsMain.at(j);
							iFieldAdditional = FieldsAdditional.at(j);
							if (pTable->Get_Field_Type(iFieldMain) == TABLE_FIELDTYPE_String){
								const char* cValue = pShape->Get_Record()->asString(iFieldAdditional);
								pShape2->Get_Record()->Set_Value(iFieldMain, cValue);
							}//if
							else{
								double dValue = pShape->Get_Record()->asDouble(iFieldAdditional);
								pShape2->Get_Record()->Set_Value(iFieldMain, dValue);
							}//else
						}//for
						for(j=0; j<pShape->Get_Part_Count(); j++){	
							for(k=0; k<pShape->Get_Point_Count(j)-1; k++){
								Point = pShape->Get_Point(k,j);	
								pShape2->Add_Point(Point.x,Point.y,j);			
							}//for
						}//for
					}//for
				}//if
			}//if
		}//for
	}//if

	return true;

}//method
