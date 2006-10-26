
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
int CSG_Shape_Polygon::On_Intersects(TSG_Rect Region)
{
	int			iPart, iPoint;
	TSG_Point	pa, pb, top_left, top_right, bottom_left, bottom_right, Point;


	//-----------------------------------------------------
	// 1. Line Intersection...

	bottom_left.x	= top_left.x		= Region.xMin;
	bottom_right.x	= top_right.x		= Region.xMax;

	bottom_left.y	= bottom_right.y	= Region.yMin;
	top_left.y		= top_right.y		= Region.yMax;

	for(iPart=0; iPart<m_nParts; iPart++)
	{
		pb	= m_Points[iPart][m_nPoints[iPart] - 1];

		for(iPoint=0; iPoint<m_nPoints[iPart]; iPoint++)
		{
			pa	= pb;
			pb	= m_Points[iPart][iPoint];

			if(	SG_Get_Crossing(Point, pa, pb, bottom_left , bottom_right, true)
			||	SG_Get_Crossing(Point, pa, pb, bottom_left , top_left    , true)
			||	SG_Get_Crossing(Point, pa, pb, bottom_right, top_right   , true)
			||	SG_Get_Crossing(Point, pa, pb, top_left    , top_right   , true)	)
			{
				return( 1 );
			}
		}
	}


	//-----------------------------------------------------
	// 2. Is region completly within extent...

	if(	is_Containing(Region.xMin, Region.yMax)
	||	is_Containing(Region.xMin, Region.yMin)
	||	is_Containing(Region.xMax, Region.yMax)
	||	is_Containing(Region.xMax, Region.yMin)	)
	{
		return( 1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_Containing(const TSG_Point &Point)
{
	return( is_Containing(Point.x, Point.y) );
}

bool CSG_Shape_Polygon::is_Containing(double x, double y)
{
	int			iPart, iPoint, nCrossings;
	TSG_Point	A, B, C, *pA, *pB;

	if( Get_Extent().Contains(x, y) )
	{
		nCrossings	= 0;

		A.x			= m_Extent.m_rect.xMin;
		B.x			= x;
		A.y = B.y	= y;

		for(iPart=0; iPart<m_nParts; iPart++)
		{
			if( m_nPoints[iPart] > 2 )
			{
				pB	= m_Points[iPart] + m_nPoints[iPart] - 1;
				pA	= m_Points[iPart];

				for(iPoint=0; iPoint<m_nPoints[iPart]; iPoint++, pB=pA++)
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
bool CSG_Shape_Polygon::is_Containing(const TSG_Point &Point, int iPart)
{
	return( is_Containing(Point.x, Point.y, iPart) );
}

bool CSG_Shape_Polygon::is_Containing(double x, double y, int iPart)
{
	int			iPoint, nCrossings;
	TSG_Point	A, B, C, *pA, *pB;

	if(	m_Extent.Contains(x, y) && iPart >= 0 && iPart < m_nParts && m_nPoints[iPart] > 2 )
	{
		nCrossings	= 0;

		A.x			= m_Extent.m_rect.xMin;
		B.x			= x;
		A.y = B.y	= y;

		pB	= m_Points[iPart] + m_nPoints[iPart] - 1;
		pA	= m_Points[iPart];

		for(iPoint=0; iPoint<m_nPoints[iPart]; iPoint++, pB=pA++)
		{
			if( SG_Get_Crossing(C, *pA, *pB, A, B) )
			{
				nCrossings++;
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
bool CSG_Shape_Polygon::is_Clockwise(int iPart)
{
	return( _Get_Area(iPart) < 0.0 );
}

//---------------------------------------------------------
bool CSG_Shape_Polygon::is_Lake(int iPart)
{
	int		jPart;

	if( iPart >= 0 && iPart < m_nParts && m_nPoints[iPart] > 2 )
	{
		for(jPart=0; jPart<m_nParts; jPart++)
		{
			if( jPart != iPart && m_nPoints[jPart] > 2 )
			{
				if( is_Containing(m_Points[iPart][0], jPart) )
				{
					return( true );
				}
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Perimeter(void)
{
	int		iPart;
	double	Length;

	for(iPart=0, Length=0.0; iPart<m_nParts; iPart++)
	{
		Length	+= Get_Perimeter(iPart);
	}

	return( Length );
}

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Perimeter(int iPart)
{
	int			iPoint;
	double		Length;
	TSG_Point	*pA, *pB;

	if( iPart >= 0 && iPart < m_nParts && m_nPoints[iPart] > 2 )
	{
		pB	= m_Points[iPart] + m_nPoints[iPart] - 1;
		pA	= m_Points[iPart];

		for(iPoint=0, Length=0.0; iPoint<m_nPoints[iPart]; iPoint++, pB=pA++)
		{
			Length	+= SG_Get_Distance(*pA, *pB);
		}

		return( Length );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Area(void)
{
	int		iPart;
	double	Area;

	for(iPart=0, Area=0.0; iPart<m_nParts; iPart++)
	{
		Area	+= is_Lake(iPart) ? -Get_Area(iPart) : Get_Area(iPart);
	}

	return( Area );
}

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Area(int iPart)
{
	return( fabs(_Get_Area(iPart)) );
}

//---------------------------------------------------------
double CSG_Shape_Polygon::_Get_Area(int iPart)
{
	int			iPoint;
	double		Area;
	TSG_Point	*pA, *pB;

	Area	= 0.0;

	if( iPart >= 0 && iPart < m_nParts && m_nPoints[iPart] > 2 )
	{
		pB	= m_Points[iPart] + m_nPoints[iPart] - 1;
		pA	= m_Points[iPart];

		for(iPoint=0; iPoint<m_nPoints[iPart]; iPoint++, pB=pA++)
		{
			Area	+= (pA->x * pB->y) - (pB->x * pA->y);
		}

		Area	/= 2.0;
	}

	return( Area );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Point CSG_Shape_Polygon::Get_Centroid(void)
{
	int			iPart, iPoint;
	double		d, a;
	TSG_Point	c, *pA, *pB;

	c.x	= c.y	= a	= 0.0;

	for(iPart=0; iPart<m_nParts; iPart++)
	{
		if( m_nPoints[iPart] > 2 )
		{
			pB	= m_Points[iPart] + m_nPoints[iPart] - 1;
			pA	= m_Points[iPart];

			for(iPoint=0; iPoint<m_nPoints[iPart]; iPoint++, pB=pA++)
			{
				d	 = pA->x * pB->y - pB->x * pA->y;
				a	+= d;
				c.x	+= d * (pA->x + pB->x);
				c.y	+= d * (pA->y + pB->y);
			}
		}
	}

	if( (d = 3.0 * a) != 0.0 )
	{
		c.x	/= d;
		c.y	/= d;
	}

	return( c );
}

//---------------------------------------------------------
TSG_Point CSG_Shape_Polygon::Get_Centroid(int iPart)
{
	int			iPoint;
	double		d, a;
	TSG_Point	c, *pA, *pB;

	c.x	= c.y	= 0.0;

	if( iPart >= 0 && iPart < m_nParts && m_nPoints[iPart] > 2 )
	{
		a	= 0.0;

		pB	= m_Points[iPart] + m_nPoints[iPart] - 1;
		pA	= m_Points[iPart];

		for(iPoint=0; iPoint<m_nPoints[iPart]; iPoint++, pB=pA++)
		{
			d	 = pA->x * pB->y - pB->x * pA->y;
			a	+= d;
			c.x	+= d * (pA->x + pB->x);
			c.y	+= d * (pA->y + pB->y);
		}

		if( (d = 3.0 * a) != 0.0 )
		{
			c.x	/= d;
			c.y	/= d;
		}
	}

	return( c );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Distance(TSG_Point Point, TSG_Point &Next, int iPart)
{
	int			i;
	double		d, Distance;
	TSG_Point	*pA, *pB, pt;

	Distance	= -1.0;

	if( iPart >= 0 && iPart < m_nParts && m_nPoints[iPart] > 2 )
	{
		if( is_Containing(Point) )
		{
			Distance	= 0.0;
		}
		else
		{
			pB	= m_Points[iPart] + m_nPoints[iPart] - 1;
			pA	= m_Points[iPart];

			Distance	= SG_Get_Nearest_Point_On_Line(Point, *pA, *pB, Next);

			for(i=0; i<m_nPoints[iPart] && Distance!=0.0; i++, pB=pA++)
			{
				if(	(d = SG_Get_Nearest_Point_On_Line(Point, *pA, *pB, pt)) >= 0.0
				&&	(d < Distance || Distance < 0.0) )
				{
					Distance	= d;
					Next		= pt;
				}
			}
		}
	}

	return( Distance );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
