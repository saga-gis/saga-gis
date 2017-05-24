/**********************************************************
 * Version $Id: polygon_to_edges_nodes.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    shapes_polygons                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               polygon_to_edges_nodes.cpp              //
//                                                       //
//                 Copyright (C) 2011 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include "polygon_to_edges_nodes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_to_Edges_Nodes::CPolygon_to_Edges_Nodes(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Polygons to Edges and Nodes"));

	Set_Author		(SG_T("O. Conrad (c) 2011"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "EDGES"		, _TL("Edges"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "NODES"		, _TL("Nodes"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_to_Edges_Nodes::On_Execute(void)
{
	CSG_Shapes		*pPolygons;

	pPolygons	= Parameters("POLYGONS")	->asShapes();
	m_pEdges	= Parameters("EDGES")		->asShapes();
	m_pNodes	= Parameters("NODES")		->asShapes();

	if(	!m_Vertices.Create(pPolygons, -1) )
	{
		Error_Set(_TL("initialisation error"));

		return( false );
	}

	//-----------------------------------------------------
	m_pEdges->Create(SHAPE_TYPE_Line , CSG_String::Format(SG_T("%s [%s]"), pPolygons->Get_Name(), _TL("Edges")));
	m_pEdges->Add_Field(_TL("ID")    , SG_DATATYPE_Int);
	m_pEdges->Add_Field(_TL("NODE_A"), SG_DATATYPE_Int);
	m_pEdges->Add_Field(_TL("NODE_B"), SG_DATATYPE_Int);
	m_pEdges->Add_Field(_TL("FACE_A"), SG_DATATYPE_Int);
	m_pEdges->Add_Field(_TL("FACE_B"), SG_DATATYPE_Int);

	m_pNodes->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pPolygons->Get_Name(), _TL("Nodes")));
	m_pNodes->Add_Field(_TL("ID")    , SG_DATATYPE_Int);
	m_pNodes->Add_Field(_TL("COUNT") , SG_DATATYPE_Int);
	m_pNodes->Add_Field(_TL("EDGES") , SG_DATATYPE_String);

	m_Nodes.Create(pPolygons->Get_Extent());

	//-----------------------------------------------------
	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape	*pPolygon	= pPolygons->Get_Shape(iPolygon);

		for(int iPart=0, iPoint; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			if( !Find_Node(pPolygon, iPart, iPoint = 0) )
			{
				CSG_Shape	*pEdge	= m_pEdges->Add_Shape();

				for(iPoint=0; iPoint<pPolygon->Get_Point_Count(); iPoint++)
				{
					pEdge->Add_Point(pPolygon->Get_Point(iPoint, iPart));
				}
			}
			else
			{
				do
				{
					Trace_Edge(pPolygon, iPart, iPoint);
				}
				while( Find_Node(pPolygon, iPart, ++iPoint) );
			}
		}
	}

	//-----------------------------------------------------
	m_Vertices	.Destroy();
	m_Nodes		.Destroy();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_to_Edges_Nodes::Find_Node(CSG_Shape *pPolygon, int iPart, int &iPoint)
{
	for( ; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
	{
		double	Distance;

		CSG_PRQuadTree_Leaf	*pLeaf	= m_Vertices.Get_Nearest_Leaf(pPolygon->Get_Point(iPoint, iPart), Distance);

		if( pLeaf->has_Statistics() && ((CSG_PRQuadTree_Leaf_List *)pLeaf)->Get_Count() > 2 )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CPolygon_to_Edges_Nodes::Trace_Edge(CSG_Shape *pPolygon, int iPart, int aPoint)
{
	CSG_Shape	*pEdge	= m_pEdges->Add_Shape();

	for(int iPoint=aPoint; pEdge->Get_Point_Count(0)<pPolygon->Get_Point_Count(iPart); iPoint=iPoint<pPolygon->Get_Point_Count(iPart)-1 ? iPoint+1 : 0)
	{
		double	Distance;

		CSG_PRQuadTree_Leaf	*pLeaf	= m_Vertices.Get_Nearest_Leaf(pPolygon->Get_Point(iPoint, iPart), Distance);

		pEdge->Add_Point(pPolygon->Get_Point(iPoint, iPart));

		if( iPoint != aPoint && pLeaf->has_Statistics() )
		{
			if( ((CSG_PRQuadTree_Leaf_List *)pLeaf)->Get_Count() > 2 )	// Node
			{
				TSG_Point	A	= pPolygon->Get_Point(aPoint, iPart);
				TSG_Point	B	= pPolygon->Get_Point(iPoint, iPart);

				if( pEdge->Get_Point_Count(0) > 2 || (A.x != B.x || A.y != B.y) )
				{
					int		Node_A	= Add_Node(A, pEdge->Get_Index());
					int		Node_B	= Add_Node(B, pEdge->Get_Index());

					pEdge->Set_Value(0, pEdge->Get_Index());
					pEdge->Set_Value(1, Node_A < Node_B ? Node_A : Node_B);
					pEdge->Set_Value(2, Node_A < Node_B ? Node_B : Node_A);
					pEdge->Set_Value(3, pPolygon->Get_Index());

					return( true );
				}

				m_pEdges->Del_Shape(pEdge);

				return( false );
			}
		}
	}

	m_pEdges->Del_Shape(pEdge);

	return( false );
}

//---------------------------------------------------------
int CPolygon_to_Edges_Nodes::Add_Node(TSG_Point &Point, int Edge_ID)
{
	int					Node_ID;
	double				Distance;
	CSG_Shape			*pNode;
	CSG_PRQuadTree_Leaf	*pLeaf	= m_Nodes.Get_Nearest_Leaf(Point, Distance);

	if( !pLeaf || Distance > 0.0 )
	{
		Node_ID	= m_pNodes->Get_Count();

		pNode	= m_pNodes->Add_Shape();
		pNode	->Set_Value(0, Node_ID);
		pNode	->Set_Value(1, 1);
		pNode	->Set_Value(2, CSG_String::Format(SG_T("%d"), Edge_ID));
		pNode	->Set_Value(3, -1);
		pNode	->Add_Point(Point);

		m_Nodes.Add_Point(Point.x, Point.y, Node_ID);
	}
	else
	{
		Node_ID	= (int)pLeaf->Get_Z();

		pNode	= m_pNodes->Get_Shape(Node_ID);
		pNode	->Add_Value(1, 1);
		pNode	->Set_Value(2, CSG_String::Format(SG_T("%s|%d"), pNode->asString(2), Edge_ID));
	}

	return( pNode->Get_Index() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
