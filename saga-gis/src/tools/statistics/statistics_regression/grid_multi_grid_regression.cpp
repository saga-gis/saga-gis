
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 statistics_regression                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              grid_multi_grid_regression.cpp           //
//                                                       //
//                 Copyright (C) 2011 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid_multi_grid_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Multi_Grid_Regression::CGrid_Multi_Grid_Regression(void)
{
	Set_Name		(_TL("Multiple Regression Analysis (Grid and Predictor Grids)"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Linear regression analysis of one grid as dependent and multiple grids as independent (predictor) variables. "
		"Details of the regression/correlation analysis will be saved to a table. "
		"Optionally the regression model is used to create a new grid with regression based values. "
		"The multiple regression analysis uses a forward selection procedure. "
	));

	Add_Reference("Bahrenberg, G., Giese, E., Nipper, J.", "1992",
		"Statistische Methoden in der Geographie 2 - Multivariate Statistik",
		"Stuttgart, 415p."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEPENDENT"		, _TL("Dependent Variable"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid_List("",
		"PREDICTORS"	, _TL("Predictors"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid("",
		"REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"RESIDUALS"		, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table("", "INFO_COEFF", CSG_String::Format("%s: %s", _TL("Details"), _TL("Coefficients")), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Table("", "INFO_MODEL", CSG_String::Format("%s: %s", _TL("Details"), _TL("Model"       )), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Table("", "INFO_STEPS", CSG_String::Format("%s: %s", _TL("Details"), _TL("Steps"       )), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Choice("",
		"RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Bool("", "COORD_X", _TL("Include X Coordinate"), _TL(""), false);
	Parameters.Add_Bool("", "COORD_Y", _TL("Include Y Coordinate"), _TL(""), false);

	Parameters.Add_Choice("",
		"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("include all"),
			_TL("forward"),
			_TL("backward"),
			_TL("stepwise")
		), 3
	);

	Parameters.Add_Double("",
		"P_VALUE"		, _TL("Significance Level"),
		_TL("Significance level (aka p-value) as threshold for automated predictor selection, given as percentage"),
		5., 0., true, 100., true
	);

	Parameters.Add_Choice("",
		"CROSSVAL"		, _TL("Cross Validation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("none"),
			_TL("leave one out"),
			_TL("2-fold"),
			_TL("k-fold")
		), 0
	);

	Parameters.Add_Int("",
		"CROSSVAL_K"	, _TL("Cross Validation Subsamples"),
		_TL("number of subsamples for k-fold cross validation"),
		10, 2, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Multi_Grid_Regression::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("CROSSVAL") )
	{
		pParameters->Set_Enabled("CROSSVAL_K", pParameter->asInt() == 3);	// k-fold
	}

	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("P_VALUE", pParameter->asInt() > 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Multi_Grid_Regression::On_Execute(void)
{
	CSG_Grid                *pDependent  = Parameters("DEPENDENT" )->asGrid    ();
	CSG_Parameter_Grid_List	*pPredictors = Parameters("PREDICTORS")->asGridList();

	//-----------------------------------------------------
	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: m_Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: m_Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: m_Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: m_Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	CSG_Matrix	Samples;	CSG_Strings	Names;

	if( !Get_Samples(pDependent, pPredictors, Samples, Names) )
	{
		Error_Set(_TL("Sample collection failed"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Regression_Multiple	Regression;

	bool	bResult;	double	p	= Parameters("P_VALUE")->asDouble() / 100.;

	switch( Parameters("METHOD")->asInt() )
	{
	default: bResult = Regression.Get_Model         (Samples      , &Names); break;
	case  1: bResult = Regression.Get_Model_Forward (Samples, p   , &Names); break;
	case  2: bResult = Regression.Get_Model_Backward(Samples,    p, &Names); break;
	case  3: bResult = Regression.Get_Model_Stepwise(Samples, p, p, &Names); break;
	}

	Message_Add(Regression.Get_Info(), false);

	if( bResult == false )
	{
		Error_Set(_TL("Regression analysis failed"));

		return( false );
	}

	//-----------------------------------------------------
	int	CrossVal;

	switch( Parameters("CROSSVAL")->asInt() )
	{
	default: CrossVal = 0                                ; break; // none
	case  1: CrossVal = 1                                ; break; // leave one out (LOOVC)
	case  2: CrossVal = 2                                ; break; // 2-fold
	case  3: CrossVal = Parameters("CROSSVAL_K")->asInt(); break; // k-fold
	}

	if( CrossVal > 0 && Regression.Get_CrossValidation(CrossVal) )
	{
		Message_Fmt("\n%s:", _TL("Cross Validation"));
		Message_Fmt("\n\t%s:\t%s"  , _TL("Type"   ), Parameters("CROSSVAL")->asString() );
		Message_Fmt("\n\t%s:\t%d"  , _TL("Samples"), Regression.Get_CV_nSamples()       );
		Message_Fmt("\n\t%s:\t%f"  , _TL("RMSE"   ), Regression.Get_CV_RMSE    ()       );
		Message_Fmt("\n\t%s:\t%.2f", _TL("NRMSE"  ), Regression.Get_CV_NRMSE   () * 100.);
		Message_Fmt("\n\t%s:\t%.2f", _TL("R2"     ), Regression.Get_CV_R2      () * 100.);
	}

	//-----------------------------------------------------
	if( Parameters("INFO_COEFF")->asTable() )
	{
		Parameters("INFO_COEFF")->asTable()->Assign(Regression.Get_Info_Regression());
		Parameters("INFO_COEFF")->asTable()->Set_Name(_TL("Regression Coefficients"));
	}

	if( Parameters("INFO_MODEL")->asTable() )
	{
		Parameters("INFO_MODEL")->asTable()->Assign(Regression.Get_Info_Model());
		Parameters("INFO_MODEL")->asTable()->Set_Name(_TL("Regression Model"));
	}

	if( Parameters("INFO_STEPS")->asTable() )
	{
		Parameters("INFO_STEPS")->asTable()->Assign(Regression.Get_Info_Steps());
		Parameters("INFO_STEPS")->asTable()->Set_Name(_TL("Regression Steps"));
	}

	//-----------------------------------------------------
	CSG_Grid	*pRegression	= Parameters("REGRESSION")->asGrid();

	pRegression->Fmt_Name("%s [OLS]", pDependent->Get_Name());

	Set_Regression(pRegression, pPredictors, Regression);

	Set_Residuals(pDependent, pRegression, Parameters("RESIDUALS")->asGrid());

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Multi_Grid_Regression::Get_Samples(CSG_Grid *pDependent, CSG_Parameter_Grid_List *pPredictors, CSG_Matrix &Samples, CSG_Strings &Names)
{
	int	nValues = 1 + pPredictors->Get_Grid_Count();

	int	xCoord = Parameters("COORD_X")->asBool() ? nValues++ : 0;
	int	yCoord = Parameters("COORD_X")->asBool() ? nValues++ : 0;

	CSG_Vector	Sample(nValues);

	Names	+= pDependent->Get_Name();	// dependent variable

	for(int i=0; i<pPredictors->Get_Grid_Count(); i++)	// predictors
	{
		Names	+= pPredictors->Get_Grid(i)->Get_Name();
	}

	if( xCoord )	{	Names	+= "X";	}
	if( yCoord )	{	Names	+= "Y";	}

	//-----------------------------------------------------
	for(int y=0; y<pDependent->Get_NY() && Set_Progress(y, pDependent->Get_NY()); y++)
	{
		double	py	= pDependent->Get_YMin() + y * pDependent->Get_Cellsize();

		for(int x=0; x<pDependent->Get_NX(); x++)
		{
			if( !pDependent->is_NoData(x, y) )
			{
				double	px	= pDependent->Get_XMin() + x * pDependent->Get_Cellsize();

				bool	bOkay = true;

				for(int i=0; bOkay && i<pPredictors->Get_Grid_Count(); i++)
				{
					bOkay	= pPredictors->Get_Grid(i)->Get_Value(px, py, Sample[1 + i], m_Resampling);
				}

				if( bOkay )
				{
					if( xCoord )	{	Sample[xCoord]	= px;	}
					if( yCoord )	{	Sample[yCoord]	= py;	}

					Sample[0]	= pDependent->asDouble(x, y);

					Samples.Add_Row(Sample);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( Samples.Get_NRows() >= pPredictors->Get_Grid_Count() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Multi_Grid_Regression::Set_Regression(CSG_Grid *pRegression, CSG_Parameter_Grid_List *pPredictors, CSG_Regression_Multiple &Regression)
{
	CSG_Array_Pointer Grids; int xCoord = 0, yCoord = 0;

	for(int i=0; i<Regression.Get_nPredictors(); i++)
	{
		if( Regression.Get_Predictor(i) < pPredictors->Get_Grid_Count() )
		{
			Grids	+= pPredictors->Get_Grid(Regression.Get_Predictor(i));
		}
		else if( Regression.Get_Predictor(i) == pPredictors->Get_Grid_Count() && Parameters("COORD_X")->asBool() )
		{
			xCoord	= 1;
		}
		else // if( Regression.Get_Predictor(iGrid) > pGrids->Get_Count() || Parameters("COORD_X")->asBool() == false )
		{
			yCoord	= 1;
		}
	}

	int	nValues	= Grids.Get_Size();

	if( xCoord )	{	xCoord	= nValues++;	}
	if( yCoord )	{	yCoord	= nValues++;	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		double	py	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<pRegression->Get_NX(); x++)
		{
			double	px	= Get_XMin() + x * Get_Cellsize();

			double	z	= Regression.Get_RConst(), pz;

			bool	bOkay	= true;

			for(int i=0; bOkay && i<Grids.Get_Size(); i++)
			{
				if( (bOkay = ((CSG_Grid *)Grids[i])->Get_Value(px, py, pz, m_Resampling)) == true )
				{
					z	+= Regression.Get_RCoeff(i) * pz;
				}
			}

			//---------------------------------------------
			if( bOkay )
			{
				if( xCoord )
				{
					z	+= Regression.Get_RCoeff(xCoord) * px;
				}

				if( yCoord )
				{
					z	+= Regression.Get_RCoeff(yCoord) * py;
				}

				pRegression->Set_Value(x, y, z);
			}
			else
			{
				pRegression->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Multi_Grid_Regression::Set_Residuals(CSG_Grid *pDependent, CSG_Grid *pRegression, CSG_Grid *pResiduals)
{
	if( !pResiduals )
	{
		return( false );
	}

	pResiduals->Fmt_Name("%s [OLS %s]", pDependent->Get_Name(), _TL("Residuals"));

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		double	py	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	px	= Get_XMin() + x * Get_Cellsize(), pz;

			if( !pRegression->is_NoData(x, y) && pDependent->Get_Value(px, py, pz, m_Resampling) )
			{
				pResiduals->Set_Value(x, y, pz - pRegression->asDouble(x, y));
			}
			else
			{
				pResiduals->Set_NoData(x, y);
			}
		}
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
