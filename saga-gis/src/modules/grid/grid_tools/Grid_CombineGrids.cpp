/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    CombineGrids.cpp
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

#include "Grid_CombineGrids.h"

CCombineGrids::CCombineGrids(void){

	CSG_Table *pLookup;
	CSG_Table_Record *pRecord;

	Parameters.Set_Name(_TL("Combine Grids"));
	Parameters.Set_Description(_TW(
		"(c) 2005 by Victor Olaya."));

	Parameters.Add_Grid(NULL, 
						"GRID1",
						_TL("Grid 1"), 						
						_TL(""), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"GRID2",
						_TL("Grid 2"), 						
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"RESULT",
						_TL("Result"), 						
						_TL(""), 
						PARAMETER_OUTPUT);

	pLookup	= Parameters.Add_FixedTable(NULL, 
										"LOOKUP", 
										_TL("LookUp Table"),
										_TL(""))->asTable();

	pLookup->Add_Field(_TL("Value in Grid 1"), SG_DATATYPE_Double);
	pLookup->Add_Field(_TL("Value in Grid 2"), SG_DATATYPE_Double);
	pLookup->Add_Field(_TL("Resulting Value"), SG_DATATYPE_Double);

	pRecord	= pLookup->Add_Record();	
	pRecord->Set_Value(0, 0.0);	
	pRecord->Set_Value(1, 0.0);	
	pRecord->Set_Value(2, 0.0);

						
}//constructor


CCombineGrids::~CCombineGrids(void)
{}

bool CCombineGrids::On_Execute(void){
	
	int x,y;
	int i;
	int iCellValue1, iCellValue2;
	int iTableValue1, iTableValue2;
	int iResultValue;
	
	CSG_Grid *pGrid1 = Parameters("GRID1")->asGrid(); 
	CSG_Grid *pGrid2 = Parameters("GRID2")->asGrid(); 
	CSG_Grid *pResult = Parameters("RESULT")->asGrid(); ;
	CSG_Table			*pLookup;
	CSG_Table_Record	*pRecord;

	pLookup	= Parameters("LOOKUP")->asTable();

	for(y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(x=0; x<Get_NX(); x++){
			iCellValue1 = pGrid1->asInt(x,y);
			iCellValue2 = pGrid2->asInt(x,y);
			for (i = 0; i < pLookup->Get_Record_Count(); i++){
				pRecord = pLookup->Get_Record(i);
				iTableValue1 = pRecord->asInt(0);
				iTableValue2 = pRecord->asInt(1);
				if (iTableValue1 == iCellValue1){
					if (iTableValue2 == iCellValue2){
						iResultValue = pRecord->asInt(2);
						pResult->Set_Value(x,y,iResultValue);
						break;
					}//if
				}//if				
			}//for
			if (i >= pLookup->Get_Record_Count()){
				pResult->Set_NoData(x,y);
			}//if
		}//for
	}//for
	return true;

}//method
