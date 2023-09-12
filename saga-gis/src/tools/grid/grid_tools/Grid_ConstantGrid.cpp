/*******************************************************************************
    ConstantGrid.cpp
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

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_ConstantGrid.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CConstantGrid::CConstantGrid(void)
{
	Set_Name		(_TL("Constant Grid"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"Constant grid creation."
	));

	Parameters.Add_String("",
		"NAME"	, _TL("Name"),
		_TL(""),
		_TL("Constant Grid")
	);

	Parameters.Add_Double("",
		"CONST"	, _TL("Constant Value"),
		_TL(""),
		1.
	);

	Parameters.Add_Data_Type("",
		"TYPE"	, _TL("Data Type"),
		_TL(""),
		SG_DATATYPES_Numeric|SG_DATATYPES_Bit
	);

	m_Grid_Target.Create(&Parameters);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CConstantGrid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CConstantGrid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConstantGrid::On_Execute(void)
{
	CSG_Grid *pGrid = m_Grid_Target.Get_Grid(Parameters("TYPE")->asDataType()->Get_Data_Type());

	if( pGrid )
	{
		pGrid->Set_Name(Parameters("NAME" )->asString());
		pGrid->Assign  (Parameters("CONST")->asDouble());

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
