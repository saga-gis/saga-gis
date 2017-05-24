/**********************************************************
 * Version $Id$
 *********************************************************/

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define BOOL3_NOTSET	-1
#define BOOL3_FALSE		 0
#define BOOL3_TRUE		 1


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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

//---------------------------------------------------------
bool CSG_Shape_Polygon_Part::Contains(const TSG_Point &Point)
{
	return( Contains(Point.x, Point.y) );
}

bool CSG_Shape_Polygon_Part::Contains(double x, double y)
{
	if(	m_nPoints > 2 && Get_Extent().Contains(x, y) )
	{
		int	nCrossings	= 0;

		TSG_Point	*pA	= m_Points;
		TSG_Point	*pB	= m_Points + m_nPoints - 1;

		for(int iPoint=0; iPoint<m_nPoints; iPoint++, pB=pA++)
		{
			if( y < pA->y )	// pA above ray
			{
				if( pB->y <= y )	// pB on or below ray
				{
					if( ((y - pB->y) * (pA->x - pB->x)) > ((x - pB->x) * (pA->y - pB->y)) )
					{
						nCrossings++;
					}
				}
			}
			else			// pA on or below ray
			{
				if( pB->y >  y )	// pB above ray
				{
					if( ((y - pB->y) * (pA->x - pB->x)) < ((x - pB->x) * (pA->y - pB->y)) )
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

//---------------------------------------------------------
double CSG_Shape_Polygon_Part::Get_Distance(TSG_Point Point, TSG_Point &Next)
{
	if( m_nPoints > 2 && !Contains(Point) )
	{
		double		Distance;
		TSG_Point	*pA, *pB, C;

		pB			= m_Points + m_nPoints - 1;
		pA			= m_Points;

		Distance	= SG_Get_Nearest_Point_On_Line(Point, *pA, *pB, Next);

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

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape_Polygon::CSG_Shape_Polygon(CSG_Shapes *pOwner, int Index)
	: CSG_Shape_Points(pOwner, Index)
{}

//---------------------------------------------------------
CSG_Shape_Polygon::~CSG_Shape_Polygon(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shape_Polygon::_Invalidate(void)
{
	CSG_Shape_Points::_Invalidate();

	if( m_bUpdate_Lakes )
	{
		m_bUpdate_Lakes	= false;

		for(int i=0; i<m_nParts; i++)
		{
			Get_Polygon_Part(i)->m_bLake	= BOOL3_NOTSET;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Intersection CSG_Shape_Polygon::On_Intersects(CSG_Shape *pShape)
{
	//-----------------------------------------------------
	int		iPart;

	bool	bIn		= false;
	bool	bOut	= false;

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
	for(iPart=0; iPart<Get_Part_Count(); iPart++)
	{
		if( Get_Point_Count(iPart) < 3 )
		{
			continue;
		}

		TSG_Point	A[2], B[2], C;	A[0]	= Get_Point(0, iPart, false);

		for(int iPoint=0; iPoint<Get_Point_Count(iPart); iPoint++)
		{
			if( pShape->Get_Type() == SHAPE_TYPE_Polygon )
			{
				if( ((CSG_Shape_Polygon *)pShape)->Contains(Get_Point(iPoint, iPart)) )
				{
					return( INTERSECTION_Overlaps );
				}
			}

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
				TSG_Point	*pa, *pb, c;

				pa	= pPart->m_Points;
				pb	= pa + pPart->m_nPoints - 1;

				for(int iPoint=0; iPoint<pPart->m_nPoints; iPoint++, pb=pa++)
				{
					if(	SG_Get_Crossing_InRegion(c, *pa, *pb, Region) )
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_Lake(int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	if( pPart && pPart->m_nPoints > 2 && m_nParts > 1 )
	{
		if( pPart->m_bLake == BOOL3_NOTSET )
		{
			int		nOuter	= 0;

			for(iPart=0; iPart<m_nParts; iPart++)
			{
				if( pPart != m_pParts[iPart] && m_pParts[iPart]->m_nPoints > 2 )
				{
					if( Contains(pPart->Get_Point(0), iPart) )
					{
						nOuter++;
					}
				}
			}

			pPart->m_bLake	= nOuter % 2 ? 1 : 0;

			m_bUpdate_Lakes	= true;
		}

		return( pPart->m_bLake == BOOL3_TRUE );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Polygon::Contains(const TSG_Point &Point, int iPart)
{
	return( Contains(Point.x, Point.y, iPart) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::Contains(const TSG_Point &Point)
{
	return( Contains(Point.x, Point.y) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::Contains(double x, double y, int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	return(	pPart && pPart->Contains(x, y) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::Contains(double x, double y)
{
	if( Get_Extent().Contains(x, y) )
	{
		int	nCrossings	= 0;

		for(int iPart=0; iPart<m_nParts; iPart++)
		{
			CSG_Shape_Part	*pPart	= m_pParts[iPart];

			if( pPart->m_nPoints > 2 && pPart->Get_Extent().Contains(x, y) )
			{
				TSG_Point	*pA	= pPart->m_Points;
				TSG_Point	*pB	= pPart->m_Points + pPart->m_nPoints - 1;

				for(int iPoint=0; iPoint<pPart->m_nPoints; iPoint++, pB=pA++)
				{
					if( y < pA->y )	// pA above ray
					{
						if( pB->y <= y )	// pB on or below ray
						{
							if( ((y - pB->y) * (pA->x - pB->x)) > ((x - pB->x) * (pA->y - pB->y)) )
							{
								nCrossings++;
							}
						}
					}
					else			// pA on or below ray
					{
						if( pB->y >  y )	// pB above ray
						{
							if( ((y - pB->y) * (pA->x - pB->x)) < ((x - pB->x) * (pA->y - pB->y)) )
							{
								nCrossings++;
							}
						}
					}
				}
			}
		}

		return( nCrossings % 2 != 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Distance(TSG_Point Point, TSG_Point &Next, int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	return(	pPart && pPart->Get_Distance(Point, Next) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
