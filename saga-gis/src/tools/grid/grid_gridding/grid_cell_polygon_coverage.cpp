
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             grid_cell_polygon_coverage.cpp            //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "grid_cell_polygon_coverage.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Cell_Polygon_Coverage::CGrid_Cell_Polygon_Coverage(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Grid Cell Area Covered by Polygons"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool calculates for each grid cell of the selected grid system "
		"the area that is covered by the input polygons layer. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(NULL,
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Grid(NULL,
		"AREA"		, _TL("Area of Coverage"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(NULL,
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("area"),
			_TL("percentage")
		), 0
	);

	Parameters.Add_Bool(NULL,
		"SELECTION"	, _TL("Only Selected Polygons"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Cell_Polygon_Coverage::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "POLYGONS") )
	{
		pParameters->Set_Enabled("SELECTION", pParameter->asShapes() && pParameter->asShapes()->Get_Selection_Count() > 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cell_Polygon_Coverage::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pArea	= Parameters("AREA")->asGrid();

	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if( pPolygons->Get_Count() <= 0 || !pPolygons->Get_Extent().Intersects(pArea->Get_Extent()) )
	{
		Error_Set(_TL("no spatial intersection between grid system and polygon layer"));

		return( false );
	}

	//-----------------------------------------------------
	bool	bSelection	= pPolygons->Get_Selection_Count() > 0 ? Parameters("SELECTION")->asBool() : false;
	bool	bPercentage	= Parameters("OUTPUT")->asInt() == 1;

	pArea->Set_Name(CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("Coverage"));

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	py	= Get_YMin() + Get_Cellsize() * (y - 0.5);

		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			double	px	= Get_XMin() + Get_Cellsize() * (x - 0.5);

			CSG_Shapes	Cell(SHAPE_TYPE_Polygon);
			CSG_Shape_Polygon	*pCell	= (CSG_Shape_Polygon *)Cell.Add_Shape();

			pCell->Add_Point(px                 , py                 );
			pCell->Add_Point(px + Get_Cellsize(), py                 );
			pCell->Add_Point(px + Get_Cellsize(), py + Get_Cellsize());
			pCell->Add_Point(px                 , py + Get_Cellsize());

			//---------------------------------------------
			if( pPolygons->Get_Extent().Intersects(pCell->Get_Extent()) )
			{
				if( bSelection )
				{
					for(int i=0; pCell->Get_Area() > 0.0 && i<pPolygons->Get_Selection_Count(); i++)
					{
						SG_Polygon_Difference(pCell, (CSG_Shape_Polygon *)pPolygons->Get_Selection(i));
					}
				}
				else
				{
					for(int i=0; pCell->Get_Area() > 0.0 && i<pPolygons->Get_Count(); i++)
					{
						SG_Polygon_Difference(pCell, (CSG_Shape_Polygon *)pPolygons->Get_Shape(i));
					}
				}
			}

			double	Area	= Get_Cellarea() - pCell->Get_Area();

			pArea->Set_Value(x, y, bPercentage ? 100.0 * Area / Get_Cellarea() : Area);
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
