/*******************************************************************************
    DistanceMatrix.cpp
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

#include "DistanceMatrix.h"
#include <vector>


CDistanceMatrix::CDistanceMatrix(void){

	
	Parameters.Set_Name(_TL("Distance Matrix"));
	Parameters.Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Computes distance between pairs of points in a layer."));

	Parameters.Add_Shapes(NULL, 
						"POINTS", 
						_TL("Points"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Table(NULL, 
						"TABLE", 
						_TL("Distance Matrix Table"),
						"",
						PARAMETER_OUTPUT);
	
}//constructor


CDistanceMatrix::~CDistanceMatrix(void)
{}

bool CDistanceMatrix::On_Execute(void){

	int i,j,k;	
	double dDist;
	CAPI_dPoints	Points;
	CShapes *pShapes = Parameters("POINTS")->asShapes();
	CTable *pTable = Parameters("TABLE")->asTable();
	CShape *pShape;
	CTable_Record *pRecord;

	pTable->Create((CTable*)NULL);
	pTable->Set_Name(_TL("Distance Matrix"));

	for(i = 0; i < pShapes->Get_Count(); i++){			
		pShape = pShapes->Get_Shape(i);					
		for(j = 0; j < pShape->Get_Part_Count(); j++){	
			for(k = 0; k < pShape->Get_Point_Count(j); k++){
				Points.Add(pShape->Get_Point(k, j).x, pShape->Get_Point(k, j).y);
			}//for
		}//for
	}//for
	for (i = 0; i < Points.Get_Count(); i++){
		pTable->Add_Field(API_Get_String(i).c_str(), TABLE_FIELDTYPE_Double);
	}//for
	for (i = 0; i < Points.Get_Count(); i++){
		pTable->Add_Record();
	}//for
	for (i = 0; i < Points.Get_Count(); i++){
		pRecord = pTable->Get_Record(i);
		for (j = i; j < Points.Get_Count(); j++){
			dDist = sqrt(pow(Points[i].x-Points[j].x,2) + pow(Points[i].y-Points[j].y,2));
			pRecord->Set_Value(j, dDist);
			pTable->Get_Record(j)->Set_Value(i, dDist);
		}//for
	}//for

	return true;

}//method
