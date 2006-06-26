
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
//                   geo_classes.cpp                     //
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
#include "geo_tools.h"

//---------------------------------------------------------
#define	IS_BETWEEN(a, x, b)		((a <= x && x <= b) || (b <= x && x <= a))


///////////////////////////////////////////////////////////
//														 //
//						CGEO_Point						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGEO_Point::CGEO_Point(void)
{
	Assign(0.0, 0.0);
}

CGEO_Point::CGEO_Point(const CGEO_Point &Point)
{
	Assign(Point);
}

CGEO_Point::CGEO_Point(const TGEO_Point &Point)
{
	Assign(Point.x, Point.y);
}

CGEO_Point::CGEO_Point(double x, double y)
{
	Assign(x, y);
}

//---------------------------------------------------------
CGEO_Point::~CGEO_Point(void)
{
}

//---------------------------------------------------------
bool CGEO_Point::operator == (const CGEO_Point &Point) const
{
	return( is_Equal(Point) );
}

bool CGEO_Point::operator != (const CGEO_Point &Point) const
{
	return( !is_Equal(Point) );
}

CGEO_Point & CGEO_Point::operator = (const CGEO_Point &Point)
{
	Assign(Point);

	return( *this );
}

void CGEO_Point::operator += (const CGEO_Point &Point)
{
	m_point.x	+= Point.Get_X();
	m_point.y	+= Point.Get_Y();
}

void CGEO_Point::operator -= (const CGEO_Point &Point)
{
	m_point.x	-= Point.Get_X();
	m_point.y	-= Point.Get_Y();
}

CGEO_Point CGEO_Point::operator + (const CGEO_Point &Point) const
{
	return( CGEO_Point(
		m_point.x + Point.Get_X(),
		m_point.y + Point.Get_Y())
	);
}

CGEO_Point CGEO_Point::operator - (const CGEO_Point &Point) const
{
	return( CGEO_Point(
		m_point.x - Point.Get_X(),
		m_point.y - Point.Get_Y())
	);
}

//---------------------------------------------------------
void CGEO_Point::Assign(double x, double y)
{
	m_point.x	= x;
	m_point.y	= y;
}

void CGEO_Point::Assign(const CGEO_Point &Point)
{
	m_point		= Point.m_point;
}

//---------------------------------------------------------
bool CGEO_Point::is_Equal(double x, double y) const
{
	return(	m_point.x == x && m_point.y == y );
}

bool CGEO_Point::is_Equal(const CGEO_Point &Point) const
{
	return(	is_Equal(Point.Get_X(), Point.Get_Y()) );
}


///////////////////////////////////////////////////////////
//														 //
//						CGEO_Rect						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGEO_Rect::CGEO_Rect(void)
{
	Assign(0.0, 0.0, 0.0, 0.0);
}

CGEO_Rect::CGEO_Rect(const CGEO_Rect &Rect)
{
	Assign(Rect.m_rect);
}

CGEO_Rect::CGEO_Rect(const TGEO_Rect &Rect)
{
	Assign(Rect.xMin, Rect.yMin, Rect.xMax, Rect.yMax);
}

CGEO_Rect::CGEO_Rect(const CGEO_Point &A, const CGEO_Point &B)
{
	Assign(A.Get_X(), A.Get_Y(), B.Get_X(), B.Get_Y());
}

CGEO_Rect::CGEO_Rect(double xMin, double yMin, double xMax, double yMax)
{
	Assign(xMin, yMin, xMax, yMax);
}

//---------------------------------------------------------
CGEO_Rect::~CGEO_Rect(void)
{
}

//---------------------------------------------------------
bool CGEO_Rect::operator == (const CGEO_Rect &Rect) const
{
	return( is_Equal(Rect) );
}

bool CGEO_Rect::operator != (const CGEO_Rect &Rect) const
{
	return( !is_Equal(Rect) );
}

CGEO_Rect & CGEO_Rect::operator = (const CGEO_Rect &Rect)
{
	Assign(Rect);

	return( *this );
}

void CGEO_Rect::operator += (const CGEO_Point &Point)
{
	Move( Point.Get_X(),  Point.Get_Y());
}

