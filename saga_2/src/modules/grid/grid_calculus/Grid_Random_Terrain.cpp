/*******************************************************************************
    Grid_Random_Terrain.cpp
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

#include "Grid_Random_Terrain.h"

CGrid_Random_Terrain::CGrid_Random_Terrain(void)
{	 
	CParameters	*pParameters;
	
	Set_Name(_TL("Random Terrain Generation"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description(_TL(
		"(c) 2004 by Victor Olaya. Terrain Generation"));

	Parameters.Add_Value(NULL, 
						"RADIUS", 
						_TL("Radius (cells)"), 
						_TL("Radius (cells)"), 
						PARAMETER_TYPE_Int, 
						15);
	
	Parameters.Add_Value(NULL, 
						"ITERATIONS", 
						_TL("Iterations"), 
						_TL("Iterations"), 
						PARAMETER_TYPE_Int, 
						10);
	
	Parameters.Add_Choice(
		NULL, 
		"TARGET_TYPE", 
		_TL("Target Dimensions"),
		"",
		CAPI_String::Format("%s|%s|%s|",_TL("User defined"), _TL("Grid Project"), _TL("Grid")), 
		0);

	Parameters.Add_Grid_List(
		NULL, 
		"TARGET_GRID", 
		_TL("Grid"),
		"",
		PARAMETER_OUTPUT_OPTIONAL, 
		false);
	
	pParameters	= Add_Extra_Parameters("USER", _TL("User defined grid"), "");

	pParameters->Add_Value(
		NULL, 
		"CELL_SIZE",
		_TL("Grid Size"),
		"",
		PARAMETER_TYPE_Double, 
		100.0, 
		0.0, 
		true);

	pParameters->Add_Value(
		NULL, 
		"COLS",
		_TL("Cols"),
		"",
		PARAMETER_TYPE_Int, 
		100, 
		0.0, 
		true);		

	pParameters->Add_Value(
		NULL, 
		"ROWS", 
		_TL("Rows"),
		"",
		PARAMETER_TYPE_Int, 
		100, 
		0.0, 
		true);

	pParameters	= Add_Extra_Parameters("GRID", _TL("Choose Grid"), "");

	pParameters->Add_Grid(NULL, 
		"GRID", 
		_TL("Grid"),
		"",
		PARAMETER_INPUT	, false
	);
}

CGrid_Random_Terrain::~CGrid_Random_Terrain(void)
{}


CGrid * CGrid_Random_Terrain::Get_Target_Grid(CParameters *pParameters )
{

	double iCellSize;
	int iRows,iCols;

	iCellSize	= pParameters->Get_Parameter("CELL_SIZE")->asDouble();
	iRows = pParameters->Get_Parameter("ROWS")->asInt();
	iCols = pParameters->Get_Parameter("COLS")->asInt();
	
	return( API_Create_Grid(GRID_TYPE_Float, iCols, iRows, iCellSize, 0, 0) );
}

bool CGrid_Random_Terrain::On_Execute(void)
{
	
	Parameters("TARGET_GRID")->asGridList()->Del_Items();

	pGrid		= NULL;

	switch( Parameters("TARGET_TYPE")->asInt() )
	{
	case 0:	// User defined...
		if( Dlg_Extra_Parameters("USER") )
		{
			pGrid	= Get_Target_Grid(Get_Extra_Parameters("USER"));
		}
		break;

	case 1:	// Grid Project...
		if( Dlg_Extra_Parameters("GRID") )
		{
			pGrid	= API_Create_Grid(Get_Extra_Parameters("GRID")->Get_Parameter("GRID")->asGrid());
		}
		break;

	case 2:	// Grid...
		if( Dlg_Extra_Parameters("GRID") )
		{
			pGrid	= Get_Extra_Parameters("GRID")->Get_Parameter("GRID")->asGrid();
		}
		break;
	}//switch

	//-------------------------------------------------
	if( pGrid )
	{
		Parameters("TARGET_GRID")->asGridList()->Add_Item(pGrid);

		pGrid->Set_Name("DEM");
		pGrid->Assign(0.0);

		int iIterations = Parameters("ITERATIONS")->asInt();
		int iRadius = Parameters("RADIUS")->asInt();

		for	(int i=0 ; i<iIterations && Set_Progress(i,iIterations) ; i++){
			addBump(pGrid, iRadius);
		}//for

		return( true );
	}//if

	return( false );

}//method


void CGrid_Random_Terrain::addBump(CGrid* pGrid, int iRadius){

	double dOffset;
	int x,y,x2,y2;
	double dDist;
	double dValue;

	x = rand() % pGrid->Get_NX();
	y = rand() % pGrid->Get_NY();

	for	(int i=-iRadius ; i<iRadius ; i++){
		for (int j=-iRadius ; j<iRadius ; j++){
			x2 = x+i;
			y2 = y+j;
			if (x2>0 && y2>0 && x2<pGrid->Get_NX() && y2 < pGrid->Get_NY()){
				dDist= M_GET_DIST(x-x2, y-y2);
				if (dDist<=iRadius){
					dOffset = (iRadius * iRadius) - (dDist * dDist);
					dValue = pGrid->asDouble(x2,y2);
					pGrid->Set_Value(x2,y2,dValue+dOffset);	
				}//else
			}//if
		}//for
	}//for


}//method
