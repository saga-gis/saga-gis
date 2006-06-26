/*******************************************************************************
    Polygon_Geometrics.cpp
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

#include "Polygon_Geometrics.h"

CPolygon_Geometrics::CPolygon_Geometrics(void){

	Set_Name(_TL("Geometrical Properties of Polygons"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Geometrical Properties of Polygons"));

	Parameters.Add_Shapes(NULL, 
						"POLYG", 
						_TL("Polygons"),
						"",
						PARAMETER_INPUT, SHAPE_TYPE_Polygon);

}//constructor


CPolygon_Geometrics::~CPolygon_Geometrics(void)
{}

bool CPolygon_Geometrics::On_Execute(void){

	CTable *pTable;
	CShapes *pPolygs;
	TGEO_Point	Point, Point2;	
	CShape *pShape;
	double dArea;
	double dPerim;
		
	pPolygs = Parameters("POLYG")->asShapes();			

	pTable=&pPolygs->Get_Table();
	pTable->Add_Field(_TL("Perimeter"), TABLE_FIELDTYPE_Double);
	pTable->Add_Field(_TL("Area"), TABLE_FIELDTYPE_Double);
	
	for(int i=0; i<pPolygs->Get_Count() && Set_Progress(i, pPolygs->Get_Count()); i++){			
		pShape = pPolygs->Get_Shape(i);		
		dPerim = 0;
		dArea = 0;
		for(int j=0; j<pShape->Get_Part_Count(); j++){	
			for(int k=0; k<pShape->Get_Point_Count(j)-1; k++){
				Point = pShape->Get_Point(k,j);
				Point2 = pShape->Get_Point(k+1,j);	
				dArea += (Point.x*Point2.y-Point.y*Point2.x);
				dPerim += (sqrt(pow(Point.x-Point2.x,2)+pow(Point.y-Point2.y,2)));
			}//for
			Point = pShape->Get_Point(pShape->Get_Point_Count(j)-1,j);
			Point2 = pShape->Get_Point(0,j);	
			dArea += (Point.x*Point2.y-Point.y*Point2.x);
			dPerim += (sqrt(pow(Point.x-Point2.x,2)+pow(Point.y-Point2.y,2)));
		}//for
		dArea= fabs(dArea/2);
		pShape->Get_Record()->Set_Value(pTable->Get_Field_Count()-2, dPerim);
		pShape->Get_Record()->Set_Value(pTable->Get_Field_Count()-1, dArea);

	}//for

	return true;

}//method