void CGEO_Rect::operator -= (const CGEO_Point &Point)
{
	Move(-Point.Get_Y(), -Point.Get_Y());
}

//---------------------------------------------------------
void CGEO_Rect::Assign(double xMin, double yMin, double xMax, double yMax)
{
	double	d;

	m_rect.xMin	= xMin;
	m_rect.yMin	= yMin;
	m_rect.xMax	= xMax;
	m_rect.yMax	= yMax;

	if( m_rect.xMin > m_rect.xMax )
	{
		d			= m_rect.xMin;
		m_rect.xMin	= m_rect.xMax;
		m_rect.xMax	= d;
	}

	if( m_rect.yMin > m_rect.yMax )
	{
		d			= m_rect.yMin;
		m_rect.yMin	= m_rect.yMax;
		m_rect.yMax	= d;
	}
}

void CGEO_Rect::Assign(const CGEO_Point &A, const CGEO_Point &B)
{
	Assign(A.Get_X(), A.Get_Y(), B.Get_X(), B.Get_Y());
}

void CGEO_Rect::Assign(const CGEO_Rect &Rect)
{
	Assign(Rect.Get_XMin(), Rect.Get_YMin(), Rect.Get_XMax(), Rect.Get_YMax());
}

//---------------------------------------------------------
void CGEO_Rect::Set_BottomLeft(double x, double y)
{
	Assign(x, y, m_rect.xMax, m_rect.yMax);
}

void CGEO_Rect::Set_BottomLeft(const CGEO_Point &Point)
{
	Set_BottomLeft(Point.Get_X(), Point.Get_Y() );
}

void CGEO_Rect::Set_TopRight(double x, double y)
{
	Assign(m_rect.xMin, m_rect.yMin, x, y);
}

void CGEO_Rect::Set_TopRight(const CGEO_Point &Point)
{
	Set_TopRight(Point.Get_X(), Point.Get_Y() );
}

//---------------------------------------------------------
bool CGEO_Rect::is_Equal(double xMin, double yMin, double xMax, double yMax) const
{
	return(	m_rect.xMin == xMin && m_rect.yMin == yMin
		&&	m_rect.xMax == xMax && m_rect.yMax == yMax	);
}

bool CGEO_Rect::is_Equal(const CGEO_Rect &Rect) const
{
	return(	is_Equal(Rect.Get_XMin(), Rect.Get_YMin(), Rect.Get_XMax(), Rect.Get_YMax()) );
}

//---------------------------------------------------------
void CGEO_Rect::Move(double dx, double dy)
{
	m_rect.xMin	+= dx;
	m_rect.yMin	+= dy;
	m_rect.xMax	+= dx;
	m_rect.yMax	+= dy;
}

void CGEO_Rect::Move(const CGEO_Point &Point)
{
	Move(Point.Get_X(), Point.Get_Y());
}

//---------------------------------------------------------
void CGEO_Rect::Inflate(double dx, double dy, bool bPercent)
{
	if( bPercent )
	{
		dx	= (Get_XRange() * 0.01 * dx) / 2.0;
		dy	= (Get_YRange() * 0.01 * dy) / 2.0;
	}

	Assign(
		m_rect.xMin - dx, m_rect.yMin - dy,
		m_rect.xMax + dx, m_rect.yMax + dy
	);
}

void CGEO_Rect::Inflate(double d, bool bPercent)
{
	Inflate(d, d, bPercent);
}

void CGEO_Rect::Deflate(double dx, double dy, bool bPercent)
{
	Inflate(-dx, -dy, bPercent);
}

void CGEO_Rect::Deflate(double d, bool bPercent)
{
	Deflate(d, d, bPercent);
}

//---------------------------------------------------------
void CGEO_Rect::Union(const CGEO_Rect &Rect)
{
	if( m_rect.xMin > Rect.Get_XMin() )
	{
		m_rect.xMin	= Rect.Get_XMin();
	}

	if( m_rect.yMin > Rect.Get_YMin() )
	{
		m_rect.yMin	= Rect.Get_YMin();
	}

	if( m_rect.xMax < Rect.Get_XMax() )
	{
		m_rect.xMax	= Rect.Get_XMax();
	}

	if( m_rect.yMax < Rect.Get_YMax() )
	{
		m_rect.yMax	= Rect.Get_YMax();
	}
}

