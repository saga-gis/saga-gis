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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "parameters.h"

#include "geo_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define BUFFER_SIZE_GROW(size)		(size < 1024 ? 32 : 1024)


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
void CSG_Point::Add(const CSG_Point &Point)
{
	m_x	+= Point.m_x;
	m_y	+= Point.m_y;
}

void CSG_Point::Subtract(const CSG_Point &Point)
{
	m_x	-= Point.Get_X();
	m_y	-= Point.Get_Y();
}

//---------------------------------------------------------
void CSG_Point::Assign(double x, double y)
{
	m_x	= x;
	m_y	= y;
}

void CSG_Point::Assign(const CSG_Point &Point)
{
	m_x	= Point.m_x;
	m_y	= Point.m_y;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Point_Z::CSG_Point_Z(void)
{
	Assign(0.0, 0.0, 0.0);
}

CSG_Point_Z::CSG_Point_Z(const CSG_Point_Z &Point)
{
	Assign(Point);
}

CSG_Point_Z::CSG_Point_Z(const TSG_Point_Z &Point)
{
	Assign(Point.x, Point.y, Point.z);
}

CSG_Point_Z::CSG_Point_Z(double x, double y, double z)
{
	Assign(x, y, z);
}

//---------------------------------------------------------
void CSG_Point_Z::Add(const CSG_Point_Z &Point)
{
	m_x	+= Point.m_x;
	m_y	+= Point.m_y;
	m_z	+= Point.m_z;
}

void CSG_Point_Z::Subtract(const CSG_Point_Z &Point)
{
	m_x	-= Point.m_x;
	m_y	-= Point.m_y;
	m_z	-= Point.m_z;
}

//---------------------------------------------------------
void CSG_Point_Z::Assign(double x, double y, double z)
{
	m_x	= x;
	m_y	= y;
	m_z	= z;
}

void CSG_Point_Z::Assign(const CSG_Point_Z &Point)
{
	m_x	= Point.m_x;
	m_y	= Point.m_y;
	m_z	= Point.m_z;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Point_ZM::CSG_Point_ZM(void)
{
	Assign(0.0, 0.0, 0.0, 0.0);
}

CSG_Point_ZM::CSG_Point_ZM(const CSG_Point_ZM &Point)
{
	Assign(Point);
}

CSG_Point_ZM::CSG_Point_ZM(const TSG_Point_ZM &Point)
{
	Assign(Point.x, Point.y, Point.z, Point.m);
}

CSG_Point_ZM::CSG_Point_ZM(double x, double y, double z, double m)
{
	Assign(x, y, z, m);
}

//---------------------------------------------------------
void CSG_Point_ZM::Add(const CSG_Point_ZM &Point)
{
	m_x	+= Point.m_x;
	m_y	+= Point.m_y;
	m_z	+= Point.m_z;
	m_m	+= Point.m_m;
}

void CSG_Point_ZM::Subtract(const CSG_Point_ZM &Point)
{
	m_x	-= Point.m_x;
	m_y	-= Point.m_y;
	m_z	-= Point.m_z;
	m_m	-= Point.m_m;
}

//---------------------------------------------------------
void CSG_Point_ZM::Assign(double x, double y, double z, double m)
{
	m_x	= x;
	m_y	= y;
	m_z	= z;
	m_m	= m;
}

void CSG_Point_ZM::Assign(const CSG_Point_ZM &Point)
{
	m_x	= Point.m_x;
	m_y	= Point.m_y;
	m_z	= Point.m_z;
	m_m	= Point.m_m;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Points::CSG_Points(void)
{
	m_nBuffer	= 0;
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

	m_nBuffer	= 0;
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
	if( m_nPoints == nPoints )
	{
		return( true );
	}

	if( nPoints <= 0 )
	{
		Clear();

		return( true );
	}

	TSG_Point	*Points	= (TSG_Point *)SG_Realloc(m_Points, nPoints * sizeof(TSG_Point));

	if( Points )
	{
		m_Points	= Points;
		m_nPoints	= nPoints;
		m_nBuffer	= nPoints;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Points::Add(double x, double y)
{
	if( m_nPoints >= m_nBuffer - 1 )
	{
		TSG_Point	*Points	= (TSG_Point *)SG_Realloc(m_Points, (m_nBuffer + BUFFER_SIZE_GROW(m_nBuffer)) * sizeof(TSG_Point));

		if( Points == NULL )
		{
			return( false );
		}

		m_Points	 = Points;
		m_nBuffer	+= BUFFER_SIZE_GROW(m_nBuffer);
	}

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
	m_nBuffer	= 0;
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

	m_nBuffer	= 0;
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
	if( m_nPoints == nPoints )
	{
		return( true );
	}

	if( nPoints <= 0 )
	{
		Clear();

		return( true );
	}

	TSG_Point_Int	*Points	= (TSG_Point_Int *)SG_Realloc(m_Points, nPoints * sizeof(TSG_Point_Int));

	if( Points )
	{
		m_Points	= Points;
		m_nPoints	= nPoints;
		m_nBuffer	= nPoints;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Points_Int::Add(int x, int y)
{
	if( m_nPoints >= m_nBuffer - 1 )
	{
		TSG_Point_Int	*Points	= (TSG_Point_Int *)SG_Realloc(m_Points, (m_nBuffer + BUFFER_SIZE_GROW(m_nBuffer)) * sizeof(TSG_Point_Int));

		if( Points == NULL )
		{
			return( false );
		}

		m_Points	 = Points;
		m_nBuffer	+= BUFFER_SIZE_GROW(m_nBuffer);
	}

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
CSG_Points_Z::CSG_Points_Z(void)
{
	m_nBuffer	= 0;
	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
CSG_Points_Z::~CSG_Points_Z(void)
{
	Clear();
}

//---------------------------------------------------------
void CSG_Points_Z::Clear(void)
{
	if( m_Points )
	{
		SG_Free(m_Points);
	}

	m_nBuffer	= 0;
	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
bool CSG_Points_Z::Assign(const CSG_Points_Z &Points)
{
	Set_Count(Points.m_nPoints);

	if( m_nPoints > 0 )
	{
		memcpy(m_Points, Points.m_Points, m_nPoints * sizeof(TSG_Point_Z));
	}

	return( true );
}

//---------------------------------------------------------
CSG_Points_Z & CSG_Points_Z::operator  = (const CSG_Points_Z &Points)
{
	Assign(Points);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Points_Z::Set_Count(int nPoints)
{
	if( m_nPoints == nPoints )
	{
		return( true );
	}

	if( nPoints <= 0 )
	{
		Clear();

		return( true );
	}

	TSG_Point_Z	*Points	= (TSG_Point_Z *)SG_Realloc(m_Points, nPoints * sizeof(TSG_Point_Z));

	if( Points )
	{
		m_Points	= Points;
		m_nPoints	= nPoints;
		m_nBuffer	= nPoints;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Points_Z::Add(double x, double y, double z)
{
	if( m_nPoints >= m_nBuffer - 1 )
	{
		TSG_Point_Z	*Points	= (TSG_Point_Z *)SG_Realloc(m_Points, (m_nBuffer + BUFFER_SIZE_GROW(m_nBuffer)) * sizeof(TSG_Point_Z));

		if( Points == NULL )
		{
			return( false );
		}

		m_Points	 = Points;
		m_nBuffer	+= BUFFER_SIZE_GROW(m_nBuffer);
	}

	m_Points[m_nPoints].x	= x;
	m_Points[m_nPoints].y	= y;
	m_Points[m_nPoints].z	= z;
	m_nPoints++;

	return( true );
}

//---------------------------------------------------------
bool CSG_Points_Z::Add(const TSG_Point_Z &Point)
{
	return( Add(Point.x, Point.y, Point.z) );
}

//---------------------------------------------------------
bool CSG_Points_Z::Del(int Index)
{
	if( Index >= 0 && Index < m_nPoints )
	{
		m_nPoints--;

		if( m_nPoints > 0 )
		{
			for(TSG_Point_Z *A=m_Points+Index, *B=m_Points+Index+1; Index<m_nPoints; Index++, A++, B++)
			{
				*A	= *B;
			}

			m_Points	= (TSG_Point_Z *)SG_Realloc(m_Points, m_nPoints * sizeof(TSG_Point_Z));
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
	if( xMin < xMax )
	{
		m_rect.xMin	= xMin;
		m_rect.xMax	= xMax;
	}
	else
	{
		m_rect.xMin	= xMax;
		m_rect.xMax	= xMin;
	}

	if( yMin < yMax )
	{
		m_rect.yMin	= yMin;
		m_rect.yMax	= yMax;
	}
	else
	{
		m_rect.yMin	= yMax;
		m_rect.yMax	= yMin;
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
bool CSG_Rect::is_Equal(double xMin, double yMin, double xMax, double yMax, double epsilon) const
{
	return(	SG_Is_Equal(m_rect.xMin, xMin, epsilon) && SG_Is_Equal(m_rect.yMin, yMin, epsilon)
		&&	SG_Is_Equal(m_rect.xMax, xMax, epsilon) && SG_Is_Equal(m_rect.yMax, yMax, epsilon)	);
}

bool CSG_Rect::is_Equal(const CSG_Rect &Rect, double epsilon) const
{
	return(	is_Equal(Rect.Get_XMin(), Rect.Get_YMin(), Rect.Get_XMax(), Rect.Get_YMax(), epsilon) );
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
void CSG_Rect::Union(const CSG_Point &Point)
{
	if( m_rect.xMin > Point.Get_X() )
	{
		m_rect.xMin	= Point.Get_X();
	}
	else if( m_rect.xMax < Point.Get_X() )
	{
		m_rect.xMax	= Point.Get_X();
	}

	if( m_rect.yMin > Point.Get_Y() )
	{
		m_rect.yMin	= Point.Get_Y();
	}
	else if( m_rect.yMax < Point.Get_Y() )
	{
		m_rect.yMax	= Point.Get_Y();
	}
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
CSG_Distance_Weighting::CSG_Distance_Weighting(void)
{
	m_Weighting		= SG_DISTWGHT_None;
	m_IDW_Power		= 1.0;
	m_IDW_bOffset	= true;
	m_Bandwidth		= 1.0;

	m_pParameters	= new CSG_Parameters(NULL, _TL("Distance Weighting"), _TL(""), SG_T("DISTANCE_WEIGHTING"));

	Create_Parameters(m_pParameters);
}

//---------------------------------------------------------
CSG_Distance_Weighting::~CSG_Distance_Weighting(void)
{
	delete(m_pParameters);
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Set_Parameters(CSG_Parameters *pParameters)
{
	m_pParameters->Assign_Values(pParameters);

	switch( m_pParameters->Get_Parameter("DW_WEIGHTING")->asInt() )
	{
	default:
	case 0:	Set_Weighting(SG_DISTWGHT_None );	break;
	case 1:	Set_Weighting(SG_DISTWGHT_IDW  );	break;
	case 2:	Set_Weighting(SG_DISTWGHT_EXP  );	break;
	case 3:	Set_Weighting(SG_DISTWGHT_GAUSS);	break;
	}

	Set_IDW_Offset	(m_pParameters->Get_Parameter("DW_IDW_OFFSET")->asBool  ());
	Set_IDW_Power	(m_pParameters->Get_Parameter("DW_IDW_POWER" )->asDouble());
	Set_BandWidth	(m_pParameters->Get_Parameter("DW_BANDWIDTH" )->asDouble());

	return( true );
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Create_Parameters(class CSG_Parameters *pParameters, bool bDialog)
{
	if( !pParameters )
	{
		return( false );
	}

	CSG_Parameter	*pNode;

	if( bDialog )
	{
		pParameters->Set_Name(_TL("Distance Weighting"));
		pParameters->Set_Identifier("DISTANCE_WEIGHTING");

		pNode	= NULL;
	}
	else
	{
		pNode	= pParameters->Add_Node(NULL, "DISTANCE_WEIGHTING", _TL("Distance Weighting"), _TL(""));
	}

	pParameters->Add_Choice(
		pNode, "DW_WEIGHTING"	, _TL("Weighting Function"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("no distance weighting"),
			_TL("inverse distance to a power"),
			_TL("exponential"),
			_TL("gaussian weighting")
		), m_Weighting
	);

	pParameters->Add_Value(
		pNode, "DW_IDW_POWER"	, _TL("Inverse Distance Weighting Power"),
		_TL(""),
		PARAMETER_TYPE_Double, m_IDW_Power, 0.0, true
	);

	pParameters->Add_Value(
		pNode, "DW_IDW_OFFSET"	, _TL("Inverse Distance Offset"),
		_TL("Calculates weights for distance plus one, avoiding division by zero for zero distances"),
		PARAMETER_TYPE_Bool, m_IDW_bOffset
	);

	pParameters->Add_Value(
		pNode, "DW_BANDWIDTH"	, _TL("Gaussian and Exponential Weighting Bandwidth"),
		_TL(""),
		PARAMETER_TYPE_Double, m_Bandwidth, 0.0, true
	);

	return( true );
}

//---------------------------------------------------------
int CSG_Distance_Weighting::Enable_Parameters(CSG_Parameters *pParameters)
{
	if( pParameters )
	{
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter("DW_WEIGHTING");

		if( pParameter )
		{
			int	Method	= pParameter->asInt();

			if( (pParameter = pParameters->Get_Parameter("DW_IDW_OFFSET")) != NULL )	pParameter->Set_Enabled(Method == 1);
			if( (pParameter = pParameters->Get_Parameter("DW_IDW_POWER" )) != NULL )	pParameter->Set_Enabled(Method == 1);
			if( (pParameter = pParameters->Get_Parameter("DW_BANDWIDTH" )) != NULL )	pParameter->Set_Enabled(Method >= 2);
		}
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Distance_Weighting::Set_Weighting(TSG_Distance_Weighting Weighting)
{
	m_pParameters->Get_Parameter("DW_WEIGHTING")->Set_Value((int)(m_Weighting	= Weighting));

	return( true );
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Set_IDW_Power(double Value)
{
	if( Value <= 0.0 )
	{
		return( false );
	}

	m_pParameters->Get_Parameter("DW_IDW_POWER")->Set_Value((int)(m_IDW_Power = Value));

	return( true );
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Set_IDW_Offset(bool bOn)
{
	m_pParameters->Get_Parameter("DW_IDW_OFFSET")->Set_Value((int)(m_IDW_bOffset = bOn));

	return( true );
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Set_BandWidth(double Value)
{
	if( Value <= 0.0 )
	{
		return( false );
	}

	m_pParameters->Get_Parameter("DW_BANDWIDTH")->Set_Value(m_Bandwidth = Value);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
