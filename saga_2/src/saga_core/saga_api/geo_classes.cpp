
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
#include <memory.h>

#include "geo_tools.h"


///////////////////////////////////////////////////////////
//														 //
//						CSG_Point						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Point::CSG_Point(void)
{
	Assign(0.0, 0.0);
}

CSG_Point::CSG_Point(const CSG_Point &Point)
{
	Assign(Point);
}

CSG_Point::CSG_Point(const TSG_Point &Point)
{
	Assign(Point.x, Point.y);
}

CSG_Point::CSG_Point(double x, double y)
{
	Assign(x, y);
}

//---------------------------------------------------------
CSG_Point::~CSG_Point(void)
{
}

//---------------------------------------------------------
bool CSG_Point::operator == (const CSG_Point &Point) const
{
	return( is_Equal(Point) );
}

bool CSG_Point::operator != (const CSG_Point &Point) const
{
	return( !is_Equal(Point) );
}

CSG_Point & CSG_Point::operator = (const CSG_Point &Point)
{
	Assign(Point);

	return( *this );
}

void CSG_Point::operator += (const CSG_Point &Point)
{
	m_point.x	+= Point.Get_X();
	m_point.y	+= Point.Get_Y();
}

void CSG_Point::operator -= (const CSG_Point &Point)
{
	m_point.x	-= Point.Get_X();
	m_point.y	-= Point.Get_Y();
}

CSG_Point CSG_Point::operator + (const CSG_Point &Point) const
{
	return( CSG_Point(
		m_point.x + Point.Get_X(),
		m_point.y + Point.Get_Y())
	);
}

CSG_Point CSG_Point::operator - (const CSG_Point &Point) const
{
	return( CSG_Point(
		m_point.x - Point.Get_X(),
		m_point.y - Point.Get_Y())
	);
}

//---------------------------------------------------------
void CSG_Point::Assign(double x, double y)
{
	m_point.x	= x;
	m_point.y	= y;
}

void CSG_Point::Assign(const CSG_Point &Point)
{
	m_point		= Point.m_point;
}

//---------------------------------------------------------
bool CSG_Point::is_Equal(double x, double y) const
{
	return(	m_point.x == x && m_point.y == y );
}

