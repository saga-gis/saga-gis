
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
#include "Grid_Value_NoData.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Value_NoData::CGrid_Value_NoData(void)
{
	Set_Name		(_TL("Change a Grid's No-Data Value"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool allows changing a grid's no-data value or value range "
		"definition. It does not change the cell values of the grid. "
		"Its main purpose is to support this type of operation for tool "
		"chains and scripting environments. "
		"If the change value option is set all no-data cells will be "
		"changed to the new no-data value. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"OUTPUT", _TL("Changed Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"TYPE"	, _TL("Type"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("single value"),
			_TL("value range")
		), 0
	);

	Parameters.Add_Double("",
		"VALUE"	, _TL("No-Data Value"),
		_TL(""),
		-99999.
	);

	Parameters.Add_Range("",
		"RANGE"	, _TL("No-Data Value Range"),
		_TL(""),
		-99999., -99999.
	);

	Parameters.Add_Bool("",
		"CHANGE", _TL("Change Values"),
		_TL(""),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Value_NoData::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRID") && pParameter->asGrid() && has_GUI() )
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

		On_Parameters_Enable(pParameters, (*pParameters)("TYPE"));
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Value_NoData::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TYPE") )
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
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	if( Parameters("OUTPUT")->asGrid() && Parameters("OUTPUT")->asGrid() != pGrid )
	{
		Parameters("OUTPUT")->asGrid()->Create(*pGrid);

		pGrid	= Parameters("OUTPUT")->asGrid();

		DataObject_Set_Parameters(pGrid, Parameters("GRID")->asGrid());

		pGrid->Fmt_Name("%s [%s]", Parameters("GRID")->asGrid()->Get_Name(), _TL("Changed"));
	}

	//-----------------------------------------------------
	double	nodata_min	= Parameters("TYPE")->asInt() == 0
		? Parameters("VALUE"    )->asDouble()
		: Parameters("RANGE.MIN")->asDouble();

	double	nodata_max	= Parameters("TYPE")->asInt() == 0
		? Parameters("VALUE"    )->asDouble()
		: Parameters("RANGE.MAX")->asDouble();

	if( nodata_min == pGrid->Get_NoData_Value  ()
	&&  nodata_max == pGrid->Get_NoData_hiValue() )
	{
		Message_Fmt("\n%s\n%s", _TL("Nothing to do!"), _TL("Targeted no-data value (range) is already present."));

		return( true );
	}

	//-----------------------------------------------------
	if( Parameters("CHANGE")->asBool() )
	{
		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( pGrid->is_NoData(x, y) )
				{
					pGrid->Set_Value(x, y, nodata_min);
				}
			}
		}
	}

	//-----------------------------------------------------
	pGrid->Set_NoData_Value_Range(nodata_min, nodata_max);

	if( pGrid == Parameters("GRID")->asGrid() )
	{
		DataObject_Update(pGrid);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
