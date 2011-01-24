
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

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Spatial Join: Retrieves information from the selected grids at the positions "
		"of the points of the selected points layer and adds it to the resulting layer."
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
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Values_AddTo_Points::On_Execute(void)
{
	int						iShape, iGrid, iField, Offset, Interpolation;
	double					Value;
	CSG_Shapes				*pShapes;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pShapes			= Parameters("RESULT")		->asShapes();
	pGrids			= Parameters("GRIDS" )		->asGridList();
	Interpolation	= Parameters("INTERPOL")	->asInt();

	//-----------------------------------------------------
	if( pGrids->Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( pShapes == NULL )
	{
		pShapes		= Parameters("SHAPES")->asShapes();
	}
	else if( pShapes != Parameters("SHAPES")->asShapes() )
	{
		pShapes->Create(*Parameters("SHAPES")->asShapes());
	}

	Offset		= pShapes->Get_Field_Count();

	//-----------------------------------------------------
	for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
	{
		pShapes->Add_Field(pGrids->asGrid(iGrid)->Get_Name(), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		for(iGrid=0, iField=Offset; iGrid<pGrids->Get_Count(); iGrid++, iField++)
		{
			CSG_Grid	*pGrid	= pGrids->asGrid(iGrid);

			if( pGrid->Get_Value(pShape->Get_Point(0), Value, Interpolation) )
			{
				pShape->Set_Value(iField, Value);
			}
			else
			{
				pShape->Set_NoData(iField);
			}
		}
	}

	//-----------------------------------------------------
	if( pShapes == Parameters("SHAPES")->asShapes() )
	{
		DataObject_Update(pShapes);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
