
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
//                                                       //
//                                                       //
//                                                       //
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Regression::CSG_Regression(void)
{
	Destroy();
}

//---------------------------------------------------------
CSG_Regression::~CSG_Regression(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Regression::Destroy(void)
{
	m_R2   = -1.; // mark as invalid (or unprocessed)

	m_Type = REGRESSION_Linear;

	m_x.Destroy();
	m_y.Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression::Add_Values(double x, double y)
{
	return( m_y.Add_Row(y) && m_x.Add_Row(x) );
}

//---------------------------------------------------------
bool CSG_Regression::Set_Values(int nValues, double *x, double *y)
{
	Destroy();

	return( m_y.Create(nValues, y) && m_x.Create(nValues, x) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_Regression::asString(void)
{
	static CSG_String s; s.Clear();

	s += CSG_String::Format("\nNumber of observations = %d\n", Get_Count());
	s += CSG_String::Format("\nPredictor variable (x):\n  min. = %.6f, max. = %.6f, mean = %.6f, stddev. = %.6f\n", m_xMin, m_xMax, m_xMean, sqrt(m_xVar));
	s += CSG_String::Format("\nDependent variable (y):\n  min. = %.6f, max. = %.6f, mean = %.6f, stddev. = %.6f\n", m_yMin, m_yMax, m_yMean, sqrt(m_yVar));
	s += CSG_String::Format(SG_T("\nRegression (r = %.4f, r² = %.4f):\n\n  y = "), m_R, m_R2);

	switch( m_Type )
	{
	case REGRESSION_Linear: s += CSG_String::Format("%.6f %+.6f * x"     , m_RConst, m_RCoeff); break; // Y = a + b * X
	case REGRESSION_Rez_X : s += CSG_String::Format("%.6f %+.6f / x"     , m_RConst, m_RCoeff); break; // Y = a + b / X
	case REGRESSION_Rez_Y : s += CSG_String::Format("%.6f / (%.6f - x)"  , m_RConst, m_RCoeff); break; // Y = a / (b - X)
	case REGRESSION_Pow   : s += CSG_String::Format("%.6f * x^%.6f"      , m_RConst, m_RCoeff); break; // Y = a * X^b
	case REGRESSION_Exp   : s += CSG_String::Format("%.6f * e^(%.6f * x)", m_RConst, m_RCoeff); break; // Y = a * e^(b * X)
	case REGRESSION_Log   : s += CSG_String::Format("%.6f %+.6f * ln(x)" , m_RConst, m_RCoeff); break; // Y = a + b * ln(X)
	}

	s += "\n";

	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Regression::Get_x(double y)	const
{
	if( m_R2 >= 0. )
	{
		switch( m_Type )
		{
		case REGRESSION_Linear:	// Y = a + b * X     -> X = (Y - a) / b
			if( m_RCoeff != 0. )
				return( (m_RConst * y) / m_RCoeff );
			break;

		case REGRESSION_Rez_X:	// Y = a + b / X     -> X = b / (Y - a)
			if( (y = y - m_RConst) != 0. )
				return( m_RCoeff / y );
			break;

		case REGRESSION_Rez_Y:	// Y = a / (b - X)   -> X = b - a / Y
			if( y != 0. )
				return( m_RCoeff - m_RConst / y );
			break;

		case REGRESSION_Pow:	// Y = a * X^b       -> X = (Y / a)^(1 / b)
			if( m_RConst != 0. && m_RCoeff != 0. )
				return( pow(y / m_RConst, 1. / m_RCoeff) );
			break;

		case REGRESSION_Exp:	// Y = a * e^(b * X) -> X = ln(Y / a) / b
			if( m_RConst != 0. && (y = y / m_RConst) > 0. && m_RCoeff != 0. )
				return( log(y) / m_RCoeff );
			break;

		case REGRESSION_Log:	// Y = a + b * ln(X) -> X = e^((Y - a) / b)
			if( m_RCoeff != 0. )
				return( exp((y - m_RConst) / m_RCoeff) );
			break;
		}
	}

	return( sqrt(-1.) ); // NaN
}

//---------------------------------------------------------
double CSG_Regression::Get_y(double x)	const
{
	if( m_R2 >= 0. )
	{
		switch( m_Type )
		{
		case REGRESSION_Linear:	// Y = a + b * X
			return( m_RConst + m_RCoeff * x );

		case REGRESSION_Rez_X:	// Y = a + b / X
			if( x != 0. )
				return( m_RConst + m_RCoeff / x );
			break;

		case REGRESSION_Rez_Y:	// Y = a / (b - X)
			if( (x = m_RCoeff - x) != 0. )
				return( m_RConst / x );
			break;

		case REGRESSION_Pow:	// Y = a * X^b
			return( m_RConst * pow(x, m_RCoeff) );

		case REGRESSION_Exp:	// Y = a e^(b * X)
			return( m_RConst * exp(m_RCoeff * x) );

		case REGRESSION_Log:	// Y = a + b * ln(X)
			if( x > 0. )
				return( m_RConst + m_RCoeff * log(x) );
			break;
		}
	}

	return( sqrt(-1.) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CSG_Regression::_Y_Transform(double y)
{
	switch( m_Type )
	{
	default:
		return( y );

	case REGRESSION_Rez_Y:
		if( y == 0. ) { y = M_FLT_EPSILON; }
		return( 1. / y );

	case REGRESSION_Pow:
	case REGRESSION_Exp:
		if( y <= 0. ) { y = M_FLT_EPSILON; }
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
		if( x == 0. ) { x = M_FLT_EPSILON; }
		return( 1. / x );

	case REGRESSION_Pow:
	case REGRESSION_Log:
		if( x <= 0. ) { x = M_FLT_EPSILON; }
		return( log(x) );
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression::Calculate(TSG_Regression_Type Type, bool bStdError)
{
	m_R2   = -1.; // mark as invalid (unprocessed)
	m_Type = Type;

	if( Get_Count() < 2 )
	{
		return( false );
	}

	CSG_Simple_Statistics sx, sy;

	for(int i=0; i<Get_Count(); i++)
	{
		sx += _X_Transform(m_x[i]);
		sy += _Y_Transform(m_y[i]);
	}

	m_xMin = sx.Get_Minimum(); m_xMax = sx.Get_Maximum(); m_xMean = sx.Get_Mean(); m_xVar = sx.Get_Variance();
	m_yMin = sy.Get_Minimum(); m_yMax = sy.Get_Maximum(); m_yMean = sy.Get_Mean(); m_yVar = sy.Get_Variance();

	if( m_xMin >= m_xMax || m_yMin >= m_yMax )
	{
		return( false );
	}

	//-----------------------------------------------------
	double s_x = 0., s_y = 0., s_xx = 0., s_xy = 0., s_dx2 = 0., s_dy2 = 0., s_dxdy = 0.;

	for(int i=0; i<Get_Count(); i++)
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

	//-----------------------------------------------------
	m_RCoeff = s_dxdy / s_dx2;
	m_RConst = (s_xx * s_y - s_x * s_xy) / (Get_Count() * s_xx - s_x * s_x);
	m_R      = s_dxdy / sqrt(s_dx2 * s_dy2);
	m_R2     = m_R*m_R;
	m_R2_Adj = SG_Regression_Get_Adjusted_R2(m_R2, Get_Count(), 1);
	m_P      = CSG_Test_Distribution::Get_F_Tail_from_R2(m_R2, 1, Get_Count());

	//-----------------------------------------------------
	if( !bStdError )
	{
		m_SE = -1.;
	}
	else
	{
		m_SE = 0.;

		for(int i=0; i<Get_Count(); i++)
		{
			double d = fabs(m_y[i] - Get_y(m_x[i]));

			m_SE += d;
		}

		m_SE /= Get_Count();
	}

	//-----------------------------------------------------
	switch( m_Type )
	{
	default: // case REGRESSION_Linear:
		return( true );

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

	CSG_Simple_Statistics s;

	if( !s.Create(m_x) ) { return( false ); } m_xMin = s.Get_Minimum(); m_xMean = s.Get_Mean(); m_xMax = s.Get_Maximum(); m_xVar = s.Get_Variance();
	if( !s.Create(m_y) ) { return( false ); } m_yMin = s.Get_Minimum(); m_yMean = s.Get_Mean(); m_yMax = s.Get_Maximum(); m_yVar = s.Get_Variance();

	return( true );
}

//---------------------------------------------------------
bool CSG_Regression::Calculate(int nValues, double *x, double *y, TSG_Regression_Type Type, bool bStdError)
{
	return( Set_Values(nValues, x, y) && Calculate(Type, bStdError) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
