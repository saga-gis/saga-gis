
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
	m_xPoints	= NULL;
	m_yPoints	= NULL;
	m_zPoints	= NULL;

	m_nPoints	= 0;
	m_nBuffer	= 0;

	m_bSplined	= false;
}

//---------------------------------------------------------
CSG_Spline::~CSG_Spline(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Spline::Destroy(void)
{
	if( m_nBuffer > 0 )
	{
		SG_Free(m_xPoints);
		SG_Free(m_yPoints);
		SG_Free(m_zPoints);

		m_xPoints	= NULL;
		m_yPoints	= NULL;
		m_zPoints	= NULL;

		m_nPoints	= 0;
		m_nBuffer	= 0;

		m_bSplined	= false;
	}
}

//---------------------------------------------------------
void CSG_Spline::Add_Value(double x, double y)
{
	int		i, iAdd;

	m_bSplined	= false;

	if( m_nPoints >= m_nBuffer )
	{
		m_nBuffer	+= 64;
		m_xPoints	 = (double *)SG_Realloc(m_xPoints, m_nPoints * sizeof(double));
		m_yPoints	 = (double *)SG_Realloc(m_yPoints, m_nPoints * sizeof(double));
		m_zPoints	 = (double *)SG_Realloc(m_zPoints, m_nPoints * sizeof(double));
	}

	m_nPoints++;

	//-----------------------------------------------------
	if( m_nPoints == 1 )
	{
		m_xPoints[0]	= x;
		m_yPoints[0]	= y;
	}
	else
	{
		for(iAdd=0; iAdd<m_nPoints-1 && m_xPoints[iAdd]<x; iAdd++)	{}

		for(i=m_nPoints-1; i>iAdd; i--)
		{
			m_xPoints[i]	= m_xPoints[i - 1];
			m_yPoints[i]	= m_yPoints[i - 1];
		}

		m_xPoints[iAdd]	= x;
		m_yPoints[iAdd]	= y;
	}
}

//---------------------------------------------------------
void CSG_Spline::Set_Values(double *x, double *y, int n)
{
	int		i;

	Destroy();

	for(i=0; i<n; i++)
	{
		Add_Value(x[i], y[i]);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Spline::Initialize(double y_A, double y_B)
{
	int		i, k;
	double	p, qn, sig, un, *u;

	if( m_nPoints > 2 )
	{
		m_bSplined	= true;
		u			= (double *)SG_Malloc(m_nPoints * sizeof(double));

		if( y_A > 0.99e30 )
		{
			m_zPoints[0]	= u[0]	= 0.0;
		}
		else
		{
			m_zPoints[0]	= -0.5;
			u[0]			= (3.0 / (m_xPoints[1] - m_xPoints[0]))
							* ((m_yPoints[1] - m_yPoints[0]) / (m_xPoints[1] - m_xPoints[0]) - y_A);
		}

		for(i=1; i<m_nPoints-1; i++)
		{
			sig				= (m_xPoints[i] - m_xPoints[i - 1]) / (m_xPoints[i + 1] - m_xPoints[i - 1]);
			p				= sig * m_zPoints[i - 1] + 2.0;
			m_zPoints[i]	= (sig - 1.0) / p;
			u[i]			= (m_yPoints[i + 1] - m_yPoints[i]) / (m_xPoints[i + 1] - m_xPoints[i])
							- (m_yPoints[i] - m_yPoints[i - 1]) / (m_xPoints[i] - m_xPoints[i - 1]);
			u[i]			= (6.0 * u[i] / (m_xPoints[i + 1] - m_xPoints[i - 1]) - sig * u[i - 1]) / p;
		}

		if( y_B > 0.99e30 )
		{
			qn	= un	= 0.0;
		}
		else
		{
			qn				= 0.5;
			un				= (3.0 / (m_xPoints[m_nPoints - 1] - m_xPoints[m_nPoints - 2]))
							* (y_B - (m_yPoints[m_nPoints - 1] - m_yPoints[m_nPoints - 2]) / (m_xPoints[m_nPoints - 1] - m_xPoints[m_nPoints - 2]));
		}

		m_zPoints[m_nPoints - 1]	= (un - qn * u[m_nPoints - 2]) / (qn * m_zPoints[m_nPoints - 2] + 1.0);

		for(k=m_nPoints-2; k>=0; k--)
		{
			m_zPoints[k]	= m_zPoints[k] * m_zPoints[k + 1] + u[k];
		}

		SG_Free(u);
	}
}

//---------------------------------------------------------
bool CSG_Spline::Get_Value(double x, double &y)
{
	int		klo, khi, k;
	double	h, b, a;

	if( m_nPoints > 2 )
	{
		if( !m_bSplined )
		{
			Initialize();
		}

		klo	= 0;
		khi	= m_nPoints - 1;

		while( khi - klo > 1 )
		{
			k	= (khi+klo) >> 1;

			if( m_xPoints[k] > x )
			{
				khi	= k;
			}
			else
			{
				klo	= k;
			}
		}

		h	= m_xPoints[khi] - m_xPoints[klo];

		if( h != 0.0 )
		{
			a	= (m_xPoints[khi] - x) / h;
			b	= (x - m_xPoints[klo]) / h;

			y	= a * m_yPoints[klo] + b * m_yPoints[khi]
				+ ((a*a*a - a) * m_zPoints[klo] + (b*b*b - b) * m_zPoints[khi]) * (h*h) / 6.0;

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
