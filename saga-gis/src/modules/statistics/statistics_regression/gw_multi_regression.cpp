/**********************************************************
 * Version $Id: gw_multi_regression.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                gw_multi_regression.cpp                //
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
#include "gw_multi_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_ARRAY_DELETE(A)		if( A ) { delete[](A); A = NULL; }
#define SG_ARRAY_FREE(A)		if( A ) { SG_Free (A); A = NULL; }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGW_Multi_Regression::CGW_Multi_Regression(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Geographically Weighted Multiple Regression"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Reference:\n"
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

	Parameters.Add_Parameters(
		pNode	, "PREDICTORS"	, _TL("Predictors"),
		_TL("")
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false);

	m_Grid_Target.Add_Grid("QUALITY"  , _TL("Quality"  ), false);
	m_Grid_Target.Add_Grid("INTERCEPT", _TL("Intercept"), false);

	Parameters.Add_Grid_List(
		NULL	, "SLOPES"		, _TL("Slopes"),
		_TL(""),
		PARAMETER_OUTPUT
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

	//-----------------------------------------------------
	m_iPredictor	= NULL;
	m_pSlopes		= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGW_Multi_Regression::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "POINTS") )
	{
		CSG_Shapes		*pPoints		= pParameters->Get_Parameter("POINTS"    )->asShapes();
		CSG_Parameters	*pAttributes	= pParameters->Get_Parameter("PREDICTORS")->asParameters();

		pAttributes->Destroy();
		pAttributes->Set_Name(_TL("Predictors"));

		if( pPoints )
		{
			for(int i=0; pPoints && i<pPoints->Get_Field_Count(); i++)
			{
				if( SG_Data_Type_is_Numeric(pPoints->Get_Field_Type(i)) )
				{
					pAttributes->Add_Value(
						NULL, SG_Get_String(i, 0), pPoints->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, false
					);
				}
			}

			m_Grid_Target.Set_User_Defined(pParameters, pPoints->Get_Extent());
		}
	}

	return( m_Grid_Target.On_Parameter_Changed(pParameters, pParameter) ? 1 : 0 );
}

//---------------------------------------------------------
int CGW_Multi_Regression::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
bool CGW_Multi_Regression::Get_Predictors(void)
{
	CSG_Shapes		*pPoints		= Parameters("POINTS"    )->asShapes();
	CSG_Parameters	*pAttributes	= Parameters("PREDICTORS")->asParameters();

	m_nPredictors	= 0;
	m_iPredictor	= new int[pPoints->Get_Field_Count()];

	for(int i=0; i<pAttributes->Get_Count(); i++)
	{
		if( pAttributes->Get_Parameter(i)->asBool() )
		{
			m_iPredictor[m_nPredictors++]	= CSG_String(pAttributes->Get_Parameter(i)->Get_Identifier()).asInt();
		}
	}

	return( m_nPredictors > 0 );
}

//---------------------------------------------------------
void CGW_Multi_Regression::Finalize(void)
{
	SG_ARRAY_DELETE(m_iPredictor);

	SG_ARRAY_FREE  (m_pSlopes);

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
bool CGW_Multi_Regression::On_Execute(void)
{
	//-----------------------------------------------------
	m_pPoints		= Parameters("POINTS"           )->asShapes();
	m_iDependent	= Parameters("DEPENDENT"        )->asInt   ();

	m_nPoints_Min	= Parameters("SEARCH_POINTS_MIN")->asInt   ();
	m_nPoints_Max	= Parameters("SEARCH_POINTS_ALL")->asInt   () == 0
					? Parameters("SEARCH_POINTS_MAX")->asInt   () : 0;
	m_Radius		= Parameters("SEARCH_RANGE"     )->asInt   () == 0
					? Parameters("SEARCH_RADIUS"    )->asDouble() : 0.0;
	m_Direction		= Parameters("SEARCH_DIRECTION" )->asInt   () == 0 ? -1 : 4;

	m_Weighting.Set_Parameters(Parameters("WEIGHTING")->asParameters());

	//-----------------------------------------------------
	if( !Get_Predictors() )
	{
		Finalize();

		return( false );
	}

	if( (m_nPoints_Max > 0 || m_Radius > 0.0) && !m_Search.Create(m_pPoints, -1) )
	{
		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	m_pQuality		= m_Grid_Target.Get_Grid("QUALITY"  );
	m_pIntercept	= m_Grid_Target.Get_Grid("INTERCEPT");

	if( !m_pQuality || !m_pIntercept )
	{
		Finalize();

		return( false );
	}

	m_pQuality  ->Set_Name(CSG_String::Format(SG_T("%s (%s)"), Parameters("DEPENDENT")->asString(), _TL("GWR Quality")));
	m_pIntercept->Set_Name(CSG_String::Format(SG_T("%s (%s)"), Parameters("DEPENDENT")->asString(), _TL("GWR Intercept")));

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pSlopes	= Parameters("SLOPES")->asGridList();

	m_pSlopes	= (CSG_Grid **)SG_Calloc(m_nPredictors, sizeof(CSG_Grid *));

	for(int i=0; i<m_nPredictors; i++)
	{
		pSlopes->Add_Item(m_pSlopes[i] = SG_Create_Grid(m_pQuality->Get_System()));

		m_pSlopes[i]->Set_Name(CSG_String::Format(SG_T("%s (%s)"), Parameters("DEPENDENT")->asString(), m_pPoints->Get_Field_Name(m_iPredictor[i])));
	}

	//-----------------------------------------------------
	int	nPoints_Max	= m_nPoints_Max > 0 ? m_nPoints_Max : m_pPoints->Get_Count();

	m_y.Create(1 + m_nPredictors, nPoints_Max);
	m_z.Create(nPoints_Max);
	m_w.Create(nPoints_Max);

	//-----------------------------------------------------
	for(int y=0; y<m_pIntercept->Get_NY() && Set_Progress(y, m_pIntercept->Get_NY()); y++)
	{
		for(int x=0; x<m_pIntercept->Get_NX(); x++)
		{
			if( !Get_Regression(x, y) )
			{
				m_pQuality  ->Set_NoData(x, y);
				m_pIntercept->Set_NoData(x, y);

				for(int i=0; i<m_nPredictors; i++)
				{
					m_pSlopes[i]->Set_NoData(x, y);
				}
			}
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
int CGW_Multi_Regression::Set_Variables(int x, int y)
{
	int			iPoint, jPoint, nPoints, iPredictor;
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

		if( !pPoint->is_NoData(m_iDependent) )
		{
			m_z[jPoint]	= pPoint->asDouble(m_iDependent);
			m_w[jPoint]	= m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0)));

			for(iPredictor=0; iPredictor<m_nPredictors; iPredictor++)
			{
				if( !pPoint->is_NoData(m_iPredictor[iPredictor]) )
				{
					m_y[jPoint][iPredictor]	= pPoint->asDouble(m_iPredictor[iPredictor]);
				}
				else
				{
					iPredictor	= m_nPredictors + 1;
				}
			}

			if( iPredictor == m_nPredictors )
			{
				jPoint++;
			}
		}
	}

	return( jPoint );
}

//---------------------------------------------------------
bool CGW_Multi_Regression::Get_Regression(int x, int y)
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
	b		= (YtW * Y).Get_Inverse() * (YtW * z);

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

	m_pQuality  ->Set_Value(x, y, tss > 0.0 ? (tss - rss) / tss : 0.0);

	m_pIntercept->Set_Value(x, y, b[0]);

	for(i=0; i<m_nPredictors; i++)
	{
		m_pSlopes[i]->Set_Value(x, y, b[i + 1]);
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
