/*******************************************************************************
    SortRaster.cpp
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

#include "Grid_SortRaster.h"

CSortRaster::CSortRaster(void){

	Parameters.Set_Name(_TL("Sort Grid"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya. Sort Grid"));

	Parameters.Add_Grid(NULL, 
						"GRID",
						_TL("Input Grid"), 						
						_TL(""), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"OUTPUT",
						_TL("Sorted Grid"), 						
						_TL(""), 
						PARAMETER_OUTPUT);

	Parameters.Add_Value(NULL,
						"DOWN",
						_TL("Down sort"),
						_TL(""),
						PARAMETER_TYPE_Bool,
						false);
						
}//constructor


CSortRaster::~CSortRaster(void)
{}

bool CSortRaster::On_Execute(void){
	
	CSG_Grid* pGrid = Parameters("GRID")->asGrid(); 
	CSG_Grid* pSortedGrid = Parameters("OUTPUT")->asGrid();
	bool bDown = Parameters("DOWN")->asBool();
	bool bCopy = false;
	long i;
	int iX, iY;
	int iCell=0;

	if (pGrid == pSortedGrid){
		pSortedGrid = SG_Create_Grid(pGrid);
		bCopy = true;
	}//if

	pSortedGrid->Set_NoData_Value(0.0);

	for(i=0; i<Get_NCells() && Set_Progress(i, Get_NCells()); i++){		
		pGrid->Get_Sorted(i, iX, iY, bDown, false);
		if (pGrid->is_NoData(iX,iY)){
			pSortedGrid->Set_NoData(iX,iY);
		}//if
		else{
	//		pSortedGrid->Set_Value(iX, iY, i);
			pSortedGrid->Set_Value(iX, iY, ++iCell);
		}//else
	}//for
	
	if (bCopy){
		pGrid->Assign(pSortedGrid);
	}//if

	return true;

}//method
