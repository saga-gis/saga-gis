
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
//              mat_regression_multiple.cpp              //
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
//                37077 Hamburg                          //
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
CSG_Regression_Weighted::CSG_Regression_Weighted(void)
{
	m_r2	= -1.0;

	m_Log_maxIter		= 30;
	m_Log_Epsilon		= 0.001;
	m_Log_Difference	= 1000.;
}

//---------------------------------------------------------
CSG_Regression_Weighted::~CSG_Regression_Weighted(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Regression_Weighted::Destroy(void)
{
	m_r2	= -1.0;

	m_y.Destroy();
	m_w.Destroy();
	m_X.Destroy();
	m_b.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression_Weighted::Add_Sample(double Weight, double Dependent, const CSG_Vector &Predictors)
{
	if( m_X.Get_NRows() == 0 )
	{
		m_X.Create(Predictors.Get_N() + 1, 1);
	}
	else if( m_X.Get_NCols() == Predictors.Get_N() + 1 )
	{
		m_X.Add_Row();
	}
	else
	{
		return( false );
	}

	m_w.Add_Row(Weight   );
	m_y.Add_Row(Dependent);

	double	*y	= m_X[m_X.Get_NRows() - 1];

	y[0]	= 1.0;

	for(int i=0; i<Predictors.Get_N(); i++)
	{
		y[i + 1]	= Predictors[i];
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression_Weighted::Calculate(const CSG_Vector &Weights, const CSG_Vector &Dependents, const CSG_Matrix &Predictors, bool bLogistic)
{
	Destroy();

	if( Weights.Get_N() == Dependents.Get_N() && Weights.Get_N() == Predictors.Get_NRows() )
	{
		for(int i=0; i<Weights.Get_N(); i++)
		{
			Add_Sample(Weights[i], Dependents[i], Predictors.Get_Row(i));

			return( Calculate(bLogistic) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Regression_Weighted::Calculate(bool bLogistic)
{
	//-----------------------------------------------------
	int	i, nSamples, nPredictors;

	if( (nSamples = m_w.Get_N()) <= (nPredictors = m_X.Get_NCols() - 1) || nSamples < 2 )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( bLogistic )
	{
		m_b	= _Log_Get_Beta(m_X, m_y, m_w);

		if( m_b.Get_N() == 0 )
		{
			return( false );
		}
	}
	else
	{
		CSG_Matrix	YtW(nSamples, 1 + nPredictors);

		for(i=0; i<nSamples; i++)
		{
			YtW[0][i]	= m_w[i];

			for(int j=1; j<=nPredictors; j++)
			{
				YtW[j][i]	= m_w[i] * m_X[i][j];
			}
		}

		m_b	= (YtW * m_X).Get_Inverse() * (YtW * m_y);
	}

	//-----------------------------------------------------
	CSG_Simple_Statistics	yStats(m_y);

	double	rss	= 0.0, tss	= 0.0;

	for(i=0; i<nSamples; i++)
	{
		double	yr	= m_b[0];

		for(int j=1; j<=nPredictors; j++)
		{
			yr	+= m_b[j] * m_X[i][j];
		}

		if( bLogistic )
		{
			yr	= 1. / (1. + exp(-yr));
		}

		rss	+= m_w[i] * SG_Get_Square(m_y[i] - yr);
		tss	+= m_w[i] * SG_Get_Square(m_y[i] - yStats.Get_Mean());
	}

	//-----------------------------------------------------
	if( tss > 0.0 && tss >= rss )
	{
		m_r2	= fabs(tss - rss) / tss;

		return( true );
	}

	m_r2	= -1.0;

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Zhang, D., Ren, N., and Hou, X. (2018):
// An improved logistic regression model based on a spatially weighted technique
// (ILRBSWT v1.0) and its application to mineral prospectivity mapping.
// Geosci. Model Dev., 11, 2525-2539, https://doi.org/10.5194/gmd-11-2525-2018.

//---------------------------------------------------------
bool CSG_Regression_Weighted::Set_Log_maxIter(int maxIter)
{
	if( maxIter > 0 )
	{
		m_Log_maxIter	= maxIter;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Regression_Weighted::Set_Log_Epsilon(double Epsilon)
{
	if( Epsilon >= 0. )
	{
		m_Log_Epsilon	= Epsilon;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Regression_Weighted::Set_Log_Difference(double Difference)
{
	if( Difference > 0. )
	{
		m_Log_Difference	= Difference;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Vector CSG_Regression_Weighted::_Log_Get_Beta(const CSG_Matrix &X, const CSG_Vector &y, const CSG_Vector &w)
{
	CSG_Vector	b(X.Get_NCols()), b_best;

	CSG_Vector	p	= _Log_Get_Props(X, b);

	for(int i=0; i<m_Log_maxIter; ++i)
	{
		CSG_Vector	b_new = _Log_Get_Beta(b, X, y, w, p);

		if( b_new.Get_N() == 0 )
		{
			return( b_best );
		}

		for(int j=0; j<b_new.Get_N(); ++j)
		{
			if( SG_is_NaN(b_new[j]) )
			{
				return( b_best );
			}
		}

		if( _Log_NoChange(b, b_new) )
		{
			return( b_new );
		}

		if( _Log_OutOfControl(b, b_new) ) 
		{
			return( b_best );
		}

		p		= _Log_Get_Props(X, b_new);
		b		= b_new;
		b_best	= b;
	}

	return( b_best );
} 

//---------------------------------------------------------
CSG_Vector CSG_Regression_Weighted::_Log_Get_Beta(const CSG_Vector &b, const CSG_Matrix &X, const CSG_Vector &y, const CSG_Vector &w, const CSG_Vector &p)
{
	CSG_Matrix Xt	= X.Get_Transpose()         ;	//     X'
	CSG_Matrix M	= Xt * _Log_Get_Xwp(p, X, w);	//     X'* w * V(t-1) * X
	CSG_Matrix N	= M.Get_Inverse() * Xt      ;	// inv(X'* w * V(t-1) * X) * X'
	CSG_Vector v	= N  * _Log_Get_Ywp(p, y, w);	// inv(X'* w * V(t-1) * X) * X' * w * (y - p(t-1))

	if( v.Get_N() == b.Get_N() )
	{
		return( b + v );
	}

	return( CSG_Vector() );
}

//---------------------------------------------------------
CSG_Matrix CSG_Regression_Weighted::_Log_Get_Xwp(const CSG_Vector &p, const CSG_Matrix &X, const CSG_Vector &w)
{
	CSG_Matrix	Xwp;

	if( p.Get_N() == X.Get_NRows() && Xwp.Create(X.Get_NCols(), X.Get_NRows()) )
	{
		for(int i=0; i<X.Get_NRows(); ++i)
		{
			for(int j=0; j<X.Get_NCols(); ++j)
			{
				Xwp[i][j]	= w[i] * p[i] * (1. - p[i]) * X[i][j];	// compute W(u) * V(u) * X
			}
		}
	}

	return( Xwp );
}

//---------------------------------------------------------
CSG_Vector CSG_Regression_Weighted::_Log_Get_Ywp(const CSG_Vector &p, const CSG_Vector &y, const CSG_Vector &w)
{
	CSG_Vector	Ywp(y.Get_N());

	if( y.Get_N() == p.Get_N() && Ywp.Create(y.Get_N()) )
	{
		for(int i=0; i<Ywp.Get_N(); i++)
		{
			Ywp[i]	= w[i] * (y[i] - p[i]);	// compute W(u) * (y - p)
		}
	}

	return( Ywp );
}

//---------------------------------------------------------
CSG_Vector CSG_Regression_Weighted::_Log_Get_Props(const CSG_Matrix &X, const CSG_Vector &b)
{
	CSG_Vector p(X.Get_NRows());

	for(int i=0; i<X.Get_NRows(); ++i)
	{
		double	z	= 0.;

		for(int j=0; j<X.Get_NCols(); ++j)
		{
			z	+= X[i][j] * b[j];
		}

		p[i]	= 1. / (1. + exp(-z));
	}

	return( p );
}

//---------------------------------------------------------
bool CSG_Regression_Weighted::_Log_NoChange(const CSG_Vector &b_old, const CSG_Vector &b_new)
{
	for(int i=0; i<b_old.Get_N(); ++i)
	{
		if( fabs(b_old[i] - b_new[i]) > m_Log_Epsilon )
		{
			return( false );
		}
	}

	return( true );	// if the new beta is equal to the old one under certain accuracy
}

//---------------------------------------------------------
bool CSG_Regression_Weighted::_Log_OutOfControl(const CSG_Vector &b_old, const CSG_Vector &b_new)
{
	for(int i=0; i<b_old.Get_N(); ++i)
	{
		if( b_old[i] == 0.0 )
		{
			return( false );
		}

		if( fabs(b_old[i] - b_new[i]) / fabs(b_old[i]) > m_Log_Difference )
		{
			return( true );
		}
	}

	return( false );	// if the new beta has obvious difference with the old one
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression_Weighted::Get_CrossValidation(int nSubSamples)
{
/*	if( Get_Predictor_Count() <= 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Regression_Weighted	Model;
	CSG_Simple_Statistics	Stats, SR, SE;

	int		i, nModels	= 0;

	for(i=0; i<m_Samples_Model.Get_NRows(); i++)
	{
		Stats	+= m_Samples_Model[i][0];
	}

	//-----------------------------------------------------
	// leave-one-out cross validation (LOOCV)
	if( nSubSamples <= 1 || nSubSamples > m_Samples_Model.Get_NRows() / 2 )
	{
		for(i=0; i<m_Samples_Model.Get_NRows() && SG_UI_Process_Get_Okay(); i++)
		{
			CSG_Vector	w(m_w);	w.Del_Row(i);	Model
			CSG_Vector	x(m_y);	x.Del_Row(i);
			CSG_Matrix	Y(m_X);	Y.Del_Row(i);

			if( Model.Calculate(w, x, Y) )
			{
				nModels++;

				double	dObsrv	= m_Samples_Model[i][0];
				double	dModel	= Model.Get_Value(CSG_Vector(m_nPredictors, m_Samples_Model[i] + 1));

				SE	+= SG_Get_Square(dModel - dObsrv);
				SR	+= SG_Get_Square(dModel - (Stats.Get_Sum() - dObsrv) / Samples.Get_NRows());
			}
		}
	}

	//-----------------------------------------------------
	// k-fold cross validation
	else
	{
		int	*SubSet	= new int[m_Samples_Model.Get_NRows()];

		for(i=0; i<m_Samples_Model.Get_NRows(); i++)
		{
			SubSet[i]	= i % nSubSamples;
		}

		//-------------------------------------------------
		for(int iSubSet=0; iSubSet<nSubSamples && SG_UI_Process_Get_Okay(); iSubSet++)
		{
			CSG_Simple_Statistics	Samples_Stats;
			CSG_Matrix	Samples(m_Samples_Model), Validation;

			for(i=Samples.Get_NRows()-1; i>=0; i--)
			{
				if( SubSet[i] == iSubSet )
				{
					Validation.Add_Row(Samples.Get_Row(i));
					Samples   .Del_Row(i);
				}
				else
				{
					Samples_Stats	+= Samples[i][0];
				}
			}

			//---------------------------------------------
			if( Model.Get_Model(Samples) )
			{
				nModels++;

				for(i=0; i<Validation.Get_NRows(); i++)
				{
					double	dObsrv	= Validation[i][0];
					double	dModel	= Model.Get_Value(CSG_Vector(m_nPredictors, Validation[i] + 1));

					SE	+= SG_Get_Square(dModel - dObsrv);
					SR	+= SG_Get_Square(dModel - Samples_Stats.Get_Mean());
				}
			}
		}

		delete[](SubSet);
	}

	//-----------------------------------------------------
	m_CV_RMSE		= sqrt(SE.Get_Mean());
	m_CV_NRMSE		= sqrt(SE.Get_Mean()) / Stats.Get_Range();
	m_CV_R2			= SR.Get_Sum() / (SR.Get_Sum() + SE.Get_Sum());
	m_CV_nSamples	= nModels;
*/
	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
