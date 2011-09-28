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
//               geostatistics_regression                //
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
	Set_Name		(_TL("Geographically Weighted Multiple Regression (Points)"));

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

	Parameters.Add_Shapes(
		NULL	, "REGRESSION"		, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGW_Multi_Regression_Points::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("POINTS")) )
	{
		CSG_Shapes		*pPoints		= pParameters->Get_Parameter("POINTS")		->asShapes();
		CSG_Parameters	*pAttributes	= pParameters->Get_Parameter("PREDICTORS")	->asParameters();

		pAttributes->Destroy();
		pAttributes->Set_Name(_TL("Predictors"));

		for(int i=0; pPoints && i<pPoints->Get_Field_Count(); i++)
		{
			if( SG_Data_Type_is_Numeric(pPoints->Get_Field_Type(i)) )
			{
				pAttributes->Add_Value(
					NULL, SG_Get_String(i, 0), pPoints->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, false
				);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGW_Multi_Regression_Points::Get_Predictors(void)
{
	int				i, iDependent, *Predictor;
	CSG_Shapes		*pPoints;
	CSG_Parameters	*pAttributes;

	//-----------------------------------------------------
	m_pPoints		= Parameters("REGRESSION")	->asShapes();
	m_Radius		= Parameters("RANGE")		->asInt() == 0 ? Parameters("RADIUS")   ->asDouble() : 0.0;
	m_Mode			= Parameters("MODE")		->asInt();
	m_nPoints_Max	= Parameters("NPOINTS")		->asInt() == 0 ? Parameters("MAXPOINTS")->asInt()    : 0;
	m_nPoints_Min	= Parameters("MINPOINTS")	->asInt();

	m_Weighting.Set_Parameters(Parameters("WEIGHTING")->asParameters());

	//-----------------------------------------------------
	iDependent		= Parameters("DEPENDENT")	->asInt();
	pPoints			= Parameters("POINTS")		->asShapes();;
	pAttributes		= Parameters("PREDICTORS")	->asParameters();

	m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s: %s]"), pPoints->Get_Name(), _TL("GWR"), pPoints->Get_Field_Name(iDependent)));
	m_pPoints->Add_Field(pPoints->Get_Field_Name(iDependent), SG_DATATYPE_Double);

	m_nPredictors	= 0;
	Predictor		= new int[pPoints->Get_Field_Count()];

	for(i=0; i<pAttributes->Get_Count(); i++)
	{
		CSG_Parameter	*pAttribute	= pAttributes->Get_Parameter(i);

		if( pAttribute->asBool() )
		{
			Predictor[m_nPredictors++]	= CSG_String(pAttribute->Get_Identifier()).asInt();

			m_pPoints->Add_Field(CSG_String::Format(SG_T("P%d %s"), m_nPredictors, pAttribute->Get_Name()), SG_DATATYPE_Double);
		}
	}

	if( m_nPredictors == 0 )
	{
		delete[](Predictor);

		Error_Set(_TL("no predictors have been selected"));

		return( false );
	}

	//-----------------------------------------------------
	m_pPoints->Add_Field(SG_T("R2")			, SG_DATATYPE_Double);
	m_pPoints->Add_Field(SG_T("REGRESSION")	, SG_DATATYPE_Double);
	m_pPoints->Add_Field(SG_T("RESIDUAL")	, SG_DATATYPE_Double);
	m_pPoints->Add_Field(SG_T("INTERCEPT")	, SG_DATATYPE_Double);

	for(i=0; i<m_nPredictors; i++)
	{
		m_pPoints->Add_Field(CSG_String::Format(SG_T("R%d"), 1 + i), SG_DATATYPE_Double);
	}

	for(int iPoint=0; iPoint<pPoints->Get_Count(); iPoint++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(iPoint);

		bool	bAdd	= !pPoint->is_NoData(iDependent);

		for(i=0; bAdd && i<m_nPredictors; i++)
		{
			if( pPoint->is_NoData(Predictor[i]) )
			{
				bAdd	= false;
			}
		}

		if( bAdd )
		{
			CSG_Shape	*pAdd	= m_pPoints->Add_Shape();

			pAdd->Add_Point(pPoint->Get_Point(0));

			pAdd->Set_Value(0, pPoint->asDouble(iDependent));

			for(i=0; i<m_nPredictors; i++)
			{
				pAdd->Set_Value(1 + i, pPoint->asDouble(Predictor[i]));
			}
		}
	}

	delete[](Predictor);

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

		m_z[iPoint]	= pPoint->asDouble(0);
		m_w[iPoint]	= m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0)));

		for(iPredictor=0; iPredictor<m_nPredictors; iPredictor++)
		{
			m_y[iPoint][iPredictor]	= pPoint->asDouble(1 + iPredictor);
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
