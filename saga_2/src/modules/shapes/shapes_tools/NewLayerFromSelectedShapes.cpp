/*******************************************************************************
    NewLayerFromSelectedShapes.cpp
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
#include "NewLayerFromSelectedShapes.h"
#include <string>

CNewLayerFromSelectedShapes::CNewLayerFromSelectedShapes(void){

	
	Parameters.Set_Name(_TL("New layer from selected shapes"));
	Parameters.Set_Description(_TL(
		"(c) 2004 by Victor Olaya. New layer from selected shapes"));

	Parameters.Add_Shapes(NULL, 
						"INPUT", 
						_TL("Input"), 
						"", 
						PARAMETER_INPUT);

	/*Parameters.Add_Shapes(NULL, 
						"OUTPUT", 
						"Output", 
						"", 
						PARAMETER_OUTPUT);*/
	
}//constructor


CNewLayerFromSelectedShapes::~CNewLayerFromSelectedShapes(void)
{}

bool CNewLayerFromSelectedShapes::On_Execute(void){

	int i;
	CShapes *pInput = Parameters("INPUT")->asShapes();
	CShapes *pOutput; //= Parameters("OUTPUT")->asShapes();
	CTable *pTable;
	CAPI_String sName;

	pTable	= &pInput->Get_Table();
	pOutput = API_Create_Shapes(pInput->Get_Type());
	
	sName = CAPI_String(pInput->Get_Name()) + _TL("(Selected)");
	pOutput->Set_Name(sName.c_str());

	for (i=0; i<pTable->Get_Field_Count(); i++){
		pOutput->Get_Table().Add_Field(pTable->Get_Field_Name(i), pTable->Get_Field_Type(i));	
	}//for

	for (i = 0; i < pInput->Get_Selection_Count(); i++){
		pOutput->Add_Shape()->Assign(pInput->Get_Selection(i));
	}//for

	DataObject_Add(pOutput, true);

	return true;

}//method