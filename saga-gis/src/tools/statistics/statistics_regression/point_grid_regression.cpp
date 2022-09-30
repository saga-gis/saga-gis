
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
//               point_grid_regression.cpp               //
//                                                       //
//                 Copyright (C) 2004 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "point_grid_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoint_Grid_Regression::CPoint_Grid_Regression(void)
{
	Set_Name		(_TL("Regression Analysis (Points and Predictor Grid)"));

	Set_Author		("O.Conrad (c) 2004");

	Set_Description	(_TW(
		"Regression analysis of point attributes with a grid as predictor. "
		"The regression function is used to create a new grid with regression based values."
	));

	Add_Reference(
		"Bahrenberg, G., Giese, E., Nipper, J.", "1990",
		"Statistische Methoden in der Geographie 1 - Univariate und bivariate Statistik",
		"Stuttgart, 233p."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"PREDICTOR"	, _TL("Predictor"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Observations"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("POINTS",
		"ATTRIBUTE"	, _TL("Dependent Variable"),
		_TL("")
	);

	Parameters.Add_Grid("",
		"REGRESSION", _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"RESIDUAL"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Regression Function"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s",
			SG_T("Y = a + b * X (linear)"),
			SG_T("Y = a + b / X"),
			SG_T("Y = a / (b - X)"),
			SG_T("Y = a * X^b (power)"),
			SG_T("Y = a e^(b * X) (exponential)"),
			SG_T("Y = a + b * ln(X) (logarithmic)")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Grid_Regression::On_Execute(void)
{
	CSG_Regression Regression;

	if( Get_Regression(Regression) )
	{
		Message_Add(Regression.asString());

		Set_Regression(Regression);

		Set_Residuals (Regression);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Grid_Regression::Get_Regression(CSG_Regression &Regression)
{
	Regression.Destroy();

	CSG_Shapes *pShapes    = Parameters("POINTS"   )->asShapes();
	int         Attribute  = Parameters("ATTRIBUTE")->asInt   ();
	CSG_Grid   *pPredictor = Parameters("PREDICTOR")->asGrid  ();
	CSG_Shapes *pResiduals = Parameters("RESIDUAL" )->asShapes();

	if( pResiduals )
	{
		pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", Parameters("ATTRIBUTE")->asString(), _TL("Residuals")));
		pResiduals->Add_Field("ID"        , SG_DATATYPE_Int   );
		pResiduals->Add_Field("X"         , SG_DATATYPE_Double);
		pResiduals->Add_Field("Y"         , SG_DATATYPE_Double);
		pResiduals->Add_Field("Y_GUESS"   , SG_DATATYPE_Double);
		pResiduals->Add_Field("Y_RESIDUAL", SG_DATATYPE_Double);
		pResiduals->Add_Field("Y_RES_VAR" , SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape *pShape = pShapes->Get_Shape(iShape);

		if( !pShape->is_NoData(Attribute) )
		{
			double Y = pShape->asDouble(Attribute);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					CSG_Point Point = pShape->Get_Point(iPoint, iPart); double X;

					if( pPredictor->Get_Value(Point, X, Resampling) )
					{
						Regression.Add_Values(X, Y);

						if( pResiduals )
						{
							CSG_Shape *pResidual = pResiduals->Add_Shape();

							pResidual->Add_Point(Point);
							pResidual->Set_Value(0, Regression.Get_Count());
							pResidual->Set_Value(1, X);
							pResidual->Set_Value(2, Y);
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	TSG_Regression_Type Type;

	switch( Parameters("METHOD")->asInt() )
	{
	default: Type = REGRESSION_Linear; break; // Y = a + b * X
	case  1: Type = REGRESSION_Rez_X ; break; // Y = a + b / X
	case  2: Type = REGRESSION_Rez_Y ; break; // Y = a / (b - X)
	case  3: Type = REGRESSION_Pow   ; break; // Y = a * X^b
	case  4: Type = REGRESSION_Exp   ; break; // Y = a e^(b * X)
	case  5: Type = REGRESSION_Log   ; break; // Y = a + b * ln(X)
	}

	return( Regression.Calculate(Type) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Grid_Regression::Set_Regression(const CSG_Regression &Regression)
{
	CSG_Grid *pPredictor  = Parameters("PREDICTOR" )->asGrid();
	CSG_Grid *pRegression = Parameters("REGRESSION")->asGrid();

	pRegression->Fmt_Name("%s [%s]", Parameters("ATTRIBUTE")->asString(), _TL("Regression Model"));

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pPredictor->is_NoData(x, y) )
			{
				pRegression->Set_NoData(x, y);
			}
			else
			{
				pRegression->Set_Value(x, y, Regression.Get_y(pPredictor->asDouble(x, y)));
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Grid_Regression::Set_Residuals(const CSG_Regression &Regression)
{
	CSG_Shapes *pResiduals = Parameters("RESIDUAL")->asShapes();

	if( !pResiduals || Regression.Get_yVariance() <= 0. )
	{
		return( false );
	}

	#pragma omp parallel for
	for(int iPoint=0; iPoint<pResiduals->Get_Count(); iPoint++)
	{
		CSG_Shape *pPoint = pResiduals->Get_Shape(iPoint);
			
		double Yreg = Regression.Get_y(pPoint->asDouble(1));
		double Yres = Yreg - pPoint->asDouble(2);

		pPoint->Set_Value(3, Yreg);
		pPoint->Set_Value(4, Yres);
		pPoint->Set_Value(5, Yres * 100. / Regression.Get_yVariance());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
