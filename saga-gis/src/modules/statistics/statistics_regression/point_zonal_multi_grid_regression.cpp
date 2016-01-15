/**********************************************************
 * Version $Id: point_zonal_multi_grid_regression.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//          point_zonal_multi_grid_regression.cpp        //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#include "point_zonal_multi_grid_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoint_Zonal_Multi_Grid_Regression::CPoint_Zonal_Multi_Grid_Regression(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Zonal Multiple Regression Analysis (Points and Predictor Grids)"));

	Set_Author		("O.Conrad, P.Gandelli (c) 2015");

	Set_Description	(_TW(
		"Linear regression analysis of point attributes using multiple predictor grids. "
		"Details of the regression/correlation analysis will be saved to a table. "
		"The regression function is used to create a new grid with regression based values. "
		"The multiple regression analysis uses a forward selection procedure. "
		"Each polygon in the zones layer is processed as individual zone. "
		"\n"
		"Reference:\n"
		"- Bahrenberg, G., Giese, E., Nipper, J. (1992): "
		"'Statistische Methoden in der Geographie 2 - Multivariate Statistik', "
		"Stuttgart, 415p.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "PREDICTORS"	, _TL("Predictors"),
		_TL(""),
		PARAMETER_INPUT, true
	);

	Parameters.Add_Shapes(
		NULL	, "ZONES"		, _TL("Zones"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ATTRIBUTE"	, _TL("Dependent Variable"),
		_TL("")
	);

	Parameters.Add_Shapes(
		NULL	, "RESIDUALS"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Value(
		NULL	, "COORD_X"		, _TL("Include X Coordinate"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "COORD_Y"		, _TL("Include Y Coordinate"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "INTERCEPT"	, _TL("Intercept"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Choice(
		NULL	,"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("include all"),
			_TL("forward"),
			_TL("backward"),
			_TL("stepwise")
		), 3
	);

	Parameters.Add_Value(
		NULL	, "P_VALUE"		, _TL("Significance Level"),
		_TL("Significance level (aka p-value) as threshold for automated predictor selection, given as percentage"),
		PARAMETER_TYPE_Double, 5.0, 0.0, true, 100.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPoint_Zonal_Multi_Grid_Regression::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("P_VALUE", pParameter->asInt() > 0);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Zonal_Multi_Grid_Regression::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPoints		= Parameters("POINTS"    )->asShapes();
	CSG_Shapes	*pZones			= Parameters("ZONES"     )->asShapes();
	CSG_Grid	*pRegression	= Parameters("REGRESSION")->asGrid  ();

	pRegression->Assign_NoData();

	CSG_Grid	Regression(*Get_System(), SG_DATATYPE_Float);

	SG_UI_Progress_Lock(true);	// suppress dialogs from popping up

	for(int i=0; i<pZones->Get_Count() && Process_Get_Okay(); i++)
	{
		CSG_Shape_Polygon	*pZone	= (CSG_Shape_Polygon *)pZones->Get_Shape(i);

		//-------------------------------------------------
		// select all points located in current zone polygon

		bool	bResult;

		CSG_Shapes Zone(SHAPE_TYPE_Polygon);	Zone.Add_Shape(pZone);

		SG_RUN_MODULE(bResult, "shapes_tools", 5,	// select points by location
			   SG_MODULE_PARAMETER_SET("LOCATIONS", &Zone)
			&& SG_MODULE_PARAMETER_SET("SHAPES"   , pPoints)
		);

		if( !bResult )
		{
			SG_UI_Process_Set_Okay();	// don't stop overall work flow, if tool execution failed for current zone
		}
		else if( pPoints->Get_Selection_Count() > 0 )
		{
			//---------------------------------------------
			// copy selected points to a new (temporary) points layer

			CSG_Shapes	Selection;

			SG_RUN_MODULE(bResult, "shapes_tools", 6,	// copy selected points to a new layer
				   SG_MODULE_PARAMETER_SET("INPUT" , pPoints)
				&& SG_MODULE_PARAMETER_SET("OUTPUT", &Selection)
			);

			pPoints->asShapes()->Select();	// unselect everything from original points layer

			//---------------------------------------------
			// perform the regression analysis, regression grid for zone is temporary

			SG_RUN_MODULE(bResult, "statistics_regression", 1,	// multiple linear regression for points and predictor grids
				   SG_MODULE_PARAMETER_SET("PREDICTORS", Parameters("PREDICTORS"))
				&& SG_MODULE_PARAMETER_SET("REGRESSION", &Regression             )
				&& SG_MODULE_PARAMETER_SET("POINTS"    , &Selection              )
				&& SG_MODULE_PARAMETER_SET("ATTRIBUTE" , Parameters("ATTRIBUTE" ))
				&& SG_MODULE_PARAMETER_SET("RESAMPLING", Parameters("RESAMPLING"))
				&& SG_MODULE_PARAMETER_SET("COORD_X"   , Parameters("COORD_X"   ))
				&& SG_MODULE_PARAMETER_SET("COORD_Y"   , Parameters("COORD_Y"   ))
				&& SG_MODULE_PARAMETER_SET("INTERCEPT" , Parameters("INTERCEPT" ))
				&& SG_MODULE_PARAMETER_SET("METHOD"    , Parameters("METHOD"    ))
				&& SG_MODULE_PARAMETER_SET("P_VALUE"   , Parameters("P_VALUE"   ))
			);

			//---------------------------------------------
			// use zone polygon as mask for copying zonal regression result to final regression grid

			if( !bResult )
			{
				SG_UI_Process_Set_Okay();	// don't stop overall work flow, if tool execution failed for current zone
			}
			else
			{
				#pragma omp parallel for	// speed up using multiple processors
				for(int y=0; y<Get_NY(); y++)
				{
					for(int x=0; x<Get_NX(); x++)
					{
						if( !Regression.is_NoData(x, y) && pZone->Contains(Get_System()->Get_Grid_to_World(x, y)) )
						{
							pRegression->Set_Value(x, y, Regression.asDouble(x, y));
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	SG_UI_Progress_Lock(false);

	//-----------------------------------------------------
	Set_Residuals(pPoints, pRegression);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Zonal_Multi_Grid_Regression::Set_Residuals(CSG_Shapes *pPoints, CSG_Grid *pRegression)
{
	CSG_Shapes	*pResiduals	= Parameters("RESIDUALS")->asShapes();
	int			iAttribute	= Parameters("ATTRIBUTE")->asInt   ();

	if( !pRegression || !pResiduals )
	{
		return( false );
	}

	//-----------------------------------------------------
	pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), Parameters("ATTRIBUTE")->asString(), _TL("Residuals")));
	pResiduals->Add_Field(pPoints->Get_Field_Name(iAttribute), SG_DATATYPE_Double);
	pResiduals->Add_Field("TREND"	, SG_DATATYPE_Double);
	pResiduals->Add_Field("RESIDUAL", SG_DATATYPE_Double);

	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	Resampling	= GRID_RESAMPLING_Bilinear;			break;
	case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline;	break;
	case  3:	Resampling	= GRID_RESAMPLING_BSpline;			break;
	}

	//-----------------------------------------------------
	for(int iShape=0; iShape<pPoints->Get_Count() && Set_Progress(iShape, pPoints->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pPoints->Get_Shape(iShape);

		if( !pShape->is_NoData(iAttribute) )
		{
			double	zShape	= pShape->asDouble(iAttribute);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					double		zGrid;
					TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

					if( pRegression->Get_Value(Point, zGrid, Resampling) )
					{
						CSG_Shape	*pResidual	= pResiduals->Add_Shape();

						pResidual->Add_Point(Point);
						pResidual->Set_Value(0, zShape);
						pResidual->Set_Value(1, zGrid);
						pResidual->Set_Value(2, zShape - zGrid);
					}
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
