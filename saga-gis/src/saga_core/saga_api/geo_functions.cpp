
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
//                  geo_functions.cpp                    //
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
#include "geo_tools.h"
#include "mat_tools.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_Is_Equal(double a, double b, double epsilon)
{
	return( fabs(a - b) <= epsilon );
}

//---------------------------------------------------------
bool		SG_Is_Equal(const TSG_Point &A, const TSG_Point &B, double epsilon)
{
	return( SG_Is_Equal(A.x, B.x, epsilon)
		&&  SG_Is_Equal(A.y, B.y, epsilon) );
}

//---------------------------------------------------------
bool		SG_Is_Between(double x, double a, double b, double epsilon)
{
	return( (a - epsilon <= x && x <= b + epsilon)
		||  (b - epsilon <= x && x <= a + epsilon) );
}

bool		SG_Is_Between(const TSG_Point &Point, const TSG_Point &Corner_A, const TSG_Point &Corner_B, double epsilon)
{
	return( SG_Is_Between(Point.x, Corner_A.x, Corner_B.x, epsilon)
		&&  SG_Is_Between(Point.y, Corner_A.y, Corner_B.y, epsilon) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double		SG_Get_Length(double dx, double dy)
{
	return( sqrt(dx*dx + dy*dy) );
}

//---------------------------------------------------------
double		SG_Get_Distance(double ax, double ay, double bx, double by, bool bPolar)
{
	return( bPolar ? SG_Get_Distance_Polar(ax, ay, bx, by) : SG_Get_Distance(ax, ay, bx, by) );
}

//---------------------------------------------------------
double		SG_Get_Distance(const TSG_Point &A, const TSG_Point &B, bool bPolar)
{
	return( bPolar ? SG_Get_Distance_Polar(A, B) : SG_Get_Distance(A, B) );
}

//---------------------------------------------------------
double		SG_Get_Distance(double ax, double ay, double bx, double by)
{
	ax	-= bx;
	ay	-= by;

	return( sqrt(ax*ax + ay*ay) );
}

//---------------------------------------------------------
double		SG_Get_Distance(const TSG_Point &A, const TSG_Point &B)
{
	double	dx,	dy;

	dx	= B.x - A.x;
	dy	= B.y - A.y;

	return( sqrt(dx*dx + dy*dy) );
}

//---------------------------------------------------------
double		SG_Get_Distance(double ax, double ay, double az, double bx, double by, double bz)
{
    ax	-= bx;
    ay	-= by;
    az  -= bz;

    return( sqrt(ax*ax + ay*ay + az*az) );
}

//---------------------------------------------------------
double		SG_Get_Distance(const TSG_Point_3D &A, const TSG_Point_3D &B)
{
    double	dx,	dy, dz;

    dx	= B.x - A.x;
    dy	= B.y - A.y;
    dz	= B.z - A.z;

    return( sqrt(dx*dx + dy*dy + dz*dz) );
}

//---------------------------------------------------------
double	SG_Get_Distance_Polar(double aLon, double aLat, double bLon, double bLat, double a, double e, bool bDegree)
{
	if( bDegree )
	{
		aLon	*= M_DEG_TO_RAD;
		aLat	*= M_DEG_TO_RAD;
		bLon	*= M_DEG_TO_RAD;
		bLat	*= M_DEG_TO_RAD;
	}

	if( e <= 0. )
	{
		return(	a * acos(sin(aLat) * sin(bLat) + cos(aLat) * cos(bLat) * cos(bLon - aLon)) );
	}
	else
	{
		double	F	= (aLat + bLat) / 2.;
		double	G	= (aLat - bLat) / 2.;
		double	l	= (aLon - bLon) / 2.;

		double	sin2_F	= SG_Get_Square(sin(F));
		double	cos2_F	= SG_Get_Square(cos(F));
		double	sin2_G	= SG_Get_Square(sin(G));
		double	cos2_G	= SG_Get_Square(cos(G));
		double	sin2_l	= SG_Get_Square(sin(l));
		double	cos2_l	= SG_Get_Square(cos(l));

		double	S	= sin2_G * cos2_l + cos2_F * sin2_l;
		double	C	= cos2_G * cos2_l + sin2_F * sin2_l;

		double	w	= atan(sqrt(S / C));
		double	D	= 2. * w * a;

		double	R	= sqrt(S * C) / w;
		double	H1	= (3. * R - 1.) / (2. * C);
		double	H2	= (3. * R + 1.) / (2. * S);

		double	f	= 1. / e;

		double	d	= D * (1. + f * H1 * sin2_F * cos2_G - f * H2 * cos2_F * sin2_G);

		return( d );
	}
}

//---------------------------------------------------------
double	SG_Get_Distance_Polar(const TSG_Point &A, const TSG_Point &B, double a, double e, bool bDegree)
{
	return( SG_Get_Distance_Polar(A.x, A.y, B.x, B.y, a, e, bDegree) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double		SG_Get_Angle_Of_Direction(double dx, double dy)
{
	if( dx == 0. )
	{
		return( dy > 0. ? 0. : dy < 0. ? M_PI_180 : -M_PI_360 );
	}

	dx	= M_PI_090 - atan2(dy, dx);

	return( dx < 0. ? M_PI_360 + dx : dx );
}

//---------------------------------------------------------
double		SG_Get_Angle_Of_Direction(double ax, double ay, double bx, double by)
{
	return( SG_Get_Angle_Of_Direction(bx - ax, by - ay) );
}

//---------------------------------------------------------
double		SG_Get_Angle_Of_Direction(const TSG_Point &A)
{
	return( SG_Get_Angle_Of_Direction(A.x, A.y) );
}

//---------------------------------------------------------
double		SG_Get_Angle_Of_Direction(const TSG_Point &A, const TSG_Point &B)
{
	return( SG_Get_Angle_Of_Direction(B.x - A.x, B.y - A.y) );
}

//---------------------------------------------------------
double		SG_Get_Angle_Difference(double a, double b)
{
	double	d	= fmod(b - a, M_PI_360);

	if( d < 0. )	d	+= M_PI_360;

	return( d > M_PI_180 ? d - M_PI_180 : d );
}

//---------------------------------------------------------
bool		SG_is_Angle_Between(double Angle, double Angle_Min, double Angle_Max, bool bCheckRange)
{
	if( bCheckRange )
	{
		Angle     = fmod(Angle    , M_PI_360); if( Angle     < 0. ) Angle     += M_PI_360;
		Angle_Min = fmod(Angle_Min, M_PI_360); if( Angle_Min < 0. ) Angle_Min += M_PI_360;
		Angle_Max = fmod(Angle_Max, M_PI_360); if( Angle_Max < 0. ) Angle_Max += M_PI_360;
	}

	return( Angle_Min <= Angle_Max
		? Angle_Min <= Angle && Angle <= Angle_Max
		: Angle_Min <= Angle || Angle <= Angle_Max
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	SG_Get_Crossing(TSG_Point &Crossing, const TSG_Point &a1, const TSG_Point &a2, const TSG_Point &b1, const TSG_Point &b2, bool bExactMatch)
{
	//-----------------------------------------------------
	if( bExactMatch
	&&	(	(M_GET_MAX(a1.x, a2.x) < M_GET_MIN(b1.x, b2.x))
		||	(M_GET_MIN(a1.x, a2.x) > M_GET_MAX(b1.x, b2.x))
		||	(M_GET_MAX(a1.y, a2.y) < M_GET_MIN(b1.y, b2.y))
		||	(M_GET_MIN(a1.y, a2.y) > M_GET_MAX(b1.y, b2.y))	) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( (a1.x == b1.x && a1.y == b1.y) || (a1.x == b2.x && a1.y == b2.y) )
	{
		Crossing	= a1;

		return( true );
	}

	if( (a2.x == b1.x && a2.y == b1.y) || (a2.x == b2.x && a2.y == b2.y) )
	{
		Crossing	= a2;

		return( true );
	}

	//-----------------------------------------------------
	double	lambda, div, a_dx, a_dy, b_dx, b_dy;

	a_dx	= a2.x - a1.x;
	a_dy	= a2.y - a1.y;

	b_dx	= b2.x - b1.x;
	b_dy	= b2.y - b1.y;

	if( (div = a_dx * b_dy - b_dx * a_dy) != 0. )
	{
		lambda		= ((b1.x - a1.x) * b_dy - b_dx * (b1.y - a1.y)) / div;

		Crossing.x	= a1.x + lambda * a_dx;
		Crossing.y	= a1.y + lambda * a_dy;

		if( !bExactMatch )
		{
			return( true );
		}
		else if( 0. <= lambda && lambda <= 1. )
		{
			lambda	= ((b1.x - a1.x) * a_dy - a_dx * (b1.y - a1.y)) / div;

			if( 0. <= lambda && lambda <= 1. )
			{
				return( true );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool	SG_Get_Crossing_InRegion(TSG_Point &Crossing, const TSG_Point &a, const TSG_Point &b, const TSG_Rect &Region)
{
	TSG_Point	ra, rb;

	//-----------------------------------------------------
	ra.y			= Region.yMin;
	rb.y			= Region.yMax;

	ra.x	= rb.x	= Region.xMin;

	if(	SG_Get_Crossing(Crossing, a, b, ra, rb, true) )
	{
		return( true );
	}

	//-----------------------------------------------------
	ra.x	= rb.x	= Region.xMax;

	if(	SG_Get_Crossing(Crossing, a, b, ra, rb, true) )
	{
		return( true );
	}

	//-----------------------------------------------------
	ra.x			= Region.xMin;
	ra.y			= Region.yMax;

	if(	SG_Get_Crossing(Crossing, a, b, ra, rb, true) )
	{
		return( true );
	}

	//-----------------------------------------------------
	ra.y	= rb.y	= Region.yMin;

	if(	SG_Get_Crossing(Crossing, a, b, ra, rb, true) )
	{
		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_Is_Point_On_Line(const TSG_Point &Point, const TSG_Point &Line_A, const TSG_Point &Line_B, bool bExactMatch, double Epsilon)
{
	if( SG_Is_Equal(Line_B.x, Line_A.x, Epsilon) )	// vertical line
	{
		return( SG_Is_Between(Point.y, Line_A.y, Line_B.y, Epsilon) && (!bExactMatch || SG_Is_Between(Point.x, Line_A.x, Line_B.x, Epsilon)) );
	}

	if( bExactMatch && !SG_Is_Between(Point, Line_A, Line_B, Epsilon) )
	{
		return( false );
	}

	double	b	= (Line_B.y - Line_A.y) / (Line_B.x - Line_A.x);
	double	a	= Line_A.y - b * Line_A.x;

	return( SG_Is_Equal(Point.y, a + b * Point.x, Epsilon) );
}

//---------------------------------------------------------
double		SG_Get_Distance_To_Line(const TSG_Point &Point, const TSG_Point &Line_A, const TSG_Point &Line_B, bool bExactMatch)
{
	TSG_Point Line_Point;

	return( SG_Get_Nearest_Point_On_Line(Point, Line_A, Line_B, Line_Point, bExactMatch) );
}

//---------------------------------------------------------
double		SG_Get_Nearest_Point_On_Line(const TSG_Point &Point, const TSG_Point &Line_A, const TSG_Point &Line_B, TSG_Point &Line_Point, bool bExactMatch)
{
	CSG_Point Point_Ortho(
		Point.x - (Line_B.y - Line_A.y),
		Point.y + (Line_B.x - Line_A.x)
	);

	if( !SG_Get_Crossing(Line_Point, Line_A, Line_B, Point, Point_Ortho, false) )
	{
		return( -1. );
	}

	if( !bExactMatch || (bExactMatch
	&&  SG_IS_BETWEEN(Line_A.x, Line_Point.x, Line_B.x)
	&&  SG_IS_BETWEEN(Line_A.y, Line_Point.y, Line_B.y)) )
	{
		return( SG_Get_Distance(Point, Line_Point) );
	}

	double Distance_A = SG_Get_Distance(Point, Line_A);
	double Distance_B = SG_Get_Distance(Point, Line_B);

	if( Distance_A < Distance_B )
	{
		Line_Point	= Line_A;

		return( Distance_A );
	}
	else
	{
		Line_Point	= Line_B;

		return( Distance_B );
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_Get_Triangle_CircumCircle(TSG_Point Triangle[3], TSG_Point &Point, double &Radius)
{
	#define A	Triangle[0]
	#define B	Triangle[1]
	#define C	Triangle[2]

	//-----------------------------------------------------
	TSG_Point	AB, AC, AB_M, AC_M, AB_N, AC_N;

	AB.x	= B.x - A.x;
	AB.y	= B.y - A.y;
	AB_M.x	= A.x + AB.x / 2.;
	AB_M.y	= A.y + AB.y / 2.;
	AB_N.x	= AB_M.x - AB.y;
	AB_N.y	= AB_M.y + AB.x;

	AC.x	= C.x - A.x;
	AC.y	= C.y - A.y;
	AC_M.x	= A.x + AC.x / 2.;
	AC_M.y	= A.y + AC.y / 2.;
	AC_N.x	= AC_M.x - AC.y;
	AC_N.y	= AC_M.y + AC.x;

	if( SG_Get_Crossing(Point, AB_M, AB_N, AC_M, AC_N, false) )
	{
		AB.x	= A.x - Point.x;
		AB.y	= A.y - Point.y;

		Radius	= sqrt(AB.x*AB.x + AB.y*AB.y);

		return( true );
	}

	return( false );

	//-----------------------------------------------------
	#undef A
	#undef B
	#undef C
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double		SG_Get_Polygon_Area(TSG_Point *Points, int nPoints)
{
	double Area = 0.;

	if( nPoints >= 3 )
	{
		TSG_Point *iP = Points, *jP = Points + nPoints - 1;

		for(int i=0; i<nPoints; i++, jP=iP++)
		{
			Area += (jP->x * iP->y) - (iP->x * jP->y);
		}

		Area /= 2.;
	}

	return( Area );
}

//---------------------------------------------------------
double		SG_Get_Polygon_Area(const CSG_Points &Points)
{
	double Area = 0.;

	if( Points.Get_Count() >= 3 )
	{
		for(sLong i=0, j=Points.Get_Count()-1; i<Points.Get_Count(); j=i++)
		{
			Area += (Points[j].x * Points[i].y)
			      - (Points[i].x * Points[j].y);
		}

		Area /= 2.;
	}

	return( Area );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
