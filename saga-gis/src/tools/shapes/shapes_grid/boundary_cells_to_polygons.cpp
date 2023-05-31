
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              boundary_cells_to_polygons.cpp           //
//                                                       //
//                 Copyright (C) 2023 by                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "boundary_cells_to_polygons.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CBoundary_Cells_to_Polygons::CBoundary_Cells_to_Polygons(void)
{
	Set_Name		(_TL("Boundary Cells to Polygons"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"The \'Boundary Cells to Polygons\' tool to constructs polygons "
		"taking all no-data cells (or those cells having a specified value) "
		"as potential boundary cells surrounding each polygon. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"GRID"          , _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"POLYGONS"      , _TL("Polygons"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice("",
		"BOUNDARY_CELLS", _TL("Boundary Cells"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("no data"),
			_TL("value")
		), 0
	);

	Parameters.Add_Double("BOUNDARY_CELLS",
		"BOUNDARY_VALUE", _TL("Value"),
		_TL(""),
		0.
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CBoundary_Cells_to_Polygons::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("BOUNDARY_CELLS") )
	{
		pParameters->Set_Enabled("BOUNDARY_VALUE", pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBoundary_Cells_to_Polygons::On_Execute(void)
{
	CSG_Grid Mask; sLong nPolygons = Set_Mask(Mask);

	if( nPolygons < 1 )
	{
		Error_Fmt(_TL("no area surrounded by boundary cells has been detected!"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();
	pPolygons->Create(SHAPE_TYPE_Polygon);
	pPolygons->Add_Field("ID", SG_DATATYPE_Int);
	pPolygons->Fmt_Name(Mask.Get_Name());

	for(int y=0; y<Get_NY() && Set_Progress(pPolygons->Get_Count(), nPolygons); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( Mask.asInt(x, y) > 0 )
			{
				CSG_Shape *pPolygon = pPolygons->Add_Shape();

				pPolygon->Set_Value(0, pPolygons->Get_Count());

				if( !Get_Polygon(Mask, x, y, pPolygon) )
				{
					pPolygons->Del_Shape(pPolygon);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( true ) // lakes
	{
		for(sLong i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
		{
			CSG_Shape_Polygon *pA = pPolygons->Get_Shape(i)->asPolygon();

			for(sLong j=i+1; j<pPolygons->Get_Count(); j++)
			{
				CSG_Shape_Polygon *pB = pPolygons->Get_Shape(j)->asPolygon();

				if( pA->Intersects(pB) == INTERSECTION_Contains )
				{
					pA->Add_Part(pB->Get_Part(0));
				}
				else if( pB->Intersects(pA) == INTERSECTION_Contains )
				{
					pB->Add_Part(pA->Get_Part(0));
				}
			}
		}
	}

	return( pPolygons->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MASK_CANDIDATE  0
#define MASK_PROCESSED -1
#define MASK_BOUNDARY  -2

//---------------------------------------------------------
inline bool CBoundary_Cells_to_Polygons::is_Boundary(CSG_Grid &Mask, int x, int y, int Direction)
{
	x += Get_xTo(Direction); y += Get_yTo(Direction);

	return( Mask.is_InGrid(x, y, false) && Mask.asInt(x, y) == MASK_BOUNDARY );
}

//---------------------------------------------------------
bool CBoundary_Cells_to_Polygons::Find_Next_Boundary(CSG_Grid &Mask, int x, int y, int &Direction, bool bClockwise)
{
	int n = bClockwise ? -1 : 1;

	for(int i=0; i<8; i++, Direction+=n)
	{
		if( is_Boundary(Mask, x, y, Direction) )
		{
			if( (Direction % 2) && is_Boundary(Mask, x, y, Direction + n) ) // prefer orthogonal neighbour direction
			{
				Direction += n;
			}

			Direction = (Direction + 8) % 8;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CBoundary_Cells_to_Polygons::Get_Polygon(CSG_Grid &Mask, int x, int y, CSG_Shape *pPolygon)
{
	int x0 = x, y0 = y, id = Mask.asInt(x, y);

	if( id < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid_Stack Boundary, Stack; Stack.Push(x0, y0);

	while( Stack.Get_Size() > 0 )//&& Process_Get_Okay() )
	{
		Stack.Pop(x, y);

		for(int i=0; i<8; i++)
		{
			int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

			if( Mask.asInt(ix, iy) == 0 )
			{
				Boundary.Push(ix, iy); Mask.Set_Value(ix, iy, MASK_BOUNDARY ); // to be processed now
			}
			else if( Mask.asInt(ix, iy) == id )
			{
				Stack   .Push(ix, iy); Mask.Set_Value(ix, iy, MASK_PROCESSED); // mark as processed
			}
		}
	}

	//-----------------------------------------------------
	int Direction = 0;

	for(int i=0; i<8; i++, Direction++)
	{
		if( Mask.asInt(Get_xTo(Direction, x0), Get_yTo(Direction, y0)) == MASK_BOUNDARY )
		{
			break;
		}
	}

	x = Get_xTo(Direction, x0);
	y = Get_yTo(Direction, y0);

	Find_Next_Boundary(Mask, x, y, Direction += 3);

	x0 = x = Get_xTo(Direction, x);
	y0 = y = Get_yTo(Direction, y);

	//-----------------------------------------------------
	do
	{
		if( pPolygon->Get_Point_Count() >= (sLong)(2 * Boundary.Get_Size()) )
		{ // under certain circumstances a boundary cell can be visited twice, ...but this should not happen very often!
			break;
		}

		pPolygon->Add_Point(Mask.Get_System().Get_Grid_to_World(x, y));

		Find_Next_Boundary(Mask, x, y, Direction += 2);

		x += Get_xTo(Direction);
		y += Get_yTo(Direction);
	}
	while( x != x0 || y != y0 ); // stop when polygon is closed

	//-----------------------------------------------------
	while( Boundary.Pop(x, y) )
	{
		Mask.Set_Value(x, y, MASK_CANDIDATE); // restore boundary cell state
	}

	return( pPolygon->asPolygon()->Get_Area() > 0. );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBoundary_Cells_to_Polygons::Set_Mask(CSG_Grid &Mask, int x, int y, int id, const CSG_Grid &Grid)
{
	if( Grid.is_InGrid(x - 1, y - 1) )
	{
		Mask.Set_Value(x, y, id);

		CSG_Grid_Stack Stack; Stack.Push(x, y);

		while( Stack.Get_Size() > 0 )//&& Process_Get_Okay() )
		{
			Stack.Pop(x, y);

			for(int i=0; i<8; i+=2)
			{
				int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

				if(	Grid.is_InGrid(ix - 1, iy - 1) && !Mask.asInt(ix, iy) )
				{
					bool bInterior = true;

					for(int j=0; bInterior && j<8; j+=2)
					{
						bInterior = Grid.is_InGrid(Get_xTo(j, ix) - 1, Get_yTo(j, iy) - 1);
					}

					Mask.Set_Value(ix, iy, bInterior ? MASK_PROCESSED : id);

					Stack.Push(ix, iy);
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
sLong CBoundary_Cells_to_Polygons::Set_Mask(CSG_Grid &Mask)
{
	CSG_Grid Grid, *pGrid = Parameters("GRID")->asGrid();

	if( Parameters("BOUNDARY_CELLS")->asInt() == 1 )
	{
		Grid.Create(Get_System(), SG_DATATYPE_Char);
		Grid.Set_Name(pGrid->Get_Name());
		Grid.Set_NoData_Value(0);

		double Value = Parameters("BOUNDARY_VALUE")->asDouble();

		#pragma omp parallel for
		for(int i=0; i<Get_NCells(); i++)
		{
			Grid.Set_Value(i, pGrid->asDouble(i) == Value ? 0 : 1);
		}

		pGrid = &Grid;
	}

	Mask.Create(SG_DATATYPE_Int, Get_NX() + 2, Get_NY() + 2, Get_Cellsize(), Get_XMin() - Get_Cellsize(), Get_YMin() - Get_Cellsize());
	Mask.Set_Name(pGrid->Get_Name());

	int nPolygons = 0;

	for(int y=0, my=1; y<Get_NY() && Set_Progress_Rows(y); y++, my++) for(int x=0, mx=1; x<Get_NX(); x++, mx++)
	{
		if( !Mask.asInt(mx, my) && !pGrid->is_NoData(x, y) )
		{
			Set_Mask(Mask, mx, my, ++nPolygons, *pGrid);
		}
	}

	return( nPolygons );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
