
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Color_Blend.cpp                  //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Grid_Color_Blend.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Color_Blend::CGrid_Color_Blend(void)
{
	Set_Name(_TL("Color Blending"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description(
		_TL("Creates a color animation based on the values of selected grids.\n")
	);

	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL	, "NSTEPS"	, _TL("Interpolation Steps"),
		"",
		PARAMETER_TYPE_Int	, 0.0, 0.0, true
	);
}

//---------------------------------------------------------
CGrid_Color_Blend::~CGrid_Color_Blend(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Color_Blend::On_Execute(void)
{
	int						iGrid, x, y, iStep, nSteps;
	double					d, dStep, Value_A, Value_B;
	CSG_Grid					*pGrid, *pGrid_A, *pGrid_B;
	CSG_Parameter_Grid_List	*pGrids;

	pGrid	= Parameters("GRID")	->asGrid();
	pGrids	= Parameters("GRIDS")	->asGridList();

	if( pGrids->Get_Count() > 1 )
	{
		nSteps	= Parameters("NSTEPS")->asInt();
		dStep	= 1.0 / (1.0 + nSteps);
		nSteps	= (1 + nSteps) * (pGrids->Get_Count() - 1);

		pGrid_B	= pGrids->asGrid(0);

		if( pGrid->Get_ZRange() > 0.0 )
		{
			pGrid->Assign(pGrid_B);
			DataObject_Update(pGrid, true);
		}
		else
		{
			pGrid->Assign(pGrid_B);
			DataObject_Update(pGrid, pGrid_B->Get_ZMin(), pGrid_B->Get_ZMax(), true);
		}

		for(iGrid=1, iStep=1; iGrid<pGrids->Get_Count() && Set_Progress(iStep, nSteps); iGrid++, iStep++)
		{
			pGrid_A	= pGrid_B;
			pGrid_B	= pGrids->asGrid(iGrid);

			for(d=dStep; d<1.0 && Set_Progress(iStep, nSteps); d+=dStep, iStep++)
			{
				for(y=0; y<Get_NY(); y++)
				{
					for(x=0; x<Get_NX(); x++)
					{
						Value_A	= pGrid_A->asDouble(x, y);
						Value_B	= pGrid_B->asDouble(x, y);

						pGrid->Set_Value(x, y, Value_A + d * (Value_B - Value_A) );
					}
				}

				DataObject_Update(pGrid, true);
			}

			pGrid->Assign(pGrid_B);
			DataObject_Update(pGrid, true);
		}

		return( true );
	}

	return( false );
}
