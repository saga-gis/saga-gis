/*******************************************************************************
    CoveredDistance.cpp
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
#include "CoveredDistance.h"

CCoveredDistance::CCoveredDistance(void){


	Parameters.Set_Name(_TL("Covered Distance"));

	Parameters.Set_Description("");

	Parameters.Add_Grid_List(
		NULL, "INPUT"	, _TL("Grids")	,
		"", PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"	, _TL("Result"),
		"",
		PARAMETER_OUTPUT
	);
	

}//constructor

CCoveredDistance::~CCoveredDistance(void){}

bool CCoveredDistance::On_Execute(void){

	int i;
	int x,y;
	double dDif;
	double dValue;

	CSG_Grid** pGrids = (CSG_Grid **) Parameters("INPUT")->asPointer();

	CSG_Grid *pResult = Parameters("RESULT")->asGrid();
	int iGrids = Parameters("GRIDS")->asInt();

	pResult->Assign(0.0);

	for (i = 0; i < iGrids - 1; i++){
		for (x = 0; x < Get_NX(); x++){
			for (y = 0; y < Get_NY(); y++){
				dValue = pResult->asDouble(x,y);
				dDif = fabs(pGrids[i+1]->asDouble(x,y) - pGrids[i]->asDouble(x,y)); 
				pResult->Set_Value(x,y,dValue + dDif);
			}//for
		}//for
	}//for

	return true;

}//method