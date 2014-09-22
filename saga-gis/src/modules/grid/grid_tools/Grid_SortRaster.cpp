/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    SortRaster.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

#include "Grid_SortRaster.h"

CSortRaster::CSortRaster(void)
{
	Set_Name		(_TL("Grid Cell Index"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"Creates an index grid according to the cell values either in ascending or descending order."
	));

	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"), 						
		_TL(""), 
		PARAMETER_INPUT
	);
	
	Parameters.Add_Grid(
		NULL	, "INDEX"	, _TL("Index"), 						
		_TL(""), 
		PARAMETER_OUTPUT, true, SG_DATATYPE_Long
	);

	Parameters.Add_Choice(
		NULL	, "ORDER"	, _TL("Sorting Order"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("ascending"),
			_TL("descending")
		)
	);
}

bool CSortRaster::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	if( !pGrid->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	CSG_Grid	*pIndex	= Parameters("INDEX")->asGrid();

	bool	bDown	= Parameters("ORDER")->asInt() == 1;

	if( pGrid == pIndex )
	{
		pIndex	= SG_Create_Grid(pGrid);
	}

	pIndex->Set_NoData_Value(0.0);

	for(sLong i=0, Index=0; i<Get_NCells() && Set_Progress(i, Get_NCells()); i++)
	{
		int	ix, iy;

		if( pGrid->Get_Sorted(i, ix, iy, bDown) )
		{
			pIndex->Set_Value(ix, iy, ++Index);
		}
		else
		{
			pIndex->Set_NoData(ix, iy);
		}
	}

	if( pGrid == Parameters("INDEX")->asGrid() )
	{
		pGrid->Assign(pIndex);

		delete(pIndex);
	}

	return( true );
}
