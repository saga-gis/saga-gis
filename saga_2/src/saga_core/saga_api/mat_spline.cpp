
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
//                    mat_spline.cpp                     //
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
#include "mat_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Spline::CSG_Spline(void)
{
	m_Values	= NULL;
	m_nValues	= 0;
	m_nBuffer	= 0;
	m_bCreated	= false;
}

//---------------------------------------------------------
CSG_Spline::~CSG_Spline(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Spline::Destroy(void)
{
	if( m_Values )
	{
		SG_Free(m_Values);

		m_Values	= NULL;
		m_nValues	= 0;
		m_nBuffer	= 0;
		m_bCreated	= false;
	}
}

//---------------------------------------------------------
bool CSG_Spline::Create(double *xValues, double *yValues, int nValues, double yA, double yB)
{
	Destroy();

	for(int i=0; i<nValues; i++)
	{
		Add(xValues[i], yValues[i]);
	}

	return( _Create(yA, yB) );
}

//---------------------------------------------------------
bool CSG_Spline::Create(double yA, double yB)
{
	return( _Create(yA, yB) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Spline::Add(double x, double y)
{
	m_bCreated	= false;

	//-----------------------------------------------------
	if( m_nValues >= m_nBuffer )
	{
		m_nBuffer	+= 64;
		m_Values	 = (TSG_Point_Z *)SG_Realloc(m_Values, m_nValues * sizeof(TSG_Point_Z));
	}

	m_nValues++;

	//-----------------------------------------------------
	if( m_nValues == 1 )
	{
		m_Values[0].x	= x;
		m_Values[0].y	= y;
	}
	else
	{
		int		i, iAdd;

		for(iAdd=0; iAdd<m_nValues-1 && m_Values[iAdd].x<x; iAdd++)	{}

		for(i=m_nValues-1; i>iAdd; i--)
		{
			m_Values[i]	= m_Values[i - 1];
		}

		m_Values[iAdd].x	= x;
		m_Values[iAdd].y	= y;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Spline::_Create(double yA, double yB)
{
	int		i, k;
	double	p, qn, sig, un, *u;

	if( m_nValues > 2 )
	{
		m_bCreated	= true;
		u			= (double *)SG_Malloc(m_nValues * sizeof(double));

		if( yA > 0.99e30 )
		{
			m_Values[0].z	= u[0]	= 0.0;
		}
		else
		{
			m_Values[0].z	= -0.5;
			u[0]			= (3.0 / (m_Values[1].x - m_Values[0].x))
							* ((m_Values[1].y - m_Values[0].y) / (m_Values[1].x - m_Values[0].x) - yA);
		}

		for(i=1; i<m_nValues-1; i++)
		{
			sig				= (m_Values[i].x - m_Values[i - 1].x) / (m_Values[i + 1].x - m_Values[i - 1].x);
			p				= sig * m_Values[i - 1].z + 2.0;
			m_Values[i].z	= (sig - 1.0) / p;
			u[i]			= (m_Values[i + 1].y - m_Values[i    ].y) / (m_Values[i + 1].x - m_Values[i    ].x)
							- (m_Values[i    ].y - m_Values[i - 1].y) / (m_Values[i    ].x - m_Values[i - 1].x);
			u[i]			= (6.0 * u[i] / (m_Values[i + 1].x - m_Values[i - 1].x) - sig * u[i - 1]) / p;
		}

		if( yB > 0.99e30 )
		{
			qn	= un	= 0.0;
		}
		else
		{
			qn				= 0.5;
			un				= (3.0 / (m_Values[m_nValues - 1].x - m_Values[m_nValues - 2].x))
							* (yB  - (m_Values[m_nValues - 1].y - m_Values[m_nValues - 2].y)
							       / (m_Values[m_nValues - 1].x - m_Values[m_nValues - 2].x));
		}

		m_Values[m_nValues - 1].z	= (un - qn * u[m_nValues - 2]) / (qn * m_Values[m_nValues - 2].z + 1.0);

		for(k=m_nValues-2; k>=0; k--)
		{
			m_Values[k].z	= m_Values[k].z * m_Values[k + 1].z + u[k];
		}

		SG_Free(u);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Spline::Get_Value(double x, double &y)
{
	if( m_bCreated || Create() )
	{
		int		klo, khi, k;
		double	h, b, a;

		klo	= 0;
		khi	= m_nValues - 1;

		while( khi - klo > 1 )
		{
			k	= (khi+klo) >> 1;

			if( m_Values[k].x > x )
			{
				khi	= k;
			}
			else
			{
				klo	= k;
			}
		}

		h	= m_Values[khi].x - m_Values[klo].x;

		if( h != 0.0 )
		{
			a	= (m_Values[khi].x - x) / h;
			b	= (x - m_Values[klo].x) / h;

			y	= a * m_Values[klo].y + b * m_Values[khi].y
				+ ((a*a*a - a) * m_Values[klo].z + (b*b*b - b) * m_Values[khi].z) * (h*h) / 6.0;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
double CSG_Spline::Get_Value(double x)
{
	Get_Value(x, x);

	return( x );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
CSG_Thin_Plate_Spline::CSG_Thin_Plate_Spline(void)
{
}

//---------------------------------------------------------
CSG_Thin_Plate_Spline::~CSG_Thin_Plate_Spline(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Thin_Plate_Spline::Destroy(void)
{
	m_Points.Clear();
	m_V.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Thin_Plate_Spline::_Get_hDistance(TSG_Point_Z A, TSG_Point_Z B)
{
	A.x	-= B.x;
	A.y	-= B.y;

	return( sqrt(A.x*A.x + A.y*A.y) );
}

//---------------------------------------------------------
double CSG_Thin_Plate_Spline::_Get_Base_Funtion(double x)
{
	return( x > 0.0 ? x*x * log(x) : 0.0 );
}

//---------------------------------------------------------
double CSG_Thin_Plate_Spline::_Get_Base_Funtion(TSG_Point_Z A, double x, double y)
{
	x	-= A.x;
	y	-= A.y;
	x	= sqrt(x*x + y*y);

	return( x > 0.0 ? x*x * log(x) : 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//	Calculate Thin Plate Spline (TPS) weights from control points.
//
bool CSG_Thin_Plate_Spline::Create(double Regularization, bool bSilent)
{
	bool		bResult	= false;
	int			n;

	//-----------------------------------------------------
	// You We need at least 3 points to define a plane
	if( (n = m_Points.Get_Count()) >= 3 )
	{
		int				i, j;
		double			a, b;
		TSG_Point_Z	Point;
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
		{
			SG_UI_Process_Set_Text(LNG("Thin Plate Spline: solving matrix"));
		}

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
double CSG_Thin_Plate_Spline::Get_Value(double x, double y)
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
