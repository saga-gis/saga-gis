
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     grid_spline                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Thin_Plate_Spline.cpp                 //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//
// Based on:
//
// Thin Plate Spline demo/example in C++
// Copyright (C) 2003, 2005 by Jarno Elonen
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation. The authors make no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
//
// Reference:
// - Donato, G., Belongie, S. (2002):
//   'Approximation Methods for Thin Plate Spline Mappings and Principal Warps'
//
//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Thin_Plate_Spline.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CThin_Plate_Spline::CThin_Plate_Spline(void)
{
}

//---------------------------------------------------------
CThin_Plate_Spline::~CThin_Plate_Spline(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CThin_Plate_Spline::Destroy(void)
{
	m_Points.Clear();
	m_V.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CThin_Plate_Spline::Add_Point(double x, double y, double z)
{
	m_Points.Add(x, y, z);
}

//---------------------------------------------------------
void CThin_Plate_Spline::Add_Point(const TSG_Point &p, double z)
{
	m_Points.Add(p.x, p.y, z);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CThin_Plate_Spline::_Get_hDistance(TSG_Point_3D A, TSG_Point_3D B)
{
	A.x	-= B.x;
	A.y	-= B.y;

	return( sqrt(A.x*A.x + A.y*A.y) );
}

//---------------------------------------------------------
double CThin_Plate_Spline::_Get_Base_Funtion(double x)
{
	return( x > 0.0 ? x*x * log(x) : 0.0 );
}

//---------------------------------------------------------
double CThin_Plate_Spline::_Get_Base_Funtion(TSG_Point_3D A, double x, double y)
{
	x	-= A.x;
	y	-= A.y;
	x	= sqrt(x*x + y*y);

	return( x > 0.0 ? x*x * log(x) : 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//	Calculate Thin Plate Spline (TPS) weights from control points.
//
bool CThin_Plate_Spline::Create(double Regularization, bool bSilent)
{
	bool		bResult	= false;
	int			n;

	//-----------------------------------------------------
	// You We need at least 3 points to define a plane
	if( (n = m_Points.Get_Count()) >= 3 )
	{
		int				i, j;
		double			a, b;
		TSG_Point_3D	Point;
		CSG_Matrix		M;

		//-------------------------------------------------
		// Allocate the matrix and vector
		M	.Create(n + 3, n + 3);
		m_V	.Create(n + 3);

		//-------------------------------------------------
		// Fill K (n x n, upper left of L) and calculate
		// mean edge length from control points
		//
		// K is symmetrical so we really have to
		// calculate only about half of the coefficients.
		for(i=0, a=0.0; i<n && (bSilent || SG_UI_Process_Set_Progress(i, n)); ++i )
		{
			Point	= m_Points[i];

			for(j=i+1; j<n; ++j)
			{
				b		 = _Get_hDistance(Point, m_Points[j]);
				a		+= b * 2.0;	// same for upper & lower tri
				M[i][j]	 = (M[j][i]	= _Get_Base_Funtion(b));
			}
		}

		a	/= (double)(n*n);

		//-------------------------------------------------
		// Fill the rest of L
		for(i=0; i<n; ++i)
		{
			// diagonal: reqularization parameters (lambda * a^2)
			M[i][i]		= Regularization * (a*a);

			// P (n x 3, upper right)
			M[i][n + 0]	= 1.0;
			M[i][n + 1]	= m_Points[i].x;
			M[i][n + 2]	= m_Points[i].y;

			// P transposed (3 x n, bottom left)
			M[n + 0][i]	= 1.0;
			M[n + 1][i]	= m_Points[i].x;
			M[n + 2][i]	= m_Points[i].y;
		}

		//-------------------------------------------------
		// O (3 x 3, lower right)
		for(i=n; i<n+3; ++i)
		{
			for(j=n; j<n+3; ++j)
			{
				M[i][j]	= 0.0;
			}
		}

		//-------------------------------------------------
		// Fill the right hand vector m_V
		for(i=0; i<n; ++i)
		{
			m_V[i]	= m_Points[i].z;
		}

		m_V[n + 0]	= m_V[n + 1]	= m_V[n + 2]	= 0.0;

		//-------------------------------------------------
		// Solve the linear system "inplace"
		if( !bSilent )
			SG_UI_Process_Set_Text(_TL("Solving Matrix"));

		bResult		= SG_Matrix_Solve(M, m_V, bSilent);
	}

	//-----------------------------------------------------
	if( !bResult )
	{
		Destroy();
	}

	return( bResult );
}

//---------------------------------------------------------
double CThin_Plate_Spline::Get_Value(double x, double y)
{
	if( m_V.Get_N() > 0 )
	{
		int		n	= m_Points.Get_Count();
		double	z	= m_V[n + 0] + m_V[n + 1] * x + m_V[n + 2] * y;

		for(int i=0; i<n; i++)
		{
			z	+= m_V[i] * _Get_Base_Funtion(m_Points[i], x, y);
		}

		return( z );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
