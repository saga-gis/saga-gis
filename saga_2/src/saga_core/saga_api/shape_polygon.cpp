
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
		TSG_Point	*pA, *pB;

		m_Area			= 0.0;
		m_Perimeter		= 0.0;

		m_Centroid.x	= 0.0;
		m_Centroid.y	= 0.0;

		pB				= m_Points + m_nPoints - 1;
		pA				= m_Points;

		for(int iPoint=0; iPoint<m_nPoints; iPoint++, pB=pA++)
		{
			double	d		 = pA->x * pB->y - pB->x * pA->y;

			m_Centroid.x	+= d * (pA->x + pB->x);
			m_Centroid.y	+= d * (pA->y + pB->y);

			m_Area			+= d;

			m_Perimeter		+= SG_Get_Distance(*pA, *pB);
		}

		if( m_Area != 0.0 )
		{
			m_Centroid.x	/= (3.0 * m_Area);
			m_Centroid.y	/= (3.0 * m_Area);
		}

		m_bClockwise	= m_Area < 0.0 ? BOOL3_TRUE : BOOL3_FALSE;

		m_Area			= fabs(m_Area) / 2.0;
	}
}

//---------------------------------------------------------
bool CSG_Shape_Polygon_Part::is_Containing(const TSG_Point &Point)
{
	return( is_Containing(Point.x, Point.y) );
}

bool CSG_Shape_Polygon_Part::is_Containing(double x, double y)
{
	if(	m_nPoints > 2 && Get_Extent().Contains(x, y) )
	{
		int			nCrossings;
		TSG_Point	A, B, C, *pA, *pB;

		nCrossings	= 0;

		A.x			= Get_Extent().Get_XMin();
		B.x			= x;
		A.y = B.y	= y;

		pB	= m_Points + m_nPoints - 1;
		pA	= m_Points;

		for(int iPoint=0, goNext=0; iPoint<m_nPoints; iPoint++, pB=pA++)
		{
			if( pA->y != pB->y )
			{
				if( pA->y == y )
				{
					goNext	= pA->y > pB->y ? 1 : -1;
				}
				else if( goNext )	// pB->y == y
				{
					if( ((goNext > 0 && pA->y > pB->y) || (goNext < 0 && pA->y < pB->y)) && pB->x <= B.x )
						nCrossings++;

					goNext	= 0;
				}
				else if( ((pB->y < y && y <= pA->y) || (pB->y > y && y >= pA->y)) && (pB->x < x || pA->x < x) )
				{
					if( SG_Get_Crossing(C, *pA, *pB, A, B) )
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
	if( m_nPoints > 2 && !is_Containing(Point) )
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
CSG_Shape_Polygon::CSG_Shape_Polygon(CSG_Shapes *pOwner, CSG_Table_Record *pRecord)
	: CSG_Shape_Points(pOwner, pRecord)
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
int CSG_Shape_Polygon::On_Intersects(TSG_Rect Region)
{
	TSG_Point	*pa, *pb, r00, r10, r01, r11, c;

	//-----------------------------------------------------
	// 1. Line Intersection...

	r00.x	= r01.x	= Region.xMin;
	r11.x	= r10.x	= Region.xMax;
	r00.y	= r10.y	= Region.yMin;
	r11.y	= r01.y	= Region.yMax;

	for(int iPart=0; iPart<m_nParts; iPart++)
	{
		CSG_Shape_Part	*pPart	= m_pParts[iPart];

		if( pPart->Get_Extent().Intersects(Region) )
		{
			pb	= m_pParts[iPart]->m_Points + m_pParts[iPart]->m_nPoints - 1;

			for(int iPoint=0; iPoint<m_pParts[iPart]->m_nPoints; iPoint++)
			{
				pa	= pb;
				pb	= m_pParts[iPart]->m_Points + iPoint;

				if(	SG_Get_Crossing(c, *pa, *pb, r00, r10, true)
				||	SG_Get_Crossing(c, *pa, *pb, r00, r01, true)
				||	SG_Get_Crossing(c, *pa, *pb, r11, r10, true)
				||	SG_Get_Crossing(c, *pa, *pb, r11, r01, true)	)
				{
					return( INTERSECTION_Overlaps );
				}
			}
		}
	}


	//-----------------------------------------------------
	// 2. Is region completly within polygon...

	if(	is_Containing(r00)
	||	is_Containing(r01)
	||	is_Containing(r11)
	||	is_Containing(r10)	)
	{
		return( INTERSECTION_Contains );
	}

	return( INTERSECTION_None );
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
					if( is_Containing(pPart->Get_Point(0), iPart) )
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
	int			iPart, nParts;
	TSG_Point	Centroid;

	Centroid.x	= 0.0;
	Centroid.y	= 0.0;

	for(iPart=0, nParts=0; iPart<m_nParts; iPart++)
	{
		if( !is_Lake(iPart) )
		{
			TSG_Point	p	= Get_Centroid(iPart);

			Centroid.x	+= p.x;
			Centroid.y	+= p.y;

			nParts++;
		}
	}

	if( nParts > 1 )
	{
		Centroid.x	/= nParts;
		Centroid.y	/= nParts;
	}

	return( Centroid );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_Containing(const TSG_Point &Point, int iPart)
{
	return( is_Containing(Point.x, Point.y, iPart) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_Containing(const TSG_Point &Point)
{
	return( is_Containing(Point.x, Point.y) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_Containing(double x, double y, int iPart)
{
	CSG_Shape_Polygon_Part	*pPart	= Get_Polygon_Part(iPart);

	return(	pPart && pPart->is_Containing(x, y) );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_Containing(double x, double y)
{
	if( Get_Extent().Contains(x, y) )
	{
		int			nCrossings;
		TSG_Point	A, B, C, *pA, *pB;

		nCrossings	= 0;

		A.x			= Get_Extent().Get_XMin();
		B.x			= x;
		A.y = B.y	= y;

		for(int iPart=0; iPart<m_nParts; iPart++)
		{
			CSG_Shape_Part	*pPart	= m_pParts[iPart];

			if( pPart->m_nPoints > 2 && pPart->Get_Extent().Contains(x, y) )
			{
				pB	= pPart->m_Points + pPart->m_nPoints - 1;
				pA	= pPart->m_Points;

				for(int iPoint=0, goNext=0; iPoint<pPart->m_nPoints; iPoint++, pB=pA++)
				{
					if( pA->y != pB->y )
					{
						if( pA->y == y )
						{
							goNext	= pA->y > pB->y ? 1 : -1;
						}
						else if( goNext )	// pB->y == y
						{
							if( ((goNext > 0 && pA->y > pB->y) || (goNext < 0 && pA->y < pB->y)) && pB->x <= B.x )
								nCrossings++;

							goNext	= 0;
						}
						else if( ((pB->y < y && y <= pA->y) || (pB->y > y && y >= pA->y)) && (pB->x < x || pA->x < x) )
						{
							if( SG_Get_Crossing(C, *pA, *pB, A, B) )
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
