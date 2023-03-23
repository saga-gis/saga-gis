
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
#include "polygon_line_intersection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Arcs
{
public:

	CSG_Arcs(void);
	CSG_Arcs(CSG_Shape_Polygon *pPolygon);

	bool					Create					(CSG_Shape_Polygon *pPolygon);

	bool					Destroy					(void);

	bool					is_Valid				(void)	const	{ return( m_pPolygon && m_pPolygon->is_Valid() );	}

	bool					Set_Lines				(CSG_Shapes     *pLines);
	bool					Set_Line				(CSG_Shape_Part *pLine );

	bool					Get_Intersection		(CSG_Shapes *pPolygons, CSG_Table_Record *pAttributes, bool bSplitParts);


private:

	enum
	{
		NODE_ID = 0,
		NODE_PART,
		NODE_POINT,
		NODE_DISTANCE
	};

	enum
	{
		ARC_ID = 0,
		ARC_PASSES,
		ARC_NODE_START,
		ARC_NODE_END
	};

	enum
	{
		VERTEX_DISTANCE = 0,
		VERTEX_CROSSING,
		VERTEX_PART,
		VERTEX_POINT
	};


	int						m_nNodes;

	CSG_Shapes				m_Nodes, m_Arcs, m_Polygon;

	CSG_Shape_Polygon		*m_pPolygon;


	void					_On_Construction		(void);

	bool					_Add_Line				(CSG_Shape_Part *pLine);
	int						_Add_Line_Segment		(const CSG_Point Segment[2], double Distance, CSG_Shapes &Vertices);
	bool					_Add_Line_Intersection	(CSG_Shapes &Vertices, int &iVertex);

	int						_Add_Node				(const CSG_Point &Point, int Polygon_Part, int Polygon_Point);

	bool					_Split_Polygon			(void);
	bool					_Split_Arc				(const CSG_Shape &Node, sLong Polygon_Offset);

	bool					_Check_Arc				(CSG_Shape_Line *pArc);

	bool					_Collect_Add_Next		(CSG_Shape_Part    &Polygon, int &ID_Arc, int &ID_Node);
	bool					_Collect_Get_Polygon	(CSG_Shape_Polygon &Polygon);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Arcs::CSG_Arcs(void)
{
	_On_Construction();
}

CSG_Arcs::CSG_Arcs(CSG_Shape_Polygon *pPolygon)
{
	_On_Construction();

	Create(pPolygon);
}

//---------------------------------------------------------
bool CSG_Arcs::Destroy(void)
{
	m_nNodes   = 0;
	m_pPolygon = NULL; m_Polygon.Destroy();

	m_Nodes.Del_Shapes();
	m_Arcs .Del_Shapes();

	return( true );
}

//---------------------------------------------------------
void CSG_Arcs::_On_Construction(void)
{
	m_nNodes   = 0;
	m_pPolygon = NULL;

	m_Nodes.Create(SHAPE_TYPE_Point);
	m_Nodes.Add_Field("ID"   , SG_DATATYPE_Int   );
	m_Nodes.Add_Field("PART" , SG_DATATYPE_Int   );
	m_Nodes.Add_Field("POINT", SG_DATATYPE_Int   );
	m_Nodes.Add_Field("DIST" , SG_DATATYPE_Double);

	m_Arcs .Create(SHAPE_TYPE_Line);
	m_Arcs .Add_Field("ID"    , SG_DATATYPE_Int  );
	m_Arcs .Add_Field("PASSES", SG_DATATYPE_Char );
	m_Arcs .Add_Field("NODE_A", SG_DATATYPE_Int  );
	m_Arcs .Add_Field("NODE_B", SG_DATATYPE_Int  );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Arcs::Create(CSG_Shape_Polygon *pPolygon)
{
	Destroy();

	if( pPolygon && pPolygon->is_Valid() )
	{
		m_Polygon.Create(SHAPE_TYPE_Polygon); // create a clean polygon copy, no duplicates except first ring vertex has to be the same as last, direction...

		m_pPolygon = m_Polygon.Add_Shape()->asPolygon();

		for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			if( pPolygon->Get_Area(iPart) > 0. ) // just in case, skip invalid parts
			{
				bool bAscending = pPolygon->is_Lake(iPart) != pPolygon->is_Clockwise(iPart);

				CSG_Point P, Pfirst = pPolygon->Get_Point(0, iPart, bAscending);

				for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
				{
					CSG_Point Pnext = pPolygon->Get_Point(iPoint, iPart, bAscending);

					if( P != Pnext ) // skip duplicates
					{
						m_pPolygon->Add_Point(P = Pnext, iPart);
					}
				}

				if( P != Pfirst )
				{
					m_pPolygon->Add_Point(Pfirst, iPart); // make sure that last vertex equals first (close the polygon)
				}
			}
		}

		return( m_pPolygon->is_Valid() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Arcs::Set_Lines(CSG_Shapes *pLines)
{
	if( m_pPolygon && m_pPolygon->is_Valid() && m_pPolygon->Get_Extent().Intersects(pLines->Get_Extent()) )
	{
		bool bIntersects = false;

		for(sLong iLine=0; iLine<pLines->Get_Count(); iLine++)
		{
			CSG_Shape_Line *pLine = pLines->Get_Shape(iLine)->asLine();

			if( pLine->Intersects(m_pPolygon) )
			{
				for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
				{
					if( _Add_Line(pLine->Get_Part(iPart)) )
					{
						bIntersects = true;
					}
				}
			}
		}

		return( bIntersects && _Split_Polygon() );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Arcs::Set_Line(CSG_Shape_Part *pLine)
{
	if( m_pPolygon && m_pPolygon->is_Valid() && m_pPolygon->Get_Extent().Intersects(pLine->Get_Extent()) )
	{
		return( _Add_Line(pLine) && _Split_Polygon() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Arcs::_Add_Line(CSG_Shape_Part *pLine)
{
	if( pLine->Get_Count() < 2 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes Vertices(SHAPE_TYPE_Point);

	Vertices.Add_Field("DISTANCE", SG_DATATYPE_Double);
	Vertices.Add_Field("CROSSING", SG_DATATYPE_Char  );
	Vertices.Add_Field("PART"    , SG_DATATYPE_Int   );
	Vertices.Add_Field("POINT"   , SG_DATATYPE_Int   );

	CSG_Point Segment[2]; Segment[1] = pLine->Get_Point(0);

	double Distance = 0.; int nCrossings = 0;

	for(int iPoint=1; iPoint<pLine->Get_Count(); iPoint++)
	{
		Segment[0] = Segment[1]; Segment[1] = pLine->Get_Point(iPoint);

		if( Segment[0].x != Segment[1].x || Segment[0].y != Segment[1].y )
		{
			nCrossings += _Add_Line_Segment(Segment, Distance, Vertices);

			Distance   += SG_Get_Distance(Segment[0].x, Segment[0].y, Segment[1].x, Segment[1].y);
		}
	}

	if( nCrossings < 2 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int nAdded = 0;

	Vertices.Set_Index(VERTEX_DISTANCE, TABLE_INDEX_Ascending);

	for(int iVertex=0; iVertex<Vertices.Get_Count(); )
	{
		if( _Add_Line_Intersection(Vertices, iVertex) )
		{
			nAdded++;
		}
	}

	return( nAdded > 0 );
}

//---------------------------------------------------------
int CSG_Arcs::_Add_Line_Segment(const CSG_Point S[2], double Distance, CSG_Shapes &Vertices)
{
	#define Add_Vertex(p, distance, crossing, part, point) { CSG_Shape &v = *Vertices.Add_Shape(); v.Add_Point(p);\
		v.Set_Value(VERTEX_DISTANCE, distance);\
		v.Set_Value(VERTEX_CROSSING, crossing);\
		v.Set_Value(VERTEX_PART    , part    );\
		v.Set_Value(VERTEX_POINT   , point   );\
	}

	int nCrossings = 0; bool bAddFirst = true;

	for(int iPart=0; iPart<m_pPolygon->Get_Part_Count(); iPart++)
	{
		TSG_Point A = m_pPolygon->Get_Point(m_pPolygon->Get_Point_Count(iPart) - 1, iPart);

		for(int iPoint=0; iPoint<m_pPolygon->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point C, B = A; A = m_pPolygon->Get_Point(iPoint, iPart);

			if( SG_Get_Crossing(C, A, B, S[0], S[1]) && S[1] != C )
			{
				nCrossings++;

				Add_Vertex(C, Distance + SG_Get_Distance(C, S[0]), 1, iPart, iPoint);

				if( S[0] == C )
				{
					bAddFirst = false;
				}
			}
		}
	}

	if( bAddFirst && m_pPolygon->Contains(S[0]) )
	{
		Add_Vertex(S[0], Distance, 0., -1, -1);
	}

	return( nCrossings );
}

//---------------------------------------------------------
bool CSG_Arcs::_Add_Line_Intersection(CSG_Shapes &Vertices, int &iVertex)
{
	if( Vertices.Get_Shape_byIndex(iVertex)->asInt(VERTEX_CROSSING) != 1 ) // not starting with a crossing, lets find the first entering one...
	{
		for(; iVertex<Vertices.Get_Count(); iVertex++)
		{
			CSG_Shape &Vertex = *Vertices.Get_Shape_byIndex(iVertex);

			if( Vertex.asInt(VERTEX_CROSSING) == 1 ) // crossing leaving the polygon
			{
				iVertex++;

				return( false );
			}
		}

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shape &First = *Vertices.Get_Shape_byIndex(iVertex);

	CSG_Shape *pArc = m_Arcs.Add_Shape();

	pArc->Set_Value(ARC_ID    , m_Arcs.Get_Count()); // ID
	pArc->Set_Value(ARC_PASSES,                  2); // PASSES => ...needs to be processed twice!

	for(; iVertex<Vertices.Get_Count(); iVertex++)
	{
		CSG_Shape &Vertex = *Vertices.Get_Shape_byIndex(iVertex);

		pArc->Add_Point(Vertex.Get_Point(0));

		if( Vertex.asInt(VERTEX_CROSSING) == 1 && pArc->Get_Point_Count() > 1 ) // crossing leaving the polygon
		{
			pArc->Set_Value(ARC_NODE_START, _Add_Node(First .Get_Point(0), First .asInt(VERTEX_PART), First .asInt(VERTEX_POINT)));
			pArc->Set_Value(ARC_NODE_END  , _Add_Node(Vertex.Get_Point(0), Vertex.asInt(VERTEX_PART), Vertex.asInt(VERTEX_POINT)));

			iVertex++;

			return( true );
		}
	}

	m_Arcs.Del_Shape(pArc); // sketch did not finish with a leaving crossing!

	return( false );
}

//---------------------------------------------------------
int CSG_Arcs::_Add_Node(const CSG_Point &Point, int Polygon_Part, int Polygon_Point)
{
	if( Polygon_Part  < 0 || Polygon_Part  >= m_pPolygon->Get_Part_Count()
	||  Polygon_Point < 0 || Polygon_Point >= m_pPolygon->Get_Point_Count(Polygon_Part) )
	{
		return( -1 ); // should never happen!
	}

	CSG_Shape &Node = *m_Nodes.Add_Shape();

	Node.Add_Point(Point);

	Node.Set_Value(NODE_ID      , ++m_nNodes);
	Node.Set_Value(NODE_PART    , Polygon_Part);
	Node.Set_Value(NODE_POINT   , Polygon_Point);
	Node.Set_Value(NODE_DISTANCE, SG_Get_Distance(Point, m_pPolygon->Get_Point(Polygon_Point, Polygon_Part)));

	return( Node.asInt(0) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Arcs::_Split_Polygon(void)
{
	if( m_Nodes.Get_Count() < 2 )
	{
		return( false );
	}

	//-----------------------------------------------------
	sLong Polygon_Offset = m_Arcs.Get_Count();

	for(int iPart=0; iPart<m_pPolygon->Get_Part_Count(); iPart++)
	{
		CSG_Shape &Arc = *m_Arcs.Add_Shape();

		Arc.Set_Value(ARC_ID        , m_Arcs.Get_Count());
		Arc.Set_Value(ARC_PASSES    ,                 -1); // needs to be processed only once!
		Arc.Set_Value(ARC_NODE_START,         ++m_nNodes);
		Arc.Set_Value(ARC_NODE_END  ,           m_nNodes); // start node == end node => polygon ring is 'self-connecting'

		for(int iPoint=0; iPoint<m_pPolygon->Get_Point_Count(iPart); iPoint++)
		{
			Arc.Add_Point(m_pPolygon->Get_Point(iPoint, iPart));
		}
	}

	//-----------------------------------------------------
	m_Nodes.Set_Index(
		NODE_PART    , TABLE_INDEX_Descending,
		NODE_POINT   , TABLE_INDEX_Descending,
		NODE_DISTANCE, TABLE_INDEX_Ascending
	);

	for(int iNode=0; iNode<m_Nodes.Get_Count(); iNode++)
	{
		_Split_Arc(*m_Nodes.Get_Shape_byIndex(iNode), Polygon_Offset);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Arcs::_Split_Arc(const CSG_Shape &Node, sLong Polygon_Offset)
{
//	SG_UI_Msg_Add(CSG_String::Format("\n%d, %d, %f", Node.asInt(1), Node.asInt(2), Node.asDouble(3)), false);

	CSG_Point Point = Node.Get_Point(0); int SplitBefore = Node.asInt(2);

	CSG_Shape *pArc = m_Arcs.Get_Shape((sLong)Node.asInt(1) + Polygon_Offset);
	CSG_Shape *pNew = m_Arcs.Add_Shape(pArc, SHAPE_COPY_ATTR);

	pNew->Set_Value(ARC_ID, m_Arcs.Get_Count());

	if( Point != pArc->Get_Point(SplitBefore) )
	{
		pNew->Add_Point(Point);
	}

	for(int i=SplitBefore; i<pArc->Get_Point_Count(); i++)
	{
		pNew->Add_Point(pArc->Get_Point(i));
	}

	pNew->Set_Value(ARC_NODE_START, Node.asInt(0));

	//-----------------------------------------------------
	for(int i=pArc->Get_Point_Count()-1; i>=SplitBefore; i--)
	{
		pArc->Del_Point(i);
	}

	if( Point != pArc->Get_Point(0, 0, false) )
	{
		pArc->Add_Point(Point);
	}

	pArc->Set_Value(ARC_NODE_END, Node.asInt(0));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Arcs::_Check_Arc(CSG_Shape_Line *pArc)
{
	for(int i=pArc->Get_Point_Count(0)-1; i>0; i--) // remove duplicates
	{
		CSG_Point A = pArc->Get_Point(i    , 0);
		CSG_Point B = pArc->Get_Point(i - 1, 0);

		if( A == B )
		{
			pArc->Del_Point(i, 0);
		}
	}

	return( pArc->Get_Point_Count(0) > 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Arcs::_Collect_Add_Next(CSG_Shape_Part &Polygon, int &ID_Arc, int &ID_Node)
{
	if( ID_Node >= 0 )
	{
		CSG_Shape *pNext = NULL; bool bAscending = true;

		double minDiff = M_PI_360, Direction = SG_Get_Angle_Of_Direction(Polygon.Get_Point(0, false), Polygon.Get_Point(1, false));

		for(int i=0; i<m_Arcs.Get_Count(); i++)
		{
			CSG_Shape *pArc = m_Arcs.Get_Shape(i); 

			if( ID_Arc != pArc->asInt(ARC_ID) )
			{
				if     ( ID_Node == pArc->asInt(ARC_NODE_START) ) // starting node
				{
					double d = Direction - SG_Get_Angle_Of_Direction(pArc->Get_Point(0, 0,  true), pArc->Get_Point(1, 0,  true)); if( d < 0. ) d += M_PI_360;

					if( d > 0. && (!pNext || minDiff > d) )
					{
						minDiff = d; pNext = pArc; bAscending =  true;
					}
				}
				else if( ID_Node == pArc->asInt(ARC_NODE_END) ) // ending node
				{
					double d = Direction - SG_Get_Angle_Of_Direction(pArc->Get_Point(0, 0, false), pArc->Get_Point(1, 0, false)); if( d < 0. ) d += M_PI_360;

					if( d > 0. && (!pNext || minDiff > d) )
					{
						minDiff = d; pNext = pArc; bAscending = false;
					}
				}
			}
		}

		if( pNext )
		{
			ID_Arc = pNext->asInt(ARC_ID); ID_Node = pNext->asInt(bAscending ? ARC_NODE_END : ARC_NODE_START);

			for(int i=1; i<pNext->Get_Point_Count(0); i++)
			{
				Polygon.Add_Point(pNext->Get_Point(i, 0, bAscending));
			}

			if( pNext->asInt(ARC_PASSES) > 1 ) // PASSES
			{
				pNext->Add_Value(1, -1);
			}
			else
			{
				m_Arcs.Del_Shape(pNext);
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Arcs::_Collect_Get_Polygon(CSG_Shape_Polygon &Polygon)
{
	CSG_Shape *pArc = NULL;

	for(int i=0; !pArc && i<m_Arcs.Get_Count(); i++)
	{
		if( m_Arcs.Get_Shape(i)->asInt(ARC_PASSES) < 0 ) // is it a polygon arc (lines would return >= +1)
		{
			pArc = m_Arcs.Get_Shape(i);
		}
	}

	if( pArc )
	{
		CSG_Shape_Part &Part = *Polygon.Get_Part(Polygon.Add_Part(pArc->Get_Part(0)) - 1);

		int ID_Arc = pArc->asInt(ARC_ID), ID_Next = pArc->asInt(ARC_NODE_END), ID_Start = pArc->asInt(ARC_NODE_START);

		m_Arcs.Del_Shape(pArc);

		while( _Collect_Add_Next(Part, ID_Arc, ID_Next) && ID_Next != ID_Start );

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Arcs::Get_Intersection(CSG_Shapes *pPolygons, CSG_Table_Record *pAttributes, bool bSplitParts)
{
	for(sLong i=m_Arcs.Get_Count()-1; i>=0; i--)
	{
		if( !_Check_Arc(m_Arcs.Get_Shape(i)->asLine()) )
		{
			m_Arcs.Del_Shape(i);
		}
	}

#ifdef _DEBUG
//	SG_UI_DataObject_Add(SG_Create_Shapes(m_Nodes), 0);
//	SG_UI_DataObject_Add(SG_Create_Shapes(m_Arcs ), 0);
#endif

	//-----------------------------------------------------
	CSG_Shape_Polygon &Polygon = *pPolygons->Add_Shape(pAttributes, SHAPE_COPY_ATTR)->asPolygon();

	while( _Collect_Get_Polygon(Polygon) );

	//-----------------------------------------------------
	if( bSplitParts && Polygon.Get_Part_Count() > 1 )
	{
		for(int iPart=0; iPart<Polygon.Get_Part_Count(); iPart++)
		{
			if( !Polygon.is_Lake(iPart) )
			{
				CSG_Shape_Polygon *pPart = pPolygons->Add_Shape(pAttributes, SHAPE_COPY_ATTR)->asPolygon();

				for(int iPoint=0; iPoint<Polygon.Get_Point_Count(iPart); iPoint++)
				{
					pPart->Add_Point(Polygon.Get_Point(iPoint, iPart));
				}

				for(int jPart=0; jPart<Polygon.Get_Part_Count(); jPart++)
				{
					if(	Polygon.is_Lake(jPart) && pPart->Contains(Polygon.Get_Point(0, jPart)) )
					{
						for(int jPoint=0, nPart=pPart->Get_Part_Count(); jPoint<Polygon.Get_Point_Count(jPart); jPoint++)
						{
							pPart->Add_Point(Polygon.Get_Point(jPoint, jPart), nPart);
						}
					}
				}
			}
		}

		pPolygons->Del_Shape(&Polygon);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Line_Intersection::CPolygon_Line_Intersection(void)
{
	Set_Name		(_TL("Polygon-Line Intersection"));

	Set_Author		("O. Conrad (c) 2011");

	Set_Description	(_TW(
		"Polygon-line intersection. Splits polygons with lines. "
		"Complex self-intersecting lines might result in "
		"unwanted artifacts. In this case the method option "
		"line-by-line might improve the result. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("", "POLYGONS" , _TL("Polygons"    ), _TL(""), PARAMETER_INPUT , SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes("", "LINES"    , _TL("Lines"       ), _TL(""), PARAMETER_INPUT , SHAPE_TYPE_Line   );
	Parameters.Add_Shapes("", "INTERSECT", _TL("Intersection"), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Polygon);

	Parameters.Add_Bool("",
		"SPLIT_PARTS", _TL("Split Parts"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"METHOD"     , _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("all lines at once"),
			_TL("line-by-line")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Line_Intersection::On_Execute(void)
{
	CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();

	if(	!pPolygons->is_Valid() || pPolygons->Get_Count() < 1 )
	{
		Error_Set(_TL("invalid input polygons"));

		return( false );
	}

	//--------------------------------------------------------
	CSG_Shapes *pLines = Parameters("LINES")->asShapes();

	if(	!pLines->is_Valid() || pLines->Get_Count() < 1 )
	{
		Error_Set(_TL("invalid input lines"));

		return( false );
	}

	//--------------------------------------------------------
	if( pLines->Get_Extent().Intersects(pPolygons->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("polygons and lines extents do not intersect at all"));

		return( false );
	}

	//--------------------------------------------------------
	CSG_Shapes *pIntersection = Parameters("INTERSECT")->asShapes();

	pIntersection->Create(SHAPE_TYPE_Polygon, NULL, pPolygons);

	pIntersection->Fmt_Name("%s [%s: %s]", pPolygons->Get_Name(), _TL("Intersection"), pLines->Get_Name());

	bool bSplitParts = Parameters("SPLIT_PARTS")->asBool();

	int Method = Parameters("METHOD")->asInt();

	//--------------------------------------------------------
	for(sLong iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon *pPolygon = pPolygons->Get_Shape(iPolygon)->asPolygon();

		if( Method == 0 ) // all lines at once
		{
			CSG_Arcs Arcs(pPolygon);

			if( Arcs.Set_Lines(pLines) )
			{
				Arcs.Get_Intersection(pIntersection, pPolygon, bSplitParts);
			}
			else
			{
				pIntersection->Add_Shape(pPolygon);
			}
		}

		//-------------------------------------------------
		else // line-by-line
		{
			CSG_Shapes Intersection(SHAPE_TYPE_Polygon, NULL, pPolygons);

			Intersection.Add_Shape(pPolygon);

			for(sLong iLine=0; iLine<pLines->Get_Count(); iLine++)
			{
				CSG_Shape *pLine = pLines->Get_Shape(iLine);

				for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
				{
					for(sLong iIntersect=Intersection.Get_Count()-1; iIntersect>=0; iIntersect--)
					{
						CSG_Arcs Arcs(Intersection.Get_Shape(iIntersect)->asPolygon());

						if( Arcs.Set_Line(pLine->Get_Part(iPart)) )
						{
							Intersection.Del_Shape(iIntersect);

							Arcs.Get_Intersection(&Intersection, NULL, true);
						}
					}
				}
			}

			if( Intersection.Get_Count() > 1 )
			{
				if( bSplitParts )
				{
					for(int iIntersect=0; iIntersect<Intersection.Get_Count(); iIntersect++)
					{
						CSG_Shape *pIntersect = pIntersection->Add_Shape(pPolygon, SHAPE_COPY_ATTR);

						pIntersect->Assign(Intersection.Get_Shape(iIntersect), false);
					}
				}
				else
				{
					CSG_Shape *pIntersect = pIntersection->Add_Shape(pPolygon, SHAPE_COPY_ATTR);

					for(int iIntersect=0; iIntersect<Intersection.Get_Count(); iIntersect++)
					{
						for(int iPart=0; iPart<Intersection.Get_Shape(iIntersect)->Get_Part_Count(); iPart++)
						{
							pIntersect->Add_Part(Intersection.Get_Shape(iIntersect)->Get_Part(iPart));
						}
					}
				}
			}
			else
			{
				pIntersection->Add_Shape(pPolygon);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
