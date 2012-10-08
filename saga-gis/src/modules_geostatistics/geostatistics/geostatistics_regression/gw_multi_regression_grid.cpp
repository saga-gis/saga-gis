/**********************************************************
 * Version $Id$
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
//             gw_multi_regression_grid.cpp              //
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
#include "gw_multi_regression_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GRID_SET_NODATA(g, x, y)	if( g ) { g->Set_NoData(x, y); }
#define GRID_SET_VALUE(g, x, y, z)	if( g ) { g->Set_Value(x, y, z); }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGW_Multi_Regression_Grid::CGW_Multi_Regression_Grid(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Geographically Weighted Multiple Regression (Points/Grids)"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Reference:\n"
		" - Lloyd, C. (2010): Spatial Data Analysis - An Introduction for GIS Users. Oxford, 206p.\n"
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

	Parameters.Add_Grid(
		NULL	, "QUALITY"		, _TL("Coefficient of Determination"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "SLOPES"		, _TL("Regression Parameters"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, false
	);

	Parameters.Add_Value(
		NULL	, "PARAMETERS"	, _TL("Output of Regression Parameters"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	pNode	= Parameters.Add_Shapes(
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
	Parameters.Add_Parameters(
		NULL	, "WEIGHTING"	, _TL("Weighting"),
		_TL("")
	);

	m_Weighting.Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Weighting.Create_Parameters(Parameters("WEIGHTING")->asParameters());

	//-----------------------------------------------------
	CSG_Parameter	*pSearch	= Parameters.Add_Node(
		NULL	, "NODE_SEARCH"			, _TL("Search Options"),
		_TL("")
	);

	pNode	= Parameters.Add_Choice(
		pSearch	, "SEARCH_RANGE"		, _TL("Search Range"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("local"),
			_TL("global")
		)
	);

	Parameters.Add_Value(
		pNode	, "SEARCH_RADIUS"		, _TL("Maximum Search Distance"),
		_TL("local maximum search distance given in map units"),
		PARAMETER_TYPE_Double	, 1000.0, 0, true
	);

	pNode	= Parameters.Add_Choice(
		pSearch	, "SEARCH_POINTS_ALL"	, _TL("Number of Points"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("maximum number of nearest points"),
			_TL("all points within search distance")
		)
	);

	Parameters.Add_Value(
		pNode	, "SEARCH_POINTS_MIN"	, _TL("Minimum"),
		_TL("minimum number of points to use"),
		PARAMETER_TYPE_Int, 4, 1, true
	);

	Parameters.Add_Value(
		pNode	, "SEARCH_POINTS_MAX"	, _TL("Maximum"),
		_TL("maximum number of nearest points"),
		PARAMETER_TYPE_Int, 20, 1, true
	);

	Parameters.Add_Choice(
		pNode	, "SEARCH_DIRECTION"	, _TL("Search Direction"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("all directions"),
			_TL("quadrants")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGW_Multi_Regression_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SEARCH_RANGE")) )
	{
		pParameters->Get_Parameter("SEARCH_RADIUS"    )->Set_Enabled(pParameter->asInt() == 0);	// local
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SEARCH_POINTS_ALL")) )
	{
		pParameters->Get_Parameter("SEARCH_POINTS_MAX")->Set_Enabled(pParameter->asInt() == 0);	// maximum number of points
		pParameters->Get_Parameter("SEARCH_DIRECTION" )->Set_Enabled(pParameter->asInt() == 0);	// maximum number of points per quadrant
	}

	m_Weighting.Enable_Parameters(pParameters);

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression_Grid::Initialize(void)
{
	bool		bAdd;
	int			iPoint, iPredictor, iDependent, Interpolation;
	double		*z;
	TSG_Point	Point;
	CSG_Shape	*pPoint;
	CSG_Shapes	*pPoints;

	//-----------------------------------------------------
	if( m_pPredictors->Get_Count() < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	pPoints			= Parameters("POINTS")		->asShapes();
	iDependent		= Parameters("DEPENDENT")	->asInt();
	Interpolation	= GRID_INTERPOLATION_BSpline;

	//-----------------------------------------------------
	m_Points.Create(SHAPE_TYPE_Point);
	m_Points.Set_Name(pPoints->Get_Name());
	m_Points.Add_Field(pPoints->Get_Field_Name(iDependent), SG_DATATYPE_Double);

	for(iPredictor=0; iPredictor<m_pPredictors->Get_Count(); iPredictor++)
	{
		m_Points.Add_Field(m_pPredictors->asGrid(iPredictor)->Get_Name(), SG_DATATYPE_Double);
	}

	z	= new double[m_pPredictors->Get_Count() + 1];

	//-----------------------------------------------------
	for(iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		pPoint	= pPoints->Get_Shape(iPoint);

		if( !pPoint->is_NoData(iDependent) )
		{
			Point	= pPoint->Get_Point(0);
			z[0]	= pPoint->asDouble(iDependent);

			for(iPredictor=0, bAdd=true; bAdd && iPredictor<m_pPredictors->Get_Count(); iPredictor++)
			{
				if( !m_pPredictors->asGrid(iPredictor)->Get_Value(Point, z[iPredictor + 1], Interpolation) )
				{
					bAdd	= false;
				}
			}

			if( bAdd )
			{
				pPoint	= m_Points.Add_Shape();
				pPoint->Add_Point(Point);

				for(iPredictor=0; iPredictor<=m_pPredictors->Get_Count(); iPredictor++)
				{
					pPoint->Set_Value(iPredictor, z[iPredictor]);
				}
			}
		}
	}

	//-----------------------------------------------------
	delete[](z);

	return( m_Points.Get_Count() > 1 );
}

//---------------------------------------------------------
void CGW_Multi_Regression_Grid::Finalize(void)
{
	m_Points.Destroy();

	m_Search.Destroy();

	m_y.Destroy();
	m_z.Destroy();
	m_w.Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression_Grid::On_Execute(void)
{
	int		i;

	//-----------------------------------------------------
	m_pPredictors	= Parameters("PREDICTORS")->asGridList();
	m_pRegression	= Parameters("REGRESSION")->asGrid    ();
	m_pQuality		= Parameters("QUALITY"   )->asGrid    ();
	m_pSlopes		= Parameters("SLOPES"    )->asGridList();

	m_nPoints_Min	= Parameters("SEARCH_POINTS_MIN")->asInt   ();
	m_nPoints_Max	= Parameters("SEARCH_POINTS_ALL")->asInt   () == 0
					? Parameters("SEARCH_POINTS_MAX")->asInt   () : 0;
	m_Radius		= Parameters("SEARCH_RANGE"     )->asInt   () == 0
					? Parameters("SEARCH_RADIUS"    )->asDouble() : 0.0;
	m_Direction		= Parameters("SEARCH_DIRECTION" )->asInt   () == 0 ? -1 : 4;

	m_Weighting.Set_Parameters(Parameters("WEIGHTING")->asParameters());

	//-----------------------------------------------------
	if( !Initialize() )
	{
		Finalize();

		return( false );
	}

	if( (m_nPoints_Max > 0 || m_Radius > 0.0) && !m_Search.Create(&m_Points, -1) )
	{
		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	m_pRegression->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_Points.Get_Name(), _TL("GWR Regression")));
	m_pQuality   ->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_Points.Get_Name(), _TL("GWR Quality")));

	m_pSlopes->Del_Items();

	if( Parameters("PARAMETERS")->asBool() )
	{
		CSG_Grid	*pGrid;

		m_pSlopes->Add_Item(pGrid = SG_Create_Grid(*Get_System()));
		pGrid->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_Points.Get_Name(), _TL("Intercept")));

		for(i=0; i<m_pPredictors->Get_Count(); i++)
		{
			m_pSlopes->Add_Item(pGrid = SG_Create_Grid(*Get_System()));
			pGrid->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_Points.Get_Name(), m_pPredictors->asGrid(i)->Get_Name()));
		}
	}

	//-----------------------------------------------------
	int	nPoints_Max	= m_nPoints_Max > 0 ? m_nPoints_Max : m_Points.Get_Count();

	m_y.Create(1 + m_pPredictors->Get_Count(), nPoints_Max);
	m_z.Create(nPoints_Max);
	m_w.Create(nPoints_Max);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bNoData	= false;

			for(i=0; !bNoData && i<m_pPredictors->Get_Count(); i++)
			{
				if( m_pPredictors->asGrid(i)->is_NoData(x, y) )
				{
					bNoData	= true;
				}
			}

			if( bNoData || !Get_Regression(x, y) )
			{
				m_pRegression->Set_NoData(x, y);
				m_pQuality   ->Set_NoData(x, y);

				for(i=0; i<m_pSlopes->Get_Count(); i++)
				{
					m_pSlopes->asGrid(i)->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	Set_Residuals();

	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGW_Multi_Regression_Grid::Set_Variables(int x, int y)
{
	int			nPoints;
	TSG_Point	Point;
	CSG_Shape	*pPoint;

	Point	= Get_System()->Get_Grid_to_World(x, y);
	nPoints	= m_Search.is_Okay() ? m_Search.Select_Nearest_Points(Point.x, Point.y, m_nPoints_Max, m_Radius, m_Direction) : m_Points.Get_Count();

	for(int iPoint=0; iPoint<nPoints; iPoint++)
	{
		if( m_Search.is_Okay() )
		{
			double	ix, iy, iz;

			m_Search.Get_Selected_Point(iPoint, ix, iy, iz);

			pPoint	= m_Points.Get_Shape((int)iz);
		}
		else
		{
			pPoint	= m_Points.Get_Shape(iPoint);
		}

		m_z[iPoint]	= pPoint->asDouble(0);
		m_w[iPoint]	= m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0)));

		for(int iPredictor=0; iPredictor<m_pPredictors->Get_Count(); iPredictor++)
		{
			m_y[iPoint][iPredictor]	= pPoint->asDouble(1 + iPredictor);
		}
	}

	return( nPoints );
}

//---------------------------------------------------------
bool CGW_Multi_Regression_Grid::Get_Regression(int x, int y)
{
	int		nPoints	= Set_Variables(x, y);

	if( nPoints < m_nPoints_Min )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			i;
	double		zMean, zr, rss, tss;
	CSG_Vector	b, z;
	CSG_Matrix	Y, YtW;

	//-----------------------------------------------------
	z  .Create(nPoints);
	Y  .Create(1 + m_pPredictors->Get_Count(), nPoints);
	YtW.Create(nPoints, 1 + m_pPredictors->Get_Count());

	for(i=0, zMean=0.0; i<nPoints; i++)
	{
		Y  [i][0]	= 1.0;
		YtW[0][i]	= m_w[i];

		for(int j=0; j<m_pPredictors->Get_Count(); j++)
		{
			Y  [i][j + 1]	= m_y[i][j];
			YtW[j + 1][i]	= m_y[i][j] * m_w[i];
		}

		zMean		+= (z[i] = m_z[i]);
	}

	//-----------------------------------------------------
	b		= (YtW * Y).Get_Inverse() * (YtW * z);

	zMean	/= nPoints;

	for(i=0, rss=0.0, tss=0.0; i<nPoints; i++)
	{
		zr	= b[0];

		for(int j=0; j<m_pPredictors->Get_Count(); j++)
		{
			zr	+= b[j + 1] * m_y[i][j];
		}

		rss	+= m_w[i] * SG_Get_Square(m_z[i] - zr);
		tss	+= m_w[i] * SG_Get_Square(m_z[i] - zMean);
	}

	m_pQuality  ->Set_Value(x, y, tss > 0.0 ? (tss - rss) / tss : 0.0);

	for(i=0; i<m_pSlopes->Get_Count(); i++)
	{
		m_pSlopes->asGrid(i)->Set_Value(x, y, b[i]);
	}

	//-----------------------------------------------------
	zr	= b[0];

	for(i=0; i<m_pPredictors->Get_Count(); i++)
	{
		zr	+= b[i + 1] * m_pPredictors->asGrid(i)->asDouble(x, y);
	}

	m_pRegression->Set_Value(x, y, zr);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression_Grid::Set_Residuals(void)
{
	CSG_Shapes	*pResiduals	= Parameters("RESIDUALS")->asShapes();

	if( !pResiduals || !m_pRegression )
	{
		return( false );
	}

	//-----------------------------------------------------
	pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), m_Points.Get_Name(), _TL("Residuals")));
	pResiduals->Add_Field(m_Points.Get_Field_Name(0), SG_DATATYPE_Double);
	pResiduals->Add_Field("TREND"	, SG_DATATYPE_Double);
	pResiduals->Add_Field("RESIDUAL", SG_DATATYPE_Double);

	//-------------------------------------------------
	for(int iShape=0; iShape<m_Points.Get_Count() && Set_Progress(iShape, m_Points.Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= m_Points.Get_Shape(iShape);
		double		 zShape	= pShape->asDouble(0);

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
