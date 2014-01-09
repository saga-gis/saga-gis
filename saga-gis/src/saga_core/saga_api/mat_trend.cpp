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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#define SWAP(a, b)	{	temp = (a); (a) = (b); (b) = temp;	}

//---------------------------------------------------------
#define EPSILON		0.001


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Trend::CFncParams::CFncParams(void)
{
	m_Count		= 0;
}

//---------------------------------------------------------
CSG_Trend::CFncParams::~CFncParams(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Trend::CFncParams::Create(const SG_Char *Variables, int nVariables)
{
	int		i;

	if( m_Count != nVariables )
	{
		Destroy();

		m_Count		= nVariables;

		m_Variables	= (SG_Char *)SG_Calloc(m_Count, sizeof(SG_Char));
		m_A			= (double  *)SG_Calloc(m_Count, sizeof(double));
		m_Atry		= (double  *)SG_Calloc(m_Count, sizeof(double));
		m_Beta		= (double  *)SG_Calloc(m_Count, sizeof(double));
		m_dA		= (double  *)SG_Calloc(m_Count, sizeof(double));
		m_dA2		= (double  *)SG_Calloc(m_Count, sizeof(double));
		m_Alpha		= (double **)SG_Calloc(m_Count, sizeof(double *));
		m_Covar		= (double **)SG_Calloc(m_Count, sizeof(double *));

		for(i=0; i<m_Count; i++)
		{
			m_Alpha[i]		= (double *)SG_Calloc(m_Count, sizeof(double));
			m_Covar[i]		= (double *)SG_Calloc(m_Count, sizeof(double));
		}
	}

	for(i=0; i<m_Count; i++)
	{
		m_Variables[i]	= Variables[i];
		m_A[i]			= 1.0;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Trend::CFncParams::Destroy(void)
{
	if( m_Count > 0 )
	{
		for(int i=0; i<m_Count; i++)
		{
			SG_Free(m_Alpha[i]);
			SG_Free(m_Covar[i]);
		}

		SG_Free(m_Variables);
		SG_Free(m_A);
		SG_Free(m_Atry);
		SG_Free(m_Beta);
		SG_Free(m_dA);
		SG_Free(m_dA2);
		SG_Free(m_Alpha);
		SG_Free(m_Covar);

		m_Count	= 0;
	}

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

//	Set_Formula(SG_T("a + b * x"));
}

//---------------------------------------------------------
CSG_Trend::~CSG_Trend(void)
{
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Trend::Set_Formula(const SG_Char *Formula)
{
	m_bOkay	= false;

	if( m_Formula.Set_Formula(Formula) )
	{
		CSG_String	vars, uvars(m_Formula.Get_Used_Variables());

		for(unsigned int i=0; i<uvars.Length(); i++)
		{
			if( uvars.c_str()[i] >= 'a' && uvars.c_str()[i] <= 'z' && uvars.c_str()[i] != 'x' )
			{
				vars.Append(uvars.c_str()[i]);
			}
		}

		return( m_Params.Create(vars.c_str(), (int)vars.Length()) );
	}

	m_Params.Destroy();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Trend::Clr_Data(void)
{
	m_Data.Clear();

	m_bOkay	= false;
}

//---------------------------------------------------------
void CSG_Trend::Set_Data(double *xData, double *yData, int nData, bool bAdd)
{
	if( !bAdd )
	{
		m_Data.Clear();
	}

	for(int i=0; i<nData; i++)
	{
		Add_Data(xData[i], yData[i]);
	}

	m_bOkay	= false;
}

//---------------------------------------------------------
void CSG_Trend::Set_Data(const CSG_Points &Data, bool bAdd)
{
	if( !bAdd )
	{
		m_Data.Clear();
	}

	for(int i=0; i<Data.Get_Count(); i++)
	{
		Add_Data(Data.Get_X(i), Data.Get_Y(i));
	}

	m_bOkay	= false;
}

//---------------------------------------------------------
void CSG_Trend::Add_Data(double x, double y)
{
	if( m_Data.Get_Count() == 0 )
	{
		m_xMin	= m_xMax	= x;
		m_yMin	= m_yMax	= y;
	}
	else
	{
		if( m_xMin > x )	m_xMin	= x;	else if( m_xMax < x )	m_xMax	= x;
		if( m_yMin > y )	m_yMin	= y;	else if( m_yMax < y )	m_yMax	= y;
	}

	m_Data.Add(x, y);

	m_bOkay	= false;
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
bool CSG_Trend::Get_Trend(double *xData, double *yData, int nData, const SG_Char *Formula)
{
	Set_Data(xData, yData, false);

	if( Formula )
	{
		Set_Formula(Formula);
	}

	return( Get_Trend() );
}

//---------------------------------------------------------
bool CSG_Trend::Get_Trend(const CSG_Points &Data, const SG_Char *Formula)
{
	Set_Data(Data, false);

	if( Formula )
	{
		Set_Formula(Formula);
	}

	return( Get_Trend() );
}

//---------------------------------------------------------
bool CSG_Trend::Get_Trend(void)
{
	CSG_String	sError;

	if( m_Formula.Get_Error(sError) )
	{
		return( false );
	}

	//-------------------------------------------------
	int		i;

	m_bOkay	= true;

	if( m_Data.Get_Count() > 1 )
	{
		if( m_Params.m_Count > 0 )
		{
			m_Lambda	= 0.001;

			_Get_mrqcof(m_Params.m_A, m_Params.m_Alpha, m_Params.m_Beta);

			m_ChiSqr_o	= m_ChiSqr;

			for(i=0; i<m_Params.m_Count; i++)
			{
				m_Params.m_Atry[i]	= m_Params.m_A[i];
			}

			//-----------------------------------------
			for(i=0; i<m_Iter_Max && m_Lambda<m_Lambda_Max && m_bOkay && SG_UI_Process_Get_Okay(false); i++)
			{
				m_bOkay	= _Fit_Function();
			}

			//-----------------------------------------
			for(i=0; i<m_Params.m_Count; i++)
			{
				m_Formula.Set_Variable(m_Params.m_Variables[i], m_Params.m_A[i]);
			}
		}

		//---------------------------------------------
		double	y_m, y_o, y_t;

		for(i=0, y_m=0.0; i<m_Data.Get_Count(); i++)
		{
			y_m	+= m_Data.Get_Y(i);
		}

		y_m	/= m_Data.Get_Count();

		for(i=0, y_o=0.0, y_t=0.0; i<m_Data.Get_Count(); i++)
		{
			y_o	+= SG_Get_Square(y_m - m_Data.Get_Y(i));
			y_t	+= SG_Get_Square(y_m - m_Formula.Get_Value(m_Data.Get_X(i)));
		}

		m_ChiSqr_o	= y_o > 0.0 ? y_t / y_o : 1.0;
	}

	return( m_bOkay );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Trend::Get_Error(void)
{
	CSG_String		Message;

	if( m_bOkay )
	{
	}
	else if( m_Formula.Get_Error(Message) )
	{
	}
	else
	{
		Message.Printf(SG_T("%s"), _TL("Error in Trend Calculation"));
	}

	return( Message );
}

//---------------------------------------------------------
CSG_String CSG_Trend::Get_Formula(int Type)
{
	int			i;
	CSG_String	s;

	switch( Type )
	{
	case SG_TREND_STRING_Formula:	default:
		s	+= m_Formula.Get_Formula().c_str();
		break;

	case SG_TREND_STRING_Function:
		s	+= m_Formula.Get_Formula().c_str();
		s	+= SG_T("\n");

		if( m_Params.m_Count > 0 )
		{
			s	+= SG_T("\n");

			for(i=0; i<m_Params.m_Count && m_bOkay; i++)
			{
				s	+= CSG_String::Format(SG_T("%c = %g\n"), m_Params.m_Variables[i], m_Params.m_A[i]);
			}
		}
		break;

	case SG_TREND_STRING_Formula_Parameters:
		s	+= m_Formula.Get_Formula().c_str();
		s	+= SG_T("\n");

		if( m_Params.m_Count > 0 )
		{
			s	+= SG_T("\n");

			for(i=0; i<m_Params.m_Count && m_bOkay; i++)
			{
				s	+= CSG_String::Format(SG_T("%c = %g\n"), m_Params.m_Variables[i], m_Params.m_A[i]);
			}
		}
		break;

	case SG_TREND_STRING_Complete:
		s	+= m_Formula.Get_Formula().c_str();
		s	+= SG_T("\n");

		if( m_Params.m_Count > 0 )
		{
			s	+= SG_T("\n");

			for(i=0; i<m_Params.m_Count && m_bOkay; i++)
			{
				s	+= CSG_String::Format(SG_T("%c = %g\n"), m_Params.m_Variables[i], m_Params.m_A[i]);
			}
		}

		s	+= SG_T("\n");
		s	+= CSG_String::Format(SG_T("N = %d\n") , Get_Data_Count());
		s	+= CSG_String::Format(SG_T("R2 = %g\n"), Get_R2() * 100.0);
		break;
	}

	return( s );
}

//---------------------------------------------------------
double CSG_Trend::Get_ChiSquare(void)
{
	if( m_bOkay )
	{
//		return( sqrt(m_ChiSqr / m_Data.Get_Count()) );	// RMS of Residuals (stdfit)
		return( m_ChiSqr );
	}

	return( 0.0 );
}

//---------------------------------------------------------
double CSG_Trend::Get_R2(void)
{
	if( m_bOkay )
	{
		return( m_ChiSqr_o );
	}

	return( 0.0 );
}

//---------------------------------------------------------
double CSG_Trend::Get_Value(double x)
{
	if( m_bOkay )
	{
		return( m_Formula.Get_Value(x) );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Trend::_Fit_Function(void)
{
	int		i, j;

	//-----------------------------------------------------
	for(i=0; i<m_Params.m_Count; i++)
	{
		for(j=0; j<m_Params.m_Count; j++)
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
	for(i=0; i<m_Params.m_Count; i++)
	{
		m_Params.m_dA[i]	= m_Params.m_dA2[i];
	}

	//-----------------------------------------------------
	if( m_Lambda == 0.0 )
	{
		double	temp;

		for(i=m_Params.m_Count-1; i>0; i--)
		{
			for(j=0; j<m_Params.m_Count; j++)
			{
				SWAP(m_Params.m_Covar[j][i], m_Params.m_Covar[j][i-1]);
			}

			for(j=0; j<m_Params.m_Count; j++)
			{
				SWAP(m_Params.m_Covar[i][j], m_Params.m_Covar[i-1][j]);
			}
		}
	}
	else
	{
		for(i=0; i<m_Params.m_Count; i++)
		{
			m_Params.m_Atry[i]	= m_Params.m_A[i] + m_Params.m_dA[i];
		}

		_Get_mrqcof(m_Params.m_Atry, m_Params.m_Covar, m_Params.m_dA);

		if( m_ChiSqr < m_ChiSqr_o )
		{
			m_Lambda	*= 0.1;
			m_ChiSqr_o	 = m_ChiSqr;

			for(i=0; i<m_Params.m_Count; i++)
			{
				for(j=0; j<m_Params.m_Count; j++)
				{
					m_Params.m_Alpha[i][j]	= m_Params.m_Covar[i][j];
				}

				m_Params.m_Beta[i]	= m_Params.m_dA[i];
			}

			for(i=0; i<m_Params.m_Count; i++)	// Achtung!! in aelteren Versionen war hier ein Fehler
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
	int		i, j, k, iCol, iRow, *indxc, *indxr, *ipiv;
	double	big, pivinv, temp;

	//-----------------------------------------------------
	indxc	= (int *)SG_Calloc(m_Params.m_Count, sizeof(int));
	indxr	= (int *)SG_Calloc(m_Params.m_Count, sizeof(int));
	ipiv	= (int *)SG_Calloc(m_Params.m_Count, sizeof(int));
	
	for(i=0; i<m_Params.m_Count; i++)
	{
		ipiv[i]	= 0;
	}

	//-----------------------------------------------------
	for(i=0, iCol=-1, iRow=-1; i<m_Params.m_Count; i++)
	{
		for(j=0, big=0.0; j<m_Params.m_Count; j++)
		{
			if( ipiv[j] != 1 )
			{
				for(k=0; k<m_Params.m_Count; k++)
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
						SG_Free(indxc);	SG_Free(indxr);	SG_Free(ipiv);	return( false );	// singular matrix...
					}
				}
			}
		}

		if( iCol < 0 || iRow < 0 )
		{
			SG_Free(indxc);	SG_Free(indxr);	SG_Free(ipiv);	return( false );	// singular matrix...
		}

		//-------------------------------------------------
		ipiv[iCol]++;

		if( iRow != iCol )
		{
			for(j=0; j<m_Params.m_Count; j++)
			{
				SWAP(m_Params.m_Covar[iRow][j], m_Params.m_Covar[iCol][j]);
			}

			SWAP(m_Params.m_dA2[iRow], m_Params.m_dA2[iCol]);
		}

		indxr[i]	= iRow;
		indxc[i]	= iCol;

		if( fabs(m_Params.m_Covar[iCol][iCol]) < 1E-300 )
		{
			SG_Free(indxc);	SG_Free(indxr);	SG_Free(ipiv);	return( false );	// singular matrix...
		}

		//-------------------------------------------------
		pivinv		= 1.0 / m_Params.m_Covar[iCol][iCol];
		m_Params.m_Covar[iCol][iCol]	= 1.0;

		for(j=0; j<m_Params.m_Count; j++)
		{
			m_Params.m_Covar[iCol][j]	*= pivinv;
		}

		m_Params.m_dA2[iCol]	*= pivinv;

		for(j=0; j<m_Params.m_Count; j++)
		{
			if( j != iCol )
			{
				temp	= m_Params.m_Covar[j][iCol];
				m_Params.m_Covar[j][iCol]	= 0.0;

				for(k=0; k<m_Params.m_Count; k++)
				{
					m_Params.m_Covar[j][k]	-= m_Params.m_Covar[iCol][k] * temp;
				}

				m_Params.m_dA2[j]	-= m_Params.m_dA2[iCol] * temp;
			}
		}
	}

	//-----------------------------------------------------
	for(i=m_Params.m_Count-1; i>=0; i--)
	{
        if( indxr[i] != indxc[i] )
		{
			for(j=0; j<m_Params.m_Count; j++)
			{
				SWAP(m_Params.m_Covar[j][indxr[i]], m_Params.m_Covar[j][indxc[i]]);
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(indxc);
	SG_Free(indxr);
	SG_Free(ipiv);

	return( true );
}

//---------------------------------------------------------
bool CSG_Trend::_Get_mrqcof(double *Parameters, double **Alpha, double *Beta)
{
	int		i, j, k;
	double	y, dy, *dy_da;

	//-----------------------------------------------------
	for(i=0; i<m_Params.m_Count; i++)
	{
		for(j=0; j<=i; j++)
		{
			Alpha[i][j] = 0.0;
		}

		Beta[i]		= 0.0;
	}

	//-----------------------------------------------------
	dy_da	= (double *)SG_Calloc(m_Params.m_Count, sizeof(double));

	for(k=0, m_ChiSqr=0.0; k<m_Data.Get_Count(); k++)
	{
		_Get_Function(m_Data[k].x, Parameters, y, dy_da);

		dy	= m_Data[k].y - y;

		for(i=0; i<m_Params.m_Count; i++)
		{
			for(j=0; j<=i; j++)
			{
				Alpha[i][j]	+= dy_da[i] * dy_da[j];
			}

			Beta[i]		+= dy * dy_da[i];
		}

		m_ChiSqr	+= dy * dy;
	}

	SG_Free(dy_da);

	//-----------------------------------------------------
	for(i=1; i<m_Params.m_Count; i++)
	{
		for(j=0; j<i; j++)
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
void CSG_Trend::_Get_Function(double x, double *Parameters, double &y, double *dy_da)
{
	int		i;

	//-----------------------------------------------------
	for(i=0; i<m_Params.m_Count; i++)
	{
		m_Formula.Set_Variable(m_Params.m_Variables[i], Parameters[i]);
	}

	y	= m_Formula.Get_Value(x);

	//-----------------------------------------------------
	for(i=0; i<m_Params.m_Count; i++)
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
