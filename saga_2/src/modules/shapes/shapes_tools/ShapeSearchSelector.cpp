/*******************************************************************************
    ShapeSearchSelector.cpp
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

#include "ShapeSearchSelector.h"

CShapeSearchSelector::CShapeSearchSelector(CShapes *pShapes,
										   CSG_String sExpression){

	int i,j;
	CTable_Record *pRecord;
	CSG_String sValue;
	int iFields;

	iFields = pShapes->Get_Table().Get_Field_Count();

	for (i = 0 ; i < pShapes->Get_Count(); i++){
		pRecord = pShapes->Get_Shape(i)->Get_Record();
		for (j = 0; j < iFields; j++){
			sValue = pRecord->asString(j);
			if (sValue.Find(sExpression) != -1){
				m_pSelectedRecords.push_back(i);
				break;
			}//if
		}//for
	}//for

}//constructor

CShapeSearchSelector::~CShapeSearchSelector(){
}//destructor


int& CShapeSearchSelector::GetSelectedRecords(){

	return m_pSelectedRecords[0];

}//method

int CShapeSearchSelector::GetSelectedRecordsCount(){

	return m_pSelectedRecords.size();

}//method

