
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  statistics_kriging                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 kriging_regression.cpp                //
//                                                       //
//                 Olaf Conrad (C) 2015                  //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
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
	//-----------------------------------------------------
	Set_Name		(_TL("Regression Kriging"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Regression Kriging for grid interpolation from irregular sample points."
	));

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"FIELD"		, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"PREDICTORS", _TL("Predictors"),
		_TL(""),
		PARAMETER_INPUT, true
	);

	Parameters.Add_Grid("",
		"REGRESSION", _TL("Regression"),
		_TL("regression model applied to predictor grids"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"PREDICTION", _TL("Prediction"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"RESIDUALS"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"VARIANCE"	, _TL("Prediction Error"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("VARIANCE",
		"TQUALITY"	, _TL("Error Measure"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("standard deviation"),
			_TL("variance")
		), 0
	);


	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Parameters.Add_Table("", "INFO_COEFF", _TL("Regression: Coefficients"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Table("", "INFO_MODEL", _TL("Regression: Model"       ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Table("", "INFO_STEPS", _TL("Regression: Steps"       ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Node("",
		"NODE_REG"	,	_TL("Regression"),
		_TL("")
	);

	Parameters.Add_Bool("NODE_REG", "COORD_X"  , _TL("Include X Coordinate"), _TL(""), false);
	Parameters.Add_Bool("NODE_REG", "COORD_Y"  , _TL("Include Y Coordinate"), _TL(""), false);
	Parameters.Add_Bool("NODE_REG", "INTERCEPT", _TL("Intercept"           ), _TL(""), true );

	Parameters.Add_Choice("NODE_REG",
		"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("include all"),
			_TL("forward"),
			_TL("backward"),
			_TL("stepwise")
		), 3
	);

	Parameters.Add_Double("NODE_REG",
		"P_VALUE"		, _TL("Significance Level"),
		_TL("Significance level (aka p-value) as threshold for automated predictor selection, given as percentage"),
		5., 0., true, 100., true
	);

	Parameters.Add_Choice("NODE_REG",
		"RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);


	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Parameters.Add_Double("",
		"VAR_MAXDIST"	, _TL("Maximum Distance"),
		_TL("maximum distance for variogram estimation, ignored if set to zero"),
		0., 0., true
	)->Set_UseInGUI(false);

	Parameters.Add_Int("",
		"VAR_NCLASSES"	, _TL("Lag Distance Classes"),
		_TL("initial number of lag distance classes for variogram estimation"),
		100, 1, true
	)->Set_UseInGUI(false);

	Parameters.Add_Int("",
		"VAR_NSKIP"		, _TL("Skip"),
		_TL(""),
		1, 1, true
	)->Set_UseInGUI(false);

	Parameters.Add_String("",
		"VAR_MODEL"		, _TL("Variogram Model"),
		_TL(""),
		"a + b * x"
	)->Set_UseInGUI(false);


	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Parameters.Add_Node("",
		"NODE_KRG"	, _TL("Kriging"),
		_TL("")
	);

	Parameters.Add_Choice("NODE_KRG",
		"KRIGING"	, _TL("Kriging Type"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Simple Kriging"),
			_TL("Ordinary Kriging")
		)
	);

	Parameters.Add_Bool("NODE_KRG",
		"LOG"		, _TL("Logarithmic Transformation"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("NODE_KRG",
		"BLOCK"		, _TL("Block Kriging"),
		_TL(""),
		false
	);

	Parameters.Add_Double("BLOCK",
		"DBLOCK"	, _TL("Block Size"),
		_TL(""),
		100., 0., true
	);


	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	m_Search_Options.Create(&Parameters, "NODE_SEARCH", 16);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CKriging_Regression::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Search_Options.On_Parameter_Changed(pParameters, pParameter);
	}

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CKriging_Regression::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("VARIANCE") )
	{
		pParameters->Set_Enabled("TQUALITY", pParameter->asPointer() != NULL);
	}

	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("P_VALUE" , pParameter->asInt() > 0);
	}

	m_Search_Options.On_Parameters_Enable(pParameters, pParameter);

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
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
	SG_RUN_TOOL_ExitOnError("statistics_regression", 1,	// Multiple Regression Analysis (Points and Predictor Grids)
			SG_TOOL_PARAMETER_SET("PREDICTORS", Parameters("PREDICTORS"))
		&&	SG_TOOL_PARAMETER_SET("POINTS"    , Parameters("POINTS"    ))
		&&	SG_TOOL_PARAMETER_SET("ATTRIBUTE" , Parameters("FIELD"     ))
		&&	SG_TOOL_PARAMETER_SET("INFO_COEFF", Parameters("INFO_COEFF"))
		&&	SG_TOOL_PARAMETER_SET("INFO_MODEL", Parameters("INFO_MODEL"))
		&&	SG_TOOL_PARAMETER_SET("INFO_STEPS", Parameters("INFO_STEPS"))
		&&	SG_TOOL_PARAMETER_SET("RESAMPLING", Parameters("RESAMPLING"))
		&&	SG_TOOL_PARAMETER_SET("COORD_X"   , Parameters("COORD_X"   ))
		&&	SG_TOOL_PARAMETER_SET("COORD_Y"   , Parameters("COORD_Y"   ))
		&&	SG_TOOL_PARAMETER_SET("INTERCEPT" , Parameters("INTERCEPT" ))
		&&	SG_TOOL_PARAMETER_SET("METHOD"    , Parameters("METHOD"    ))
		&&	SG_TOOL_PARAMETER_SET("P_VALUE"   , Parameters("P_VALUE"   ))
		&&	SG_TOOL_PARAMETER_SET("REGRESSION", pRegression)
		&&	SG_TOOL_PARAMETER_SET("RESIDUALS" , &Points )
	);

	//-----------------------------------------------------
	CSG_Tool	*pK	= Parameters("KRIGING")->asInt() == 0 ? (CSG_Tool *)&m_SK : (CSG_Tool *)&m_OK;

	Process_Set_Text(pK->Get_Name());

	pK->Set_Manager(NULL);

	if( !pK->Set_Parameter("POINTS"           , &Points)
	||  !pK->Set_Parameter("FIELD"            , 2)	// residual
	||  !pK->Set_Parameter("LOG"              , Parameters("LOG"              ))
	||  !pK->Set_Parameter("BLOCK"            , Parameters("BLOCK"            ))
	||  !pK->Set_Parameter("DBLOCK"           , Parameters("DBLOCK"           ))
	||  !pK->Set_Parameter("SEARCH_RANGE"     , Parameters("SEARCH_RANGE"     ))
	||  !pK->Set_Parameter("SEARCH_RADIUS"    , Parameters("SEARCH_RADIUS"    ))
	||  !pK->Set_Parameter("SEARCH_POINTS_ALL", Parameters("SEARCH_POINTS_ALL"))
	||  !pK->Set_Parameter("SEARCH_POINTS_MIN", Parameters("SEARCH_POINTS_MIN"))
	||  !pK->Set_Parameter("SEARCH_POINTS_MAX", Parameters("SEARCH_POINTS_MAX"))
	||  !pK->Set_Parameter("TARGET_DEFINITION", 1)	// grid or grid system
	||  !pK->Set_Parameter("PREDICTION"       , pResiduals)
	||  !pK->Set_Parameter("VARIANCE"         , pVariance )

	|| (!SG_UI_Get_Window_Main() && (	// saga_cmd
	    !pK->Set_Parameter("VAR_MAXDIST"      , Parameters("VAR_MAXDIST"      ))
	||  !pK->Set_Parameter("VAR_NCLASSES"     , Parameters("VAR_NCLASSES"     ))
	||  !pK->Set_Parameter("VAR_NSKIP"        , Parameters("VAR_NSKIP"        ))
	||  !pK->Set_Parameter("VAR_MODEL"        , Parameters("VAR_MODEL"        )))) )
	{
		Error_Set(CSG_String::Format("%s [%s].[%s]", _TL("could not initialize tool"), _TL("statistics_regression"), pK->Get_Name().c_str()));

		return( false );
	}

	if( !pK->Execute() )
	{
		Error_Set(CSG_String::Format("%s [%s].[%s]", _TL("could not execute tool"   ), _TL("statistics_regression"), pK->Get_Name().c_str()));

		return( false );
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
				pPrediction->Set_Value(x, y, pRegression->asDouble(x, y) + pResiduals->asDouble(x, y));
			}
		}
	}

	//-----------------------------------------------------
	pRegression->Fmt_Name("%s.%s [%s]", Parameters("POINTS")->asShapes()->Get_Name(), Parameters("FIELD")->asString(), _TL("Regression"));
	pPrediction->Fmt_Name("%s.%s [%s]", Parameters("POINTS")->asShapes()->Get_Name(), Parameters("FIELD")->asString(), _TL("Prediction"));

	if( Parameters("RESIDUALS")->asGrid() )
	{
		pResiduals->Fmt_Name("%s.%s [%s]", Parameters("POINTS")->asShapes()->Get_Name(), Parameters("FIELD")->asString(), _TL("Residuals"));
	}

	if( pVariance )
	{
		pVariance ->Fmt_Name("%s.%s [%s]", Parameters("POINTS")->asShapes()->Get_Name(), Parameters("FIELD")->asString(), _TL("Quality"));
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
