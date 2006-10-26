/*******************************************************************************
    Grid_CVA.cpp
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

#include "Grid_CVA.h"


CGrid_CVA::CGrid_CVA(void){

	Set_Name(_TL("Change Vector Analysis"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Change Vector Analysis"));

	Parameters.Add_Grid(NULL, 
						"A1", 
						_TL("Grid A. Init"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"A2", 
						_TL("Grid A. Final"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"B1", 
						_TL("Grid B. Init"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"B2", 
						_TL("Grid B. Final"), 
						"", 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"DIST", 
						_TL("Distance"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL, 
						"ANGLE", 
						_TL("Angle"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);


}//constructor


CGrid_CVA::~CGrid_CVA(void)
{}

bool CGrid_CVA::On_Execute(void){
	
	double a1,a2,b1,b2;
	double dDist, dAngle;
	
	CSG_Grid* pA1 = Parameters("A1")->asGrid(); 
	CSG_Grid* pA2 = Parameters("A2")->asGrid(); 
	CSG_Grid* pB1 = Parameters("B1")->asGrid(); 
	CSG_Grid* pB2 = Parameters("B2")->asGrid(); 
	CSG_Grid* pDist = Parameters("DIST")->asGrid(); 
	CSG_Grid* pAngle = Parameters("ANGLE")->asGrid();
	pDist->Assign(0.0);
	pAngle->Assign(0.0);

    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){
			a1 = pA1->asDouble(x,y);
			a2 = pA2->asDouble(x,y);
			b1 = pB1->asDouble(x,y);
			b2 = pB2->asDouble(x,y);
			dDist = sqrt((a1-a2)*(a1-a2)+(b1-b2)*(b1-b2));
			dAngle = atan((a1-a2)/(b1-b2));			
			pDist->Set_Value(x,y,dDist);
			pAngle->Set_Value(x,y,dAngle);
        }// for
    }// for

	return true;

}//method
