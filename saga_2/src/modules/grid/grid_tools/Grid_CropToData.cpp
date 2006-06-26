/*******************************************************************************
    CropToData.cpp
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

#include "Grid_CropToData.h"

CCropToData::CCropToData(void){

	Parameters.Set_Name(_TL("Crop to Data"));
	Parameters.Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Crop grids to valid data cells"));

	Parameters.Add_Grid_List(NULL, 
						"INPUT", 
						_TL("Input Grids"), 
						"", 
						PARAMETER_INPUT);

	/*Parameters.Add_Grid(NULL, 
						"GRID",
						"Grid", 						
						"", 
						PARAMETER_INPUT);*/
	
}//constructor


CCropToData::~CCropToData(void)
{}

bool CCropToData::On_Execute(void){
	
	//CGrid* pGrid = Parameters("GRID")->asGrid(); 
	CGrid* pCroppedGrid; 	
	CGrid** pGrids;
	int iGrids;
	int iMinX = 1000000;
	int iMaxX = 0;
	int iMinY = 1000000;
	int iMaxY = 0;
	double dMinX, dMinY;
	int iNX;
	int iNY;
	int i;
	int x,y;

	if (Parameters("INPUT")->asInt() <= 0){
		Message_Add(_TL("No grids selected"));
		return (false);
	}//if
	
	iGrids		= Parameters("INPUT")->asInt();
	pGrids		=(CGrid **)Parameters("INPUT")->asPointer();
	
	for (i = 0 ; i < iGrids ; i++){
		if (pGrids[i]->is_Compatible(pGrids[0]->Get_System())){					
			for(y=0; y<Get_NY() && Set_Progress(y); y++){		
				for(x=0; x<Get_NX(); x++){
					if (!pGrids[i]->is_NoData(x,y)){
						if (x<iMinX){
							iMinX = x;
						}//if
						if (x>iMaxX){
							iMaxX = x;
						}//if
						if (y<iMinY){
							iMinY = y;
						}//if
						if (y>iMaxY){
							iMaxY = y;
						}//if
					}//if            
				}//for
			}//for		
		}//if
	}//for
	iNX = iMaxX-iMinX+1;
	iNY = iMaxY-iMinY+1;
	dMinX = pGrids[0]->Get_XMin() + iMinX * pGrids[0]->Get_Cellsize();
	dMinY = pGrids[0]->Get_YMin() + iMinY * pGrids[0]->Get_Cellsize();

	if (iNX != pGrids[0]->Get_NX() || iNY != pGrids[0]->Get_NY()){
		for (i = 0 ; i < iGrids ; i++){
			if (pGrids[i]->is_Compatible(pGrids[0]->Get_System())){	
				pCroppedGrid = new CGrid(pGrids[i]->Get_Type(), iNX, iNY, pGrids[i]->Get_Cellsize(), dMinX, dMinY);	
				pCroppedGrid->Assign(pGrids[i], GRID_INTERPOLATION_BSpline);
				DataObject_Add(pCroppedGrid);
			}//if
		}//for
	}//if

	return true;

}//method