
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
	CSG_Arcs(CSG_Shape_Polygon *pPolygon, double Tolerance);

	bool					Destroy					(void);

	bool					is_Valid				(void)	const	{ return( m_Arcs.Get_Count() > 0 );	}

	bool					Set_Tolerance			(double Tolerance);

	bool					Set_Polygon				(CSG_Shape_Polygon *pPolygon);

	bool					Add_Lines				(CSG_Shapes *pLines, CSG_Shape_Polygon *pPolygon);

	bool					Get_Intersection		(CSG_Shapes *pPolygons, CSG_Table_Record *pAttributes, bool bSplitParts);


private:

	double					m_Tolerance;

	CSG_Shapes				m_Arcs;


	void					_On_Construction		(void);

	bool					_Add_Line				(CSG_Shape_Part *pLine, CSG_Shape_Polygon *pPolygon);
	int						_Add_Line_Segment		(CSG_Shape_Polygon *pPolygon, const TSG_Point Segment[2], double Distance, CSG_Shapes &Vertices);
	bool					_Add_Line_Intersection	(CSG_Shapes &Vertices, int &iStart);

	bool					_Add_Node				(const CSG_Point &Point, int Polygon_Part);

	bool					_Split_Arc				(CSG_Shape_Line *pArc, int iPoint, const CSG_Point &Point);

	bool					_Check_Arc				(CSG_Shape_Line *pArc);

	bool					_Collect_Add_Next		(CSG_Shape_Part    &Polygon);
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

	Set_Polygon(pPolygon);
}

CSG_Arcs::CSG_Arcs(CSG_Shape_Polygon *pPolygon, double Tolerance)
{
	_On_Construction();

	Set_Polygon(pPolygon);

	if( Tolerance >= 0. )
	{
		m_Tolerance = Tolerance;
	}
}

//---------------------------------------------------------
bool CSG_Arcs::Destroy(void)
{
	m_Arcs.Del_Shapes();

	return( true );
}

