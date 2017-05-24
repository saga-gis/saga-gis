/**********************************************************
 * Version $Id: Grid_Value_NoData.cpp 2871 2016-03-30 11:32:35Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Value_NoData.cpp                //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "Grid_Value_NoData.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Value_NoData::CGrid_Value_NoData(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Change a Grid's No-Data Value"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool allows changing a grid's no-data value or value range "
		"definition. It does not change the cell values of the grid. "
		"Its main purpose is to support this type of operation for tool "
		"chains and scripting environments."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"	, _TL("Type"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("single value"),
			_TL("value range")
		), 0
	);

	Parameters.Add_Double(
		NULL	, "VALUE"	, _TL("No-Data Value"),
		_TL(""),
		-99999.
	);

	Parameters.Add_Range(
		NULL	, "RANGE"	, _TL("No-Data Value Range"),
		_TL(""),
		-99999., -99999.
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Value_NoData::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "GRID") && pParameter->asGrid() )
	{
		CSG_Grid	*pGrid	= pParameter->asGrid();

		pParameters->Set_Parameter("VALUE",
			pGrid->Get_NoData_Value()
		);

		pParameters->Get_Parameter("RANGE")->asRange()->Set_Range(
			pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue()
		);

		pParameters->Set_Parameter("TYPE",
			pGrid->Get_NoData_Value() < pGrid->Get_NoData_hiValue() ? 1 : 0
		);

		On_Parameters_Enable(pParameters, pParameters->Get_Parameter("TYPE"));
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Value_NoData::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TYPE") )
	{
		pParameters->Set_Enabled("VALUE", pParameter->asInt() == 0);
		pParameters->Set_Enabled("RANGE", pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Value_NoData::On_Execute(void)
{
	//-----------------------------------------------------
	bool	bUpdate;

	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	if( Parameters("TYPE")->asInt() == 0 )
	{
		bUpdate	= pGrid->Set_NoData_Value(
			Parameters("VALUE")->asDouble()
		);
	}
	else
	{
		bUpdate	= pGrid->Set_NoData_Value_Range(
			Parameters("RANGE")->asRange()->Get_LoVal(),
			Parameters("RANGE")->asRange()->Get_HiVal()
		);
	}

	if( bUpdate )
	{
		DataObject_Update(pGrid);
	}

	//-------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
