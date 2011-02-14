/*******************************************************************************
    Cost_PolarToRect.cpp
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

#include "Cost_PolarToRect.h"


CCost_PolarToRect::CCost_PolarToRect(void){

	Set_Name		(_TL("Polar To Rect"));
	Set_Author		(SG_T("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Polar to Rect conversion for paired Vector data."));

	Parameters.Add_Grid(NULL, 
						"ANGLE", 
						_TL("Angle. In radians"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"MAGNITUDE", 
						_TL("Magnitude"), 
						_TL(""), 
						PARAMETER_INPUT);
	
	Parameters.Add_Grid(NULL, 
						"X", 
						_TL("X Component"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Double);
	
	Parameters.Add_Grid(NULL, 
						"Y", 
						_TL("Y Component"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Double);

}//constructor


CCost_PolarToRect::~CCost_PolarToRect(void)
{}

bool CCost_PolarToRect::On_Execute(void){
	
	double dX,dY;
	double dMagnitude, dAngle;
	
	CSG_Grid* pAngle = Parameters("ANGLE")->asGrid(); 
	CSG_Grid* pMagnitude = Parameters("MAGNITUDE")->asGrid(); 
	CSG_Grid* pX = Parameters("X")->asGrid(); 
	CSG_Grid* pY = Parameters("Y")->asGrid(); 

	
    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){
			if( pAngle->is_NoData(x, y) || pMagnitude->is_NoData(x, y) )
			{
				pX->Set_NoData(x, y);
				pY->Set_NoData(x, y);
				continue;
			}
			dAngle = pAngle->asDouble(x,y);
			dMagnitude = pMagnitude->asDouble(x,y);
			dX = cos(dAngle)*dMagnitude;
			dY = sin(dAngle)*dMagnitude;			
			pX->Set_Value(x,y,dX);
			pY->Set_Value(x,y,dY);
        }// for
    }// for

	return true;

}//method
