/*******************************************************************************
    AddCoordinates.cpp
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

#include "AddCoordinates.h"



CAddCoordinates::CAddCoordinates(void){

	
	Parameters.Set_Name(_TL("Add Coordinates to points"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya."));

	Parameters.Add_Shapes(NULL, 
						"POINTS", 
						_TL("Points"), 
						_TL(""), 
						PARAMETER_INPUT);
	
}//constructor


CAddCoordinates::~CAddCoordinates(void)
{}

bool CAddCoordinates::On_Execute(void){

	int i;
	int iXField, iYField;
//	double dDist;
	CSG_Shape *pShape;
	CSG_Shapes *pShapes = Parameters("POINTS")->asShapes();
	CSG_Table *pTable = &pShapes->Get_Table();

	pTable->Add_Field("X", TABLE_FIELDTYPE_Double);
	iXField = pTable->Get_Field_Count() - 1;
	pTable->Add_Field("Y", TABLE_FIELDTYPE_Double);
	iYField = pTable->Get_Field_Count() - 1;

	for(i = 0; i < pShapes->Get_Count(); i++){			
		pShape = pShapes->Get_Shape(i);					
		pTable->Get_Record(i)->Set_Value(iXField, pShape->Get_Point(0).x);
		pTable->Get_Record(i)->Set_Value(iYField, pShape->Get_Point(0).y);		
	}//for

	return true;

}//method