bool CSG_Point::is_Equal(const CSG_Point &Point) const
{
	return(	is_Equal(Point.Get_X(), Point.Get_Y()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Points::CSG_Points(void)
{
	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
CSG_Points::~CSG_Points(void)
{
	Clear();
}

//---------------------------------------------------------
void CSG_Points::Clear(void)
{
	if( m_Points )
	{
		SG_Free(m_Points);
	}

	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
bool CSG_Points::Assign(const CSG_Points &Points)
{
	Set_Count(Points.m_nPoints);

	if( m_nPoints > 0 )
	{
		memcpy(m_Points, Points.m_Points, m_nPoints * sizeof(TSG_Point));
	}

	return( true );
}

//---------------------------------------------------------
CSG_Points & CSG_Points::operator  = (const CSG_Points &Points)
{
	Assign(Points);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Points::Set_Count(int nPoints)
{
	m_nPoints	= nPoints;
	m_Points	= (TSG_Point *)SG_Realloc(m_Points, m_nPoints * sizeof(TSG_Point));

	return( true );
}

//---------------------------------------------------------
bool CSG_Points::Add(double x, double y)
{
	m_Points	= (TSG_Point *)SG_Realloc(m_Points, (m_nPoints + 1) * sizeof(TSG_Point));
	m_Points[m_nPoints].x	= x;
	m_Points[m_nPoints].y	= y;
	m_nPoints++;

	return( true );
}

//---------------------------------------------------------
bool CSG_Points::Add(const TSG_Point &Point)
{
	return( Add(Point.x, Point.y) );
}

//---------------------------------------------------------
bool CSG_Points::Del(int Index)
{
	if( Index >= 0 && Index < m_nPoints )
	{
		m_nPoints--;

		if( m_nPoints > 0 )
		{
			for(TSG_Point *A=m_Points+Index, *B=m_Points+Index+1; Index<m_nPoints; Index++, A++, B++)
			{
				*A	= *B;
			}

			m_Points	= (TSG_Point *)SG_Realloc(m_Points, m_nPoints * sizeof(TSG_Point));
		}
		else
		{
			SG_Free(m_Points);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Points_Int::CSG_Points_Int(void)
{
	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
CSG_Points_Int::~CSG_Points_Int(void)
{
	Clear();
}

//---------------------------------------------------------
void CSG_Points_Int::Clear(void)
{
	if( m_Points )
	{
		SG_Free(m_Points);
	}

	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
bool CSG_Points_Int::Assign(const CSG_Points_Int &Points)
{
	Set_Count(Points.m_nPoints);

	if( m_nPoints > 0 )
	{
		memcpy(m_Points, Points.m_Points, m_nPoints * sizeof(TSG_Point_Int));
	}

	return( true );
}

//---------------------------------------------------------
CSG_Points_Int & CSG_Points_Int::operator  = (const CSG_Points_Int &Points)
{
	Assign(Points);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Points_Int::Set_Count(int nPoints)
{
	m_nPoints	= nPoints;
	m_Points	= (TSG_Point_Int *)SG_Realloc(m_Points, m_nPoints * sizeof(TSG_Point_Int));

	return( true );
}

//---------------------------------------------------------
bool CSG_Points_Int::Add(int x, int y)
{
	m_Points	= (TSG_Point_Int *)SG_Realloc(m_Points, (m_nPoints + 1) * sizeof(TSG_Point_Int));
	m_Points[m_nPoints].x	= x;
	m_Points[m_nPoints].y	= y;
	m_nPoints++;

	return( true );
}

//---------------------------------------------------------
bool CSG_Points_Int::Add(const TSG_Point_Int &Point)
{
	return( Add(Point.x, Point.y) );
}

//---------------------------------------------------------
bool CSG_Points_Int::Del(int Index)
{
	if( Index >= 0 && Index < m_nPoints )
	{
		m_nPoints--;

		if( m_nPoints > 0 )
		{
			for(TSG_Point_Int *A=m_Points+Index, *B=m_Points+Index+1; Index<m_nPoints; Index++, A++, B++)
			{
				*A	= *B;
			}

			m_Points	= (TSG_Point_Int *)SG_Realloc(m_Points, m_nPoints * sizeof(TSG_Point_Int));
		}
		else
		{
			SG_Free(m_Points);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Points_3D::CSG_Points_3D(void)
{
	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
CSG_Points_3D::~CSG_Points_3D(void)
{
	Clear();
}

//---------------------------------------------------------
void CSG_Points_3D::Clear(void)
{
	if( m_Points )
	{
		SG_Free(m_Points);
	}

	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
bool CSG_Points_3D::Assign(const CSG_Points_3D &Points)
{
	Set_Count(Points.m_nPoints);

	if( m_nPoints > 0 )
	{
		memcpy(m_Points, Points.m_Points, m_nPoints * sizeof(TSG_Point_3D));
	}

	return( true );
}

//---------------------------------------------------------
CSG_Points_3D & CSG_Points_3D::operator  = (const CSG_Points_3D &Points)
{
	Assign(Points);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Points_3D::Set_Count(int nPoints)
{
	m_nPoints	= nPoints;
	m_Points	= (TSG_Point_3D *)SG_Realloc(m_Points, m_nPoints * sizeof(TSG_Point_3D));

	return( true );
}

//---------------------------------------------------------
bool CSG_Points_3D::Add(double x, double y, double z)
{
	m_Points	= (TSG_Point_3D *)SG_Realloc(m_Points, (m_nPoints + 1) * sizeof(TSG_Point_3D));
	m_Points[m_nPoints].x	= x;
	m_Points[m_nPoints].y	= y;
	m_Points[m_nPoints].z	= z;
	m_nPoints++;

	return( true );
}

//---------------------------------------------------------
bool CSG_Points_3D::Add(const TSG_Point_3D &Point)
{
	return( Add(Point.x, Point.y, Point.z) );
}

//---------------------------------------------------------
bool CSG_Points_3D::Del(int Index)
{
	if( Index >= 0 && Index < m_nPoints )
	{
		m_nPoints--;

		if( m_nPoints > 0 )
		{
			for(TSG_Point_3D *A=m_Points+Index, *B=m_Points+Index+1; Index<m_nPoints; Index++, A++, B++)
			{
				*A	= *B;
			}

			m_Points	= (TSG_Point_3D *)SG_Realloc(m_Points, m_nPoints * sizeof(TSG_Point_3D));
		}
		else
		{
			SG_Free(m_Points);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						CSG_Rect						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rect::CSG_Rect(void)
{
	Assign(0.0, 0.0, 0.0, 0.0);
}

CSG_Rect::CSG_Rect(const CSG_Rect &Rect)
{
	Assign(Rect.m_rect);
}

CSG_Rect::CSG_Rect(const TSG_Rect &Rect)
{
	Assign(Rect.xMin, Rect.yMin, Rect.xMax, Rect.yMax);
}

CSG_Rect::CSG_Rect(const CSG_Point &A, const CSG_Point &B)
{
	Assign(A.Get_X(), A.Get_Y(), B.Get_X(), B.Get_Y());
}

CSG_Rect::CSG_Rect(double xMin, double yMin, double xMax, double yMax)
{
	Assign(xMin, yMin, xMax, yMax);
}

//---------------------------------------------------------
CSG_Rect::~CSG_Rect(void)
{}

//---------------------------------------------------------
bool CSG_Rect::operator == (const CSG_Rect &Rect) const
{
	return( is_Equal(Rect) );
}

bool CSG_Rect::operator != (const CSG_Rect &Rect) const
{
	return( !is_Equal(Rect) );
}

CSG_Rect & CSG_Rect::operator = (const CSG_Rect &Rect)
{
	Assign(Rect);

	return( *this );
}

void CSG_Rect::operator += (const CSG_Point &Point)
{
	Move( Point.Get_X(),  Point.Get_Y());
}

void CSG_Rect::operator -= (const CSG_Point &Point)
{
	Move(-Point.Get_Y(), -Point.Get_Y());
}

//---------------------------------------------------------
void CSG_Rect::Assign(double xMin, double yMin, double xMax, double yMax)
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

void CSG_Rect::Assign(const CSG_Point &A, const CSG_Point &B)
{
	Assign(A.Get_X(), A.Get_Y(), B.Get_X(), B.Get_Y());
}

void CSG_Rect::Assign(const CSG_Rect &Rect)
{
	Assign(Rect.Get_XMin(), Rect.Get_YMin(), Rect.Get_XMax(), Rect.Get_YMax());
}

//---------------------------------------------------------
void CSG_Rect::Set_BottomLeft(double x, double y)
{
	Assign(x, y, m_rect.xMax, m_rect.yMax);
}

void CSG_Rect::Set_BottomLeft(const CSG_Point &Point)
{
	Set_BottomLeft(Point.Get_X(), Point.Get_Y() );
}

void CSG_Rect::Set_TopRight(double x, double y)
{
	Assign(m_rect.xMin, m_rect.yMin, x, y);
}

void CSG_Rect::Set_TopRight(const CSG_Point &Point)
{
	Set_TopRight(Point.Get_X(), Point.Get_Y() );
}

//---------------------------------------------------------
bool CSG_Rect::is_Equal(double xMin, double yMin, double xMax, double yMax) const
{
	return(	m_rect.xMin == xMin && m_rect.yMin == yMin
		&&	m_rect.xMax == xMax && m_rect.yMax == yMax	);
}

bool CSG_Rect::is_Equal(const CSG_Rect &Rect) const
{
	return(	is_Equal(Rect.Get_XMin(), Rect.Get_YMin(), Rect.Get_XMax(), Rect.Get_YMax()) );
}

//---------------------------------------------------------
void CSG_Rect::Move(double dx, double dy)
{
	m_rect.xMin	+= dx;
	m_rect.yMin	+= dy;
	m_rect.xMax	+= dx;
	m_rect.yMax	+= dy;
}

void CSG_Rect::Move(const CSG_Point &Point)
{
	Move(Point.Get_X(), Point.Get_Y());
}

//---------------------------------------------------------
void CSG_Rect::Inflate(double dx, double dy, bool bPercent)
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

void CSG_Rect::Inflate(double d, bool bPercent)
{
	Inflate(d, d, bPercent);
}

void CSG_Rect::Deflate(double dx, double dy, bool bPercent)
{
	Inflate(-dx, -dy, bPercent);
}

void CSG_Rect::Deflate(double d, bool bPercent)
{
	Deflate(d, d, bPercent);
}

//---------------------------------------------------------
void CSG_Rect::Union(const CSG_Rect &Rect)
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
bool CSG_Rect::Intersect(const CSG_Rect &Rect)
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
TSG_Intersection CSG_Rect::Intersects(const CSG_Rect &Rect) const
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
bool CSG_Rect::Contains(double x, double y) const
{
	return(	m_rect.xMin <= x && x <= m_rect.xMax
		&&	m_rect.yMin <= y && y <= m_rect.yMax
	);
}

bool CSG_Rect::Contains(const CSG_Point &Point) const
{
	return( Contains(Point.Get_X(), Point.Get_Y()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rects::CSG_Rects(void)
{
	m_nRects	= 0;
	m_Rects		= NULL;
}

//---------------------------------------------------------
CSG_Rects::~CSG_Rects(void)
{
	Clear();
}

//---------------------------------------------------------
void CSG_Rects::Clear(void)
{
	if( m_Rects )
	{
		for(int i=0; i<m_nRects; i++)
		{
			delete(m_Rects[i]);
		}

		SG_Free(m_Rects);
	}

	m_nRects	= 0;
	m_Rects		= NULL;
}

//---------------------------------------------------------
bool CSG_Rects::Assign(const CSG_Rects &Rects)
{
	Clear();

	for(int i=0; i<Rects.m_nRects; i++)
	{
		Add(*Rects.m_Rects[i]);
	}

	return( true );
}

//---------------------------------------------------------
CSG_Rects & CSG_Rects::operator  = (const CSG_Rects &Rects)
{
	Assign(Rects);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Rects::Add(void)
{
	return( Add(CSG_Rect()) );
}

//---------------------------------------------------------
bool CSG_Rects::Add(double xMin, double yMin, double xMax, double yMax)
{
	return( Add(CSG_Rect(xMin, yMin, xMax, yMax)) );
}

//---------------------------------------------------------
bool CSG_Rects::Add(const CSG_Rect &Rect)
{
	m_Rects				= (CSG_Rect **)SG_Realloc(m_Rects, (m_nRects + 1) * sizeof(CSG_Rect *));
	m_Rects[m_nRects]	= new CSG_Rect(Rect);
	m_nRects++;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
