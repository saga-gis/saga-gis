/**********************************************************
 * Version $Id$
 *********************************************************/

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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description(
		_TL("Creates a color animation based on the values of selected grids.\n")
	);

	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL	, "NSTEPS"	, _TL("Interpolation Steps"),
		_TL(""),
		PARAMETER_TYPE_Int	, 0.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "RANGE"	, _TL("Color Stretch"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("fit to each grid"),
			_TL("fit to overall range"),
			_TL("fit to overall 1.5 standard deviation"),
			_TL("fit to overall 2.0 standard deviation")
		), 3
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
	int						iGrid, x, y, iStep, nSteps, Range;
	double					d, dStep, Value_A, Value_B, Range_Min, Range_Max;
	CSG_Grid				*pGrid, *pGrid_A, *pGrid_B;
	CSG_Parameter_Grid_List	*pGrids;

	pGrid	= Parameters("GRID")	->asGrid();
	pGrids	= Parameters("GRIDS")	->asGridList();
	Range	= Parameters("RANGE")	->asInt();

	if( pGrids->Get_Count() > 1 )
	{
		nSteps	= Parameters("NSTEPS")->asInt();
		dStep	= 1.0 / (1.0 + nSteps);
		nSteps	= (1 + nSteps) * (pGrids->Get_Count() - 1);

		pGrid_B	= pGrids->asGrid(0);
		pGrid->Assign(pGrid_B);

		switch( Range )
		{
		case 0:
			Range_Min	= pGrid->Get_ZMin();
			Range_Max	= pGrid->Get_ZMax();
			break;

		case 1:
			Range_Min	= pGrid->Get_ZMin();
			Range_Max	= pGrid->Get_ZMax();

			for(iGrid=1; iGrid<pGrids->Get_Count(); iGrid++)
			{
				if( Range_Min > pGrids->asGrid(iGrid)->Get_ZMin() )	Range_Min	= pGrids->asGrid(iGrid)->Get_ZMin();
				if( Range_Max < pGrids->asGrid(iGrid)->Get_ZMax() )	Range_Max	= pGrids->asGrid(iGrid)->Get_ZMax();
			}
			break;

		case 2:
			Range_Min	= pGrid->Get_ArithMean() - 1.5 * pGrid->Get_StdDev();
			Range_Max	= pGrid->Get_ArithMean() + 1.5 * pGrid->Get_StdDev();

			for(iGrid=1; iGrid<pGrids->Get_Count(); iGrid++)
			{
				double	Min	= pGrids->asGrid(iGrid)->Get_ArithMean() - 1.5 * pGrids->asGrid(iGrid)->Get_StdDev();
				double	Max	= pGrids->asGrid(iGrid)->Get_ArithMean() + 1.5 * pGrids->asGrid(iGrid)->Get_StdDev();

				if( Range_Min > Min )	Range_Min	= Min;
				if( Range_Max < Max )	Range_Max	= Max;
			}
			break;

		case 3:
			Range_Min	= pGrid->Get_ArithMean() - 2.0 * pGrid->Get_StdDev();
			Range_Max	= pGrid->Get_ArithMean() + 2.0 * pGrid->Get_StdDev();

			for(iGrid=1; iGrid<pGrids->Get_Count(); iGrid++)
			{
				double	Min	= pGrids->asGrid(iGrid)->Get_ArithMean() - 2.0 * pGrids->asGrid(iGrid)->Get_StdDev();
				double	Max	= pGrids->asGrid(iGrid)->Get_ArithMean() + 2.0 * pGrids->asGrid(iGrid)->Get_StdDev();

				if( Range_Min > Min )	Range_Min	= Min;
				if( Range_Max < Max )	Range_Max	= Max;
			}
			break;
		}

		DataObject_Update(pGrid, Range_Min, Range_Max, SG_UI_DATAOBJECT_SHOW);

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

				if( Range == 0 )
				{
					DataObject_Update(pGrid);
				}
				else
				{
					DataObject_Update(pGrid, Range_Min, Range_Max);
				}
			}

			pGrid->Assign(pGrid_B);

			if( Range == 0 )
			{
				DataObject_Update(pGrid);
			}
			else
			{
				DataObject_Update(pGrid, Range_Min, Range_Max);
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
