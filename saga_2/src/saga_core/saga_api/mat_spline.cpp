
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
//														 //
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
//														 //
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
//														 //
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
//														 //
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
