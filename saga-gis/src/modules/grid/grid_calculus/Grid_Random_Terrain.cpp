/**********************************************************
 * Version $Id$
 *********************************************************/
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

#include "Grid_Random_Terrain.h"

CGrid_Random_Terrain::CGrid_Random_Terrain(void)
{	 
	CSG_Parameters	*pParameters;
	
	Set_Name(_TL("Random Terrain Generation"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description	(_TW(
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
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),_TL("User defined"), _TL("Grid Project"), _TL("Grid")), 
		0);

	Parameters.Add_Grid_List(
		NULL, 
		"TARGET_GRID", 
		_TL("Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, 
		false);
	
	pParameters	= Add_Parameters("USER", _TL("User defined grid"), _TL(""));

	pParameters->Add_Value(
		NULL, 
		"CELL_SIZE",
		_TL("Grid Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 
		100.0, 
		0.0, 
		true);

	pParameters->Add_Value(
		NULL, 
		"COLS",
		_TL("Cols"),
		_TL(""),
		PARAMETER_TYPE_Int, 
		100, 
		0.0, 
		true);		

	pParameters->Add_Value(
		NULL, 
		"ROWS", 
		_TL("Rows"),
		_TL(""),
		PARAMETER_TYPE_Int, 
		100, 
		0.0, 
		true);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("GET_SYSTEM"	, _TL("Choose Grid Project"), _TL(""));

	pParameters->Add_Grid_System(
		NULL, "SYSTEM"		, _TL("System")		, _TL("")
	);


	pParameters	= Add_Parameters("GRID", _TL("Choose Grid"), _TL(""));

	pParameters->Add_Grid(NULL, 
		"GRID", 
		_TL("Grid"),
		_TL(""),
		PARAMETER_INPUT	, false
	);
}

CGrid_Random_Terrain::~CGrid_Random_Terrain(void)
{}


CSG_Grid * CGrid_Random_Terrain::Get_Target_Grid(CSG_Parameters *pParameters )
{

	double iCellSize;
	int iRows,iCols;

	iCellSize	= pParameters->Get_Parameter("CELL_SIZE")->asDouble();
	iRows = pParameters->Get_Parameter("ROWS")->asInt();
	iCols = pParameters->Get_Parameter("COLS")->asInt();
	
	return( SG_Create_Grid(SG_DATATYPE_Float, iCols, iRows, iCellSize, 0, 0) );
}

bool CGrid_Random_Terrain::On_Execute(void)
{
	
	Parameters("TARGET_GRID")->asGridList()->Del_Items();

	pGrid		= NULL;

	switch( Parameters("TARGET_TYPE")->asInt() )
	{
	case 0:	// User defined...
		if( Dlg_Parameters("USER") )
		{
			pGrid	= Get_Target_Grid(Get_Parameters("USER"));
		}
		break;

	case 1:	// Grid Project...
		if( Dlg_Parameters("GET_SYSTEM") )
		{
			pGrid	= SG_Create_Grid(
						*Get_Parameters("GET_SYSTEM")->Get_Parameter("SYSTEM")->asGrid_System()
					);
		}
		break;

	case 2:	// Grid...
		if( Dlg_Parameters("GRID") )
		{
			pGrid	= Get_Parameters("GRID")->Get_Parameter("GRID")->asGrid();
		}
		break;
	}//switch

	//-------------------------------------------------
	if( pGrid )
	{
		Parameters("TARGET_GRID")->asGridList()->Add_Item(pGrid);

		pGrid->Set_Name(_TL("DEM"));
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


void CGrid_Random_Terrain::addBump(CSG_Grid* pGrid, int iRadius){

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
				dDist= M_GET_LENGTH(x-x2, y-y2);
				if (dDist<=iRadius){
					dOffset = (iRadius * iRadius) - (dDist * dDist);
					dValue = pGrid->asDouble(x2,y2);
					pGrid->Set_Value(x2,y2,dValue+dOffset);	
				}//else
			}//if
		}//for
	}//for


}//method
