
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
//             gw_multi_regression_points.cpp            //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gw_multi_regression_points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGW_Multi_Regression_Points::CGW_Multi_Regression_Points(void)
{
	Set_Name		(_TL("GWR for Multiple Predictors"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Geographically Weighted Regression for multiple predictors. "
		"Regression details are stored in a copy of input points. "
	));

	GWR_Add_References(true);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"DEPENDENT"	, _TL("Dependent Variable"),
		_TL("")
	);

	Parameters.Add_Table_Fields("POINTS",
		"PREDICTORS", _TL("Predictors"),
		_TL("")
	);

	Parameters.Add_Shapes("",
		"REGRESSION", _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Bool("",
		"LOGISTIC"	, _TL("Logistic Regression"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	m_Weighting.Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Weighting.Create_Parameters(&Parameters, false);

	//-----------------------------------------------------
	m_Search.Create(&Parameters, Parameters.Add_Node("", "NODE_SEARCH", _TL("Search Options"), _TL("")), 16);

	Parameters("SEARCH_RANGE"     )->Set_Value(1);
	Parameters("SEARCH_POINTS_ALL")->Set_Value(1);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGW_Multi_Regression_Points::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Search.On_Parameter_Changed(pParameters, pParameter);

		pParameters->Set_Parameter("DW_BANDWIDTH", GWR_Fit_To_Density(pParameter->asShapes(), 4.0, 1));
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGW_Multi_Regression_Points::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Search.On_Parameters_Enable(pParameters, pParameter);

	m_Weighting.Enable_Parameters(pParameters);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression_Points::Initialize(void)
{
	//-----------------------------------------------------
	m_pPoints	= Parameters("REGRESSION")->asShapes();

	m_Weighting.Set_Parameters(&Parameters);

	//-----------------------------------------------------
	int			iDependent	= Parameters("DEPENDENT")->asInt   ();
	CSG_Shapes	*pPoints	= Parameters("POINTS"   )->asShapes();

	m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format("%s.%s [%s]", pPoints->Get_Name(), pPoints->Get_Field_Name(iDependent), _TL("GWR")));
	m_pPoints->Add_Field(pPoints->Get_Field_Name(iDependent), SG_DATATYPE_Double);

	//-----------------------------------------------------
	CSG_Parameter_Table_Fields	*pFields	= Parameters("PREDICTORS")->asTableFields();

	if( (m_nPredictors = pFields->Get_Count()) <= 0 )
	{
		Error_Set(_TL("no predictors have been selected"));

		return( false );
	}

	//-----------------------------------------------------
	int		i;

	for(i=0; i<m_nPredictors; i++)	// predictor values
	{
		m_pPoints->Add_Field(pPoints->Get_Field_Name(pFields->Get_Index(i)), SG_DATATYPE_Double);
	}

	m_pPoints->Add_Field("DEPENDENT" , SG_DATATYPE_Double);	// m_nPredictors + 0
	m_pPoints->Add_Field("R2"        , SG_DATATYPE_Double);	// m_nPredictors + 1
	m_pPoints->Add_Field("REGRESSION", SG_DATATYPE_Double);	// m_nPredictors + 2
	m_pPoints->Add_Field("RESIDUAL"  , SG_DATATYPE_Double);	// m_nPredictors + 3
	m_pPoints->Add_Field("INTERCEPT" , SG_DATATYPE_Double);	// m_nPredictors + 4

	for(i=0; i<m_nPredictors; i++)	// predictor model slopes
	{
		m_pPoints->Add_Field(CSG_String::Format("%s.%s", _TL("SLOPE"), pPoints->Get_Field_Name(pFields->Get_Index(i))), SG_DATATYPE_Double);
	}

	for(int iPoint=0; iPoint<pPoints->Get_Count(); iPoint++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(iPoint);

		bool	bOkay	= !pPoint->is_NoData(iDependent);

		for(i=0; bOkay && i<m_nPredictors; i++)
		{
			bOkay	= !pPoint->is_NoData(pFields->Get_Index(i));
		}

		if( bOkay )
		{
			CSG_Shape	*pAdd	= m_pPoints->Add_Shape(pPoint, SHAPE_COPY_GEOM);

			for(i=0; i<m_nPredictors; i++)
			{
				pAdd->Set_Value(i, pPoint->asDouble(pFields->Get_Index(i)));
			}

			pAdd->Set_Value(m_nPredictors, pPoint->asDouble(iDependent));
		}
	}

	//-----------------------------------------------------
	if( m_pPoints->Get_Count() == 0 )
	{
		Error_Set(_TL("invalid data"));

		return( false );
	}

	if( !m_Search.Initialize(m_pPoints, -1) )
	{
		Error_Set(_TL("failed to create searche engine"));

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
void CGW_Multi_Regression_Points::Finalize(void)
{
	m_Search.Finalize();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression_Points::On_Execute(void)
{
	if( !Initialize() )
	{
		Finalize();

		return( false );
	}

	bool	bLogistic	= Parameters("LOGISTIC")->asBool();

	//-----------------------------------------------------
	for(int iPoint=0; iPoint<m_pPoints->Get_Count() && Set_Progress(iPoint, m_pPoints->Get_Count()); iPoint++)
	{
		CSG_Shape	*pPoint = m_pPoints->Get_Shape(iPoint);

		CSG_Regression_Weighted	Model;

		if( Get_Model(pPoint->Get_Point(0), Model, bLogistic) )
		{
			double	Value	= Model[0];

			for(int i=1; i<=m_nPredictors; i++)
			{
				pPoint->Set_Value(m_nPredictors + 4 + i, Model[i]);

				Value	+= Model[i] * pPoint->asDouble(i);
			}

			if( bLogistic )
			{
				Value	= 1. / (1. + exp(-Value));
			}

			pPoint->Set_Value(m_nPredictors + 1, Model.Get_R2());
			pPoint->Set_Value(m_nPredictors + 2, Value);
			pPoint->Set_Value(m_nPredictors + 3, pPoint->asDouble(0) - Value);	// Residual
		}
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression_Points::Get_Model(const TSG_Point &Point, CSG_Regression_Weighted &Model, bool bLogistic)
{
	int	nPoints	= m_Search.Set_Location(Point);

	CSG_Vector	Predictors(m_nPredictors);

	Model.Destroy();

	for(int iPoint=0; iPoint<nPoints; iPoint++)
	{
		double	ix, iy, iz;

		CSG_Shape	*pPoint = m_Search.Do_Use_All() && m_Search.Get_Point(iPoint, ix, iy, iz)
			? m_pPoints->Get_Shape((int)iz)
			: m_pPoints->Get_Shape(iPoint);

		for(int iPredictor=0; iPredictor<m_nPredictors; iPredictor++)
		{
			Predictors[iPredictor]	= pPoint->asDouble(iPredictor);
		}

		Model.Add_Sample(
			m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0))),
			pPoint->asDouble(m_nPredictors), Predictors
		);
	}

	return( Model.Calculate(bLogistic) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
