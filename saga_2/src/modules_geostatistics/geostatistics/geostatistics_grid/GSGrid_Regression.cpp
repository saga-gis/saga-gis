
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Geostatistics_Grid                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 GSGrid_Regression.cpp                 //
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
#include "GSGrid_Regression.h"


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
CGSGrid_Regression::CGSGrid_Regression(void)
{
	CParameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Regression Analysis (Grid/Points)"));

	Set_Author		(_TL("Copyrights (c) 2004 by Olaf Conrad"));

	Set_Description	(_TL(
		"Regression analysis of point attributes with grid values. "
		"The regression function is used to create a new grid with (extrapolated) values. \n"
		"\n"
		"Reference:\n"
		"- Bahrenberg, G., Giese, E., Nipper, J. (1990): "
		"'Statistische Methoden in der Geographie 1 - Univariate und bivariate Statistik', "
		"Stuttgart, 233p.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ATTRIBUTE"	, _TL("Attribute"),
		""
	);

	Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "RESIDUAL"	, _TL("Residuals"),
		"",
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL	,"INTERPOL"		, _TL("Grid Interpolation"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Regression Function"),
		"",
		"Y = a + b * X (linear)|"
		"Y = a + b / X|"
		"Y = a / (b - X)|"
		"Y = a * X^b (power)|"
		"Y = a e^(b * X) (exponential)|"
		"Y = a + b * ln(X) (logarithmic)|", 0
	);
}

//---------------------------------------------------------
CGSGrid_Regression::~CGSGrid_Regression(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Regression::On_Execute(void)
{
	int						iAttribute;
	TSG_Regression_Type	Type;
	CShapes					*pShapes, *pResiduals;
	CGrid					*pGrid, *pRegression;

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
bool CGSGrid_Regression::Get_Regression(CGrid *pGrid, CShapes *pShapes, CShapes *pResiduals, int iAttribute, TSG_Regression_Type Type)
{
	int			iShape, iPart, iPoint;
	double		zShape, zGrid;
	TSG_Point	Point;
	CShape		*pShape, *pResidual;

	//-----------------------------------------------------
	if( pResiduals )
	{
		pResiduals->Create(SHAPE_TYPE_Point, _TL("Residuals"));
		pResiduals->Get_Table().Add_Field("ID"			, TABLE_FIELDTYPE_Int);
		pResiduals->Get_Table().Add_Field("Y"			, TABLE_FIELDTYPE_Double);
		pResiduals->Get_Table().Add_Field("X"			, TABLE_FIELDTYPE_Double);
		pResiduals->Get_Table().Add_Field("Y_GUESS"		, TABLE_FIELDTYPE_Double);
		pResiduals->Get_Table().Add_Field("Y_RESIDUAL"	, TABLE_FIELDTYPE_Double);
		pResiduals->Get_Table().Add_Field("Y_RES_VAR"	, TABLE_FIELDTYPE_Double);
	}

	m_Regression.Destroy();

	//-----------------------------------------------------
	for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		pShape	= pShapes->Get_Shape(iShape);
		zShape	= pShape->Get_Record()->asDouble(iAttribute);

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
						pResidual->Get_Record()->Set_Value(0, m_Regression.Get_Count());
						pResidual->Get_Record()->Set_Value(1, zShape);
						pResidual->Get_Record()->Set_Value(2, zGrid);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( m_Regression.Calculate(Type) );
}

//---------------------------------------------------------
bool CGSGrid_Regression::Set_Regression(CGrid *pGrid, CGrid *pRegression)
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
bool CGSGrid_Regression::Set_Residuals(CShapes *pResiduals)
{
	int		iPoint;
	double	m, b, v, y;
	CShape	*pPoint;

	if( pResiduals )
	{
		m	= m_Regression.Get_Coefficient();
		b	= m_Regression.Get_Constant();
		v	= 100.0 / m_Regression.Get_yVariance();

		for(iPoint=0; iPoint<pResiduals->Get_Count() && Set_Progress(iPoint, pResiduals->Get_Count()); iPoint++)
		{
			pPoint	= pResiduals->Get_Shape(iPoint);

			pPoint->Get_Record()->Set_Value(3, y = pPoint->Get_Record()->asDouble(2) * m + b);
			pPoint->Get_Record()->Set_Value(4, y = pPoint->Get_Record()->asDouble(1) - y);
			pPoint->Get_Record()->Set_Value(5, y * v);
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