//---------------------------------------------------------
bool CGEO_Rect::Intersect(const CGEO_Rect &Rect)
{
	switch( Intersects(Rect) )
	{
	case INTERSECTION_None: default:
		return( false );

	case INTERSECTION_Identical:
	case INTERSECTION_Contained:
		break;

	case INTERSECTION_Contains:
		m_rect	= Rect.m_rect;
		break;

	case INTERSECTION_Overlaps:
		if( m_rect.xMin < Rect.Get_XMin() )
		{
			m_rect.xMin	= Rect.Get_XMin();
		}

		if( m_rect.yMin < Rect.Get_YMin() )
		{
			m_rect.yMin	= Rect.Get_YMin();
		}

		if( m_rect.xMax > Rect.Get_XMax() )
		{
			m_rect.xMax	= Rect.Get_XMax();
		}

		if( m_rect.yMax > Rect.Get_YMax() )
		{
			m_rect.yMax	= Rect.Get_YMax();
		}
		break;
	}

	return( true );
}

//---------------------------------------------------------
TGEO_Intersection CGEO_Rect::Intersects(const CGEO_Rect &Rect) const
{
	if(	m_rect.xMax < Rect.Get_XMin() || Rect.Get_XMax() < m_rect.xMin
	||	m_rect.yMax < Rect.Get_YMin() || Rect.Get_YMax() < m_rect.yMin )
	{
		return( INTERSECTION_None );
	}

	if(	is_Equal(Rect) )
	{
		return( INTERSECTION_Identical );
	}

	if(	Contains(Rect.Get_XMin(), Rect.Get_YMin())
	&&	Contains(Rect.Get_XMax(), Rect.Get_YMax()) )
	{
		return( INTERSECTION_Contains );
	}

	if(	Rect.Contains(Get_XMin(), Get_YMin())
	&&	Rect.Contains(Get_XMax(), Get_YMax()) )
	{
		return( INTERSECTION_Contained );
	}

	return( INTERSECTION_Overlaps );
}

//---------------------------------------------------------
bool CGEO_Rect::Contains(double x, double y) const
{
	return(	m_rect.xMin <= x && x <= m_rect.xMax
		&&	m_rect.yMin <= y && y <= m_rect.yMax
	);
}

bool CGEO_Rect::Contains(const CGEO_Point &Point) const
{
	return( Contains(Point.Get_X(), Point.Get_Y()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGEO_Rects::CGEO_Rects(void)
{
	m_nRects	= 0;
	m_Rects		= NULL;
}

//---------------------------------------------------------
CGEO_Rects::~CGEO_Rects(void)
{
	Clear();
}

//---------------------------------------------------------
void CGEO_Rects::Clear(void)
{
	if( m_Rects )
	{
		for(int i=0; i<m_nRects; i++)
		{
			delete(m_Rects[i]);
		}

		API_Free(m_Rects);
	}

	m_nRects	= 0;
	m_Rects		= NULL;
}

//---------------------------------------------------------
bool CGEO_Rects::Assign(const CGEO_Rects &Rects)
{
	Clear();

	for(int i=0; i<Rects.m_nRects; i++)
	{
		Add(*Rects.m_Rects[i]);
	}

	return( true );
}

//---------------------------------------------------------
CGEO_Rects & CGEO_Rects::operator  = (const CGEO_Rects &Rects)
{
	Assign(Rects);

	return( *this );
}

//---------------------------------------------------------
bool CGEO_Rects::Add(void)
{
	return( Add(CGEO_Rect()) );
}

//---------------------------------------------------------
bool CGEO_Rects::Add(double xMin, double yMin, double xMax, double yMax)
{
	return( Add(CGEO_Rect(xMin, yMin, xMax, yMax)) );
}

//---------------------------------------------------------
bool CGEO_Rects::Add(const CGEO_Rect &Rect)
{
	m_Rects				= (CGEO_Rect **)API_Realloc(m_Rects, (m_nRects + 1) * sizeof(CGEO_Rect *));
	m_Rects[m_nRects]	= new CGEO_Rect(Rect);
	m_nRects++;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
