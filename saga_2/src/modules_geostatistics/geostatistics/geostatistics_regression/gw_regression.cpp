
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
//                   gw_regression.cpp                   //
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
#include "gw_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGW_Regression::CGW_Regression(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Geographically Weighted Regression"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Reference:\n"
		" - Lloyd, C. (2010): Spatial Data Analysis - An Introduction for GIS Users. Oxford, 206p.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "INTERCEPT"	, _TL("Intercept"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "SLOPE"		, _TL("Slope"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "QUALITY"		, _TL("Coefficient of Determination"),
		_TL("")
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

	Parameters.Add_Table_Field(
		pNode	, "PREDICTOR"	, _TL("Predictor"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grids"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("user defined"),
			_TL("grid")
		), 0
	);

	m_Grid_Target.Add_Parameters_User(Add_Parameters("USER", _TL("User Defined Grid")	, _TL("")));
	m_Grid_Target.Add_Parameters_Grid(Add_Parameters("GRID", _TL("Choose Grid")			, _TL("")), false);
	m_Grid_Target.Add_Grid_Parameter(SG_T("QUALITY")   , _TL("Quality")  , false);
	m_Grid_Target.Add_Grid_Parameter(SG_T("INTERCEPT") , _TL("Intercept"), false);
	m_Grid_Target.Add_Grid_Parameter(SG_T("SLOPE")     , _TL("Slope")    , false);

	//-----------------------------------------------------
	Parameters.Add_Parameters(
		NULL	, "WEIGHTING"	, _TL("Weighting"),
		_TL("")
	)->asParameters()->Assign(m_Weighting.Get_Parameters());

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
int CGW_Regression::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Regression::On_Execute(void)
{
	//-----------------------------------------------------
	m_pPoints		= Parameters("POINTS")		->asShapes();
	m_iDependent	= Parameters("DEPENDENT")	->asInt();
	m_iPredictor	= Parameters("PREDICTOR")	->asInt();
	m_Radius		= Parameters("RANGE")		->asInt() == 0 ? Parameters("RADIUS")   ->asDouble() : 0.0;
	m_Mode			= Parameters("MODE")		->asInt();
	m_nPoints_Max	= Parameters("NPOINTS")		->asInt() == 0 ? Parameters("MAXPOINTS")->asInt()    : 0;
	m_nPoints_Min	= Parameters("MINPOINTS")	->asInt();

	m_Weighting.Set_Parameters(Parameters("WEIGHTING")->asParameters());

	//-----------------------------------------------------
	if( (m_nPoints_Max > 0 || m_Radius > 0.0) && !m_Search.Create(m_pPoints, -1) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pIntercept	= NULL;
	m_pSlope		= NULL;
	m_pQuality		= NULL;

	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( m_Grid_Target.Init_User(m_pPoints->Get_Extent()) && Dlg_Parameters("USER") )
		{
			m_pIntercept	= m_Grid_Target.Get_User();
			m_pSlope		= m_Grid_Target.Get_User();
			m_pQuality		= m_Grid_Target.Get_User();
		}
		break;

	case 1:	// grid...
		if( Dlg_Parameters("GRID") )
		{
			m_pIntercept	= m_Grid_Target.Get_Grid(SG_T("INTERCEPT"));
			m_pSlope		= m_Grid_Target.Get_Grid(SG_T("SLOPE"));
			m_pQuality		= m_Grid_Target.Get_Grid(SG_T("QUALITY"));
		}
		break;
	}

	if( m_pIntercept == NULL )
	{
		m_Search.Destroy();

		return( false );
	}

	m_pIntercept->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_pPoints->Get_Name(), _TL("GWR Intercept")));
	m_pSlope    ->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_pPoints->Get_Name(), _TL("GWR Slope")));
	m_pQuality  ->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_pPoints->Get_Name(), _TL("GWR Quality")));

	Parameters("INTERCEPT")->Set_Value(m_pIntercept);
	Parameters("SLOPE")    ->Set_Value(m_pSlope);
	Parameters("QUALITY")  ->Set_Value(m_pQuality);

	//-----------------------------------------------------
	int	nPoints_Max	= m_nPoints_Max > 0 ? m_nPoints_Max : m_pPoints->Get_Count();

	m_y.Create(nPoints_Max);
	m_z.Create(nPoints_Max);
	m_w.Create(nPoints_Max);

	//-----------------------------------------------------
	for(int y=0; y<m_pIntercept->Get_NY() && Set_Progress(y, m_pIntercept->Get_NY()); y++)
	{
		for(int x=0; x<m_pIntercept->Get_NX(); x++)
		{
			if( !Get_Regression(x, y) )
			{
				m_pIntercept->Set_NoData(x, y);
				m_pSlope    ->Set_NoData(x, y);
				m_pQuality  ->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
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
int CGW_Regression::Set_Variables(int x, int y)
{
	int			nPoints;
	TSG_Point	Point;
	CSG_Shape	*pPoint;

	Point	= m_pIntercept->Get_System().Get_Grid_to_World(x, y);
	nPoints	= m_Search.is_Okay() ? m_Search.Select_Nearest_Points(Point.x, Point.y, m_nPoints_Max, m_Radius, m_Mode == 0 ? -1 : 4) : m_pPoints->Get_Count();

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

		m_z[iPoint]	= pPoint->asDouble(m_iDependent);
		m_y[iPoint]	= pPoint->asDouble(m_iPredictor);
		m_w[iPoint]	= m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0)));
	}

	return( nPoints );
}

//---------------------------------------------------------
bool CGW_Regression::Get_Regression(int x, int y)
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

	m_pIntercept->Set_Value(x, y, b[0]);
	m_pSlope    ->Set_Value(x, y, b[1]);
	m_pQuality  ->Set_Value(x, y, (tss - rss) / tss);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
