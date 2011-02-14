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
class SAGA_API_DLL_EXPORT CSG_Point
{
public:
	CSG_Point(void);
	CSG_Point(const CSG_Point &Point);
	CSG_Point(const TSG_Point &Point);
	CSG_Point(double x, double y);

	virtual ~CSG_Point(void)	{}

	virtual TSG_Point_Type		Get_Type		(void)	const	{	return( SG_POINT_TYPE_XY );	}

	operator TSG_Point							(void)	const	{	TSG_Point p; p.x = m_x; p.y = m_y; return( p );	}

	double						Get_X			(void)	const	{	return( m_x );	}
	void						Set_X			(double x)		{	m_x	= x;		}
	double						Get_Y			(void)	const	{	return( m_y );	}
	void						Set_Y			(double y)		{	m_y	= y;		}

	virtual bool				operator ==		(const CSG_Point &Point)	const	{	return(  is_Equal(Point) );	}
	virtual bool				operator !=		(const CSG_Point &Point)	const	{	return( !is_Equal(Point) );	}

	virtual CSG_Point			operator +		(const CSG_Point &Point)	const	{	return( CSG_Point(m_x + Point.m_x, m_y + Point.m_y)	);	}
	virtual CSG_Point			operator -		(const CSG_Point &Point)	const	{	return( CSG_Point(m_x - Point.m_x, m_y - Point.m_y)	);	}

	virtual CSG_Point &			operator  =		(const CSG_Point &Point)			{	Assign  (Point);	return( *this );	}
	virtual CSG_Point &			operator +=		(const CSG_Point &Point)			{	Add     (Point);	return( *this );	}
	virtual CSG_Point &			operator -=		(const CSG_Point &Point)			{	Subtract(Point);	return( *this );	}

	virtual void				Assign			(double x, double y);
	virtual void				Assign			(const CSG_Point &Point);
	virtual void				Add				(const CSG_Point &Point);
	virtual void				Subtract		(const CSG_Point &Point);

	virtual bool				is_Equal		(double x, double y)		const	{	return(	m_x == x && m_y == y );	}
	virtual bool				is_Equal		(const CSG_Point &Point)	const	{	return(	m_x == Point.m_x && m_y == Point.m_y );	}


protected:

