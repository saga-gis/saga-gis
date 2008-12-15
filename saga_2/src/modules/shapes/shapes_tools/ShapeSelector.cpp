/*******************************************************************************
    ShapeSelector.cpp
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
#include "ShapeSelector.h"
#include "Polygon_Clipper.h"


CShapeSelector::CShapeSelector(CSG_Shapes *pShapes,
							   CSG_Shapes *pShapes2,
							   int iCondition){

	int i,j;
	float fArea, fArea2;
	CSG_Shape *pShape, *pShape2, *pIntersect;
	bool bSelect;
	TSG_Point Point;
	CSG_Shapes	Intersect;

	Intersect.Create(SHAPE_TYPE_Polygon);
	pIntersect = Intersect.Add_Shape();

	for (i = 0; i < pShapes->Get_Count(); i++){
		pShape = pShapes->Get_Shape(i);
		bSelect = false;
		for (j = 0; j < pShapes2->Get_Count(); j++){
			if (pShapes2->Get_Record(j)->is_Selected()){
				pShape2 = pShapes2->Get_Shape(i);
				switch (iCondition){
				case 0: //intersect
					if (GPC_Intersection(pShape, pShape2, pIntersect)){
						bSelect = true;
						break;
					}//if
				case 1: //are completely within
					GPC_Intersection(pShape, pShape2, pIntersect);
					fArea = ((CSG_Shape_Polygon*)pShape)->Get_Area();
					fArea2 = ((CSG_Shape_Polygon*)pIntersect)->Get_Area();
					if (fArea == fArea2){
						bSelect = true;
					}//if
					break;
				case 2: //Completely contain
					GPC_Intersection(pShape, pShape2, pIntersect);
					fArea = ((CSG_Shape_Polygon*)pShape2)->Get_Area();
					fArea2 = ((CSG_Shape_Polygon*)pIntersect)->Get_Area();
					if (fArea == fArea2){
						bSelect = true;
					}//if
					break;
				case 3: //have their center in
					Point = ((CSG_Shape_Polygon*)pShape)->Get_Centroid();
					if (((CSG_Shape_Polygon*)pShape2)->is_Containing(Point)){
						bSelect = true;
					}//if
					break;
				case 4: //contain center of
					Point = ((CSG_Shape_Polygon*)pShape2)->Get_Centroid();
					if (((CSG_Shape_Polygon*)pShape)->is_Containing(Point)){
						bSelect = true;
					}//if
					break;
				}//switch
				if (bSelect){
					m_pSelectedRecords.push_back(i);
					break;
				}//if
			}//if
		}//for
	}//for


}//constructor

CShapeSelector::~CShapeSelector(){

}//destructor


int& CShapeSelector::GetSelectedRecords(){

	return m_pSelectedRecords[0];

}//method

int CShapeSelector::GetSelectedRecordsCount(){

	return m_pSelectedRecords.size();

}//method
