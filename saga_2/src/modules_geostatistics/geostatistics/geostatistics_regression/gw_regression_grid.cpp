
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
#include "gw_regression_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GRID_INIT(g, s)			if( g ) { g->Set_Name(CSG_String::Format(SG_T("%s (%s - %s)"), m_pPoints->Get_Name(), s, m_pPredictor->Get_Name())); }
#define GRID_SET_NODATA(g, x, y)	if( g ) { g->Set_NoData(x, y); }
#define GRID_SET_VALUE(g, x, y, z)	if( g ) { g->Set_Value(x, y, z); }


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
	Set_Name		(_TL("Geographically Weighted Regression (Points/Grid)"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Reference:\n"
		" - Lloyd, C. (2010): Spatial Data Analysis - An Introduction for GIS Users. Oxford, 206p.\n"
	));

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
	pNode	= Parameters.Add_Choice(
		NULL	, "WEIGHTING"	, _TL("Distance Weighting"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("inverse distance to a power"),
			_TL("exponential weighting scheme"),
			_TL("gaussian weighting scheme"),
			_TL("none (moving window regression)")
		), 2
	);

	Parameters.Add_Value(
		pNode	, "POWER"		, _TL("Inverse Distance Power"),
		_TL(""),
		PARAMETER_TYPE_Double	, 2.0
	);

	Parameters.Add_Value(
		pNode	, "BANDWIDTH"	, _TL("Exponential and Gaussian Weighting Bandwidth"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1.0, 0.0, true
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "RANGE"		, _TL("Search Range"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("search radius (local)"),
			_TL("no search radius (global)")
		)
	);

	Parameters.Add_Value(
		pNode	, "RADIUS"		, _TL("Search Radius"),
		_TL(""),
		PARAMETER_TYPE_Double	, 100.0
	);

	Parameters.Add_Choice(
		pNode	, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("all directions"),
			_TL("quadrants")
		)
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "NPOINTS"		, _TL("Number of Points"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("maximum number of observations"),
			_TL("all points")
		)
	);

	Parameters.Add_Value(
		pNode	, "MAXPOINTS"	, _TL("Maximum Number of Observations"),
		_TL(""),
		PARAMETER_TYPE_Int		, 10, 2, true
	);

	Parameters.Add_Value(
		NULL	, "MINPOINTS"	, _TL("Minimum Number of Observations"),
		_TL(""),
		PARAMETER_TYPE_Int		,  4, 2, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Regression_Grid::On_Execute(void)
{
	//-----------------------------------------------------
	m_pPredictor	= Parameters("PREDICTOR")	->asGrid();
	m_pRegression	= Parameters("REGRESSION")	->asGrid();
	m_pQuality		= Parameters("QUALITY")		->asGrid();
	m_pIntercept	= Parameters("INTERCEPT")	->asGrid();
	m_pSlope		= Parameters("SLOPE")		->asGrid();
	m_pPoints		= Parameters("POINTS")		->asShapes();
	m_iDependent	= Parameters("DEPENDENT")	->asInt();
	m_Weighting		= Parameters("WEIGHTING")	->asInt();
	m_Power			= Parameters("POWER")		->asDouble();
	m_Bandwidth		= Parameters("BANDWIDTH")	->asDouble();
	m_Radius		= Parameters("RANGE")		->asInt() == 0 ? Parameters("RADIUS")   ->asDouble() : 0.0;
	m_Mode			= Parameters("MODE")		->asInt();
	m_nPoints_Max	= Parameters("NPOINTS")		->asInt() == 0 ? Parameters("MAXPOINTS")->asInt()    : 0;
	m_nPoints_Min	= Parameters("MINPOINTS")	->asInt();

	//-----------------------------------------------------
	if( (m_nPoints_Max > 0 || m_Radius > 0.0) && !m_Search.Create(m_pPoints, -1) )
	{
		return( false );
	}

	//-----------------------------------------------------
	GRID_INIT(m_pRegression, _TL("GWR Regression"));
	GRID_INIT(m_pQuality   , _TL("GWR Quality"));
	GRID_INIT(m_pIntercept , _TL("GWR Intercept"));
	GRID_INIT(m_pSlope     , _TL("GWR Slope"));

	//-----------------------------------------------------
	int	nPoints_Max	= m_nPoints_Max > 0 ? m_nPoints_Max : m_pPoints->Get_Count();

	m_y.Create(nPoints_Max);
	m_z.Create(nPoints_Max);
	m_w.Create(nPoints_Max);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pPredictor->is_NoData(x, y) || !Get_Regression(x, y) )
			{
				GRID_SET_NODATA(m_pRegression , x, y);
				GRID_SET_NODATA(m_pIntercept  , x, y);
				GRID_SET_NODATA(m_pSlope      , x, y);
				GRID_SET_NODATA(m_pQuality    , x, y);
			}
		}
	}

	//-----------------------------------------------------
	Set_Residuals();

	m_y.Destroy();
	m_z.Destroy();
	m_w.Destroy();

	m_Search.Destroy();

	DataObject_Update(m_pIntercept);
	DataObject_Update(m_pSlope);
	DataObject_Update(m_pQuality);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CGW_Regression_Grid::Get_Weight(double Distance)
{
	switch( m_Weighting )
	{
	default:	return( pow(1.0 + Distance, -m_Power) );
	case 1:		return( exp(-Distance / m_Bandwidth) );
	case 2:		return( exp(-0.5 * SG_Get_Square(Distance / m_Bandwidth)) );
	case 3:		return( 1.0 );
	}
}

