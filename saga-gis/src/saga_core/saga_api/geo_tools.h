
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** \file geo_tools.h
* Tools for geometric/geographic data types and related functions.
* @see CSG_Point
* @see CSG_Rect
* @see CSG_Projections
*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "api_core.h"
#include "metadata.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	SG_IS_BETWEEN(a, x, b)	(((a) <= (x) && (x) <= (b)) || ((b) <= (x) && (x) <= (a)))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Intersection
{
	INTERSECTION_None			 = 0,
	INTERSECTION_Identical,
	INTERSECTION_Overlaps,
	INTERSECTION_Contained,
	INTERSECTION_Contains
}
TSG_Intersection;

//---------------------------------------------------------
typedef enum ESG_Point_Type
{
	SG_POINT_TYPE_XY			 = 0,
	SG_POINT_TYPE_XYZ,
	SG_POINT_TYPE_XYZM,
	SG_POINT_TYPE_XY_Int
}
TSG_Point_Type;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct SSG_Point
{
	double						x, y;
}
TSG_Point;

//---------------------------------------------------------
typedef struct SSG_Point_Z
{
	double						x, y, z;
}
TSG_Point_Z;

//---------------------------------------------------------
typedef struct SSG_Point_ZM
{
	double						x, y, z, m;
}
TSG_Point_ZM;

//---------------------------------------------------------
typedef struct SSG_Point_Int
{
	int							x, y;
}
TSG_Point_Int;

//---------------------------------------------------------
typedef struct SSG_Rect
{
	double						xMin, yMin, xMax, yMax;
}
TSG_Rect;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool		SG_Is_Equal			(double a, double b, double epsilon = 0.);
SAGA_API_DLL_EXPORT bool		SG_Is_Equal			(const TSG_Point &A, const TSG_Point &B, double epsilon = 0.);

