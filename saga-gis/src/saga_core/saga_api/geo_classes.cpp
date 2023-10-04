
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
#include "parameters.h"

#include "geo_tools.h"


///////////////////////////////////////////////////////////
//														 //
//						CSG_Point						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Point::CSG_Point(void)
{
	Assign(0., 0.);
}

CSG_Point::CSG_Point(const CSG_Point &Point)
{
	Assign(Point.x, Point.y);
}

CSG_Point::CSG_Point(const TSG_Point &Point)
{
	Assign(Point.x, Point.y);
}

CSG_Point::CSG_Point(double _x, double _y)
{
	Assign(_x, _y);
}

//---------------------------------------------------------
void CSG_Point::Assign(double _x, double _y)
{
	x	= _x;
	y	= _y;
}

void CSG_Point::Assign(const CSG_Point &Point)
{
	x	= Point.x;
	y	= Point.y;
}

//---------------------------------------------------------
void CSG_Point::Add(const CSG_Point &Point)
{
	x	+= Point.x;
	y	+= Point.y;
}

void CSG_Point::Subtract(const CSG_Point &Point)
{
	x	-= Point.x;
	y	-= Point.y;
}

void CSG_Point::Multiply(const CSG_Point &Point)
{
	x	*= Point.x;
	y	*= Point.y;
}

void CSG_Point::Multiply(double Value)
{
	x	*= Value;
	y	*= Value;
}

void CSG_Point::Divide(double Value)
{
	x	/= Value;
	y	/= Value;
}

