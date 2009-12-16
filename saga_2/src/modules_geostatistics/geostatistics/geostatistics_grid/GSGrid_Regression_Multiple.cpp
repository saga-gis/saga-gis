
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
//             GSGrid_Regression_Multiple.cpp            //
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
#include "GSGrid_Regression_Multiple.h"


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
CGSGrid_Regression_Multiple::CGSGrid_Regression_Multiple(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Multiple Regression Analysis (Grids/Points)"));

	Set_Author		(SG_T("(c) 2004 by O.Conrad"));

	Set_Description	(_TW(
		"Linear regression analysis of point attributes with multiple grids. "
		"Details of the regression/correlation analysis will be saved to a table. "
		"The regression function is used to create a new grid with (extrapolated) values. "
		"The multiple regression analysis uses a forward selection procedure. \n"
		"\n"
		"Reference:\n"
		"- Bahrenberg, G., Giese, E., Nipper, J. (1992): "
		"'Statistische Methoden in der Geographie 2 - Multivariate Statistik', "
		"Stuttgart, 415p.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
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

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Details"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "RESIDUAL"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "COORDS"		, _TL("Include Coordinates"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Regression_Multiple::On_Execute(void)
{
	int						iAttribute;
	CSG_Table				*pTable;
	CSG_Shapes				*pShapes, *pResiduals;
	CSG_Grid				*pRegression;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids			= Parameters("GRIDS")		->asGridList();
	pRegression		= Parameters("REGRESSION")	->asGrid();
	pTable			= Parameters("TABLE")		->asTable();
	pShapes			= Parameters("SHAPES")		->asShapes();
	pResiduals		= Parameters("RESIDUAL")	->asShapes();
	iAttribute		= Parameters("ATTRIBUTE")	->asInt();
	m_bCoords		= Parameters("COORDS")		->asBool();
	m_Interpolation	= Parameters("INTERPOL")	->asInt();

	//-----------------------------------------------------
	if( Get_Regression(pGrids, pShapes, iAttribute) )
	{
		pRegression->Set_Name(CSG_String::Format(SG_T("%s (%s)"), pShapes->Get_Name(), Get_Name()));

		Set_Regression(pGrids, pRegression);

		Set_Residuals(pShapes, iAttribute, pResiduals, pRegression);

		Set_Message(pGrids);

		if( pTable )
		{
			pTable->Assign(m_Regression.Get_Result());

			pTable->Set_Name(_TL("Multiple Regression Analysis"));
		}

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
bool CGSGrid_Regression_Multiple::Get_Regression(CSG_Parameter_Grid_List *pGrids, CSG_Shapes *pShapes, int iAttribute)
{
	int			iGrid;
	double		zGrid;
	CSG_Table	Table;

	//-----------------------------------------------------
	Table.Add_Field(pShapes->Get_Name(), SG_DATATYPE_Double);

	for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
	{
		Table.Add_Field(pGrids->asGrid(iGrid)->Get_Name(), SG_DATATYPE_Double);
	}

	if( m_bCoords )
	{
		Table.Add_Field(SG_T("X"), SG_DATATYPE_Double);
		Table.Add_Field(SG_T("Y"), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);
		double		zShape	= pShape->asDouble(iAttribute);

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point			Point		= pShape->Get_Point(iPoint, iPart);
				CSG_Table_Record	*pRecord	= Table.Add_Record();

				pRecord->Set_Value(0, zShape);

				for(iGrid=0; iGrid<pGrids->Get_Count() && pRecord; iGrid++)
				{
					if( pGrids->asGrid(iGrid)->Get_Value(Point, zGrid, m_Interpolation, GRID_ZFACTOR) )
					{
						pRecord->Set_Value(1 + iGrid, zGrid);
					}
					else
					{
						Table.Del_Record(Table.Get_Record_Count() - 1);

						pRecord	= NULL;
					}
				}

				if( m_bCoords && pRecord )
				{
					pRecord->Set_Value(pGrids->Get_Count() + 1, Point.x);
					pRecord->Set_Value(pGrids->Get_Count() + 2, Point.y);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( m_Regression.Calculate(Table) );
}

//---------------------------------------------------------
bool CGSGrid_Regression_Multiple::Set_Regression(CSG_Parameter_Grid_List *pGrids, CSG_Grid *pRegression)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int		i, n;
			double	z	= m_Regression.Get_RConst();

			for(i=0, n=0; i==n && i<pGrids->Get_Count(); i++)
			{
				if( !pGrids->asGrid(i)->is_NoData(x, y) )
				{
					z	+= m_Regression.Get_RCoeff(i) * pGrids->asGrid(i)->asDouble(x, y);
					n	++;
				}
			}

			if( n != pGrids->Get_Count() )
			{
				pRegression->Set_NoData(x, y);
			}
			else
			{
				if( m_bCoords )
				{
					z	+= m_Regression.Get_RCoeff(n + 0) * Get_System()->Get_xGrid_to_World(x);
					z	+= m_Regression.Get_RCoeff(n + 1) * Get_System()->Get_yGrid_to_World(y);
				}

				pRegression->Set_Value (x, y, z);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGSGrid_Regression_Multiple::Set_Residuals(CSG_Shapes *pShapes, int iAttribute, CSG_Shapes *pResiduals, CSG_Grid *pRegression)
{
	int			iPoint, iPart, iShape;
	double		zShape, zGrid;
	TSG_Point	Point;
	CSG_Shape		*pShape, *pResidual;

	//-----------------------------------------------------
	if( pResiduals )
	{
		pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Residuals")));
		pResiduals->Add_Field(pShapes->Get_Field_Name(iAttribute), SG_DATATYPE_Double);
		pResiduals->Add_Field("TREND"	, SG_DATATYPE_Double);
		pResiduals->Add_Field("RESIDUAL", SG_DATATYPE_Double);

		//-------------------------------------------------
		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			pShape	= pShapes->Get_Shape(iShape);
			zShape	= pShape->asDouble(iAttribute);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Point	= pShape->Get_Point(iPoint, iPart);

					if( pRegression->Get_Value(Point, zGrid, m_Interpolation, GRID_ZFACTOR) )
					{
						pResidual	= pResiduals->Add_Shape();
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

//---------------------------------------------------------
void CGSGrid_Regression_Multiple::Set_Message(CSG_Parameter_Grid_List *pGrids)
{
	int		i, j;

	Message_Add(SG_T("\n"), false);
	Message_Add(CSG_String::Format(SG_T("\n%s:"), _TL("Regression")), false);
	Message_Add(CSG_String::Format(SG_T("\n Y = %f"), m_Regression.Get_RConst()), false);

	for(i=0; i<pGrids->Get_Count(); i++)
	{
		if( (j = m_Regression.Get_Ordered(i)) >= 0 && j < pGrids->Get_Count() )
		{
			Message_Add(CSG_String::Format(SG_T(" %+f*[%s]"), m_Regression.Get_RCoeff(j), pGrids->asGrid(j)->Get_Name()), false);
		}
	}

	Message_Add(SG_T("\n"), false);
	Message_Add(CSG_String::Format(SG_T("\n%s:"), _TL("Correlation")), false);

	for(i=0; i<pGrids->Get_Count(); i++)
	{
		if( (j = m_Regression.Get_Ordered(i)) >= 0 && j < pGrids->Get_Count() )
		{
			Message_Add(CSG_String::Format(SG_T("\n%d: R\xc2\xb2 = %f%% [%f%%] -> %s"), i + 1, 100.0 * m_Regression.Get_R2(j), 100.0 * m_Regression.Get_R2_Change(j), pGrids->asGrid(j)->Get_Name()), false);
		}
	}

	Message_Add(SG_T("\n"), false);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
