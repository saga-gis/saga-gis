/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Grid_Buffer.cpp
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
#include "Grid_Buffer.h"

//---------------------------------------------------------
enum
{
	EMPTY	= 0,
	BUFFER	= 1,
	FEATURE	= 2
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Buffer::CGrid_Buffer(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Grid Buffer"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"This tool creates buffers around features in a grid. Features are defined by any value greater than zero. "
		"With the buffer distance method 'cell's value', the feature grid's cell values are used as buffer distance. "
		"In any case the buffer distance has to be specified using map units. "
		"The output buffer grid cell values refer to 1 := inside the buffer, 2 := feature location. "
	));

	Parameters.Add_Grid(
		NULL	, "FEATURES"	, _TL("Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "BUFFER"		, _TL("Buffer"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Type"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("fixed"),
			_TL("cell's value")
		), 0
	);

	Parameters.Add_Double(
		pNode	, "DISTANCE"	, _TL("Distance"),
		_TL("Fixed buffer distance given in map units."),
		1000.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Buffer::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TYPE") )
	{
		pParameters->Set_Enabled("DISTANCE", pParameter->asInt() == 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Buffer::On_Execute(void)
{
	CSG_Grid	*pFeatures	= Parameters("FEATURES")->asGrid();
	CSG_Grid	*pBuffer	= Parameters("BUFFER"  )->asGrid();

	pBuffer->Set_NoData_Value(EMPTY);
	pBuffer->Assign_NoData();
	pBuffer->Set_Name("%s [%s]", pFeatures->Get_Name(), _TL("Buffer"));

	bool	bFixed	= Parameters("TYPE")->asInt() == 0;

	int	Distance	= (int)(0.5 + Parameters("DISTANCE")->asDouble() / Get_Cellsize());

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pFeatures->is_NoData(x, y) && pFeatures->asDouble(x, y) > 0.0 )
			{
				if( !bFixed )
				{
					Distance	= (int)(0.5 + pFeatures->asDouble(x, y) / Get_Cellsize());
				}

				for(int iy=y-Distance; iy<=y+Distance; iy++)
				{
					for(int ix=x-Distance; ix<=x+Distance; ix++)
					{
						if( is_InGrid(ix, iy) && pBuffer->is_NoData(ix, iy) && SG_Get_Distance(x, y, ix, iy) <= Distance )
						{
							pBuffer->Set_Value(ix, iy, pFeatures->is_NoData(ix, iy) || pFeatures->asDouble(ix, iy) <= 0.0
								? BUFFER : FEATURE
							);
						}
					}
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
