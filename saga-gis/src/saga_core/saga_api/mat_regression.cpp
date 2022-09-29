
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
double SG_Regression_Get_Adjusted_R2(double r2, int n, int p, TSG_Regression_Correction Correction)
{
	double r = 1. - r2;

	switch( Correction )
	{
	case REGRESSION_CORR_None: default:
		return( r2 );

	case REGRESSION_CORR_Smith:
		r2	= 1. - ((n     ) / (n - p    )) * r;
		break;

	case REGRESSION_CORR_Wherry_1:
		r2	= 1. - ((n - 1.) / (n - p - 1.)) * r;
		break;

	case REGRESSION_CORR_Wherry_2:
		r2	= 1. - ((n - 1.) / (n - p     )) * r;
		break;

	case REGRESSION_CORR_Olkin_Pratt:
	//	r2	= 1. - ((n - 3.) / (n - p - 2.)) * (r + (2. / (n - p)) * r*r);
		r2	= 1. - ((n - 3.) * r / (n - p - 1.)) * (1. + (2. * r) / (n - p + 1.));
		break;

	case REGRESSION_CORR_Pratt:
		r2	= 1. - ((n - 3.) * r / (n - p - 1.)) * (1. + (2. * r) / (n - p - 2.3));
		break;

	case REGRESSION_CORR_Claudy_3:
		r2	= 1. - ((n - 4.) * r / (n - p - 1.)) * (1. + (2. * r) / (n - p + 1.));
		break;
	}

	return( r2 < 0. ? 0. : r2 > 1. ? 1. : r2 );
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_Regression::asString(void)
{
	static CSG_String s;

	s.Printf(
		"N = %d\n"
		"  Min. = %.6f  Max. = %.6f\n  Arithmetic Mean = %.6f\n  Variance = %.6f\n  Standard Deviation = %.6f\n"
		"  Min. = %.6f  Max. = %.6f\n  Arithmetic Mean = %.6f\n  Variance = %.6f\n  Standard Deviation = %.6f\n"
		"Linear Regression:\n  Y = %.6f * X %+.6f\n  (r=%.4f, r\xc2\xb2=%.4f)",
		m_nValues,
		m_xMin, m_xMax, m_xMean, m_xVar, sqrt(m_xVar),
		m_yMin, m_yMax, m_yMean, m_yVar, sqrt(m_yVar),
		m_RCoeff, m_RConst, m_R, m_R*m_R
	);

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Regression::Get_x(double y)	const
{
	if( m_nValues > 0. )
	{
		switch( m_Type )
		{
		case REGRESSION_Linear:	// Y = a + b * X     -> X = (Y - a) / b
			if( m_RCoeff != 0. )
				return( (m_RConst * y) / m_RCoeff );

		case REGRESSION_Rez_X:	// Y = a + b / X     -> X = b / (Y - a)
			if( (y = y - m_RConst) != 0. )
				return( m_RCoeff / y );

		case REGRESSION_Rez_Y:	// Y = a / (b - X)   -> X = b - a / Y
			if( y != 0. )
				return( m_RCoeff - m_RConst / y );

		case REGRESSION_Pow:	// Y = a * X^b       -> X = (Y / a)^(1 / b)
			if( m_RConst != 0. && m_RCoeff != 0. )
				return( pow(y / m_RConst, 1. / m_RCoeff) );

		case REGRESSION_Exp:	// Y = a * e^(b * X) -> X = ln(Y / a) / b
			if( m_RConst != 0. && (y = y / m_RConst) > 0. && m_RCoeff != 0. )
			return( log(y) / m_RCoeff );

		case REGRESSION_Log:	// Y = a + b * ln(X) -> X = e^((Y - a) / b)
			if( m_RCoeff != 0. )
				return( exp((y - m_RConst) / m_RCoeff) );
		}
	}

	return( sqrt(-1.) ); // NaN
}

//---------------------------------------------------------
double CSG_Regression::Get_y(double x)	const
{
	if( m_nValues > 0. )
	{
		switch( m_Type )
		{
		case REGRESSION_Linear:	// Y = a + b * X
			return( m_RConst + m_RCoeff * x );

		case REGRESSION_Rez_X:	// Y = a + b / X
			if( x != 0. )
				return( m_RConst + m_RCoeff / x );

		case REGRESSION_Rez_Y:	// Y = a / (b - X)
			if( (x = m_RCoeff - x) != 0. )
				return( m_RConst / x );

		case REGRESSION_Pow:	// Y = a * X^b
			return( m_RConst * pow(x, m_RCoeff) );

		case REGRESSION_Exp:	// Y = a e^(b * X)
			return( m_RConst * exp(m_RCoeff * x) );

		case REGRESSION_Log:	// Y = a + b * ln(X)
			if( x > 0. )
				return( m_RConst + m_RCoeff * log(x) );
		}
	}

	return( sqrt(-1.) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression::_Get_MinMeanMax(double &xMin, double &xMean, double &xMax, double &yMin, double &yMean, double &yMax)
{
	if( m_nValues > 0 )
	{
		xMin = xMean = xMax = m_x[0];
		yMin = yMean = yMax = m_y[0];

		for(int i=1; i<m_nValues; i++)
		{
			xMean += m_x[i]; M_SET_MINMAX(xMin, xMax, m_x[i]);
			yMean += m_y[i]; M_SET_MINMAX(yMin, yMax, m_y[i]);
		}

		xMean /= m_nValues;
		yMean /= m_nValues;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
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
		if( y == 0. ) { y = M_ALMOST_ZERO; }
		return( 1. / y );

	case REGRESSION_Pow:
	case REGRESSION_Exp:
		if( y <= 0. ) { y = M_ALMOST_ZERO; }
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
		if( x == 0. ) { x = M_ALMOST_ZERO; }
		return( 1. / x );

	case REGRESSION_Pow:
	case REGRESSION_Log:
		if( x <= 0. ) { x = M_ALMOST_ZERO; }
		return( log(x) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression::_Linear(void)
{
	if( m_nValues > 1 )
	{
		m_xMean = m_xMin = m_xMax = _X_Transform(m_x[0]);
		m_yMean = m_yMin = m_yMax = _Y_Transform(m_y[0]);

		for(int i=1; i<m_nValues; i++)
		{
			double x = _X_Transform(m_x[i]); m_xMean += x; M_SET_MINMAX(m_xMin, m_xMax, x);
			double y = _Y_Transform(m_y[i]); m_yMean += y; M_SET_MINMAX(m_yMin, m_yMax, y);
		}

		m_xMean	/= m_nValues;
		m_yMean	/= m_nValues;

		//-------------------------------------------------
		if( m_xMin < m_xMax && m_yMin < m_yMax )
		{
			double s_x = 0., s_y = 0., s_xx = 0., s_xy = 0., s_dx2 = 0., s_dy2 = 0., s_dxdy = 0.;

			for(int i=0; i<m_nValues; i++)
			{
				double x = _X_Transform(m_x[i]);
				double y = _Y_Transform(m_y[i]);

				s_x     += x;
				s_y     += y;
				s_xx    += x * x;
				s_xy    += x * y;

				x       -= m_xMean;
				y       -= m_yMean;

				s_dx2   += x * x;
				s_dy2   += y * y;
				s_dxdy  += x * y;
			}

			//---------------------------------------------
			m_xVar   = s_dx2 / m_nValues;
			m_yVar   = s_dy2 / m_nValues;

			m_RCoeff = s_dxdy / s_dx2;
			m_RConst = (s_xx * s_y - s_x * s_xy) / (m_nValues * s_xx - s_x * s_x);
			m_R      = s_dxdy / sqrt(s_dx2 * s_dy2);
			m_R_Adj  = SG_Regression_Get_Adjusted_R2(m_R*m_R, m_nValues, 1);
			m_P      = CSG_Test_Distribution::Get_F_Tail_from_R2(m_R*m_R, 1, m_nValues);

			return( m_R != 0. );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression::Calculate(TSG_Regression_Type Type)
{
	m_Type	= Type;

	if( _Linear() )
	{
		switch( m_Type )
		{
		case REGRESSION_Linear:	default: {
			break; }

		case REGRESSION_Rez_X: {
			m_xVar   = 1. / m_xVar;
			break; }

		case REGRESSION_Rez_Y: {
			double d = m_RConst;
			m_RConst = 1. / m_RCoeff;
			m_RCoeff = d  * m_RCoeff;
			m_yVar   = 1. / m_yVar;
			break; }

		case REGRESSION_Pow: {
			m_RConst = exp(m_RConst);
			m_xVar   = exp(m_xVar);
			m_yVar   = exp(m_yVar);
			break; }

		case REGRESSION_Exp: {
			m_RConst = exp(m_RConst);
			m_yVar   = exp(m_yVar);
			break; }

		case REGRESSION_Log: {
			m_xVar  = exp(m_xVar);
			break; }
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
	Destroy(); m_nValues = nValues; m_x = x; m_y = y;

	return( Calculate(Type) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