//---------------------------------------------------------
double CSG_Point::Get_Length(void)	const
{
	return( sqrt(x*x + y*y) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Point_3D::CSG_Point_3D(void)
{
	Assign(0., 0., 0.);
}

CSG_Point_3D::CSG_Point_3D(const CSG_Point_3D &Point)
{
	Assign(Point.x, Point.y, Point.z);
}

CSG_Point_3D::CSG_Point_3D(const TSG_Point_3D &Point)
{
	Assign(Point.x, Point.y, Point.z);
}

CSG_Point_3D::CSG_Point_3D(double _x, double _y, double _z)
{
	Assign(_x, _y, _z);
}

//---------------------------------------------------------
void CSG_Point_3D::Assign(double _x, double _y, double _z)
{
	x	= _x;
	y	= _y;
	z	= _z;
}

void CSG_Point_3D::Assign(const CSG_Point_3D &Point)
{
	x	= Point.x;
	y	= Point.y;
	z	= Point.z;
}

//---------------------------------------------------------
void CSG_Point_3D::Add(const CSG_Point_3D &Point)
{
	x	+= Point.x;
	y	+= Point.y;
	z	+= Point.z;
}

void CSG_Point_3D::Subtract(const CSG_Point_3D &Point)
{
	x	-= Point.x;
	y	-= Point.y;
	z	-= Point.z;
}

void CSG_Point_3D::Multiply(const CSG_Point_3D &Point)
{
	x	*= Point.x;
	y	*= Point.y;
	z	*= Point.z;
}

void CSG_Point_3D::Multiply(double Value)
{
	x	*= Value;
	y	*= Value;
	z	*= Value;
}

void CSG_Point_3D::Divide(double Value)
{
	x	/= Value;
	y	/= Value;
	z	/= Value;
}

//---------------------------------------------------------
double CSG_Point_3D::Get_Length(void)	const
{
	return( sqrt(x*x + y*y + z*z) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Point_4D::CSG_Point_4D(void)
{
	Assign(0., 0., 0., 0.);
}

CSG_Point_4D::CSG_Point_4D(const CSG_Point_4D &Point)
{
	Assign(Point.x, Point.y, Point.z, Point.m);
}

CSG_Point_4D::CSG_Point_4D(const TSG_Point_4D &Point)
{
	Assign(Point.x, Point.y, Point.z, Point.m);
}

CSG_Point_4D::CSG_Point_4D(double _x, double _y, double _z, double _m)
{
	Assign(_x, _y, _z, _m);
}

//---------------------------------------------------------
void CSG_Point_4D::Assign(double _x, double _y, double _z, double _m)
{
	x	= _x;
	y	= _y;
	z	= _z;
	m	= _m;
}

void CSG_Point_4D::Assign(const CSG_Point_4D &Point)
{
	x	= Point.x;
	y	= Point.y;
	z	= Point.z;
	m	= Point.m;
}

//---------------------------------------------------------
void CSG_Point_4D::Add(const CSG_Point_4D &Point)
{
	x	+= Point.x;
	y	+= Point.y;
	z	+= Point.z;
	m	+= Point.m;
}

void CSG_Point_4D::Subtract(const CSG_Point_4D &Point)
{
	x	-= Point.x;
	y	-= Point.y;
	z	-= Point.z;
	m	-= Point.m;
}

void CSG_Point_4D::Multiply(const CSG_Point_4D &Point)
{
	x	*= Point.x;
	y	*= Point.y;
	z	*= Point.z;
	m	*= Point.m;
}

void CSG_Point_4D::Multiply(double Value)
{
	x	*= Value;
	y	*= Value;
	z	*= Value;
	m	*= Value;
}

void CSG_Point_4D::Divide(double Value)
{
	x	/= Value;
	y	/= Value;
	z	/= Value;
	m	/= Value;
}

//---------------------------------------------------------
double CSG_Point_4D::Get_Length(void)	const
{
	return( sqrt(x*x + y*y + z*z + m*m) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Points::CSG_Points(void)
{
	m_Points.Create(sizeof(TSG_Point), 0, TSG_Array_Growth::SG_ARRAY_GROWTH_1);
}

CSG_Points::CSG_Points(const CSG_Points &Points)
{
	m_Points.Create(sizeof(TSG_Point), 0, TSG_Array_Growth::SG_ARRAY_GROWTH_1);

	Assign(Points);
}

CSG_Points::CSG_Points(sLong nPoints, TSG_Array_Growth Growth)
{
	m_Points.Create(sizeof(TSG_Point), nPoints, Growth);
}

//---------------------------------------------------------
bool CSG_Points::Assign(const CSG_Points &Points)
{
	if( m_Points.Set_Array(Points.m_Points.Get_Size()) )
	{
		if( m_Points.Get_Size() > 0 )
		{
			memcpy(m_Points.Get_Array(), Points.m_Points.Get_Array(), m_Points.Get_Size() * m_Points.Get_Value_Size());
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Points::Add(double x, double y)
{
	if( m_Points.Inc_Array() )
	{
		TSG_Point *Point = (TSG_Point *)m_Points.Get_Entry(m_Points.Get_Size() - 1);

		Point->x = x;
		Point->y = y;

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
CSG_Lines::CSG_Lines(void)
{
	m_Lines.Create(0, TSG_Array_Growth::SG_ARRAY_GROWTH_0);
}

CSG_Lines::CSG_Lines(const CSG_Lines &Lines)
{
	m_Lines.Create(0, TSG_Array_Growth::SG_ARRAY_GROWTH_0);

	Create(Lines);
}

bool CSG_Lines::Create(const CSG_Lines &Lines)
{
	return( Assign(Lines) );
}

CSG_Lines::CSG_Lines(sLong nLines)
{
	m_Lines.Create(0, TSG_Array_Growth::SG_ARRAY_GROWTH_0);

	Create(nLines);
}

bool CSG_Lines::Create(sLong nLines)
{
	return( Set_Count(nLines) );
}

//---------------------------------------------------------
CSG_Lines::~CSG_Lines(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Lines::Destroy(void)
{
	return( Set_Count(0) );
}

//---------------------------------------------------------
bool CSG_Lines::Clear(void)
{
	return( Set_Count(0) );
}

//---------------------------------------------------------
bool CSG_Lines::Assign(const CSG_Lines &Lines)
{
	if( Set_Count(Lines.Get_Count()) )
	{
		for(sLong i=0; i<Lines.Get_Count(); i++)
		{
			Get_Line(i).Assign(Lines[i]);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Points & CSG_Lines::Add(void)
{
	Set_Count(Get_Count() + 1);

	return( Get_Line(Get_Count() - 1) );
}

//---------------------------------------------------------
bool CSG_Lines::Add(const CSG_Points &Line)
{
	Add().Assign(Line);

	return( true );
}

//---------------------------------------------------------
bool CSG_Lines::Add(const CSG_Lines &Lines)
{
	for(sLong i=0; i<Lines.Get_Count(); i++)
	{
		Add(Lines[i]);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Lines::Del(sLong Index)
{
	if( Index >= 0 && Index < m_Lines.Get_Size() )
	{
		delete((CSG_Points *)m_Lines[Index]);

		return( m_Lines.Del(Index) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Lines::Set_Count(sLong new_Count)
{
	if( new_Count < 0 )
	{
		new_Count = 0;
	}

	sLong old_Count = m_Lines.Get_Size();

	if( new_Count > old_Count )
	{
		m_Lines.Set_Array(new_Count);

		for(sLong i=old_Count; i<new_Count; i++)
		{
			m_Lines[i] = new CSG_Points;
		}
	}
	else if( new_Count < old_Count )
	{
		for(sLong i=new_Count; i<old_Count; i++)
		{
			delete((CSG_Points *)m_Lines[i]);
		}

		m_Lines.Set_Array(new_Count);
	}

	return( true );
}

double CSG_Lines::Get_Length(void) const
{
	double Length = 0.;

	for(sLong i=0; i<Get_Count(); i++)
	{
		Length += Get_Length(i);
	}

	return( Length );
}

double CSG_Lines::Get_Length(sLong Index) const
{
	double Length = 0.;

	if( Index >= 0 && Index < Get_Count() )
	{
		const CSG_Points &Line = Get_Line(Index);

		if( Line.Get_Count() > 1 )
		{
			for(sLong i=0, j=1; j<Line.Get_Count(); i++, j++)
			{
				Length += SG_Get_Distance(Line[i], Line[j]);
			}
		}
	}

	return( Length );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Points_3D::CSG_Points_3D(void)
{
	m_Points.Create(sizeof(TSG_Point_3D), 0, TSG_Array_Growth::SG_ARRAY_GROWTH_1);
}

CSG_Points_3D::CSG_Points_3D(const CSG_Points_3D &Points)
{
	m_Points.Create(sizeof(CSG_Points_3D), 0, TSG_Array_Growth::SG_ARRAY_GROWTH_1);

	Assign(Points);
}

CSG_Points_3D::CSG_Points_3D(sLong nPoints, TSG_Array_Growth Growth)
{
	m_Points.Create(sizeof(TSG_Point_3D), nPoints, Growth);
}

//---------------------------------------------------------
bool CSG_Points_3D::Assign(const CSG_Points_3D &Points)
{
	if( m_Points.Set_Array(Points.m_Points.Get_Size()) )
	{
		if( m_Points.Get_Size() > 0 )
		{
			memcpy(m_Points.Get_Array(), Points.m_Points.Get_Array(), m_Points.Get_Size() * m_Points.Get_Value_Size());
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Points_3D::Add(double x, double y, double z)
{
	if( m_Points.Inc_Array() )
	{
		TSG_Point_3D *Point = (TSG_Point_3D *)m_Points.Get_Entry(m_Points.Get_Size() - 1);

		Point->x = x;
		Point->y = y;
		Point->z = z;

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
	m_Points.Create(sizeof(TSG_Point_Int), 0, TSG_Array_Growth::SG_ARRAY_GROWTH_1);
}

CSG_Points_Int::CSG_Points_Int(const CSG_Points_Int &Points)
{
	m_Points.Create(sizeof(TSG_Point_Int), 0, TSG_Array_Growth::SG_ARRAY_GROWTH_1);

	Assign(Points);
}

CSG_Points_Int::CSG_Points_Int(sLong nPoints, TSG_Array_Growth Growth)
{
	m_Points.Create(sizeof(TSG_Point_Int), nPoints, Growth);
}

//---------------------------------------------------------
bool CSG_Points_Int::Assign(const CSG_Points_Int &Points)
{
	if( m_Points.Set_Array(Points.m_Points.Get_Size()) )
	{
		if( m_Points.Get_Size() > 0 )
		{
			memcpy(m_Points.Get_Array(), Points.m_Points.Get_Array(), m_Points.Get_Size() * m_Points.Get_Value_Size());
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Points_Int::Add(int x, int y)
{
	if( m_Points.Inc_Array() )
	{
		TSG_Point_Int *Point = (TSG_Point_Int *)m_Points.Get_Entry(m_Points.Get_Size() - 1);

		Point->x = x;
		Point->y = y;

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
	Assign(0., 0., 0., 0.);
}

CSG_Rect::CSG_Rect(const CSG_Rect &Rect)
{
	Assign(Rect.xMin, Rect.yMin, Rect.xMax, Rect.yMax);
}

CSG_Rect::CSG_Rect(const TSG_Rect &Rect)
{
	Assign(Rect.xMin, Rect.yMin, Rect.xMax, Rect.yMax);
}

CSG_Rect::CSG_Rect(const CSG_Point &A, const CSG_Point &B)
{
	Assign(A.x, A.y, B.x, B.y);
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
	Move( Point.x,  Point.y);
}

void CSG_Rect::operator -= (const CSG_Point &Point)
{
	Move(-Point.y, -Point.y);
}

//---------------------------------------------------------
void CSG_Rect::Assign(double xMin, double yMin, double xMax, double yMax)
{
	if( xMin < xMax )
	{
		this->xMin	= xMin;
		this->xMax	= xMax;
	}
	else
	{
		this->xMin	= xMax;
		this->xMax	= xMin;
	}

	if( yMin < yMax )
	{
		this->yMin	= yMin;
		this->yMax	= yMax;
	}
	else
	{
		this->yMin	= yMax;
		this->yMax	= yMin;
	}
}

void CSG_Rect::Assign(const CSG_Point &A, const CSG_Point &B)
{
	Assign(A.x, A.y, B.x, B.y);
}

void CSG_Rect::Assign(const CSG_Rect &Rect)
{
	Assign(Rect.xMin, Rect.yMin, Rect.xMax, Rect.yMax);
}

//---------------------------------------------------------
void CSG_Rect::Set_BottomLeft(double x, double y)
{
	Assign(x, y, xMax, yMax);
}

void CSG_Rect::Set_BottomLeft(const CSG_Point &Point)
{
	Set_BottomLeft(Point.x, Point.y );
}

void CSG_Rect::Set_TopRight(double x, double y)
{
	Assign(xMin, yMin, x, y);
}

void CSG_Rect::Set_TopRight(const CSG_Point &Point)
{
	Set_TopRight(Point.x, Point.y );
}

//---------------------------------------------------------
bool CSG_Rect::is_Equal(double xMin, double yMin, double xMax, double yMax, double epsilon) const
{
	return( SG_Is_Equal(this->xMin, xMin, epsilon) && SG_Is_Equal(this->yMin, yMin, epsilon)
	     && SG_Is_Equal(this->xMax, xMax, epsilon) && SG_Is_Equal(this->yMax, yMax, epsilon) );
}

bool CSG_Rect::is_Equal(const CSG_Rect &Rect, double epsilon) const
{
	return(	is_Equal(Rect.xMin, Rect.yMin, Rect.xMax, Rect.yMax, epsilon) );
}

//---------------------------------------------------------
void CSG_Rect::Move(double dx, double dy)
{
	xMin += dx; yMin += dy;
	xMax += dx; yMax += dy;
}

void CSG_Rect::Move(const CSG_Point &Point)
{
	Move(Point.x, Point.y);
}

//---------------------------------------------------------
void CSG_Rect::Inflate(double dx, double dy, bool bPercent)
{
	if( bPercent )
	{
		dx = (Get_XRange() * 0.01 * dx) / 2.;
		dy = (Get_YRange() * 0.01 * dy) / 2.;
	}

	Assign(
		xMin - dx, yMin - dy,
		xMax + dx, yMax + dy
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
void CSG_Rect::Union(double x, double y)
{
	if( xMin > x ) { xMin = x; } else if( xMax < x ) { xMax = x; }
	if( yMin > y ) { yMin = y; } else if( yMax < y ) { yMax = y; }
}

//---------------------------------------------------------
void CSG_Rect::Union(const CSG_Point &Point)
{
	if( xMin > Point.x ) { xMin = Point.x; } else if( xMax < Point.x ) { xMax = Point.x; }
	if( yMin > Point.y ) { yMin = Point.y; } else if( yMax < Point.y ) { yMax = Point.y; }
}

//---------------------------------------------------------
void CSG_Rect::Union(const CSG_Rect &Rect)
{
	if( xMin > Rect.Get_XMin() ) { xMin = Rect.Get_XMin(); }
	if( yMin > Rect.Get_YMin() ) { yMin = Rect.Get_YMin(); }
	if( xMax < Rect.Get_XMax() ) { xMax = Rect.Get_XMax(); }
	if( yMax < Rect.Get_YMax() ) { yMax = Rect.Get_YMax(); }
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
		(*this)	= Rect;
		break;

	case INTERSECTION_Overlaps:
		if( xMin < Rect.Get_XMin() ) { xMin = Rect.Get_XMin(); }
		if( yMin < Rect.Get_YMin() ) { yMin = Rect.Get_YMin(); }
		if( xMax > Rect.Get_XMax() ) { xMax = Rect.Get_XMax(); }
		if( yMax > Rect.Get_YMax() ) { yMax = Rect.Get_YMax(); }
		break;
	}

	return( true );
}

//---------------------------------------------------------
TSG_Intersection CSG_Rect::Intersects(const CSG_Rect &Rect) const
{
	if(	xMax < Rect.Get_XMin() || Rect.Get_XMax() < xMin
	||	yMax < Rect.Get_YMin() || Rect.Get_YMax() < yMin )
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
	return( xMin <= x && x <= xMax && yMin <= y && y <= yMax );
}

bool CSG_Rect::Contains(const CSG_Point &Point) const
{
	return( Contains(Point.x, Point.y) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rects::CSG_Rects(void)
{
	m_nRects = 0; m_Rects = NULL;
}

//---------------------------------------------------------
CSG_Rects::CSG_Rects(const CSG_Rects &Rects)
{
	m_nRects = 0; m_Rects = NULL;

	Assign(Rects);
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

	m_nRects = 0;
	m_Rects  = NULL;
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
	m_Rects           = (CSG_Rect **)SG_Realloc(m_Rects, ((uLong)m_nRects + 1) * sizeof(CSG_Rect *));
	m_Rects[m_nRects] = new CSG_Rect(Rect);
	m_nRects++;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						CSG_Rect_Int					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rect_Int::CSG_Rect_Int(void)
{
	Assign(0, 0, 0, 0);
}

CSG_Rect_Int::CSG_Rect_Int(const CSG_Rect_Int &Rect)
{
	Assign(Rect.xMin, Rect.yMin, Rect.xMax, Rect.yMax);
}

CSG_Rect_Int::CSG_Rect_Int(const TSG_Rect_Int &Rect)
{
	Assign(Rect.xMin, Rect.yMin, Rect.xMax, Rect.yMax);
}

CSG_Rect_Int::CSG_Rect_Int(const TSG_Point_Int &A, const TSG_Point_Int &B)
{
	Assign(A.x, A.y, B.x, B.y);
}

CSG_Rect_Int::CSG_Rect_Int(int xMin, int yMin, int xMax, int yMax)
{
	Assign(xMin, yMin, xMax, yMax);
}

//---------------------------------------------------------
CSG_Rect_Int::~CSG_Rect_Int(void)
{}

//---------------------------------------------------------
bool CSG_Rect_Int::operator == (const CSG_Rect_Int &Rect) const
{
	return( is_Equal(Rect) );
}

bool CSG_Rect_Int::operator != (const CSG_Rect_Int &Rect) const
{
	return( !is_Equal(Rect) );
}

CSG_Rect_Int & CSG_Rect_Int::operator = (const CSG_Rect_Int &Rect)
{
	Assign(Rect);

	return( *this );
}

void CSG_Rect_Int::operator += (const TSG_Point_Int &Point)
{
	Move( Point.x,  Point.y);
}

void CSG_Rect_Int::operator -= (const TSG_Point_Int &Point)
{
	Move(-Point.y, -Point.y);
}

//---------------------------------------------------------
void CSG_Rect_Int::Assign(int xMin, int yMin, int xMax, int yMax)
{
	if( xMin < xMax )
	{
		this->xMin	= xMin;
		this->xMax	= xMax;
	}
	else
	{
		this->xMin	= xMax;
		this->xMax	= xMin;
	}

	if( yMin < yMax )
	{
		this->yMin	= yMin;
		this->yMax	= yMax;
	}
	else
	{
		this->yMin	= yMax;
		this->yMax	= yMin;
	}
}

void CSG_Rect_Int::Assign(const TSG_Point_Int &A, const TSG_Point_Int &B)
{
	Assign(A.x, A.y, B.x, B.y);
}

void CSG_Rect_Int::Assign(const CSG_Rect_Int &Rect)
{
	Assign(Rect.xMin, Rect.yMin, Rect.xMax, Rect.yMax);
}

//---------------------------------------------------------
void CSG_Rect_Int::Set_BottomLeft(int x, int y)
{
	Assign(x, y, xMax, yMax);
}

void CSG_Rect_Int::Set_BottomLeft(const TSG_Point_Int &Point)
{
	Set_BottomLeft(Point.x, Point.y );
}

void CSG_Rect_Int::Set_TopRight(int x, int y)
{
	Assign(xMin, yMin, x, y);
}

void CSG_Rect_Int::Set_TopRight(const TSG_Point_Int &Point)
{
	Set_TopRight(Point.x, Point.y );
}

//---------------------------------------------------------
bool CSG_Rect_Int::is_Equal(int xMin, int yMin, int xMax, int yMax) const
{
	return( (this->xMin == xMin) && (this->yMin == yMin)
		&&  (this->xMax == xMax) && (this->yMax == yMax) );
}

bool CSG_Rect_Int::is_Equal(const CSG_Rect_Int &Rect) const
{
	return(	is_Equal(Rect.xMin, Rect.yMin, Rect.xMax, Rect.yMax) );
}

//---------------------------------------------------------
void CSG_Rect_Int::Move(int dx, int dy)
{
	xMin += dx; yMin += dy;
	xMax += dx; yMax += dy;
}

void CSG_Rect_Int::Move(const TSG_Point_Int &Point)
{
	Move(Point.x, Point.y);
}

//---------------------------------------------------------
void CSG_Rect_Int::Inflate(int dx, int dy)
{
	Assign(xMin - dx, yMin - dy, xMax + dx, yMax + dy);
}

void CSG_Rect_Int::Inflate(int d)
{
	Inflate(d, d);
}

void CSG_Rect_Int::Deflate(int dx, int dy)
{
	Inflate(-dx, -dy);
}

void CSG_Rect_Int::Deflate(int d)
{
	Deflate(d, d);
}

//---------------------------------------------------------
void CSG_Rect_Int::Union(int x, int y)
{
	if( xMin > x ) { xMin = x; } else if( xMax < x ) { xMax = x; }
	if( yMin > y ) { yMin = y; } else if( yMax < y ) { yMax = y; }
}

//---------------------------------------------------------
void CSG_Rect_Int::Union(const TSG_Point_Int &Point)
{
	if( xMin > Point.x ) { xMin = Point.x; } else if( xMax < Point.x ) { xMax = Point.x; }
	if( yMin > Point.y ) { yMin = Point.y; } else if( yMax < Point.y ) { yMax = Point.y; }
}

//---------------------------------------------------------
void CSG_Rect_Int::Union(const CSG_Rect_Int &Rect)
{
	if( xMin > Rect.Get_XMin() ) { xMin = Rect.Get_XMin(); }
	if( yMin > Rect.Get_YMin() ) { yMin = Rect.Get_YMin(); }
	if( xMax < Rect.Get_XMax() ) { xMax = Rect.Get_XMax(); }
	if( yMax < Rect.Get_YMax() ) { yMax = Rect.Get_YMax(); }
}

//---------------------------------------------------------
bool CSG_Rect_Int::Intersect(const CSG_Rect_Int &Rect)
{
	switch( Intersects(Rect) )
	{
	case INTERSECTION_None: default:
		return( false );

	case INTERSECTION_Identical:
	case INTERSECTION_Contained:
		break;

	case INTERSECTION_Contains:
		(*this)	= Rect;
		break;

	case INTERSECTION_Overlaps:
		if( xMin < Rect.Get_XMin() ) { xMin = Rect.Get_XMin(); }
		if( yMin < Rect.Get_YMin() ) { yMin = Rect.Get_YMin(); }
		if( xMax > Rect.Get_XMax() ) { xMax = Rect.Get_XMax(); }
		if( yMax > Rect.Get_YMax() ) { yMax = Rect.Get_YMax(); }
		break;
	}

	return( true );
}

//---------------------------------------------------------
TSG_Intersection CSG_Rect_Int::Intersects(const CSG_Rect_Int &Rect) const
{
	if(	xMax < Rect.Get_XMin() || Rect.Get_XMax() < xMin
	||	yMax < Rect.Get_YMin() || Rect.Get_YMax() < yMin )
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
bool CSG_Rect_Int::Contains(double x, double y) const
{
	return( xMin <= x && x <= xMax && yMin <= y && y <= yMax );
}

bool CSG_Rect_Int::Contains(const TSG_Point_Int &Point) const
{
	return( Contains(Point.x, Point.y) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rects_Int::CSG_Rects_Int(void)
{
	m_nRects = 0;
	m_Rects  = NULL;
}

//---------------------------------------------------------
CSG_Rects_Int::~CSG_Rects_Int(void)
{
	Clear();
}

//---------------------------------------------------------
void CSG_Rects_Int::Clear(void)
{
	if( m_Rects )
	{
		for(int i=0; i<m_nRects; i++)
		{
			delete(m_Rects[i]);
		}

		SG_Free(m_Rects);
	}

	m_nRects = 0;
	m_Rects  = NULL;
}

//---------------------------------------------------------
bool CSG_Rects_Int::Assign(const CSG_Rects_Int &Rects)
{
	Clear();

	for(int i=0; i<Rects.m_nRects; i++)
	{
		Add(*Rects.m_Rects[i]);
	}

	return( true );
}

//---------------------------------------------------------
CSG_Rects_Int & CSG_Rects_Int::operator  = (const CSG_Rects_Int &Rects)
{
	Assign(Rects);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Rects_Int::Add(void)
{
	return( Add(CSG_Rect_Int()) );
}

//---------------------------------------------------------
bool CSG_Rects_Int::Add(int xMin, int yMin, int xMax, int yMax)
{
	return( Add(CSG_Rect_Int(xMin, yMin, xMax, yMax)) );
}

//---------------------------------------------------------
bool CSG_Rects_Int::Add(const CSG_Rect_Int &Rect)
{
	m_Rects           = (CSG_Rect_Int **)SG_Realloc(m_Rects, ((uLong)m_nRects + 1) * sizeof(CSG_Rect_Int *));
	m_Rects[m_nRects] = new CSG_Rect_Int(Rect);
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
	m_Weighting   = SG_DISTWGHT_None;

	m_IDW_Power   = 2.;
	m_IDW_bOffset = true;

	m_Bandwidth   = 1.;
}

//---------------------------------------------------------
CSG_Distance_Weighting::~CSG_Distance_Weighting(void)
{}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Create_Parameters(CSG_Parameters &Parameters, const CSG_String &Parent, bool bIDW_Offset)
{
	if( Add_Parameters(Parameters, Parent, bIDW_Offset) )
	{
		if( Parameters("DW_WEIGHTING" ) ) { Parameters("DW_WEIGHTING" )->Set_Value((int)m_Weighting  ); }
		if( Parameters("DW_IDW_POWER" ) ) { Parameters("DW_IDW_POWER" )->Set_Value(     m_IDW_Power  ); }
		if( Parameters("DW_IDW_OFFSET") ) { Parameters("DW_IDW_OFFSET")->Set_Value(     m_IDW_bOffset); }
		if( Parameters("DW_BANDWIDTH" ) ) { Parameters("DW_BANDWIDTH" )->Set_Value(     m_Bandwidth  ); }

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Add_Parameters(CSG_Parameters &Parameters, const CSG_String &Parent, bool bIDW_Offset)
{
	Parameters.Add_Choice(Parent,
		"DW_WEIGHTING"	, _TL("Weighting Function"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("no distance weighting"),
			_TL("inverse distance to a power"),
			_TL("exponential"),
			_TL("gaussian")
		), 0
	);

	Parameters.Add_Double("DW_WEIGHTING",
		"DW_IDW_POWER"	, _TL("Power"),
		_TL(""),
		2., 0., true
	);

	if( bIDW_Offset )
	{
		Parameters.Add_Bool  ("DW_WEIGHTING",
			"DW_IDW_OFFSET"	, _TL("Offset"),
			_TL("Calculates weights for distance plus one, avoiding division by zero for zero distances"),
			true
		);
	}

	Parameters.Add_Double("DW_WEIGHTING",
		"DW_BANDWIDTH"	, _TL("Bandwidth"),
		_TL("Bandwidth for exponential and Gaussian weighting"),
		1., 0., true
	);

	return( true );
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Enable_Parameters(CSG_Parameters &Parameters)
{
	if( Parameters("DW_WEIGHTING") )
	{
		int	Method	= Parameters("DW_WEIGHTING")->asInt();

		Parameters.Set_Enabled("DW_IDW_OFFSET", Method == 1);
		Parameters.Set_Enabled("DW_IDW_POWER" , Method == 1);
		Parameters.Set_Enabled("DW_BANDWIDTH" , Method >= 2);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Set_Parameters(CSG_Parameters &Parameters)
{
	if( Parameters("DW_WEIGHTING") )
	{
		switch( Parameters("DW_WEIGHTING")->asInt() )
		{
		case 0: Set_Weighting(SG_DISTWGHT_None ); break;
		case 1: Set_Weighting(SG_DISTWGHT_IDW  ); break;
		case 2: Set_Weighting(SG_DISTWGHT_EXP  ); break;
		case 3: Set_Weighting(SG_DISTWGHT_GAUSS); break;
		}
	}

	if( Parameters("DW_IDW_OFFSET") )
	{
		Set_IDW_Offset(Parameters("DW_IDW_OFFSET")->asBool  ());
	}

	if( Parameters("DW_IDW_POWER" ) )
	{
		Set_IDW_Power (Parameters("DW_IDW_POWER" )->asDouble());
	}

	if( Parameters("DW_BANDWIDTH" ) )
	{
		Set_BandWidth (Parameters("DW_BANDWIDTH" )->asDouble());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Distance_Weighting::Set_Weighting(TSG_Distance_Weighting Weighting)
{
	m_Weighting	= Weighting;

	return( true );
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Set_IDW_Power(double Value)
{
	if( Value <= 0. )
	{
		return( false );
	}

	m_IDW_Power	= Value;

	return( true );
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Set_IDW_Offset(bool bOn)
{
	m_IDW_bOffset	= bOn;

	return( true );
}

//---------------------------------------------------------
bool CSG_Distance_Weighting::Set_BandWidth(double Value)
{
	if( Value <= 0. )
	{
		return( false );
	}

	m_Bandwidth	= Value;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
