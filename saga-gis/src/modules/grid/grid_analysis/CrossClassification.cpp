/*******************************************************************************
    CrossClassification.cpp
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
#include "CrossClassification.h"

CCrossClassification::CCrossClassification(void){

	Parameters.Set_Name(_TL("Cross-Classification and Tabulation"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya. Cross-Classification and Tabulation"));

	Parameters.Add_Grid(NULL,
						"INPUT",
						_TL("Input Grid 1"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL,
						"INPUT2",
						_TL("Input Grid 2"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL,
						"RESULTGRID",
						_TL("Cross-Classification Grid"),
						_TL(""),
						PARAMETER_OUTPUT);

	Parameters.Add_Table(NULL,
						"RESULTTABLE",
						_TL("Cross-Tabulation Table"),
						_TL(""),
						PARAMETER_OUTPUT);

	Parameters.Add_Value(NULL,
						"MAXNUMCLASS",
						_TL("Max. Number of Classes"),
						_TL("Maximum number of classes in the entire grid."),
						PARAMETER_TYPE_Int,
						10);

}//constructor

CCrossClassification::~CCrossClassification(void){}

bool CCrossClassification::On_Execute(void){

	CSG_Grid* pInput = Parameters("INPUT")->asGrid();
	CSG_Grid* pInput2 = Parameters("INPUT2")->asGrid();
	CSG_Grid* pOutput = Parameters("RESULTGRID")->asGrid();
	CSG_Table* pTable = Parameters("RESULTTABLE")->asTable();
	CSG_Table_Record* pRecord;
	int iNumClasses = Parameters("MAXNUMCLASS")->asInt();
	int iClass, iClass2;
	int iNewClass;
	int i,j;
	int x,y;
	int **pTabulation = new int* [iNumClasses];	
	int *pTotal, iTotal;

	pTable->Create((CSG_Table*)NULL);
	pTable->Set_Name(_TL("Cross-Tabulation"));

	for (i = 0; i < iNumClasses; i++){
		pTable->Add_Field(SG_Get_String(i+1,0).c_str(), SG_DATATYPE_Int);
		pTabulation[i] = new int[iNumClasses];
		for(j=0; j < iNumClasses; j++){
			pTabulation[i][j] = 0;
		}//for
	}//for
	pTable->Add_Field(_TL("Total"), SG_DATATYPE_Int);
	
	for(y=0; y<Get_NY() && Set_Progress(y); y++){
		for(x=0; x<Get_NX(); x++){
			if (!pInput->is_NoData(x,y) && !pInput2->is_NoData(x,y)){
				iClass = pInput->asInt(x,y)-1;
				iClass2 = pInput2->asInt(x,y)-1;
				if (iClass < iNumClasses && iClass >= 0 
						&& iClass2 < iNumClasses && iClass2 >= 0){					
					pTabulation[iClass][iClass2]++;					
				}//if
				iNewClass = iClass * iNumClasses + iClass2;
				pOutput->Set_Value(x,y,iNewClass);
			}//if
		}//for
	}//for

	pTotal = new int[iNumClasses];
	for(i=0; i<iNumClasses; i++){
		pTotal[i] = 0;
	}//for
	for(i=0; i<iNumClasses; i++){
		pRecord	= pTable->Add_Record();
		iTotal = 0;
	 	for(j=0; j<iNumClasses; j++){
			pRecord->Set_Value(j, pTabulation[i][j]);
			iTotal += pTabulation[i][j];
			pTotal[j] += pTabulation[i][j];
		}//for
		pRecord->Set_Value(j, iTotal);
	}//for
	pRecord	= pTable->Add_Record();	
	for(i=0; i<iNumClasses; i++){
		pRecord->Set_Value(i, pTotal[i]);		
	}//for
	
	for (i = 0; i < iNumClasses; i++){
		delete [] pTabulation[i];
	}//for

	delete [] pTabulation;
	delete [] pTotal;

	return true;

}//method