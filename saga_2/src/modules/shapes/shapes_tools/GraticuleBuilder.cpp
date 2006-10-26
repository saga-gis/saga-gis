/*******************************************************************************
    GraticuleBuilder.cpp
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

#include "GraticuleBuilder.h"


CGraticuleBuilder::CGraticuleBuilder(void){

	Parameters.Set_Name(_TL("Create graticule"));
	Parameters.Set_Description(_TL(
		"(c) 2004 by Victor Olaya. "));

	Parameters.Add_Shapes(NULL, 
						"LINES", 
						_TL("Graticule"), 
						"", 
						PARAMETER_OUTPUT);

	Parameters.Add_Shapes(NULL, 
						"LIMIT_SHAPES", 
						_TL("Boundaries"), 
						"", 
						PARAMETER_INPUT_OPTIONAL);

				
	Parameters.Add_Range(NULL, 
							"X_EXTENT", 
							_TL("Width"),
							"");

	Parameters.Add_Range(NULL, 
							"Y_EXTENT", 
							_TL("Height"),
							"");

	Parameters.Add_Value(NULL, 
						"DISTX", 
						_TL("Distance X"), 
						"", 
						PARAMETER_TYPE_Double, 
						100);

	Parameters.Add_Value(NULL, 
						"DISTY", 
						_TL("Distance Y"), 
						"", 
						PARAMETER_TYPE_Double, 
						100);

	Parameters.Add_Choice(NULL, 
						"TYPE", 
						_TL("Graticule type"), 
						"", 
						_TL("Lines|"
						"Rectangles|"),									
						0);

}//constructor


CGraticuleBuilder::~CGraticuleBuilder(void)
{}


bool CGraticuleBuilder::On_Execute(void){
	
	CSG_Shape *pShape;
	CSG_Shapes *pLimit;
	double x,y;
	double dXMin;
	double dYMin;
	double dXMax;
	double dYMax;
	
	pLimit = Parameters("LIMIT_SHAPES")->asShapes();
	
	if (pLimit == NULL){
		dXMin	= ((CSG_Parameter_Range *) Parameters("X_EXTENT")->Get_Data())->Get_LoVal();
		dYMin	= ((CSG_Parameter_Range *) Parameters("Y_EXTENT")->Get_Data())->Get_LoVal();
		dXMax	= ((CSG_Parameter_Range *) Parameters("X_EXTENT")->Get_Data())->Get_HiVal();
		dYMax	= ((CSG_Parameter_Range *) Parameters("Y_EXTENT")->Get_Data())->Get_HiVal();
	}//if
	else{
		dXMin = pLimit->Get_Extent().Get_XMin();
		dXMax = pLimit->Get_Extent().Get_XMax();
		dYMin = pLimit->Get_Extent().Get_YMin();
		dYMax = pLimit->Get_Extent().Get_YMax();
	}//else

	int iElement = 0;
	int iType = Parameters("TYPE")->asInt();

	double dDistanceX = Parameters("DISTX")->asDouble();
	double dDistanceY = Parameters("DISTY")->asDouble();
	
	if (dDistanceX<=0 || dDistanceY<=0){
		return false;
	}//if

	CSG_Shapes *pGraticule = Parameters("LINES")->asShapes();
	
	if (iType == 0){
		pGraticule->Create(SHAPE_TYPE_Line, _TL("Graticule"));
		pGraticule->Get_Table().Add_Field("ID", TABLE_FIELDTYPE_Int);
		for (x=dXMin; x<dXMax; x=x+dDistanceX){
			pShape = pGraticule->Add_Shape();
			pShape->Add_Point(x,dYMin);
			pShape->Add_Point(x,dYMax);
			pShape->Get_Record()->Set_Value(0,iElement++);
		}//for
		for (y=dYMin; y<dYMax; y=y+dDistanceY){
			pShape = pGraticule->Add_Shape();
			pShape->Add_Point(dXMin,y);
			pShape->Add_Point(dXMax,y);		
			pShape->Get_Record()->Set_Value(0,iElement++);
		}//for
	}//if
	else{
		pGraticule->Create(SHAPE_TYPE_Polygon, _TL("Graticule"));
		pGraticule->Get_Table().Add_Field("ID", TABLE_FIELDTYPE_Int);
		for (x=dXMin; x<dXMax-dDistanceX; x=x+dDistanceX){
			for (y=dYMin; y<dYMax-dDistanceY; y=y+dDistanceY){
				pShape = pGraticule->Add_Shape();
				pShape->Add_Point(x,y);
				pShape->Add_Point(x,y+dDistanceY);		
				pShape->Add_Point(x+dDistanceX,y+dDistanceY);
				pShape->Add_Point(x+dDistanceX,y);		
				pShape->Get_Record()->Set_Value(0,iElement++);
			}//for
		}//for		
	}//else

	return true;

}//method