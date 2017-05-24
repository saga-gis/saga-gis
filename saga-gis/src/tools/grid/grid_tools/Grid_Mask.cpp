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
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Grid Masking"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	// 2. Standard in- and output...

	Parameters.Add_Grid(
		NULL	, "GRID"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "MASK"			, _TL("Mask"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid(
		NULL	, "MASKED"			, _TL("Masked Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Mask::On_Execute(void)
{
	int			x, y;
	double		z;
	TSG_Point	p;
	CSG_Grid	*pGrid, *pMask, *pMasked;

	pGrid	= Parameters("GRID")	->asGrid();
	pMask	= Parameters("MASK")	->asGrid();
	pMasked	= Parameters("MASKED")	->asGrid();

	if( !pGrid->is_Intersecting(pMask->Get_Extent()) )
	{
		Message_Add(_TL("no intersection with mask grid."));

		return( false );
	}

	if( pMasked == NULL )
	{
		pMasked	= pGrid;

		Parameters("MASKED")->Set_Value(pMasked);
	}
	else if( pMasked != pGrid )
	{
		pMasked->Assign(pGrid);
	}

	Process_Set_Text(_TL("masking..."));

	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			if( !pMasked->is_NoData(x, y) && !pMask->Get_Value(p, z, GRID_RESAMPLING_NearestNeighbour) )
			{
				pMasked->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
