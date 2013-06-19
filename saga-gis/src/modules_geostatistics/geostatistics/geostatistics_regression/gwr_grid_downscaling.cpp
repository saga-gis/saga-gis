/**********************************************************
 * Version $Id: gwr_grid_downscaling.cpp 1633 2013-03-22 13:35:15Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//               geostatistics_regression                //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("GWR Grid Downscaling"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Reference:\n"
		"- Fotheringham, S.A., Brunsdon, C., Charlton, M. (2002):"
		" Geographically Weighted Regression: the analysis of spatially varying relationships. John Wiley & Sons."
		" <a target=\"_blank\" href=\"http://onlinelibrary.wiley.com/doi/10.1111/j.1538-4632.2003.tb01114.x/abstract\">online</a>.\n"
		"\n"
		"- Fotheringham, S.A., Charlton, M., Brunsdon, C. (1998):"
		" Geographically weighted regression: a natural evolution of the expansion method for spatial data analysis."
		" Environment and Planning A 30(11), 1905–1927."
		" <a target=\"_blank\" href=\"http://www.envplan.com/abstract.cgi?id=a301905\">online</a>.\n"
		"\n"
		"- Lloyd, C. (2010): Spatial Data Analysis - An Introduction for GIS Users. Oxford, 206p.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "PREDICTORS"	, _TL("Predictors"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Grid_System(
		NULL	, "GRID_SYSTEM"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Grid(
		pNode	, "DEPENDENT"	, _TL("Dependent Variable"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid(
		pNode	, "QUALITY"		, _TL("Coefficient of Determination"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_Grid(
		pNode	, "RESIDUALS"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_Grid_List(
		pNode	, "MODEL"		, _TL("Regression Parameters"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, false
	);

	Parameters.Add_Value(
		NULL	, "MODEL_OUT"	, _TL("Output of Model Parameters"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "SEARCH_RANGE"		, _TL("Search Range"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("local"),
			_TL("global")
		)
	);

	Parameters.Add_Value(
		pNode	, "SEARCH_RADIUS"		, _TL("Search Distance [Cells]"),
		_TL(""),
		PARAMETER_TYPE_Int, 10, 1, true
	);

	m_Search.Get_Weighting().Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Search.Get_Weighting().Set_BandWidth(7.0);
	m_Search.Get_Weighting().Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGWR_Grid_Downscaling::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SEARCH_RANGE")) )
	{
		pParameters->Get_Parameter("SEARCH_RADIUS")->Set_Enabled(pParameter->asInt() == 0);	// local
	}

	m_Search.Get_Weighting().Enable_Parameters(pParameters);

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
		m_pPredictors[i]	->Assign(pPredictors->asGrid(i), GRID_INTERPOLATION_NearestNeighbour);	// GRID_INTERPOLATION_Mean_Cells

		m_pModel     [i]	= SG_Create_Grid(m_pDependent->Get_System());
		m_pModel     [i]	->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pPredictors->asGrid(i)->Get_Name(), _TL("Factor")));
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
		for(i=0; i<m_nPredictors; i++)
		{
			delete(m_pModel[i]);
		}

		delete(m_pModel[m_nPredictors]);
	}

	SG_FREE_SAFE(m_pModel);
	SG_FREE_SAFE(m_pPredictors);

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGWR_Grid_Downscaling::Set_Model(double x, double y, double &Value)
{
	if( !m_pModel[m_nPredictors]->Get_Value(x, y, Value, GRID_INTERPOLATION_BSpline) )
	{
		return( false );
	}

	double	Model, Predictor;

	for(int i=0; i<m_nPredictors; i++)
	{
		if( !m_pModel     [i]->Get_Value(x, y, Model    , GRID_INTERPOLATION_BSpline)
		||  !m_pPredictors[i]->Get_Value(x, y, Predictor, GRID_INTERPOLATION_NearestNeighbour) )
		{
			return( false );
		}

		Value	+= Model * Predictor;
	}

	return( true );
}

//---------------------------------------------------------
bool CGWR_Grid_Downscaling::Set_Model(void)
{
	CSG_Grid	*pRegression	= Parameters("REGRESSION")->asGrid();

	pRegression->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pDependent->Get_Name(), _TL("GWR")));

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	p_y	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Value, p_x	= Get_XMin() + x * Get_Cellsize();

			if( Set_Model(p_x, p_y, Value) )
			{
				pRegression->Set_Value(x, y, Value);
			}
			else
			{
				pRegression->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGWR_Grid_Downscaling::Get_Model(void)
{
	//-----------------------------------------------------
	m_pQuality		= Parameters("QUALITY"  )->asGrid();
	m_pQuality		->Set_Name(CSG_String::Format(SG_T("%s [%s, %s]"), m_pDependent->Get_Name(), _TL("GWR"), _TL("Quality")));

	m_pResiduals	= Parameters("RESIDUALS")->asGrid();
	m_pResiduals	->Set_Name(CSG_String::Format(SG_T("%s [%s, %s]"), m_pDependent->Get_Name(), _TL("GWR"), _TL("Residuals")));

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
			if( !Get_Regression(x, y) )
			{
				m_pQuality  ->Set_NoData(x, y);
				m_pResiduals->Set_NoData(x, y);

				for(int i=0; i<=m_nPredictors; i++)
				{
					m_pModel[i]->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Search.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGWR_Grid_Downscaling::Get_Variables(int x, int y, CSG_Vector &z, CSG_Vector &w, CSG_Matrix &Y)
{
	int		n	= 0;

	z.Create(m_Search.Get_Count());
	w.Create(m_Search.Get_Count());
	Y.Create(1 + m_nPredictors, m_Search.Get_Count());

	//-----------------------------------------------------
	for(int i=0, ix, iy; i<m_Search.Get_Count(); i++)
	{
		double	id, iw;

		if( m_Search.Get_Values(i, ix = x, iy = y, id, iw, true) && m_pDependent->is_InGrid(ix, iy) )
		{
			z[n]	= m_pDependent->asDouble(ix, iy);
			w[n]	= iw;
			Y[n][0]	= 1.0;

			for(int j=0; j<m_nPredictors && iw>0.0; j++)
			{
				if( !m_pPredictors[j]->is_NoData(ix, iy) )
				{
					Y[n][j + 1]	= m_pPredictors[j]->asDouble(ix, iy);
				}
				else
				{
					iw	= 0.0;
				}
			}

			if( iw > 0.0 )
			{
				n++;
			}
		}
	}

	//-----------------------------------------------------
	z.Set_Rows(n);
	w.Set_Rows(n);
	Y.Set_Rows(n);

	return( n );
}

//---------------------------------------------------------
bool CGWR_Grid_Downscaling::Get_Regression(int x, int y)
{
	//-----------------------------------------------------
	int			i, nPoints;
	double		zMean, zr, rss, tss;
	CSG_Vector	b, z, w;
	CSG_Matrix	Y, YtW;

	if( (nPoints = Get_Variables(x, y, z, w, Y)) <= m_nPredictors )
	{
		return( false );
	}

	//-----------------------------------------------------
	YtW.Create(nPoints, 1 + m_nPredictors);

	for(i=0, zMean=0.0; i<nPoints; i++)
	{
		zMean		+= z[i];
		YtW[0][i]	 = w[i];

		for(int j=1; j<=m_nPredictors; j++)
		{
			YtW[j][i]	= Y[i][j] * w[i];
		}
	}

	//-----------------------------------------------------
	b		= (YtW * Y).Get_Inverse() * (YtW * z);

	zMean	/= nPoints;

	for(i=0, rss=0.0, tss=0.0; i<nPoints; i++)
	{
		zr	= b[0];

		for(int j=1; j<=m_nPredictors; j++)
		{
			zr	+= b[j] * Y[i][j];
		}

		rss	+= w[i] * SG_Get_Square(z[i] - zr);
		tss	+= w[i] * SG_Get_Square(z[i] - zMean);
	}

	m_pQuality->Set_Value(x, y, tss > 0.0 ? (tss - rss) / tss : 0.0);

	for(i=0; i<m_nPredictors; i++)
	{
		m_pModel[i]->Set_Value(x, y, b[i + 1]);
	}

	m_pModel[m_nPredictors]->Set_Value(x, y, b[0]);

	//-----------------------------------------------------
	if( m_pDependent->is_NoData(x, y) )
	{
		m_pResiduals->Set_NoData(x, y);
	}
	else
	{
		zr	= b[0];

		for(i=0; i<m_nPredictors; i++)
		{
			if( m_pPredictors[i]->is_NoData(x, y) )
			{
				m_pResiduals->Set_NoData(x, y);

				return( true );
			}

			zr	+= b[i + 1] * m_pPredictors[i]->asDouble(x, y);
		}

		m_pResiduals->Set_Value(x, y, m_pDependent->asDouble(x, y) - zr);
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
