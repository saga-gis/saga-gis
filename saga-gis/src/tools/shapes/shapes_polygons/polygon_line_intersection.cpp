/**********************************************************
 * Version $Id: polygon_line_intersection.cpp 911 2011-02-14 16:38:15Z reklov_w $
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
//             polygon_line_intersection.cpp             //
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
#include "polygon_line_intersection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Network_Node
{
	friend class CSG_Network;

public:

	int						Get_ID				(void)		const	{	return( m_ID );		}
	const TSG_Point &		Get_Point			(void)		const	{	return( m_Point );	}

	int						Get_Edge_Count		(void)		const	{	return( m_Edges.Get_Count() );	}
	int						Get_Edge			(int iEdge)	const	{	return( iEdge >= 0 && iEdge < m_Edges.Get_Count() ? m_Edges[iEdge].asInt(0) : -1 );	}
	double					Get_Direction		(int iEdge)	const	{	return( m_Edges[iEdge][1] );	}

	int						Get_Edge_Next		(int iEdge, bool bClockwise = true)	const
	{
		if( Get_Edge_Count() > 1 )
		{
			for(int i=0; i<m_Edges.Get_Count(); i++)
			{
				if( m_Edges.Get_Record_byIndex(i)->asInt(0) == iEdge )
				{
					i	= bClockwise
						? i < m_Edges.Get_Count() - 1 ? i + 1 : 0
						: i > 0 ? i - 1 : m_Edges.Get_Count() - 1;

					return( m_Edges.Get_Record_byIndex(i)->asInt(0) );
				}
			}
		}

		return( -1 );
	}


private:

	int						m_ID;

	TSG_Point				m_Point;

	CSG_Table				m_Edges;


	CSG_Network_Node(int ID, const TSG_Point &Point)
	{
		m_ID	= ID;
		m_Point	= Point;

		m_Edges.Add_Field(SG_T("ID") , SG_DATATYPE_Int);
		m_Edges.Add_Field(SG_T("DIR"), SG_DATATYPE_Double);
	}

	bool					Add_Edge			(int ID, double Direction)
	{
		CSG_Table_Record	*pEdge	= m_Edges.Add_Record();

		pEdge->Set_Value(0, ID);
		pEdge->Set_Value(1, Direction);

		m_Edges.Set_Index(1, TABLE_INDEX_Ascending);

		return( true );
	}

	bool					Del_Edge			(int ID)
	{
		int	n	= 0;

		for(int i=m_Edges.Get_Count()-1; i>=0; i--)
		{
			if( m_Edges[i].asInt(0) == ID )
			{
				m_Edges.Del_Record(i);

				n++;
			}
		}

		if( n > 0 )
		{
			m_Edges.Set_Index(1, TABLE_INDEX_Ascending);

			return( true );
		}

		return( false );
	}

};

//---------------------------------------------------------
class CSG_Network
{
public:
							CSG_Network			(void);
	bool					Create				(void);

							CSG_Network			(CSG_Shapes *pLines);
	bool					Create				(CSG_Shapes *pLines);

	virtual					~CSG_Network		(void);
	bool					Destroy				(void);

	const CSG_Shapes &		Get_Edges			(void)	const	{	return( m_Edges );		}

	int						Get_Node_Count		(void)	const	{	return( (int)m_Nodes.Get_Size() );	}
	CSG_Network_Node &		Get_Node			(int i)	const	{	return( *((CSG_Network_Node **)m_Nodes.Get_Array())[i] );	}

	bool					Add_Shape			(CSG_Shape *pShape);

	bool					Update				(void);
	bool					Remove_End_Nodes	(void);


private:

	CSG_Array				m_Nodes;

	CSG_Shapes				m_Edges;


	void					_On_Construction	(void);

	bool					_Add_Line			(CSG_Shape *pLine, int ID);

	int						_Add_Node			(CSG_PRQuadTree &Search, int Edge_ID, const TSG_Point &Node_Point, const TSG_Point &Dir_Point);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Network::CSG_Network(void)
{
	_On_Construction();

	Create();
}

bool CSG_Network::Create(void)
{
	Destroy();

	return( true );
}

//---------------------------------------------------------
CSG_Network::CSG_Network(CSG_Shapes *pLines)
{
	_On_Construction();

	Create(pLines);
}

bool CSG_Network::Create(CSG_Shapes *pLines)
{
	Destroy();

	if( !pLines || pLines->Get_Type() != SHAPE_TYPE_Line || !pLines->is_Valid() )
	{
		return( false );
	}

	for(int iLine=0; iLine<pLines->Get_Count(); iLine++)
	{
		Add_Shape(pLines->Get_Shape(iLine));
	}

	return( true );
}

//---------------------------------------------------------
CSG_Network::~CSG_Network(void)
{
	Destroy();
}

bool CSG_Network::Destroy(void)
{
	for(int i=0; i<Get_Node_Count(); i++)
	{
		delete(&Get_Node(i));
	}

	m_Nodes.Set_Array(0);

	m_Edges.Del_Records();

	return( true );
}

//---------------------------------------------------------
void CSG_Network::_On_Construction(void)
{
	m_Nodes.Create(sizeof(CSG_Network_Node **), 0, SG_ARRAY_GROWTH_1);

	m_Edges.Create(SHAPE_TYPE_Line , SG_T("EDGES"));
	m_Edges.Add_Field(SG_T("ID")		, SG_DATATYPE_Int);
	m_Edges.Add_Field(SG_T("NODE_A")	, SG_DATATYPE_Int);
	m_Edges.Add_Field(SG_T("NODE_B")	, SG_DATATYPE_Int);
	m_Edges.Add_Field(SG_T("SHAPE_TYPE"), SG_DATATYPE_Int);
	m_Edges.Add_Field(SG_T("PROCESSED")	, SG_DATATYPE_Int);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Network::Add_Shape(CSG_Shape *pShape)
{
	if( !pShape || !pShape->is_Valid() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	Part(SHAPE_TYPE_Line);
	CSG_Shape	*pPart	= Part.Add_Shape();

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( pShape->Get_Point_Count(iPart) > 1 )
		{
			bool	bAscending	= pShape->Get_Type() != SHAPE_TYPE_Polygon
				|| ((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) != ((CSG_Shape_Polygon *)pShape)->is_Clockwise(iPart);

			CSG_Point	q, p	= pShape->Get_Point(0, iPart, bAscending);

			pPart->Add_Point(p);

			for(int iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				if( !p.is_Equal(q = pShape->Get_Point(iPoint, iPart, bAscending)) )
				{
					p	= q;

					pPart->Add_Point(p);
				}
			}

			if( pPart->Get_Point_Count(0) > 1 )
			{
				_Add_Line(pPart, pShape->Get_Type());
			}

			pPart->Del_Parts();
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Network::_Add_Line(CSG_Shape *pLine, int ID)
{
	int			iEdge, iPoint, iCrossing;
	CSG_Shape	*pEdge, *pCrossing;
	CSG_Shapes	Crossings(SHAPE_TYPE_Point);

	//-----------------------------------------------------
	// 1. find crossings

	Crossings.Add_Field(SG_T("LINE_POINT")	, SG_DATATYPE_Int);
	Crossings.Add_Field(SG_T("EDGE_ID")		, SG_DATATYPE_Int);
	Crossings.Add_Field(SG_T("EDGE_POINT")	, SG_DATATYPE_Int);
	Crossings.Add_Field(SG_T("EDGE_DIST")	, SG_DATATYPE_Double);

	for(iEdge=0; iEdge<m_Edges.Get_Count(); iEdge++)
	{
		pEdge	= m_Edges.Get_Shape(iEdge);

		if( pEdge->Intersects(pLine) )
		{
			TSG_Point	a	= pEdge->Get_Point(0);

			for(int iEdge_Point=1; iEdge_Point<pEdge->Get_Point_Count(0); iEdge_Point++)
			{
				TSG_Point	b	= a;	a	= pEdge->Get_Point(iEdge_Point);
				TSG_Point	A	= pLine->Get_Point(0);

				for(iPoint=1; iPoint<pLine->Get_Point_Count(0); iPoint++)
				{
					TSG_Point	C, B	= A;	A	= pLine->Get_Point(iPoint);

					if( SG_Get_Crossing(C, A, B, a, b) )
					{
						pCrossing	= Crossings.Add_Shape();
						pCrossing->Add_Point(C);
						pCrossing->Set_Value(0, iPoint);
						pCrossing->Set_Value(1, iEdge);
						pCrossing->Set_Value(2, iEdge_Point);
						pCrossing->Set_Value(3, SG_Get_Distance(C, b));
					}
				}
			}
		}
	}


	//-----------------------------------------------------
	// 2. add new line's vertices

	Crossings.Set_Index(0, TABLE_INDEX_Ascending);

	pEdge		= m_Edges.Add_Shape();
	pEdge		->Set_Value(3, ID);

	for(iCrossing=0, iPoint=0; iCrossing<Crossings.Get_Count(); iCrossing++)
	{
		pCrossing	= Crossings.Get_Shape_byIndex(iCrossing);

		while( iPoint < pCrossing->asInt(0) )
		{
			pEdge->Add_Point(pLine->Get_Point(iPoint++));
		}

		pEdge->Add_Point(pCrossing->Get_Point(0));

		pEdge		= m_Edges.Add_Shape();
		pEdge		->Set_Value(3, ID);
		pEdge		->Add_Point(pCrossing->Get_Point(0));
	}

	while( iPoint < pLine->Get_Point_Count(0) )
	{
		pEdge->Add_Point(pLine->Get_Point(iPoint++));
	}


	//-----------------------------------------------------
	// 3. split edges, if necessary

	Crossings.Set_Index(1, TABLE_INDEX_Descending, 2, TABLE_INDEX_Ascending, 3, TABLE_INDEX_Ascending);

	for(iCrossing=0; iCrossing<Crossings.Get_Count(); )
	{
		pCrossing	= Crossings.Get_Shape_byIndex(iCrossing);
		iEdge		= pCrossing->asInt(1);
		pLine		= m_Edges.Get_Shape(iEdge);
		ID			= pLine->asInt(0);
		iPoint		= 0;
		pEdge		= m_Edges.Add_Shape();
		pEdge		->Set_Value(3, pLine->asInt(3));

		while( 1 )
		{
			while( iPoint < pCrossing->asInt(2) )
			{
				pEdge->Add_Point(pLine->Get_Point(iPoint++));
			}

			pEdge->Add_Point(pCrossing->Get_Point(0));

			if( ++iCrossing < Crossings.Get_Count() && iEdge == Crossings.Get_Shape_byIndex(iCrossing)->asInt(1) )
			{
				pEdge		= m_Edges.Add_Shape();
				pEdge		->Set_Value(3, pLine->asInt(3));

				pEdge->Add_Point(pCrossing->Get_Point(0));

				pCrossing	= Crossings.Get_Shape_byIndex(iCrossing);
			}
			else
			{
				if( iPoint < pLine->Get_Point_Count() )
				{
					pEdge		= m_Edges.Add_Shape();
					pEdge		->Set_Value(3, pLine->asInt(3));
		
					pEdge->Add_Point(pCrossing->Get_Point(0));

					while( iPoint < pLine->Get_Point_Count() )
					{
						pEdge->Add_Point(pLine->Get_Point(iPoint++));
					}
				}

				break;
			}
		}

		m_Edges.Del_Shape(iEdge);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Network::Update(void)
{
	int		iEdge;

	//-----------------------------------------------------
	for(iEdge=m_Edges.Get_Count()-1; iEdge>=0; iEdge--)
	{
		CSG_Shape	*pEdge	= m_Edges.Get_Shape(iEdge);

		if( !(((CSG_Shape_Line *)pEdge)->Get_Length() > 0.0) )
		{
			m_Edges.Del_Shape(iEdge);
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<Get_Node_Count(); i++)
	{
		delete(&Get_Node(i));
	}

	m_Nodes.Set_Array(0);

	//-----------------------------------------------------
	CSG_PRQuadTree	Search(m_Edges.Get_Extent());

	for(iEdge=0; iEdge<m_Edges.Get_Count(); iEdge++)
	{
		CSG_Shape	*pEdge	= m_Edges.Get_Shape(iEdge);

		pEdge->Set_Value(0, iEdge);

		pEdge->Set_Value(1, _Add_Node(Search, iEdge,
			pEdge->Get_Point(0),
			pEdge->Get_Point(1)
		));

		pEdge->Set_Value(2, _Add_Node(Search, iEdge,
			pEdge->Get_Point(pEdge->Get_Point_Count(0) - 1),
			pEdge->Get_Point(pEdge->Get_Point_Count(0) - 2)
		));
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
int CSG_Network::_Add_Node(CSG_PRQuadTree &Search, int Edge_ID, const TSG_Point &Node_Point, const TSG_Point &Dir_Point)
{
	int					Node_ID;
	double				Distance;
	CSG_PRQuadTree_Leaf	*pLeaf	= Search.Get_Nearest_Leaf(Node_Point, Distance);

	if( !pLeaf || Distance > 0.0 )//00001 )
	{
		Node_ID	= Get_Node_Count();

		m_Nodes.Inc_Array();

		((CSG_Network_Node **)m_Nodes.Get_Array())[Node_ID]	= new CSG_Network_Node(Node_ID, Node_Point);

		Search.Add_Point(Node_Point.x, Node_Point.y, Node_ID);
	}
	else
	{
		Node_ID	= (int)pLeaf->Get_Z();
	}

	Get_Node(Node_ID).Add_Edge(Edge_ID, SG_Get_Angle_Of_Direction(Node_Point, Dir_Point));

	return( Node_ID );
}

//---------------------------------------------------------
bool CSG_Network::Remove_End_Nodes(void)
{
	int		iEdge, n;

	//-----------------------------------------------------
	for(iEdge=0; iEdge<m_Edges.Get_Count(); iEdge++)
	{
		CSG_Shape	*pEdge	= m_Edges.Get_Shape(iEdge);

		if( pEdge->asInt(3) == SHAPE_TYPE_Line )
		{
			bool	bRemove	= false;

			for(int iNode=1; iNode<=2 && !bRemove; iNode++)
			{
				CSG_Network_Node	&Node	= Get_Node(pEdge->asInt(iNode));
				CSG_Point			Point	= Node.Get_Point();
				CSG_Shape			*pNext;

				if(	(	(pNext = m_Edges.Get_Shape(Node.Get_Edge_Next(pEdge->asInt(0),  true))) != NULL
						&&	pNext->asInt(3) == SHAPE_TYPE_Polygon
						&&	!Point.is_Equal(pNext->Get_Point(0, 0, false)) )
				||	(	(pNext = m_Edges.Get_Shape(Node.Get_Edge_Next(pEdge->asInt(0), false))) != NULL
						&&	pNext->asInt(3) == SHAPE_TYPE_Polygon
						&&	!Point.is_Equal(pNext->Get_Point(0, 0,  true)) ) )
				{
					bRemove	= true;
				}
			}

			if( bRemove )
			{
				Get_Node(pEdge->asInt(1)).Del_Edge(pEdge->asInt(0));
				Get_Node(pEdge->asInt(2)).Del_Edge(pEdge->asInt(0));

				pEdge->Set_Value(4, 1);
			}
		}
	}

	//-----------------------------------------------------
	do
	{
		for(n=0, iEdge=0; iEdge<m_Edges.Get_Count(); iEdge++)
		{
			CSG_Shape	*pEdge	= m_Edges.Get_Shape(iEdge);

			if( pEdge->asInt(3) == SHAPE_TYPE_Line && pEdge->asInt(4) == 0 )
			{
				if(	Get_Node(pEdge->asInt(1)).Get_Edge_Count() <= 1
				||	Get_Node(pEdge->asInt(2)).Get_Edge_Count() <= 1 )
				{
					Get_Node(pEdge->asInt(1)).Del_Edge(pEdge->asInt(0));
					Get_Node(pEdge->asInt(2)).Del_Edge(pEdge->asInt(0));

					pEdge->Set_Value(4, 1);

					n++;
				}
			}
		}
	}
	while( n > 0 );

	//-----------------------------------------------------
	for(iEdge=m_Edges.Get_Count()-1; iEdge>=0; iEdge--)
	{
		if( m_Edges[iEdge][4] )
		{
			m_Edges.Del_Shape(iEdge);
		}
	}

	//-----------------------------------------------------
	return( Update() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Line_Intersection::CPolygon_Line_Intersection(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Polygon-Line Intersection"));

	Set_Author		(SG_T("O. Conrad (c) 2011"));

	Set_Description	(_TW(
		"Polygon-line intersection. Splits polygons with lines. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "INTERSECT"	, _TL("Intersection"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Line_Intersection::On_Execute(void)
{
	CSG_Shapes	*pPolygons;

	//--------------------------------------------------------
	pPolygons		= Parameters("POLYGONS")	->asShapes();
	m_pLines		= Parameters("LINES")		->asShapes();
	m_pIntersection	= Parameters("INTERSECT")	->asShapes();

	//--------------------------------------------------------
	if(	!m_pLines ->is_Valid() || m_pLines ->Get_Count() < 1
	||	!pPolygons->is_Valid() || pPolygons->Get_Count() < 1
	||	m_pLines->Get_Extent().Intersects(pPolygons->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("no shapes for intersection found"));

		return( false );
	}

	//--------------------------------------------------------
	m_pIntersection->Create(SHAPE_TYPE_Polygon,
		CSG_String::Format(SG_T("%s [%s: %s]"), pPolygons->Get_Name(), _TL("Intersection"), m_pLines->Get_Name()),
		pPolygons
	);

	//--------------------------------------------------------
	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		if( !Get_Intersection((CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon)) )
		{
			m_pIntersection->Add_Shape(pPolygons->Get_Shape(iPolygon));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool Trace_Polygon(CSG_Shape *pPolygon, CSG_Network &Network, int iEdge)
{
	bool		bAscending	= true;
	CSG_Shape	*pEdge		= Network.Get_Edges().Get_Shape(iEdge);

	if( pEdge->asInt(3) == SHAPE_TYPE_Polygon )
	{
		if( pEdge->asInt(4) )
		{
			return( false );
		}

		bAscending	= true;
	}
	else if( (pEdge->asInt(4) & 0x1) == 0 )
	{
		bAscending	= true;
	}
	else if( (pEdge->asInt(4) & 0x2) == 0 )
	{
		bAscending	= false;
	}
	else
	{
		return( false );
	}

	while( pEdge != NULL )
	{
		pEdge->Set_Value(4, pEdge->asInt(4) | (bAscending ? 0x1 : 0x2));

		for(int iPoint=0; iPoint<pEdge->Get_Point_Count(0); iPoint++)
		{
			pPolygon->Add_Point(pEdge->Get_Point(iPoint, 0, bAscending));
		}

		int	End_Node	= pEdge->asInt(bAscending ? 2 : 1);

		iEdge		= Network.Get_Node(End_Node).Get_Edge_Next(iEdge, false);
		pEdge		= Network.Get_Edges().Get_Shape(iEdge);

		if( pEdge )
		{
			bAscending	= pEdge->asInt(3) == SHAPE_TYPE_Polygon || End_Node == pEdge->asInt(1);

			if( (pEdge->asInt(4) & (bAscending ? 0x1 : 0x2)) )
			{
				pEdge	= NULL;
			}
		}
	}

	return( pPolygon->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Line_Intersection::Get_Intersection(CSG_Shape_Polygon *pPolygon)
{
	CSG_Network	Network;

	for(int iLine=0; iLine<m_pLines->Get_Count(); iLine++)
	{
		CSG_Shape	*pLine	= m_pLines->Get_Shape(iLine);

		if( pLine->Intersects(pPolygon) )
		{
			Network.Add_Shape(pLine);
		}
	}

	if( Network.Get_Edges().Get_Count() == 0 )
	{
		return( false );
	}

	Network.Add_Shape(pPolygon);
	Network.Update();
	Network.Remove_End_Nodes();

	//-----------------------------------------------------
	int			iEdge, iPolygon;
	CSG_Shapes	Intersection(SHAPE_TYPE_Polygon);

	Intersection.Add_Field(SG_T("ID"), SG_DATATYPE_Int);

	for(iEdge=0; iEdge<Network.Get_Edges().Get_Count(); iEdge++)
	{
		CSG_Shape	*pEdge	= Network.Get_Edges().Get_Shape(iEdge);

		if( pEdge->asInt(3) == SHAPE_TYPE_Polygon )
		{
			Trace_Polygon(Intersection.Add_Shape(), Network, iEdge);
		}
		else if( pPolygon->Contains(pEdge->Get_Point(0)) && pPolygon->Contains(pEdge->Get_Point(pEdge->Get_Point_Count(0) - 1)) )
		{
			Trace_Polygon(Intersection.Add_Shape(), Network, iEdge);
			Trace_Polygon(Intersection.Add_Shape(), Network, iEdge);
		}
	}

	//-----------------------------------------------------
	for(iPolygon=0; iPolygon<Intersection.Get_Count(); iPolygon++)	// 1. outer rings
	{
		CSG_Shape	*pIntersect	= Intersection.Get_Shape(iPolygon);

		if( pIntersect->Get_Point_Count() > 0 && ((CSG_Shape_Polygon *)pIntersect)->is_Clockwise(0) == true )
		{
			pIntersect->Set_Value(0, m_pIntersection->Get_Count());

			((CSG_Table_Record *)m_pIntersection->Add_Shape(pIntersect, SHAPE_COPY_GEOM))->Assign(pPolygon);
		}
	}

	for(iPolygon=0; iPolygon<Intersection.Get_Count(); iPolygon++)	// 2. inner rings
	{
		CSG_Shape	*pIntersect	= Intersection.Get_Shape(iPolygon);

		if( pIntersect->Get_Point_Count() > 0 && ((CSG_Shape_Polygon *)pIntersect)->is_Clockwise(0) == false )
		{
			for(int j=0; j<Intersection.Get_Count(); j++)
			{
				if( ((CSG_Shape_Polygon *)Intersection.Get_Shape(j))->Contains(pIntersect->Get_Point(0)) )
				{
					CSG_Shape	*pShape	= m_pIntersection->Get_Shape(Intersection[j].asInt(0));

					for(int iPoint=0, iPart=pShape->Get_Part_Count(); iPoint<pIntersect->Get_Point_Count(0); iPoint++)
					{
						pShape->Add_Point(pIntersect->Get_Point(iPoint), iPart);
					}

					break;
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

/*/---------------------------------------------------------
{
	//-----------------------------------------------------
	CSG_Shapes	p(SHAPE_TYPE_Point);

	p.Add_Field(SG_T("ID")   , SG_DATATYPE_Int);
	p.Add_Field(SG_T("COUNT"), SG_DATATYPE_Int);

	for(int i=0; i<Network.Get_Node_Count(); i++)
	{
		CSG_Shape	*pNode	= p.Add_Shape();
			
		pNode->Add_Point(Network.Get_Node(i).Get_Point());
		pNode->Set_Value(0, Network.Get_Node(i).Get_ID());
		pNode->Set_Value(1, Network.Get_Node(i).Get_Edge_Count());
	}

	DataObject_Add(SG_Create_Shapes(p));

	DataObject_Add(SG_Create_Shapes(Network.Get_Edges()));

	//-----------------------------------------------------
	CSG_Shapes	l(SHAPE_TYPE_Line);

	l.Add_Field(SG_T("ID")   , SG_DATATYPE_Int);
	l.Add_Field(SG_T("COUNT"), SG_DATATYPE_Int);
	l.Add_Field(SG_T("EDGE") , SG_DATATYPE_Int);
	l.Add_Field(SG_T("DIR")  , SG_DATATYPE_Double);
	l.Add_Field(SG_T("ORDER"), SG_DATATYPE_Int);

	for(int i=0; i<Network.Get_Node_Count(); i++)
	{
		TSG_Point	P	= Network.Get_Node(i).Get_Point();

		for(int j=0; j<Network.Get_Node(i).Get_Edge_Count(); j++)
		{
			CSG_Shape	*pNode	= l.Add_Shape();

			pNode->Set_Value(0, Network.Get_Node(i).Get_ID());
			pNode->Set_Value(1, Network.Get_Node(i).Get_Edge_Count());
			pNode->Set_Value(2, Network.Get_Node(i).Get_Edge(j));
			pNode->Set_Value(3, Network.Get_Node(i).Get_Direction(j) * M_RAD_TO_DEG);
			pNode->Set_Value(4, j);
			pNode->Add_Point(P);
			pNode->Add_Point(
				P.x	+ 10.0 * sin(Network.Get_Node(i).Get_Direction(j)),
				P.y	+ 10.0 * cos(Network.Get_Node(i).Get_Direction(j))
			);
		}
	}

	//-----------------------------------------------------
	CSG_Shapes	p(SHAPE_TYPE_Point);

	p.Add_Field(SG_T("COUNT"), SG_DATATYPE_Int);

	for(int i=0; i<Network.Get_Node_Count(); i++)
	{
		CSG_Shape	*pNode	= p.Add_Shape();
			
		pNode->Add_Point(Network.Get_Node(i).Get_Point());
		pNode->Set_Value(0, Network.Get_Node(i).Get_Edge_Count());
	}

	DataObject_Add(SG_Create_Shapes(p));
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
