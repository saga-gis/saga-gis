
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include "gw_multi_regression_points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGW_Multi_Regression_Points::CGW_Multi_Regression_Points(void)
{
	Set_Name		(_TL("GWR for Multiple Predictors (Shapes)"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Geographically Weighted Regression for multiple predictors. "
		"Regression details are stored in a copy of the input data set. "
		"If the input data set is not a point data set, the feature "
		"centroids are used as spatial reference. "
	));

	GWR_Add_References(true);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("POINTS",
		"DEPENDENT"	, _TL("Dependent Variable"),
		_TL("")
	);

	Parameters.Add_Table_Fields("POINTS",
		"PREDICTORS", _TL("Predictors"),
		_TL("")
	);

	Parameters.Add_Shapes("",
		"REGRESSION", _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"LOGISTIC"	, _TL("Logistic Regression"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	m_Weighting.Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Weighting.Create_Parameters(Parameters);

	//-----------------------------------------------------
	m_Search.Create(&Parameters, Parameters.Add_Node("", "NODE_SEARCH", _TL("Search Options"), _TL("")), 16);

	Parameters("SEARCH_RANGE"     )->Set_Value(1);
	Parameters("SEARCH_POINTS_ALL")->Set_Value(1);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGW_Multi_Regression_Points::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Search.On_Parameter_Changed(pParameters, pParameter);

		pParameters->Set_Parameter("DW_BANDWIDTH", GWR_Fit_To_Density(pParameter->asShapes(), 4., 1));
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGW_Multi_Regression_Points::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Search.On_Parameters_Enable(pParameters, pParameter);

	m_Weighting.Enable_Parameters(*pParameters);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression_Points::Initialize(void)
{
	m_Weighting.Set_Parameters(Parameters);

	//-----------------------------------------------------
	CSG_Parameter_Table_Fields	*pFields	= Parameters("PREDICTORS")->asTableFields();

	if( (m_nPredictors = pFields->Get_Count()) < 1 )
	{
		Error_Set(_TL("no predictors have been selected"));

		return( false );
	}

	int	Dependent	= Parameters("DEPENDENT")->asInt();

	//-----------------------------------------------------
	CSG_Shapes	*pShapes	= Parameters("POINTS")->asShapes();

	m_pShapes	= Parameters("REGRESSION")->asShapes();

	m_pShapes->Create(pShapes->Get_Type(), CSG_String::Format("%s.%s [%s]", pShapes->Get_Name(), pShapes->Get_Field_Name(Dependent), _TL("GWR")));

	CSG_Shapes	*pPoints	= m_pShapes->Get_Type() == SHAPE_TYPE_Point ? m_pShapes : &m_Centroids;

	if( pPoints == &m_Centroids )
	{
		m_Centroids.Create(SHAPE_TYPE_Point); m_Centroids.Add_Field("ID", SG_DATATYPE_DWord);
	}

	//-----------------------------------------------------
	for(int i=0; i<m_nPredictors; i++)	// predictor values
	{
		m_pShapes->Add_Field(pShapes->Get_Field_Name(pFields->Get_Index(i)), SG_DATATYPE_Double);
	}

	m_pShapes->Add_Field(CSG_String::Format("Observed (%s)", pShapes->Get_Field_Name(Dependent)), SG_DATATYPE_Double);

	m_pShapes->Add_Field("Predicted"   , SG_DATATYPE_Double);	// m_nPredictors + 1
	m_pShapes->Add_Field("Residual"    , SG_DATATYPE_Double);	// m_nPredictors + 2
	m_pShapes->Add_Field("Std.Residual", SG_DATATYPE_Double);	// m_nPredictors + 3
	m_pShapes->Add_Field("Local R2"    , SG_DATATYPE_Double);	// m_nPredictors + 4
	m_pShapes->Add_Field("Intercept"   , SG_DATATYPE_Double);	// m_nPredictors + 5

	for(int i=0; i<m_nPredictors; i++)	// m_nPredictors + 6..., predictor model slope(s)
	{
		m_pShapes->Add_Field(CSG_String::Format("Slope.%d (%s)", i + 1, pShapes->Get_Field_Name(pFields->Get_Index(i))), SG_DATATYPE_Double);
	}

	for(int iShape=0; iShape<pShapes->Get_Count(); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		bool	bOkay	= pShape->is_Valid() && !pShape->is_NoData(Dependent);

		for(int i=0; bOkay && i<m_nPredictors; i++)
		{
			bOkay	= !pShape->is_NoData(pFields->Get_Index(i));
		}

		if( bOkay )
		{
			CSG_Shape	*pCopy	= m_pShapes->Add_Shape(pShape, SHAPE_COPY_GEOM);

			for(int i=0; i<m_nPredictors; i++)
			{
				pCopy->Set_Value(i, pShape->asDouble(pFields->Get_Index(i)));
			}

			pCopy->Set_Value(m_nPredictors, pShape->asDouble(Dependent));

			if( pPoints == &m_Centroids )
			{
				m_Centroids.Add_Shape()->Set_Point(pCopy->Get_Centroid(), 0);
			}
		}
	}

	//-----------------------------------------------------
	if( m_pShapes->Get_Count() < 1 )
	{
		Error_Set(_TL("invalid data"));

		return( false );
	}

	if( !m_Search.Initialize(pPoints, -1) )
	{
		Error_Set(_TL("failed to create searche engine"));

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CGW_Multi_Regression_Points::Finalize(void)
{
	m_Search.Finalize();

	m_Centroids.Destroy();

	Set_Classification();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression_Points::Set_Classification(void)
{
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(m_pShapes, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		pLUT->asTable()->Del_Records();

		#define ADD_CLASS(r, g, b, min, max, name) { CSG_Table_Record *pClass = pLUT->asTable()->Add_Record();\
			pClass->Set_Value(0, SG_GET_RGB(r, g, b));\
			pClass->Set_Value(1, name);\
			pClass->Set_Value(2, "");\
			pClass->Set_Value(3, min);\
			pClass->Set_Value(4, max);\
		}

		ADD_CLASS( 69, 117, 181, -1e6, -2.5,      "< -2.5 Std.Dev");
		ADD_CLASS(132, 158, 168, -2.5, -1.5, "-2.5 - -1.5 Std.Dev");
		ADD_CLASS(192, 204, 190, -1.5, -0.5, "-1.5 - -0.5 Std.Dev");
		ADD_CLASS(255, 255, 191, -0.5,  0.5,  "-0.5 - 0.5 Std.Dev");
		ADD_CLASS(250, 185, 132,  0.5,  1.5,   "0.5 - 1.5 Std.Dev");
		ADD_CLASS(237, 117,  81,  1.5,  2.5,   "1.5 - 2.5 Std.Dev");
		ADD_CLASS(214,  47,  39,  2.5,  1e6,       "> 2.5 Std.Dev");

		DataObject_Set_Parameter(m_pShapes, pLUT);
		DataObject_Set_Parameter(m_pShapes, "COLORS_TYPE",                 1);	// Color Classification Type: Lookup Table
		DataObject_Set_Parameter(m_pShapes, "LUT_ATTRIB" , m_nPredictors + 3);	// Lookup Table Attribute   : Std.Residual
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression_Points::On_Execute(void)
{
	if( !Initialize() )
	{
		Finalize();

		return( false );
	}

	bool	bLogistic	= Parameters("LOGISTIC")->asBool();

	CSG_Simple_Statistics	Residuals;

	//-----------------------------------------------------
	for(int iShape=0; iShape<m_pShapes->Get_Count() && Set_Progress(iShape, m_pShapes->Get_Count()); iShape++)
	{
		CSG_Regression_Weighted	Model;

		CSG_Shape	*pShape = m_pShapes->Get_Shape(iShape);

		if( Get_Model(pShape->Get_Centroid(), Model, bLogistic) )
		{
			double	Value	= Model[0];	// Intercept

			pShape->Set_Value(m_nPredictors + 5, Model[0]);

			for(int i=0; i<m_nPredictors; i++)
			{
				Value	+= Model[1 + i] * pShape->asDouble(i);

				pShape->Set_Value(m_nPredictors + 6 + i, Model[1 + i]);
			}

			if( bLogistic )
			{
				Value	= 1. / (1. + exp(-Value));
			}

			double	Residual = pShape->asDouble(m_nPredictors) - Value; Residuals += Residual;

			pShape->Set_Value(m_nPredictors + 1, Value         ); // Predicted
			pShape->Set_Value(m_nPredictors + 2, Residual      ); // Residual
			pShape->Set_Value(m_nPredictors + 4, Model.Get_R2()); // Local R2
		}
		else for(int i=1+m_nPredictors; i<m_pShapes->Get_Field_Count(); i++)
		{
			pShape->Set_NoData(i);
		}
	}

	//-----------------------------------------------------
	for(int iShape=0; iShape<m_pShapes->Get_Count() && Set_Progress(iShape, m_pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape = m_pShapes->Get_Shape(iShape);

		if( Residuals.Get_StdDev() > 0. && !pShape->is_NoData(m_nPredictors + 2) )
		{
			pShape->Set_Value(m_nPredictors + 3, pShape->asDouble(m_nPredictors + 2) / Residuals.Get_StdDev()); // Std.Residual
		}
		else
		{
			pShape->Set_NoData(m_nPredictors + 3);
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
bool CGW_Multi_Regression_Points::Get_Model(const TSG_Point &Point, CSG_Regression_Weighted &Model, bool bLogistic)
{
	int	nShapes	= m_Search.Set_Location(Point);

	CSG_Vector	Predictors(m_nPredictors);

	Model.Destroy();

	for(int iShape=0; iShape<nShapes; iShape++)
	{
		double	ix, iy, iz; int Index = !m_Search.Do_Use_All() && m_Search.Get_Point(iShape, ix, iy, iz) ? (int)iz : iShape;

		CSG_Shape	*pShape = m_pShapes->Get_Shape(Index);

		for(int i=0; i<m_nPredictors; i++)
		{
			Predictors[i]	= pShape->asDouble(i);
		}

		Model.Add_Sample(m_Weighting.Get_Weight(SG_Get_Distance(Point, pShape->Get_Centroid())), pShape->asDouble(m_nPredictors), Predictors);
	}

	return( Model.Calculate(bLogistic) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
