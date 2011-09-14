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
#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum ESG_Multiple_Regression_Info_Model
{
	MLR_MODEL_R2	= 0,
	MLR_MODEL_R2_ADJ,
	MLR_MODEL_SE,
	MLR_MODEL_SSR,
	MLR_MODEL_SSE,
	MLR_MODEL_SST,
	MLR_MODEL_MSR,
	MLR_MODEL_MSE,
	MLR_MODEL_F,
	MLR_MODEL_SIG,
	MLR_MODEL_NPREDICT,
	MLR_MODEL_NSAMPLES
};

//---------------------------------------------------------
enum ESG_Multiple_Regression_Info_Steps
{
	MLR_STEP_NR		= 0,
	MLR_STEP_R,
	MLR_STEP_R2,
	MLR_STEP_R2_ADJ,
	MLR_STEP_SE,
	MLR_STEP_SSR,
	MLR_STEP_SSE,
	MLR_STEP_MSR,
	MLR_STEP_MSE,
	MLR_STEP_DF,
	MLR_STEP_F,
	MLR_STEP_SIG,
	MLR_STEP_VAR_F,
	MLR_STEP_VAR_SIG,
	MLR_STEP_DIR,
	MLR_STEP_VAR
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Regression_Multiple::CSG_Regression_Multiple(void)
{
	m_pRegression	= new CSG_Table;

	m_pRegression	->Add_Field("ID"			, SG_DATATYPE_Int);		// MLR_VAR_ID
	m_pRegression	->Add_Field("VARIABLE"		, SG_DATATYPE_String);	// MLR_VAR_NAME
	m_pRegression	->Add_Field("REGCOEFF"		, SG_DATATYPE_Double);	// MLR_VAR_RCOEFF
	m_pRegression	->Add_Field("R"				, SG_DATATYPE_Double);	// MLR_VAR_R
	m_pRegression	->Add_Field("R2"			, SG_DATATYPE_Double);	// MLR_VAR_R2
	m_pRegression	->Add_Field("R2_ADJ"		, SG_DATATYPE_Double);	// MLR_VAR_R2_ADJ
	m_pRegression	->Add_Field("STD_ERROR"		, SG_DATATYPE_Double);	// MLR_VAR_SE
	m_pRegression	->Add_Field("T"				, SG_DATATYPE_Double);	// MLR_VAR_T
	m_pRegression	->Add_Field("SIG"			, SG_DATATYPE_Double);	// MLR_VAR_SIG
	m_pRegression	->Add_Field("P"				, SG_DATATYPE_Double);	// MLR_VAR_P

	//-----------------------------------------------------
	m_pSteps		= new CSG_Table;

	m_pSteps		->Add_Field("MODEL"			, SG_DATATYPE_Int);		// MLR_STEP_NR
	m_pSteps		->Add_Field("R"				, SG_DATATYPE_Double);	// MLR_STEP_R
	m_pSteps		->Add_Field("R2"			, SG_DATATYPE_Double);	// MLR_STEP_R2
	m_pSteps		->Add_Field("R2_ADJ"		, SG_DATATYPE_Double);	// MLR_STEP_R2_ADJ
	m_pSteps		->Add_Field("STD_ERROR"		, SG_DATATYPE_Double);	// MLR_STEP_SE
	m_pSteps		->Add_Field("SSR"			, SG_DATATYPE_Double);	// MLR_STEP_SSR
	m_pSteps		->Add_Field("SSE"			, SG_DATATYPE_Double);	// MLR_STEP_SSE
	m_pSteps		->Add_Field("MSR"			, SG_DATATYPE_Double);	// MLR_STEP_MSR
	m_pSteps		->Add_Field("MSE"			, SG_DATATYPE_Double);	// MLR_STEP_MSE
	m_pSteps		->Add_Field("DF"			, SG_DATATYPE_Double);	// MLR_STEP_DF
	m_pSteps		->Add_Field("F"				, SG_DATATYPE_Double);	// MLR_STEP_F
	m_pSteps		->Add_Field("SIG"			, SG_DATATYPE_Double);	// MLR_STEP_SIG
	m_pSteps		->Add_Field("VAR_F"			, SG_DATATYPE_Double);	// MLR_STEP_VAR_F
	m_pSteps		->Add_Field("VAR_SIG"		, SG_DATATYPE_Double);	// MLR_STEP_VAR_SIG
	m_pSteps		->Add_Field("DIR"			, SG_DATATYPE_String);	// MLR_STEP_DIR
	m_pSteps		->Add_Field("VARIABLE"		, SG_DATATYPE_String);	// MLR_STEP_VAR

	//-----------------------------------------------------
	m_pModel		= new CSG_Table;

	m_pModel		->Add_Field("PARAMETER"		, SG_DATATYPE_String);
	m_pModel		->Add_Field("VALUE"			, SG_DATATYPE_Double);

	m_pModel		->Add_Record()->Set_Value(0, SG_T("R2"			));	// MLR_MODEL_R2
	m_pModel		->Add_Record()->Set_Value(0, SG_T("R2_ADJ"		));	// MLR_MODEL_R2_ADJ
	m_pModel		->Add_Record()->Set_Value(0, SG_T("STD_ERROR"	));	// MLR_MODEL_SE
	m_pModel		->Add_Record()->Set_Value(0, SG_T("SSR"			));	// MLR_MODEL_SSR
	m_pModel		->Add_Record()->Set_Value(0, SG_T("SSE"			));	// MLR_MODEL_SSE
	m_pModel		->Add_Record()->Set_Value(0, SG_T("SST"			));	// MLR_MODEL_SST
	m_pModel		->Add_Record()->Set_Value(0, SG_T("MSR"			));	// MLR_MODEL_MSR
	m_pModel		->Add_Record()->Set_Value(0, SG_T("MSE"			));	// MLR_MODEL_MSE
	m_pModel		->Add_Record()->Set_Value(0, SG_T("F"			));	// MLR_MODEL_F
	m_pModel		->Add_Record()->Set_Value(0, SG_T("SIG"			));	// MLR_MODEL_SIG
	m_pModel		->Add_Record()->Set_Value(0, SG_T("PREDICTORS"	));	// MLR_MODEL_NPREDICT
	m_pModel		->Add_Record()->Set_Value(0, SG_T("SAMPLES"		));	// MLR_MODEL_NSAMPLES

	//-----------------------------------------------------
	m_Predictor		= NULL;
	m_nPredictors	= 0;
}

//---------------------------------------------------------
CSG_Regression_Multiple::~CSG_Regression_Multiple(void)
{
	Destroy();

	delete(m_pRegression);
	delete(m_pModel);
	delete(m_pSteps);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Regression_Multiple::Destroy(void)
{
	m_Names.Clear();

	m_pRegression	->Del_Records();
	m_pSteps		->Del_Records();

	for(int i=0; i<m_pModel->Get_Count(); i++)
	{
		m_pModel->Get_Record(i)->Set_NoData(1);
	}

	if( m_Predictor )
	{
		delete[](m_bIncluded);
		delete[](m_Predictor);

		m_Predictor		= NULL;
		m_nPredictors	= 0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression_Multiple::_Initialize(const CSG_Matrix &Samples, CSG_Strings *pNames, bool bInclude)
{
	Destroy();

	int		i, nSamples, nPredictors;

	if(	(nPredictors = Samples.Get_NCols() - 1) < 1
	||	(nSamples    = Samples.Get_NRows()) <= nPredictors )
	{
		return( false );
	}

	//-------------------------------------------------
	m_bIncluded		= new int[nPredictors];
	m_Predictor		= new int[nPredictors];
	m_nPredictors	= 0;

	for(i=0; i<nPredictors; i++)
	{
		m_bIncluded[i]	= bInclude;
		m_Predictor[i]	= i;
	}

	//-------------------------------------------------
	for(i=0; i<=nPredictors; i++)
	{
		m_Names	+= pNames && pNames->Get_Count() == Samples.Get_NCols()
			? pNames->Get_String(i)
			: i == 0 ? LNG("Dependent") : CSG_String::Format(SG_T("%d. %s"), i + 1, LNG("Predictor"));
	}

	//-------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Regression_Multiple::_Get_F(int nPredictors, int nSamples, double r2_full, double r2_reduced)
{
	return( (nSamples - nPredictors - 1) * (r2_full - r2_reduced) / (1.0 - r2_full) );
}

//---------------------------------------------------------
double CSG_Regression_Multiple::_Get_P(int nPredictors, int nSamples, double r2_full, double r2_reduced)
{
	double	f	= (nSamples - nPredictors - 1) * (r2_full - r2_reduced) / (1.0 - r2_full);

	return( CSG_Test_Distribution::Get_F_Tail(f, nPredictors, nSamples - nPredictors - 1, TESTDIST_TYPE_Right) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression_Multiple::_Get_Regression(const CSG_Matrix &Samples)
{
	int		nPredictors = Samples.Get_NX() - 1;
	int		nSamples    = Samples.Get_NY();

	//-----------------------------------------------------
	int			i, j;
	double		Ym, SSR, SSE, SST, MSR, MSE, SE, R2, F;
	CSG_Vector	Y, Yr, B;
	CSG_Matrix	X, Xt, C;

	Y	.Create(nSamples);
	X	.Create(nPredictors + 1, nSamples);
	Xt	.Create(nSamples, nPredictors + 1);

	//-----------------------------------------------------
	for(i=0, Ym=0.0; i<nSamples; i++)
	{
		Ym	+= Y[i]	= Samples[i][0];

		X [i][0]	= 1.0;
		Xt[0][i]	= 1.0;

		for(j=1; j<=nPredictors; j++)
		{
			X[i][j]	= Xt[j][i] = Samples[i][j];
		}
	}

	Ym	/= nSamples;

	//-----------------------------------------------------
	C	= (Xt * X).Get_Inverse();

	B	= C * (Xt * Y);

	//-----------------------------------------------------
	Yr	= X * B;

	for(i=0, SSE=0.0, SSR=0.0, SST=0.0; i<nSamples; i++)
	{
		SSE	+= SG_Get_Square(Yr[i] - Y[i]);
		SSR	+= SG_Get_Square(Yr[i] - Ym);
	//	SST	+= SG_Get_Square(Y [i] - Ym);
	}

//	SSE	= SST - SSR;
	SST	= SSR + SSE;
	MSR	= SSR / nPredictors;
	MSE	= SSE / (nSamples - nPredictors - 1);
	SE	= sqrt(SSE / (nSamples - nPredictors));
	R2	= SSR / SST;
	F	= MSR / MSE;	// 	= (nSamples - nPredictors - 1) * (R2 / nPredictors) / (1.0 - R2);

	//-----------------------------------------------------
	m_pModel->Get_Record(MLR_MODEL_R2      )->Set_Value(1, R2);
	m_pModel->Get_Record(MLR_MODEL_R2_ADJ  )->Set_Value(1, SG_Regression_Get_Adjusted_R2(R2, nSamples, nPredictors));
	m_pModel->Get_Record(MLR_MODEL_SE      )->Set_Value(1, SE);
	m_pModel->Get_Record(MLR_MODEL_SSR     )->Set_Value(1, SSR);
	m_pModel->Get_Record(MLR_MODEL_SSE     )->Set_Value(1, SSE);
	m_pModel->Get_Record(MLR_MODEL_SST     )->Set_Value(1, SST);
	m_pModel->Get_Record(MLR_MODEL_MSR     )->Set_Value(1, MSR);
	m_pModel->Get_Record(MLR_MODEL_MSE     )->Set_Value(1, MSE);
	m_pModel->Get_Record(MLR_MODEL_F       )->Set_Value(1, F);
	m_pModel->Get_Record(MLR_MODEL_SIG     )->Set_Value(1, CSG_Test_Distribution::Get_F_Tail_from_R2(R2, nPredictors, nSamples));
	m_pModel->Get_Record(MLR_MODEL_NPREDICT)->Set_Value(1, nPredictors);
	m_pModel->Get_Record(MLR_MODEL_NSAMPLES)->Set_Value(1, nSamples);

	//-----------------------------------------------------
	CSG_Matrix	P	= SG_Get_Correlation_Matrix(Samples, true).Get_Inverse();	// get partial correlation

	for(j=0; j<=nPredictors; j++)
	{
		double	se	= SE * sqrt(fabs(C[j][j]));
		double	t	= B[j] / se;
		double	r	= -P[j][0] / sqrt(P[j][j] * P[0][0]);

		CSG_Table_Record	*pRecord	= m_pRegression->Add_Record();

		pRecord->Set_Value(MLR_VAR_ID		, j - 1);
		pRecord->Set_Value(MLR_VAR_NAME		, m_Names[j]);
		pRecord->Set_Value(MLR_VAR_RCOEFF	, B[j]);
		pRecord->Set_Value(MLR_VAR_R		, r);
		pRecord->Set_Value(MLR_VAR_R2		, r*r);
		pRecord->Set_Value(MLR_VAR_R2_ADJ	, SG_Regression_Get_Adjusted_R2(r*r, nSamples, nPredictors));
		pRecord->Set_Value(MLR_VAR_SE		, se);
		pRecord->Set_Value(MLR_VAR_T		, t);
		pRecord->Set_Value(MLR_VAR_SIG		, CSG_Test_Distribution::Get_T_Tail(t, nSamples - nPredictors, TESTDIST_TYPE_TwoTail));
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression_Multiple::_Set_Step_Info(const CSG_Matrix &X)
{
	CSG_Regression_Multiple	R;

	if( R.Calculate(X) )
	{
		m_pModel		->Assign(R.m_pModel);
		m_pRegression	->Assign(R.m_pRegression);

		m_pRegression->Get_Record(0)->Set_Value(MLR_VAR_NAME, m_Names[0]);

		for(int i=0; i<m_nPredictors; i++)
		{
			CSG_Table_Record	*pRecord	= m_pRegression->Get_Record(1 + i);

			pRecord->Set_Value(MLR_VAR_ID  , m_Predictor[i]);
			pRecord->Set_Value(MLR_VAR_NAME, m_Names[1 + m_Predictor[i]]);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Regression_Multiple::_Set_Step_Info(const CSG_Matrix &X, double R2_prev, int iVariable, bool bIn)
{
	CSG_Regression_Multiple R;

	R.Calculate(X);

	CSG_Table_Record	*pRecord	= m_pSteps->Add_Record();

	pRecord->Set_Value(MLR_STEP_NR		, m_pSteps->Get_Count());
	pRecord->Set_Value(MLR_STEP_R		, sqrt(R.Get_R2()));
	pRecord->Set_Value(MLR_STEP_R2		, R.Get_R2());
	pRecord->Set_Value(MLR_STEP_R2_ADJ	, R.Get_R2_Adj());
	pRecord->Set_Value(MLR_STEP_SE		, R.Get_StdError());
	pRecord->Set_Value(MLR_STEP_SSR		, R.m_pModel->Get_Record(MLR_MODEL_SSR)->asDouble(1));
	pRecord->Set_Value(MLR_STEP_SSE		, R.m_pModel->Get_Record(MLR_MODEL_SSE)->asDouble(1));
	pRecord->Set_Value(MLR_STEP_MSR		, R.m_pModel->Get_Record(MLR_MODEL_MSR)->asDouble(1));
	pRecord->Set_Value(MLR_STEP_MSE		, R.m_pModel->Get_Record(MLR_MODEL_MSE)->asDouble(1));
	pRecord->Set_Value(MLR_STEP_DF		, X.Get_NRows() - m_nPredictors - 1);
	pRecord->Set_Value(MLR_STEP_F		, R.m_pModel->Get_Record(MLR_MODEL_F  )->asDouble(1));
	pRecord->Set_Value(MLR_STEP_SIG		, R.m_pModel->Get_Record(MLR_MODEL_SIG)->asDouble(1));
	pRecord->Set_Value(MLR_STEP_VAR_F	, _Get_F(1, X.Get_NRows(), R.Get_R2(), R2_prev));
	pRecord->Set_Value(MLR_STEP_VAR_SIG	, _Get_P(1, X.Get_NRows(), R.Get_R2(), R2_prev));
	pRecord->Set_Value(MLR_STEP_DIR		, bIn ? SG_T(">>") : SG_T("<<"));
	pRecord->Set_Value(MLR_STEP_VAR		, m_Names[1 + iVariable]);

	return( true );
}

//---------------------------------------------------------
int CSG_Regression_Multiple::_Get_Step_In(CSG_Matrix &X, double P_in, double &R2, const CSG_Matrix &Samples)
{
	int		iNext, iIndependent, nIndependents, nPredictors, nSamples;
	double	rNext;

	CSG_Regression_Multiple R;

	nPredictors		= X.Get_NX();
	nSamples		= Samples.Get_NY();
	nIndependents	= Samples.Get_NX() - 1;

	X.Add_Cols(1);

	//-----------------------------------------------------
	for(iIndependent=0, iNext=-1, rNext=0.0; iIndependent<nIndependents; iIndependent++)
	{
		if( !m_bIncluded[iIndependent] )
		{
			X.Set_Col(1 + m_nPredictors, Samples.Get_Col(1 + iIndependent));

			if( R.Calculate(X) && (iNext < 0 || rNext < R.Get_R2()) )
			{
				iNext	= iIndependent;
				rNext	= R.Get_R2();
			}
		}
	}

	//-----------------------------------------------------
	if( iNext >= 0 && _Get_P(1, nSamples, rNext, R2) <= P_in )
	{
		m_bIncluded[iNext]			= true;
		m_Predictor[m_nPredictors]	= iNext;

		m_nPredictors++;

		X.Set_Col(m_nPredictors, Samples.Get_Col(1 + iNext));

		_Set_Step_Info(X, R2, iNext, true);

		R2	= rNext;

		return( iNext );
	}

	X.Del_Col(nPredictors);

	return( -1 );
}

//---------------------------------------------------------
int CSG_Regression_Multiple::_Get_Step_Out(CSG_Matrix &X, double P_out, double &R2)
{
	int		iNext, iIndependent, nIndependents, nPredictors, nSamples;
	double	rNext;

	CSG_Regression_Multiple R;

	if( R2 <= 0.0 )
	{
		R.Calculate(X);

		R2	= R.Get_R2();
	}

	nSamples		= X.Get_NY();
	nPredictors		= X.Get_NX() - 2;
	nIndependents	= X.Get_NX() - 1;

	//-----------------------------------------------------
	for(iIndependent=0, iNext=-1, rNext=1.0; iIndependent<nIndependents; iIndependent++)
	{
		if( m_bIncluded[iIndependent] )
		{
			CSG_Matrix	X_reduced(X);

			X_reduced.Del_Col(1 + iIndependent);

			if( R.Calculate(X_reduced) && (iNext < 0 || rNext < R.Get_R2()) )
			{
				iNext	= iIndependent;
				rNext	= R.Get_R2();
			}
		}
	}

	//-----------------------------------------------------
	if( iNext >= 0 && _Get_P(1, nSamples, R2, rNext) > P_out )
	{
		X.Del_Col(1 + iNext);

		_Set_Step_Info(X, R2, iNext, false);

		R2	= R.Get_R2();

		m_nPredictors--;

		m_bIncluded[iNext]	= false;

		for(int j=iNext; j<nPredictors; j++)
		{
			m_Predictor[j]	= m_Predictor[j + 1];
		}

		return( iNext );
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression_Multiple::Calculate(const CSG_Matrix &Samples, CSG_Strings *pNames)
{
	if( !_Initialize(Samples, pNames, true) )
	{
		return( false );
	}

	_Get_Regression(Samples);

	return( true );
}

//---------------------------------------------------------
bool CSG_Regression_Multiple::Calculate_Forward(const CSG_Matrix &Samples, double P_in, CSG_Strings *pNames)
{
	if( !_Initialize(Samples, pNames, false) )
	{
		return( false );
	}

//	return( _Get_Forward(Samples, P_in );

	//-----------------------------------------------------
	CSG_Matrix		X(1, Samples.Get_NY(), Samples.Get_Col(0).Get_Data());

	m_nPredictors	= 0;

	double	R2		= 0.0;

	while( _Get_Step_In(X, P_in, R2, Samples) >= 0 );

	//-----------------------------------------------------
	if( m_nPredictors > 0 )
	{
		_Set_Step_Info(X);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Regression_Multiple::Calculate_Backward(const CSG_Matrix &Samples, double P_out, CSG_Strings *pNames)
{
	if( !_Initialize(Samples, pNames, true) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Matrix		X(Samples);

	m_nPredictors	= Samples.Get_NX() - 1;

	double	R2		= 0.0;

	while( _Get_Step_Out(X, P_out, R2) >= 0 );

	//-----------------------------------------------------
	if( m_nPredictors > 0 )
	{
		_Set_Step_Info(X);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Regression_Multiple::Calculate_Stepwise(const CSG_Matrix &Samples, double P_in, double P_out, CSG_Strings *pNames)
{
	if( !_Initialize(Samples, pNames, false) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Matrix		X(1, Samples.Get_NY(), Samples.Get_Col(0).Get_Data());

	m_nPredictors	= 0;

	double	R2		= 0.0;

	if( P_out <= P_in )
	{
		P_out	= P_in + 0.001;
	}

	while( _Get_Step_In(X, P_in, R2, Samples) >= 0 && SG_UI_Process_Get_Okay() )
	{
		if( m_nPredictors > 1 )
		{
			_Get_Step_Out(X, P_out, R2);
		}
	}

	//-----------------------------------------------------
	if( m_nPredictors > 0 )
	{
		_Set_Step_Info(X);

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
double CSG_Regression_Multiple::Get_R2			(void)	const	{	return( m_pModel->Get_Record(MLR_MODEL_R2      )->asDouble(1) );	}
double CSG_Regression_Multiple::Get_R2_Adj		(void)	const	{	return( m_pModel->Get_Record(MLR_MODEL_R2_ADJ  )->asDouble(1) );	}
double CSG_Regression_Multiple::Get_StdError	(void)	const	{	return( m_pModel->Get_Record(MLR_MODEL_SE      )->asDouble(1) );	}
double CSG_Regression_Multiple::Get_F			(void)	const	{	return( m_pModel->Get_Record(MLR_MODEL_F       )->asDouble(1) );	}
double CSG_Regression_Multiple::Get_P			(void)	const	{	return( m_pModel->Get_Record(MLR_MODEL_SIG     )->asDouble(1) );	}
int    CSG_Regression_Multiple::Get_nPredictors	(void)	const	{	return( m_pModel->Get_Record(MLR_MODEL_NPREDICT)->asInt   (1) );	}
int    CSG_Regression_Multiple::Get_nSamples	(void)	const	{	return( m_pModel->Get_Record(MLR_MODEL_NSAMPLES)->asInt   (1) );	}
int    CSG_Regression_Multiple::Get_DegFreedom	(void)	const	{	return( Get_nSamples() - Get_nPredictors() - 1 );	}

//---------------------------------------------------------
const SG_Char * CSG_Regression_Multiple::Get_Name(int iVariable) const
{
	if( iVariable >= 0 && iVariable < m_pRegression->Get_Count() - 1 )
	{
		return( m_pRegression->Get_Record(1 + iVariable)->asString(MLR_VAR_NAME) );
	}

	return( SG_T("") );
}

//---------------------------------------------------------
double CSG_Regression_Multiple::Get_RConst(void) const
{
	if( m_pRegression->Get_Count() > 0 )
	{
		return( m_pRegression->Get_Record(0)->asDouble(MLR_VAR_RCOEFF) );
	}

	return( 0.0 );
}

//---------------------------------------------------------
double CSG_Regression_Multiple::Get_Parameter(int iVariable, int Parameter)	const
{
	if( iVariable >= 0 && iVariable < m_pRegression->Get_Count() - 1 && Parameter >= 0 && Parameter <= MLR_VAR_P )
	{
		return( m_pRegression->Get_Record(1 + iVariable)->asDouble(Parameter) );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Regression_Multiple::Get_Info(void)	const
{
	int			i;
	CSG_String	s;

	if( Get_nPredictors() < 1 )
	{
		return( s );
	}

	//-----------------------------------------------------
	if( m_pSteps->Get_Count() > 0 )
	{
		s	+= CSG_String::Format(SG_T("\n%s:\n\n"), _TL("Steps"));
		s	+= CSG_String::Format(SG_T("No.   \tR     \tR2    \tR2 adj\tStdErr\tF     \tP     \tF step\tP step\tVariable\n"));
		s	+= CSG_String::Format(SG_T("------\t------\t------\t------\t------\t------\t------\t------\t------\t------\n"));

		for(i=0; i<m_pSteps->Get_Count(); i++)
		{
			CSG_Table_Record	*pRecord	= m_pSteps->Get_Record(i);

			s	+= CSG_String::Format(SG_T("%d.\t%.2f\t%.2f\t%.2f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%s %s\n"),
				pRecord->asInt   (MLR_STEP_NR     ),
				pRecord->asDouble(MLR_STEP_R      ),
				pRecord->asDouble(MLR_STEP_R2     ) * 100.0,
				pRecord->asDouble(MLR_STEP_R2_ADJ ) * 100.0,
				pRecord->asDouble(MLR_STEP_SE     ),
				pRecord->asDouble(MLR_STEP_F      ),
				pRecord->asDouble(MLR_STEP_SIG    ) * 100.0,
				pRecord->asDouble(MLR_STEP_VAR_F  ),
				pRecord->asDouble(MLR_STEP_VAR_SIG) * 100.0,
				pRecord->asString(MLR_STEP_DIR    ),
				pRecord->asString(MLR_STEP_VAR    )
			);
		}
	}

	//-----------------------------------------------------
	s	+= CSG_String::Format(SG_T("\n%s:\n\n"), _TL("Correlation"));
	s	+= CSG_String::Format(SG_T("No.   \tR     \tR2    \tR2 adj\tStdErr\tt     \tSig.  \tb     \tVariable\n"));
	s	+= CSG_String::Format(SG_T("------\t------\t------\t------\t------\t------\t------\t------\t------\n"));

	for(i=0; i<m_pRegression->Get_Count(); i++)
	{
		CSG_Table_Record	*pRecord	= m_pRegression->Get_Record(i);

		s	+= CSG_String::Format(SG_T("%d.\t%.2f\t%.2f\t%.2f\t%.3f\t%.3f\t%.3f\t%.3f\t%s\n"),
			i,
			pRecord->asDouble(MLR_VAR_R     ),
			pRecord->asDouble(MLR_VAR_R2    ) * 100.0,
			pRecord->asDouble(MLR_VAR_R2_ADJ) * 100.0,
			pRecord->asDouble(MLR_VAR_SE    ),
			pRecord->asDouble(MLR_VAR_T     ),
			pRecord->asDouble(MLR_VAR_SIG   ) * 100.0,
			pRecord->asDouble(MLR_VAR_RCOEFF),
			pRecord->asString(MLR_VAR_NAME  )
		);
	}

	//-----------------------------------------------------
	s	+= SG_T("\n");

	s	+= CSG_String::Format(SG_T("%s: %f (%s: %d)\n"), LNG("Residual standard error"), Get_StdError(), LNG("degrees of freedom"), Get_DegFreedom());
	s	+= CSG_String::Format(SG_T("%s: %f (%s: %f)\n"), LNG("Multiple R-squared"), 100.0 * Get_R2(), LNG("adjusted"), 100.0 * Get_R2_Adj());
	s	+= CSG_String::Format(SG_T("%s: %f (%d/%d DF), %s: %g\n"), LNG("F-statistic"), Get_F(), Get_nPredictors(), Get_DegFreedom(), LNG("p-value"), Get_P());

	//-----------------------------------------------------
	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression_Multiple::__Get_Forward(const CSG_Matrix &Samples, double p_in)
{
	int		nIndependents	= Samples.Get_NX() - 1;
	int		nSamples		= Samples.Get_NY();

	//-----------------------------------------------------
	int			i;
	double		r2, r2_tot, dr2, p;
	CSG_Matrix	z	= Samples.Get_Transpose();

	//-------------------------------------------------
	for(i=0, r2_tot=0.0, m_nPredictors=0; i<nIndependents && i==m_nPredictors; i++)
	{
		if( __Get_Forward(nSamples, nIndependents, z.Get_Data() + 1, z[0], m_Predictor[i], r2) )
		{
			dr2	= r2 * (1.0 - r2_tot);

			if( p_in > (p = _Get_P(1, nSamples, r2_tot + dr2, r2_tot)) )
			{
				m_nPredictors++;

				r2_tot	+= dr2;

				CSG_Table_Record	*pRecord	= m_pSteps->Add_Record();

				pRecord->Set_Value(MLR_STEP_NR		, m_nPredictors);
				pRecord->Set_Value(MLR_STEP_R		, sqrt(r2_tot));
				pRecord->Set_Value(MLR_STEP_R2		, r2_tot);
				pRecord->Set_Value(MLR_STEP_R2_ADJ	, SG_Regression_Get_Adjusted_R2(r2_tot, nSamples, m_nPredictors));
				pRecord->Set_Value(MLR_STEP_VAR_F	, _Get_F(1, nSamples, r2_tot + dr2, r2_tot));
				pRecord->Set_Value(MLR_STEP_VAR_SIG	, p);
				pRecord->Set_Value(MLR_STEP_DIR		, SG_T(">>"));
				pRecord->Set_Value(MLR_STEP_VAR		, m_Names[1 + m_Predictor[i]]);
			}
		}
	}

	//-------------------------------------------------
	if( m_nPredictors > 0 )
	{
		CSG_Matrix	X;

		X.Add_Col(Samples.Get_Col(0));	// Dependent

		for(i=0; i<m_nPredictors; i++)	// Independents
		{
			X.Add_Col(Samples.Get_Col(1 + m_Predictor[i]));
		}

		_Set_Step_Info(X);
	}

	//-------------------------------------------------
	return( m_nPredictors > 0 );
}

//---------------------------------------------------------
bool CSG_Regression_Multiple::__Get_Forward(int nSamples, int nPredictors, double **X, double *Y, int &iMax, double &rMax)
{
	int				i, n;
	double			*XMax;
	CSG_Regression	r;

	//-----------------------------------------------------
	for(i=0, n=0, iMax=-1, rMax=0.0; i<nPredictors; i++)
	{
		if( X[i] && r.Calculate(nSamples, X[i], Y) )
		{
			n++;

			if( iMax < 0 || rMax < r.Get_R2() )
			{
				iMax	= i;
				rMax	= r.Get_R2();
			}
		}
	}

	//-----------------------------------------------------
	if( n > 1 )
	{
		XMax	= X[iMax];
		X[iMax]	= NULL;

		for(i=0; i<nPredictors; i++)
		{
			if( X[i] )
			{
				__Eliminate(nSamples, XMax, X[i]);
			}
		}

		__Eliminate(nSamples, XMax, Y);
	}

	return( iMax >= 0 );
}

//---------------------------------------------------------
bool CSG_Regression_Multiple::__Eliminate(int nSamples, double *X, double *Y)
{
	CSG_Regression	r;

	if( r.Calculate(nSamples, X, Y) )
	{
		for(int i=0; i<nSamples; i++)
		{
			Y[i]	-= r.Get_Constant() + r.Get_Coefficient() * X[i];
		}

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
