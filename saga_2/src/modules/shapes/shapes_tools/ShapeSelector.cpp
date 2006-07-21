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
#include "Intersection_GPC.h"


CShapeSelector::CShapeSelector(CShapes *pShapes,
							   CShapes *pShapes2,
							   int iCondition){

	int i,j;
	float fArea, fArea2;
	CShape *pShape, *pShape2, *pIntersect;
	bool bSelect;
	TSG_Point Point;
	CShapes	Intersect;

	Intersect.Create(SHAPE_TYPE_Polygon);
	pIntersect = Intersect.Add_Shape();

	for (i = 0; i < pShapes->Get_Count(); i++){
		pShape = pShapes->Get_Shape(i);
		bSelect = false;
		for (j = 0; j < pShapes2->Get_Count(); j++){
			if (pShapes2->Get_Table().Get_Record(j)->is_Selected()){
				pShape2 = pShapes2->Get_Shape(i);
				switch (iCondition){
				case 0: //intersect
					if (GPC_Intersection(pShape, pShape2, pIntersect)){
						bSelect = true;
						break;
					}//if
				case 1: //are completely within
					GPC_Intersection(pShape, pShape2, pIntersect);
					fArea = ((CShape_Polygon*)pShape)->Get_Area();
					fArea2 = ((CShape_Polygon*)pIntersect)->Get_Area();
					if (fArea == fArea2){
						bSelect = true;
					}//if
					break;
				case 2: //Completely contain
					GPC_Intersection(pShape, pShape2, pIntersect);
					fArea = ((CShape_Polygon*)pShape2)->Get_Area();
					fArea2 = ((CShape_Polygon*)pIntersect)->Get_Area();
					if (fArea == fArea2){
						bSelect = true;
					}//if
					break;
				case 3: //have their center in
					Point = ((CShape_Polygon*)pShape)->Get_Centroid();
					if (((CShape_Polygon*)pShape2)->is_Containing(Point)){
						bSelect = true;
					}//if
					break;
				case 4: //contain center of
					Point = ((CShape_Polygon*)pShape2)->Get_Centroid();
					if (((CShape_Polygon*)pShape)->is_Containing(Point)){
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

bool CShapeSelector::GPC_Intersection(CShape *pShape_A, CShape *pShape_B, CShape *pShape_AB)
{
	bool		bResult;
	int			iPoint, nPoints, iPart;
	gpc_polygon	poly_A, poly_B, poly_AB;
	gpc_vertex	*Contour;

	bResult	= false;

	if(	GPC_Create_Polygon(pShape_A, &poly_A)
	&&	GPC_Create_Polygon(pShape_B, &poly_B) )
	{
		gpc_polygon_clip(GPC_INT, &poly_A, &poly_B, &poly_AB);

		if( poly_AB.num_contours > 0 )
		{
			pShape_AB->Del_AllParts();

			for(iPart=0; iPart<poly_AB.num_contours; iPart++)
			{
				Contour	= poly_AB.contour[iPart].vertex;
				nPoints	= poly_AB.contour[iPart].num_vertices;

				for(iPoint=0; iPoint<nPoints; iPoint++)
				{
					pShape_AB->Add_Point(Contour[iPoint].x, Contour[iPoint].y, iPart);
				}
			}

			bResult	= true;
		}

		gpc_free_polygon(&poly_AB);
	}

	gpc_free_polygon(&poly_A);
	gpc_free_polygon(&poly_B);

	return( bResult );
}

bool CShapeSelector::GPC_Create_Polygon(CShape *pShape, gpc_polygon *pPolygon)
{
	int				iPoint, iPart;
	TSG_Point		Point;
	gpc_vertex		*Contour;
	gpc_vertex_list	vList;

	pPolygon->contour		= NULL;
	pPolygon->hole			= NULL;
	pPolygon->num_contours	= 0;

	//-----------------------------------------------------
	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( pShape->Get_Point_Count(iPart) > 0 )
		{
			Contour	= (gpc_vertex *)malloc(pShape->Get_Point_Count(iPart) * sizeof(gpc_vertex));

			for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				Point	= pShape->Get_Point(iPoint, iPart);
				Contour[iPoint].x	= Point.x;
				Contour[iPoint].y	= Point.y;
			}

			vList.num_vertices	= pShape->Get_Point_Count(iPart);
			vList.vertex		= Contour;
			gpc_add_contour(pPolygon, &vList, ((CShape_Polygon *)pShape)->is_Lake(iPart) ? 1 : 0);

			free(Contour);
		}
	}

	return( pPolygon->num_contours > 0 );
}