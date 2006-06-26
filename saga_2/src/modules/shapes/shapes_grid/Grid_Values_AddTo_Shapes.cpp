
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Grid_Values_AddTo_Shapes.cpp             //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#include "Grid_Values_AddTo_Shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Values_AddTo_Shapes::CGrid_Values_AddTo_Shapes(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Get Grid Data for Shapes"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Retrieves information from the selected grids at the positions of the shapes of "
		"the selected shapes layer and adds it to the resulting shapes layer.")
	);


	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		"",
		PARAMETER_INPUT, false
	);

	Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Shapes (Grid Information)"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "INTERPOL"	, _TL("Interpolation"),
		"",
		_TL(
		"Nearest Neighbor|"
		"Bilinear Interpolation|"
		"Inverse Distance Interpolation|"
		"Bicubic Spline Interpolation|"
		"B-Spline Interpolation|")	, 4
	);
}

//---------------------------------------------------------
CGrid_Values_AddTo_Shapes::~CGrid_Values_AddTo_Shapes(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Values_AddTo_Shapes::On_Execute(void)
{
	int			iGrid, iShape;
	CShapes		*pShapes;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Count() > 0 && Parameters("SHAPES")->asShapes()->Get_Type() == SHAPE_TYPE_Point )
	{
		pResult		= Parameters("RESULT")->asShapes();
		Interpol	= Parameters("INTERPOL")->asInt();

		if( pResult == Parameters("SHAPES")->asShapes() )
		{
			pShapes		= API_Create_Shapes(*pResult);
		}
		else
		{
			pShapes		= Parameters("SHAPES")->asShapes();
		}

		pResult->Create(pShapes->Get_Type());

		//-------------------------------------------------
		for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
		{
			pResult->Get_Table().Add_Field(pGrids->asGrid(iGrid)->Get_Name(), TABLE_FIELDTYPE_Double);
		}

		//-------------------------------------------------
		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			switch( pShapes->Get_Type() )
			{
			case SHAPE_TYPE_Point:
				Get_Data_Point(		pShapes->Get_Shape(iShape) );
				break;

			case SHAPE_TYPE_Line:
				Get_Data_Line(		pShapes->Get_Shape(iShape) );
				break;

			case SHAPE_TYPE_Polygon:
				Get_Data_Polygon(	pShapes->Get_Shape(iShape) );
				break;
			}
		}

		//-------------------------------------------------
		if( pResult == Parameters("SHAPES")->asShapes() )
		{
			delete(pShapes);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CGrid_Values_AddTo_Shapes::Get_Data_Point(CShape *pShape)
{
	int			iPart, iPoint, nValues, iGrid, Interpolation = 1;
	double		Value_Sum;
	TGEO_Point	Point;
	CGrid		*pGrid;
	CShape		*pShape_Result;

	pShape_Result	= pResult->Add_Shape();

	//-----------------------------------------------------
	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			pShape_Result->Add_Point(pShape->Get_Point(iPoint, iPart), iPart);
		}
	}

	//-----------------------------------------------------
	for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
	{
		Value_Sum	= 0.0;
		nValues		= 0;
		pGrid		= pGrids->asGrid(iGrid);

		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				Point	= pShape->Get_Point(iPoint, iPart);

				if( pGrid->is_InGrid_byPos(Point) )
				{
					Value_Sum	+= pGrid->Get_Value(Point, Interpol, true);
					nValues		++;
				}
			}
		}

		if( nValues > 0 )
		{
			pShape_Result->Get_Record()->Set_Value(iGrid, Value_Sum / (double)nValues);
		}
	}
}

//---------------------------------------------------------
void CGrid_Values_AddTo_Shapes::Get_Data_Line(CShape *pShape)
{
}

//---------------------------------------------------------
void CGrid_Values_AddTo_Shapes::Get_Data_Polygon(CShape *pShape)
{
}
