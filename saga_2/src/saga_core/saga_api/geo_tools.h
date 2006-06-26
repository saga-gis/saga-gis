
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
#ifndef HEADER_INCLUDED__SAGA_API__geo_tools_H
#define HEADER_INCLUDED__SAGA_API__geo_tools_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//						Enumerations					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	INTERSECTION_None			 = 0,
	INTERSECTION_Identical,
	INTERSECTION_Overlaps,
	INTERSECTION_Contained,
	INTERSECTION_Contains
}
TGEO_Intersection;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct
{
	double						x, y;
}
TGEO_Point;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CGEO_Point
{
public:
	CGEO_Point(void);
	CGEO_Point(const CGEO_Point &Point);
	CGEO_Point(const TGEO_Point &Point);
	CGEO_Point(double x, double y);

	~CGEO_Point(void);

	operator TGEO_Point &						(void)			{	return( m_point );		}

	double						Get_X			(void) const	{	return( m_point.x );	}
	double						Get_Y			(void) const	{	return( m_point.y );	}

	bool						operator ==		(const CGEO_Point &Point) const;
	bool						operator !=		(const CGEO_Point &Point) const;

	CGEO_Point &				operator  =		(const CGEO_Point &Point);
	void						operator +=		(const CGEO_Point &Point);
	void						operator -=		(const CGEO_Point &Point);

	CGEO_Point					operator +		(const CGEO_Point &Point) const;
	CGEO_Point					operator -		(const CGEO_Point &Point) const;

	void						Assign			(double x, double y);
	void						Assign			(const CGEO_Point &Point);

	bool						is_Equal		(double x, double y) const;
	bool						is_Equal		(const CGEO_Point &Point) const;


	//-----------------------------------------------------
	TGEO_Point					m_point;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct
{
	double						xMin, yMin, xMax, yMax;
}
TGEO_Rect;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CGEO_Rect
{
public:
	CGEO_Rect(void);
	CGEO_Rect(const CGEO_Rect &Rect);
	CGEO_Rect(const TGEO_Rect &Rect);
	CGEO_Rect(const CGEO_Point &A, const CGEO_Point &B);
	CGEO_Rect(double xMin, double yMin, double xMax, double yMax);

	~CGEO_Rect(void);

	operator TGEO_Rect &						(void)	{	return( m_rect );	}

	bool						operator ==		(const CGEO_Rect &Rect) const;
	bool						operator !=		(const CGEO_Rect &Rect) const;

	CGEO_Rect &					operator  =		(const CGEO_Rect &Rect);
	void						operator +=		(const CGEO_Point &Point);
	void						operator -=		(const CGEO_Point &Point);

	void						Assign			(double xMin, double yMin, double xMax, double yMax);
	void						Assign			(const CGEO_Point &A, const CGEO_Point &B);
	void						Assign			(const CGEO_Rect &Rect);

	void						Set_BottomLeft	(double x, double y);
	void						Set_BottomLeft	(const CGEO_Point &Point);
	void						Set_TopRight	(double x, double y);
	void						Set_TopRight	(const CGEO_Point &Point);

	bool						is_Equal		(double xMin, double yMin, double xMax, double yMax) const;
	bool						is_Equal		(const CGEO_Rect &Rect) const;

	double						Get_XMin		(void) const	{	return( m_rect.xMin );	}
	double						Get_XMax		(void) const	{	return( m_rect.xMax );	}
	double						Get_YMin		(void) const	{	return( m_rect.yMin );	}
	double						Get_YMax		(void) const	{	return( m_rect.yMax );	}

	double						Get_XRange		(void) const	{	return( m_rect.xMax - m_rect.xMin );	}
	double						Get_YRange		(void) const	{	return( m_rect.yMax - m_rect.yMin );	}

	CGEO_Point					Get_TopLeft		(void)			{	return( CGEO_Point(m_rect.xMin, m_rect.yMax) );	}
	CGEO_Point					Get_BottomRight	(void)			{	return( CGEO_Point(m_rect.xMax, m_rect.yMin) );	}

	CGEO_Point					Get_Center		(void) const	{	return( CGEO_Point(Get_XCenter(), Get_YCenter()) );	}
	double						Get_XCenter		(void) const	{	return( (m_rect.xMin + m_rect.xMax) / 2.0 );	}
	double						Get_YCenter		(void) const	{	return( (m_rect.yMin + m_rect.yMax) / 2.0 );	}

	void						Move			(double dx, double dy);
	void						Move			(const CGEO_Point &Point);

	void						Inflate			(double d, bool bPercent = true);
	void						Deflate			(double d, bool bPercent = true);
	void						Inflate			(double dx, double dy, bool bPercent = true);
	void						Deflate			(double dx, double dy, bool bPercent = true);

	void						Union			(const CGEO_Rect &Rect);
	bool						Intersect		(const CGEO_Rect &Rect);

	TGEO_Intersection			Intersects		(const CGEO_Rect &Rect)		const;

	bool						Contains		(double x, double y)		const;
	bool						Contains		(const CGEO_Point &Point)	const;


	//-----------------------------------------------------
	TGEO_Rect					m_rect;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CGEO_Rects
{
public:
	CGEO_Rects(void);
	virtual ~CGEO_Rects(void);

	void							Clear				(void);

	CGEO_Rects &					operator  =			(const CGEO_Rects &Points);
	bool							Assign				(const CGEO_Rects &Points);

	bool							Add					(void);
	bool							Add					(double xMin, double yMin, double xMax, double yMax);
	bool							Add					(const CGEO_Rect &Rect);

	int								Get_Count			(void)		{	return( m_nRects );	}

	CGEO_Rect &						operator []			(int Index)	{	return( *m_Rects[Index] );	}
	CGEO_Rect &						Get_Rect			(int Index)	{	return( *m_Rects[Index] );	}


private:

	int								m_nRects;

	CGEO_Rect						**m_Rects;

};


///////////////////////////////////////////////////////////
//														 //
//						Functions						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT double		GEO_Get_Distance				(double xA, double yA, double xB, double yB);
SAGA_API_DLL_EXPORT double		GEO_Get_Distance				(TGEO_Point A, TGEO_Point B);

SAGA_API_DLL_EXPORT double		GEO_Get_Angle_Of_Direction		(double x, double y);

SAGA_API_DLL_EXPORT bool		GEO_Get_Crossing				(TGEO_Point &Crossing, TGEO_Point a1, TGEO_Point a2, TGEO_Point b1, TGEO_Point b2, bool bExactMatch = true);
SAGA_API_DLL_EXPORT bool		GEO_Get_Crossing_InRegion		(TGEO_Point &Crossing, TGEO_Point a, TGEO_Point b, TGEO_Rect Region);

SAGA_API_DLL_EXPORT double		GEO_Get_Nearest_Point_On_Line	(TGEO_Point Point, TGEO_Point Ln_A, TGEO_Point Ln_B, TGEO_Point &Ln_Point, bool bExactMatch = true);

SAGA_API_DLL_EXPORT bool		GEO_Get_Triangle_CircumCircle	(TGEO_Point Triangle[3], TGEO_Point &Point, double &Radius);

SAGA_API_DLL_EXPORT double		GEO_Get_Polygon_Area			(TGEO_Point *Points, int nPoints);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__geo_tools_H
