/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Shapes_Tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 quadtree_structure.cpp                //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "quadtree_structure.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CQuadTree_Structure::CQuadTree_Structure(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("QuadTree Structure to Shapes"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ATTRIBUTE"	, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);
}

//---------------------------------------------------------
bool CQuadTree_Structure::On_Execute(void)
{
	CSG_PRQuadTree	QT(Parameters("SHAPES")->asShapes(), Parameters("ATTRIBUTE")->asInt());

	Get_Shapes(Parameters("LINES")		->asShapes(), (CSG_PRQuadTree_Item *)&QT.Get_Root(), SHAPE_TYPE_Line);
	Get_Shapes(Parameters("POLYGONS")	->asShapes(), (CSG_PRQuadTree_Item *)&QT.Get_Root(), SHAPE_TYPE_Polygon);

	return( true );
}

//---------------------------------------------------------
bool CQuadTree_Structure::Get_Shapes(CSG_Shapes *pShapes, CSG_PRQuadTree_Item *pItem, TSG_Shape_Type Type)
{
	CSG_Shape	*pShape;

	if( pItem )
	{
		switch( Type )
		{
		default:
			return( false );

		case SHAPE_TYPE_Line:
			pShapes->Create(SHAPE_TYPE_Line, _TL("QuadTree - Lines"));
			pShapes->Add_Field("LEVEL", SG_DATATYPE_Int);

			pShape	= pShapes->Add_Shape();
			pShape->Set_Value(0, 0.0);
			pShape->Add_Point(pItem->Get_xMin(), pItem->Get_yMin());
			pShape->Add_Point(pItem->Get_xMin(), pItem->Get_yMax());
			pShape->Add_Point(pItem->Get_xMax(), pItem->Get_yMax());
			pShape->Add_Point(pItem->Get_xMax(), pItem->Get_yMin());
			pShape->Add_Point(pItem->Get_xMin(), pItem->Get_yMin());
			break;

		case SHAPE_TYPE_Polygon:
			pShapes->Create(SHAPE_TYPE_Polygon, _TL("QuadTree - Squares"));
			pShapes->Add_Field("LEVEL", SG_DATATYPE_Int);
			pShapes->Add_Field("VALUE", SG_DATATYPE_Double);
			break;
		}

		Get_Shapes(pShapes, pItem, 1);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CQuadTree_Structure::Get_Shapes(CSG_Shapes *pShapes, CSG_PRQuadTree_Item *pItem, int Level)
{
	if( pItem )
	{
		if( pShapes->Get_Type() == SHAPE_TYPE_Line )
		{
			if( pItem->is_Node() )
			{
				CSG_Shape	*pShape	= pShapes->Add_Shape();
				pShape->Set_Value(0, Level);
				pShape->Add_Point(pItem->Get_xMin(), pItem->Get_yCenter(), 0);
				pShape->Add_Point(pItem->Get_xMax(), pItem->Get_yCenter(), 0);
				pShape->Add_Point(pItem->Get_xCenter(), pItem->Get_yMin(), 1);
				pShape->Add_Point(pItem->Get_xCenter(), pItem->Get_yMax(), 1);

				for(int i=0; i<4; i++)
				{
					Get_Shapes(pShapes, pItem->asNode()->Get_Child(i), Level + 1);
				}
			}
		}

		if( pShapes->Get_Type() == SHAPE_TYPE_Polygon )
		{
			if( pItem->is_Node() )
			{
				for(int i=0; i<4; i++)
				{
					Get_Shapes(pShapes, pItem->asNode()->Get_Child(i), Level + 1);
				}
			}
			else // if( pItem->is_Leaf() )
			{
				CSG_Shape	*pShape	= pShapes->Add_Shape();
				pShape->Set_Value(0, Level + 1);
				pShape->Set_Value(1, pItem->asLeaf()->Get_Z());
				pShape->Add_Point(pItem->Get_xMin(), pItem->Get_yMin());
				pShape->Add_Point(pItem->Get_xMin(), pItem->Get_yMax());
				pShape->Add_Point(pItem->Get_xMax(), pItem->Get_yMax());
				pShape->Add_Point(pItem->Get_xMax(), pItem->Get_yMin());
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
