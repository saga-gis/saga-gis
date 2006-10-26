/*******************************************************************************
    ConstantGrid.cpp
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
#include "Grid_ConstantGrid.h"

CConstantGrid::CConstantGrid(void){

	Parameters.Set_Name(_TL("Create Constant Grid"));
	Parameters.Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Constant Grid Creation"));

	Parameters.Add_Grid(NULL, 
						"GRID",
						_TL("Base Grid"), 						
						"", 
						PARAMETER_INPUT);
	
	Parameters.Add_Value(NULL, 
						"VALUE", 
						_TL("Value"), 
						"", 
						PARAMETER_TYPE_Double, 
						1);
						
}//constructor


CConstantGrid::~CConstantGrid(void)
{}

bool CConstantGrid::On_Execute(void){
	
	CSG_Grid* pBaseGrid = Parameters("GRID")->asGrid(); 
	CSG_Grid* pGrid = SG_Create_Grid(pBaseGrid);
	double dValue = Parameters("VALUE")->asDouble();

	pGrid->Assign(dValue);

	DataObject_Add(pGrid);

	return true;

}//method
