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
CSG_Regression_Weighted::CSG_Regression_Weighted(void)
{
	m_r2	= -1.0;
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

	m_x.Destroy();
	m_w.Destroy();
	m_Y.Destroy();
	m_b.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression_Weighted::Add_Sample(double Weight, double Dependent, const CSG_Vector &Predictors)
{
	if( m_Y.Get_NRows() == 0 )
	{
		m_Y.Create(Predictors.Get_N() + 1, 1);
	}
	else if( m_Y.Get_NCols() == Predictors.Get_N() + 1 )
	{
		m_Y.Add_Row();
	}
	else
	{
		return( false );
	}

	m_w.Add_Row(Weight   );
	m_x.Add_Row(Dependent);

	double	*y	= m_Y[m_Y.Get_NRows() - 1];

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
bool CSG_Regression_Weighted::Calculate(const CSG_Vector &Weights, const CSG_Vector &Dependents, const CSG_Matrix &Predictors)
{
	Destroy();

	if( Weights.Get_N() == Dependents.Get_N() && Weights.Get_N() == Predictors.Get_NRows() )
	{
		for(int i=0; i<Weights.Get_N(); i++)
		{
			Add_Sample(Weights[i], Dependents[i], Predictors.Get_Row(i));

			return( Calculate() );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Regression_Weighted::Calculate(void)
{
	//-----------------------------------------------------
	int			i, nSamples, nPredictors;

	if( (nSamples = m_w.Get_N()) <= (nPredictors = m_Y.Get_NCols() - 1) || nSamples < 2 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Matrix	YtW(nSamples, 1 + nPredictors);

	double	xMean	= 0.0;

	for(i=0; i<nSamples; i++)
	{
		xMean		+= m_x[i];
		YtW[0][i]	 = m_w[i];

		for(int j=1; j<=nPredictors; j++)
		{
			YtW[j][i]	= m_w[i] * m_Y[i][j];
		}
	}

	xMean	/= nSamples;

	m_b		 = (YtW * m_Y).Get_Inverse() * (YtW * m_x);

	//-----------------------------------------------------
	double	rss	= 0.0, tss	= 0.0;

	for(i=0; i<nSamples; i++)
	{
		double	xr	= m_b[0];

		for(int j=1; j<=nPredictors; j++)
		{
			xr	+= m_b[j] * m_Y[i][j];
		}

		rss	+= m_w[i] * SG_Get_Square(m_x[i] - xr);
		tss	+= m_w[i] * SG_Get_Square(m_x[i] - xMean);
	}

	//-----------------------------------------------------
	if( tss > 0.0 && tss >= rss )
	{
		m_r2	= (tss - rss) / tss;

		return( true );
	}

	m_r2	= -1.0;

	return( false );
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
			CSG_Vector	x(m_x);	x.Del_Row(i);
			CSG_Matrix	Y(m_Y);	Y.Del_Row(i);

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