//---------------------------------------------------------
void CSG_Arcs::_On_Construction(void)
{
	m_Tolerance = 0.001;

	m_Arcs.Create(SHAPE_TYPE_Line);

	m_Arcs.Add_Field("PART", SG_DATATYPE_Int   );
	m_Arcs.Add_Field("PROC", SG_DATATYPE_Char  );
	m_Arcs.Add_Field("DIR0", SG_DATATYPE_Double);
	m_Arcs.Add_Field("DIR1", SG_DATATYPE_Double);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Arcs::Set_Polygon(CSG_Shape_Polygon *pPolygon)
{
	Destroy();

	if( !pPolygon || !pPolygon->is_Valid() )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		if( pPolygon->Get_Area(iPart) > 0. ) // just in case, skip invalid parts
		{
			bool bAscending = pPolygon->is_Lake(iPart) != pPolygon->is_Clockwise(iPart);

			CSG_Shape &Arc = *m_Arcs.Add_Shape();

			Arc.Set_Value(0, iPart); // PART
			Arc.Set_Value(1, 1    ); // PROC

			CSG_Point P, Pfirst = pPolygon->Get_Point(0, iPart, bAscending);

			Arc.Add_Point(P = Pfirst);

			for(int iPoint=1; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
			{
				CSG_Point Pnext = pPolygon->Get_Point(iPoint, iPart, bAscending);

				if( P != Pnext ) // skip duplicates
				{
					Arc.Add_Point(P = Pnext);
				}
			}

			if( P != Pfirst )
			{
				Arc.Add_Point(Pfirst); // make sure that last vertex equals first (close the polygon)
			}
		}
	}

	return( m_Arcs.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Arcs::Add_Lines(CSG_Shapes *pLines, CSG_Shape_Polygon *pPolygon)
{
	bool bIntersects = false;

	for(int iLine=0; iLine<pLines->Get_Count(); iLine++)
	{
		CSG_Shape_Line *pLine = pLines->Get_Shape(iLine)->asLine();

		if( pLine->Intersects(pPolygon) )
		{
			for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
			{
				if( _Add_Line(pLine->Get_Part(iPart), pPolygon) )
				{
					bIntersects = true;
				}
			}
		}
	}

	//-----------------------------------------------------
	return( bIntersects );
}

//---------------------------------------------------------
bool CSG_Arcs::_Add_Line(CSG_Shape_Part *pLine, CSG_Shape_Polygon *pPolygon)
{
	if( pLine->Get_Count() < 2 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes Vertices(SHAPE_TYPE_Point);

	Vertices.Add_Field("DISTANCE", SG_DATATYPE_Double);
	Vertices.Add_Field("CROSSING", SG_DATATYPE_Char  );
	Vertices.Add_Field("POLYPART", SG_DATATYPE_Int   );

	TSG_Point Segment[2]; Segment[1] = pLine->Get_Point(0);

	double Distance = 0.; int nCrossings = 0;

	for(int iPoint=1; iPoint<pLine->Get_Count(); iPoint++)
	{
		Segment[0] = Segment[1]; Segment[1] = pLine->Get_Point(iPoint);

		if( Segment[0].x != Segment[1].x || Segment[0].y != Segment[1].y )
		{
			nCrossings += _Add_Line_Segment(pPolygon, Segment, Distance, Vertices);

			Distance   += SG_Get_Distance(Segment[0].x, Segment[0].y, Segment[1].x, Segment[1].y);
		}
	}

	if( nCrossings < 2 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int nAdded = 0;

	Vertices.Set_Index(0, TABLE_INDEX_Ascending);

	for(int i=0; i<Vertices.Get_Count(); )
	{
		if( _Add_Line_Intersection(Vertices, i) )
		{
			nAdded++;
		}
	}

	return( nAdded > 0 );
}

//---------------------------------------------------------
int CSG_Arcs::_Add_Line_Segment(CSG_Shape_Polygon *pPolygon, const TSG_Point S[2], double Distance, CSG_Shapes &Vertices)
{
	#define Add_Vertex(p, d, c, i) { CSG_Shape &v = *Vertices.Add_Shape();\
		v.Add_Point(p); v.Set_Value(0, d); v.Set_Value(1, c); v.Set_Value(2, i);\
	}

	int nCrossings = 0; bool bAddFirst = true;

	for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		TSG_Point A = pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);

		for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point C, B = A; A = pPolygon->Get_Point(iPoint, iPart);

			if( SG_Get_Crossing(C, A, B, S[0], S[1]) )
			{
				nCrossings++;

				Add_Vertex(C, Distance + SG_Get_Distance(C, S[0]), 1, iPart);

				if( C.x == S[0].x && C.y == S[0].y )
				{
					bAddFirst = false;
				}
			}
		}
	}

	if( bAddFirst && pPolygon->Contains(S[0]) )
	{
		Add_Vertex(S[0], Distance, 0, -1);
	}

	return( nCrossings );
}

//---------------------------------------------------------
bool CSG_Arcs::_Add_Line_Intersection(CSG_Shapes &Vertices, int &i)
{
	if( Vertices.Get_Shape_byIndex(i)->asInt(1) != 1 ) // not starting with a crossing, lets find the first entering one...
	{
		for(; i<Vertices.Get_Count(); i++)
		{
			CSG_Shape &Vertex = *Vertices.Get_Shape_byIndex(i);

			if( Vertex.asInt(1) == 1 ) // crossing leaving the polygon
			{
				i++;

				return( false );
			}
		}

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shape &First = *Vertices.Get_Shape_byIndex(i);

	CSG_Shape *pArc = m_Arcs.Add_Shape();

	pArc->Set_Value(0, -1); // TYPE => splitting line strings...
	pArc->Set_Value(1,  2); // PROC => ...need to be processed twice!

	for(; i<Vertices.Get_Count(); i++)
	{
		CSG_Shape &Vertex = *Vertices.Get_Shape_byIndex(i);

		pArc->Add_Point(Vertex.Get_Point(0));

		if( Vertex.asInt(1) == 1 && pArc->Get_Point_Count() > 1 ) // crossing leaving the polygon
		{
			i++;

			_Add_Node(First .Get_Point(0), First .asInt(2));
			_Add_Node(Vertex.Get_Point(0), Vertex.asInt(2));

			return( true );
		}
	}

	m_Arcs.Del_Shape(pArc); // sketch did not finish with a leaving crossing!

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Arcs::_Add_Node(const CSG_Point &Point, int Polygon_Part)
{
	for(int i=0; i<m_Arcs.Get_Count(); i++)
	{
		CSG_Shape_Line *pArc = m_Arcs.Get_Shape(i)->asLine();

		if( pArc->asInt(0) == Polygon_Part )
		{
			CSG_Point Segment[2]; Segment[1] = pArc->Get_Point(0);

			for(int iPoint=1; iPoint<pArc->Get_Point_Count(); iPoint++)
			{
				Segment[0] = Segment[1]; Segment[1] = pArc->Get_Point(iPoint);

				if( SG_Is_Point_On_Line(Point, Segment[0], Segment[1], true, m_Tolerance) )
				{
					_Split_Arc(pArc, iPoint, Point);

					return( true );
				}
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Arcs::_Split_Arc(CSG_Shape_Line *pArc, int iPoint, const CSG_Point &Point)
{
	CSG_Shape *pNew = m_Arcs.Add_Shape(pArc, SHAPE_COPY_ATTR);

	if( Point != pArc->Get_Point(iPoint) )
	{
		pNew->Add_Point(Point);
	}

	for(int i=iPoint; i<pArc->Get_Point_Count(); i++)
	{
		pNew->Add_Point(pArc->Get_Point(i));
	}

	for(int i=pArc->Get_Point_Count()-1; i>=iPoint; i--)
	{
		pArc->Del_Point(i);
	}

	if( Point != pArc->Get_Point(0, 0, false) )
	{
		pArc->Add_Point(Point);
	}

	return( true );
}

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

	if( pArc->Get_Point_Count(0) > 1 ) // update end node directions
	{
		pArc->Set_Value(2, SG_Get_Angle_Of_Direction(pArc->Get_Point(0, 0,  true), pArc->Get_Point(1, 0,  true)));
		pArc->Set_Value(3, SG_Get_Angle_Of_Direction(pArc->Get_Point(0, 0, false), pArc->Get_Point(1, 0, false)));

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Arcs::_Collect_Add_Next(CSG_Shape_Part &Polygon)
{
	CSG_Shape *pNext = NULL; bool bAscending = true;

	CSG_Point Node = Polygon.Get_Point(0, false); // get last point of current polygon ring

	double minDiff = M_PI_360, Direction = SG_Get_Angle_Of_Direction(Node, Polygon.Get_Point(1, false));

	for(int i=0; i<m_Arcs.Get_Count(); i++)
	{
		CSG_Shape *pArc = m_Arcs.Get_Shape(i); 

		if     ( Node == pArc->Get_Point(0, 0,  true) )
		{
			double Diff = Direction - pArc->asDouble(2); if( Diff < 0. ) Diff += M_PI_360;

			if( Diff > 0. && (!pNext || minDiff > Diff) )
			{
				pNext = pArc; minDiff = Diff; bAscending = true;
			}
		}
		else if( Node == pArc->Get_Point(0, 0, false) )
		{
			double Diff = Direction - pArc->asDouble(3); if( Diff < 0. ) Diff += M_PI_360;

			if( Diff > 0. && (!pNext || minDiff > Diff) )
			{
				pNext = pArc; minDiff = Diff; bAscending = false;
			}
		}
	}

	//-----------------------------------------------------
	if( pNext )
	{
		for(int i=1; i<pNext->Get_Point_Count(0); i++)
		{
			Polygon.Add_Point(pNext->Get_Point(i, 0, bAscending));
		}

		if( pNext->asInt(1) > 1 ) // PROC
		{
			pNext->Add_Value(1, -1);
		}
		else
		{
			m_Arcs.Del_Shape(pNext);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Arcs::_Collect_Get_Polygon(CSG_Shape_Polygon &Polygon)
{
	CSG_Shape *pArc = NULL;

	for(int i=0; !pArc && i<m_Arcs.Get_Count(); i++)
	{
		if( m_Arcs.Get_Shape(i)->asInt(0) >= 0 ) // is it a polygon arc (lines would return '-1')
		{
			pArc = m_Arcs.Get_Shape(i);
		}
	}

	if( pArc )
	{
		CSG_Shape_Part &Part = *Polygon.Get_Part(Polygon.Add_Part(pArc->Get_Part(0)) - 1);

		m_Arcs.Del_Shape(pArc);

		while( _Collect_Add_Next(Part) );

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Arcs::Get_Intersection(CSG_Shapes *pPolygons, CSG_Table_Record *pAttributes, bool bSplitParts)
{
	for(int i=m_Arcs.Get_Count()-1; i>=0; i--)
	{
		if( !_Check_Arc(m_Arcs.Get_Shape(i)->asLine()) )
		{
			m_Arcs.Del_Shape(i);
		}
	}

	#ifdef _DEBUG
		SG_UI_DataObject_Add(SG_Create_Shapes(m_Arcs), 0);
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
	double Tolerance = sqrt(pPolygons->Get_Extent().Get_Area()) / 1000000.;

	//--------------------------------------------------------
	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon *pPolygon = pPolygons->Get_Shape(iPolygon)->asPolygon();

		CSG_Arcs Arcs(pPolygon, Tolerance);

		if( Arcs.is_Valid() && Arcs.Add_Lines(pLines, pPolygon) )
		{
			Arcs.Get_Intersection(pIntersection, pPolygon, bSplitParts);
		}
		else
		{
			pIntersection->Add_Shape(pPolygon);
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
