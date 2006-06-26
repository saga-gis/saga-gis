
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
//					Tools - Functions					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "geo_tools.h"
#include "mat_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	IS_BETWEEN(a, x, b)		((a <= x && x <= b) || (b <= x && x <= a))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double		GEO_Get_Distance(double xA, double yA, double xB, double yB)
{
	xA	-= xB;
	yA	-= yB;

	return( sqrt(xA*xA + yA*yA) );
}

//---------------------------------------------------------
double		GEO_Get_Distance(TGEO_Point A, TGEO_Point B)
{
	A.x -= B.x;
	A.y -= B.y;

	return( sqrt(A.x*A.x + A.y*A.y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double		GEO_Get_Angle_Of_Direction(double x, double y)
{
	double	d;

	if( x != 0.0 )
		d	= M_PI_180 + atan2(y, x);
	else
		d	= y > 0.0 ? M_PI_270 : (y < 0.0 ? M_PI_090 : 0.0);

	return( d );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	GEO_Get_Crossing(TGEO_Point &Crossing, TGEO_Point a1, TGEO_Point a2, TGEO_Point b1, TGEO_Point b2, bool bExactMatch)
{
	double	lambda, div, a_dx, a_dy, b_dx, b_dy;

	a_dx	= a2.x - a1.x;
	a_dy	= a2.y - a1.y;

	b_dx	= b2.x - b1.x;
	b_dy	= b2.y - b1.y;

	if( (div = a_dx * b_dy - b_dx * a_dy) != 0.0 )
	{
		lambda		= ((b1.x - a1.x) * b_dy - b_dx * (b1.y - a1.y)) / div;

		Crossing.x	= a1.x + lambda * a_dx;
		Crossing.y	= a1.y + lambda * a_dy;

		if( !bExactMatch )
		{
			return( true );
		}
		else if( 0.0 <= lambda && lambda <= 1.0 )
		{
			lambda	= ((b1.x - a1.x) * a_dy - a_dx * (b1.y - a1.y)) / div;

			if( 0.0 <= lambda && lambda <= 1.0 )
			{
				return( true );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool	GEO_Get_Crossing_InRegion(TGEO_Point &Crossing, TGEO_Point a, TGEO_Point b, TGEO_Rect Region)
{
	bool		bResult;
	TGEO_Point	pExt_XY, pExt_X, pExt_Y;
	CGEO_Rect	r(Region);

	//-----------------------------------------------------
	if( !r.Contains(a) && r.Contains(b) )
	{
		pExt_X	= a;
		a		= b;
		b		= pExt_X;
		bResult	= true;
	}
	else if( r.Contains(a) && !r.Contains(b) )
	{
		bResult	= true;
	}
	else
	{
		bResult	= false;
	}

	//-----------------------------------------------------
	if( bResult )
	{
		if( a.x > b.x )
		{
			pExt_XY.x	= Region.xMin;
			pExt_X.x	= Region.xMin;
			pExt_Y.x	= Region.xMax;
		}
		else
		{
			pExt_XY.x	= Region.xMax;
			pExt_X.x	= Region.xMax;
			pExt_Y.x	= Region.xMin;
		}
		if( a.y > b.y )
		{
			pExt_XY.y	= Region.yMin;
			pExt_Y.y	= Region.yMin;
			pExt_X.y	= Region.yMax;
		}
		else
		{
			pExt_XY.y	= Region.yMax;
			pExt_Y.y	= Region.yMax;
			pExt_X.y	= Region.yMin;
		}

		//-------------------------------------------------
		if( !GEO_Get_Crossing(Crossing, a, b, pExt_X, pExt_XY) )
		{
			if( !GEO_Get_Crossing(Crossing, a, b, pExt_Y, pExt_XY) )
			{
				bResult	= false;
			}
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double		GEO_Get_Nearest_Point_On_Line(TGEO_Point Point, TGEO_Point Ln_A, TGEO_Point Ln_B, TGEO_Point &Ln_Point, bool bExactMatch)
{
	double		dx, dy, Distance, d;
	TGEO_Point	Point_B;

	Point_B.x	= Point.x - (Ln_B.y - Ln_A.y);
	Point_B.y	= Point.y + (Ln_B.x - Ln_A.x);

	if( GEO_Get_Crossing(Ln_Point, Ln_A, Ln_B, Point, Point_B, false) )
	{
		if( !bExactMatch || (bExactMatch && IS_BETWEEN(Ln_A.x, Ln_Point.x, Ln_B.x) && IS_BETWEEN(Ln_A.y, Ln_Point.y, Ln_B.y)) )
		{
			dx			= Point.x - Ln_Point.x;
			dy			= Point.y - Ln_Point.y;
			Distance	= sqrt(dx*dx + dy*dy);
		}
		else
		{
			dx			= Point.x - Ln_A.x;
			dy			= Point.y - Ln_A.y;
			d			= sqrt(dx*dx + dy*dy);

			dx			= Point.x - Ln_B.x;
			dy			= Point.y - Ln_B.y;
			Distance	= sqrt(dx*dx + dy*dy);

			if( d < Distance )
			{
				Distance	= d;
				Ln_Point	= Ln_A;
			}
			else
			{
				Ln_Point	= Ln_B;
			}
		}

		return( Distance );
	}

	return( -1.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define A	Triangle[0]
#define B	Triangle[1]
#define C	Triangle[2]

//---------------------------------------------------------
bool		GEO_Get_Triangle_CircumCircle(TGEO_Point Triangle[3], TGEO_Point &Point, double &Radius)
{
	TGEO_Point	AB, AC, AB_M, AC_M, AB_N, AC_N;

	AB.x	= B.x - A.x;
	AB.y	= B.y - A.y;
	AB_M.x	= A.x + AB.x / 2.0;
	AB_M.y	= A.y + AB.y / 2.0;
	AB_N.x	= AB_M.x - AB.y;
	AB_N.y	= AB_M.y + AB.x;

	AC.x	= C.x - A.x;
	AC.y	= C.y - A.y;
	AC_M.x	= A.x + AC.x / 2.0;
	AC_M.y	= A.y + AC.y / 2.0;
	AC_N.x	= AC_M.x - AC.y;
	AC_N.y	= AC_M.y + AC.x;

	if( GEO_Get_Crossing(Point, AB_M, AB_N, AC_M, AC_N, false) )
	{
		AB.x	= A.x - Point.x;
		AB.y	= A.y - Point.y;

		Radius	= sqrt(AB.x*AB.x + AB.y*AB.y);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
#undef A
#undef B
#undef C


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double		GEO_Get_Polygon_Area(TGEO_Point *Points, int nPoints)
{
	int			iPoint;
	double		Area;
	TGEO_Point	*jP, *iP;

	Area	= 0.0;

	if( nPoints >= 3 )
	{
		jP		= Points + nPoints - 1;

		for(iPoint=0; iPoint<nPoints; iPoint++)
		{
			iP		= jP;
			jP		= Points + iPoint;

			Area	+= (iP->x * jP->y) - (jP->x * iP->y);
		}

		Area	/= 2.0;
	}

	return( Area );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
