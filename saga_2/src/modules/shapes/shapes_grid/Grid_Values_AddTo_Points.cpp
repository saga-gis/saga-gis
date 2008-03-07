
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
//              Grid_Values_AddTo_Points.cpp             //
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
#include "Grid_Values_AddTo_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Values_AddTo_Points::CGrid_Values_AddTo_Points(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Add Grid Values to Points"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"Retrieves information from the selected grids at the positions of the points of "
		"the selected points layer and adds it to the resulting layer."
	));


	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT			, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT			, false
	);

	Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT		, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL	, "INTERPOL"	, _TL("Interpolation"),
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

//---------------------------------------------------------
CGrid_Values_AddTo_Points::~CGrid_Values_AddTo_Points(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Values_AddTo_Points::On_Execute(void)
{
	bool					bZFactor;
	int						iShape, iGrid, iField, nFields, Interpol;
	TSG_Point				Point;
	CSG_Grid					*pGrid;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Shape					*pShape;
	CSG_Shapes					*pShapes;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS" )	->asGridList();
	pShapes		= Parameters("SHAPES")	->asShapes();

	Interpol	= Parameters("INTERPOL")->asInt();
	bZFactor	= true;

	if( pGrids->Get_Count() > 0 && pShapes->Get_Type() == SHAPE_TYPE_Point )
	{
		if( pShapes != Parameters("RESULT")->asShapes() )
		{
			pShapes		= Parameters("RESULT")->asShapes();
			pShapes->Assign(Parameters("SHAPES")->asShapes());
		}

		nFields		= pShapes->Get_Table().Get_Field_Count();

		for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
		{
			pShapes->Get_Table().Add_Field(pGrids->asGrid(iGrid)->Get_Name(), TABLE_FIELDTYPE_Double);
		}

		//-------------------------------------------------
		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			pShape	= pShapes->Get_Shape(iShape);
			Point	= pShape->Get_Point(0);

			for(iGrid=0, iField=nFields; iGrid<pGrids->Get_Count(); iGrid++, iField++)
			{
				pGrid	= pGrids->asGrid(iGrid);

				pShape->Get_Record()->Set_Value(iField,
					pGrid->is_InGrid_byPos(Point) ? pGrid->Get_Value(Point, Interpol, bZFactor) : -99999
				);
			}
		}

		return( true );
	}

	return( false );
}
