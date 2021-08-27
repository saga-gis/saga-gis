/*******************************************************************************
    InvertNoData.cpp
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_InvertNoData.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInvertNoData::CInvertNoData(void)
{
	Set_Name		(_TL("Invert Data/No-Data"));

	Set_Author		("Victor Olaya (c) 2005");

	Set_Description	(_TW(
		"Converts valid data cells to no-data cells and no-data cells to "
		"the user specified value. Mostly suitable when dealing with masks."
	));

	Parameters.Add_Grid("",
		"INPUT" , _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);
	
	Parameters.Add_Grid("", 
		"OUTPUT", _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double("",
		"VALUE" , _TL("Data Value"),
		_TL(""),
		1.
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInvertNoData::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("OUTPUT")->asGrid();

	if( pGrid && pGrid != Parameters("INPUT")->asGrid() )
	{
		pGrid->Create(*Parameters("INPUT")->asGrid());

		pGrid->Fmt_Name("%s [%s]", Parameters("INPUT")->asGrid()->Get_Name(), _TL("inverse no-data"));
	}
	else
	{
		pGrid	= Parameters("INPUT")->asGrid();
	}

	//-----------------------------------------------------
	double	Value	= Parameters("VALUE")->asDouble();

	if( SG_IS_BETWEEN(pGrid->Get_NoData_Value(), Value, pGrid->Get_NoData_Value(true)) )
	{
		Message_Fmt("%s: %s", _TL("Warning"), _TL("targeted data value is a no-data value"));
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pGrid->is_NoData(x, y) )
			{
				pGrid->Set_Value(x, y, Value);
			}
			else
			{
				pGrid->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	if( pGrid != Parameters("OUTPUT")->asGrid() )
	{
		DataObject_Update(pGrid);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
