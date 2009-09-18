/*******************************************************************************
    GridsFromTableAndGrid.cpp
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

#include "GridsFromTableAndGrid.h"
#include <string.h>

CGridsFromTableAndGrid::CGridsFromTableAndGrid(){

	CSG_Parameter	*pNode_0, *pNode_1;

	Parameters.Set_Name(_TL("Grids from classified grid and table"));

	Parameters.Set_Description(_TW("Creates several grids using a classified grid and a table with data values for each class."));

	pNode_0	= Parameters.Add_Table(	NULL, 
									"TABLE", 
									_TL("Table"),
									_TL(""),	
									PARAMETER_INPUT);

	pNode_1	= Parameters.Add_Table_Field(pNode_0, 
									"TABLE_ID", 
									_TL("Field"),
									_TL(""));
	
	Parameters.Add_Grid(NULL, 
						"CLASSES",
						_TL("Classified Grid"), 						
						_TL(""), 
						PARAMETER_INPUT);
	
}//constructor

CGridsFromTableAndGrid::~CGridsFromTableAndGrid(void){}

bool CGridsFromTableAndGrid::On_Execute(void){

	int i,j;
	int x,y;
	CSG_Grid* pClassifiedGrid = Parameters("CLASSES")->asGrid();
	CSG_Table* pTable = Parameters("TABLE")->asTable();
	int iField = Parameters("TABLE_ID")->asInt();
	CSG_Table_Record* pRecord;
	int iMax;
	int iValue;
	float **pValues;
	int *pValidFields;
	int iValidFields = 0;
	CSG_Grid **pGrid;

	if (pTable->Get_Record_Count()){
		pRecord = pTable->Get_Record(0);
		iMax = pRecord->asInt(iField);
		for (i = 0; i < pTable->Get_Record_Count(); i++){
			pRecord = pTable->Get_Record(i);
			iValue = pRecord->asInt(iField);
			if (iValue > iMax){
				iMax = iValue;
			}//if				
		}//for
		pValues = new float*[iMax+1];
		for (i = 0; i < pTable->Get_Field_Count(); i++){
			if (pTable->Get_Field_Type(i) == SG_DATATYPE_String || i == iField){
			}//if
			else{
				iValidFields++;
			}//else			
		}//for
		pValidFields = new int[iValidFields];
		iValidFields = 0;
		for (i = 0; i < pTable->Get_Field_Count(); i++){
			if (pTable->Get_Field_Type(i) == SG_DATATYPE_String || i == iField){
			}//if
			else{
				pValidFields[iValidFields] = i;
				iValidFields++;
			}//else			
		}//for
		for (i = 0; i < iMax+1; i++){
			pValues[i] = new float[iValidFields];				
		}//for
		for (i = 0; i < pTable->Get_Record_Count(); i++){
			pRecord = pTable->Get_Record(i);
			iValue = pRecord->asInt(iField);
			for (j = 0; j < iValidFields; j++){
				pValues[iValue][j] = pRecord->asFloat(pValidFields[j]);
			}//for			
		}//for

		pGrid = new CSG_Grid*[iValidFields];
		for (i = 0; i < iValidFields; i++){
			pGrid[i] = new CSG_Grid(pClassifiedGrid);
		}//if

		for(y=0; y<Get_NY() && Set_Progress(y); y++){		
			for(x=0; x<Get_NX(); x++){	
				iValue = pClassifiedGrid->asInt(x,y);
				if (iValue > 0 && iValue < iMax + 1){
					for (i = 0; i < iValidFields; i++){
						pGrid[i]->Set_Value(x, y, pValues[iValue][i]);
					}//for
				}//if
			}//for
		}//for		
	
		for (i = 0; i < iValidFields; i++){
			DataObject_Add(pGrid[i], true);
		}//for
	}//if
	else{
		Message_Add(_TL("Selected table contains no valid records"));
	}//else

	return true;


}//method