/**********************************************************
 * Version $Id: kriging_regression.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 Geostatistics_Kriging                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 kriging_regression.cpp                //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include "kriging_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Regression::CKriging_Regression(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Regression Kriging"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Regression Kriging for grid interpolation from irregular sample points."
	));

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "PREDICTORS"	, _TL("Predictors"),
		_TL(""),
		PARAMETER_INPUT, true
	);

	Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		_TL("regression model applied to predictor grids"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "PREDICTION"	, _TL("Prediction"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESIDUALS"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "VARIANCE"	, _TL("Quality Measure"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "TQUALITY"	, _TL("Type of Quality Measure"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("standard deviation"),
			_TL("variance")
		), 0
	);

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "LOG"			, _TL("Logarithmic Transformation"),
		_TL(""),
		PARAMETER_TYPE_Bool
	);

	pNode	= Parameters.Add_Value(
		NULL	, "BLOCK"		, _TL("Block Kriging"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		pNode	, "DBLOCK"		, _TL("Block Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	if( !SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Value(
			NULL	, "VAR_MAXDIST"		, _TL("Maximum Distance"),
			_TL(""),
			PARAMETER_TYPE_Double, -1.0
		);

		Parameters.Add_Value(
			NULL	, "VAR_NCLASSES"	, _TL("Lag Distance Classes"),
			_TL("initial number of lag distance classes"),
			PARAMETER_TYPE_Int, 100, 1, true
		);

		Parameters.Add_Value(
			NULL	, "VAR_NSKIP"		, _TL("Skip"),
			_TL(""),
			PARAMETER_TYPE_Int, 1, 1, true
		);

		Parameters.Add_String(
			NULL	, "VAR_MODEL"		, _TL("Model"),
			_TL(""),
			SG_T("a + b * x")
		);
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Parameters.Add_Table(NULL	, "INFO_COEFF"	, _TL("Regression: Coefficients"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Table(NULL	, "INFO_MODEL"	, _TL("Regression: Model"       ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Table(NULL	, "INFO_STEPS"	, _TL("Regression: Steps"       ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	pNode	= Parameters.Add_Node(
		NULL	, "NODE_REG",	_TL("Regression"),
		_TL("")
	);

	Parameters.Add_Value(pNode	, "COORD_X"		, _TL("Include X Coordinate"), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode	, "COORD_Y"		, _TL("Include Y Coordinate"), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode	, "INTERCEPT"	, _TL("Intercept"           ), _TL(""), PARAMETER_TYPE_Bool, true );

	Parameters.Add_Choice(
		pNode	,"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("include all"),
			_TL("forward"),
			_TL("backward"),
			_TL("stepwise")
		), 3
	);

	Parameters.Add_Value(
		pNode	, "P_VALUE"		, _TL("Significance Level"),
		_TL("Significance level (aka p-value) as threshold for automated predictor selection, given as percentage"),
		PARAMETER_TYPE_Double, 5.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Choice(
		pNode	,"INTERPOL"		, _TL("Grid Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	m_Search.Create(&Parameters, Parameters.Add_Node(NULL, "NODE_SEARCH", _TL("Search Options"), _TL("")), 16);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CKriging_Regression::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "POINTS") )
	{
		m_Search.On_Parameter_Changed(pParameters, pParameter);
	}

	//-----------------------------------------------------
	return( CSG_Module_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CKriging_Regression::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Search.On_Parameters_Enable(pParameters, pParameter);

	//-----------------------------------------------------
	return( CSG_Module_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Regression::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	Points(SHAPE_TYPE_Point);

	CSG_Grid	*pPrediction	= Parameters("PREDICTION")->asGrid();
	CSG_Grid	*pRegression	= Parameters("REGRESSION")->asGrid();
	CSG_Grid	*pResiduals		= Parameters("RESIDUALS" )->asGrid();
	CSG_Grid	*pVariance		= Parameters("VARIANCE"  )->asGrid();

	//-----------------------------------------------------
	if( !pResiduals )
	{
		pResiduals	= pPrediction;
	}

	//-----------------------------------------------------
	SG_RUN_MODULE_ExitOnError("statistics_regression", 1,	// Multiple Regression Analysis (Points and Predictor Grids)
			SG_MODULE_PARAMETER_SET("PREDICTORS", Parameters("PREDICTORS"))
		&&	SG_MODULE_PARAMETER_SET("POINTS"    , Parameters("POINTS"    ))
		&&	SG_MODULE_PARAMETER_SET("ATTRIBUTE" , Parameters("FIELD"     ))
		&&	SG_MODULE_PARAMETER_SET("INFO_COEFF", Parameters("INFO_COEFF"))
		&&	SG_MODULE_PARAMETER_SET("INFO_MODEL", Parameters("INFO_MODEL"))
		&&	SG_MODULE_PARAMETER_SET("INFO_STEPS", Parameters("INFO_STEPS"))
		&&	SG_MODULE_PARAMETER_SET("INTERPOL"  , Parameters("INTERPOL"  ))
		&&	SG_MODULE_PARAMETER_SET("COORD_X"   , Parameters("COORD_X"   ))
		&&	SG_MODULE_PARAMETER_SET("COORD_Y"   , Parameters("COORD_Y"   ))
		&&	SG_MODULE_PARAMETER_SET("INTERCEPT" , Parameters("INTERCEPT" ))
		&&	SG_MODULE_PARAMETER_SET("METHOD"    , Parameters("METHOD"    ))
		&&	SG_MODULE_PARAMETER_SET("P_VALUE"   , Parameters("P_VALUE"   ))
		&&	SG_MODULE_PARAMETER_SET("REGRESSION", pRegression)
		&&	SG_MODULE_PARAMETER_SET("RESIDUALS" , &Points )
	);

	//-----------------------------------------------------
	if( !SG_UI_Get_Window_Main() )	// saga_cmd
	{
		SG_RUN_MODULE_ExitOnError("statistics_kriging", 0,	// Ordinary Kriging
			   SG_MODULE_PARAMETER_SET("POINTS"           , &Points)
			&& SG_MODULE_PARAMETER_SET("FIELD"            , 2)
			&& SG_MODULE_PARAMETER_SET("LOG"              , Parameters("LOG"              ))
			&& SG_MODULE_PARAMETER_SET("BLOCK"            , Parameters("BLOCK"            ))
			&& SG_MODULE_PARAMETER_SET("DBLOCK"           , Parameters("DBLOCK"           ))
			&& SG_MODULE_PARAMETER_SET("SEARCH_RANGE"     , Parameters("SEARCH_RANGE"     ))
			&& SG_MODULE_PARAMETER_SET("SEARCH_RADIUS"    , Parameters("SEARCH_RADIUS"    ))
			&& SG_MODULE_PARAMETER_SET("SEARCH_POINTS_ALL", Parameters("SEARCH_POINTS_ALL"))
			&& SG_MODULE_PARAMETER_SET("SEARCH_POINTS_MIN", Parameters("SEARCH_POINTS_MIN"))
			&& SG_MODULE_PARAMETER_SET("SEARCH_POINTS_MAX", Parameters("SEARCH_POINTS_MAX"))
			&& SG_MODULE_PARAMETER_SET("SEARCH_DIRECTION" , Parameters("SEARCH_DIRECTION" ))
			&& SG_MODULE_PARAMETER_SET("VAR_MAXDIST"      , Parameters("VAR_MAXDIST"      ))
			&& SG_MODULE_PARAMETER_SET("VAR_NCLASSES"     , Parameters("VAR_NCLASSES"     ))
			&& SG_MODULE_PARAMETER_SET("VAR_NSKIP"        , Parameters("VAR_NSKIP"        ))
			&& SG_MODULE_PARAMETER_SET("VAR_MODEL"        , Parameters("VAR_MODEL"        ))
			&& SG_MODULE_PARAMETER_SET("TARGET_DEFINITION", 1)	// grid or grid system
			&& SG_MODULE_PARAMETER_SET("TARGET_PREDICTION", pResiduals)
			&& SG_MODULE_PARAMETER_SET("TARGET_VARIANCE"  , pVariance )
		);
	}
	else
	{
		SG_RUN_MODULE_ExitOnError("statistics_kriging", 0,	// Ordinary Kriging
			   SG_MODULE_PARAMETER_SET("POINTS"           , &Points)
			&& SG_MODULE_PARAMETER_SET("FIELD"            , 2)
			&& SG_MODULE_PARAMETER_SET("LOG"              , Parameters("LOG"              ))
			&& SG_MODULE_PARAMETER_SET("BLOCK"            , Parameters("BLOCK"            ))
			&& SG_MODULE_PARAMETER_SET("DBLOCK"           , Parameters("DBLOCK"           ))
			&& SG_MODULE_PARAMETER_SET("SEARCH_RANGE"     , Parameters("SEARCH_RANGE"     ))
			&& SG_MODULE_PARAMETER_SET("SEARCH_RADIUS"    , Parameters("SEARCH_RADIUS"    ))
			&& SG_MODULE_PARAMETER_SET("SEARCH_POINTS_ALL", Parameters("SEARCH_POINTS_ALL"))
			&& SG_MODULE_PARAMETER_SET("SEARCH_POINTS_MIN", Parameters("SEARCH_POINTS_MIN"))
			&& SG_MODULE_PARAMETER_SET("SEARCH_POINTS_MAX", Parameters("SEARCH_POINTS_MAX"))
			&& SG_MODULE_PARAMETER_SET("SEARCH_DIRECTION" , Parameters("SEARCH_DIRECTION" ))
			&& SG_MODULE_PARAMETER_SET("DEFINITION"       , 1)	// grid or grid system
			&& SG_MODULE_PARAMETER_SET("PREDICTION"       , pResiduals)
			&& SG_MODULE_PARAMETER_SET("VARIANCE"         , pVariance )
		);
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pRegression->is_NoData(x, y) || pResiduals->is_NoData(x, y) )
			{
				pPrediction->Set_NoData(x, y);
			}
			else
			{
				pPrediction->Add_Value(x, y, pRegression->asDouble(x, y) + pResiduals->asDouble(x, y));
			}
		}
	}

	//-----------------------------------------------------
	pRegression->Set_Name(CSG_String::Format("%s.%s [%s]", Parameters("POINTS")->asGrid()->Get_Name(), Parameters("FIELD")->asString(), _TL("Regression")));
	pPrediction->Set_Name(CSG_String::Format("%s.%s [%s]", Parameters("POINTS")->asGrid()->Get_Name(), Parameters("FIELD")->asString(), _TL("Prediction")));

	if( Parameters("RESIDUALS")->asGrid() )
	{
		pResiduals->Set_Name(CSG_String::Format("%s.%s [%s]", Parameters("POINTS")->asGrid()->Get_Name(), Parameters("FIELD")->asString(), _TL("Residuals")));
	}

	if( pVariance )
	{
		pVariance->Set_Name(CSG_String::Format("%s.%s [%s]", Parameters("POINTS")->asGrid()->Get_Name(), Parameters("FIELD")->asString(), _TL("Quality")));
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
