/*******************************************************************************
    TransformShapes.cpp
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

#include "TransformShapes.h"


CTransformShapes::CTransformShapes(void){

	
	CParameter *pNode_0, *pNode_1;
	Parameters.Set_Name(_TL("Transform Shapes"));
	Parameters.Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Use this module to move, rotate and/or scale shapes."));

	Parameters.Add_Shapes(NULL, 
						"OUT", 
						_TL("Output"), 
						"", 
						PARAMETER_OUTPUT);

	Parameters.Add_Shapes(NULL, 
						"IN", 
						_TL("Shapes"),
						"",
						PARAMETER_INPUT);

	pNode_0	= Parameters.Add_Node(NULL, 
								"MOVE",
								_TL("Move"),
								"");

	pNode_1 = Parameters.Add_Value(pNode_0, 
								"DX", 
								_TL("dX"), 
								_TL("dX(GridUnits)"), 
								PARAMETER_TYPE_Double, 
								0);

	pNode_1 = Parameters.Add_Value(pNode_0, 
								"DY", 
								_TL("dY"), 
								_TL("dY(GridUnits)"), 
								PARAMETER_TYPE_Double, 
								0);


	pNode_0	= Parameters.Add_Node(NULL, 
								"ROTATE",
								_TL("Rotate"),
								"");
	
	pNode_1 = Parameters.Add_Value(pNode_0, 
								"ANGLE", 
								_TL("Angle"), 
								_TL("Angle in degrees, counting clockwise from north"), 
								PARAMETER_TYPE_Double, 
								0);

	pNode_0	= Parameters.Add_Node(NULL, 
								"SCALE",
								_TL("Scale"),
								"");

	pNode_1 = Parameters.Add_Value(pNode_0, 
								"SCALEX", 
								_TL("Scale Factor X"), 
								_TL("Scale Factor X"), 
								PARAMETER_TYPE_Double, 
								2);

	pNode_1 = Parameters.Add_Value(pNode_0, 
								"SCALEY", 
								_TL("Scale Factor Y"), 
								_TL("Scale Factor Y"), 
								PARAMETER_TYPE_Double, 
								1);

	pNode_0	= Parameters.Add_Node(NULL, 
								"ANCHOR",
								_TL("Anchor Point"),
								"");

	pNode_1 = Parameters.Add_Value(pNode_0, 
								"ANCHORX", 
								"X", 
								"X", 
								PARAMETER_TYPE_Double, 
								0);

	pNode_1 = Parameters.Add_Value(pNode_0, 
								"ANCHORY", 
								"Y", 
								"Y", 
								PARAMETER_TYPE_Double, 
								0);
	
	
}//constructor


CTransformShapes::~CTransformShapes(void)
{}

bool CTransformShapes::On_Execute(void){


	bool bCopy;
	m_dScaleX = Parameters("SCALEX")->asDouble();
	m_dScaleY = Parameters("SCALEY")->asDouble();
	m_dDX = Parameters("DX")->asDouble();
	m_dDY = Parameters("DY")->asDouble();
	m_dAngle = Parameters("ANGLE")->asDouble() * M_DEG_TO_RAD;
	m_dAnchorX = Parameters("ANCHORX")->asDouble();
	m_dAnchorY = Parameters("ANCHORY")->asDouble();
	TSG_Point	Point;	
	CShape *pShape, *pShape2;
	CShapes *pIn = Parameters("IN")->asShapes();
	CShapes *pOut= Parameters("OUT")->asShapes();
	double dX=0, dY=0, dX2=0, dY2=0;
	int i;

	if(pIn == pOut){
		bCopy = true;
		pOut	= SG_Create_Shapes();
	}//if
	else{
		bCopy = false;
	}//else

	pOut->Create(pIn->Get_Type(), _TL("Transformed Shapes"));

	pOut->Get_Table().Create(pIn->Get_Table());	

	for(i=0; i<pIn->Get_Count(); i++){			
		pShape = pIn->Get_Shape(i);					
		pShape2 = pOut->Add_Shape();	
		for(int j=0; j<pShape->Get_Part_Count(); j++){	
			for(int k=0; k<pShape->Get_Point_Count(j); k++){
				Point = pShape->Get_Point(k,j);
				dX = Point.x;
				dY = Point.y;
				//move
				dX+=m_dDX;
				dY+=m_dDY;
				//rotate and scale
				dX-=m_dAnchorX;
				dY-=m_dAnchorY;
				dX2 = m_dScaleX * (dX * cos(m_dAngle) - dY * sin(m_dAngle));
				dY2 = m_dScaleY * (dX* sin(m_dAngle) + dY * cos(m_dAngle));
				dX2+=m_dAnchorX;
				dY2+=m_dAnchorY;
				pShape2->Add_Point(dX2,dY2,j);
			}//for
		}//for
	}//for

	if(bCopy){
		pIn->Assign(pOut);
		delete(pOut);
	}//if

	return true;

}//method
