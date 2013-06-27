/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                grid_rectangle_clip.cpp                //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid_rectangle_clip.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Rectangle_Clip::CGrid_Rectangle_Clip(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Clip Grid with Rectangle"));

	Set_Author		(_TL("V. Wichmann (c) 2013"));

	Set_Description	(_TW(
		"Clips the input grid with the (rectangular) extent of a shapefile. "
		"The clipped grid will have the extent of the shapefile.\n"
		"Select shapes from the shapefile prior to module execution "
        "in case you like to use only a subset from the shapefile "
        "for clipping.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Input"),
		_TL("The grid to clip."),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Extent"),
		_TL("The shapefile to use for clipping."),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		NULL	, "CELLS"		, _TL("Border"),
		_TL("Set grid extent to grid cells (pixel as area) or to grid nodes (pixel as point)."),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("grid cells"),
			_TL("grid nodes")
		), 0
	);

	Parameters.Add_Grid_Output(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL("The clipped grid.")
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Rectangle_Clip::On_Execute(void)
{
	CSG_Grid		*pGrid_in, *pGrid_out;
	CSG_Shapes		*pShapes;
	bool			bCells;


	//-----------------------------------------------------
	pGrid_in	= Parameters("INPUT")	->asGrid();
	pShapes		= Parameters("SHAPES")	->asShapes();
	bCells		= Parameters("CELLS")	->asInt() == 0;


	//-----------------------------------------------------
	CSG_Rect	Extent;

	if( pShapes->Get_Selection_Count() > 0 )
	{
		bool	bFirst = true;

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			if( pShapes->Get_Shape(iShape)->is_Selected() )
			{
				if( bFirst )
				{
					Extent = pShapes->Get_Shape(iShape)->Get_Extent();
					bFirst = false;
				}
				else
				{
					Extent.Union(pShapes->Get_Shape(iShape)->Get_Extent());
				}
			}
		}
	}
	else
		Extent = pShapes->Get_Extent();


	//-----------------------------------------------------
	if( bCells )
		Extent.Deflate(0.5 * pGrid_in->Get_Cellsize(), 0.5 * pGrid_in->Get_Cellsize(), false);

	CSG_Grid_System		GridSystem(pGrid_in->Get_Cellsize(), Extent);

	pGrid_out = SG_Create_Grid(GridSystem, pGrid_in->Get_Type());
	pGrid_out->Set_NoData_Value(pGrid_in->Get_NoData_Value());
	pGrid_out->Set_Name(CSG_String::Format(SG_T("%s_clip"), pGrid_in->Get_Name()));
	pGrid_out->Assign_NoData();

	//-----------------------------------------------------
	for(int y=0; y<pGrid_out->Get_NY() && Set_Progress(y, pGrid_out->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pGrid_out->Get_NX(); x++)
		{
			double	xWorld	= GridSystem.Get_xGrid_to_World(x);
			double	yWorld	= GridSystem.Get_yGrid_to_World(y);

			int		xGrid, yGrid;
			pGrid_in->Get_System().Get_World_to_Grid(xGrid, yGrid, xWorld, yWorld);

			if (pGrid_in->is_InGrid(xGrid, yGrid))
				pGrid_out->Set_Value(x, y, pGrid_in->asDouble(xGrid, yGrid));
		}
	}


	//-----------------------------------------------------
	Parameters("OUTPUT")->Set_Value(pGrid_out);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
