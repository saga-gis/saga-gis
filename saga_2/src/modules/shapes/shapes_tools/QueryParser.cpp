/*******************************************************************************
    QueryParser.cpp
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

#include "QueryParser.h"

CQueryParser::CQueryParser(CSG_Shapes *pShapes, CSG_String sExpression){

	CSG_Formula Formula;
	int i,j;
	int iFields;
	CSG_Table *pTable;
	CSG_Table_Record *pRecord;
	double *pFieldValues;
	double dValue;

	pTable = &pShapes->Get_Table();
	iFields = pTable->Get_Field_Count();
	Formula.Set_Formula(sExpression.c_str());
	pFieldValues= new double[iFields];

	for (i = 0; i < pTable->Get_Record_Count(); i++){
		pRecord = pTable->Get_Record(i);
		for (j = 0; j < iFields; j++){
			pFieldValues[j] = pRecord->asDouble(j);
		}//for
		dValue = Formula.Val(pFieldValues, iFields);
		if (dValue){
			m_pSelectedRecords.push_back(i);
		}//if
	}//for

}//constructor

CQueryParser::~CQueryParser(){

	//delete [] m_pSelectedFields;

}//destructor

int& CQueryParser::GetSelectedRecords(){

	return m_pSelectedRecords[0];

}//method

int CQueryParser::GetSelectedRecordsCount(){

	return m_pSelectedRecords.size();
	//m_iSelectedFieldsCount;

}//method
