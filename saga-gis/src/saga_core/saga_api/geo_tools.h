
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
//                      geo_tools.h                      //
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
#ifndef HEADER_INCLUDED__SAGA_API__geo_tools_H
#define HEADER_INCLUDED__SAGA_API__geo_tools_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** \file geo_tools.h
* Tools for geometric/geographic data types and related functions.
* @see CSG_Point
* @see CSG_Rect
* @see CSG_Projections
*/


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "api_core.h"
#include "metadata.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	SG_IS_BETWEEN(a, x, b)	(((a) <= (x) && (x) <= (b)) || ((b) <= (x) && (x) <= (a)))


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	INTERSECTION_None = 0,
	INTERSECTION_Identical,
	INTERSECTION_Overlaps,
	INTERSECTION_Contained,
	INTERSECTION_Contains
}
TSG_Intersection;

//---------------------------------------------------------
enum class TSG_Point_Type
{
	SG_POINT_TYPE_2D = 0,
	SG_POINT_TYPE_3D,
	SG_POINT_TYPE_4D,
	SG_POINT_TYPE_2D_Int
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct SSG_Point
{
	double						x, y;
}
TSG_Point;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Point : public TSG_Point
{
public:
	CSG_Point(void);
	CSG_Point(const CSG_Point &Point);
	CSG_Point(const TSG_Point &Point);
	CSG_Point(double x, double y);

	virtual ~CSG_Point(void)	{}

	virtual TSG_Point_Type		Get_Type		(void)	const	{	return( TSG_Point_Type::SG_POINT_TYPE_2D );	}

	virtual bool				operator ==		(const CSG_Point &Point)	const	{	return(  is_Equal(Point) );	}
	virtual bool				operator !=		(const CSG_Point &Point)	const	{	return( !is_Equal(Point) );	}

	virtual CSG_Point			operator +		(const CSG_Point &Point)	const	{	return( CSG_Point(x + Point.x, y + Point.y)	);	}
	virtual CSG_Point			operator -		(const CSG_Point &Point)	const	{	return( CSG_Point(x - Point.x, y - Point.y)	);	}

	virtual CSG_Point &			operator  =		(const CSG_Point &Point)			{	Assign  (Point);	return( *this );	}
	virtual CSG_Point &			operator +=		(const CSG_Point &Point)			{	Add     (Point);	return( *this );	}
	virtual CSG_Point &			operator -=		(const CSG_Point &Point)			{	Subtract(Point);	return( *this );	}
	virtual CSG_Point &			operator *=		(const CSG_Point &Point)			{	Multiply(Point);	return( *this );	}

	CSG_Point					operator *		(double Value)				const	{	return( CSG_Point(x * Value, y * Value)	);	}
	CSG_Point					operator /		(double Value)				const	{	return( CSG_Point(x / Value, y / Value)	);	}
	virtual CSG_Point &			operator *=		(double Value)						{	Multiply(Value);	return( *this );	}
	virtual CSG_Point &			operator /=		(double Value)						{	Divide  (Value);	return( *this );	}

	virtual void				Assign			(double x, double y);
	virtual void				Assign			(const CSG_Point &Point);

	virtual void				Add				(const CSG_Point &Point);
	virtual void				Subtract		(const CSG_Point &Point);
	virtual void				Multiply		(const CSG_Point &Point);

	virtual void				Multiply		(double Value);
	virtual void				Divide			(double Value);

	virtual double				Get_Length		(void)	const;

	virtual bool				is_Equal		(const CSG_Point &Point, double epsilon = 0.)	const	{	return(	is_Equal(Point.x, Point.y, epsilon) ); }
	virtual bool				is_Equal		(double _x,   double _y, double epsilon = 0.)	const
	{
		return(	fabs(x - _x) <= epsilon && fabs(y - _y) <= epsilon );
	}

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Points
{
public:
	CSG_Points(void);
	CSG_Points(const CSG_Points &Points);
	CSG_Points(sLong nPoints, TSG_Array_Growth Growth = TSG_Array_Growth::SG_ARRAY_GROWTH_1);

	bool						Clear			(void)              { return( m_Points.Destroy() ); }

	CSG_Points &				operator  =		(const CSG_Points &Points) { Assign(Points); return( *this ); }
	bool						Assign			(const CSG_Points &Points);

	bool						Add				(double x, double y);
	bool						Add				(const TSG_Point &Point) { return( Add(Point.x, Point.y) ); }
	bool						Del				(sLong Index)       { return( m_Points.Del_Entry(Index) ); }

	bool						Set_Count		(sLong nPoints)     { return( m_Points.Set_Array(nPoints) ); }
	sLong						Get_Count		(void)        const { return( m_Points.Get_Size() ); }

	TSG_Point *					Get_Points		(void)        const { return( (TSG_Point *)m_Points.Get_Array() ); }

	TSG_Point &					operator []		(sLong Index)       { return( Get_Points()[Index] ); }
	const TSG_Point &			operator []		(sLong Index) const { return( Get_Points()[Index] ); }


private:

	CSG_Array					m_Points;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Lines
{
public:
	CSG_Lines(void);
	virtual ~CSG_Lines(void);

								CSG_Lines		(const CSG_Lines &Lines);
	bool						Create			(const CSG_Lines &Lines);

								CSG_Lines		(sLong nLines);
	bool						Create			(sLong nLines);

	bool						Destroy			(void);
	bool						Clear			(void);

	CSG_Lines &					operator  =		(const CSG_Lines &Lines) { Assign(Lines); return( *this ); }
	bool						Assign			(const CSG_Lines &Lines);

	CSG_Points &				Add				(void);
	bool						Add				(const CSG_Points &Line);
	bool						Add				(const CSG_Lines &Lines);
	bool						Del				(sLong Index);

	bool						Set_Count		(sLong nLines);
	sLong						Get_Count		(void)        const { return( m_Lines.Get_Size() ); }

	CSG_Points &				Get_Line		(sLong Index)       { return( *((CSG_Points *)m_Lines[Index]) ); }
	const CSG_Points &			Get_Line		(sLong Index) const { return( *((CSG_Points *)m_Lines[Index]) ); }

	CSG_Points &				operator []		(sLong Index)       { return( Get_Line(Index) ); }
	const CSG_Points &			operator []		(sLong Index) const { return( Get_Line(Index) ); }

	double						Get_Length		(void)        const;
	double						Get_Length		(sLong Index) const;


private:

	CSG_Array_Pointer			m_Lines;

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct SSG_Point_3D
{
	double						x, y, z;
}
TSG_Point_3D;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Point_3D : public TSG_Point_3D
{
public:
	CSG_Point_3D(void);
	CSG_Point_3D(const CSG_Point_3D &Point);
	CSG_Point_3D(const TSG_Point_3D &Point);
	CSG_Point_3D(double x, double y, double z);

	virtual ~CSG_Point_3D(void)	{}

	virtual TSG_Point_Type		Get_Type		(void)	const	{	return( TSG_Point_Type::SG_POINT_TYPE_3D );	}

	virtual bool				operator ==		(const CSG_Point_3D &Point)	const	{	return(  is_Equal(Point) );	}
	virtual bool				operator !=		(const CSG_Point_3D &Point)	const	{	return( !is_Equal(Point) );	}

	virtual CSG_Point_3D		operator +		(const CSG_Point_3D &Point)	const	{	return( CSG_Point_3D(x + Point.x, y + Point.y, z + Point.z)	);	}
	virtual CSG_Point_3D		operator -		(const CSG_Point_3D &Point)	const	{	return( CSG_Point_3D(x - Point.x, y - Point.y, z - Point.z)	);	}

	virtual CSG_Point_3D &		operator  =		(const CSG_Point_3D &Point)			{	Assign  (Point);	return( *this );	}
	virtual CSG_Point_3D &		operator +=		(const CSG_Point_3D &Point)			{	Add     (Point);	return( *this );	}
	virtual CSG_Point_3D &		operator -=		(const CSG_Point_3D &Point)			{	Subtract(Point);	return( *this );	}
	virtual CSG_Point_3D &		operator *=		(const CSG_Point_3D &Point)			{	Multiply(Point);	return( *this );	}

	CSG_Point_3D				operator *		(double Value)				const	{	return( CSG_Point_3D(x * Value, y * Value, z * Value)	);	}
	CSG_Point_3D				operator /		(double Value)				const	{	return( CSG_Point_3D(x / Value, y / Value, z / Value)	);	}
	virtual CSG_Point_3D &		operator *=		(double Value)						{	Multiply(Value);	return( *this );	}
	virtual CSG_Point_3D &		operator /=		(double Value)						{	Divide  (Value);	return( *this );	}

	virtual void				Assign			(double x, double y, double z);
	virtual void				Assign			(const CSG_Point_3D &Point);

	virtual void				Add				(const CSG_Point_3D &Point);
	virtual void				Subtract		(const CSG_Point_3D &Point);
	virtual void				Multiply		(const CSG_Point_3D &Point);

	virtual void				Multiply		(double Value);
	virtual void				Divide			(double Value);

	virtual double				Get_Length		(void)	const;

	virtual bool				is_Equal		(const CSG_Point_3D &Point      , double epsilon = 0.)	const	{	return(	is_Equal(Point.x, Point.y, Point.y, epsilon) );	}
	virtual bool				is_Equal		(double _x, double _y, double _z, double epsilon = 0.)	const
	{
		return(	fabs(x - _x) <= epsilon && fabs(y - _y) <= epsilon && fabs(z - _z) <= epsilon );
	}

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Points_3D
{
public:
	CSG_Points_3D(void);
	CSG_Points_3D(const CSG_Points_3D &Points);
	CSG_Points_3D(sLong nPoints, TSG_Array_Growth Growth = TSG_Array_Growth::SG_ARRAY_GROWTH_1);

	bool						Clear			(void)              { return( m_Points.Destroy() ); }

	CSG_Points_3D &				operator  =		(const CSG_Points_3D &Points) { Assign(Points); return( *this ); }
	bool						Assign			(const CSG_Points_3D &Points);

	bool						Add				(double x, double y, double z);
	bool						Add				(const TSG_Point_3D &Point) { return( Add(Point.x, Point.y, Point.z) ); }
	bool						Del				(sLong Index)       { return( m_Points.Del_Entry(Index) ); }

	bool						Set_Count		(sLong nPoints)     { return( m_Points.Set_Array(nPoints) ); }
	sLong						Get_Count		(void)        const { return( m_Points.Get_Size() ); }

	TSG_Point_3D *				Get_Points		(void)        const { return( (TSG_Point_3D *)m_Points.Get_Array() ); }

	TSG_Point_3D &				operator []		(sLong Index)       { return( Get_Points()[Index] ); }
	const TSG_Point_3D &		operator []		(sLong Index) const { return( Get_Points()[Index] ); }


private:

	CSG_Array					m_Points;

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct SSG_Point_4D
{
	double						x, y, z, m;
}
TSG_Point_4D;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Point_4D : public TSG_Point_4D
{
public:
	CSG_Point_4D(void);
	CSG_Point_4D(const CSG_Point_4D &Point);
	CSG_Point_4D(const TSG_Point_4D &Point);
	CSG_Point_4D(double x, double y, double z, double m);

	virtual ~CSG_Point_4D(void)	{}

	virtual TSG_Point_Type		Get_Type		(void)	const	{	return( TSG_Point_Type::SG_POINT_TYPE_4D );	}

	virtual bool				operator ==		(const CSG_Point_4D &Point)	const	{	return(  is_Equal(Point) );	}
	virtual bool				operator !=		(const CSG_Point_4D &Point)	const	{	return( !is_Equal(Point) );	}

	virtual CSG_Point_4D		operator +		(const CSG_Point_4D &Point)	const	{	return( CSG_Point_4D(x + Point.x, y + Point.y, z + Point.z, m + Point.m) );	}
	virtual CSG_Point_4D		operator -		(const CSG_Point_4D &Point)	const	{	return( CSG_Point_4D(x - Point.x, y - Point.y, z - Point.z, m - Point.m) );	}

	CSG_Point_4D				operator *		(double Value)				const	{	return( CSG_Point_4D(x * Value, y * Value, z * Value, m * Value)	);	}
	CSG_Point_4D				operator /		(double Value)				const	{	return( CSG_Point_4D(x / Value, y / Value, z / Value, m * Value)	);	}
	virtual CSG_Point_4D &		operator *=		(double Value)						{	Multiply(Value);	return( *this );	}
	virtual CSG_Point_4D &		operator /=		(double Value)						{	Divide  (Value);	return( *this );	}

	virtual CSG_Point_4D &		operator  =		(const CSG_Point_4D &Point)			{	Assign  (Point);	return( *this );	}
	virtual CSG_Point_4D &		operator +=		(const CSG_Point_4D &Point)			{	Add     (Point);	return( *this );	}
	virtual CSG_Point_4D &		operator -=		(const CSG_Point_4D &Point)			{	Subtract(Point);	return( *this );	}
	virtual CSG_Point_4D &		operator *=		(const CSG_Point_4D &Point)			{	Multiply(Point);	return( *this );	}

	virtual void				Assign			(double x, double y, double z, double m);
	virtual void				Assign			(const CSG_Point_4D &Point);

	virtual void				Add				(const CSG_Point_4D &Point);
	virtual void				Subtract		(const CSG_Point_4D &Point);
	virtual void				Multiply		(const CSG_Point_4D &Point);

	virtual void				Multiply		(double Value);
	virtual void				Divide			(double Value);

	virtual double				Get_Length		(void)	const;

	virtual bool				is_Equal		(const CSG_Point_4D &Point                 , double epsilon = 0.)	const	{	return(	is_Equal(Point.x, Point.y, Point.y, Point.m, epsilon) );	}
	virtual bool				is_Equal		(double _x, double _y, double _z, double _m, double epsilon = 0.)	const
	{
		return(	fabs(x - _x) <= epsilon && fabs(y - _y) <= epsilon && fabs(z - _z) <= epsilon && fabs(m - _m) <= epsilon );
	}

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct SSG_Point_Int
{
	int							x, y;
}
TSG_Point_Int;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Points_Int
{
public:
	CSG_Points_Int(void);
	CSG_Points_Int(const CSG_Points_Int &Points);
	CSG_Points_Int(sLong nPoints, TSG_Array_Growth Growth = TSG_Array_Growth::SG_ARRAY_GROWTH_1);

	bool						Clear			(void)              { return( m_Points.Destroy() ); }

	CSG_Points_Int &			operator  =		(const CSG_Points_Int &Points) { Assign(Points); return( *this ); }
	bool						Assign			(const CSG_Points_Int &Points);

	bool						Add				(int x, int y);
	bool						Add				(const TSG_Point_Int &Point) { return( Add(Point.x, Point.y) ); }
	bool						Del				(sLong Index)       { return( m_Points.Del_Entry(Index) ); }

	bool						Set_Count		(sLong nPoints)     { return( m_Points.Set_Array(nPoints) ); }
	sLong						Get_Count		(void)        const { return( m_Points.Get_Size() ); }

	TSG_Point_Int *				Get_Points		(void)        const { return( (TSG_Point_Int *)m_Points.Get_Array() ); }

	TSG_Point_Int &				operator []		(sLong Index)       { return( Get_Points()[Index] ); }
	const TSG_Point_Int &		operator []		(sLong Index) const { return( Get_Points()[Index] ); }


private:

	CSG_Array					m_Points;

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct SSG_Rect
{
	double						xMin, yMin, xMax, yMax;
}
TSG_Rect;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Rect : public TSG_Rect
{
public:
	CSG_Rect(void);
	CSG_Rect(const CSG_Rect &Rect);
	CSG_Rect(const TSG_Rect &Rect);
	CSG_Rect(const CSG_Point &A, const CSG_Point &B);
	CSG_Rect(double xMin, double yMin, double xMax, double yMax);

	~CSG_Rect(void);

	bool						Create			(double xMin, double yMin, double xMax, double yMax);
	bool						Create			(const CSG_Point &A, const CSG_Point &B);
	bool						Create			(const CSG_Rect &Rect);

	bool						operator ==		(const CSG_Rect &Rect) const { return(  is_Equal(Rect) ); }
	bool						operator !=		(const CSG_Rect &Rect) const { return( !is_Equal(Rect) ); }

	CSG_Rect &					operator  =		(const CSG_Rect &Rect)   { Assign(Rect); return( *this ); }

	CSG_Rect &					operator +=		(const CSG_Point &Point) { Move(  Point.x,  Point.y); return( *this ); }
	CSG_Rect &					operator -=		(const CSG_Point &Point) { Move( -Point.x, -Point.y); return( *this ); }

	CSG_Rect &					Assign			(double xMin, double yMin, double xMax, double yMax);
	CSG_Rect &					Assign			(const CSG_Point &A, const CSG_Point &B);
	CSG_Rect &					Assign			(const CSG_Rect &Rect);

	CSG_Rect &					Set_BottomLeft	(double x, double y);
	CSG_Rect &					Set_BottomLeft	(const CSG_Point &Point);
	CSG_Rect &					Set_TopRight	(double x, double y);
	CSG_Rect &					Set_TopRight	(const CSG_Point &Point);

	double						Get_XMin		(void) const	{	return( xMin );	}
	double						Get_XMax		(void) const	{	return( xMax );	}
	double						Get_YMin		(void) const	{	return( yMin );	}
	double						Get_YMax		(void) const	{	return( yMax );	}

	double						Get_XRange		(void) const	{	return( xMax - xMin );	}
	double						Get_YRange		(void) const	{	return( yMax - yMin );	}

	double						Get_Area		(void) const	{	return( Get_XRange() * Get_YRange() );	}
	double						Get_Diameter	(void) const	{	return( sqrt(Get_XRange()*Get_XRange() + Get_YRange()*Get_YRange()) );	}

	CSG_Point					Get_TopLeft		(void) const	{	return( CSG_Point(xMin, yMax) );	}
	CSG_Point					Get_BottomRight	(void) const	{	return( CSG_Point(xMax, yMin) );	}

	CSG_Point					Get_Center		(void) const	{	return( CSG_Point(Get_XCenter(), Get_YCenter()) );	}
	double						Get_XCenter		(void) const	{	return( (xMin + xMax) / 2. );	}
	double						Get_YCenter		(void) const	{	return( (yMin + yMax) / 2. );	}

	CSG_Rect &					Move			(double dx, double dy);
	CSG_Rect &					Move			(const CSG_Point &Point);

	CSG_Rect &					Inflate			(double d, bool bPercent = true);
	CSG_Rect &					Deflate			(double d, bool bPercent = true);
	CSG_Rect &					Inflate			(double dx, double dy, bool bPercent = true);
	CSG_Rect &					Deflate			(double dx, double dy, bool bPercent = true);

	CSG_Rect &					Union			(double x, double y);
	CSG_Rect &					Union			(const CSG_Point &Point);
	CSG_Rect &					Union			(const CSG_Rect &Rect);
	bool						Intersect		(const CSG_Rect &Rect);

	bool						is_Equal		(double xMin, double yMin, double xMax, double yMax, double epsilon = 0.) const;
	bool						is_Equal		(const CSG_Rect &Rect                              , double epsilon = 0.) const;

	bool						Contains		(double x, double y)		const;
	bool						Contains		(const CSG_Point &Point)	const;

	TSG_Intersection			Intersects		(const CSG_Rect &Rect)		const;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Rects
{
public:
	CSG_Rects(void);
	CSG_Rects(const CSG_Rects &Rects);
	virtual ~CSG_Rects(void);

	void						Clear			(void);

	CSG_Rects &					operator  =		(const CSG_Rects &Rects);
	bool						Assign			(const CSG_Rects &Rects);

	bool						Add				(void);
	bool						Add				(double xMin, double yMin, double xMax, double yMax);
	bool						Add				(const CSG_Rect &Rect);

	int							Get_Count		(void)	const	{	return( m_nRects );	}

	CSG_Rect &					operator []		(int Index)		{	return( *m_Rects[Index] );	}
	CSG_Rect &					Get_Rect		(int Index)		{	return( *m_Rects[Index] );	}


private:

	int							m_nRects;

	CSG_Rect					**m_Rects;

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct SSG_Rect_Int
{
	int							xMin, yMin, xMax, yMax;
}
TSG_Rect_Int;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Rect_Int : public TSG_Rect_Int
{
public:
	CSG_Rect_Int(void);
	CSG_Rect_Int(const CSG_Rect_Int &Rect);
	CSG_Rect_Int(const TSG_Rect_Int &Rect);
	CSG_Rect_Int(const TSG_Point_Int &A, const TSG_Point_Int &B);
	CSG_Rect_Int(int xMin, int yMin, int xMax, int yMax);

	~CSG_Rect_Int(void);

	bool						Create			(int xMin, int yMin, int xMax, int yMax);
	bool						Create			(const TSG_Point_Int &A, const TSG_Point_Int &B);
	bool						Create			(const CSG_Rect_Int &Rect);

	bool						operator ==		(const CSG_Rect_Int &Rect) const { return(  is_Equal(Rect) ); }
	bool						operator !=		(const CSG_Rect_Int &Rect) const { return( !is_Equal(Rect) ); }

	CSG_Rect_Int &				operator  =		(const CSG_Rect_Int &Rect)   { Assign(Rect); return( *this ); }

	CSG_Rect_Int &				operator +=		(const TSG_Point_Int &Point) { Move( Point.x,  Point.y); return( *this ); }
	CSG_Rect_Int &				operator -=		(const TSG_Point_Int &Point) { Move(-Point.x, -Point.y); return( *this ); }

	CSG_Rect_Int &				Assign			(int xMin, int yMin, int xMax, int yMax);
	CSG_Rect_Int &				Assign			(const TSG_Point_Int &A, const TSG_Point_Int &B);
	CSG_Rect_Int &				Assign			(const CSG_Rect_Int &Rect);

	CSG_Rect_Int &				Set_BottomLeft	(int x, int y);
	CSG_Rect_Int &				Set_BottomLeft	(const TSG_Point_Int &Point);
	CSG_Rect_Int &				Set_TopRight	(int x, int y);
	CSG_Rect_Int &				Set_TopRight	(const TSG_Point_Int &Point);

	int							Get_XMin		(void) const	{	return( xMin );	}
	int							Get_XMax		(void) const	{	return( xMax );	}
	int							Get_YMin		(void) const	{	return( yMin );	}
	int							Get_YMax		(void) const	{	return( yMax );	}

	int							Get_XRange		(void) const	{	return( xMax - xMin );	}
	int							Get_YRange		(void) const	{	return( yMax - yMin );	}

	double						Get_Area		(void) const	{	return( ((double)Get_XRange() * (double)Get_YRange()) );	}
	double						Get_Diameter	(void) const	{	double x = (double)xMax - xMin, y = (double)yMax - yMin; return( sqrt(x*x + y*y) );	}

	TSG_Point_Int				Get_TopLeft		(void) const	{	TSG_Point_Int p; p.x = xMin; p.y = yMax; return( p );	}
	TSG_Point_Int				Get_BottomRight	(void) const	{	TSG_Point_Int p; p.x = xMax; p.y = yMin; return( p );	}

	CSG_Rect_Int &				Move			(int dx, int dy);
	CSG_Rect_Int &				Move			(const TSG_Point_Int &Point);

	CSG_Rect_Int &				Inflate			(int d);
	CSG_Rect_Int &				Deflate			(int d);
	CSG_Rect_Int &				Inflate			(int dx, int dy);
	CSG_Rect_Int &				Deflate			(int dx, int dy);

	CSG_Rect_Int &				Union			(int x, int y);
	CSG_Rect_Int &				Union			(const TSG_Point_Int &Point);
	CSG_Rect_Int &				Union			(const CSG_Rect_Int &Rect);
	bool						Intersect		(const CSG_Rect_Int &Rect);

	bool						is_Equal		(int xMin, int yMin, int xMax, int yMax) const;
	bool						is_Equal		(const CSG_Rect_Int &Rect              ) const;

	bool						Contains		(double x, double y)         const;
	bool						Contains		(const TSG_Point_Int &Point) const;

	TSG_Intersection			Intersects		(const CSG_Rect_Int &Rect)   const;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Rects_Int
{
public:
	CSG_Rects_Int(void);
	virtual ~CSG_Rects_Int(void);

	void						Clear			(void);

	CSG_Rects_Int &				operator  =		(const CSG_Rects_Int &Rects);
	bool						Assign			(const CSG_Rects_Int &Rects);

	bool						Add				(void);
	bool						Add				(int xMin, int yMin, int xMax, int yMax);
	bool						Add				(const CSG_Rect_Int &Rect);

	int							Get_Count		(void)	const	{	return( m_nRects );	}

	CSG_Rect_Int &				operator []		(int Index)		{	return( *m_Rects[Index] );	}
	CSG_Rect_Int &				Get_Rect		(int Index)		{	return( *m_Rects[Index] );	}


private:

	int							m_nRects;

	CSG_Rect_Int				**m_Rects;

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	SG_DISTWGHT_None	= 0,
	SG_DISTWGHT_IDW,
	SG_DISTWGHT_EXP,
	SG_DISTWGHT_GAUSS
}
TSG_Distance_Weighting;


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Distance_Weighting
{
public:
	CSG_Distance_Weighting(void);
	virtual ~CSG_Distance_Weighting(void);

	static bool				Enable_Parameters	(class CSG_Parameters &Parameters);
	bool					Create_Parameters	(class CSG_Parameters &Parameters, const CSG_String &Parent = "", bool bIDW_Offset = false);
	static bool				Add_Parameters		(class CSG_Parameters &Parameters, const CSG_String &Parent = "", bool bIDW_Offset = false);
	bool					Set_Parameters		(class CSG_Parameters &Parameters);

	TSG_Distance_Weighting	Get_Weighting		(void)	const	{	return( m_Weighting   );	}
	bool					Set_Weighting		(TSG_Distance_Weighting Weighting);

	double					Get_IDW_Power		(void)	const	{	return( m_IDW_Power   );	}
	bool					Set_IDW_Power		(double Value);

	bool					Get_IDW_Offset		(void)	const	{	return( m_IDW_bOffset );	}
	bool					Set_IDW_Offset		(bool bOn = true);

	double					Get_BandWidth		(void)	const	{	return( m_Bandwidth   );	}
	bool					Set_BandWidth		(double Value);

	//-----------------------------------------------------
	double					Get_Weight			(double Distance)	const
	{
		if( Distance < 0. )
		{
			return( 0. );
		}

		switch( m_Weighting )
		{
		case SG_DISTWGHT_IDW  :
			return( m_IDW_bOffset
				? pow(1. + Distance, -m_IDW_Power) : Distance > 0.
				? pow(     Distance, -m_IDW_Power) : 0.
			);

		case SG_DISTWGHT_EXP  :
			return( exp(-Distance / m_Bandwidth) );

		case SG_DISTWGHT_GAUSS:
			Distance	/= m_Bandwidth;
			return( exp(-0.5 * Distance*Distance) );

		default: // case SG_DISTWGHT_None:
			return( 1. );
		}
	}


private:

	bool					m_IDW_bOffset;

	double					m_IDW_Power, m_Bandwidth;

	TSG_Distance_Weighting	m_Weighting;

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum class ESG_CRS_Format
{
	WKT1, WKT2, PROJ, ESRI, Undefined, WKT = WKT1, Proj4 = PROJ, EPSG
};

//---------------------------------------------------------
enum class ESG_CRS_Type
{
	Geographic, Geocentric, Projection, Undefined
};

//---------------------------------------------------------
enum class ESG_Projection_Unit
{
	Kilometer,
	Meter,
	Decimeter,
	Centimeter,
	Millimeter,
	Int_Nautical_Mile,
	Int_Inch,
	Int_Foot,
	Int_Yard,
	Int_Statute_Mile,
	Int_Fathom,
	Int_Chain,
	Int_Link,
	US_Inch,
	US_Foot,
	US_Yard,
	US_Chain,
	US_Statute_Mile,
	Indian_Yard,
	Indian_Foot,
	Indian_Chain,
	Undefined, First = Kilometer
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Projection
{
	friend class CSG_Projections;

public:
	CSG_Projection(void);
	virtual ~CSG_Projection(void);

	void							Destroy					(void);

									CSG_Projection			(const CSG_Projection &Projection);
	bool							Create					(const CSG_Projection &Projection);
	CSG_Projection &				operator =				(const CSG_Projection &Projection) { Create(Projection); return( *this ); }

									CSG_Projection			(int Code, const SG_Char *Authority = NULL);
	bool							Create					(int Code, const SG_Char *Authority = NULL);
	CSG_Projection &				operator =				(int Code)                         { Create(Code      ); return( *this ); }

									CSG_Projection			(const CSG_String &Definition, ESG_CRS_Format Format = ESG_CRS_Format::WKT);
	bool							Create					(const CSG_String &Definition, ESG_CRS_Format Format = ESG_CRS_Format::WKT);
	CSG_Projection &				operator =				(const CSG_String &Definition)     { Create(Definition); return( *this ); }

									CSG_Projection			(const CSG_String &WKT, const CSG_String &Proj4);
	bool							Create					(const CSG_String &WKT, const CSG_String &Proj4);

	bool							Assign					(const CSG_Projection &Projection)                                          { return( Create(Projection        ) ); }
	bool							Assign					(int Code, const SG_Char *Authority = NULL)                                 { return( Create(Code, Authority   ) ); }
	bool							Assign					(const CSG_String &Definition, ESG_CRS_Format Format = ESG_CRS_Format::WKT) { return( Create(Definition, Format) ); }
	bool							Assign					(const CSG_String &WKT, const CSG_String &Proj4)                            { return( Create(WKT, Proj4        ) ); }

	bool							is_Okay					(void)	const	{	return( m_Type != ESG_CRS_Type::Undefined );	}
	bool							is_Equal				(const CSG_Projection &Projection) const;
	bool							operator ==				(const CSG_Projection &Projection) const { return( is_Equal(Projection) == true  ); }
	bool							operator !=				(const CSG_Projection &Projection) const { return( is_Equal(Projection) == false ); }

	static const CSG_Projection &	Get_GCS_WGS84			(void);
	bool							Set_GCS_WGS84			(void);

	static CSG_Projection			Get_UTM_WGS84			(int Zone, bool bSouth = false);
	bool							Set_UTM_WGS84			(int Zone, bool bSouth = false);

	bool							Load					(const CSG_String &File, ESG_CRS_Format Format = ESG_CRS_Format::Undefined);
	bool							Save					(const CSG_String &File, ESG_CRS_Format Format = ESG_CRS_Format::WKT) const;

	bool							Load					(CSG_File &Stream      , ESG_CRS_Format Format = ESG_CRS_Format::Undefined);
	bool							Save					(CSG_File &Stream      , ESG_CRS_Format Format = ESG_CRS_Format::WKT) const;

	bool							Load					(const CSG_MetaData &Projection);
	bool							Save					(      CSG_MetaData &Projection) const;

	CSG_String						Get_Description			(bool bDetails = false)	const;

	const CSG_String &				Get_Name				(void)	const	{	return( m_Name      );	}

	const CSG_String &				Get_WKT					(void)	const	{	return( m_WKT1      );	}
	const CSG_String &				Get_WKT1				(void)	const	{	return( m_WKT1      );	}
	const CSG_String &				Get_WKT2				(void)	const	{	return( m_WKT2      );	}
	const CSG_String &				Get_PROJ				(void)	const	{	return( m_PROJ      );	}
	const CSG_String &				Get_ESRI				(void)	const	{	return( m_ESRI      );	}
	const CSG_String &				Get_Authority			(void)	const	{	return( m_Authority );	}
	int								Get_Code				(void)	const	{	return( m_Code      );	}

	const CSG_String &				Get_Proj4				(void)	const	{	return( m_PROJ      );	}
	int								Get_Authority_ID		(void)	const	{	return( m_Code      );	}
	int								Get_EPSG				(void)	const	{	return( m_Authority.CmpNoCase("EPSG") ? -1 : m_Code );	}

	bool							is_Geographic			(void)	const	{	return( m_Type == ESG_CRS_Type::Geographic );	}
	bool							is_Geocentric			(void)	const	{	return( m_Type == ESG_CRS_Type::Geocentric );	}
	bool							is_Projection			(void)	const	{	return( m_Type == ESG_CRS_Type::Projection );	}

	ESG_CRS_Type					Get_Type				(void)	const	{	return( m_Type );	}
	CSG_String						Get_Type_Identifier		(void)	const;
	CSG_String						Get_Type_Name			(void)	const;

	ESG_Projection_Unit				Get_Unit				(void)	const	{	return( m_Unit );	}
	CSG_String						Get_Unit_Identifier		(void)	const;
	CSG_String						Get_Unit_Name			(void)	const;
	double							Get_Unit_To_Meter		(void)	const;


private:

	int								m_Code;

	ESG_CRS_Type					m_Type;

	ESG_Projection_Unit				m_Unit;

	CSG_String						m_Name, m_Authority, m_PROJ, m_WKT1, m_WKT2, m_ESRI;


};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** CSG_Projections is a projections dictionary and translator
  * for EPSG codes, OGC Well-Known-Text, and Proj.4.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Projections
{
	friend class CSG_Projection;

public:
	CSG_Projections(void);
	virtual ~CSG_Projections(void);

									CSG_Projections			(bool LoadDefault);
	bool							Create					(bool LoadDefault = true);

									CSG_Projections			(const CSG_String &File);
	bool							Create					(const CSG_String &File);

	void							Destroy					(void);

	static bool						Parse					(const CSG_String &Definition, CSG_String *WKT1 = NULL, CSG_String *WKT2 = NULL, CSG_String *PROJ = NULL, CSG_String *ESRI = NULL);

	bool							Load					(const CSG_String &File, bool bAppend = false);
	bool							Save					(const CSG_String &File);

	bool							Add						(const CSG_Projection &Projection);
	bool							Add						(const SG_Char *WKT, const SG_Char *Proj4, const SG_Char *Authority, int Authority_ID);

	sLong							Get_Count				(void)	const;

	CSG_Projection					operator []				(sLong Index)	const	{	return( Get_Projection(Index) );	}
	CSG_Projection					Get_Projection			(sLong Index)	const;
	const SG_Char *					Get_Projection			(                            int Code, const SG_Char *Authority = NULL) const;
	bool							Get_Projection			(CSG_Projection &Projection, int Code, const SG_Char *Authority = NULL) const;

	CSG_String						Get_Names_List			(ESG_CRS_Type Type = ESG_CRS_Type::Undefined, bool bAddSelect = true)	const;

	static ESG_CRS_Type				Get_CRS_Type			(const CSG_String &Identifier);
	static CSG_String				Get_CRS_Type_Identifier	(ESG_CRS_Type Type);
	static CSG_String				Get_CRS_Type_Name		(ESG_CRS_Type Type);

	static ESG_Projection_Unit		Get_Unit				(const CSG_String &Identifier);
	static const CSG_String			Get_Unit_Identifier		(ESG_Projection_Unit Unit);
	static const CSG_String			Get_Unit_Name			(ESG_Projection_Unit Unit, bool bSimple = true);
	static double					Get_Unit_To_Meter		(ESG_Projection_Unit Unit);


private:

	class CSG_Table					*m_pProjections;


	void							_On_Construction			(void);


private:

	CSG_Translator					m_WKT_to_Proj4, m_Proj4_to_WKT, m_EPSG_to_Idx;


	static CSG_MetaData				_WKT_to_MetaData			(const CSG_String &WKT);
	static bool						_WKT_to_MetaData			(CSG_MetaData &MetaData, const CSG_String &WKT);

	bool							_WKT_to_Proj4				(CSG_String &Proj4, const CSG_String &WKT  )	const;
	bool							_WKT_from_Proj4				(CSG_String &WKT  , const CSG_String &Proj4)	const;
	bool							_WKT_to_Proj4_Set_Datum		(CSG_String &Proj4, const CSG_MetaData &WKT)	const;

	static bool						_Proj4_Find_Parameter		(                   const CSG_String &Proj4, const CSG_String &Key);
	static bool						_Proj4_Read_Parameter		(CSG_String &Value, const CSG_String &Proj4, const CSG_String &Key);
	static bool						_Proj4_Get_Ellipsoid		(CSG_String &Value, const CSG_String &Proj4);
	static bool						_Proj4_Get_Datum			(CSG_String &Value, const CSG_String &Proj4);
	static bool						_Proj4_Get_Prime_Meridian	(CSG_String &Value, const CSG_String &Proj4);
	static bool						_Proj4_Get_Unit				(CSG_String &Value, const CSG_String &Proj4);

	bool							_EPSG_to_Proj4				(CSG_String &Proj4, int EPSG_Code)	const;
	bool							_EPSG_to_WKT				(CSG_String &WKT  , int EPSG_Code)	const;

	bool							_Set_Dictionary				(void);
	bool							_Set_Dictionary				(CSG_Table &Dictionary, int Direction);

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_Projections &	SG_Get_Projections		(void);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool				SG_Get_Projected		(class CSG_Shapes *pSource, class CSG_Shapes *pTarget, const CSG_Projection &Target);

SAGA_API_DLL_EXPORT bool				SG_Get_Projected		(const CSG_Projection &Source, const CSG_Projection &Target, TSG_Point &Point    );
SAGA_API_DLL_EXPORT bool				SG_Get_Projected		(const CSG_Projection &Source, const CSG_Projection &Target, TSG_Rect  &Rectangle);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool				SG_Grid_Get_Geographic_Coordinates	(CSG_Grid *pGrid, CSG_Grid *pLon, CSG_Grid *pLat);


///////////////////////////////////////////////////////////
//                                                       //
//						Functions						 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool		SG_Is_Equal						(double a, double b, double epsilon = 0.);
SAGA_API_DLL_EXPORT bool		SG_Is_Equal						(const TSG_Point &A, const TSG_Point &B, double epsilon = 0.);

SAGA_API_DLL_EXPORT bool		SG_Is_Between					(double x, double a, double b, double epsilon = 0.);
SAGA_API_DLL_EXPORT bool		SG_Is_Between					(const TSG_Point &Point, const TSG_Point &Corner_A, const TSG_Point &Corner_B, double epsilon = 0.);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT double		SG_Get_Length					(double dx, double dy);

SAGA_API_DLL_EXPORT double		SG_Get_Distance					(double ax, double ay, double bx, double by, bool bPolar);
SAGA_API_DLL_EXPORT double		SG_Get_Distance					(const TSG_Point &A, const TSG_Point &B    , bool bPolar);

SAGA_API_DLL_EXPORT double		SG_Get_Distance					(double ax, double ay, double bx, double by);
SAGA_API_DLL_EXPORT double		SG_Get_Distance					(const TSG_Point &A, const TSG_Point &B);

SAGA_API_DLL_EXPORT double		SG_Get_Distance					(double ax, double ay, double az, double bx, double by, double bz);
SAGA_API_DLL_EXPORT double		SG_Get_Distance					(const TSG_Point_3D &A, const TSG_Point_3D &B);

SAGA_API_DLL_EXPORT double		SG_Get_Distance_Polar			(double aLon, double aLat, double bLon, double bLat, double a = 6378137., double e = 298.257223563, bool bDegree = true);
SAGA_API_DLL_EXPORT double		SG_Get_Distance_Polar			(const TSG_Point &A      , const TSG_Point &B      , double a = 6378137., double e = 298.257223563, bool bDegree = true);

SAGA_API_DLL_EXPORT double		SG_Get_Angle_Of_Direction		(double dx, double dy);
SAGA_API_DLL_EXPORT double		SG_Get_Angle_Of_Direction		(double ax, double ay, double bx, double by);
SAGA_API_DLL_EXPORT double		SG_Get_Angle_Of_Direction		(const TSG_Point &A);
SAGA_API_DLL_EXPORT double		SG_Get_Angle_Of_Direction		(const TSG_Point &A, const TSG_Point &B);
SAGA_API_DLL_EXPORT double		SG_Get_Angle_Difference			(double a, double b);
SAGA_API_DLL_EXPORT bool		SG_is_Angle_Between				(double Angle, double Angle_Min, double Angle_Max, bool bCheckRange = true);

SAGA_API_DLL_EXPORT bool		SG_Get_Crossing					(TSG_Point &Crossing, const TSG_Point &a1, const TSG_Point &a2, const TSG_Point &b1, const TSG_Point &b2, bool bExactMatch = true);
SAGA_API_DLL_EXPORT bool		SG_Get_Crossing_InRegion		(TSG_Point &Crossing, const TSG_Point &a , const TSG_Point & b, const TSG_Rect &Region);

SAGA_API_DLL_EXPORT bool		SG_Is_Point_On_Line				(const TSG_Point &Point, const TSG_Point &Line_A, const TSG_Point &Line_B, bool bExactMatch = false, double Epsilon = 0.);
SAGA_API_DLL_EXPORT double		SG_Get_Distance_To_Line			(const TSG_Point &Point, const TSG_Point &Line_A, const TSG_Point &Line_B, bool bExactMatch = true);
SAGA_API_DLL_EXPORT double		SG_Get_Nearest_Point_On_Line	(const TSG_Point &Point, const TSG_Point &Line_A, const TSG_Point &Line_B, TSG_Point &Line_Point, bool bExactMatch = true);

SAGA_API_DLL_EXPORT bool		SG_Get_Triangle_CircumCircle	(TSG_Point Triangle[3], TSG_Point &Point, double &Radius);

SAGA_API_DLL_EXPORT double		SG_Get_Polygon_Area				(TSG_Point *Points, int nPoints);
SAGA_API_DLL_EXPORT double		SG_Get_Polygon_Area				(const CSG_Points &Points);


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__geo_tools_H
