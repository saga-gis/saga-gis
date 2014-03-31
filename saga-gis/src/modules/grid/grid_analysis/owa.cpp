/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    OWA.cpp
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/

#include "owa.h"

COWA::COWA(void){

	Parameters.Set_Name(_TL("Ordered Weighted Averaging (OWA)"));
	Parameters.Set_Description(_TW(
		"(c) 2006 by Victor Olaya. Ordered Weighted Averaging (OWA)"));

	Parameters.Add_Grid_List(NULL,
						"GRIDS", 
						_TL("Input Grids"),
						_TL("Input Grids"),
						PARAMETER_INPUT);

	Parameters.Add_FixedTable(NULL, 
							"WEIGHTS", 
							_TL("Weights"),
							_TL(""));

	Parameters.Add_Grid(NULL,
						"OUTPUT",
						_TL("Output Grid"),
						_TL(""),
						PARAMETER_OUTPUT);

	CSG_Table_Record *pRecord;
	CSG_Table *pWeights = Parameters("WEIGHTS")->asTable();

	pWeights->Set_Name(_TL("Weights"));

	pWeights->Add_Field(_TL("Weight")	, SG_DATATYPE_Double);

	pRecord	= pWeights->Add_Record();
	pRecord->Set_Value(0, 1);

}//constructor


COWA::~COWA(void){}

bool COWA::On_Execute(void){

	bool bAllValuesAreOK;
	int i;	
	int x,y;	
	double *pCoefs;
	double *pOrderedValues;
	double dValue;
	double dSum = 0;
	CSG_Grid *pOutputGrid;
// OC:	CSG_Grid **pGrids;
	CSG_Table_Record *pRecord;
	CSG_Table *pTable;
	CSG_Parameter_Grid_List* pGridsList;

	pTable = Parameters("WEIGHTS")->asTable();
	pOutputGrid = Parameters("OUTPUT")->asGrid();

	if( (pGridsList = (CSG_Parameter_Grid_List *)Parameters("GRIDS")->Get_Data()) != 
			NULL && pGridsList->Get_Count() > 0 ){

		if (pTable->Get_Record_Count() < pGridsList->Get_Count()){
			Message_Add(_TL("Error : Wrong weights table. Check table dimensions"));
			return false;
		}//if
		pCoefs			= new double [pGridsList->Get_Count()];
		pOrderedValues	= new double [pGridsList->Get_Count()];	// OC:
		for (i = 0; i<pGridsList->Get_Count(); i++){
			pRecord = pTable->Get_Record(i);
			pCoefs[i] = pRecord->asDouble(0);
			dSum += pRecord->asDouble(0);
		}//for
		for (i = 0; i<pGridsList->Get_Count(); i++){
			pCoefs[i] = pCoefs[i] / dSum;	
		}//for
		for(y=0; y<Get_NY() && Set_Progress(y); y++){
			for(x=0; x<Get_NX(); x++){
				dValue = 0;
				bAllValuesAreOK = true;
				for (i = 0; i<pGridsList->Get_Count(); i++){
// OC:				if (!pGrids[i]->is_NoData(x,y)){
					if (!pGridsList->asGrid(i)->is_NoData(x,y)){
// OC:					pOrderedValues[i] = pGrids[i]->asDouble(x,y);
						pOrderedValues[i] = pGridsList->asGrid(i)->asDouble(x,y);
					}//if
					else{
						bAllValuesAreOK = false;
						break;
					}//else
				}//for
				if (bAllValuesAreOK){
					Sort(pOrderedValues, pGridsList->Get_Count());
					for (i = 0; i<pGridsList->Get_Count(); i++){
						dValue += pCoefs[i] * pOrderedValues[i];
					}//for
					pOutputGrid->Set_Value(x,y,dValue);
				}//if
				else{
					pOutputGrid->Set_NoData(x,y);
				}//else
			}//for
		}//for

	delete [] pCoefs;
	delete [] pOrderedValues;

	}//if
	return true;

}//method

#define SWAP(a,b)	{dTemp=(a);(a)=(b);(b)=dTemp;}

void COWA::Sort(double *arr, int size){

     int indexOfMin, pass, j;
	 double dTemp;

     for (pass = 0; pass < size - 1; pass++){
           indexOfMin = pass;

           for (j = pass + 1; j < size; j++)
                 if (arr[j] < arr[indexOfMin])
                       indexOfMin = j;

           SWAP(arr[pass], arr[indexOfMin]);
     }//for

}// method

