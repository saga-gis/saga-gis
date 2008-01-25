/*******************************************************************************
    TableCalculatorShapes.cpp
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
#include "Table_CalculatorShapes.h"

CTableCalculatorShapes::CTableCalculatorShapes(void){

	Parameters.Set_Name(_TL("Table calculator for shapes"));

	Parameters.Set_Description(_TW("Calculation of new values in attributes tables."));

	Parameters.Add_Shapes(NULL,
						"SHAPES",
						_TL("Shapes"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_String(NULL, "FORMULA", _TL("Formula"), _TL(""), SG_T("a+b"));

	Parameters.Add_String(NULL, "NAME", _TL("Field Name"), _TL(""), SG_T("a+b"));


}//constructor


CTableCalculatorShapes::~CTableCalculatorShapes(void){}

bool CTableCalculatorShapes::On_Execute(void){

	int iFields;
	int i,j;
	double dValue;
	double *pFieldValues;
	CSG_Shapes* pShapes;
	CSG_Table *pTable;
	CSG_Table_Record *pRecord;
	const SG_Char *pFormula;
	CSG_Formula Formula;

	pShapes = Parameters("SHAPES")->asShapes();
	pTable	= &pShapes->Get_Table();
	iFields = pTable->Get_Field_Count();

	pTable->Add_Field(Parameters("NAME")->asString(), TABLE_FIELDTYPE_Double);

	pFormula = Parameters("FORMULA")->asString();

	Formula.Set_Formula(pFormula);

	int Pos;
	CSG_String Msg;
	if (Formula.Get_Error(&Pos, &Msg)){
		CSG_String msg;
		msg.Printf(_TL("Syntax error at position #%d: \n%s\n"), Pos, pFormula);

		Message_Add(msg);

		msg.Printf(SG_T("\n%s\n"), Msg);

		Message_Add(msg);

		return false;
	}//if

	pFieldValues= new double[iFields];

	for (i = 0; i < pTable->Get_Record_Count(); i++){

		pRecord = pTable->Get_Record(i);

		for (j = 0; j < iFields; j++){
			pFieldValues[j] = pRecord->asDouble(j);
		}//for

		dValue = Formula.Get_Value(pFieldValues, iFields);

		pRecord->Set_Value(iFields, dValue);

	}//for

	delete[] pFieldValues;
	return (true);

	//DataObject_Update(pShapes);

}//method
