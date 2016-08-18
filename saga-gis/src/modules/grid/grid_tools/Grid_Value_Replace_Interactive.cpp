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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Value_Replace_Interactive.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Value_Replace_Interactive::CGrid_Value_Replace_Interactive(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Change Cell Values"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description	(_TW(
		"The tool allows one to interactively change cell values of the input grid. "
		"Once the tool is executed and running, you can use the Action tool to select "
		"grid cells. While working on a grid, you can change (and apply) the 'New Value' "
		"and the 'Method' parameters without stopping and re-starting the tool.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"), 
		_TL("The grid to modify."), 
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL	, "VALUE"	, _TL("Value"), 
		_TL("The value to apply."), 
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Method"),
		_TL("Choose how to apply the new value."),
		CSG_String::Format("%s|%s|%s|",
			_TL("set"),
			_TL("add"),
			_TL("subtract")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"	, _TL("Radius"), 
		_TL("Change all values within radius."), 
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	//-----------------------------------------------------
	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_NONE);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Value_Replace_Interactive::On_Execute(void)
{
	m_pGrid		= Parameters("GRID"  )->asGrid  ();
	m_Value		= Parameters("VALUE" )->asDouble();
	m_Method	= Parameters("METHOD")->asInt   ();

	m_Kernel.Set_Radius(Parameters("RADIUS")->asDouble());

	return( true );
}

//---------------------------------------------------------
bool CGrid_Value_Replace_Interactive::On_Execute_Finish(void)
{
	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Value_Replace_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
	//-----------------------------------------------------
	case TOOL_INTERACTIVE_LDOWN:
	case TOOL_INTERACTIVE_MOVE_LDOWN:
		{
			TSG_Point_Int	Point;

			if( !Get_Grid_Pos(Point.x, Point.y) )
			{
				return( false );
			}

			if( Mode == TOOL_INTERACTIVE_LDOWN )
			{
				m_Last	= Point;
			}
			else if( m_Last.x == Point.x && m_Last.y == Point.y )	// don't do it twice for the same point
			{
				return( false );
			}

			if( m_Kernel.Get_Count() <= 1 )
			{
				Set_Value(Point.x, Point.y);
			}
			else
			{
				for(int i=0; i<m_Kernel.Get_Count(); i++)
				{
					Set_Value(m_Kernel.Get_X(i, Point.x), m_Kernel.Get_Y(i, Point.y));
				}
			}
		}
		return( true );

	case TOOL_INTERACTIVE_LUP:
		{
			DataObject_Update(m_pGrid, SG_UI_DATAOBJECT_UPDATE_ONLY);
		}
		return( true );

	//-----------------------------------------------------
	default:
		return( true );
	}
}

//---------------------------------------------------------
inline void CGrid_Value_Replace_Interactive::Set_Value(int x, int y)
{
	if( is_InGrid(x, y) )
	{
		switch( m_Method )
		{
		default:	m_pGrid->Set_Value(x, y,  m_Value);	break;
		case  1:	m_pGrid->Add_Value(x, y,  m_Value);	break;
		case  2:	m_pGrid->Add_Value(x, y, -m_Value);	break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
