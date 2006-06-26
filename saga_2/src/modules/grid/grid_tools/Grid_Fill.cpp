
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Fill.cpp                         //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Andre Ringeler                   //
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
//    e-mail:     aringel@saga-gis.org                   //
//                                                       //
//    contact:    Andre Ringeler                         //
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
#include "Grid_Fill.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Fill::CGrid_Fill(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Change Grid Values - Flood Fill"));

	Set_Author(_TL("Copyrights (c) 2005 by Andre Ringeler"));

	Set_Description(_TL(
		"Interactively use the flood fill method to replace a grid's cell values. "
		"If the target is not set, the changes will be stored to the original grid. ")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL	, "GRID_IN"		, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "GRID_OUT"	, _TL("Changed Grid"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "FILL"	, _TL("Fill Value"),
		"",
		PARAMETER_TYPE_Double, 1
	); 

	Parameters.Add_Value(
		NULL, "UPPER"	, _TL("Upper Border"),
		"",
		PARAMETER_TYPE_Double, 1000
	); 
	
	Parameters.Add_Value(
		NULL, "LOWER"	, _TL("Lower Border"),
		"",
		PARAMETER_TYPE_Double, 0
	);

	//-----------------------------------------------------
	
}

//---------------------------------------------------------
CGrid_Fill::~CGrid_Fill(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Fill::Push(int x, int y)
{
	Stack[StackPos].x = x;
	Stack[StackPos].y = y;

	StackPos++;
}

void CGrid_Fill::Pop(int &x, int &y)
{
	StackPos--;

	x = Stack[StackPos].x;
	y = Stack[StackPos].y;
}

bool CGrid_Fill::On_Execute(void)
{
	
	if( Parameters("GRID_OUT")->asGrid() == NULL || Parameters("GRID_IN")->asGrid() == Parameters("GRID_OUT")->asGrid() )
	{
		m_pGrid	= Parameters("GRID_IN")	->asGrid();
	}
	else
	{
		m_pGrid	= Parameters("GRID_OUT")->asGrid();
		m_pGrid->Assign(Parameters("GRID_IN")->asGrid());
	}

	m_Fill_Value = Parameters("FILL")->asDouble();

	m_Upper_Border = Parameters("UPPER")->asDouble();
	m_Lower_Border = Parameters("LOWER")->asDouble();

	//-------------------------------------------------
	StackPos = 0;
	StackSize = m_pGrid->Get_NX() * m_pGrid->Get_NY() *4;
	Stack = (INT_Point*) new INT_Point [ StackSize ] ;

	//-------------------------------------------------
	

	return( true );
	
}

bool CGrid_Fill::On_Execute_Position(CGEO_Point ptWorld, TModule_Interactive_Mode Mode)
{
	CGrid_System	*System;
	int				x, y, neu_x, neu_y; 
	int				i;
	
	System	=	Get_System(); 
	
	switch( Mode )
	{
	//-----------------------------------------------------
	case MODULE_INTERACTIVE_LDOWN:
	{
		x = System->Get_xWorld_to_Grid( ptWorld.Get_X() );
		y = System->Get_yWorld_to_Grid( ptWorld.Get_Y() );

		Push( x,  y);

		while (StackPos && Set_Progress(StackPos, StackSize))
		{
			Pop (x , y);

			for (	i=0;	i < 4;	i++)
			{
				neu_x = Get_xTo( i*2 , x);
				neu_y = Get_yTo( i*2 , y);

				if (   m_pGrid->is_InGrid( neu_x , neu_y) 
					&& (m_pGrid->asDouble( neu_x , neu_y)!= m_Fill_Value)
					&& (m_pGrid->asDouble( neu_x , neu_y) < m_Upper_Border)
					&& (m_pGrid->asDouble( neu_x , neu_y) > m_Lower_Border) )
				{
					Push (neu_x , neu_y);
					m_pGrid->Set_Value(neu_x , neu_y , m_Fill_Value );
				}
			}
		}
	}

	DataObject_Update(m_pGrid, m_pGrid->Get_ZMin(), m_pGrid->Get_ZMax());
	return( true );
	
	}

	return( false );
}
