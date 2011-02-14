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
double		SG_Get_Length(double dx, double dy)
{
	return( sqrt(dx*dx + dy*dy) );
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double		SG_Get_Angle_Of_Direction(double dx, double dy)
{
	if( dx == 0.0 )
	{
		return( dy > 0.0 ? 0.0 : M_PI_180 );
	}

	dx	= M_PI_090 - atan2(dy, dx);

	return( dx < 0.0 ? M_PI_360 + dx : dx );
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	SG_Get_Crossing(TSG_Point &Crossing, const TSG_Point &a1, const TSG_Point &a2, const TSG_Point &b1, const TSG_Point &b2, bool bExactMatch)
{
	double	lambda, div, a_dx, a_dy, b_dx, b_dy;

	if( bExactMatch
	&&	(	(M_GET_MAX(a1.x, a2.x) < M_GET_MIN(b1.x, b2.x))
		||	(M_GET_MIN(a1.x, a2.x) > M_GET_MAX(b1.x, b2.x))
		||	(M_GET_MAX(a1.y, a2.y) < M_GET_MIN(b1.y, b2.y))
		||	(M_GET_MIN(a1.y, a2.y) > M_GET_MAX(b1.y, b2.y))	) )
	{
		return( false );
	}

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
bool	SG_Get_Crossing_InRegion(TSG_Point &Crossing, const TSG_Point &_a, const TSG_Point &_b, const TSG_Rect &Region)
{
	bool		bResult;
	TSG_Point	a, b, pExt_XY, pExt_X, pExt_Y;
	CSG_Rect	r(Region);

	a	= _a;
	b	= _b;

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
		if( !SG_Get_Crossing(Crossing, a, b, pExt_X, pExt_XY) )
		{
			if( !SG_Get_Crossing(Crossing, a, b, pExt_Y, pExt_XY) )
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
double		SG_Get_Nearest_Point_On_Line(const TSG_Point &Point, const TSG_Point &Ln_A, const TSG_Point &Ln_B, TSG_Point &Ln_Point, bool bExactMatch)
{
	double		dx, dy, Distance, d;
	TSG_Point	Point_B;

	Point_B.x	= Point.x - (Ln_B.y - Ln_A.y);
	Point_B.y	= Point.y + (Ln_B.x - Ln_A.x);

	if( SG_Get_Crossing(Ln_Point, Ln_A, Ln_B, Point, Point_B, false) )
	{
		if( !bExactMatch || (bExactMatch && SG_IS_BETWEEN(Ln_A.x, Ln_Point.x, Ln_B.x) && SG_IS_BETWEEN(Ln_A.y, Ln_Point.y, Ln_B.y)) )
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
bool		SG_Get_Triangle_CircumCircle(TSG_Point Triangle[3], TSG_Point &Point, double &Radius)
{
	TSG_Point	AB, AC, AB_M, AC_M, AB_N, AC_N;

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

	if( SG_Get_Crossing(Point, AB_M, AB_N, AC_M, AC_N, false) )
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
double		SG_Get_Polygon_Area(TSG_Point *Points, int nPoints)
{
	double	Area	= 0.0;

	if( nPoints >= 3 )
	{
		int			i;
		TSG_Point	*jP, *iP;

		for(i=0, iP=Points, jP=Points+nPoints-1; i<nPoints; i++, jP=iP++)
		{
			Area	+= (jP->x * iP->y) - (iP->x * jP->y);
		}

		Area	/= 2.0;
	}

	return( Area );
}

//---------------------------------------------------------
double		SG_Get_Polygon_Area(const CSG_Points &Points)
{
	double	Area	= 0.0;

	if( Points.Get_Count() >= 3 )
	{
		for(int i=0, j=Points.Get_Count()-1; i<Points.Get_Count(); j=i++)
		{
			Area	+= (Points.Get_X(j) * Points.Get_Y(i))
					 - (Points.Get_X(i) * Points.Get_Y(j));
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
