
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
			pb	= m_pParts[iPart]->m_Points + m_pParts[iPart]->Get_Count() - 1;

			for(int iPoint=0; iPoint<m_pParts[iPart]->Get_Count(); iPoint++)
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
bool CSG_Shape_Polygon::is_Containing(const TSG_Point &Point)
{
	return( is_Containing(Point.x, Point.y) );
}

bool CSG_Shape_Polygon::is_Containing(double x, double y)
{
	if( Get_Extent().Contains(x, y) )
	{
		int			nCrossings;
		TSG_Point	A, B, C, *pA, *pB;

		nCrossings	= 0;

		A.x			= m_Extent.m_rect.xMin;
		B.x			= x;
		A.y = B.y	= y;

		for(int iPart=0; iPart<m_nParts; iPart++)
		{
			CSG_Shape_Part	*pPart	= m_pParts[iPart];

			if( pPart->Get_Count() > 2 && pPart->Get_Extent().Contains(x, y) )
			{
				pB	= pPart->m_Points + pPart->Get_Count() - 1;
				pA	= pPart->m_Points;

				for(int iPoint=0, goNext=0; iPoint<pPart->Get_Count(); iPoint++, pB=pA++)
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

				/*	if( SG_Get_Crossing(C, *pA, *pB, A, B) )
					{
						nCrossings++;
					}/**/
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
	CSG_Shape_Part	*pPart	= Get_Part(iPart);

	if(	pPart && pPart->Get_Count() > 2 && pPart->Get_Extent().Contains(x, y) )
	{
		int			nCrossings;
		TSG_Point	A, B, C, *pA, *pB;

		nCrossings	= 0;

		A.x			= m_Extent.m_rect.xMin;
		B.x			= x;
		A.y = B.y	= y;

		pB	= pPart->m_Points + pPart->Get_Count() - 1;
		pA	= pPart->m_Points;

		for(int iPoint=0, goNext=0; iPoint<pPart->Get_Count(); iPoint++, pB=pA++)
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
	CSG_Shape_Part	*pPart	= Get_Part(iPart);

	if( iPart >= 0 && iPart < m_nParts && m_pParts[iPart]->Get_Count() > 2 )
	{
		int		nOuter	= 0;

		for(iPart=0; iPart<m_nParts; iPart++)
		{
			if( pPart != m_pParts[iPart] && m_pParts[iPart]->Get_Count() > 2 )
			{
				if( is_Containing(pPart->Get_Point(0), iPart) )
				{
					nOuter++;
				}
			}
		}

		return( nOuter % 2 != 0 );
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
	CSG_Shape_Part	*pPart	= Get_Part(iPart);

	if( pPart && pPart->Get_Count() > 2 )
	{
		int			iPoint;
		double		Length;
		TSG_Point	*pA, *pB;

		pB	= pPart->m_Points + pPart->Get_Count() - 1;
		pA	= pPart->m_Points;

		for(iPoint=0, Length=0.0; iPoint<pPart->Get_Count(); iPoint++, pB=pA++)
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
	CSG_Shape_Part	*pPart	= Get_Part(iPart);

	if( pPart && pPart->Get_Count() > 2 )
	{
		double		Area;
		TSG_Point	*pA, *pB;

		Area	= 0.0;

		pB		= pPart->m_Points + pPart->Get_Count() - 1;
		pA		= pPart->m_Points;

		for(int iPoint=0; iPoint<pPart->Get_Count(); iPoint++, pB=pA++)
		{
			Area	+= (pA->x * pB->y) - (pB->x * pA->y);
		}

		return( Area / 2.0 );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Point CSG_Shape_Polygon::Get_Centroid(void)
{
	double		d, a;
	TSG_Point	c, *pA, *pB;

	c.x	= c.y	= a	= 0.0;

	for(int iPart=0; iPart<m_nParts; iPart++)
	{
		CSG_Shape_Part	*pPart	= m_pParts[iPart];

		if( pPart->Get_Count() > 2 )
		{
			pB	= pPart->m_Points + pPart->Get_Count() - 1;
			pA	= pPart->m_Points;

			for(int iPoint=0; iPoint<pPart->Get_Count(); iPoint++, pB=pA++)
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
	CSG_Shape_Part	*pPart	= Get_Part(iPart);

	if( pPart && pPart->Get_Count() > 2 )
	{
		double		d, a;
		TSG_Point	c, *pA, *pB;

		c.x	= c.y	= a	= 0.0;

		pB	= pPart->m_Points + pPart->Get_Count() - 1;
		pA	= pPart->m_Points;

		for(int iPoint=0; iPoint<pPart->Get_Count(); iPoint++, pB=pA++)
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

		return( c );
	}

	return( CSG_Point(0.0, 0.0) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Polygon::Get_Distance(TSG_Point Point, TSG_Point &Next, int iPart)
{
	double	Distance;

	if( is_Containing(Point, iPart) )
	{
		Distance	=  0.0;
	}
	else
	{
		Distance	= -1.0;

		CSG_Shape_Part	*pPart	= Get_Part(iPart);

		if( pPart != NULL && pPart->Get_Count() > 2 )
		{
			double		d;
			TSG_Point	*pA, *pB, pt;

			pB	= pPart->m_Points + pPart->Get_Count() - 1;
			pA	= pPart->m_Points;

			Distance	= SG_Get_Nearest_Point_On_Line(Point, *pA, *pB, Next);

			for(int iPoint=0; iPoint<pPart->Get_Count() && Distance!=0.0; iPoint++, pB=pA++)
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
