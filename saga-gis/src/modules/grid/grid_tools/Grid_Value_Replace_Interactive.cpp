/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Grid_Value_Replace_Interactive.cpp
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

#include "Grid_Value_Replace_Interactive.h"

CGrid_Value_Replace_Interactive::CGrid_Value_Replace_Interactive(void)
{

	Set_Name		(_TL("Change Cell Values"));
	Set_Author		(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description	(_TW("The module allows to interactively change cell values of the input grid. "
						"Once the module is executed and running, you can use the Action tool to select "
						"grid cells. While working on a grid, you can change (and apply) the 'New Value' "
						"and the 'Method' parameters without stopping and re-starting the module.\n\n"
						));

	Parameters.Add_Grid(
		NULL,	"GRID",	_TL("Grid"), 
		_TL("The grid to modify."), 
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL,	"NEWVALUE",	_TL("New Value"), 
		_TL("The value to apply."), 
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Method"),
		_TL("Choose how to apply the new value."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("set constant value"),
			_TL("add value"),
			_TL("subtract value")
		), 0
	);

}//constructor


CGrid_Value_Replace_Interactive::~CGrid_Value_Replace_Interactive(void)
{
	On_Execute_Finish();
}


bool CGrid_Value_Replace_Interactive::On_Execute(void)
{
	m_pGrid		= Parameters("GRID")->asGrid();

	return( true );
}//method


bool CGrid_Value_Replace_Interactive::On_Execute_Finish(void)
{
	return( true );
}

bool CGrid_Value_Replace_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{	
	int iX, iY;		

	//-----------------------------------------------------
	if(	Mode != MODULE_INTERACTIVE_LDOWN || !Get_Grid_Pos(iX, iY) )
	{
		return( false );
	}

	double	dNewValue	= Parameters("NEWVALUE")->asDouble();
	int		iMethod		= Parameters("METHOD")->asInt();

	double	dValue		= m_pGrid->asDouble(iX, iY);

	switch( iMethod )
	{
	default:
	case 0:				m_pGrid->Set_Value(iX, iY, dNewValue);				break;
	case 1:				m_pGrid->Set_Value(iX, iY, dValue + dNewValue);		break;
	case 2:				m_pGrid->Set_Value(iX, iY, dValue - dNewValue);		break;
	}

	DataObject_Update(m_pGrid, SG_UI_DATAOBJECT_UPDATE_ONLY);

	return( true );
}//method
