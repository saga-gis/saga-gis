/*******************************************************************************
    Cost_RectToPolar.cpp
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

#include "Cost_RectToPolar.h"


CCost_RectToPolar::CCost_RectToPolar(void){

	Set_Name(_TL("Rect To Polar"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Rect to Polar conversion for paired Vector data."));

	Parameters.Add_Grid(NULL, 
						"X", 
						_TL("X Component"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"Y", 
						_TL("Y Component"), 
						"", 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"ANGLE", 
						_TL("Angle. In radians"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);
	
	Parameters.Add_Grid(NULL, 
						"MAGNITUDE", 
						_TL("Magnitude"), 
						"", 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);

}//constructor


CCost_RectToPolar::~CCost_RectToPolar(void)
{}

bool CCost_RectToPolar::On_Execute(void){
	
	double dX,dY;
	double dMagnitude, dAngle;
	double PI = 3.141592;
	
	CSG_Grid* pAngle = Parameters("ANGLE")->asGrid(); 
	CSG_Grid* pMagnitude = Parameters("MAGNITUD")->asGrid(); 
	CSG_Grid* pX = Parameters("X")->asGrid(); 
	CSG_Grid* pY = Parameters("Y")->asGrid(); 

	pX->Assign(0.0);
	pY->Assign(0.0);
	
    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){
			dX = pX->asDouble(x,y);
			dY = pY->asDouble(x,y);
			dMagnitude =sqrt(dX*dX+dY*dY);
			dAngle = atan((double)y/x);	
			if (x*y>0){
				if (y<0 && x<0){
					dAngle+=PI;
				}//if
			}//if
			else {
				if (y<0){
					dAngle = 2*PI-dAngle;
				}//if
				else{
					dAngle = PI-dAngle;
				}//else
			}//else

			pMagnitude->Set_Value(x,y,dMagnitude);
			pAngle->Set_Value(x,y,dAngle);
        }// for
    }// for

	return true;

}//method
