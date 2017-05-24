
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  grid_collection.cpp                  //
//                                                       //
//                 Copyright (C) 2017 by                 //
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
//    e-mail:     oconrad@saga-gis.org                   //
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
#include "grid_collection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Create::CGrids_Create(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Create a Grid Collection"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"LIST"		, _TL("Single Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grids("",
		"GRIDS"		, _TL("Grid Collection"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrids_Create::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Create::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pList	= Parameters("LIST")->asGridList();

	CSG_Grids	*pGrids	= Parameters("GRIDS")->asGrids();

	pGrids->Del_Grids();

	//-----------------------------------------------------
	for(int i=0; i<pList->Get_Grid_Count() && Set_Progress(i, pList->Get_Grid_Count()); i++)
	{
		pGrids->Add_Grid(i, pList->Get_Grid(i));
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
CGrids_Extract::CGrids_Extract(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Extract Grids from a Grid Collection"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"LIST"		, _TL("Single Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grids("",
		"GRIDS"		, _TL("Grid Collection"),
		_TL(""),
		PARAMETER_INPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrids_Extract::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Extract::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pList	= Parameters("LIST")->asGridList();

	CSG_Grids	*pGrids	= Parameters("GRIDS")->asGrids();

	pList->Del_Items();

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Grid_Count() && Set_Progress(i, pGrids->Get_Grid_Count()); i++)
	{
		CSG_Grid	*pGrid	= SG_Create_Grid(pGrids->Get_Grid(i));

		pList->Add_Item(pGrid);
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
