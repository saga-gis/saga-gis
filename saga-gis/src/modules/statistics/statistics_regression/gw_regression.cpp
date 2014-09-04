/**********************************************************
 * Version $Id: gw_regression.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 statistics_regression                 //
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
	Set_Name		(_TL("GWR for Single Predictor (Gridded Model Output)"));

	Set_Author		("O.Conrad (c) 2010");

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
		" - Lloyd, C. (2010): Spatial Data Analysis - An Introduction for GIS Users. Oxford, 206p.\n"
	));

	//-----------------------------------------------------
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
	m_Grid_Target.Create(SG_UI_Get_Window_Main() ? &Parameters : Add_Parameters("TARGET", _TL("Target System"), _TL("")));

	m_Grid_Target.Add_Grid("INTERCEPT", _TL("Intercept"), false);
	m_Grid_Target.Add_Grid("SLOPE"    , _TL("Slope"    ), false);
	m_Grid_Target.Add_Grid("QUALITY"  , _TL("Quality"  ), false);

	//-----------------------------------------------------
	m_Weighting.Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Weighting.Create_Parameters(&Parameters, false);

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
int CGW_Regression::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "POINTS") && pParameter->asShapes() )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes()->Get_Extent());
	}

	return( m_Grid_Target.On_Parameter_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CGW_Regression::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SEARCH_RANGE") )
	{
		pParameters->Set_Enabled("SEARCH_RADIUS"    , pParameter->asInt() == 0);	// local
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SEARCH_POINTS_ALL") )
	{
		pParameters->Set_Enabled("SEARCH_POINTS_MAX", pParameter->asInt() == 0);	// maximum number of points
		pParameters->Set_Enabled("SEARCH_DIRECTION" , pParameter->asInt() == 0);	// maximum number of points per quadrant
	}

	m_Weighting.Enable_Parameters(pParameters);

	return( m_Grid_Target.On_Parameters_Enable(pParameters, pParameter) ? 1 : 0 );
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
	m_pPoints		= Parameters("POINTS"   )->asShapes();
	m_iDependent	= Parameters("DEPENDENT")->asInt   ();
	m_iPredictor	= Parameters("PREDICTOR")->asInt   ();

	m_nPoints_Min	= Parameters("SEARCH_POINTS_MIN")->asInt   ();
	m_nPoints_Max	= Parameters("SEARCH_POINTS_ALL")->asInt   () == 0
					? Parameters("SEARCH_POINTS_MAX")->asInt   () : 0;
	m_Radius		= Parameters("SEARCH_RANGE"     )->asInt   () == 0
					? Parameters("SEARCH_RADIUS"    )->asDouble() : 0.0;
	m_Direction		= Parameters("SEARCH_DIRECTION" )->asInt   () == 0 ? -1 : 4;

	m_Weighting.Set_Parameters(&Parameters);

	//-----------------------------------------------------
	if( (m_nPoints_Max > 0 || m_Radius > 0.0) && !m_Search.Create(m_pPoints, -1) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Grid_Target.Set_User_Defined(Get_Parameters("TARGET"), m_pPoints->Get_Extent());	Dlg_Parameters("TARGET");	// if called from saga_cmd

	m_pQuality		= m_Grid_Target.Get_Grid("QUALITY"  );
	m_pSlope		= m_Grid_Target.Get_Grid("SLOPE"    );
	m_pIntercept	= m_Grid_Target.Get_Grid("INTERCEPT");

	if( !m_pIntercept || !m_pSlope || !m_pQuality )
	{
		m_Search.Destroy();

		return( false );
	}

	m_pIntercept->Set_Name(CSG_String::Format(SG_T("%s (%s)"), Parameters("DEPENDENT")->asString(), _TL("GWR Intercept")));
	m_pSlope    ->Set_Name(CSG_String::Format(SG_T("%s (%s)"), Parameters("DEPENDENT")->asString(), _TL("GWR Slope")));
	m_pQuality  ->Set_Name(CSG_String::Format(SG_T("%s (%s)"), Parameters("DEPENDENT")->asString(), _TL("GWR Quality")));

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
	int			iPoint, jPoint, nPoints;
	TSG_Point	Point;
	CSG_Shape	*pPoint;

	Point	= m_pIntercept->Get_System().Get_Grid_to_World(x, y);
	nPoints	= m_Search.is_Okay() ? (int)m_Search.Select_Nearest_Points(Point.x, Point.y, m_nPoints_Max, m_Radius, m_Direction) : m_pPoints->Get_Count();

	for(iPoint=0, jPoint=0; iPoint<nPoints; iPoint++)
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

		if( !pPoint->is_NoData(m_iDependent) && !pPoint->is_NoData(m_iPredictor) )
		{
			m_z[jPoint]	= pPoint->asDouble(m_iDependent);
			m_y[jPoint]	= pPoint->asDouble(m_iPredictor);
			m_w[jPoint]	= m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0)));

			jPoint++;
		}
	}

	return( jPoint );
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
