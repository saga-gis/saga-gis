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

CGrid_Value_Replace_Interactive::CGrid_Value_Replace_Interactive(void){

	Set_Name(_TL("Change Cell Values"));
	Set_Author(_TL("Copyrights (c) 2004 by Victor Olaya"));
	Set_Description(_TL("(c) 2004 by Victor Olaya."));

	Parameters.Add_Grid(NULL, 
						"GRID", 
						_TL("Grid"), 
						"", 
						PARAMETER_INPUT);

	Parameters.Add_Value(NULL, 
						"NEWVALUE", 
						_TL("New Value"), 
						_TL("New Value"), 
						PARAMETER_TYPE_Double, 
						0);

}//constructor


CGrid_Value_Replace_Interactive::~CGrid_Value_Replace_Interactive(void){
	On_Execute_Finish();
}


bool CGrid_Value_Replace_Interactive::On_Execute(void){
	
	
	m_pGrid = Parameters("GRID")->asGrid(); 
	m_dNewValue = Parameters("NEWVALUE")->asDouble();

	return true;

}//method

bool CGrid_Value_Replace_Interactive::On_Execute_Finish(void)
{
	return( true );
}

bool CGrid_Value_Replace_Interactive::On_Execute_Position(CSG_Point ptWorld, TModule_Interactive_Mode Mode)
{	
	int iX, iY;		

	//-----------------------------------------------------
	if(	Mode != MODULE_INTERACTIVE_LDOWN || !Get_Grid_Pos(iX, iY) )
	{
		return( false );
	}

	double dValue = m_pGrid->asDouble(iX,iY);

	m_pGrid->Set_Value(iX,iY,m_dNewValue);

	DataObject_Update(m_pGrid, true);

	return (true);

}//method
