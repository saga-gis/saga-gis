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
	Set_Author(_TL("Victor Olaya & Volker Wichmann (c) 2004-2010"));
	Set_Description(_TW(
					"Rect to polar conversion for paired vector data.\n"
					"The module supports two conventions on how to measure and "
					"output the angle of direction:\n"
					"(a) mathematical: angle is output in radians, whereby "
					"east is zero, and the angle increases counterclockwise\n"
					"(b) geographical: angle is output in degrees, whereby "
					"north is zero, and the angle increases clockwise\n\n")
	);

	Parameters.Add_Grid(NULL, 
						"X", 
						_TL("X Component"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"Y", 
						_TL("Y Component"), 
						_TL(""), 
						PARAMETER_INPUT);

	Parameters.Add_Choice(
		NULL	, "METHOD_ANGLE"	, _TL("Method Angle"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("mathematical"),
			_TL("geographical")
		), 0
	);
	
	Parameters.Add_Grid(NULL, 
						"ANGLE", 
						_TL("Angle"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Double);
	
	Parameters.Add_Grid(NULL, 
						"MAGNITUDE", 
						_TL("Magnitude"), 
						_TL(""), 
						PARAMETER_OUTPUT, 
						true, 
						SG_DATATYPE_Double);

}//constructor


CCost_RectToPolar::~CCost_RectToPolar(void)
{}

bool CCost_RectToPolar::On_Execute(void){
	
	int		method;
	double	dX,dY;
	double	dMagnitude, dAngle;
	
	CSG_Grid* pAngle = Parameters("ANGLE")->asGrid(); 
	CSG_Grid* pMagnitude = Parameters("MAGNITUDE")->asGrid(); 
	CSG_Grid* pX = Parameters("X")->asGrid(); 
	CSG_Grid* pY = Parameters("Y")->asGrid(); 
	method = Parameters("METHOD_ANGLE")->asInt();

    for(int y=0; y<Get_NY() && Set_Progress(y); y++){		
		for(int x=0; x<Get_NX(); x++){
			if (pX->is_NoData(x, y) || pY->is_NoData(x, y))
			{
				pMagnitude->Set_NoData(x, y);
				pAngle->Set_NoData(x, y);
				continue;
			}

			dX = pX->asDouble(x,y);
			dY = pY->asDouble(x,y);

			dMagnitude =sqrt(dX*dX+dY*dY);

			if( dX > 0.0 && dY < 0.0 )
			{
				dAngle = atan(dY/dX) + M_PI_360;
			}
			else if( dX > 0.0 && y >= 0.0 )
			{
				dAngle = atan((double)dY/dX);
			}
			else if( dX == 0.0 && dY > 0.0 )
			{
				dAngle = M_PI_090;
			}
			else if( dX == 0.0 && dY == 0.0 )
			{
				dAngle = pMagnitude->Get_NoData_Value();
			}
			else if( dX == 0.0 && dY < 0.0 )
			{
				dAngle = M_PI_270;
			}
			else if( dX < 0.0 )
			{
				dAngle = atan((double)dY/dX) + M_PI;
			}

			if( method == 1 && dX != 0.0 && dY != 0.0 )
				dAngle = fmod((450.0 - dAngle * M_RAD_TO_DEG), 360);

			pMagnitude->Set_Value(x,y,dMagnitude);
			pAngle->Set_Value(x,y,dAngle);
        }// for
    }// for

	return true;

}//method
