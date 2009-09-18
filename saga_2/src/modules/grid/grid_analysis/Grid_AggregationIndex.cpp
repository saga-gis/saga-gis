/*******************************************************************************
    AggregationIndex.cpp
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

#include "Grid_AggregationIndex.h"


CAggregationIndex::CAggregationIndex(void){

	Set_Name		(_TL("Aggregation Index"));

	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Aggregation Index\r\n"
		"References:\r\n"
		"1. Hong S. He, et al. An aggregation index to quantify spatial patterns of landscapes, Landscape Ecology 15, 591-601,2000\r\n"
	));

	Parameters.Add_Grid(NULL,
						"INPUT",
						_TL("Input Grid"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Value(NULL,
						"MAXNUMCLASS",
						_TL("Max. Number of Classes"),
						_TL("Maximum number of classes in the entire grid."),
						PARAMETER_TYPE_Int,
						10);

	Parameters.Add_Table(NULL,
						"RESULT",
						_TL("Result"),
						_TL(""),
						PARAMETER_OUTPUT);

}//constructor


CAggregationIndex::~CAggregationIndex(void){}

bool CAggregationIndex::On_Execute(void){

	int i;
	int iClass;
	int iXOffset[] = {-1,0,0,1};
	int iYOffset[] = {0,-1,1,0};
	int iTotalArea = 0;
	int iLargestInt;
	int iMaxEii;
	int iRemainder;
	int iNumClasses = Parameters("MAXNUMCLASS")->asInt();
	CSG_Grid* pInput = Parameters("INPUT")->asGrid();
	CSG_Table* pTable = Parameters("RESULT")->asTable();
	CSG_Table_Record* pRecord;

	float** pData = new float*[iNumClasses];
	for (i = 0; i < iNumClasses; i++){
		pData[i] = new float[2];
		pData[i][0] = 0;
		pData[i][1] = 0;
	}//for

	pTable->Create((CSG_Table*)NULL);
	pTable->Set_Name(_TL("Aggregation Index"));
	pTable->Add_Field(_TL("Class"), SG_DATATYPE_Int);
	pTable->Add_Field(_TL("Area"), SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Area[%]"), SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Aggregation Index"), SG_DATATYPE_Double);

	for(int y=1; y<Get_NY()-1 && Set_Progress(y); y++){
		for(int x=1; x<Get_NX()-1; x++){
			if (!pInput->is_NoData(x,y)){
				iClass = pInput->asInt(x,y);
				if (iClass <= iNumClasses && iClass > 0){
					iTotalArea++;
					pData[iClass-1][0]++;
					for (i=0; i<4; i++){
						if (pInput->asInt(x + iXOffset[i],y + iYOffset[i]) == iClass){
							pData[iClass-1][1]++;
						}//if
					}//for
				}//if
			}//if
		}//for
	}//for

	for(i=0; i<iNumClasses; i++){
		pRecord	= pTable->Add_Record();
		pRecord->Set_Value(0, i + 1);
		pRecord->Set_Value(1, pData[i][0]);
		if (pData[i][0]){
			pRecord->Set_Value(2, (float) pData[i][0] / (float) iTotalArea * 100.0);
			iLargestInt = (int) floor(sqrt(pData[i][0]));
			iRemainder = (int) (pData[i][0] - (iLargestInt * iLargestInt));
			if (iRemainder){
				if (iRemainder < iLargestInt){
					iMaxEii = 2 * iLargestInt * (iLargestInt - 1) + 2 * iRemainder - 1;
				}//if
				else{
					iMaxEii = 2 * iLargestInt * (iLargestInt - 1) + 2 * iRemainder - 2;
				}//else
			}//if
			else{
				iMaxEii = 2 * iLargestInt * (iLargestInt - 1);
			}//else
			pRecord->Set_Value(3, (float) pData[i][1] / (float) iMaxEii / 2.0);
		}//if
		else{
			pRecord->Set_Value(2, 0.0);
			pRecord->Set_Value(3, 0.0);
		}//else
	}//for

	for (i = 0; i < iNumClasses; i++){
		delete [] pData[i];
	}//for

	delete [] pData;

	return true;

}//method
