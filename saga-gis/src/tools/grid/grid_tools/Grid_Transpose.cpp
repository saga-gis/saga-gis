/**********************************************************
 * Version $Id: Grid_Transpose.cpp 1834 2013-09-11 18:33:29Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      grid_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Transpose.cpp                   //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include "Grid_Transpose.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Transpose::CGrid_Transpose(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Transpose Grids"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "TRANSPOSED"	, _TL("Transposed Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "MIRROR_X"	, _TL("Mirror Horizontally"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "MIRROR_Y"	, _TL("Mirror Vertically"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Transpose::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	bool	bMirror_X	= Parameters("MIRROR_X")->asBool();
	bool	bMirror_Y	= Parameters("MIRROR_Y")->asBool();

	//-----------------------------------------------------
	CSG_Grid_System	System;

	if( !System.Assign(Get_Cellsize(), Get_YMin(), Get_XMin(), Get_NY(), Get_NX()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	Parameters("TRANSPOSED")->asGridList()->Del_Items();

	for(int i=0; i<pGrids->Get_Grid_Count() && Process_Get_Okay(); i++)
	{
		CSG_Grid	*pGrid	= pGrids->Get_Grid(i);
		CSG_Grid	*pTransposed	= SG_Create_Grid(System, pGrid->Get_Type());

		pTransposed->Set_Name(pGrid->Get_Name());
		pTransposed->Set_Unit(pGrid->Get_Unit());
		pTransposed->Set_Scaling(pGrid->Get_Scaling(), pGrid->Get_Offset());
		pTransposed->Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_Value(true));
		pTransposed->Get_MetaData  ()	= pGrid->Get_MetaData  ();
		pTransposed->Get_Projection()	= pGrid->Get_Projection();

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			int	iy	= bMirror_Y ? Get_NY() - 1 - y : y;

			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				int	ix	= bMirror_X ? Get_NX() - 1 - x : x;

				pTransposed->Set_Value(iy, ix, pGrid->asDouble(x, y));
			}
		}

		Parameters("TRANSPOSED")->asGridList()->Add_Item(pTransposed);
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
