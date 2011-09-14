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
//                  mat_regression.cpp                   //
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
double SG_Regression_Get_Adjusted_R2(double r2, int n, int p, TSG_Regression_Correction Correction)
{
	double	r	= 1.0 - r2;

	switch( Correction )
	{
	case REGRESSION_CORR_None: default:
		return( r2 );

	case REGRESSION_CORR_Smith:
		r2	= 1.0 - ((n      ) / (n - p      )) * r;
		break;

	case REGRESSION_CORR_Wherry_1:
		r2	= 1.0 - ((n - 1.0) / (n - p - 1.0)) * r;
		break;

	case REGRESSION_CORR_Wherry_2:
		r2	= 1.0 - ((n - 1.0) / (n - p      )) * r;
		break;

	case REGRESSION_CORR_Olkin_Pratt:
	//	r2	= 1.0 - ((n - 3.0) / (n - p - 2.0)) * (r + (2.0 / (n - p)) * r*r);
		r2	= 1.0 - ((n - 3.0) * r / (n - p - 1.0)) * (1.0 + (2.0 * r) / (n - p + 1.0));
		break;

	case REGRESSION_CORR_Pratt:
		r2	= 1.0 - ((n - 3.0) * r / (n - p - 1.0)) * (1.0 + (2.0 * r) / (n - p - 2.3));
		break;

	case REGRESSION_CORR_Claudy_3:
		r2	= 1.0 - ((n - 4.0) * r / (n - p - 1.0)) * (1.0 + (2.0 * r) / (n - p + 1.0));
		break;
	}

	return( r2 < 0.0 ? 0.0 : r2 > 1.0 ? 1.0 : r2 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Regression::CSG_Regression(void)
{
	m_nBuffer	= 0;
	m_nValues	= 0;
	m_x			= NULL;
	m_y			= NULL;

	m_Type		= REGRESSION_Linear;
}

//---------------------------------------------------------
CSG_Regression::~CSG_Regression(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Regression::Destroy(void)
{
	if( m_nBuffer > 0 )
	{
		SG_Free(m_x);
		SG_Free(m_y);

		m_nBuffer	= 0;
	}

	m_nValues	= 0;
	m_x			= NULL;
	m_y			= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Regression::Add_Values(double x, double y)
{
	if( m_nValues >= m_nBuffer )
	{
		m_nBuffer	+= 64;
		m_x	= (double *)SG_Realloc(m_x, m_nBuffer * sizeof(double));
		m_y	= (double *)SG_Realloc(m_y, m_nBuffer * sizeof(double));
	}

	m_x[m_nValues]	= x;
	m_y[m_nValues]	= y;

	m_nValues++;
}

//---------------------------------------------------------
void CSG_Regression::Set_Values(int nValues, double *x, double *y)
{
	Destroy();

	for(int i=0; i<nValues; i++)
	{
		Add_Values(x[i], y[i]);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_Regression::asString(void)
{
	static CSG_String	s;

	s.Printf(
		SG_T("N = %d\n")
		SG_T("  Min. = %.6f  Max. = %.6f\n  Arithmetic Mean = %.6f\n  Variance = %.6f\n  Standard Deviation = %.6f\n")
		SG_T("  Min. = %.6f  Max. = %.6f\n  Arithmetic Mean = %.6f\n  Variance = %.6f\n  Standard Deviation = %.6f\n")
		SG_T("Linear Regression:\n  Y = %.6f * X %+.6f\n  (r=%.4f, r\xc2\xb2=%.4f)"),
		m_nValues,
		m_xMin, m_xMax, m_xMean, m_xVar, sqrt(m_xVar),
		m_yMin, m_yMax, m_yMean, m_yVar, sqrt(m_yVar),
		m_RCoeff, m_RConst, m_R, m_R*m_R
	);

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Regression::Get_x(double y)	const
{
	if( m_nValues > 0.0 )
	{
		switch( m_Type )
		{
		case REGRESSION_Linear:	// Y = a + b * X		-> X = (Y - a) / b
			if( m_RCoeff != 0.0 )
				return( (m_RConst * y) / m_RCoeff );

		case REGRESSION_Rez_X:	// Y = a + b / X		-> X = b / (Y - a)
			if( (y = y - m_RConst) != 0.0 )
				return( m_RCoeff / y );

		case REGRESSION_Rez_Y:	// Y = a / (b - X)		-> X = b - a / Y
			if( y != 0.0 )
				return( m_RCoeff - m_RConst / y );

		case REGRESSION_Pow:	// Y = a * X^b			-> X = (Y / a)^(1 / b)
			if( m_RConst != 0.0 && m_RCoeff != 0.0 )
				return( pow(y / m_RConst, 1.0 / m_RCoeff) );

		case REGRESSION_Exp:	// Y = a * e^(b * X)	-> X = ln(Y / a) / b
			if( m_RConst != 0.0 && (y = y / m_RConst) > 0.0 && m_RCoeff != 0.0 )
			return( log(y) / m_RCoeff );

		case REGRESSION_Log:	// Y = a + b * ln(X)	-> X = e^((Y - a) / b)
			if( m_RCoeff != 0.0 )
				return( exp((y - m_RConst) / m_RCoeff) );
		}
	}

	return( sqrt(-1.0) );
}

//---------------------------------------------------------
double CSG_Regression::Get_y(double x)	const
{
	if( m_nValues > 0.0 )
	{
		switch( m_Type )
		{
		case REGRESSION_Linear:	// Y = a + b * X
			return( m_RConst + m_RCoeff * x );

		case REGRESSION_Rez_X:	// Y = a + b / X
			if( x != 0.0 )
				return( m_RConst + m_RCoeff / x );

		case REGRESSION_Rez_Y:	// Y = a / (b - X)
			if( (x = m_RCoeff - x) != 0.0 )
				return( m_RConst / x );

		case REGRESSION_Pow:	// Y = a * X^b
			return( m_RConst * pow(x, m_RCoeff) );

		case REGRESSION_Exp:	// Y = a e^(b * X)
			return( m_RConst * exp(m_RCoeff * x) );

		case REGRESSION_Log:	// Y = a + b * ln(X)
			if( x > 0.0 )
				return( m_RConst + m_RCoeff * log(x) );
		}
	}

	return( sqrt(-1.0) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression::_Get_MinMeanMax(double &xMin, double &xMean, double &xMax, double &yMin, double &yMean, double &yMax)
{
	int		i;
	double	x, y;

	if( m_nValues > 0 )
	{
		xMin = xMean = xMax = m_x[0];
		yMin = yMean = yMax = m_y[0];

		for(i=1; i<m_nValues; i++)
		{
			xMean	+= (x = m_x[i]);
			yMean	+= (y = m_y[i]);

			M_SET_MINMAX(xMin, xMax, x);
			M_SET_MINMAX(yMin, yMax, y);
		}

		xMean	/= m_nValues;
		yMean	/= m_nValues;

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
inline double CSG_Regression::_Y_Transform(double y)
{
	switch( m_Type )
	{
	default:
		return( y );

	case REGRESSION_Rez_Y:
		if( y == 0.0 )	y	= M_ALMOST_ZERO;
		return( 1.0 / y );

	case REGRESSION_Pow:
	case REGRESSION_Exp:
		if( y <= 0.0 )	y	= M_ALMOST_ZERO;
		return( log(y) );
	}
}

//---------------------------------------------------------
inline double CSG_Regression::_X_Transform(double x)
{
	switch( m_Type )
	{
	default:
		return( x );

	case REGRESSION_Rez_X:
		if( x == 0.0 )	x	= M_ALMOST_ZERO;
		return( 1.0 / x );

	case REGRESSION_Pow:
	case REGRESSION_Log:
		if( x <= 0.0 )	x	= M_ALMOST_ZERO;
		return( log(x) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression::_Linear(void)
{
	int		i;
	double	x, y, s_xx, s_xy, s_x, s_y, s_dx2, s_dy2, s_dxdy;

	//-----------------------------------------------------
	if( m_nValues > 1 )
	{
		m_xMean	= m_xMin = m_xMax = _X_Transform(m_x[0]);
		m_yMean	= m_yMin = m_yMax = _Y_Transform(m_y[0]);

		for(i=1; i<m_nValues; i++)
		{
			m_xMean	+= (x = _X_Transform(m_x[i]));
			m_yMean	+= (y = _Y_Transform(m_y[i]));

			M_SET_MINMAX(m_xMin, m_xMax, x);
			M_SET_MINMAX(m_yMin, m_yMax, y);
		}

		m_xMean	/= m_nValues;
		m_yMean	/= m_nValues;

		//-------------------------------------------------
		if( m_xMin < m_xMax && m_yMin < m_yMax )
		{
			s_x = s_y = s_xx = s_xy = s_dx2 = s_dy2 = s_dxdy = 0.0;

			for(i=0; i<m_nValues; i++)
			{
				x		 = _X_Transform(m_x[i]);
				y		 = _Y_Transform(m_y[i]);

				s_x		+= x;
				s_y		+= y;
				s_xx	+= x * x;
				s_xy	+= x * y;

				x		-= m_xMean;
				y		-= m_yMean;

				s_dx2	+= x * x;
				s_dy2	+= y * y;
				s_dxdy	+= x * y;
			}

			//---------------------------------------------
			m_xVar		= s_dx2 / m_nValues;
			m_yVar		= s_dy2 / m_nValues;

			m_RCoeff	= s_dxdy / s_dx2;
			m_RConst	= (s_xx * s_y - s_x * s_xy) / (m_nValues * s_xx - s_x * s_x);
			m_R			= s_dxdy / sqrt(s_dx2 * s_dy2);

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
bool CSG_Regression::Calculate(TSG_Regression_Type Type)
{
	double	d;

	m_Type	= Type;

	if( _Linear() )
	{
		switch( m_Type )
		{
		case REGRESSION_Linear:	default:
			break;

		case REGRESSION_Rez_X:
			m_xVar		= 1.0 / m_xVar;
			break;

		case REGRESSION_Rez_Y:
			d			= m_RConst;
			m_RConst	= 1.0 / m_RCoeff;
			m_RCoeff	= d   * m_RCoeff;
			m_yVar		= 1.0 / m_yVar;
			break;

		case REGRESSION_Pow:
			m_RConst	= exp(m_RConst);
			m_xVar		= exp(m_xVar);
			m_yVar		= exp(m_yVar);
			break;

		case REGRESSION_Exp:
			m_RConst	= exp(m_RConst);
			m_yVar		= exp(m_yVar);
			break;

		case REGRESSION_Log:
			m_xVar		= exp(m_xVar);
			break;
		}

		if( m_Type != REGRESSION_Linear )
		{
			_Get_MinMeanMax(
				m_xMin, m_xMean, m_xMax,
				m_yMin, m_yMean, m_yMax
			);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Regression::Calculate(int nValues, double *x, double *y, TSG_Regression_Type Type)
{
	bool	bResult;

	Destroy();

	m_nValues	= nValues;
	m_x			= x;
	m_y			= y;

	bResult		= Calculate(Type);

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
