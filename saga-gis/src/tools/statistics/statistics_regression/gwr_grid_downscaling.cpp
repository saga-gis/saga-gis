/**********************************************************
 * Version $Id: gwr_grid_downscaling.cpp 1633 2013-03-22 13:35:15Z oconrad $
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
//                gwr_grid_downscaling.cpp               //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
#include "gwr_grid_downscaling.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGWR_Grid_Downscaling::CGWR_Grid_Downscaling(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("GWR for Grid Downscaling"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		""
	));

	Add_Reference(
		"Fotheringham, S.A., Brunsdon, C., Charlton, M", "2002",
		"Geographically weighted regression: the analysis of spatially varying relationships",
		"John Wiley & Sons.",
		SG_T("http://onlinelibrary.wiley.com/doi/10.1111/j.1538-4632.2003.tb01114.x/abstract")
	);

	Add_Reference(
		"Fotheringham, S.A., Charlton, M., Brunsdon, C.", "1998",
		"Geographically weighted regression: a natural evolution of the expansion method for spatial data analysis.",
		"Environment and Planning A 30(11), 1905–1927.",
		SG_T("http://www.envplan.com/abstract.cgi?id=a301905")
	);

	Add_Reference(
		"Lloyd, C.", "2010",
		"Spatial data analysis - an introduction for GIS users",
		"Oxford, 206p."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"PREDICTORS"	, _TL("Predictors"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"REG_RESCORR"	, _TL("Regression with Residual Correction"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_System("",
		"GRID_SYSTEM"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Grid("GRID_SYSTEM",
		"DEPENDENT"		, _TL("Dependent Variable"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid("GRID_SYSTEM",
		"QUALITY"		, _TL("Coefficient of Determination"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_Grid("GRID_SYSTEM",
		"RESIDUALS"		, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_Grid_List("GRID_SYSTEM",
		"MODEL"			, _TL("Regression Parameters"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, false
	);

	Parameters.Add_Bool("",
		"MODEL_OUT"		, _TL("Output of Model Parameters"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"SEARCH_RANGE"	, _TL("Search Range"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("local"),
			_TL("global")
		)
	);

	Parameters.Add_Int("SEARCH_RANGE",
		"SEARCH_RADIUS"	, _TL("Search Distance [Cells]"),
		_TL(""),
		10, 1, true
	);

	m_Search.Get_Weighting().Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Search.Get_Weighting().Set_BandWidth(7.0);
	m_Search.Get_Weighting().Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGWR_Grid_Downscaling::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SEARCH_RANGE") )
	{
		pParameters->Set_Enabled("SEARCH_RADIUS", pParameter->asInt() == 0);	// local
	}

	m_Search.Get_Weighting().Enable_Parameters(pParameters);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGWR_Grid_Downscaling::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pPredictors	= Parameters("PREDICTORS")->asGridList();

	if( (m_nPredictors = pPredictors->Get_Count()) <= 0 )
	{
		return( false );
	}

	m_pDependent	= Parameters("DEPENDENT")->asGrid();

	if( !m_pDependent->Get_Extent().Intersects(Get_System()->Get_Extent()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		i;

	Process_Set_Text(_TL("upscaling of predictors"));

	m_pPredictors	= (CSG_Grid **)SG_Calloc(m_nPredictors    , sizeof(CSG_Grid *));
	m_pModel		= (CSG_Grid **)SG_Calloc(m_nPredictors + 1, sizeof(CSG_Grid *));

	for(i=0; i<m_nPredictors; i++)
	{
		m_pPredictors[i]	= SG_Create_Grid(m_pDependent->Get_System());
		m_pPredictors[i]	->Assign(pPredictors->asGrid(i), GRID_RESAMPLING_NearestNeighbour);	// GRID_RESAMPLING_Mean_Cells

		m_pModel     [i]	= SG_Create_Grid(m_pDependent->Get_System());
		m_pModel     [i]	->Set_Name(CSG_String::Format("%s [%s]", pPredictors->asGrid(i)->Get_Name(), _TL("Factor")));
	}

	m_pModel[m_nPredictors]	= SG_Create_Grid(m_pDependent->Get_System());
	m_pModel[m_nPredictors]	->Set_Name(_TL("Intercept"));

	//-----------------------------------------------------
	Process_Set_Text(_TL("model creation"));

	bool	bResult	= Get_Model();

	//-----------------------------------------------------
	for(i=0; i<m_nPredictors; i++)
	{
		delete(m_pPredictors[i]);

		m_pPredictors[i]	= pPredictors->asGrid(i);
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Process_Set_Text(_TL("downscaling"));

		bResult	= Set_Model();
	}

	//-----------------------------------------------------
	if( Parameters("MODEL_OUT")->asBool() )
	{
		CSG_Parameter_Grid_List	*pModel	= Parameters("MODEL")->asGridList();

		pModel->Del_Items();
		pModel->Add_Item(m_pModel[m_nPredictors]);

		for(i=0; i<m_nPredictors; i++)
		{
			pModel->Add_Item(m_pModel[i]);
		}
	}
	else
	{
		for(i=0; i<=m_nPredictors; i++)
		{
			delete(m_pModel[i]);
		}
	}

	SG_FREE_SAFE(m_pModel);
	SG_FREE_SAFE(m_pPredictors);

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGWR_Grid_Downscaling::Set_Model(double x, double y, double &Value, double &Residual)
{
	if( !m_pModel[m_nPredictors]->Get_Value(x, y, Value, GRID_RESAMPLING_BSpline) )
	{
		return( false );
	}

	double	Model, Predictor;

	for(int i=0; i<m_nPredictors; i++)
	{
		if( !m_pModel     [i]->Get_Value(x, y, Model    , GRID_RESAMPLING_BSpline)
		||  !m_pPredictors[i]->Get_Value(x, y, Predictor, GRID_RESAMPLING_BSpline) )
		{
			return( false );
		}

		Value	+= Model * Predictor;
	}

	if( !m_pResiduals->Get_Value(x, y, Residual, GRID_RESAMPLING_BSpline) )
	{
		Residual	= 0.0;
	}

	return( true );
}

//---------------------------------------------------------
bool CGWR_Grid_Downscaling::Set_Model(void)
{
	CSG_Grid	*pRegression	= Parameters("REGRESSION" )->asGrid();
	CSG_Grid	*pReg_ResCorr	= Parameters("REG_RESCORR")->asGrid();

	pRegression->Set_Name(CSG_String::Format("%s [%s]", m_pDependent->Get_Name(), _TL("GWR")));

	if( pReg_ResCorr )
	{
		pReg_ResCorr->Set_Name(CSG_String::Format("%s [%s, %s]", m_pDependent->Get_Name(), _TL("GWR"), _TL("Residual Correction")));
	}

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	p_y	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Value, Residual, p_x	= Get_XMin() + x * Get_Cellsize();

			if( Set_Model(p_x, p_y, Value, Residual) )
			{
				pRegression->Set_Value(x, y, Value);

				if( pReg_ResCorr )
				{
					pReg_ResCorr->Set_Value(x, y, Value + Residual);
				}
			}
			else
			{
				pRegression->Set_NoData(x, y);

				if( pReg_ResCorr )
				{
					pReg_ResCorr->Set_NoData(x, y);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGWR_Grid_Downscaling::Get_Model(void)
{
	//-----------------------------------------------------
	m_pQuality		= Parameters("QUALITY"  )->asGrid();
	m_pQuality		->Set_Name(CSG_String::Format("%s [%s, %s]", m_pDependent->Get_Name(), _TL("GWR"), _TL("Quality")));

	m_pResiduals	= Parameters("RESIDUALS")->asGrid();
	m_pResiduals	->Set_Name(CSG_String::Format("%s [%s, %s]", m_pDependent->Get_Name(), _TL("GWR"), _TL("Residuals")));

	//-----------------------------------------------------
	m_Search.Get_Weighting().Set_Parameters(&Parameters);

	m_Search.Set_Radius(Parameters("SEARCH_RANGE")->asInt() == 0
		? Parameters("SEARCH_RADIUS")->asInt() : 1 + (int)(SG_Get_Length(m_pDependent->Get_NX(), m_pDependent->Get_NY()))
	);

	//-----------------------------------------------------
	CSG_Grid_System	System(m_pDependent->Get_System());

	for(int y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<System.Get_NX(); x++)
		{
			CSG_Regression_Weighted	Model;

			if( Get_Model(x, y, Model) )
			{
				m_pQuality->Set_Value(x, y, Model.Get_R2());

				m_pModel[m_nPredictors]->Set_Value(x, y, Model[0]);	// intercept

				for(int i=0; i<m_nPredictors; i++)
				{
					m_pModel[i]->Set_Value(x, y, Model[i + 1]);
				}
			}
			else
			{
				m_pQuality->Set_NoData(x, y);

				for(int i=0; i<=m_nPredictors; i++)
				{
					m_pModel[i]->Set_NoData(x, y);
				}

				m_pResiduals->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Search.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGWR_Grid_Downscaling::Get_Model(int x, int y, CSG_Regression_Weighted &Model)
{
	//-----------------------------------------------------
	CSG_Vector	Predictors(m_nPredictors);

	Model.Destroy();

	//-----------------------------------------------------
	for(int i=0, ix, iy; i<m_Search.Get_Count(); i++)
	{
		double	Distance, Weight;

		if( m_Search.Get_Values(i, ix = x, iy = y, Distance, Weight, true) && m_pDependent->is_InGrid(ix, iy) )
		{
			for(int iPredictor=0; iPredictor<m_nPredictors && Weight>0.0; iPredictor++)
			{
				if( !m_pPredictors[iPredictor]->is_NoData(ix, iy) )
				{
					Predictors[iPredictor]	= m_pPredictors[iPredictor]->asDouble(ix, iy);
				}
				else
				{
					Weight	= 0.0;
				}
			}

			if( Weight > 0.0 )
			{
				Model.Add_Sample(Weight, m_pDependent->asDouble(ix, iy), Predictors);
			}
		}
	}

	//-----------------------------------------------------
	if( Model.Calculate() )
	{
		m_pResiduals->Set_NoData(x, y);

		if( m_pDependent->is_NoData(x, y) )
		{
			return( true );
		}

		double	Value	= Model[0];

		for(int iPredictor=0; iPredictor<m_nPredictors; iPredictor++)
		{
			if( m_pPredictors[iPredictor]->is_NoData(x, y) )
			{
				return( true );
			}

			Value	+= Model[1 + iPredictor] * m_pPredictors[iPredictor]->asDouble(x, y);
		}

		m_pResiduals->Set_Value(x, y, m_pDependent->asDouble(x, y) - Value);

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
