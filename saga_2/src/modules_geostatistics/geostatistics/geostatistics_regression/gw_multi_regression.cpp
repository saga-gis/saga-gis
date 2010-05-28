
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
	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grids"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("user defined"),
			_TL("grid")
		), 0
	);

	m_Grid_Target.Add_Parameters_User(Add_Parameters("USER", _TL("User Defined Grid")	, _TL("")), false);
	m_Grid_Target.Add_Parameters_Grid(Add_Parameters("GRID", _TL("Choose Grid")			, _TL("")), false);

	m_Grid_Target.Add_Grid_Parameter(SG_T("QUALITY")   , _TL("Quality")  , false);
	m_Grid_Target.Add_Grid_Parameter(SG_T("INTERCEPT") , _TL("Intercept"), false);

	//-----------------------------------------------------
	m_Weighting.Set_Weighting(SG_DISTWGHT_GAUSS);

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
	if( m_Grid_Target.On_User_Changed(pParameters, pParameter) )
	{
		return( true );
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("POINTS")) )
	{
		CSG_Shapes		*pPoints		= pParameters->Get_Parameter("POINTS")		->asShapes();
		CSG_Parameters	*pAttributes	= pParameters->Get_Parameter("PREDICTORS")	->asParameters();

		pAttributes->Destroy();
		pAttributes->Set_Name(_TL("Predictors"));

		for(int i=0; pPoints && i<pPoints->Get_Field_Count(); i++)
		{
			switch( pPoints->Get_Field_Type(i) )
			{
			default:	// not numeric
				break;

			case SG_DATATYPE_Byte:
			case SG_DATATYPE_Char:
			case SG_DATATYPE_Word:
			case SG_DATATYPE_Short:
			case SG_DATATYPE_DWord:
			case SG_DATATYPE_Int:
			case SG_DATATYPE_ULong:
			case SG_DATATYPE_Long:
			case SG_DATATYPE_Float:
			case SG_DATATYPE_Double:
				pAttributes->Add_Value(
					NULL, SG_Get_String(i, 0), pPoints->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, false
				);
				break;
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGW_Multi_Regression::Get_Predictors(void)
{
	int				i;
	CSG_Shapes		*pPoints		= Parameters("POINTS")		->asShapes();
	CSG_Parameters	*pAttributes	= Parameters("PREDICTORS")	->asParameters();

	m_nPredictors	= 0;
	m_iPredictor	= new int[pPoints->Get_Field_Count()];

	for(i=0; i<pAttributes->Get_Count(); i++)
	{
		if( pAttributes->Get_Parameter(i)->asBool() )
		{
			m_iPredictor[m_nPredictors++]	= CSG_String(pAttributes->Get_Parameter(i)->Get_Identifier()).asInt();
		}
	}

	CSG_Parameters	*pGrids	= Get_Parameters("GRID"), Tmp;

	Tmp.Assign(pGrids);

	pGrids->Create(this, Tmp.Get_Name(), Tmp.Get_Description(), Tmp.Get_Identifier(), false);
	m_Grid_Target.Add_Grid_Parameter(SG_T("QUALITY")   , _TL("Quality")  , false);
	m_Grid_Target.Add_Grid_Parameter(SG_T("INTERCEPT") , _TL("Intercept"), false);

	pGrids->Get_Parameter("QUALITY")->Get_Parent()->asGrid_System()->Assign(*Tmp("QUALITY")->Get_Parent()->asGrid_System());
	pGrids->Get_Parameter("QUALITY")  ->Set_Value(Tmp("QUALITY")  ->asGrid());
	pGrids->Get_Parameter("INTERCEPT")->Set_Value(Tmp("INTERCEPT")->asGrid());

	for(i=0; i<m_nPredictors; i++)
	{
		m_Grid_Target.Add_Grid_Parameter(SG_Get_String(i, 0),
			CSG_String::Format(SG_T("%s [%s]"), _TL("Slope"), pPoints->Get_Field_Name(m_iPredictor[i])), false
		);

		if( Tmp(SG_Get_String(i, 0)) )
		{
			pGrids->Get_Parameter(SG_Get_String(i, 0))->Set_Value(Tmp(SG_Get_String(i, 0))->asGrid());
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
	int		i;

	//-----------------------------------------------------
	m_pPoints		= Parameters("POINTS")		->asShapes();
	m_iDependent	= Parameters("DEPENDENT")	->asInt();
	m_Radius		= Parameters("RANGE")		->asInt() == 0 ? Parameters("RADIUS")   ->asDouble() : 0.0;
	m_Mode			= Parameters("MODE")		->asInt();
	m_nPoints_Max	= Parameters("NPOINTS")		->asInt() == 0 ? Parameters("MAXPOINTS")->asInt()    : 0;
	m_nPoints_Min	= Parameters("MINPOINTS")	->asInt();

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
	m_pQuality		= NULL;
	m_pIntercept	= NULL;
	m_pSlopes		= (CSG_Grid **)SG_Calloc(m_nPredictors, sizeof(CSG_Grid *));

	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( m_Grid_Target.Init_User(m_pPoints->Get_Extent()) && Dlg_Parameters("USER") )
		{
			m_pQuality		= m_Grid_Target.Get_User(SG_T("QUALITY"  ));
			m_pIntercept	= m_Grid_Target.Get_User(SG_T("INTERCEPT"));

			for(i=0; i<m_nPredictors; i++)
			{
				m_pSlopes[i]	= m_Grid_Target.Get_User(SG_Get_String(i, 0));
			}
		}
		break;

	case 1:	// grid...
		if( Dlg_Parameters("GRID") )
		{
			m_pQuality		= m_Grid_Target.Get_Grid(SG_T("QUALITY"  ));
			m_pIntercept	= m_Grid_Target.Get_Grid(SG_T("INTERCEPT"));

			for(i=0; i<m_nPredictors; i++)
			{
				m_pSlopes[i]	= m_Grid_Target.Get_Grid(SG_Get_String(i, 0));
			}
		}
		break;
	}

	if( m_pQuality == NULL )
	{
		Finalize();

		return( false );
	}

	m_pQuality  ->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_pPoints->Get_Name(), _TL("GWR Quality")));
	m_pIntercept->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_pPoints->Get_Name(), _TL("GWR Intercept")));

	for(i=0; i<m_nPredictors; i++)
	{
		m_pSlopes[i]->Set_Name(CSG_String::Format(SG_T("%s (%s)"), m_pPoints->Get_Name(), m_pPoints->Get_Field_Name(m_iPredictor[i])));
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

				for(i=0; i<m_nPredictors; i++)
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
	nPoints	= m_Search.is_Okay() ? m_Search.Select_Nearest_Points(Point.x, Point.y, m_nPoints_Max, m_Radius, m_Mode == 0 ? -1 : 4) : m_pPoints->Get_Count();

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

		m_z[iPoint]	= pPoint->asDouble(m_iDependent);
		m_w[iPoint]	= m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0)));

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
