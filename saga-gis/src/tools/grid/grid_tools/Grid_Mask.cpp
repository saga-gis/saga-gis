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
//                     Grid_Tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Grid_Mask.cpp                      //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#include "Grid_Mask.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Mask::CGrid_Mask(void)
{
	Set_Name		(_TL("Grid Masking"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Cells of the input grid will be set to no-data, if their cell "
		"center lies outside or within a no-data cell of the mask grid."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "MASK"	, _TL("Mask"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid(
		"", "MASKED", _TL("Masked Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Mask::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();
	CSG_Grid	*pMask	= Parameters("MASK")->asGrid();

	if( !pGrid->is_Intersecting(pMask->Get_Extent()) )
	{
		Message_Add(_TL("no intersection with mask grid."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pMasked	= Parameters("MASKED")->asGrid();

	if( pMasked && pMasked != pGrid )
	{
		pMasked->Create(*pGrid);
		pMasked->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("masked"));

		pGrid	= pMasked;
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("masking..."));

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	py	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				double	px	= Get_XMin() + x * Get_Cellsize();

				if( !pMask->is_InGrid_byPos(px, py) )
				{
					pGrid->Set_NoData(x, y);
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
