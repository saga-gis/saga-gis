/**********************************************************
 * Version $Id: gw_regression_grid.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                 gw_regression_grid.cpp                //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gw_regression_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GRID_INIT(g, s)		if( g ) { g->Set_Name(CSG_String::Format(SG_T("%s (%s - %s)"), Parameters("DEPENDENT")->asString(), s, m_pPredictor->Get_Name())); }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGW_Regression_Grid::CGW_Regression_Grid(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("GWR for Single Predictor Grid"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Geographically Weighted Regression for a single predictor supplied as grid, "
		"to which the regression model is applied. Further details can be stored optionally.\n"
		"Reference:\n"
	) + GWR_References);

	//-----------------------------------------------------
	pNode = Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "DEPENDENT"	, _TL("Dependent Variable"),
		_TL("")
	);

	Parameters.Add_Shapes(
		NULL	, "RESIDUALS"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "PREDICTOR"	, _TL("Predictor"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "QUALITY"		, _TL("Coefficient of Determination"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "INTERCEPT"	, _TL("Intercept"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "SLOPE"		, _TL("Slope"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	m_Weighting.Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Weighting.Create_Parameters(&Parameters, false);

	//-----------------------------------------------------
	m_Search.Create(&Parameters, Parameters.Add_Node(NULL, "NODE_SEARCH", _TL("Search Options"), _TL("")), 16);

	Parameters("SEARCH_RANGE"     )->Set_Value(1);
	Parameters("SEARCH_POINTS_ALL")->Set_Value(1);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGW_Regression_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Search.On_Parameter_Changed(pParameters, pParameter);

		pParameters->Set_Parameter("DW_BANDWIDTH", GWR_Fit_To_Density(pParameter->asShapes(), 4.0, 1));
	}

	return( 1 );
}

//---------------------------------------------------------
int CGW_Regression_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Search.On_Parameters_Enable(pParameters, pParameter);

	m_Weighting.Enable_Parameters(pParameters);

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Regression_Grid::On_Execute(void)
{
	//-----------------------------------------------------
	m_pPredictor	= Parameters("PREDICTOR" )->asGrid  ();
	m_pRegression	= Parameters("REGRESSION")->asGrid  ();
	m_pQuality		= Parameters("QUALITY"   )->asGrid  ();
	m_pIntercept	= Parameters("INTERCEPT" )->asGrid  ();
	m_pSlope		= Parameters("SLOPE"     )->asGrid  ();
	m_pPoints		= Parameters("POINTS"    )->asShapes();
	m_iDependent	= Parameters("DEPENDENT" )->asInt   ();

	m_Weighting.Set_Parameters(&Parameters);

	//-----------------------------------------------------
	if( !m_Search.Initialize(m_pPoints, -1) )
	{
		return( false );
	}

	//-----------------------------------------------------
	GRID_INIT(m_pRegression, _TL("GWR Regression"));
	GRID_INIT(m_pQuality   , _TL("GWR Quality"));
	GRID_INIT(m_pIntercept , _TL("GWR Intercept"));
	GRID_INIT(m_pSlope     , _TL("GWR Slope"));

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Regression_Weighted	Model;

			if (!m_pPredictor->is_NoData(x, y) && Get_Model(x, y, Model))
			{
				SG_GRID_PTR_SAFE_SET_VALUE(m_pRegression, x, y, Model[0] + Model[1] * m_pPredictor->asDouble(x, y));
				SG_GRID_PTR_SAFE_SET_VALUE(m_pIntercept , x, y, Model[0]);
				SG_GRID_PTR_SAFE_SET_VALUE(m_pSlope     , x, y, Model[1]);
				SG_GRID_PTR_SAFE_SET_VALUE(m_pQuality   , x, y, Model.Get_CV_R2());
			}
			else
			{
				SG_GRID_PTR_SAFE_SET_NODATA(m_pRegression, x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(m_pIntercept , x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(m_pSlope     , x, y);
				SG_GRID_PTR_SAFE_SET_NODATA(m_pQuality   , x, y);
			}
		}
	}

	//-----------------------------------------------------
	Set_Residuals();

	m_Search.Finalize();

	DataObject_Update(m_pIntercept);
	DataObject_Update(m_pSlope);
	DataObject_Update(m_pQuality);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Regression_Grid::Get_Model(int x, int y, CSG_Regression_Weighted &Model)
{
	//-----------------------------------------------------
	TSG_Point	Point	= Get_System()->Get_Grid_to_World(x, y);
	int			nPoints = m_Search.Set_Location(Point);

	Model.Destroy();

	for(int iPoint=0; iPoint<nPoints; iPoint++)
	{
		double	ix, iy, iz;

		CSG_Shape	*pPoint = m_Search.Do_Use_All() && m_Search.Get_Point(iPoint, ix, iy, iz)
			? m_pPoints->Get_Shape((int)iz)
			: m_pPoints->Get_Shape(iPoint);

		if( !pPoint->is_NoData(m_iDependent) && m_pPredictor->Get_Value(pPoint->Get_Point(0), iz) )
		{
			Model.Add_Sample(
				m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0))),
				pPoint->asDouble(m_iDependent), CSG_Vector(1, &iz)
			);
		}
	}

	//-----------------------------------------------------
	return( Model.Calculate() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Regression_Grid::Set_Residuals(void)
{
	CSG_Shapes	*pResiduals	= Parameters("RESIDUALS")->asShapes();

	if( !pResiduals || !m_pPoints || !m_pRegression )
	{
		return( false );
	}

	//-----------------------------------------------------
	pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), m_pPoints->Get_Name(), _TL("Residuals")));
	pResiduals->Add_Field(m_pPoints->Get_Field_Name(m_iDependent), SG_DATATYPE_Double);
	pResiduals->Add_Field("TREND"	, SG_DATATYPE_Double);
	pResiduals->Add_Field("RESIDUAL", SG_DATATYPE_Double);

	//-------------------------------------------------
	for(int iShape=0; iShape<m_pPoints->Get_Count() && Set_Progress(iShape, m_pPoints->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= m_pPoints->Get_Shape(iShape);
		double		 zShape	= pShape->asDouble(m_iDependent);

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				double	zRegression;

				TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

				if( m_pRegression->Get_Value(Point, zRegression) )
				{
					CSG_Shape	*pResidual	= pResiduals->Add_Shape();

					pResidual->Add_Point(Point);
					pResidual->Set_Value(0, zShape);
					pResidual->Set_Value(1, zRegression);
					pResidual->Set_Value(2, zShape - zRegression);
				}
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
