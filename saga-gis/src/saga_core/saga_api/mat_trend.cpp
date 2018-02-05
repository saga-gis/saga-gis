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
//                     mat_trend.cpp                     //
//                                                       //
//                 Copyright (C) 2006 by                 //
//                      Olaf Conrad                      //
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
//-------------------------------------------------------//
//                                                       //
//  Based on the LMFit.h/cpp, Fit.h/cpp source codes of  //
//    A. Ringeler (see 'table_calculus' sub project).    //
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
#define SWAP(a, b)	{	double temp = (a); (a) = (b); (b) = temp;	}

//---------------------------------------------------------
#define EPSILON		0.001


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Trend::CParams::Create(const CSG_String &Variables)
{
	if( m_Variables.Length() != Variables.Length() )
	{
		m_Variables	= Variables;

		m_A    .Create(Get_Count());
		m_Atry .Create(Get_Count());
		m_Beta .Create(Get_Count());
		m_dA   .Create(Get_Count());
		m_dA2  .Create(Get_Count());

		m_Alpha.Create(Get_Count(), Get_Count());
		m_Covar.Create(Get_Count(), Get_Count());
	}

	m_A.Assign(1.0);

	return( true );
}

//---------------------------------------------------------
bool CSG_Trend::CParams::Destroy(void)
{
	m_Variables.Clear();

	m_A    .Destroy();
	m_Atry .Destroy();
	m_Beta .Destroy();
	m_dA   .Destroy();
	m_dA2  .Destroy();
	m_Alpha.Destroy();
	m_Covar.Destroy();

	m_Alpha.Destroy();
	m_Covar.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Trend::CSG_Trend(void)
{
	m_Lambda_Max	= 10000;
	m_Iter_Max		= 1000;
	m_bOkay			= false;

//	Set_Formula("a + b * x");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Trend::Set_Formula(const CSG_String &Formula)
{
	m_bOkay	= false;

	m_Params.Destroy();

	if( m_Formula.Set_Formula(Formula) )
	{
		CSG_String	Params, Used(m_Formula.Get_Used_Variables());

		for(size_t i=0; i<Used.Length(); i++)
		{
			if( Used[i] >= 'a' && Used[i] <= 'z' && Used[i] != 'x' )
			{
				Params	+= Used[i];
			}
		}

		return( m_Params.Create(Params) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Trend::Init_Parameter(const SG_Char &Variable, double Value)
{
	for(size_t i=0; i<m_Params.m_Variables.Length(); i++)
	{
		if( Variable == m_Params.m_Variables[i] )
		{
			m_Params.m_A[i]	= Value;

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Trend::Clr_Data(void)
{
	m_Data.Destroy();

	m_bOkay	= false;
}

//---------------------------------------------------------
void CSG_Trend::Set_Data(double *x, double *y, int n, bool bAdd)
{
	if( !bAdd )
	{
		m_Data.Destroy();
	}

	for(int i=0; i<n; i++)
	{
		Add_Data(x[i], y[i]);
	}
}

//---------------------------------------------------------
void CSG_Trend::Set_Data(const CSG_Points &Data, bool bAdd)
{
	if( !bAdd )
	{
		m_Data.Destroy();
	}

	for(int i=0; i<Data.Get_Count(); i++)
	{
		Add_Data(Data.Get_X(i), Data.Get_Y(i));
	}
}

//---------------------------------------------------------
bool CSG_Trend::Add_Data(double x, double y)
{
	int	n	= m_Data.Get_NCols();

	if( n > 0 )
	{
		if( !m_Data.Add_Col() )
		{
			return( false );
		}

		if( m_xMin > x ) m_xMin = x; else if( m_xMax < x ) m_xMax = x;
		if( m_yMin > y ) m_yMin = y; else if( m_yMax < y ) m_yMax = y;
	}
	else
	{
		if( !m_Data.Create(1, 2) )
		{
			return( false );
		}

		m_xMin = m_xMax = x;
		m_yMin = m_yMax = y;
	}

	m_Data[0][n]	= x;
	m_Data[1][n]	= y;

	m_bOkay	= false;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Trend::Set_Max_Iterations(int Iterations)
{
	if( Iterations > 0 )
	{
		m_Iter_Max		= Iterations;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Trend::Set_Max_Lambda(double Lambda)
{
	if( Lambda > 0.0 )
	{
		m_Lambda_Max	= Lambda;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Trend::Get_Trend(double *x, double *y, int n, const CSG_String &Formula)
{
	Set_Data(x, y, n, false);

	return( !Formula.is_Empty() && !Set_Formula(Formula) ? false : Get_Trend() );
}

//---------------------------------------------------------
bool CSG_Trend::Get_Trend(const CSG_Points &Data, const CSG_String &Formula)
{
	Set_Data(Data, false);

	return( !Formula.is_Empty() && !Set_Formula(Formula) ? false : Get_Trend() );
}

//---------------------------------------------------------
bool CSG_Trend::Get_Trend(const CSG_String &Formula)
{
	return( !Formula.is_Empty() && !Set_Formula(Formula) ? false : Get_Trend() );
}

//---------------------------------------------------------
bool CSG_Trend::Get_Trend(void)
{
	CSG_String	sError;

	if( m_Formula.Get_Error(sError) )
	{
		return( false );
	}

	if( Get_Data_Count() <= 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_bOkay	= true;

	int		i;

	if( m_Params.Get_Count() > 0 )
	{
		m_Lambda	= 0.001;

		_Get_mrqcof(m_Params.m_A, m_Params.m_Alpha, m_Params.m_Beta);

		m_ChiSqr_o	= m_ChiSqr;

		for(i=0; i<m_Params.Get_Count(); i++)
		{
			m_Params.m_Atry[i]	= m_Params.m_A[i];
		}

		//-------------------------------------------------
		for(i=0; i<m_Iter_Max && m_Lambda<m_Lambda_Max && m_bOkay && SG_UI_Process_Get_Okay(false); i++)
		{
			m_bOkay	= _Fit_Function();
		}

		//-------------------------------------------------
		for(i=0; i<m_Params.Get_Count(); i++)
		{
			m_Formula.Set_Variable(m_Params.m_Variables[i], m_Params.m_A[i]);
		}
	}

	//-----------------------------------------------------
	double	y_m, y_o, y_t;

	for(i=0, y_m=0.0; i<Get_Data_Count(); i++)
	{
		y_m	+= Get_Data_Y(i);
	}

	y_m	/= Get_Data_Count();

	for(i=0, y_o=0.0, y_t=0.0; i<Get_Data_Count(); i++)
	{
		y_o	+= SG_Get_Square(y_m -                     Get_Data_Y(i) );
		y_t	+= SG_Get_Square(y_m - m_Formula.Get_Value(Get_Data_X(i)));
	}

	m_ChiSqr_o	= y_o > 0.0 ? y_t / y_o : 0.0;

	return( m_bOkay );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Trend::Get_Error(void)
{
	CSG_String	Message;

	if( !m_bOkay )
	{
		if( !m_Formula.Get_Error(Message) )
		{
			Message.Printf(_TL("Error in Trend Calculation"));
		}
	}

	return( Message );
}

//---------------------------------------------------------
CSG_String CSG_Trend::Get_Formula(int Type)
{
	CSG_String	s;

	switch( Type )
	{
	case SG_TREND_STRING_Formula:	default:
		s	+= m_Formula.Get_Formula().c_str();
		break;

	case SG_TREND_STRING_Function:
		s	+= m_Formula.Get_Formula().c_str();
		s	+= "\n";

		if( m_Params.Get_Count() > 0 )
		{
			s	+= "\n";

			for(int i=0; i<m_Params.Get_Count() && m_bOkay; i++)
			{
				s	+= CSG_String::Format("%c = %g\n", m_Params.m_Variables[i], m_Params.m_A[i]);
			}
		}
		break;

	case SG_TREND_STRING_Formula_Parameters:
		s	+= m_Formula.Get_Formula().c_str();
		s	+= "\n";

		if( m_Params.Get_Count() > 0 )
		{
			s	+= "\n";

			for(int i=0; i<m_Params.Get_Count() && m_bOkay; i++)
			{
				s	+= CSG_String::Format("%c = %g\n", m_Params.m_Variables[i], m_Params.m_A[i]);
			}
		}
		break;

	case SG_TREND_STRING_Complete:
		s	+= m_Formula.Get_Formula().c_str();
		s	+= "\n";

		if( m_Params.Get_Count() > 0 )
		{
			s	+= "\n";

			for(int i=0; i<m_Params.Get_Count() && m_bOkay; i++)
			{
				s	+= CSG_String::Format("%c = %g\n", m_Params.m_Variables[i], m_Params.m_A[i]);
			}
		}

		s	+= "\n";
		s	+= CSG_String::Format("N = %d\n" , Get_Data_Count());
		s	+= CSG_String::Format("R2 = %g\n", Get_R2() * 100.0);
		break;
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Trend::_Fit_Function(void)
{
	int		i, j;

	//-----------------------------------------------------
	for(i=0; i<m_Params.Get_Count(); i++)
	{
		for(j=0; j<m_Params.Get_Count(); j++)
		{
			m_Params.m_Covar[i][j]	= m_Params.m_Alpha[i][j];
		}

		m_Params.m_Covar[i][i]	= m_Params.m_Alpha[i][i] * (1.0 + m_Lambda);
		m_Params.m_dA2  [i]		= m_Params.m_Beta [i];
	}

	//-----------------------------------------------------
	if( _Get_Gaussj() == false )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(i=0; i<m_Params.Get_Count(); i++)
	{
		m_Params.m_dA[i]	= m_Params.m_dA2[i];
	}

	//-----------------------------------------------------
	if( m_Lambda == 0.0 )
	{
		for(i=m_Params.Get_Count()-1; i>0; i--)
		{
			for(j=0; j<m_Params.Get_Count(); j++)
			{
				SWAP(m_Params.m_Covar[j][i], m_Params.m_Covar[j][i-1]);
			}

			for(j=0; j<m_Params.Get_Count(); j++)
			{
				SWAP(m_Params.m_Covar[i][j], m_Params.m_Covar[i-1][j]);
			}
		}
	}
	else
	{
		for(i=0; i<m_Params.Get_Count(); i++)
		{
			m_Params.m_Atry[i]	= m_Params.m_A[i] + m_Params.m_dA[i];
		}

		_Get_mrqcof(m_Params.m_Atry, m_Params.m_Covar, m_Params.m_dA);

		if( m_ChiSqr < m_ChiSqr_o )
		{
			m_Lambda	*= 0.1;
			m_ChiSqr_o	 = m_ChiSqr;

			for(i=0; i<m_Params.Get_Count(); i++)
			{
				for(j=0; j<m_Params.Get_Count(); j++)
				{
					m_Params.m_Alpha[i][j]	= m_Params.m_Covar[i][j];
				}

				m_Params.m_Beta[i]	= m_Params.m_dA[i];
			}

			for(i=0; i<m_Params.Get_Count(); i++)	// Achtung!! in aelteren Versionen war hier ein Fehler
			{
				m_Params.m_A[i]		= m_Params.m_Atry[i];
			}
		}
		else 
		{
			m_Lambda	*= 10.0;
			m_ChiSqr	 = m_ChiSqr_o;
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSG_Trend::_Get_Gaussj(void)
{
	int		i, j, k, iCol, iRow;

	//-----------------------------------------------------
	CSG_Array_Int	indxc(m_Params.Get_Count());
	CSG_Array_Int	indxr(m_Params.Get_Count());
	CSG_Array_Int	ipiv (m_Params.Get_Count());
	
	for(i=0; i<m_Params.Get_Count(); i++)
	{
		ipiv[i]	= 0;
	}

	//-----------------------------------------------------
	for(i=0, iCol=-1, iRow=-1; i<m_Params.Get_Count(); i++)
	{
		double	big	= 0.0;

		for(j=0; j<m_Params.Get_Count(); j++)
		{
			if( ipiv[j] != 1 )
			{
				for(k=0; k<m_Params.Get_Count(); k++)
				{
					if( ipiv[k] == 0 )
					{
						if( fabs(m_Params.m_Covar[j][k]) >= big )
						{
							big		= fabs(m_Params.m_Covar[j][k]);
							iRow	= j;
							iCol	= k;
						}
					}
					else if( ipiv[k] > 1 )
					{
						return( false );	// singular matrix...
					}
				}
			}
		}

		if( iCol < 0 || iRow < 0 )
		{
			return( false );	// singular matrix...
		}

		//-------------------------------------------------
		ipiv[iCol]++;

		if( iRow != iCol )
		{
			for(j=0; j<m_Params.Get_Count(); j++)
			{
				SWAP(m_Params.m_Covar[iRow][j], m_Params.m_Covar[iCol][j]);
			}

			SWAP(m_Params.m_dA2[iRow], m_Params.m_dA2[iCol]);
		}

		indxr[i]	= iRow;
		indxc[i]	= iCol;

		if( fabs(m_Params.m_Covar[iCol][iCol]) < 1E-300 )
		{
			return( false );	// singular matrix...
		}

		//-------------------------------------------------
		double	pivinv	= 1.0 / m_Params.m_Covar[iCol][iCol];

		m_Params.m_Covar[iCol][iCol]	= 1.0;

		for(j=0; j<m_Params.Get_Count(); j++)
		{
			m_Params.m_Covar[iCol][j]	*= pivinv;
		}

		m_Params.m_dA2[iCol]	*= pivinv;

		//-------------------------------------------------
		for(j=0; j<m_Params.Get_Count(); j++)
		{
			if( j != iCol )
			{
				double	temp	= m_Params.m_Covar[j][iCol];

				m_Params.m_Covar[j][iCol]	= 0.0;

				for(k=0; k<m_Params.Get_Count(); k++)
				{
					m_Params.m_Covar[j][k]	-= m_Params.m_Covar[iCol][k] * temp;
				}

				m_Params.m_dA2[j]	-= m_Params.m_dA2[iCol] * temp;
			}
		}
	}

	//-----------------------------------------------------
	for(i=m_Params.Get_Count()-1; i>=0; i--)
	{
        if( indxr[i] != indxc[i] )
		{
			for(j=0; j<m_Params.Get_Count(); j++)
			{
				SWAP(m_Params.m_Covar[j][indxr[i]], m_Params.m_Covar[j][indxc[i]]);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSG_Trend::_Get_mrqcof(CSG_Vector &Parameters, CSG_Matrix &Alpha, CSG_Vector &Beta)
{
	CSG_Vector	dy_da(m_Params.Get_Count());

	Alpha.Assign(0.0);
	Beta .Assign(0.0);

	m_ChiSqr	= 0.0;

	for(int k=0; k<Get_Data_Count(); k++)
	{
		double	y;

		_Get_Function(y, dy_da.Get_Data(), Get_Data_X(k), Parameters);

		double	dy	= Get_Data_Y(k) - y;

		for(int i=0; i<m_Params.Get_Count(); i++)
		{
			for(int j=0; j<=i; j++)
			{
				Alpha[i][j]	+= dy_da[i] * dy_da[j];
			}

			Beta[i]	+= dy * dy_da[i];
		}

		m_ChiSqr	+= dy * dy;
	}

	//-----------------------------------------------------
	for(int i=1; i<m_Params.Get_Count(); i++)
	{
		for(int j=0; j<i; j++)
		{
			Alpha[j][i]	= Alpha[i][j];
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Trend::_Get_Function(double &y, double *dy_da, double x, const double *Parameters)
{
	int		i;

	//-----------------------------------------------------
	for(i=0; i<m_Params.Get_Count(); i++)
	{
		m_Formula.Set_Variable(m_Params.m_Variables[i], Parameters[i]);
	}

	y	= m_Formula.Get_Value(x);

	//-----------------------------------------------------
	for(i=0; i<m_Params.Get_Count(); i++)
	{
		m_Formula.Set_Variable(m_Params.m_Variables[i], Parameters[i] + EPSILON);

		dy_da[i]	 = m_Formula.Get_Value(x);
		dy_da[i]	-= y;
		dy_da[i]	/= EPSILON;

		m_Formula.Set_Variable(m_Params.m_Variables[i], Parameters[i] - EPSILON);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Trend_Polynom::CSG_Trend_Polynom(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Trend_Polynom::Destroy(void)
{
	m_Order	= 0;

	Clr_Data();

	return( true );
}

//---------------------------------------------------------
bool CSG_Trend_Polynom::Set_Order(int Order)
{
	m_a.Destroy();

	if( Order > 0 )
	{
		m_Order	= Order;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Trend_Polynom::Clr_Data(void)
{
	m_a.Destroy();
	m_y.Destroy();
	m_x.Destroy();

	return( true );
}

//---------------------------------------------------------
bool CSG_Trend_Polynom::Set_Data(double *x, double *y, int n, bool bAdd)
{
	if( !bAdd )
	{
		Clr_Data();
	}

	m_x.Add_Rows(n);
	m_y.Add_Rows(n);

	for(int i=0, j=m_x.Get_N()-1; i<n; i++)
	{
		m_x[j]	= x[i];
		m_y[j]	= y[i];
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Trend_Polynom::Add_Data(double x, double y)
{
	return( m_x.Add_Row(x) && m_y.Add_Row(y) );
}

//---------------------------------------------------------
bool CSG_Trend_Polynom::Get_Trend(void)
{
	if( m_Order < 1 || m_x.Get_N() <= m_Order )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			i;
	double		d, Ym, SSE, SSR;
	CSG_Matrix	X, Xt, C;

	//-----------------------------------------------------
	X .Create(m_Order + 1, m_y.Get_N());
	Xt.Create(m_y.Get_N(), m_Order + 1);

	for(i=0, Ym=0.0; i<m_y.Get_N(); i++)
	{
		X[i][0] = Xt[0][i] = d = 1.0;

		for(int j=1; j<=m_Order; j++)
		{
			X[i][j] = Xt[j][i] = (d = d * m_x[i]);
		}

		Ym	+= m_y[i];
	}

	Ym	/= m_y.Get_N();

	m_a	= (Xt * X).Get_Inverse() * (Xt * m_y);

	//-----------------------------------------------------
	CSG_Vector	Yr	= X * m_a;

	for(i=0, SSE=0.0, SSR=0.0; i<m_y.Get_N(); i++)
	{
		SSE	+= SG_Get_Square(Yr[i] - m_y[i]);
		SSR	+= SG_Get_Square(Yr[i] - Ym    );
	}

	m_r2	= SSR / (SSR + SSE);

	return( true );
}

//---------------------------------------------------------
double CSG_Trend_Polynom::Get_Value(double x)	const
{
	if( m_a.Get_N() > 0 )
	{
		double	y	= m_a(0);
		double	d	= 1.0;

		for(int i=1; i<m_a.Get_N(); i++)
		{
			d	*= x;
			y	+= d * m_a(i);
		}

		return( y );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
