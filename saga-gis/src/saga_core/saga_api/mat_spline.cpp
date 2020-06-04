
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
#include "mat_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Spline::CSG_Spline(void)
{
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
	m_x.Destroy();
	m_y.Destroy();
	m_z.Destroy();

	m_bCreated	= false;
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

	m_x.Add_Row(x);
	m_y.Add_Row(y);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Spline::_Create(double yA, double yB)
{
	if( Get_Count() < 3 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			i, k, n	= Get_Count();
	double		p, qn, sig, un;
	CSG_Vector	u;

	//-----------------------------------------------------
	CSG_Index	Index(n, m_x.Get_Data());
	CSG_Vector	x(m_x), y(m_y);

	for(i=0; i<n; i++)
	{
		m_x[i]	= x[Index[i]];
		m_y[i]	= y[Index[i]];
	}

	//-----------------------------------------------------
	u  .Create(n);
	m_z.Create(n);

	if( yA > 0.99e30 )
	{
		m_z[0]	= u[0] = 0.;
	}
	else
	{
		m_z[0]	= -0.5;
		u  [0]	= (3. / (m_x[1] - m_x[0])) * ((m_y[1] - m_y[0]) / (m_x[1] - m_x[0]) - yA);
	}

	//-----------------------------------------------------
	for(i=1; i<n-1; i++)
	{
		sig		= (m_x[i] - m_x[i - 1]) / (m_x[i + 1] - m_x[i - 1]);
		p		= sig * m_z[i - 1] + 2.;
		m_z[i]	= (sig - 1.) / p;
		u  [i]	= (m_y[i + 1] - m_y[i    ]) / (m_x[i + 1] - m_x[i    ])
				- (m_y[i    ] - m_y[i - 1]) / (m_x[i    ] - m_x[i - 1]);
		u  [i]	= (6. * u[i] / (m_x[i + 1] - m_x[i - 1]) - sig * u[i - 1]) / p;
	}

	if( yB > 0.99e30 )
	{
		qn = un	= 0.;
	}
	else
	{
		qn		= 0.5;
		un		= (3. / (m_x[n - 1] - m_x[n - 2]))
				* (yB - (m_y[n - 1] - m_y[n - 2])
				      / (m_x[n - 1] - m_x[n - 2]));
	}

	m_z[n - 1]	= (un - qn * u[n - 2]) / (qn * m_z[n - 2] + 1.);

	for(k=n-2; k>=0; k--)
	{
		m_z[k]	= m_z[k] * m_z[k + 1] + u[k];
	}

	//-----------------------------------------------------
	m_bCreated	= true;

	return( true );
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
		khi	= Get_Count() - 1;

		while( khi - klo > 1 )
		{
			k	= (khi+klo) >> 1;

			if( m_x[k] > x )
			{
				khi	= k;
			}
			else
			{
				klo	= k;
			}
		}

		h	= m_x[khi] - m_x[klo];

		if( h != 0. )
		{
			a	= (m_x[khi] - x) / h;
			b	= (x - m_x[klo]) / h;

			y	= a * m_y[klo] + b * m_y[khi]
				+ ((a*a*a - a) * m_z[klo] + (b*b*b - b) * m_z[khi]) * (h*h) / 6.;

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
	return( x > 0. ? x*x * log(x) : 0. );
}

//---------------------------------------------------------
double CSG_Thin_Plate_Spline::_Get_Base_Funtion(TSG_Point_Z A, double x, double y)
{
	x	-= A.x;
	y	-= A.y;
	
	double	d	= sqrt(x*x + y*y);

	return( d > 0. ? d*d * log(d) : 0. );
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
	CSG_Matrix	M;

	//-----------------------------------------------------
	// We need at least 3 points to define a plane
	if( (n = m_Points.Get_Count()) >= 3 && M.Create(n + 3, n + 3) && m_V.Create(n + 3) )
	{
		int			i, j;
		double		a, b;
		TSG_Point_Z	Point;

		//-------------------------------------------------
		// Fill K (n x n, upper left of L) and calculate
		// mean edge length from control points
		//
		// K is symmetrical so we really have to
		// calculate only about half of the coefficients.
		for(i=0, a=0.; i<n && (bSilent || SG_UI_Process_Set_Progress(i, n)); ++i )
		{
			Point	= m_Points[i];

			for(j=i+1; j<n; ++j)
			{
				b		 = _Get_hDistance(Point, m_Points[j]);
				a		+= b * 2.;	// same for upper & lower tri
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
			M[i][n + 0]	= 1.;
			M[i][n + 1]	= m_Points[i].x;
			M[i][n + 2]	= m_Points[i].y;

			// P transposed (3 x n, bottom left)
			M[n + 0][i]	= 1.;
			M[n + 1][i]	= m_Points[i].x;
			M[n + 2][i]	= m_Points[i].y;
		}

		//-------------------------------------------------
		// O (3 x 3, lower right)
		for(i=n; i<n+3; ++i)
		{
			for(j=n; j<n+3; ++j)
			{
				M[i][j]	= 0.;
			}
		}

		//-------------------------------------------------
		// Fill the right hand vector m_V
		for(i=0; i<n; ++i)
		{
			m_V[i]	= m_Points[i].z;
		}

		m_V[n + 0] = m_V[n + 1] = m_V[n + 2] = 0.;

		//-------------------------------------------------
		// Solve the linear system "inplace"
		if( !bSilent )
		{
			SG_UI_Process_Set_Text(_TL("Thin Plate Spline: solving matrix"));
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

	return( 0. );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
