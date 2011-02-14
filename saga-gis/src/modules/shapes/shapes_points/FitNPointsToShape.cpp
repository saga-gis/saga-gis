/*******************************************************************************
    FitNPointsToShape.cpp
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

#include "FitNPointsToShape.h"

#define MAX_REP 30

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

CFitNPointsToShape::CFitNPointsToShape(void){

	Parameters.Set_Name(_TL("Fit N Points to shape"));
	Parameters.Set_Description(_TW(
		"(c) 2004 by Victor Olaya. Fit N Points to shape"));

	Parameters.Add_Shapes(NULL, 
						"POINTS", 
						_TL("Points"), 
						_TL(""), 
						PARAMETER_OUTPUT);

	Parameters.Add_Shapes(NULL, 
						"SHAPES", 
						_TL("Shapes"),
						_TL(""),
						PARAMETER_INPUT);

	Parameters.Add_Value(NULL, 
						"NUMPOINTS", 
						_TL("Number of points"), 
						_TL("Number of points"), 
						PARAMETER_TYPE_Int, 
						100,
						1,
						true);

}//constructor


CFitNPointsToShape::~CFitNPointsToShape(void)
{}

bool CFitNPointsToShape::On_Execute(void){

	CSG_Shapes *pShapes, *pPoints;
	CSG_Shape *pShape, *pShape2;		
	bool bCopy;
	int iPoints;
	int iPointsIn = 0;
	int iRep = 0;
	int i,j;
	double x,y;	
	double dArea;
	double dDist;
	double dDistInf, dDistSup;
	bool bFirstTime;
	TSG_Rect Extent;

	pShapes = Parameters("SHAPES")->asShapes();
	pPoints = Parameters("POINTS")->asShapes();
	iPoints = Parameters("NUMPOINTS")->asInt();

	if(pShapes == pPoints){
		bCopy = true;
		pPoints	= SG_Create_Shapes();
	}//if
	else{
		bCopy = false;
	}//else

	pPoints->Create(SHAPE_TYPE_Point, _TL("Point Grid"));
	pPoints->Add_Field("X", SG_DATATYPE_Double);
	pPoints->Add_Field("Y", SG_DATATYPE_Double);

	for (i = 0; i < pShapes->Get_Selection_Count(); i++){
		iRep = 0;

		pShape = pShapes->Get_Selection(i);	
		dArea = ((CSG_Shape_Polygon*)pShape)->Get_Area();
		dDist = sqrt(dArea / (double) iPoints);
		dDistInf = sqrt(dArea / (double) (iPoints + 2));
		dDistSup = sqrt(dArea / (double) (iPoints - min(2, iPoints-1)));
		
		Extent = ((CSG_Shape_Polygon*)pShape)->Get_Extent();

		bFirstTime = true;
		do{
			if (!bFirstTime){
				for (j = 0; j < iPointsIn; j++){
					pPoints->Del_Shape(pPoints->Get_Count()-1);
				}//for
			}//if		
			iPointsIn = 0;
			iRep++;	
			for (x=Extent.xMin; x<Extent.xMax; x=x+dDist){
				for (y=Extent.yMin; y<Extent.yMax; y=y+dDist){ 
					if (((CSG_Shape_Polygon*)pShape)->Contains(x,y)){
						pShape2 = pPoints->Add_Shape();
						pShape2->Add_Point(x,y);
						pShape2->Set_Value(0, x);
						pShape2->Set_Value(1, y);
						iPointsIn++;
					}//if
				}//for
			}//for
			if (iPointsIn > iPoints){
				dDistInf = dDist;
				dDist = (dDistInf + dDistSup) / 2.;
			}//if
			else if (iPointsIn < iPoints){
				dDistSup = dDist;
				dDist = (dDistInf + dDistSup) / 2.;
			}//if
			bFirstTime = false;
		}while(iPointsIn != iPoints && iRep < MAX_REP);
	
	}//for

	if(bCopy){
		pShapes->Assign(pPoints);
		delete(pPoints);
	}//if

	return true;

}//method
