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
	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Geometrical Properties of Polygons"));

	Parameters.Add_Shapes(NULL, 
						"POLYG", 
						_TL("Polygons"),
						_TL(""),
						PARAMETER_INPUT, SHAPE_TYPE_Polygon);

	Parameters.Add_Value(
		NULL, "SAVE", _TL("Save Shapefile"),
		_TL("Save the results to the input shapefile."),
		PARAMETER_TYPE_Bool, false
	);

}//constructor


CPolygon_Geometrics::~CPolygon_Geometrics(void)
{}

bool CPolygon_Geometrics::On_Execute(void){

	CSG_Shapes *pPolygs;
	TSG_Point	Point, Point2;	
	CSG_Shape *pShape;
	double dArea;
	double dPerim;
	bool	bSave;
		
	pPolygs = Parameters("POLYG")->asShapes();
	bSave	= Parameters("SAVE")->asBool();

	pPolygs->Add_Field(_TL("Perimeter"), SG_DATATYPE_Double);
	pPolygs->Add_Field(_TL("Area"), SG_DATATYPE_Double);
	
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
		pShape->Set_Value(pPolygs->Get_Field_Count()-2, dPerim);
		pShape->Set_Value(pPolygs->Get_Field_Count()-1, dArea);

	}//for

	DataObject_Update(pPolygs);

	if (bSave)
		pPolygs->Save(pPolygs->Get_File_Name());

	return true;

}//method