//---------------------------------------------------------
int CGW_Regression_Grid::Set_Variables(int x, int y)
{
	int			nPoints, nTotal;
	double		z;
	TSG_Point	Point;
	CSG_Shape	*pPoint;

	Point	= Get_System()->Get_Grid_to_World(x, y);
	nPoints	= m_Search.is_Okay() ? m_Search.Select_Nearest_Points(Point.x, Point.y, m_nPoints_Max, m_Radius, m_Mode == 0 ? -1 : 4) : m_pPoints->Get_Count();
	nTotal	= 0;

	for(int iPoint=0; iPoint<nPoints; iPoint++)
	{
		if( m_Search.is_Okay() )
		{
			double	ix, iy, iz;

			m_Search.Get_Selected_Point(iPoint, ix, iy, iz);

			pPoint	= m_pPoints->Get_Shape((int)iz);
		}
		else
		{
			pPoint	= m_pPoints->Get_Shape(iPoint);
		}

		if( m_pPredictor->Get_Value(pPoint->Get_Point(0), z) )
		{
			m_w[nTotal]	= Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0)));
			m_z[nTotal]	= pPoint->asDouble(m_iDependent);
			m_y[nTotal]	= z;

			nTotal++;
		}
	}

	return( nTotal );
}

//---------------------------------------------------------
bool CGW_Regression_Grid::Get_Regression(int x, int y)
{
	int		nPoints	= Set_Variables(x, y);

	if( nPoints < m_nPoints_Min )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			i;
	double		zMean, rss, tss;
	CSG_Vector	b, z;
	CSG_Matrix	Y, YtW;

	//-----------------------------------------------------
	z  .Create(nPoints);
	Y  .Create(2, nPoints);
	YtW.Create(nPoints, 2);

	for(i=0, zMean=0.0; i<nPoints; i++)
	{
		Y  [i][0]	= 1.0;
		Y  [i][1]	= m_y[i];
		YtW[0][i]	= m_w[i];
		YtW[1][i]	= m_w[i] * m_y[i];

		zMean		+= (z[i] = m_z[i]);
	}

	//-----------------------------------------------------
	b		= (YtW * Y).Get_Inverse() * (YtW * z);

	zMean	/= nPoints;

	for(i=0, rss=0.0, tss=0.0; i<nPoints; i++)
	{
		rss	+= m_w[i] * SG_Get_Square(m_z[i] - (b[0] + b[1] * m_y[i]));
		tss	+= m_w[i] * SG_Get_Square(m_z[i] - zMean);
	}

	GRID_SET_VALUE(m_pRegression, x, y, b[0] + b[1] * m_pPredictor->asDouble(x, y));
	GRID_SET_VALUE(m_pIntercept , x, y, b[0]);
	GRID_SET_VALUE(m_pSlope     , x, y, b[1]);
	GRID_SET_VALUE(m_pQuality   , x, y, (tss - rss) / tss);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
