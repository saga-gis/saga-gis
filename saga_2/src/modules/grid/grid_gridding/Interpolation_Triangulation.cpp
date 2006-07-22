
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Triangulation.cpp                   //
//                                                       //
//                 Copyright (C) 2004 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Interpolation_Triangulation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation_Triangulation::CInterpolation_Triangulation(void)
{
	Set_Name		(_TL("Triangulation"));

	Set_Author		(_TL("Copyrights (c) 2004 by Olaf Conrad"));

	Set_Description	(_TL(
		"Gridding of a shapes layer using Delaunay Triangulation."
	));
}

//---------------------------------------------------------
CInterpolation_Triangulation::~CInterpolation_Triangulation(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_Triangulation::Interpolate(void)
{
	int				iTriangle, iPoint;
	TPoint			p[3];
	CTIN_Triangle	*pTriangle;
	CTIN			TIN;

	if( TIN.Create(m_pShapes) )
	{
		m_pGrid->Assign_NoData();

		for(iTriangle=0; iTriangle<TIN.Get_Triangle_Count() && Set_Progress(iTriangle, TIN.Get_Triangle_Count()); iTriangle++)
		{
			pTriangle	= TIN.Get_Triangle(iTriangle);

			if( m_pGrid->Get_Extent().Intersects(pTriangle->Get_Extent()) != INTERSECTION_None )
			{
				for(iPoint=0; iPoint<3; iPoint++)
				{
					p[iPoint].x	= m_pGrid->Get_System().Get_xWorld_to_Grid(pTriangle->Get_Point(iPoint)->Get_X());
					p[iPoint].y	= m_pGrid->Get_System().Get_yWorld_to_Grid(pTriangle->Get_Point(iPoint)->Get_Y());
					p[iPoint].z	= pTriangle->Get_Point(iPoint)->Get_Record()->asDouble(m_zField);
				}

				Set_Triangle(p);
			}
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
#define SORT_POINTS_Y(a, b)	if( p[a].y < p[b].y ) {	pp = p[a]; p[a] = p[b]; p[b] = pp;	}
#define SORT_POINTS_X(a, b)	if( p[a].x < p[b].x ) {	pp = p[a]; p[a] = p[b]; p[b] = pp;	}

//---------------------------------------------------------
void CInterpolation_Triangulation::Set_Triangle(TPoint p[3])
{
	int		i, j, y, y_j;
	double	x, x_a, dx, dx_a, dy, z, z_a, dz, dz_a;
	TPoint	pp;

	//-----------------------------------------------------
	SORT_POINTS_Y(1, 0);
	SORT_POINTS_Y(2, 0);
	SORT_POINTS_Y(2, 1);

	//-----------------------------------------------------
	if( p[2].y == p[0].y )
	{
		if( p[0].y >= 0 && p[0].y < m_pGrid->Get_NY() )
		{
			SORT_POINTS_X(1, 0);
			SORT_POINTS_X(2, 0);
			SORT_POINTS_X(2, 1);

			//---------------------------------------------
			if( p[2].x == p[0].x )
			{
				if(	p[0].x >= 0 && p[0].x < m_pGrid->Get_NX() )
				{
					m_pGrid->Set_Value(p[0].x, p[0].y, p[0].z > p[1].z
						? (p[0].z > p[2].z ? p[0].z : p[2].z)
						: (p[1].z > p[2].z ? p[1].z : p[2].z)
					);
				}
			}

			//---------------------------------------------
			else
			{
				Set_Triangle_Line(p[0].x, p[1].x, p[0].y, p[0].z, p[1].z);
				Set_Triangle_Line(p[1].x, p[2].x, p[0].y, p[1].z, p[2].z);
			}
		}
	}

	//-----------------------------------------------------
	else if( !((p[0].y < 0 && p[2].y < 0) || (p[0].y >= m_pGrid->Get_NY() && p[2].y >= m_pGrid->Get_NY())) )
	{
		dy		=  p[2].y - p[0].y;
		dx_a	= (p[2].x - p[0].x) / dy;
		dz_a	= (p[2].z - p[0].z) / dy;
		x_a		=  p[0].x;
		z_a		=  p[0].z;

		for(i=0, j=1; i<2; i++, j++)
		{
			if( (dy	=  p[j].y - p[i].y) > 0.0 )
			{
				dx		= (p[j].x - p[i].x) / dy;
				dz		= (p[j].z - p[i].z) / dy;
				x		=  p[i].x;
				z		=  p[i].z;

				if( (y = p[i].y) < 0 )
				{
					x		-= y * dx;
					z		-= y * dz;
					y		 = 0;
					x_a		 = p[0].x - p[0].y * dx_a;
					z_a		 = p[0].z - p[0].y * dz_a;
				}

				if( (y_j = p[j].y) > m_pGrid->Get_NY() )
				{
					y_j		= m_pGrid->Get_NY();
				}

				for( ; y<y_j; y++, x+=dx, z+=dz, x_a+=dx_a, z_a+=dz_a)
				{
					if( x < x_a )
					{
						Set_Triangle_Line((int)x, (int)x_a, y, z, z_a);
					}
					else
					{
						Set_Triangle_Line((int)x_a, (int)x, y, z_a, z);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
inline void CInterpolation_Triangulation::Set_Triangle_Line(int xa, int xb, int y, double za, double zb)
{
	double	dz;

	if( (dz = xb - xa) > 0.0 )
	{
		dz	= (zb - za) / dz;

		if( xa < 0 )
		{
			za	-= dz * xa;
			xa	 = 0;
		}

		if( xb >= m_pGrid->Get_NX() )
		{
			xb	= m_pGrid->Get_NX() - 1;
		}

		for(int x=xa; x<=xb; x++, za+=dz)
		{
			m_pGrid->Set_Value(x, y, za);
		}
	}
	else if( xa >= 0 && xa < m_pGrid->Get_NX() )
	{
		m_pGrid->Set_Value(xa, y, za);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
