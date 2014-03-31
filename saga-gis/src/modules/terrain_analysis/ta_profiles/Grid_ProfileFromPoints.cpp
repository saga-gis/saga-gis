/**********************************************************
 * Version $Id: Grid_ProfileFromPoints.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*******************************************************************************
    ProfileFromPoints.cpp
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

#include "Grid_ProfileFromPoints.h"

CProfileFromPoints::CProfileFromPoints(void){

	CSG_Parameter	*pNode_0, *pNode_1;

	Parameters.Set_Name(_TL("Profile from points"));
	Parameters.Set_Description(_TW("(c) 2004 by Victor Olaya. Creates a profile from coordinates stored in a table"));

	Parameters.Add_Grid(NULL, 
						"GRID",
						_TL("Grid"), 						
						_TL(""), 
						PARAMETER_INPUT);

	pNode_0 = Parameters.Add_Table(NULL,
								"TABLE", 
								_TL("Input"),
								_TL(""),	
								PARAMETER_INPUT);
	
	pNode_1	= Parameters.Add_Table_Field(pNode_0,
									"X",
									_TL("X"),
									_TL(""));

	pNode_1	= Parameters.Add_Table_Field(pNode_0,
									"Y",
									_TL("Y"),
									_TL(""));

	Parameters.Add_Table(NULL, 
						"RESULT", 
						_TL("Result"), 
						_TL(""), 
						PARAMETER_OUTPUT);

}//constructor


CProfileFromPoints::~CProfileFromPoints(void)
{}

bool CProfileFromPoints::On_Execute(void){
	
	CSG_Table* pTable;	
	CSG_Table* pProfileTable;
	CSG_Table_Record* pRecord;
	CSG_Grid* pGrid;	
	int iXField, iYField;	
	int i;
	int x1,x2,y1,y2;
	float fPartialDist;
	float fDist = 0;


	pGrid = Parameters("GRID")->asGrid();
	pTable = Parameters("TABLE")->asTable();
	pProfileTable = Parameters("RESULT")->asTable();
	iXField = Parameters("X")->asInt();
	iYField = Parameters("Y")->asInt();	
	
	pProfileTable->Create((CSG_Table*)NULL);
	pProfileTable->Set_Name(_TL("Profile"));
	pProfileTable->Add_Field(_TL("Distance"), SG_DATATYPE_Double);
	pProfileTable->Add_Field("Z", SG_DATATYPE_Double);

	for (i = 0; i < pTable->Get_Record_Count()-1; i++){
		
		x1=(int)(0.5 + (pTable->Get_Record(i  )->asDouble(iXField) - pGrid->Get_XMin()) / pGrid->Get_Cellsize());
		x2=(int)(0.5 + (pTable->Get_Record(i+1)->asDouble(iXField) - pGrid->Get_XMin()) / pGrid->Get_Cellsize());
		y1=(int)(0.5 + (pTable->Get_Record(i  )->asDouble(iYField) - pGrid->Get_YMin()) / pGrid->Get_Cellsize());			
		y2=(int)(0.5 + (pTable->Get_Record(i+1)->asDouble(iYField) - pGrid->Get_YMin()) / pGrid->Get_Cellsize());			

        int x = x1, y = y1, D = 0, HX = x2 - x1, HY = y2 - y1,
                c, M, xInc = 1, yInc = 1, iLastX = x1, iLastY = y1;

        if (HX < 0) {
            xInc = -1;
            HX = -HX;
        }//if
        if (HY < 0) {
            yInc = -1;
            HY = -HY;
        }//if
        if (HY <= HX) {
            c = 2 * HX;
            M = 2 * HY;
            for (;;) {                
                fPartialDist = (float)(M_GET_LENGTH(x-iLastX, y-iLastY) * pGrid->Get_Cellsize());
                if (pGrid->is_InGrid(x,y) && fPartialDist){
					fDist+=fPartialDist;
                	pRecord = pProfileTable->Add_Record();
					pRecord->Set_Value(0, fDist);
					pRecord->Set_Value(1, pGrid->asFloat(x,y));
                }//if
                iLastX = x;
                iLastY = y;
                if (x == x2) {
                    break;
                }// if
                x += xInc;
                D += M;
                if (D > HX) {
                    y += yInc;
                    D -= c;
                }// if
            }// for
        }// if
        else {
            c = 2 * HY;
            M = 2 * HX;
            for (;;) {
                fPartialDist = (float)(M_GET_LENGTH(x-iLastX, y-iLastY) * pGrid->Get_Cellsize());
                if (pGrid->is_InGrid(x,y) && fPartialDist){
					fDist+=fPartialDist;
                	pRecord = pProfileTable->Add_Record();
					pRecord->Set_Value(0, fDist);
					pRecord->Set_Value(1, pGrid->asFloat(x,y));
                }//if
                iLastX = x;
                iLastY = y;
                if (y == y2) {
                    break;
                }// if
                y += yInc;
                D += M;
                if (D > HY) {
                    x += xInc;
                    D -= c;
                }// if
            }// for
        }// else        
     
	}//for

	return true;

}// method


