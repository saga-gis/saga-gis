
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_points                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                snap_points_to_grid.cpp                //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
#include "snap_points_to_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSnap_Points_to_Grid::CSnap_Points_to_Grid(void)
{
	Set_Name		(_TL("Snap Points to Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Moves all points to grid cell positions that have the highest or"
		"lowest value respectively within the given search distance around "
		"each point."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("", "INPUT"   , _TL("Points"         ), _TL(""), PARAMETER_INPUT          , SHAPE_TYPE_Point);
	Parameters.Add_Grid  ("", "GRID"    , _TL("Grid"           ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Shapes("", "OUTPUT"  , _TL("Result"         ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point);
	Parameters.Add_Shapes("", "MOVES"   , _TL("Moves"          ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line );
	Parameters.Add_Double("", "DISTANCE", _TL("Search Distance"), _TL("map units"), 0., 0., true);

	Parameters.Add_Choice("", "SHAPE"   , _TL("Search Shape"   ), _TL(""), CSG_String::Format("%s|%s",
		_TL("circle"),
		_TL("square")), 0
	);

	Parameters.Add_Choice("", "EXTREME" , _TL("Extreme"        ), _TL(""), CSG_String::Format("%s|%s",
		_TL("minimum"),
		_TL("maximum")), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSnap_Points_to_Grid::On_Execute(void)
{
	CSG_Shapes *pInput = Parameters("INPUT")->asShapes();

	if( !pInput->is_Valid() )
	{
		Error_Set(_TL("invalid points layer"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pGrid = Parameters("GRID")->asGrid();

	if( pInput->Get_Extent().Intersects(pGrid->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("points layer and grid do not intersect at all"));

		return( false );
	}

	//-----------------------------------------------------
	double Distance = Parameters("DISTANCE")->asDouble() / Get_Cellsize();

	if( Distance < 0.5 )
	{
		if( !Error_Set(_TL("search distance is less than half of grid cell size")) )	// ignore
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	CSG_Shapes *pPoints = Parameters("OUTPUT")->asShapes();

	if( pPoints && pPoints != pInput )
	{
		pPoints->Create(*pInput);
	}
	else
	{
		Parameters("RESULT")->Set_Value(pPoints	= pInput);
	}

	pPoints->Fmt_Name("%s [%s: %s]", pInput->Get_Name(), _TL("snapped"), pGrid->Get_Name());

	//-----------------------------------------------------
	CSG_Shapes *pMoves = Parameters("MOVES")->asShapes();

	if( pMoves )
	{
		pMoves->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s: %s]", pInput->Get_Name(), _TL("snap move"), pGrid->Get_Name()), pPoints);
	}

	//-----------------------------------------------------
	CSG_Grid_Cell_Addressor Search;

	Search.Set_Radius(Distance, Parameters("SHAPE")->asInt() == 1);

	int Sign = Parameters("EXTREME")->asInt() == 1 ? 1 : -1;

	//-----------------------------------------------------
	for(sLong iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		CSG_Shape *pPoint = pPoints->Get_Shape(iPoint); TSG_Point Point = pPoint->Get_Point();

		int x = Get_System().Get_xWorld_to_Grid(Point.x);
		int y = Get_System().Get_yWorld_to_Grid(Point.y);

		int iMax = -1; double dMax;

		for(int i=0; i<Search.Get_Count(); i++)
		{
			int ix = Search.Get_X(i, x);
			int iy = Search.Get_Y(i, y);

			if( pGrid->is_InGrid(ix, iy) )
			{
				if( iMax < 0 || dMax < Sign * pGrid->asDouble(ix, iy) )
				{
					iMax = i; dMax = Sign * pGrid->asDouble(ix, iy);
				}
			}
		}

		//-------------------------------------------------
		if( iMax >= 0 )
		{
			TSG_Point snap_Point = pGrid->Get_System().Get_Grid_to_World(Search.Get_X(iMax, x), Search.Get_Y(iMax, y));

			if( SG_Get_Distance(Point, snap_Point) > 0. )
			{
				pPoint->Set_Point(snap_Point, 0);

				if( pMoves )
				{
					CSG_Shape *pMove = pMoves->Add_Shape(pPoint, SHAPE_COPY_ATTR);

					pMove->Add_Point(Point);
					pMove->Add_Point(snap_Point);
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
