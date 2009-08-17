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

//---------------------------------------------------------
#include "ShapeSelector.h"
#include "Polygon_Clipper.h"

//---------------------------------------------------------
CShapeSelector::CShapeSelector(CSG_Shapes *pShapes, CSG_Shapes *pShapes2, int iCondition, bool bFromSelection)
{
	CSG_Shapes			Intersect(SHAPE_TYPE_Polygon);
	CSG_Shape_Polygon	*pIntersect	= (CSG_Shape_Polygon *)Intersect.Add_Shape();

	for(int i=0; i<pShapes->Get_Count(); i++)
	{
		CSG_Shape_Polygon	*pShape	= (CSG_Shape_Polygon *)pShapes->Get_Shape(i);

		bool	bSelect	= false;

		for(int j=0; !bSelect && j<pShapes2->Get_Count(); j++)
		{
			if( !bFromSelection || pShapes2->Get_Record(j)->is_Selected() )
			{
				CSG_Shape_Polygon	*pShape2 = (CSG_Shape_Polygon *)pShapes2->Get_Shape(j);

				switch( iCondition )
				{
				case 0: //intersect
					if( GPC_Intersection(pShape, pShape2, pIntersect) )
					{
						bSelect = true;
					}
					break;

				case 1: //are completely within
					if( GPC_Intersection(pShape, pShape2, pIntersect)
					&&  pIntersect->Get_Area() == pShape->Get_Area() )
					{
						bSelect = true;
					}
					break;

				case 2: //Completely contain
					if( GPC_Intersection(pShape, pShape2, pIntersect)
					&&	pIntersect->Get_Area() == pShape2->Get_Area() )
					{
						bSelect = true;
					}
					break;

				case 3: //have their center in
					if( pShape2->is_Containing(pShape->Get_Centroid()) )
					{
						bSelect = true;
					}
					break;

				case 4: //contain center of
					if( pShape->is_Containing(pShape2->Get_Centroid()) )
					{
						bSelect = true;
					}
					break;
				}

				if( bSelect )
				{
					m_Selection.push_back(i);
				}
			}
		}
	}
}
