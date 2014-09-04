/**********************************************************
 * Version $Id: gw_multi_regression_points.cpp 911 2011-02-14 16:38:15Z reklov_w $
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
#include "gw_multi_regression_points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_ARRAY_FREE(A)		if( A ) { SG_Free (A); A = NULL; }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGW_Multi_Regression_Points::CGW_Multi_Regression_Points(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("GWR for Multiple Predictors"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Geographically Weighted Regression for multiple predictors. "
		"Regression details are stored in a copy of input points.\n"
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

	Parameters.Add_Table_Fields(
		pNode	, "PREDICTORS"	, _TL("Predictors"),
		_TL("")
	);

	Parameters.Add_Shapes(
		NULL	, "REGRESSION"		, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

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
int CGW_Multi_Regression_Points::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( 1 );
}

//---------------------------------------------------------
int CGW_Multi_Regression_Points::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
bool CGW_Multi_Regression_Points::Get_Predictors(void)
{
	int				i, iDependent;
	CSG_Shapes		*pPoints;

	//-----------------------------------------------------
	m_pPoints		= Parameters("REGRESSION"       )->asShapes();

	m_nPoints_Min	= Parameters("SEARCH_POINTS_MIN")->asInt   ();
	m_nPoints_Max	= Parameters("SEARCH_POINTS_ALL")->asInt   () == 0
					? Parameters("SEARCH_POINTS_MAX")->asInt   () : 0;
	m_Radius		= Parameters("SEARCH_RANGE"     )->asInt   () == 0
					? Parameters("SEARCH_RADIUS"    )->asDouble() : 0.0;
	m_Direction		= Parameters("SEARCH_DIRECTION" )->asInt   () == 0 ? -1 : 4;

	m_Weighting.Set_Parameters(&Parameters);

	//-----------------------------------------------------
	iDependent	= Parameters("DEPENDENT")->asInt   ();
	pPoints		= Parameters("POINTS"   )->asShapes();

	m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s.%s [%s]"), pPoints->Get_Name(), pPoints->Get_Field_Name(iDependent), _TL("GWR")));
	m_pPoints->Add_Field(pPoints->Get_Field_Name(iDependent), SG_DATATYPE_Double);

	//-----------------------------------------------------
	CSG_Parameter_Table_Fields	*pFields	= Parameters("PREDICTORS")->asTableFields();

	if( (m_nPredictors = pFields->Get_Count()) <= 0 )
	{
		Error_Set(_TL("no predictors have been selected"));

		return( false );
	}

	//-----------------------------------------------------
	for(i=0; i<m_nPredictors; i++)
	{
		m_pPoints->Add_Field(pPoints->Get_Field_Name(pFields->Get_Index(i)), SG_DATATYPE_Double);
	}

	m_pPoints->Add_Field("DEPENDENT" , SG_DATATYPE_Double);	// m_nPredictors + 0
	m_pPoints->Add_Field("R2"        , SG_DATATYPE_Double);	// m_nPredictors + 1
	m_pPoints->Add_Field("REGRESSION", SG_DATATYPE_Double);	// m_nPredictors + 2
	m_pPoints->Add_Field("RESIDUAL"  , SG_DATATYPE_Double);	// m_nPredictors + 3
	m_pPoints->Add_Field("INTERCEPT" , SG_DATATYPE_Double);	// m_nPredictors + 4

	for(int iPoint=0; iPoint<pPoints->Get_Count(); iPoint++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(iPoint);

		bool	bAdd	= !pPoint->is_NoData(iDependent);

		for(i=0; bAdd && i<m_nPredictors; i++)
		{
			bAdd	= !pPoint->is_NoData(pFields->Get_Index(i));
		}

		if( bAdd )
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

	if( (m_nPoints_Max > 0 || m_Radius > 0.0) && !m_Search.Create(m_pPoints, -1) )
	{
		Error_Set(_TL("failed to create searche engine"));

		return( false );
	}

	//-----------------------------------------------------
	int	nPoints_Max	= m_nPoints_Max > 0 ? m_nPoints_Max : m_pPoints->Get_Count();

	m_y.Create(1 + m_nPredictors, nPoints_Max);
	m_z.Create(nPoints_Max);
	m_w.Create(nPoints_Max);

	return( true );
}

//---------------------------------------------------------
void CGW_Multi_Regression_Points::Finalize(void)
{
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
bool CGW_Multi_Regression_Points::On_Execute(void)
{
	//-----------------------------------------------------
	if( !Get_Predictors() )
	{
		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	for(int iPoint=0; iPoint<m_pPoints->Get_Count() && Set_Progress(iPoint, m_pPoints->Get_Count()); iPoint++)
	{
		if( !Get_Regression(m_pPoints->Get_Shape(iPoint)) )
		{
		}
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGW_Multi_Regression_Points::Set_Variables(const TSG_Point &Point)
{
	int			iPoint, jPoint, nPoints, iPredictor;
	CSG_Shape	*pPoint;

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

		m_z[iPoint]	= pPoint->asDouble(m_nPredictors);
		m_w[iPoint]	= m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0)));

		for(iPredictor=0; iPredictor<m_nPredictors; iPredictor++)
		{
			m_y[iPoint][iPredictor]	= pPoint->asDouble(iPredictor);
		}
	}

	return( nPoints );
}

//---------------------------------------------------------
bool CGW_Multi_Regression_Points::Get_Regression(CSG_Shape *pPoint)
{
	int		nPoints	= Set_Variables(pPoint->Get_Point(0));

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
	Y  .Create(1 + m_nPredictors, nPoints);
	YtW.Create(nPoints, 1 + m_nPredictors);

	for(i=0, zMean=0.0; i<nPoints; i++)
	{
		Y  [i][0]	= 1.0;
		YtW[0][i]	= m_w[i];

		for(int j=0; j<m_nPredictors; j++)
		{
			Y  [i][j + 1]	= m_y[i][j];
			YtW[j + 1][i]	= m_y[i][j] * m_w[i];
		}

		zMean		+= (z[i] = m_z[i]);
	}

	//-----------------------------------------------------
	b		= (YtW * Y).Get_Inverse() * (YtW * m_z);

	zMean	/= nPoints;

	for(i=0, rss=0.0, tss=0.0; i<nPoints; i++)
	{
		double	zr	= b[0];

		for(int j=0; j<m_nPredictors; j++)
		{
			zr	+= b[j + 1] * m_y[i][j];
		}

		rss	+= m_w[i] * SG_Get_Square(m_z[i] - zr);
		tss	+= m_w[i] * SG_Get_Square(m_z[i] - zMean);
	}

	//-----------------------------------------------------
	double	Regression	= b[0];

	for(i=1; i<=m_nPredictors; i++)
	{
		Regression	+= b[i] * pPoint->asDouble(i);
	}

	//-----------------------------------------------------
	pPoint->Set_Value(1 + m_nPredictors, tss > 0.0 ? (tss - rss) / tss : 0.0);	// R2
	pPoint->Set_Value(2 + m_nPredictors, Regression);							// Regression
	pPoint->Set_Value(3 + m_nPredictors, pPoint->asDouble(0) - Regression);		// Residual

	for(i=0; i<=m_nPredictors; i++)
	{
		pPoint->Set_Value(4 + m_nPredictors + i, b[i]);
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
