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
		_TL(""),
		true
	);

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Duplicated Points"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);
}

//---------------------------------------------------------
bool CQuadTree_Structure::On_Execute(void)
{
	CSG_PRQuadTree	QT(Parameters("SHAPES")->asShapes(), Parameters("ATTRIBUTE")->asInt());

	Get_Shapes(Parameters("POLYGONS")->asShapes(), (CSG_PRQuadTree_Item *)&QT.Get_Root());
	Get_Shapes(Parameters("LINES")   ->asShapes(), (CSG_PRQuadTree_Item *)&QT.Get_Root());
	Get_Shapes(Parameters("POINTS")  ->asShapes(), (CSG_PRQuadTree_Item *)&QT.Get_Root());

	return( true );
}

//---------------------------------------------------------
bool CQuadTree_Structure::Get_Shapes(CSG_Shapes *pShapes, CSG_PRQuadTree_Item *pItem)
{
	CSG_Shape	*pShape;

	if( pShapes && pItem )
	{
		switch( pShapes->Get_Type() )
		{
		default:
			return( false );

		case SHAPE_TYPE_Point:
			pShapes->Create(SHAPE_TYPE_Point  , CSG_String::Format(SG_T("%s [%s, %s]"), Parameters("SHAPES")->asShapes()->Get_Name(), _TL("QuadTree"), _TL("Duplicates")));
			pShapes->Add_Field("COUNT", SG_DATATYPE_Int);
			pShapes->Add_Field("VALUE", SG_DATATYPE_String);
			break;

		case SHAPE_TYPE_Line:
			pShapes->Create(SHAPE_TYPE_Line   , CSG_String::Format(SG_T("%s [%s, %s]"), Parameters("SHAPES")->asShapes()->Get_Name(), _TL("QuadTree"), _TL("Squares")));
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
			pShapes->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s, %s]"), Parameters("SHAPES")->asShapes()->Get_Name(), _TL("QuadTree"), _TL("Leafs")));
			pShapes->Add_Field("LEVEL", SG_DATATYPE_Int);
			pShapes->Add_Field("VALUE", SG_DATATYPE_Double);
			pShapes->Add_Field("COUNT", SG_DATATYPE_Int);
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
		switch( pShapes->Get_Type() )
		{

		//-------------------------------------------------
		case SHAPE_TYPE_Point:
		{
			if( pItem->is_Leaf() )
			{
				CSG_Shape	*pShape	= pShapes->Add_Shape();

				pShape->Add_Point(pItem->asLeaf()->Get_Point());

				if( pItem->has_Statistics() )
				{
					CSG_PRQuadTree_Leaf_List	*pList	= (CSG_PRQuadTree_Leaf_List *)pItem;

					pShape->Set_Value(0, pList->Get_Count());
					pShape->Set_Value(1, SG_Get_String(pList->Get_Value(0), -2).c_str());

					for(int i=1; i<pList->Get_Count(); i++)
					{
						pShape->Set_Value(1, CSG_String::Format(SG_T("%s|%s"), pShape->asString(1),
							SG_Get_String(pList->Get_Value(i), -2).c_str()
						));
					}
				}
				else
				{
					pShape->Set_Value(0, 1);
					pShape->Set_Value(1, SG_Get_String(pItem->asLeaf()->Get_Z(), -2).c_str());
				}
			}
		}
		break;

		//-------------------------------------------------
		case SHAPE_TYPE_Line:
		{
			if( pItem->is_Node() )
			{
				CSG_Shape	*pShape	= pShapes->Add_Shape();
				pShape->Set_Value(0, Level);
				pShape->Add_Point(pItem->Get_xMin(), pItem->Get_yCenter(), 0);
				pShape->Add_Point(pItem->Get_xMax(), pItem->Get_yCenter(), 0);
				pShape->Add_Point(pItem->Get_xCenter(), pItem->Get_yMin(), 1);
				pShape->Add_Point(pItem->Get_xCenter(), pItem->Get_yMax(), 1);
			}
		}
		break;

		//-------------------------------------------------
		case SHAPE_TYPE_Polygon:
		{
			if( pItem->is_Leaf() )
			{
				CSG_Shape	*pShape	= pShapes->Add_Shape();
				pShape->Set_Value(0, Level + 1);
				pShape->Set_Value(1, pItem->asLeaf()->Get_Z());
				pShape->Set_Value(2, pItem->has_Statistics() ? ((CSG_PRQuadTree_Leaf_List *)pItem)->Get_Count() : 1);
				pShape->Add_Point(pItem->Get_xMin(), pItem->Get_yMin());
				pShape->Add_Point(pItem->Get_xMin(), pItem->Get_yMax());
				pShape->Add_Point(pItem->Get_xMax(), pItem->Get_yMax());
				pShape->Add_Point(pItem->Get_xMax(), pItem->Get_yMin());
			}
		}
		break;
		}

		//-------------------------------------------------
		if( pItem->is_Node() )
		{
			for(int i=0; i<4; i++)
			{
				Get_Shapes(pShapes, pItem->asNode()->Get_Child(i), Level + 1);
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
