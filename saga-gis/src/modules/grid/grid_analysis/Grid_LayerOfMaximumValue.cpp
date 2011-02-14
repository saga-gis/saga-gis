/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    CLayerOfMaximumValue.cpp
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

#define MAXIMUM 0
#define MINIMUM 1

#include "Grid_LayerOfMaximumValue.h"

CLayerOfMaximumValue::CLayerOfMaximumValue(void)
{
	Parameters.Set_Name(_TL("Layer of extreme value"));

	Parameters.Set_Description(_TW(
		"It creates a new grid containing the ID of the grid with the maximum (minimum) value.\r\n"
		"Copyright 2005 Victor Olaya: e-mail: volaya@ya.com")		
	);

	Parameters.Add_Grid_List(NULL, 
							"GRIDS", 
							_TL("Grids"),
							_TL(""),
							PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"RESULT", 
						_TL("Result"),
						_TL(""),
						PARAMETER_OUTPUT);

	Parameters.Add_Choice(NULL, 
						"CRITERIA", 
						_TL("Method"), 
						_TL(""), 
						CSG_String::Format(SG_T("%s|%s|"),_TL("Maximum"), _TL("Minimum")),						
						0);
}

//---------------------------------------------------------
CLayerOfMaximumValue::~CLayerOfMaximumValue(void)
{}

bool CLayerOfMaximumValue::On_Execute(void)
{
	int				iGrid, nGrids, x, y;
	double			dExtremeValue, dValTemp;
	int				iExtremeGrid;
	int				iCriteria;
	CSG_Grid			*pResult, *pGrid;
	CSG_Parameter_Grid_List	*pParm_Grids;
	bool bMeetsCriteria;

	pParm_Grids	= Parameters("GRIDS")	->asGridList();
	pResult		= Parameters("RESULT")		->asGrid();
	nGrids		= pParm_Grids			->Get_Count();
	iCriteria = Parameters("CRITERIA")->asInt();	

	//-----------------------------------------------------
	if( nGrids > 1 )
	{
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				for(iGrid=0; iGrid<nGrids; iGrid++){
					pGrid = pParm_Grids->asGrid(iGrid);
					if (!pGrid->is_NoData(x,y)){
						dExtremeValue = pGrid->asDouble(x, y);						
						break;
					}//if
				}//for

				iExtremeGrid = 0;

				for(iGrid=0; iGrid<nGrids; iGrid++){

					pGrid = pParm_Grids->asGrid(iGrid);

					if (!pGrid->is_NoData(x,y)){ 

						dValTemp	= pGrid->asDouble(x, y);

						switch (iCriteria){
						case MINIMUM:
							bMeetsCriteria = (dExtremeValue >= dValTemp);
							break;
						case MAXIMUM:
							bMeetsCriteria = (dExtremeValue <= dValTemp);
							break;
						default:
							break;
						}//switch
						
						if  (bMeetsCriteria)
						{
							dExtremeValue = dValTemp; 
							iExtremeGrid = iGrid + 1;
						}
					}
				}

				if (iExtremeGrid){
					pResult->Set_Value(x, y, iExtremeGrid); 
				}//if
				else{
					pResult->Set_NoData(x,y);
				}//else
			}
		}
	}
	else if( nGrids > 0 )
	{
		pResult->Assign(1);
	}

	//-----------------------------------------------------
	return( nGrids > 0 );
}
