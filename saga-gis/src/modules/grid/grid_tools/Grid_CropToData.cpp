/**********************************************************
 * Version $Id$
 *********************************************************/
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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

	Set_Author		(SG_T("V.Olaya (c) 2004"));

	Set_Description	(_TW(
		"Crop grids to valid data cells"
	));

	Parameters.Add_Grid_List(
		NULL	, "INPUT"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "OUTPUT"	, _TL("Cropped Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCropToData::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pGrids	= Parameters("INPUT")->asGridList();

	//-----------------------------------------------------
	if( pGrids->Get_Count() <= 0 )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	//-----------------------------------------------------
	bool	bCrop	= false;

	int		xMin, yMin, xMax, yMax;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bData	= false;

			for(int i=0; i<pGrids->Get_Count() && !bData; i++)
			{
				if( !pGrids->asGrid(i)->is_NoData(x, y) )
				{
					bData	= true;
				}
			}

			if( bData )
			{
				if( bCrop == false )
				{
					bCrop	= true;

					xMin	= xMax	= x;
					yMin	= yMax	= y;
				}
				else
				{
					if( xMin > x )
					{
						xMin	= x;
					}
					else if( xMax < x )
					{
						xMax	= x;
					}

					if( yMin > y )
					{
						yMin	= y;
					}
					else if( yMax < y )
					{
						yMax	= y;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( bCrop == false )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("nothing to crop"), _TL("no valid data found in grid(s)")));
	}
	else if( (1 + xMax - xMin) == Get_NX() && (1 + yMax - yMin) == Get_NY() )
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("nothing to crop"), _TL("valid data cells match original grid extent")));
	}
	else
	{
		CSG_Parameter_Grid_List	*pCropped	= Parameters("OUTPUT")->asGridList();

		pCropped->Del_Items();

		for(int i=0; i<pGrids->Get_Count(); i++)
		{
			CSG_Grid	*pGrid	= SG_Create_Grid(
				pGrids->asGrid(i)->Get_Type(),
				1 + xMax - xMin,
				1 + yMax - yMin,
				Get_Cellsize(),
				Get_XMin() + xMin * Get_Cellsize(),
				Get_YMin() + yMin * Get_Cellsize()
			);

			pGrid->Assign(pGrids->asGrid(i), GRID_INTERPOLATION_NearestNeighbour);
			pGrid->Set_Name(pGrids->asGrid(i)->Get_Name());

			pCropped->Add_Item(pGrid);
		}
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
