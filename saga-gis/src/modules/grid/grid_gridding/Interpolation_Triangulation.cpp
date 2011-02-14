/**********************************************************
 * Version $Id$
 *********************************************************/

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

	Set_Author		(SG_T("O.Conrad (c) 2004"));

	Set_Description	(_TW(
		"Gridding of a shapes layer using Delaunay Triangulation."
	));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_Triangulation::Interpolate(void)
{
	CSG_TIN	TIN;

	if( TIN.Create(Get_Points()) )
	{
		m_pGrid->Assign_NoData();

		for(int iTriangle=0; iTriangle<TIN.Get_Triangle_Count() && Set_Progress(iTriangle, TIN.Get_Triangle_Count()); iTriangle++)
		{
			CSG_TIN_Triangle	*pTriangle	= TIN.Get_Triangle(iTriangle);

			if( m_pGrid->Get_Extent().Intersects(pTriangle->Get_Extent()) != INTERSECTION_None )
			{
				TSG_Point_Z	p[3];

				for(int iPoint=0; iPoint<3; iPoint++)
				{
					p[iPoint].x	= (pTriangle->Get_Node(iPoint)->Get_X() - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize();
					p[iPoint].y	= (pTriangle->Get_Node(iPoint)->Get_Y() - m_pGrid->Get_YMin()) / m_pGrid->Get_Cellsize();
					p[iPoint].z	=  pTriangle->Get_Node(iPoint)->asDouble(m_zField);
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
void CInterpolation_Triangulation::Set_Triangle(TSG_Point_Z p[3])
{
	//-----------------------------------------------------
	if( p[1].y < p[0].y ) {	TSG_Point_Z pp = p[1]; p[1] = p[0]; p[0] = pp;	}
	if( p[2].y < p[0].y ) {	TSG_Point_Z pp = p[2]; p[2] = p[0]; p[0] = pp;	}
	if( p[2].y < p[1].y ) {	TSG_Point_Z pp = p[2]; p[2] = p[1]; p[1] = pp;	}

	//-----------------------------------------------------
	TSG_Rect	r;

	r.yMin	= p[0].y;
	r.yMax	= p[2].y;
	r.xMin	= p[0].x < p[1].x ? (p[0].x < p[2].x ? p[0].x : p[2].x) : (p[1].x < p[2].x ? p[1].x : p[2].x);
	r.xMax	= p[0].x > p[1].x ? (p[0].x > p[2].x ? p[0].x : p[2].x) : (p[1].x > p[2].x ? p[1].x : p[2].x);

	if( r.yMin >= r.yMax || r.xMin >= r.xMax )
	{
		return;	// no area
	}

	if( (r.yMin < 0.0 && r.yMax < 0.0) || (r.yMin >= m_pGrid->Get_NY() && r.yMax >= m_pGrid->Get_NY())
	||	(r.xMin < 0.0 && r.xMax < 0.0) || (r.xMin >= m_pGrid->Get_NX() && r.xMax >= m_pGrid->Get_NX()) )
	{
		return;	// completely outside grid
	}

	//-----------------------------------------------------
	TSG_Point_Z	d[3];

	if( (d[0].y	= p[2].y - p[0].y) != 0.0 )
	{
		d[0].x	= (p[2].x - p[0].x) / d[0].y;
		d[0].z	= (p[2].z - p[0].z) / d[0].y;
	}

	if( (d[1].y	= p[1].y - p[0].y) != 0.0 )
	{
		d[1].x	= (p[1].x - p[0].x) / d[1].y;
		d[1].z	= (p[1].z - p[0].z) / d[1].y;
	}

	if( (d[2].y	= p[2].y - p[1].y) != 0.0 )
	{
		d[2].x	= (p[2].x - p[1].x) / d[2].y;
		d[2].z	= (p[2].z - p[1].z) / d[2].y;
	}

	//-----------------------------------------------------
	int	ay	= (int)r.yMin;	if( ay < 0 )	ay	= 0;	if( ay < r.yMin )	ay++;
	int	by	= (int)r.yMax;	if( by >= m_pGrid->Get_NY() )	by	= m_pGrid->Get_NY() - 1;

	for(int y=ay; y<=by; y++)
	{
		if( y <= p[1].y && d[1].y > 0.0 )
		{
			Set_Triangle_Line(y,
				p[0].x + (y - p[0].y) * d[0].x,
				p[0].z + (y - p[0].y) * d[0].z,
				p[0].x + (y - p[0].y) * d[1].x,
				p[0].z + (y - p[0].y) * d[1].z
			);
		}
		else if( d[2].y > 0.0 )
		{
			Set_Triangle_Line(y,
				p[0].x + (y - p[0].y) * d[0].x,
				p[0].z + (y - p[0].y) * d[0].z,
				p[1].x + (y - p[1].y) * d[2].x,
				p[1].z + (y - p[1].y) * d[2].z
			);
		}
	}
}

//---------------------------------------------------------
inline void CInterpolation_Triangulation::Set_Triangle_Line(int y, double xa, double za, double xb, double zb)
{
	if( xb < xa )
	{
		double	d;

		d	= xa;	xa	= xb;	xb	= d;
		d	= za;	za	= zb;	zb	= d;
	}

	if( xb > xa )
	{
		double	dz	= (zb - za) / (xb - xa);
		int		ax	= (int)xa;	if( ax < 0 )	ax	= 0;	if( ax < xa )	ax++;
		int		bx	= (int)xb;	if( bx >= m_pGrid->Get_NX() )	bx	= m_pGrid->Get_NX() - 1;

		for(int x=ax; x<=bx; x++)
		{
			double	z	= za + dz * (x - xa);

			if( m_pGrid->is_NoData(x, y) || m_pGrid->asDouble(x, y) < z )
			{
				m_pGrid->Set_Value(x, y, z);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
