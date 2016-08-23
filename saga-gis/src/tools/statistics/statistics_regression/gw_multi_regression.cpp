/**********************************************************
 * Version $Id: gw_multi_regression.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
CGW_Multi_Regression::CGW_Multi_Regression(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("GWR for Multiple Predictors (Gridded Model Output)"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Geographically Weighted Regression for multiple predictors. "
		"Predictors have to be supplied as attributes of ingoing points data. "
		"Regression model parameters are generated as continuous fields, i.e. as grids.\n"
		"Reference:\n"
	) + GWR_References);

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
		NULL	, "REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("QUALITY"  , _TL("Quality"  ), false);
	m_Grid_Target.Add_Grid("INTERCEPT", _TL("Intercept"), false);

	Parameters.Add_Grid_List(
		NULL	, "SLOPES"		, _TL("Slopes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	m_Weighting.Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Weighting.Create_Parameters(&Parameters, false);

	//-----------------------------------------------------
	m_Search.Create(&Parameters, Parameters.Add_Node(NULL, "NODE_SEARCH", _TL("Search Options"), _TL("")), 16);

	Parameters("SEARCH_RANGE"     )->Set_Value(1);
	Parameters("SEARCH_POINTS_ALL")->Set_Value(1);

	//-----------------------------------------------------
	m_iPredictor	= NULL;
	m_pSlopes		= NULL;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGW_Multi_Regression::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "POINTS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());

		m_Search.On_Parameter_Changed(pParameters, pParameter);

		pParameters->Set_Parameter("DW_BANDWIDTH", GWR_Fit_To_Density(pParameter->asShapes(), 4.0, 1));
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGW_Multi_Regression::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Weighting.Enable_Parameters(pParameters);

	m_Search     .On_Parameters_Enable(pParameters, pParameter);
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression::Initialize(void)
{
	CSG_Parameter_Table_Fields	*pFields	= Parameters("PREDICTORS")->asTableFields();

	if( (m_nPredictors = pFields->Get_Count()) > 0 )
	{
		m_iPredictor	= new int[m_nPredictors];

		for(int i=0; i<m_nPredictors; i++)
		{
			m_iPredictor[i]	= pFields->Get_Index(i);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CGW_Multi_Regression::Finalize(void)
{
	SG_DELETE_ARRAY(m_iPredictor);
	SG_FREE_SAFE   (m_pSlopes);

	m_Search.Finalize();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression::On_Execute(void)
{
	//-----------------------------------------------------
	m_pPoints		= Parameters("POINTS"   )->asShapes();
	m_iDependent	= Parameters("DEPENDENT")->asInt   ();

	//-----------------------------------------------------
	if( !Initialize() )
	{
		Finalize();

		return( false );
	}

	if( !m_Search.Initialize(m_pPoints, -1) )
	{
		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	m_Weighting.Set_Parameters(&Parameters);

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
	for(int y=0; y<m_pIntercept->Get_NY() && Set_Progress(y, m_pIntercept->Get_NY()); y++)
	{
		for(int x=0; x<m_pIntercept->Get_NX(); x++)
		{
			CSG_Regression_Weighted	Model;

			if( Get_Model(x, y, Model) )
			{
				m_pQuality  ->Set_Value(x, y, Model.Get_R2());
				m_pIntercept->Set_Value(x, y, Model[0]);

				for(int i=0; i<m_nPredictors; i++)
				{
					m_pSlopes[i]->Set_Value(x, y, Model[i + 1]);
				}
			}
			else
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression::Get_Model(int x, int y, CSG_Regression_Weighted &Model)
{
	//-----------------------------------------------------
	TSG_Point	Point	= m_pIntercept->Get_System().Get_Grid_to_World(x, y);
	int			nPoints = m_Search.Set_Location(Point);

	CSG_Vector	Predictors(m_nPredictors);

	Model.Destroy();

	for(int iPoint=0; iPoint<nPoints; iPoint++)
	{
		double	ix, iy, iz;

		CSG_Shape	*pPoint = m_Search.Do_Use_All() && m_Search.Get_Point(iPoint, ix, iy, iz)
			? m_pPoints->Get_Shape((int)iz)
			: m_pPoints->Get_Shape(iPoint);

		if( !pPoint->is_NoData(m_iDependent) )
		{
			bool	bOkay	= true;

			for(int iPredictor=0; iPredictor<m_nPredictors && bOkay; iPredictor++)
			{
				if( !pPoint->is_NoData(m_iPredictor[iPredictor]) )
				{
					Predictors[iPredictor]	= pPoint->asDouble(m_iPredictor[iPredictor]);
				}
				else
				{
					bOkay	= false;
				}
			}

			if( bOkay )
			{
				Model.Add_Sample(
					m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0))),
					pPoint->asDouble(m_iDependent), Predictors
				);
			}
		}
	}

	//-----------------------------------------------------
	return( Model.Calculate() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
