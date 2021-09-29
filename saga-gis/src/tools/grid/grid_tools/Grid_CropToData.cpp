
/*******************************************************************************
    CropToData.cpp
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_CropToData.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCropToData::CCropToData(void)
{
	Set_Name		(_TL("Crop to Data"));

	Set_Author		("V.Olaya (c) 2004");

	Set_Description	(_TW(
		"Crop grids to valid data cells. "
	));

	Parameters.Add_Grid_List("",
		"INPUT"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"OUTPUT", _TL("Cropped Grids"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, false
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCropToData::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pGrids	= Parameters("INPUT")->asGridList();

	if( pGrids->Get_Grid_Count() <= 0 )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text("%s...", _TL("analyzing"));

	bool bCrop = false; int xMin, yMin, xMax, yMax;

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			bool bData = false;

			for(int i=0; i<pGrids->Get_Grid_Count() && !bData; i++)
			{
				bData = !pGrids->Get_Grid(i)->is_NoData(x, y);
			}

			if( bData )
			{
				if( bCrop == false )
				{
					bCrop = true;

					xMin = xMax = x;
					yMin = yMax = y;
				}
				else
				{
					if( xMin > x ) { xMin = x; } else if( xMax < x ) { xMax = x; }
					if( yMin > y ) { yMin = y; } else if( yMax < y ) { yMax = y; }
				}
			}
		}
	}

	//-----------------------------------------------------
	if( bCrop == false )
	{
		Message_Fmt("\n%s: %s", _TL("nothing to crop"), _TL("no valid data found in grid(s)"));

		return( false );
	}

	CSG_Grid_System System( Get_Cellsize(),
		Get_XMin() + xMin * Get_Cellsize(),
		Get_YMin() + yMin * Get_Cellsize(),
		1 + xMax - xMin,
		1 + yMax - yMin
	);

	if( System.Get_NX() == Get_NX() && System.Get_NY() == Get_NY() )
	{
		Message_Fmt("\n%s: %s", _TL("nothing to crop"), _TL("valid data cells match original grid extent"));

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text("%s...", _TL("cropping"));

	Message_Fmt("\n%s\nx: %d - %d -> (%d)\ny: %d - %d -> %d", _TL("cropping"),
		Get_NX(), Get_NX() - System.Get_NX(), System.Get_NX(),
		Get_NY(), Get_NY() - System.Get_NY(), System.Get_NY()
	);

	CSG_Parameter_Grid_List *pCropped = Parameters("OUTPUT")->asGridList(); pCropped->Del_Items();

	for(int i=0; i<pGrids->Get_Grid_Count(); i++)
	{
		CSG_Grid *pGrid = pGrids->Get_Grid(i);
		CSG_Grid *pCrop = SG_Create_Grid(System, pGrid->Get_Type());

		pCrop->Set_Name             (pGrid->Get_Name        ());
		pCrop->Set_Description      (pGrid->Get_Description ());
		pCrop->Set_Unit             (pGrid->Get_Unit        ());
		pCrop->Set_Scaling          (pGrid->Get_Scaling     ());
		pCrop->Set_NoData_Value     (pGrid->Get_NoData_Value());
		pCrop->Get_MetaData().Create(pGrid->Get_MetaData    ());

		pCrop->Assign(pGrid, GRID_RESAMPLING_NearestNeighbour);

		pCropped->Add_Item(pCrop);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
