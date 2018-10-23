/**********************************************************
 * Version $Id$
 *********************************************************/

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
//            grid_local_extremes_to_points.cpp          //
//                                                       //
//                 Copyright (C) 2009 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid_local_extremes_to_points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Local_Extremes_to_Points::CGrid_Local_Extremes_to_Points(void)
{
	Set_Name		(_TL("Local Minima and Maxima"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"Extracts local grid value minima and maxima of to vector points."
	));

	Parameters.Add_Grid(
		"", "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		"", "MINIMA"	, _TL("Minima"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		"", "MAXIMA"	, _TL("Maxima"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Bool(
		"", "IDENTITY"	, _TL("Identical Values"),
		_TL("If set, neighbour cells with the same value as the center cell are interpreted to be lower for minima and higher for maxima identification."),
		false
	);

	Parameters.Add_Bool(
		"", "ABSOLUTE"	, _TL("Absolute"),
		_TL("If set, only the grid's absolute maximum/minimum point is returned."),
		false
	);

	Parameters.Add_Bool(
		"", "BOUNDARY"	, _TL("Boundary Cells"),
		_TL(""),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Local_Extremes_to_Points::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	bool	bIdentity	= Parameters("IDENTITY")->asBool();
	bool	bAbsolute	= Parameters("ABSOLUTE")->asBool();
	bool	bBoundary	= Parameters("BOUNDARY")->asBool();

	//-----------------------------------------------------
	CSG_Shapes	*pMinima	= Parameters("MINIMA")->asShapes();

	pMinima->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", pGrid->Get_Name(), _TL("Local Minima")));
	pMinima->Add_Field("GRID_X", SG_DATATYPE_Int   );
	pMinima->Add_Field("GRID_Y", SG_DATATYPE_Int   );
	pMinima->Add_Field("X"     , SG_DATATYPE_Double);
	pMinima->Add_Field("Y"     , SG_DATATYPE_Double);
	pMinima->Add_Field("Z"     , SG_DATATYPE_Double);

	//-----------------------------------------------------
	CSG_Shapes	*pMaxima	= Parameters("MAXIMA")->asShapes();

	pMaxima->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", pGrid->Get_Name(), _TL("Local Maxima")));
	pMaxima->Add_Field("GRID_X", SG_DATATYPE_Int   );
	pMaxima->Add_Field("GRID_Y", SG_DATATYPE_Int   );
	pMaxima->Add_Field("X"     , SG_DATATYPE_Double);
	pMaxima->Add_Field("Y"     , SG_DATATYPE_Double);
	pMaxima->Add_Field("Z"     , SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				double	z	= pGrid->asDouble(x, y);

				bool	bMinimum	= true;
				bool	bMaximum	= true;

				for(int i=0; i<8 && (bMinimum || bMaximum); i++)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( pGrid->is_InGrid(ix, iy) )
					{
						double	iz	= pGrid->asDouble(ix, iy);

						if( bIdentity )
						{
							if( iz < z )
							{
								bMinimum	= false;
							}

							if( iz > z )
							{
								bMaximum	= false;
							}
						}
						else
						{
							if( iz <= z )
							{
								bMinimum	= false;
							}

							if( iz >= z )
							{
								bMaximum	= false;
							}
						}
					}
					else if( !bBoundary )
					{
						bMinimum	= bMaximum	= false;
					}
				}

				//-----------------------------------------
				if( bAbsolute )
				{
					if( bMinimum && pMinima->Get_Count() > 0 && pMinima->Get_Shape(0)->asDouble(4) <= z )	{	bMinimum	= false;	}
					if( bMaximum && pMaxima->Get_Count() > 0 && pMaxima->Get_Shape(0)->asDouble(4) >= z )	{	bMaximum	= false;	}
				}

				CSG_Shape	*pPoint	=
					bMinimum ? (bAbsolute && pMinima->Get_Count() > 0 ? pMinima->Get_Shape(0) : pMinima->Add_Shape()) :
					bMaximum ? (bAbsolute && pMaxima->Get_Count() > 0 ? pMaxima->Get_Shape(0) : pMaxima->Add_Shape()) : NULL;

				if( pPoint )
				{
					TSG_Point	p	= Get_System().Get_Grid_to_World(x, y);

					pPoint->Set_Point(p,   0);
					pPoint->Set_Value(0,   x);
					pPoint->Set_Value(1,   y);
					pPoint->Set_Value(2, p.x);
					pPoint->Set_Value(3, p.y);
					pPoint->Set_Value(4,   z);
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
