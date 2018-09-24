/**********************************************************
 * Version $Id: point_multi_grid_regression.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//             point_multi_grid_regression.cpp           //
//                                                       //
//                 Copyright (C) 2004 by                 //
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
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "point_multi_grid_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoint_Multi_Grid_Regression::CPoint_Multi_Grid_Regression(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Multiple Regression Analysis (Points and Predictor Grids)"));

	Set_Author		("O.Conrad (c) 2004");

	Set_Description	(_TW(
		"Linear regression analysis of point attributes with multiple grids. "
		"Details of the regression/correlation analysis will be saved to a table. "
		"The regression function is used to create a new grid with regression based values. "
		"The multiple regression analysis uses a forward selection procedure."
	));

	Add_Reference(
		"Bahrenberg, G., Giese, E., Nipper, J.", "1992",
		"Statistische Methoden in der Geographie 2 - Multivariate Statistik",
		"Stuttgart, 415p."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"PREDICTORS"	, _TL("Predictors"),
		_TL(""),
		PARAMETER_INPUT, true
	);

	Parameters.Add_Shapes("",
		"POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("POINTS",
		"ATTRIBUTE"		, _TL("Dependent Variable"),
		_TL("")
	);

	Parameters.Add_Table("",
		"INFO_COEFF"	, _TL("Details: Coefficients"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table("",
		"INFO_MODEL"	, _TL("Details: Model"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table("",
		"INFO_STEPS"	, _TL("Details: Steps"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes("",
		"RESIDUALS"		, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid("",
		"REGRESSION"	, _TL("Regression"),
		_TL("regression model applied to predictor grids"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"REGRESCORR"	, _TL("Regression with Residual Correction"),
		_TL("regression model applied to predictor grids with interpolated residuals added"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Bool("",
		"COORD_X"		, _TL("Include X Coordinate"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"COORD_Y"		, _TL("Include Y Coordinate"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"INTERCEPT"		, _TL("Intercept"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("include all"),
			_TL("forward"),
			_TL("backward"),
			_TL("stepwise")
		), 3
	);

	Parameters.Add_Double("",
		"P_VALUE"		, _TL("Significance Level"),
		_TL("Significance level (aka p-value) as threshold for automated predictor selection, given as percentage"),
		5.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Choice("",
		"CROSSVAL"		, _TL("Cross Validation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
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

	Parameters.Add_Choice("",
		"RESIDUAL_COR"	, _TL("Residual Interpolation"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Multilevel B-Spline Interpolation"),
			_TL("Inverse Distance Weighted")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPoint_Multi_Grid_Regression::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("CROSSVAL") )
	{
		pParameters->Set_Enabled("CROSSVAL_K", pParameter->asInt() == 3);	// k-fold
	}

	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("P_VALUE", pParameter->asInt() > 0);
	}

	if(	pParameter->Cmp_Identifier("REGRESCORR") )
	{
		pParameters->Set_Enabled("RESIDUAL_COR", pParameter->asPointer() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::On_Execute(void)
{
	bool					bResult;
	int						iAttribute;
	double					P;
	CSG_Strings				Names;
	CSG_Matrix				Samples;
	CSG_Shapes				*pPoints;
	CSG_Grid				*pRegression;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids		= Parameters("PREDICTORS")->asGridList();
	pRegression	= Parameters("REGRESSION")->asGrid();
	pPoints		= Parameters("POINTS"    )->asShapes();
	iAttribute	= Parameters("ATTRIBUTE" )->asInt();
	P			= Parameters("P_VALUE"   )->asDouble() / 100.0;

	//-----------------------------------------------------
	if( !Get_Samples(pGrids, pPoints, iAttribute, Samples, Names) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Regression.Set_With_Intercept(Parameters("INTERCEPT")->asBool());

	switch( Parameters("METHOD")->asInt() )
	{
	default: bResult = m_Regression.Get_Model         (Samples      , &Names);	break;
	case  1: bResult = m_Regression.Get_Model_Forward (Samples, P   , &Names);	break;
	case  2: bResult = m_Regression.Get_Model_Backward(Samples,    P, &Names);	break;
	case  3: bResult = m_Regression.Get_Model_Stepwise(Samples, P, P, &Names);	break;
	}

	if( bResult == false )
	{
		Error_Set(_TL("regression analysis could not detect a significant predictor"));

		return( false );
	}

	//-----------------------------------------------------
	Message_Add(m_Regression.Get_Info(), false);

	//-----------------------------------------------------
	int	CrossVal;

	switch( Parameters("CROSSVAL")->asInt() )
	{
	default: CrossVal = 0;                                 break;	// none
	case  1: CrossVal = 1;                                 break;	// leave one out (LOOVC)
	case  2: CrossVal = 2;                                 break;	// 2-fold
	case  3: CrossVal = Parameters("CROSSVAL_K")->asInt(); break;	// k-fold
	}

	if( CrossVal > 0 && m_Regression.Get_CrossValidation(CrossVal) )
	{
		Message_Fmt("\n%s:", _TL("Cross Validation"));
		Message_Fmt("\n\t%s:\t%s"  , _TL("Type"   ), Parameters("CROSSVAL")->asString() );
		Message_Fmt("\n\t%s:\t%d"  , _TL("Samples"), m_Regression.Get_CV_nSamples()     );
		Message_Fmt("\n\t%s:\t%f"  , _TL("RMSE"   ), m_Regression.Get_CV_RMSE()         );
		Message_Fmt("\n\t%s:\t%.2f", _TL("NRMSE"  ), m_Regression.Get_CV_NRMSE() * 100.0);
		Message_Fmt("\n\t%s:\t%.2f", _TL("R2"     ), m_Regression.Get_CV_R2()    * 100.0);
	}

	//-----------------------------------------------------
	Set_Regression(pGrids, pRegression, CSG_String::Format("%s.%s [%s]", pPoints->Get_Name(), Parameters("ATTRIBUTE")->asString(), _TL("Regression")));

	//-----------------------------------------------------
	if( Parameters("INFO_COEFF")->asTable() )
	{
		Parameters("INFO_COEFF")->asTable()->Assign(m_Regression.Get_Info_Regression());
		Parameters("INFO_COEFF")->asTable()->Set_Name(_TL("MLRA Coefficients"));
	}

	if( Parameters("INFO_MODEL")->asTable() )
	{
		Parameters("INFO_MODEL")->asTable()->Assign(m_Regression.Get_Info_Model());
		Parameters("INFO_MODEL")->asTable()->Set_Name(_TL("MLRA Model"));
	}

	if( Parameters("INFO_STEPS")->asTable() )
	{
		Parameters("INFO_STEPS")->asTable()->Assign(m_Regression.Get_Info_Steps());
		Parameters("INFO_STEPS")->asTable()->Set_Name(_TL("MLRA Steps"));
	}

	//-----------------------------------------------------
	Set_Residuals(Parameters("RESIDUALS")->asShapes());

	Set_Residual_Corr(pRegression, Parameters("RESIDUALS")->asShapes(), Parameters("REGRESCORR")->asGrid());

	m_Regression.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::Get_Samples(CSG_Parameter_Grid_List *pGrids, CSG_Shapes *pPoints, int iAttribute, CSG_Matrix &Samples, CSG_Strings &Names)
{
	int			iGrid;
	double		zGrid;
	CSG_Vector	Sample;

	//-----------------------------------------------------
	bool	bCoord_X	= Parameters("COORD_X")->asBool();
	bool	bCoord_Y	= Parameters("COORD_Y")->asBool();

	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1: Resampling	= GRID_RESAMPLING_Bilinear        ;	break;
	case  2: Resampling	= GRID_RESAMPLING_BicubicSpline   ;	break;
	case  3: Resampling	= GRID_RESAMPLING_BSpline         ;	break;
	}

	Names	+= pPoints->Get_Field_Name(iAttribute);		// Dependent Variable

	for(iGrid=0; iGrid<pGrids->Get_Grid_Count(); iGrid++)	// Independent Variables
	{
		Names	+= pGrids->Get_Grid(iGrid)->Get_Name();
	}

	if( bCoord_X )	{	Names	+= "X";	}
	if( bCoord_Y )	{	Names	+= "Y";	}

	Sample.Create(1 + pGrids->Get_Grid_Count() + (bCoord_X ? 1 : 0) + (bCoord_Y ? 1 : 0));

	//-----------------------------------------------------
	for(int iShape=0; iShape<pPoints->Get_Count() && Set_Progress(iShape, pPoints->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pPoints->Get_Shape(iShape);

		if( !pShape->is_NoData(iAttribute) )
		{
			Sample[0]	= pShape->asDouble(iAttribute);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					bool		bAdd	= true;
					TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

					for(iGrid=0; iGrid<pGrids->Get_Grid_Count() && bAdd; iGrid++)
					{
						if( pGrids->Get_Grid(iGrid)->Get_Value(Point, zGrid, Resampling) )
						{
							Sample[1 + iGrid]	= zGrid;
						}
						else
						{
							bAdd	= false;
						}
					}

					if( bAdd )
					{
						if( bCoord_X )	{	Sample[1 + iGrid++]	= Point.x;	}
						if( bCoord_Y )	{	Sample[1 + iGrid++]	= Point.y;	}

						Samples.Add_Row(Sample);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( Samples.Get_NRows() >= pGrids->Get_Grid_Count() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::Set_Regression(CSG_Parameter_Grid_List *pGrids, CSG_Grid *pRegression, const CSG_String &Name)
{
	if( !pRegression )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			iGrid, nGrids, x, y;
	TSG_Point	p;

	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	Resampling	= GRID_RESAMPLING_Bilinear;			break;
	case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline;	break;
	case  3:	Resampling	= GRID_RESAMPLING_BSpline;			break;
	}

	CSG_Grid	**ppGrids	= (CSG_Grid **)SG_Malloc(m_Regression.Get_nPredictors() * sizeof(CSG_Grid *));

	int	iCoord_X	= -1;
	int	iCoord_Y	= -1;

	for(iGrid=0, nGrids=0; iGrid<m_Regression.Get_nPredictors(); iGrid++)
	{
		if( m_Regression.Get_Predictor(iGrid) < pGrids->Get_Grid_Count() )
		{
			ppGrids[nGrids++]	= pGrids->Get_Grid(m_Regression.Get_Predictor(iGrid));
		}
		else if( m_Regression.Get_Predictor(iGrid) == pGrids->Get_Grid_Count() && Parameters("COORD_X")->asBool() )
		{
			iCoord_X = iGrid;
		}
		else // if( m_Regression.Get_Predictor(iGrid) > pGrids->Get_Count() || Parameters("COORD_X")->asBool() == false )
		{
			iCoord_Y = iGrid;
		}
	}

	pRegression->Set_Name(Name);

	//-----------------------------------------------------
	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			bool	bOkay;
			double	z	= m_Regression.Get_RConst();

			for(iGrid=0, bOkay=true; bOkay && iGrid<nGrids; iGrid++)
			{
				double	zGrid;

				if( ppGrids[iGrid]->Get_Value(p, zGrid, Resampling) )
				{
					z	+= m_Regression.Get_RCoeff(iGrid) * zGrid;
				}
				else
				{
					bOkay	= false;
				}
			}

			//---------------------------------------------
			if( bOkay )
			{
				if( iCoord_X >= 0 )
				{
					z	+= m_Regression.Get_RCoeff(iCoord_X) * p.x;
				}

				if( iCoord_Y >= 0 )
				{
					z	+= m_Regression.Get_RCoeff(iCoord_Y) * p.y;
				}

				pRegression->Set_Value (x, y, z);
			}
			else
			{
				pRegression->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(ppGrids);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::Set_Residuals(CSG_Shapes *pResiduals)
{
	if( !pResiduals )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pPoints		= Parameters("POINTS"    )->asShapes();
	CSG_Grid	*pRegression	= Parameters("REGRESSION")->asGrid();
	int			iAttribute		= Parameters("ATTRIBUTE" )->asInt();

	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	Resampling	= GRID_RESAMPLING_Bilinear;			break;
	case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline;	break;
	case  3:	Resampling	= GRID_RESAMPLING_BSpline;			break;
	}

	//-----------------------------------------------------
	pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format("%s.%s [%s]", pPoints->Get_Name(), Parameters("ATTRIBUTE")->asString(), _TL("Residuals")));
	pResiduals->Add_Field(pPoints->Get_Field_Name(iAttribute), SG_DATATYPE_Double);
	pResiduals->Add_Field("TREND"	, SG_DATATYPE_Double);
	pResiduals->Add_Field("RESIDUAL", SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(int iShape=0; iShape<pPoints->Get_Count() && Set_Progress(iShape, pPoints->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pPoints->Get_Shape(iShape);

		if( !pShape->is_NoData(iAttribute) )
		{
			double	zShape	= pShape->asDouble(iAttribute);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					double		zGrid;
					TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

					if( pRegression->Get_Value(Point, zGrid, Resampling) )
					{
						CSG_Shape	*pResidual	= pResiduals->Add_Shape();

						pResidual->Add_Point(Point);
						pResidual->Set_Value(0, zShape);
						pResidual->Set_Value(1, zGrid);
						pResidual->Set_Value(2, zShape - zGrid);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::Set_Residual_Corr(CSG_Grid *pRegression, CSG_Shapes *pResiduals, CSG_Grid *pCorrection)
{
	//-----------------------------------------------------
	if( !pCorrection )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	Residuals;

	if( !pResiduals )
	{
		if( !Set_Residuals(&Residuals) )
		{
			return( false );
		}

		pResiduals	= &Residuals;
	}

	//-----------------------------------------------------
	switch( Parameters("RESIDUAL_COR")->asInt() )
	{
	default:	// Multlevel B-Spline Interpolation
		SG_RUN_TOOL_ExitOnError("grid_spline", 4,
				SG_TOOL_PARAMETER_SET("SHAPES"           , pResiduals)
			&&  SG_TOOL_PARAMETER_SET("FIELD"            , 2)
			&&  SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
			&&  SG_TOOL_PARAMETER_SET("TARGET_OUT_GRID"  , pCorrection)
		);
		break;

	case  1:	// Inverse Distance Weighted
		SG_RUN_TOOL_ExitOnError("grid_gridding", 1,
				SG_TOOL_PARAMETER_SET("SHAPES"           , pResiduals)
			&&  SG_TOOL_PARAMETER_SET("FIELD"            , 2)
			&&  SG_TOOL_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
			&&  SG_TOOL_PARAMETER_SET("TARGET_OUT_GRID"  , pCorrection)
			&&  SG_TOOL_PARAMETER_SET("SEARCH_RANGE"     , 1)	// global
			&&  SG_TOOL_PARAMETER_SET("SEARCH_POINTS_ALL", 1)	// all points within search distance
		);
		break;
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pRegression->is_NoData(x, y) || pCorrection->is_NoData(x, y) )
			{
				pCorrection->Set_NoData(x, y);
			}
			else
			{
				pCorrection->Add_Value(x, y, pRegression->asDouble(x, y));
			}
		}
	}

	pCorrection->Set_Name(CSG_String::Format("%s.%s [%s]", Parameters("POINTS")->asShapes()->Get_Name(), Parameters("ATTRIBUTE")->asString(), _TL("Residual Corrected Regression")));

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

//	//-----------------------------------------------------
//	Parameters.Add_Choice(
//		NULL	,"CORRECTION"	, _TL("Adjustment"),
//		_TL(""),
//		CSG_String::Format("%s|%s|%s|%s|%s|%s|",
//			_TL("Smith"),
//			_TL("Wherry 1"),
//			_TL("Wherry 2"),
//			_TL("Olkin & Pratt"),
//			_TL("Pratt"),
//			_TL("Claudy 3")
//		), 1
//	);
//
//	TSG_Regression_Correction	m_Correction;
//
//	switch( Parameters("CORRECTION")->asInt() )
//	{
//	case 0:	m_Correction	= REGRESSION_CORR_Smith;		break;
//	case 1:	m_Correction	= REGRESSION_CORR_Wherry_1;		break;
//	case 2:	m_Correction	= REGRESSION_CORR_Wherry_2;		break;
//	case 3:	m_Correction	= REGRESSION_CORR_Olkin_Pratt;	break;
//	case 4:	m_Correction	= REGRESSION_CORR_Pratt;		break;
//	case 5:	m_Correction	= REGRESSION_CORR_Claudy_3;		break;
//	}
//


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
