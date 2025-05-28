
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   shape_polygon.cpp                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define BOOL3_NOTSET	-1
#define BOOL3_FALSE		 0
#define BOOL3_TRUE		 1


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape_Polygon_Part::CSG_Shape_Polygon_Part(CSG_Shape_Points *pOwner)
	: CSG_Shape_Part(pOwner)
{
	m_bLake			= BOOL3_NOTSET;
	m_bClockwise	= BOOL3_NOTSET;
}

//---------------------------------------------------------
CSG_Shape_Polygon_Part::~CSG_Shape_Polygon_Part(void)
{}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shape_Polygon_Part::_Invalidate(void)
{
	CSG_Shape_Part::_Invalidate();

	m_bLake			= BOOL3_NOTSET;
	m_bClockwise	= BOOL3_NOTSET;
}

//---------------------------------------------------------
void CSG_Shape_Polygon_Part::_Update_Area(void)
{
	if( m_nPoints > 2 && m_bClockwise == BOOL3_NOTSET )
	{
		TSG_Point	*pPoint, A, B;

		m_Area			= 0.0;
		m_Perimeter		= 0.0;

		m_Centroid.x	= 0.0;
		m_Centroid.y	= 0.0;

		pPoint			= m_Points + m_nPoints - 1;
		B.x				= pPoint->x - Get_Extent().Get_XCenter();
		B.y				= pPoint->y - Get_Extent().Get_YCenter();
		pPoint			= m_Points;

		for(int iPoint=0; iPoint<m_nPoints; iPoint++, pPoint++, B=A)
		{
			A.x				 = pPoint->x - Get_Extent().Get_XCenter();
			A.y				 = pPoint->y - Get_Extent().Get_YCenter();

			double	d		 = B.x * A.y - A.x * B.y;

			m_Centroid.x	+= d * (A.x + B.x);
			m_Centroid.y	+= d * (A.y + B.y);

			m_Area			+= d;

			m_Perimeter		+= SG_Get_Distance(A, B);
		}

		if( m_Area != 0.0 )
		{
			m_Centroid.x	/= (3.0 * m_Area);	m_Centroid.x	+= Get_Extent().Get_XCenter();
			m_Centroid.y	/= (3.0 * m_Area);	m_Centroid.y	+= Get_Extent().Get_YCenter();
		}

		m_bClockwise	= m_Area < 0.0 ? BOOL3_TRUE : BOOL3_FALSE;

		m_Area			= fabs(m_Area) / 2.0;
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Polygon_Point_Relation CSG_Shape_Polygon_Part::Get_Point_Relation(const CSG_Point &p)
{
	return( Get_Point_Relation(p.x, p.y) );
}

//---------------------------------------------------------
TSG_Polygon_Point_Relation CSG_Shape_Polygon_Part::Get_Point_Relation(double x, double y)
{
	if(	m_nPoints > 2 && Get_Extent().Contains(x, y) )
	{
		TSG_Point *pB = m_Points + m_nPoints - 1;

		if( x == pB->x && y == pB->y ) // check identity
		{
			return( SG_POLYGON_POINT_Vertex );
		}

		TSG_Point *pA = m_Points; bool bInterior = false;

		for(int iPoint=0; iPoint<m_nPoints; iPoint++, pB=pA++)
		{
			if( pA->x == pB->x && pA->y == pB->y ) // ignore duplicates
			{
				continue;
			}

			if( x == pA->x && y == pA->y ) // check identity (previous vertex has already been checked)
			{
				return( SG_POLYGON_POINT_Vertex );
			}

			if( pA->y == pB->y && y == pA->y && SG_IS_BETWEEN(pA->x, x, pB->x) )
			{
				return( SG_POLYGON_POINT_Edge );
			}

			if( SG_IS_BETWEEN(pA->y, y, pB->y) )
			{
				if( (y == pA->y && pB->y >= pA->y) || (y == pB->y && pA->y >= pB->y) ) // filter out "ray pass vertex" problem by treating the line a little lower
				{
					continue;
				}

				double c = (pB->x - x) * (pA->y - y) - (pA->x - x) * (pB->y - y); // cross product PA X PB, P(x, y) is on left side of AB if c > 0.

				if( c == 0. )
				{
					return( SG_POLYGON_POINT_Edge );
				}

				if( (pB->y < pA->y) == (c > 0.) )
				{
					bInterior = !bInterior;
				}
			}
		}

		if( bInterior )
		{
			return( SG_POLYGON_POINT_Interior );
		}
	}

	return( SG_POLYGON_POINT_Outside );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Polygon_Part::is_OnEdge(const CSG_Point &p)
{
	return( is_OnEdge(p.x, p.y) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon_Part::is_OnEdge(double x, double y)
{
	TSG_Polygon_Point_Relation	r	= Get_Point_Relation(x, y);

	return( r == SG_POLYGON_POINT_Vertex
		||  r == SG_POLYGON_POINT_Edge
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Polygon_Part::Contains(const CSG_Point &p)
{
	return( Contains(p.x, p.y) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon_Part::Contains(double x, double y)
{
	TSG_Polygon_Point_Relation	r	= Get_Point_Relation(x, y);

	return( r == SG_POLYGON_POINT_Interior
		||  r == SG_POLYGON_POINT_Vertex
		||  r == SG_POLYGON_POINT_Edge
	);

	//-----------------------------------------------------
	if(	m_nPoints > 2 && Get_Extent().Contains(x, y) )
	{
		int	nCrossings	= 0;

		TSG_Point	*pA	= m_Points;
		TSG_Point	*pB	= m_Points + m_nPoints - 1;

		for(int iPoint=0; iPoint<m_nPoints; iPoint++, pB=pA++)
		{
			if( y <= pA->y )	// pA on or above ray
			{
				if( pB->y <= y )	// pB on or below ray
				{
					if( ((y - pB->y) * (pA->x - pB->x)) >= ((x - pB->x) * (pA->y - pB->y)) )
					{
						nCrossings++;
					}
				}
			}
			else				// pA below ray
			{
				if( pB->y >= y )	// pB above ray
				{
					if( ((y - pB->y) * (pA->x - pB->x)) <= ((x - pB->x) * (pA->y - pB->y)) )
					{
						nCrossings++;
					}
				}
			}
		}

		return( nCrossings % 2 != 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* Returns true if this polygon part touches that defined by pPart, i.e. the
* polygons share vertices or have vertices placed on the edge
* of the other. If bSimpleCheck is true the check is only 
* performed until one shared vertex is found, which is
* sufficient if it can be expected that the polygons do not
* overlap.
*/
//---------------------------------------------------------
bool CSG_Shape_Polygon_Part::is_Neighbour(CSG_Shape_Polygon_Part *pPart, bool bSimpleCheck)
{
	if( !Get_Extent().Intersects(pPart->Get_Extent()) )
	{
		return( false );
	}

	bool bNeighbour = false;

	//---------------------------------------------------------
	for(int iPoint=0; iPoint<pPart->Get_Count(); iPoint++)
	{
		switch( Get_Point_Relation(pPart->Get_Point(iPoint)) )
		{
		case SG_POLYGON_POINT_Outside :	break;
		case SG_POLYGON_POINT_Interior:	return( false );
		case SG_POLYGON_POINT_Vertex  :
		case SG_POLYGON_POINT_Edge    :	if( bSimpleCheck )	{	return( true );	}
			bNeighbour	= true;
			break;
		}
	}

	//---------------------------------------------------------
	for(int iPoint=0; iPoint<Get_Count(); iPoint++)
	{
		switch( pPart->Get_Point_Relation(Get_Point(iPoint)) )
		{
		case SG_POLYGON_POINT_Outside :	break;
		case SG_POLYGON_POINT_Interior:	return( false );
		case SG_POLYGON_POINT_Vertex  :
		case SG_POLYGON_POINT_Edge    :	if( bSimpleCheck )	{	return( true );	}
			bNeighbour	= true;
			break;
		}
	}

	return( bNeighbour );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shape_Polygon_Part::_Add_Edge_Points(CSG_Shape_Polygon_Part *pPart, const CSG_Point &P0, const CSG_Point &P1, CSG_Points &Part, double Epsilon)
{
	CSG_Points Points;

	for(int i=0; i<pPart->Get_Count(); i++)
	{
		CSG_Point P = pPart->Get_Point(i);

		if( P != P0 && P != P1 && SG_Is_Point_On_Line(P, P0, P1, true, Epsilon) )
		{
			Points.Add(P);
		}
	}

	if( Points.Get_Count() == 1 )
	{
		Part.Add(Points[0]);
	}
	else if( Points.Get_Count() > 1 )
	{
		double *Distances = new double[Points.Get_Count()];

		for(sLong i=0; i<Points.Get_Count(); i++)
		{
			Distances[i] = SG_Get_Distance(P0, Points[i]);
		}

		CSG_Index Index(Points.Get_Count(), Distances); delete[](Distances);

		for(sLong i=0; i<Points.Get_Count(); i++)
		{
			Part.Add(Points[Index[i]]);
		}
	}
}

//---------------------------------------------------------
bool CSG_Shape_Polygon_Part::_is_OnEdge(CSG_Shape_Polygon_Part *pPart, const CSG_Point &P, bool bVertex, double Epsilon)
{
	CSG_Point P1 = pPart->Get_Point(0, false); // close the ring, starting with last point!

	if( !bVertex && P == P1 )
	{
		return( false );
	}

	for(int i=0; i<pPart->Get_Count(); i++)
	{
		CSG_Point P0 = P1; P1 = pPart->Get_Point(i);

		if( !bVertex && P == P1 )
		{
			return( false );
		}

		if( SG_Is_Point_On_Line(P, P0, P1, true, Epsilon) )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
/**
* Returns all edge segments shared by this polygon part and
* the one provide by pPart argument as CSG_Lines object.
* If bVertexCheck is true vertices lying on edged segments
* of the other polygon part will be recognized. If not only
* vertices are taken into respect that are shared by both
* polygon parts. Due to precision issues the tolerance factor
* specified with Epsilon should be slightly greater than zero
* when using the bVertexCheck option.
*/
//---------------------------------------------------------
CSG_Lines CSG_Shape_Polygon_Part::Get_Shared_Edges(CSG_Shape_Polygon_Part *pPart, bool bVertexCheck, double Epsilon)
{
	CSG_Lines Edges;

	if( Get_Extent().Intersects(pPart->Get_Extent()) )
	{
		if( bVertexCheck == false )
		{
			bool bOnEdge = is_OnEdge(pPart->Get_Point(0, false)); // close the ring, starting with last point!

			if( bOnEdge )
			{
				Edges.Add().Add(pPart->Get_Point(0, false));
			}

			for(int i=0; i<pPart->Get_Count(); i++)
			{
				CSG_Point Point = pPart->Get_Point(i);

				if( is_OnEdge(Point) )
				{
					if( !bOnEdge ) // start a new edge segment
					{
						Edges.Add().Add(Point);
					}
					else // continue edge segment
					{
						Edges[Edges.Get_Count() - 1].Add(Point);
					}

					bOnEdge = true;
				}
				else
				{
					bOnEdge = false;
				}
			}
		}
		else
		{
			CSG_Points Part; CSG_Point P1 = Get_Point(0, false); // close the ring, starting with last point!

			if( _is_OnEdge(pPart, P1, true, Epsilon) ) { Part.Add(P1); }

			for(int i=0; i<Get_Count(); i++)
			{
				CSG_Point P0 = P1; P1 = Get_Point(i);

				_Add_Edge_Points(pPart, P0, P1, Part, Epsilon);

				if( _is_OnEdge(pPart, P1, true, Epsilon) ) { Part.Add(P1); }
			}

			//---------------------------------------------
			if( Part.Get_Count() > 1 )
			{
				P1 = Part[0]; bool bOnEdge = false;
				CSG_Point C0 = P1;

				for(sLong i=1, n=-1; i<Part.Get_Count(); i++)
				{
					CSG_Point C1, P0 = P1; P1 = Part[i];

					C1.x = P0.x + 0.5 * (P1.x - P0.x);
					C1.y = P0.y + 0.5 * (P1.y - P0.y);

					if( C0 == C1 )
					{
						continue; // do not add the same edge twice (in different directions)
					}

					if( _is_OnEdge(this, C1, true, Epsilon) && _is_OnEdge(pPart, C1, true, Epsilon) )
					{
						if( !bOnEdge ) // start a new edge segment
						{
							n = Edges.Get_Count(); Edges.Add();
							Edges[n].Add(P0);
							Edges[n].Add(P1);
						}
						else // continue edge segment
						{
							Edges[n].Add(P1);
						}

						bOnEdge = true;
					}
					else
					{
						bOnEdge = false;
					}

					C0 = C1;
				}
			}
		}
	}

	return( Edges );
}

//---------------------------------------------------------
/**
* This function requests the edges shared by this polygon part
* and the one provided by the pPart argument and returns
* the total length. See Get_Shared_Edges() for more details.
*/
//---------------------------------------------------------
double CSG_Shape_Polygon_Part::Get_Shared_Length(CSG_Shape_Polygon_Part *pPart, bool bVertexCheck, double Epsilon)
{
	CSG_Lines Edges(Get_Shared_Edges(pPart, bVertexCheck, Epsilon));

	return( Edges.Get_Length() );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Polygon_Part::Get_Distance(TSG_Point Point, TSG_Point &Next)
{
	if( m_nPoints < 1 )
	{
		return( -1.0 );
	}

	if( Contains(Point) )
	{
		return( 0.0 );
	}

	TSG_Point	*pB	= m_Points + m_nPoints - 1;
	TSG_Point	*pA	= m_Points, C;

	double	Distance	= SG_Get_Nearest_Point_On_Line(Point, *pA, *pB, Next);

	for(int iPoint=0; iPoint<m_nPoints && Distance>0.0; iPoint++, pB=pA++)
	{
		double	d	= SG_Get_Nearest_Point_On_Line(Point, *pA, *pB, C);

		if(	d >= 0.0 && d < Distance )
		{
			Distance	= d;
			Next		= C;
		}
	}

	return( Distance );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape_Polygon::CSG_Shape_Polygon(CSG_Shapes *pOwner, sLong Index)
	: CSG_Shape_Points(pOwner, Index)
{
	m_bUpdate_Lakes = true;
}

//---------------------------------------------------------
CSG_Shape_Polygon::~CSG_Shape_Polygon(void)
{}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shape_Polygon::_Invalidate(void)
{
	CSG_Shape_Points::_Invalidate();

	if( m_bUpdate_Lakes )
	{
		m_bUpdate_Lakes = false;

		for(int i=0; i<m_nParts; i++)
		{
			Get_Polygon_Part(i)->m_bLake = BOOL3_NOTSET;
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Intersection CSG_Shape_Polygon::On_Intersects(CSG_Shape *pShape)
{
	//-----------------------------------------------------
	bool bIn  = false;
	bool bOut = false;

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			if( Contains(pShape->Get_Point(iPoint, iPart)) )
			{
				bIn		= true;
			}
			else
			{
				bOut	= true;
			}

			if( bIn && bOut )	// some vertices are in, some are out
			{
				return( INTERSECTION_Overlaps );
			}
		}
	}

	//-----------------------------------------------------
	if( pShape->Get_Type() == SHAPE_TYPE_Point || pShape->Get_Type() == SHAPE_TYPE_Points )
	{
		return( bIn ? INTERSECTION_Contains : INTERSECTION_None );	// there are no other options for points
	}

	//-----------------------------------------------------
	for(int iPart=0; iPart<Get_Part_Count(); iPart++)
	{
		if( Get_Point_Count(iPart) < 3 )
		{
			continue;
		}

		TSG_Point	A[2], B[2], C;	A[0]	= Get_Point(0, iPart, false);

		for(int iPoint=0; iPoint<Get_Point_Count(iPart); iPoint++)
		{
			A[1]	= A[0];	A[0]	= Get_Point(iPoint, iPart);

			for(int jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
			{
				//-----------------------------------------
				if( pShape->Get_Type() == SHAPE_TYPE_Line && pShape->Get_Point_Count(jPart) >= 2 )
				{
					B[0]	= pShape->Get_Point(0, jPart);

					for(int jPoint=1; jPoint<pShape->Get_Point_Count(jPart); jPoint++)
					{
						B[1]	= B[0];	B[0]	= pShape->Get_Point(jPoint, jPart);

						if( SG_Get_Crossing(C, A[0], A[1], B[0], B[1]) )
						{
							return( INTERSECTION_Overlaps );
						}
					}
				}

				//-----------------------------------------
				if( pShape->Get_Type() == SHAPE_TYPE_Polygon && pShape->Get_Point_Count(jPart) >= 3 )
				{
					B[0]	= pShape->Get_Point(0, jPart, false);

					for(int jPoint=0; jPoint<pShape->Get_Point_Count(jPart); jPoint++)
					{
						B[1]	= B[0];	B[0]	= pShape->Get_Point(jPoint, jPart);

						if( SG_Get_Crossing(C, A[0], A[1], B[0], B[1]) )
						{
							return( INTERSECTION_Overlaps );
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( bIn ? INTERSECTION_Contains : INTERSECTION_None );
}

//---------------------------------------------------------
TSG_Intersection CSG_Shape_Polygon::On_Intersects(TSG_Rect Region)
{
	// called if polygon's bounding box contains or overlaps with region.
	// now let's figure out how region intersects with polygon itself

	//-----------------------------------------------------
	for(int iPart=0; iPart<m_nParts; iPart++)
	{
		CSG_Shape_Part	*pPart	= m_pParts[iPart];

		switch( pPart->Get_Extent().Intersects(Region) )
		{
		case INTERSECTION_None:			// region and polygon part are distinct
			break;

		case INTERSECTION_Identical:	// region contains polygon part
		case INTERSECTION_Contained:
			return( Get_Extent().Intersects(Region) );

		case INTERSECTION_Contains:
		case INTERSECTION_Overlaps:		// region at least partly contained by polygon part's extent, now let's look at the polygon part itself!
			if( pPart->Get_Count() > 2 )
			{
				TSG_Point	*pB	= pPart->m_Points + pPart->m_nPoints - 1;
				TSG_Point	*pA	= pPart->m_Points, C;

				for(int iPoint=0; iPoint<pPart->m_nPoints; iPoint++, pB=pA++)
				{
					if(	SG_Get_Crossing_InRegion(C, *pA, *pB, Region) )
					{
						return( INTERSECTION_Overlaps );
					}
				}
			}
			break;
		}
	}
	
	//-----------------------------------------------------
	return( Contains(Region.xMin, Region.yMin) ? INTERSECTION_Contains : INTERSECTION_None );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_Lake(int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	if( !pPart )
	{
		return( false );
	}

	if( pPart->m_bLake == BOOL3_NOTSET )
	{
		if( pPart->m_nPoints < 1 || m_nParts <= 1 )
		{
			pPart->m_bLake	= BOOL3_FALSE;
		}
		else
		{
			m_bUpdate_Lakes	= true;

			pPart->m_bLake	= BOOL3_FALSE;

			for(int iPoint=0; iPoint<pPart->m_nPoints; iPoint++)	// find a point that is not on vertex/edge
			{
				TSG_Point	p	= pPart->Get_Point(iPoint);
				bool	bEdge	= false;
				int	nContained	= 0;

				for(iPart=0; !bEdge && iPart<m_nParts; iPart++)
				{
					if( pPart != m_pParts[iPart] )
					{
						switch( Get_Polygon_Part(iPart)->Get_Point_Relation(p) )
						{
						case SG_POLYGON_POINT_Outside : break;
						case SG_POLYGON_POINT_Interior: nContained++; break;
						case SG_POLYGON_POINT_Vertex  :
						case SG_POLYGON_POINT_Edge    : bEdge = true; break;
						}
					}
				}

				if( !bEdge )
				{
					pPart->m_bLake	= nContained % 2 ? BOOL3_TRUE : BOOL3_FALSE;

					break;
				}
			}
		}
	}

	return( pPart->m_bLake == BOOL3_TRUE );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_Clockwise(int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	return(	pPart && pPart->is_Clockwise() );
}

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Perimeter(int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	return( pPart ? pPart->Get_Perimeter() : 0.0 );
}

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Perimeter(void)
{
	double	Perimeter	= 0.0;

	for(int iPart=0; iPart<m_nParts; iPart++)
	{
		Perimeter	+= Get_Perimeter(iPart);
	}

	return( Perimeter );
}

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Area(int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	return( pPart ? pPart->Get_Area() : 0.0 );
}

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Area(void)
{
	double	Area	= 0.0;

	for(int iPart=0; iPart<m_nParts; iPart++)
	{
		Area	+= is_Lake(iPart) ? -Get_Area(iPart) : Get_Area(iPart);
	}

	return( Area );
}

//---------------------------------------------------------
TSG_Point CSG_Shape_Polygon::Get_Centroid(int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	if(	pPart )
	{
		return( pPart->Get_Centroid() );
	}

	return( CSG_Point(0.0, 0.0) );
}

//---------------------------------------------------------
TSG_Point CSG_Shape_Polygon::Get_Centroid(void)
{
	if( m_nParts == 1 )
	{
		return( Get_Centroid(0) );
	}

	int			iPart;
	double		Weights;
	TSG_Point	Centroid;

	Centroid.x	= 0.0;
	Centroid.y	= 0.0;

	for(iPart=0, Weights=0.0; iPart<m_nParts; iPart++)
	{
		if( !is_Lake(iPart) )
		{
			TSG_Point	p	= Get_Centroid(iPart);
			double		w	= Get_Area    (iPart);

			Centroid.x	+= w * p.x;
			Centroid.y	+= w * p.y;

			Weights		+= w;
		}
	}

	if( Weights > 0.0 )
	{
		Centroid.x	/= Weights;
		Centroid.y	/= Weights;
	}

	return( Centroid );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Polygon_Point_Relation CSG_Shape_Polygon::Get_Point_Relation(const CSG_Point &p, int iPart)
{
	return( Get_Point_Relation(p.x, p.y, iPart) );
}

//---------------------------------------------------------
TSG_Polygon_Point_Relation CSG_Shape_Polygon::Get_Point_Relation(double x, double y, int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	return(	pPart ? pPart->Get_Point_Relation(x, y) : SG_POLYGON_POINT_Outside );
}

//---------------------------------------------------------
TSG_Polygon_Point_Relation CSG_Shape_Polygon::Get_Point_Relation(const CSG_Point &p)
{
	return( Get_Point_Relation(p.x, p.y) );
}

//---------------------------------------------------------
TSG_Polygon_Point_Relation CSG_Shape_Polygon::Get_Point_Relation(double x, double y)
{
	if( Get_Extent().Contains(x, y) )
	{
		int	nContained	= 0;

		for(int iPart=0; iPart<m_nParts; iPart++)
		{
			switch( Get_Polygon_Part(iPart)->Get_Point_Relation(x, y) )
			{
			case SG_POLYGON_POINT_Outside :	break;
			case SG_POLYGON_POINT_Interior: nContained++; break;
			case SG_POLYGON_POINT_Vertex  : return( SG_POLYGON_POINT_Vertex );
			case SG_POLYGON_POINT_Edge    : return( SG_POLYGON_POINT_Edge );
			}
		}

		if( nContained % 2 != 0 )
		{
			return( SG_POLYGON_POINT_Interior );
		}
	}

	return( SG_POLYGON_POINT_Outside );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_OnEdge(const CSG_Point &p, int iPart)
{
	return( is_OnEdge(p.x, p.y, iPart) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_OnEdge(double x, double y, int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	return(	pPart && pPart->is_OnEdge(x, y) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_OnEdge(const CSG_Point &p)
{
	return( is_OnEdge(p.x, p.y) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_OnEdge(double x, double y)
{
	if( Get_Extent().Contains(x, y) )
	{
		for(int iPart=0; iPart<m_nParts; iPart++)
		{
			if( Get_Polygon_Part(iPart)->is_OnEdge(x, y) )
			{
				return( true );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Polygon::Contains(const CSG_Point &p, int iPart)
{
	return( Contains(p.x, p.y, iPart) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::Contains(double x, double y, int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	return(	pPart && pPart->Contains(x, y) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::Contains(const CSG_Point &p)
{
	return( Contains(p.x, p.y) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::Contains(double x, double y)
{
	if( Get_Extent().Contains(x, y) )
	{
		int	nContained	= 0;

		for(int iPart=0; iPart<m_nParts; iPart++)
		{
			if( Get_Polygon_Part(iPart)->Contains(x, y) )
			{
				nContained++;
			}
		}

		return( nContained % 2 != 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Returns true if this polygon touches pPolyogon, i.e. the
  * polygons share vertices or have vertices placed on the edge
  * of the other. If bSimpleCheck is true the check is only 
  * performed until one shared vertex is found, which is
  * sufficient if it can be expected that the polygons do not
  * overlap.
*/
//---------------------------------------------------------
bool CSG_Shape_Polygon::is_Neighbour(CSG_Shape_Polygon *pPolygon, bool bSimpleCheck)
{
	if( !Get_Extent().Intersects(pPolygon->Get_Extent()) )
	{
		return( false );
	}

	bool bNeighbour = false;

	//---------------------------------------------------------
	for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(); iPoint++)
	{
		switch( Get_Point_Relation(pPolygon->Get_Point(iPoint)) )
		{
		case SG_POLYGON_POINT_Outside :	break;
		case SG_POLYGON_POINT_Interior:	return( false );
		case SG_POLYGON_POINT_Vertex  :
		case SG_POLYGON_POINT_Edge    :	if( bSimpleCheck )	{	return( true );	}
			bNeighbour	= true;
			break;
		}
	}

	//---------------------------------------------------------
	for(int iPoint=0; iPoint<Get_Point_Count(); iPoint++)
	{
		switch( pPolygon->Get_Point_Relation(Get_Point(iPoint)) )
		{
		case SG_POLYGON_POINT_Outside :	break;
		case SG_POLYGON_POINT_Interior:	return( false );
		case SG_POLYGON_POINT_Vertex  :
		case SG_POLYGON_POINT_Edge    :	if( bSimpleCheck )	{	return( true );	}
			bNeighbour	= true;
			break;
		}
	}

	return( bNeighbour );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* Returns all edge segments shared by this polygon and the
* one provide by the pPolygon argument as CSG_Lines object.
* If bVertexCheck is true vertices lying on edged segments
* of the other polygon will be recognized. If not only
* vertices are taken into respect that are shared by both
* polygons. Due to precision issues the tolerance factor
* specified with Epsilon argument should be slightly greater
* than zero when using the bVertexCheck option.
*/
//---------------------------------------------------------
CSG_Lines CSG_Shape_Polygon::Get_Shared_Edges(CSG_Shape_Polygon *pPolygon, bool bVertexCheck, double Epsilon)
{
	CSG_Lines Edges;

	if( Get_Extent().Intersects(pPolygon->Get_Extent()) )
	{
		for(int i=0; i<Get_Part_Count(); i++)
		{
			for(int j=0; j<pPolygon->Get_Part_Count(); j++)
			{
				Edges.Add(Get_Polygon_Part(i)->Get_Shared_Edges(pPolygon->Get_Polygon_Part(j), bVertexCheck, Epsilon));
			}
		}
	}

	return( Edges );
}

//---------------------------------------------------------
/**
* This function requests the edges shared by this polygon
* and the one provided by the pPolygon argument and returns
* the total length. See Get_Shared_Edges() for more details.
*/
//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Shared_Length(CSG_Shape_Polygon *pPolygon, bool bVertexCheck, double Epsilon)
{
	CSG_Lines Edges(Get_Shared_Edges(pPolygon, bVertexCheck, Epsilon));

	return( Edges.Get_Length() );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Distance(TSG_Point Point, TSG_Point &Next, int iPart)	const
{
	CSG_Shape_Polygon_Part *pPart = Get_Polygon_Part(iPart);

	return(	pPart ? pPart->Get_Distance(Point, Next) : -1. );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