SAGA_API_DLL_EXPORT bool		SG_Is_Between		(double x, double a, double b, double epsilon = 0.);
SAGA_API_DLL_EXPORT bool		SG_Is_Between		(const TSG_Point &Point, const TSG_Point &Corner_A, const TSG_Point &Corner_B, double epsilon = 0.);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Point
{
public:

	double						x, y;


	CSG_Point(void);
	CSG_Point(const CSG_Point &Point);
	CSG_Point(const TSG_Point &Point);
	CSG_Point(double x, double y);

	virtual ~CSG_Point(void)	{}

	virtual TSG_Point_Type		Get_Type		(void)	const	{	return( SG_POINT_TYPE_XY );	}

	operator TSG_Point							(void)	const	{	TSG_Point p; p.x = x; p.y = y; return( p );	}

	double						Get_X			(void)	const	{	return( x );	}
	void						Set_X			(double Value)	{	x	= Value;	}
	double						Get_Y			(void)	const	{	return( y );	}
	void						Set_Y			(double Value)	{	y	= Value;	}

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

	virtual bool				is_Equal		(double _x, double _y  , double epsilon = 0.)	const	{	return( is_Equal(CSG_Point(x, y), epsilon) );	}
	virtual bool				is_Equal		(const CSG_Point &Point, double epsilon = 0.)	const
	{	return(	SG_Is_Equal(x, Point.x, epsilon)
		     && SG_Is_Equal(y, Point.y, epsilon) );
	}

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Point_Z : public CSG_Point
{
public:

	double						z;


	CSG_Point_Z(void);
	CSG_Point_Z(const CSG_Point_Z &Point);
	CSG_Point_Z(const TSG_Point_Z &Point);
	CSG_Point_Z(double x, double y, double z);

	virtual ~CSG_Point_Z(void)	{}

	virtual TSG_Point_Type		Get_Type		(void)	const	{	return( SG_POINT_TYPE_XYZ );	}

	operator TSG_Point_Z						(void)	const	{	TSG_Point_Z p; p.x = x; p.y = y; p.z = z; return( p );	}

	double						Get_Z			(void)	const	{	return( z );	}
	void						Set_Z			(double Value)	{	z	= Value;	}

	virtual bool				operator ==		(const CSG_Point_Z &Point)	const	{	return(  is_Equal(Point) );	}
	virtual bool				operator !=		(const CSG_Point_Z &Point)	const	{	return( !is_Equal(Point) );	}

	virtual CSG_Point_Z			operator +		(const CSG_Point_Z &Point)	const	{	return( CSG_Point_Z(x + Point.x, y + Point.y, z + Point.z)	);	}
	virtual CSG_Point_Z			operator -		(const CSG_Point_Z &Point)	const	{	return( CSG_Point_Z(x - Point.x, y - Point.y, z - Point.z)	);	}

	virtual CSG_Point_Z &		operator  =		(const CSG_Point_Z &Point)			{	Assign  (Point);	return( *this );	}
	virtual CSG_Point_Z &		operator +=		(const CSG_Point_Z &Point)			{	Add     (Point);	return( *this );	}
	virtual CSG_Point_Z &		operator -=		(const CSG_Point_Z &Point)			{	Subtract(Point);	return( *this );	}
	virtual CSG_Point_Z &		operator *=		(const CSG_Point_Z &Point)			{	Multiply(Point);	return( *this );	}

	CSG_Point_Z					operator *		(double Value)				const	{	return( CSG_Point_Z(x * Value, y * Value, z * Value)	);	}
	CSG_Point_Z					operator /		(double Value)				const	{	return( CSG_Point_Z(x / Value, y / Value, z / Value)	);	}
	virtual CSG_Point_Z &		operator *=		(double Value)						{	Multiply(Value);	return( *this );	}
	virtual CSG_Point_Z &		operator /=		(double Value)						{	Divide  (Value);	return( *this );	}

	virtual void				Assign			(double x, double y, double z);
	virtual void				Assign			(const CSG_Point_Z &Point);

	virtual void				Add				(const CSG_Point_Z &Point);
	virtual void				Subtract		(const CSG_Point_Z &Point);
	virtual void				Multiply		(const CSG_Point_Z &Point);

	virtual void				Multiply		(double Value);
	virtual void				Divide			(double Value);

	virtual double				Get_Length		(void)	const;

	virtual bool				is_Equal		(double _x, double _y, double _z, double epsilon = 0.)	const	{	return(	is_Equal(CSG_Point_Z(x, y, z), epsilon) );	}
	virtual bool				is_Equal		(const CSG_Point_Z &Point       , double epsilon = 0.)	const
	{	return(	SG_Is_Equal(x, Point.x, epsilon)
		     && SG_Is_Equal(y, Point.y, epsilon)
		     && SG_Is_Equal(z, Point.z, epsilon) );
	}

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Point_ZM : public CSG_Point_Z
{
public:

	double						m;


	CSG_Point_ZM(void);
	CSG_Point_ZM(const CSG_Point_ZM &Point);
	CSG_Point_ZM(const TSG_Point_ZM &Point);
	CSG_Point_ZM(double x, double y, double z, double m);

	virtual ~CSG_Point_ZM(void)	{}

	virtual TSG_Point_Type		Get_Type		(void)	const	{	return( SG_POINT_TYPE_XYZM );	}

	operator TSG_Point_ZM						(void)	const	{	TSG_Point_ZM p; p.x = x; p.y = y; p.z = z; p.m = m; return( p );	}

	double						Get_M			(void)	const	{	return( m );	}
	void						Set_M			(double Value)	{	m	= Value;	}

	virtual bool				operator ==		(const CSG_Point_ZM &Point)	const	{	return(  is_Equal(Point) );	}
	virtual bool				operator !=		(const CSG_Point_ZM &Point)	const	{	return( !is_Equal(Point) );	}

	virtual CSG_Point_ZM		operator +		(const CSG_Point_ZM &Point)	const	{	return( CSG_Point_ZM(x + Point.x, y + Point.y, z + Point.z, m + Point.m) );	}
	virtual CSG_Point_ZM		operator -		(const CSG_Point_ZM &Point)	const	{	return( CSG_Point_ZM(x - Point.x, y - Point.y, z - Point.z, m - Point.m) );	}

	CSG_Point_ZM				operator *		(double Value)				const	{	return( CSG_Point_ZM(x * Value, y * Value, z * Value, m * Value)	);	}
	CSG_Point_ZM				operator /		(double Value)				const	{	return( CSG_Point_ZM(x / Value, y / Value, z / Value, m * Value)	);	}
	virtual CSG_Point_ZM &		operator *=		(double Value)						{	Multiply(Value);	return( *this );	}
	virtual CSG_Point_ZM &		operator /=		(double Value)						{	Divide  (Value);	return( *this );	}

	virtual CSG_Point_ZM &		operator  =		(const CSG_Point_ZM &Point)			{	Assign  (Point);	return( *this );	}
	virtual CSG_Point_ZM &		operator +=		(const CSG_Point_ZM &Point)			{	Add     (Point);	return( *this );	}
	virtual CSG_Point_ZM &		operator -=		(const CSG_Point_ZM &Point)			{	Subtract(Point);	return( *this );	}
	virtual CSG_Point_ZM &		operator *=		(const CSG_Point_ZM &Point)			{	Multiply(Point);	return( *this );	}

	virtual void				Assign			(double x, double y, double z, double m);
	virtual void				Assign			(const CSG_Point_ZM &Point);

	virtual void				Add				(const CSG_Point_ZM &Point);
	virtual void				Subtract		(const CSG_Point_ZM &Point);
	virtual void				Multiply		(const CSG_Point_ZM &Point);

	virtual void				Multiply		(double Value);
	virtual void				Divide			(double Value);

	virtual double				Get_Length		(void)	const;

	virtual bool				is_Equal		(double x, double y, double z, double m, double epsilon = 0.)	const	{	return(	is_Equal(CSG_Point_ZM(x, y, z, m), epsilon) );	}
	virtual bool				is_Equal		(const CSG_Point_ZM &Point             , double epsilon = 0.)	const
	{	return(	SG_Is_Equal(x, Point.x, epsilon)
		     && SG_Is_Equal(y, Point.y, epsilon)
		     && SG_Is_Equal(z, Point.z, epsilon)
		     && SG_Is_Equal(m, Point.m, epsilon) );
	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Points
{
public:
	CSG_Points(void);
	virtual ~CSG_Points(void);

	void						Clear			(void);

	CSG_Points &				operator  =		(const CSG_Points &Points);
	bool						Assign			(const CSG_Points &Points);

	bool						Add				(double x, double y);
	bool						Add				(const TSG_Point &Point);
	bool						Del				(int Index);

	bool						Set_Count		(int nPoints);
	int							Get_Count		(void)		const	{	return( m_nPoints );	}

	TSG_Point &					operator []		(int Index)			{	return( m_Points[Index]   );	}
	TSG_Point &					Get_Point		(int Index)			{	return( m_Points[Index]   );	}
	double						Get_X			(int Index) const	{	return( m_Points[Index].x );	}
	double						Get_Y			(int Index) const	{	return( m_Points[Index].y );	}


private:

	int							m_nPoints, m_nBuffer;

	TSG_Point					*m_Points;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Points_Int
{
public:
	CSG_Points_Int(void);
	virtual ~CSG_Points_Int(void);

	void						Clear			(void);

	CSG_Points_Int &			operator  =		(const CSG_Points_Int &Points);
	bool						Assign			(const CSG_Points_Int &Points);

	bool						Add				(int x, int y);
	bool						Add				(const TSG_Point_Int &Point);
	bool						Del				(int Index);

	bool						Set_Count		(int nPoints);
	int							Get_Count		(void)		const	{	return( m_nPoints );	}

	TSG_Point_Int &				operator []		(int Index)			{	return( m_Points[Index]   );	}
	TSG_Point_Int &				Get_Point		(int Index)			{	return( m_Points[Index]   );	}
	int							Get_X			(int Index) const	{	return( m_Points[Index].x );	}
	int							Get_Y			(int Index) const	{	return( m_Points[Index].y );	}


private:

	int							m_nPoints, m_nBuffer;

	TSG_Point_Int				*m_Points;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Points_Z
{
public:
	CSG_Points_Z(void);
	virtual ~CSG_Points_Z(void);

	void						Clear			(void);

	CSG_Points_Z &				operator  =		(const CSG_Points_Z &Points);
	bool						Assign			(const CSG_Points_Z &Points);

	bool						Add				(double x, double y, double z);
	bool						Add				(const TSG_Point_Z &Point);
	bool						Del				(int Index);

	bool						Set_Count		(int nPoints);
	int							Get_Count		(void)		const	{	return( m_nPoints );	}

	TSG_Point_Z &				operator []		(int Index)			{	return( m_Points[Index]   );	}
	TSG_Point_Z &				Get_Point		(int Index)			{	return( m_Points[Index]   );	}
	double						Get_X			(int Index)	const	{	return( m_Points[Index].x );	}
	double						Get_Y			(int Index)	const	{	return( m_Points[Index].y );	}
	double						Get_Z			(int Index)	const	{	return( m_Points[Index].z );	}


private:

	int							m_nPoints, m_nBuffer;

	TSG_Point_Z					*m_Points;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Rect
{
public:
	CSG_Rect(void);
	CSG_Rect(const CSG_Rect &Rect);
	CSG_Rect(const TSG_Rect &Rect);
	CSG_Rect(const CSG_Point &A, const CSG_Point &B);
	CSG_Rect(double xMin, double yMin, double xMax, double yMax);

	~CSG_Rect(void);

	operator const TSG_Rect &					(void) const	{	return( m_rect );	}

	bool						operator ==		(const CSG_Rect &Rect) const;
	bool						operator !=		(const CSG_Rect &Rect) const;

	CSG_Rect &					operator  =		(const CSG_Rect &Rect);
	void						operator +=		(const CSG_Point &Point);
	void						operator -=		(const CSG_Point &Point);

	void						Assign			(double xMin, double yMin, double xMax, double yMax);
	void						Assign			(const CSG_Point &A, const CSG_Point &B);
	void						Assign			(const CSG_Rect &Rect);

	void						Set_BottomLeft	(double x, double y);
	void						Set_BottomLeft	(const CSG_Point &Point);
	void						Set_TopRight	(double x, double y);
	void						Set_TopRight	(const CSG_Point &Point);

	bool						is_Equal		(double xMin, double yMin, double xMax, double yMax, double epsilon = 0.) const;
	bool						is_Equal		(const CSG_Rect &Rect                              , double epsilon = 0.) const;

	double						Get_XMin		(void) const	{	return( m_rect.xMin );	}
	double						Get_XMax		(void) const	{	return( m_rect.xMax );	}
	double						Get_YMin		(void) const	{	return( m_rect.yMin );	}
	double						Get_YMax		(void) const	{	return( m_rect.yMax );	}

	double						Get_XRange		(void) const	{	return( m_rect.xMax - m_rect.xMin );	}
	double						Get_YRange		(void) const	{	return( m_rect.yMax - m_rect.yMin );	}

	double						Get_Area		(void) const	{	return( Get_XRange() * Get_YRange() );	}
	double						Get_Diameter	(void) const	{	return( sqrt(Get_XRange()*Get_XRange() + Get_YRange()*Get_YRange()) );	}

	CSG_Point					Get_TopLeft		(void) const	{	return( CSG_Point(m_rect.xMin, m_rect.yMax) );	}
	CSG_Point					Get_BottomRight	(void) const	{	return( CSG_Point(m_rect.xMax, m_rect.yMin) );	}

	CSG_Point					Get_Center		(void) const	{	return( CSG_Point(Get_XCenter(), Get_YCenter()) );	}
	double						Get_XCenter		(void) const	{	return( (m_rect.xMin + m_rect.xMax) / 2. );	}
	double						Get_YCenter		(void) const	{	return( (m_rect.yMin + m_rect.yMax) / 2. );	}

	void						Move			(double dx, double dy);
	void						Move			(const CSG_Point &Point);

	void						Inflate			(double d, bool bPercent = true);
	void						Deflate			(double d, bool bPercent = true);
	void						Inflate			(double dx, double dy, bool bPercent = true);
	void						Deflate			(double dx, double dy, bool bPercent = true);

	void						Union			(const CSG_Point &Point);
	void						Union			(const CSG_Rect &Rect);
	bool						Intersect		(const CSG_Rect &Rect);

	TSG_Intersection			Intersects		(const CSG_Rect &Rect)		const;

	bool						Contains		(double x, double y)		const;
	bool						Contains		(const CSG_Point &Point)	const;


	//-----------------------------------------------------
	TSG_Rect					m_rect;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Rects
{
public:
	CSG_Rects(void);
	virtual ~CSG_Rects(void);

	void						Clear			(void);

	CSG_Rects &					operator  =		(const CSG_Rects &Points);
	bool						Assign			(const CSG_Rects &Points);

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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Distance_Weighting
{
	SG_DISTWGHT_None	= 0,
	SG_DISTWGHT_IDW,
	SG_DISTWGHT_EXP,
	SG_DISTWGHT_GAUSS
}
TSG_Distance_Weighting;


///////////////////////////////////////////////////////////
//														 //
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Projection_Format
{
	SG_PROJ_FMT_WKT,
	SG_PROJ_FMT_Proj4,
	SG_PROJ_FMT_EPSG,
	SG_PROJ_FMT_Undefined
}
TSG_Projection_Format;

//---------------------------------------------------------
typedef enum ESG_Projection_Type
{
	SG_PROJ_TYPE_CS_Projected,
	SG_PROJ_TYPE_CS_Geographic,
	SG_PROJ_TYPE_CS_Geocentric,
	SG_PROJ_TYPE_CS_Undefined
}
TSG_Projection_Type;

//---------------------------------------------------------
typedef enum ESG_Projection_Unit
{
	SG_PROJ_UNIT_Kilometer,
	SG_PROJ_UNIT_Meter,
	SG_PROJ_UNIT_Decimeter,
	SG_PROJ_UNIT_Centimeter,
	SG_PROJ_UNIT_Millimeter,
	SG_PROJ_UNIT_Int_Nautical_Mile,
	SG_PROJ_UNIT_Int_Inch,
	SG_PROJ_UNIT_Int_Foot,
	SG_PROJ_UNIT_Int_Yard,
	SG_PROJ_UNIT_Int_Statute_Mile,
	SG_PROJ_UNIT_Int_Fathom,
	SG_PROJ_UNIT_Int_Chain,
	SG_PROJ_UNIT_Int_Link,
	SG_PROJ_UNIT_US_Inch,
	SG_PROJ_UNIT_US_Foot,
	SG_PROJ_UNIT_US_Yard,
	SG_PROJ_UNIT_US_Chain,
	SG_PROJ_UNIT_US_Statute_Mile,
	SG_PROJ_UNIT_Indian_Yard,
	SG_PROJ_UNIT_Indian_Foot,
	SG_PROJ_UNIT_Indian_Chain,
	SG_PROJ_UNIT_Undefined
}
TSG_Projection_Unit;

//---------------------------------------------------------
SAGA_API_DLL_EXPORT TSG_Projection_Type	SG_Get_Projection_Type				(const CSG_String &Identifier);
SAGA_API_DLL_EXPORT CSG_String			SG_Get_Projection_Type_Identifier	(TSG_Projection_Type Type);
SAGA_API_DLL_EXPORT CSG_String			SG_Get_Projection_Type_Name			(TSG_Projection_Type Type);

SAGA_API_DLL_EXPORT TSG_Projection_Unit	SG_Get_Projection_Unit				(const CSG_String &Identifier);
SAGA_API_DLL_EXPORT CSG_String			SG_Get_Projection_Unit_Identifier	(TSG_Projection_Unit Unit);
SAGA_API_DLL_EXPORT CSG_String			SG_Get_Projection_Unit_Name			(TSG_Projection_Unit Unit, bool bSimple = false);
SAGA_API_DLL_EXPORT double				SG_Get_Projection_Unit_To_Meter		(TSG_Projection_Unit Unit);


///////////////////////////////////////////////////////////
//														 //
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
	bool							Assign					(const CSG_Projection &Projection);
	CSG_Projection &				operator =				(const CSG_Projection &Projection)	{	Assign(Projection);	return( *this );	}

									CSG_Projection			(int Authority_ID, const SG_Char *Authority = NULL);
	bool							Create					(int Authority_ID, const SG_Char *Authority = NULL);
	bool							Assign					(int Authority_ID, const SG_Char *Authority = NULL);
	CSG_Projection &				operator =				(int Authority_ID)					{	Assign(Authority_ID);	return( *this );	}

									CSG_Projection			(const CSG_String &Projection, TSG_Projection_Format Format = SG_PROJ_FMT_WKT);
	bool							Create					(const CSG_String &Projection, TSG_Projection_Format Format = SG_PROJ_FMT_WKT);
	bool							Assign					(const CSG_String &Projection, TSG_Projection_Format Format = SG_PROJ_FMT_WKT);
	CSG_Projection &				operator =				(const CSG_String &Projection)		{	Assign(Projection);	return( *this );	}

									CSG_Projection			(const CSG_String &WKT, const CSG_String &Proj4);
	bool							Create					(const CSG_String &WKT, const CSG_String &Proj4);
	bool							Assign					(const CSG_String &WKT, const CSG_String &Proj4);

	bool							is_Okay					(void)	const	{	return( m_Type != SG_PROJ_TYPE_CS_Undefined );	}
	bool							is_Equal				(const CSG_Projection &Projection)	const;
	bool							operator ==				(const CSG_Projection &Projection)	const	{	return( is_Equal(Projection) == true  );	}
	bool							operator !=				(const CSG_Projection &Projection)	const	{	return( is_Equal(Projection) == false );	}

	bool							Set_GCS_WGS84			(void);
	bool							Set_UTM_WGS84			(int Zone, bool bSouth = false);

	bool							Load					(const CSG_String &FileName, TSG_Projection_Format Format = SG_PROJ_FMT_WKT);
	bool							Save					(const CSG_String &FileName, TSG_Projection_Format Format = SG_PROJ_FMT_WKT)	const;

	bool							Load					(CSG_File &Stream, TSG_Projection_Format Format = SG_PROJ_FMT_WKT);
	bool							Save					(CSG_File &Stream, TSG_Projection_Format Format = SG_PROJ_FMT_WKT)	const;

	bool							Load					(const CSG_MetaData &Projection);
	bool							Save					(      CSG_MetaData &Projection)	const;

	const CSG_String &				Get_Name				(void)	const	{	return( m_Name          );	}
	const CSG_String &				Get_WKT					(void)	const	{	return( m_WKT           );	}
	const CSG_String &				Get_Proj4				(void)	const	{	return( m_Proj4         );	}
	const CSG_String &				Get_Authority			(void)	const	{	return( m_Authority     );	}
	int								Get_Authority_ID		(void)	const	{	return( m_Authority_ID  );	}
	int								Get_EPSG				(void)	const	{	return( m_Authority.Cmp("EPSG") ? -1 : m_Authority_ID );	}

	CSG_String						Get_Description			(void)	const;

	TSG_Projection_Type				Get_Type				(void)	const	{	return( m_Type  );	}
	CSG_String						Get_Type_Identifier		(void)	const	{	return( SG_Get_Projection_Type_Identifier(m_Type) );	}
	CSG_String						Get_Type_Name			(void)	const	{	return( SG_Get_Projection_Type_Name      (m_Type) );	}

	TSG_Projection_Unit				Get_Unit				(void)	const	{	return( m_Unit  );	}
	CSG_String						Get_Unit_Identifier		(void)	const	{	return( SG_Get_Projection_Unit_Identifier(m_Unit) );	}
	CSG_String						Get_Unit_Name			(void)	const	{	return( m_Unit_Name     );	}
	double							Get_Unit_To_Meter		(void)	const	{	return( m_Unit_To_Meter );	}


private:

	int								m_Authority_ID;

	double							m_Unit_To_Meter;

	TSG_Projection_Type				m_Type;

	TSG_Projection_Unit				m_Unit;

	CSG_String						m_Name, m_WKT, m_Proj4, m_Authority, m_Unit_Name;


};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** CSG_Projections is a projections dictionary and translator
  * for EPSG codes, OGC Well-Known-Text, and Proj.4.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Projections
{
public:
	CSG_Projections(void);
	virtual ~CSG_Projections(void);

									CSG_Projections			(const CSG_String &File_DB);
	bool							Create					(const CSG_String &File_DB);

	void							Destroy					(void);

	bool							Reset_Dictionary		(void);
	bool							Load_Dictionary			(const CSG_String &FileName);
	bool							Save_Dictionary			(const CSG_String &FileName);

	bool							Load_DB					(const CSG_String &FileName, bool bAppend = false);
	bool							Save_DB					(const CSG_String &FileName);

	int								Get_Count				(void)	const;

	bool							Add						(const CSG_Projection &Projection);
	bool							Add						(const SG_Char *WKT, const SG_Char *Proj4, const SG_Char *Authority, int Authority_ID);

	CSG_Projection					Get_Projection			(int Index)	const;
	CSG_Projection					operator []				(int Index) const	{	return( Get_Projection(Index) );	}

	bool							Get_Projection			(CSG_Projection &Projection, int EPSG_ID)									const;
	bool							Get_Projection			(CSG_Projection &Projection, const CSG_String &Authority, int Authority_ID)	const;

	CSG_String						Get_Names_List			(TSG_Projection_Type Type = SG_PROJ_TYPE_CS_Undefined)	const;

	static CSG_MetaData				WKT_to_MetaData			(const CSG_String &WKT);

	bool							WKT_to_Proj4			(CSG_String &Proj4, const CSG_String &WKT  )	const;
	bool							WKT_from_Proj4			(CSG_String &WKT  , const CSG_String &Proj4)	const;

	bool							EPSG_to_Proj4			(CSG_String &Proj4, int EPSG_Code)				const;
	bool							EPSG_to_WKT				(CSG_String &WKT  , int EPSG_Code)				const;


private:

	CSG_Translator					m_WKT_to_Proj4, m_Proj4_to_WKT, m_EPSG_to_Idx;

	class CSG_Table					*m_pProjections;


	void							_On_Construction			(void);

	static bool						_WKT_to_MetaData			(CSG_MetaData &MetaData, const CSG_String &WKT);

	bool							_WKT_to_Proj4_Set_Datum		(CSG_String &Proj4, const CSG_MetaData &WKT)	const;

	bool							_Proj4_Read_Parameter		(CSG_String &Value, const CSG_String &Proj4, const CSG_String &Key)	const;
	bool							_Proj4_Get_Ellipsoid		(CSG_String &Value, const CSG_String &Proj4)	const;
	bool							_Proj4_Get_Datum			(CSG_String &Value, const CSG_String &Proj4)	const;
	bool							_Proj4_Get_Prime_Meridian	(CSG_String &Value, const CSG_String &Proj4)	const;
	bool							_Proj4_Get_Unit				(CSG_String &Value, const CSG_String &Proj4)	const;

	bool							_Set_Dictionary				(CSG_Table      &Dictionary, int Direction);
	bool							_Set_Dictionary				(CSG_Translator &Dictionary, int Direction);

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_Projections &	SG_Get_Projections	(void);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool		SG_Get_Projected				(class CSG_Shapes *pSource, class CSG_Shapes *pTarget, const CSG_Projection &Target);

SAGA_API_DLL_EXPORT bool		SG_Get_Projected				(const CSG_Projection &Source, const CSG_Projection &Target, TSG_Point &Point    );
SAGA_API_DLL_EXPORT bool		SG_Get_Projected				(const CSG_Projection &Source, const CSG_Projection &Target, TSG_Rect  &Rectangle);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool		SG_Grid_Get_Geographic_Coordinates		(CSG_Grid *pGrid, CSG_Grid *pLon, CSG_Grid *pLat);


///////////////////////////////////////////////////////////
//														 //
//						Functions						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT double		SG_Get_Length					(double dx, double dy);

SAGA_API_DLL_EXPORT double		SG_Get_Distance					(double ax, double ay, double bx, double by, bool bPolar);
SAGA_API_DLL_EXPORT double		SG_Get_Distance					(const TSG_Point &A, const TSG_Point &B    , bool bPolar);

SAGA_API_DLL_EXPORT double		SG_Get_Distance					(double ax, double ay, double bx, double by);
SAGA_API_DLL_EXPORT double		SG_Get_Distance					(const TSG_Point &A, const TSG_Point &B);

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

SAGA_API_DLL_EXPORT bool		SG_Is_Point_On_Line				(const TSG_Point &Point, const TSG_Point &Ln_A, const TSG_Point &Ln_B, bool bExactMatch = false, double Epsilon = 0.);
SAGA_API_DLL_EXPORT double		SG_Get_Nearest_Point_On_Line	(const TSG_Point &Point, const TSG_Point &Ln_A, const TSG_Point &Ln_B, TSG_Point &Ln_Point, bool bExactMatch = true);

SAGA_API_DLL_EXPORT bool		SG_Get_Triangle_CircumCircle	(TSG_Point Triangle[3], TSG_Point &Point, double &Radius);

SAGA_API_DLL_EXPORT double		SG_Get_Polygon_Area				(TSG_Point *Points, int nPoints);
SAGA_API_DLL_EXPORT double		SG_Get_Polygon_Area				(const CSG_Points &Points);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__geo_tools_H
