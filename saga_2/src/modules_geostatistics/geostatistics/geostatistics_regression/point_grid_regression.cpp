
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "point_grid_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GRID_ZFACTOR	true


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoint_Grid_Regression::CPoint_Grid_Regression(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Regression Analysis (Points/Grid)"));

	Set_Author		(SG_T("O.Conrad (c) 2004"));

	Set_Description	(_TW(
		"Regression analysis of point attributes with grid values. "
		"The regression function is used to create a new grid with regression based values. \n"
		"\n"
		"Reference:\n"
		"- Bahrenberg, G., Giese, E., Nipper, J. (1990): "
		"'Statistische Methoden in der Geographie 1 - Univariate und bivariate Statistik', "
		"Stuttgart, 233p.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ATTRIBUTE"	, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "RESIDUAL"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL	,"INTERPOL"		, _TL("Grid Interpolation"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Regression Function"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("Y = a + b * X (linear)"),
			_TL("Y = a + b / X"),
			_TL("Y = a / (b - X)"),
			_TL("Y = a * X^b (power)"),
			_TL("Y = a e^(b * X) (exponential)"),
			_TL("Y = a + b * ln(X) (logarithmic)")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Grid_Regression::On_Execute(void)
{
	int					iAttribute;
	TSG_Regression_Type	Type;
	CSG_Shapes			*pShapes, *pResiduals;
	CSG_Grid			*pGrid, *pRegression;

	//-----------------------------------------------------
	pGrid			= Parameters("GRID")		->asGrid();
	pRegression		= Parameters("REGRESSION")	->asGrid();
	pShapes			= Parameters("SHAPES")		->asShapes();
	pResiduals		= Parameters("RESIDUAL")	->asShapes();
	iAttribute		= Parameters("ATTRIBUTE")	->asInt();
	m_Interpolation	= Parameters("INTERPOL")	->asInt();

	switch( Parameters("METHOD")->asInt() )
	{
	default:
	case 0:	Type	= REGRESSION_Linear;	break;	// Y = a + b * X
	case 1:	Type	= REGRESSION_Rez_X;		break;	// Y = a + b / X
	case 2:	Type	= REGRESSION_Rez_Y;		break;	// Y = a / (b - X)
	case 3:	Type	= REGRESSION_Pow;		break;	// Y = a * X^b
	case 4:	Type	= REGRESSION_Exp;		break;	// Y = a e^(b * X)
	case 5:	Type	= REGRESSION_Log;		break;	// Y = a + b * ln(X)
	}

	//-----------------------------------------------------
	if( Get_Regression(pGrid, pShapes, pResiduals, iAttribute, Type) )
	{
		pRegression->Set_Name(CSG_String::Format(SG_T("%s (%s)"), pShapes->Get_Name(), Get_Name()));

		Set_Regression(pGrid, pRegression);

		Set_Residuals(pResiduals);

		Message_Add(m_Regression.asString());

		m_Regression.Destroy();

		return( true );
	}

	//-----------------------------------------------------
	m_Regression.Destroy();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Grid_Regression::Get_Regression(CSG_Grid *pGrid, CSG_Shapes *pShapes, CSG_Shapes *pResiduals, int iAttribute, TSG_Regression_Type Type)
{
	int			iShape, iPart, iPoint;
	double		zShape, zGrid;
	TSG_Point	Point;
	CSG_Shape	*pShape, *pResidual;

	//-----------------------------------------------------
	if( pResiduals )
	{
		pResiduals->Create(SHAPE_TYPE_Point, _TL("Residuals"));
		pResiduals->Add_Field("ID"			, SG_DATATYPE_Int);
		pResiduals->Add_Field("Y"			, SG_DATATYPE_Double);
		pResiduals->Add_Field("X"			, SG_DATATYPE_Double);
		pResiduals->Add_Field("Y_GUESS"		, SG_DATATYPE_Double);
		pResiduals->Add_Field("Y_RESIDUAL"	, SG_DATATYPE_Double);
		pResiduals->Add_Field("Y_RES_VAR"	, SG_DATATYPE_Double);
	}

	m_Regression.Destroy();

	//-----------------------------------------------------
	for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		pShape	= pShapes->Get_Shape(iShape);
		zShape	= pShape->asDouble(iAttribute);

		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				if( pGrid->Get_Value(Point = pShape->Get_Point(iPoint, iPart), zGrid, m_Interpolation, GRID_ZFACTOR) )
				{
					m_Regression.Add_Values(zGrid, zShape);

					if( pResiduals )
					{
						pResidual	= pResiduals->Add_Shape();
						pResidual->Add_Point(Point);
						pResidual->Set_Value(0, m_Regression.Get_Count());
						pResidual->Set_Value(1, zShape);
						pResidual->Set_Value(2, zGrid);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( m_Regression.Calculate(Type) );
}

//---------------------------------------------------------
bool CPoint_Grid_Regression::Set_Regression(CSG_Grid *pGrid, CSG_Grid *pRegression)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pGrid->is_NoData(x, y) )
				pRegression->Set_NoData(x, y);
			else
				pRegression->Set_Value (x, y, m_Regression.Get_y(pGrid->asDouble(x, y, GRID_ZFACTOR)));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CPoint_Grid_Regression::Set_Residuals(CSG_Shapes *pResiduals)
{
	int		iPoint;
	double	m, b, v, y;
	CSG_Shape	*pPoint;

	if( pResiduals )
	{
		m	= m_Regression.Get_Coefficient();
		b	= m_Regression.Get_Constant();
		v	= 100.0 / m_Regression.Get_yVariance();

		for(iPoint=0; iPoint<pResiduals->Get_Count() && Set_Progress(iPoint, pResiduals->Get_Count()); iPoint++)
		{
			pPoint	= pResiduals->Get_Shape(iPoint);

			pPoint->Set_Value(3, y = pPoint->asDouble(2) * m + b);
			pPoint->Set_Value(4, y = pPoint->asDouble(1) - y);
			pPoint->Set_Value(5, y * v);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
