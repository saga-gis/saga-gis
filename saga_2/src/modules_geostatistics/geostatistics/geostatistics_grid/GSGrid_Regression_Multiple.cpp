
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
	CParameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Multiple Regression Analysis (Grids/Points)"));

	Set_Author		(_TL("Copyrights (c) 2004 by Olaf Conrad"));

	Set_Description	(_TL(
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

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Details"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "RESIDUAL"	, _TL("Residuals"),
		"",
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		"",
		PARAMETER_OUTPUT
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
}

//---------------------------------------------------------
CGSGrid_Regression_Multiple::~CGSGrid_Regression_Multiple(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Regression_Multiple::On_Execute(void)
{
	int						iAttribute;
	CTable					*pTable;
	CShapes					*pShapes, *pResiduals;
	CGrid					*pRegression;
	CParameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids			= Parameters("GRIDS")		->asGridList();
	pRegression		= Parameters("REGRESSION")	->asGrid();
	pTable			= Parameters("TABLE")		->asTable();
	pShapes			= Parameters("SHAPES")		->asShapes();
	pResiduals		= Parameters("RESIDUAL")	->asShapes();
	iAttribute		= Parameters("ATTRIBUTE")	->asInt();
	m_Interpolation	= Parameters("INTERPOL")	->asInt();

	//-----------------------------------------------------
	if( Get_Regression(pGrids, pShapes, iAttribute) )
	{
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
bool CGSGrid_Regression_Multiple::Get_Regression(CParameter_Grid_List *pGrids, CShapes *pShapes, int iAttribute)
{
	int				iShape, iPart, iPoint, iGrid;
	double			zShape, zGrid;
	TSG_Point		Point;
	CTable			Table;
	CTable_Record	*pRecord;
	CShape			*pShape;

	//-----------------------------------------------------
	Table.Destroy();
	Table.Add_Field(pShapes->Get_Name(), TABLE_FIELDTYPE_Double);

	for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
	{
		Table.Add_Field(pGrids->asGrid(iGrid)->Get_Name(), TABLE_FIELDTYPE_Double);
	}

	//-----------------------------------------------------
	for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		pShape	= pShapes->Get_Shape(iShape);
		zShape	= pShape->Get_Record()->asDouble(iAttribute);

		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				Point	= pShape->Get_Point(iPoint, iPart);
				pRecord	= Table.Add_Record();
				pRecord->Set_Value(0, zShape);

				for(iGrid=0; iGrid<pGrids->Get_Count() && pRecord; iGrid++)
				{
					if( pGrids->asGrid(iGrid)->Get_Value(Point, zGrid, m_Interpolation, GRID_ZFACTOR) )
					{
						pRecord->Set_Value(1 + iGrid, zGrid);
					}
					else
					{
						pRecord	= NULL;
						Table.Del_Record(Table.Get_Record_Count() - 1);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( m_Regression.Calculate(Table) );
}

//---------------------------------------------------------
bool CGSGrid_Regression_Multiple::Set_Regression(CParameter_Grid_List *pGrids, CGrid *pRegression)
{
	bool	bOk;
	int		x, y, i;
	double	z;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			z	= m_Regression.Get_RConst();

			for(i=0, bOk=true; i<pGrids->Get_Count() && bOk; i++)
			{
				if( (bOk = !pGrids->asGrid(i)->is_NoData(x, y)) == true )
				{
					z	+= m_Regression.Get_RCoeff(i) * pGrids->asGrid(i)->asDouble(x, y);
				}
			}

			if( bOk )
			{
				pRegression->Set_Value (x, y, z);
			}
			else
			{
				pRegression->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGSGrid_Regression_Multiple::Set_Residuals(CShapes *pShapes, int iAttribute, CShapes *pResiduals, CGrid *pRegression)
{
	int			iPoint, iPart, iShape;
	double		zShape, zGrid;
	TSG_Point	Point;
	CShape		*pShape, *pResidual;

	//-----------------------------------------------------
	if( pResiduals )
	{
		pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", pShapes->Get_Name(), _TL("Residuals")));
		pResiduals->Get_Table().Add_Field(pShapes->Get_Table().Get_Field_Name(iAttribute), TABLE_FIELDTYPE_Double);
		pResiduals->Get_Table().Add_Field("TREND"	, TABLE_FIELDTYPE_Double);
		pResiduals->Get_Table().Add_Field("RESIDUAL", TABLE_FIELDTYPE_Double);

		//-------------------------------------------------
		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			pShape	= pShapes->Get_Shape(iShape);
			zShape	= pShape->Get_Record()->asDouble(iAttribute);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Point	= pShape->Get_Point(iPoint, iPart);

					if( pRegression->Get_Value(Point, zGrid, m_Interpolation, GRID_ZFACTOR) )
					{
						pResidual	= pResiduals->Add_Shape();
						pResidual->Add_Point(Point);
						pResidual->Get_Record()->Set_Value(0, zShape);
						pResidual->Get_Record()->Set_Value(1, zGrid);
						pResidual->Get_Record()->Set_Value(2, zShape - zGrid);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
void CGSGrid_Regression_Multiple::Set_Message(CParameter_Grid_List *pGrids)
{
	int		i, j;

	Message_Add("\n", false);
	Message_Add(CSG_String::Format("\n%s:", _TL("Regression")), false);
	Message_Add(CSG_String::Format("\n Y = %f", m_Regression.Get_RConst()), false);

	for(i=0; i<pGrids->Get_Count(); i++)
	{
		if( (j = m_Regression.Get_Ordered(i)) >= 0 && j < pGrids->Get_Count() )
		{
			Message_Add(CSG_String::Format(" %+f*[%s]", m_Regression.Get_RCoeff(j), pGrids->asGrid(j)->Get_Name()), false);
		}
	}

	Message_Add("\n", false);
	Message_Add(CSG_String::Format("\n%s:", _TL("Correlation")), false);

	for(i=0; i<pGrids->Get_Count(); i++)
	{
		if( (j = m_Regression.Get_Ordered(i)) >= 0 && j < pGrids->Get_Count() )
		{
			Message_Add(CSG_String::Format("\n%d: R² = %f%% [%f%%] -> %s", i + 1, 100.0 * m_Regression.Get_R2(j), 100.0 * m_Regression.Get_R2_Change(j), pGrids->asGrid(j)->Get_Name()), false);
		}
	}

	Message_Add("\n", false);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