	double						m_x, m_y;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Point_Z : public CSG_Point
{
public:
	CSG_Point_Z(void);
	CSG_Point_Z(const CSG_Point_Z &Point);
	CSG_Point_Z(const TSG_Point_Z &Point);
	CSG_Point_Z(double x, double y, double z);

	virtual ~CSG_Point_Z(void)	{}

	virtual TSG_Point_Type		Get_Type		(void)	const	{	return( SG_POINT_TYPE_XYZ );	}

	operator TSG_Point_Z						(void)	const	{	TSG_Point_Z p; p.x = m_x; p.y = m_y; p.z = m_z; return( p );	}

	double						Get_Z			(void)	const	{	return( m_z );	}
	void						Set_Z			(double z)		{	m_z	= z;		}

	virtual bool				operator ==		(const CSG_Point_Z &Point)	const	{	return(  is_Equal(Point) );	}
	virtual bool				operator !=		(const CSG_Point_Z &Point)	const	{	return( !is_Equal(Point) );	}

	virtual CSG_Point_Z			operator +		(const CSG_Point_Z &Point)	const	{	return( CSG_Point_Z(m_x + Point.m_x, m_y + Point.m_y, m_z + Point.m_z)	);	}
	virtual CSG_Point_Z			operator -		(const CSG_Point_Z &Point)	const	{	return( CSG_Point_Z(m_x - Point.m_x, m_y - Point.m_y, m_z - Point.m_z)	);	}

	virtual CSG_Point_Z &		operator  =		(const CSG_Point_Z &Point)			{	Assign  (Point);	return( *this );	}
	virtual CSG_Point_Z &		operator +=		(const CSG_Point_Z &Point)			{	Add     (Point);	return( *this );	}
	virtual CSG_Point_Z &		operator -=		(const CSG_Point_Z &Point)			{	Subtract(Point);	return( *this );	}

	virtual void				Assign			(double x, double y, double z);
	virtual void				Assign			(const CSG_Point_Z &Point);
	virtual void				Add				(const CSG_Point_Z &Point);
	virtual void				Subtract		(const CSG_Point_Z &Point);

	virtual bool				is_Equal		(double x, double y, double z)	const	{	return(	m_x == x && m_y == y && m_z == z );	}
	virtual bool				is_Equal		(const CSG_Point_Z &Point)		const	{	return(	m_x == Point.m_x && m_y == Point.m_y && m_z == Point.m_z );	}


protected:

	double						m_z;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Point_ZM : public CSG_Point_Z
{
public:
	CSG_Point_ZM(void);
	CSG_Point_ZM(const CSG_Point_ZM &Point);
	CSG_Point_ZM(const TSG_Point_ZM &Point);
	CSG_Point_ZM(double x, double y, double z, double m);

	virtual ~CSG_Point_ZM(void)	{}

	virtual TSG_Point_Type		Get_Type		(void)	const	{	return( SG_POINT_TYPE_XYZM );	}

	operator TSG_Point_ZM						(void)	const	{	TSG_Point_ZM p; p.x = m_x; p.y = m_y; p.z = m_z; p.m = m_m; return( p );	}

	double						Get_M			(void)	const	{	return( m_m );	}
	void						Set_M			(double m)		{	m_m	= m;		}

	virtual bool				operator ==		(const CSG_Point_ZM &Point)	const	{	return(  is_Equal(Point) );	}
	virtual bool				operator !=		(const CSG_Point_ZM &Point)	const	{	return( !is_Equal(Point) );	}

	virtual CSG_Point_ZM		operator +		(const CSG_Point_ZM &Point)	const	{	return( CSG_Point_ZM(m_x + Point.m_x, m_y + Point.m_y, m_z + Point.m_z, m_m + Point.m_m) );	}
	virtual CSG_Point_ZM		operator -		(const CSG_Point_ZM &Point)	const	{	return( CSG_Point_ZM(m_x - Point.m_x, m_y - Point.m_y, m_z - Point.m_z, m_m - Point.m_m) );	}

	virtual CSG_Point_ZM &		operator  =		(const CSG_Point_ZM &Point)			{	Assign  (Point);	return( *this );	}
	virtual CSG_Point_ZM &		operator +=		(const CSG_Point_ZM &Point)			{	Add     (Point);	return( *this );	}
	virtual CSG_Point_ZM &		operator -=		(const CSG_Point_ZM &Point)			{	Subtract(Point);	return( *this );	}

	virtual void				Assign			(double x, double y, double z, double m);
	virtual void				Assign			(const CSG_Point_ZM &Point);
	virtual void				Add				(const CSG_Point_ZM &Point);
	virtual void				Subtract		(const CSG_Point_ZM &Point);

	virtual bool				is_Equal		(double x, double y, double z, double m)	const	{	return(	m_x == x && m_y == y && m_z == z && m_m == m );	}
	virtual bool				is_Equal		(const CSG_Point_ZM &Point)					const	{	return(	m_x == Point.m_x && m_y == Point.m_y && m_z == Point.m_z && m_m == Point.m_m );	}


protected:

	double						m_m;

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

	bool						is_Equal		(double xMin, double yMin, double xMax, double yMax) const;
	bool						is_Equal		(const CSG_Rect &Rect) const;

	double						Get_XMin		(void) const	{	return( m_rect.xMin );	}
	double						Get_XMax		(void) const	{	return( m_rect.xMax );	}
	double						Get_YMin		(void) const	{	return( m_rect.yMin );	}
	double						Get_YMax		(void) const	{	return( m_rect.yMax );	}

	double						Get_XRange		(void) const	{	return( m_rect.xMax - m_rect.xMin );	}
	double						Get_YRange		(void) const	{	return( m_rect.yMax - m_rect.yMin );	}

	CSG_Point					Get_TopLeft		(void)			{	return( CSG_Point(m_rect.xMin, m_rect.yMax) );	}
	CSG_Point					Get_BottomRight	(void)			{	return( CSG_Point(m_rect.xMax, m_rect.yMin) );	}

	CSG_Point					Get_Center		(void) const	{	return( CSG_Point(Get_XCenter(), Get_YCenter()) );	}
	double						Get_XCenter		(void) const	{	return( (m_rect.xMin + m_rect.xMax) / 2.0 );	}
	double						Get_YCenter		(void) const	{	return( (m_rect.yMin + m_rect.yMax) / 2.0 );	}

	void						Move			(double dx, double dy);
	void						Move			(const CSG_Point &Point);

	void						Inflate			(double d, bool bPercent = true);
	void						Deflate			(double d, bool bPercent = true);
	void						Inflate			(double dx, double dy, bool bPercent = true);
	void						Deflate			(double dx, double dy, bool bPercent = true);

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

	class CSG_Parameters *	Get_Parameters		(void)	const		{	return( m_pParameters );	}
	bool					Set_Parameters		(class CSG_Parameters *pParameters);

	TSG_Distance_Weighting	Get_Weighting		(void)	const		{	return( m_Weighting );		}
	bool					Set_Weighting		(TSG_Distance_Weighting Weighting);

	double					Get_IDW_Power		(void)	const		{	return( m_IDW_Power );		}
	bool					Set_IDW_Power		(double Value);

	bool					Get_IDW_Offset		(void)	const		{	return( m_IDW_bOffset );	}
	bool					Set_IDW_Offset		(bool bOn = true);

	double					Get_BandWidth		(void)	const		{	return( m_Bandwidth );		}
	bool					Set_BandWidth		(double Value);

	//-----------------------------------------------------
	double					Get_Weight			(double Distance)	const
	{
		if( Distance < 0.0 )
		{
			return( 0.0 );
		}

		switch( m_Weighting )
		{
		case SG_DISTWGHT_None: default:
			return( 1.0 );

		case SG_DISTWGHT_IDW:
			if( m_IDW_bOffset )
				return( pow(1.0 + Distance, -m_IDW_Power) );
			else
				return( Distance > 0.0 ? pow(Distance, -m_IDW_Power) : 0.0 );

		case SG_DISTWGHT_EXP:
			return( exp(-Distance / m_Bandwidth) );

		case SG_DISTWGHT_GAUSS:
			Distance	/= m_Bandwidth;
			return( exp(-0.5 * Distance*Distance) );
		}
	}


private:

	bool					m_IDW_bOffset;

	double					m_IDW_Power, m_Bandwidth;

	TSG_Distance_Weighting	m_Weighting;

	class CSG_Parameters	*m_pParameters;

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
	SG_PROJ_TYPE_CS_Undefined	= 0,
	SG_PROJ_TYPE_CS_Projected,
	SG_PROJ_TYPE_CS_Geographic,
	SG_PROJ_TYPE_CS_Geocentric
}
TSG_Projection_Type;

//---------------------------------------------------------
const SG_Char	gSG_Projection_Type_Identifier[][32]	=
{
	SG_T("Undefined"),
	SG_T("PROJCS"),
	SG_T("GEOGCS"),
	SG_T("GEOCCS")
};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_String	SG_Get_Projection_Type_Name	(TSG_Projection_Type Type);

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

									CSG_Projection			(int EPSG_ID);
	bool							Create					(int EPSG_ID);
	bool							Assign					(int EPSG_ID);
	CSG_Projection &				operator =				(int EPSG_ID)						{	Assign(EPSG_ID);	return( *this );	}

									CSG_Projection			(const CSG_String &Projection, TSG_Projection_Format Format = SG_PROJ_FMT_WKT);
	bool							Create					(const CSG_String &Projection, TSG_Projection_Format Format = SG_PROJ_FMT_WKT);
	bool							Assign					(const CSG_String &Projection, TSG_Projection_Format Format = SG_PROJ_FMT_WKT);
	CSG_Projection &				operator =				(const CSG_String &Projection)		{	Assign(Projection);	return( *this );	}

	bool							is_Okay					(void)	const	{	return( m_Type != SG_PROJ_TYPE_CS_Undefined );	}
	bool							is_Equal				(const CSG_Projection &Projection)	const;
	bool							operator ==				(const CSG_Projection &Projection)	const	{	return( is_Equal(Projection) == true  );	}
	bool							operator !=				(const CSG_Projection &Projection)	const	{	return( is_Equal(Projection) == false );	}

	bool							Load					(const CSG_String &File_Name, TSG_Projection_Format Format = SG_PROJ_FMT_WKT);
	bool							Save					(const CSG_String &File_Name, TSG_Projection_Format Format = SG_PROJ_FMT_WKT)	const;

	bool							Load					(const CSG_MetaData &Projection);
	bool							Save					(      CSG_MetaData &Projection)	const;

	const CSG_String &				Get_Name				(void)	const	{	return( m_Name          );	}
	const CSG_String &				Get_WKT					(void)	const	{	return( m_WKT           );	}
	const CSG_String &				Get_Proj4				(void)	const	{	return( m_Proj4         );	}
	const CSG_String &				Get_Authority			(void)	const	{	return( m_Authority     );	}
	int								Get_Authority_ID		(void)	const	{	return( m_Authority_ID  );	}
	int								Get_EPSG				(void)	const	{	return( m_Authority.Cmp(SG_T("EPSG")) ? -1 : m_Authority_ID );	}

	CSG_String						Get_Description			(void)	const;

	TSG_Projection_Type				Get_Type				(void)	const	{	return( m_Type  );	}
	CSG_String						Get_Type_Name			(void)	const	{	return( gSG_Projection_Type_Identifier[m_Type] );	}
	CSG_String						Get_Type_Identifier		(void)	const	{	return( SG_Get_Projection_Type_Name(m_Type) );		}


private:

	int								m_Authority_ID;

	TSG_Projection_Type				m_Type;

	CSG_String						m_Name, m_WKT, m_Proj4, m_Authority;


};

//---------------------------------------------------------
/** CSG_Projections is a projections dictionary and translator
  * for EPSG codes, OGc Well-Known-Text, and Proj.4.
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
	bool							Load_Dictionary			(const CSG_String &File_Name);
	bool							Save_Dictionary			(const CSG_String &File_Name);

	bool							Load_DB					(const CSG_String &File_Name, bool bAppend = false);
	bool							Save_DB					(const CSG_String &File_Name);

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


	void							_On_Construction		(void);

	static bool						_WKT_to_MetaData		(CSG_MetaData &MetaData, const CSG_String &WKT);

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


///////////////////////////////////////////////////////////
//														 //
//						Functions						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT double		SG_Get_Length					(double dx, double dy);
SAGA_API_DLL_EXPORT double		SG_Get_Distance					(double ax, double ay, double bx, double by);
SAGA_API_DLL_EXPORT double		SG_Get_Distance					(const TSG_Point &A, const TSG_Point &B);

SAGA_API_DLL_EXPORT double		SG_Get_Angle_Of_Direction		(double dx, double dy);
SAGA_API_DLL_EXPORT double		SG_Get_Angle_Of_Direction		(double ax, double ay, double bx, double by);
SAGA_API_DLL_EXPORT double		SG_Get_Angle_Of_Direction		(const TSG_Point &A);
SAGA_API_DLL_EXPORT double		SG_Get_Angle_Of_Direction		(const TSG_Point &A, const TSG_Point &B);

SAGA_API_DLL_EXPORT bool		SG_Get_Crossing					(TSG_Point &Crossing, const TSG_Point &a1, const TSG_Point &a2, const TSG_Point &b1, const TSG_Point &b2, bool bExactMatch = true);
SAGA_API_DLL_EXPORT bool		SG_Get_Crossing_InRegion		(TSG_Point &Crossing, const TSG_Point &a , const TSG_Point & b, const TSG_Rect &Region);

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
