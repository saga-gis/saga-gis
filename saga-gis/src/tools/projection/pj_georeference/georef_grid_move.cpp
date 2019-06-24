/**********************************************************
 * Version $Id: georef_grid_move.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   Grid_Georeference                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Georef_Grid_Move.cpp                 //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//    e-mail:     oconrad@geowiss.uni-hamburg.de         //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "georef_grid_move.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoref_Grid_Move::CGeoref_Grid_Move(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Move Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"This tool allows one to interactively move a grid to a new location. Once the tool is "
		"running, use the 'Action' tool to drag a line from a point you've located in the grid to "
		"the new location to which you like to move that point. This will shift the grid "
		"temporarily by this amount (vector). Once you are finished and stop the tool, you will "
		"get asked if you like to apply the transformation. If you apply it, a new and shifted "
		"output grid will be created.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_Grid(
		NULL	, "SOURCE"	, _TL("Source"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_LINE);
}

//---------------------------------------------------------
CGeoref_Grid_Move::~CGeoref_Grid_Move(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid_Move::On_Execute(void)
{
	m_pGrid		= Parameters("SOURCE")->asGrid();
	m_pSource	= NULL;
	m_bModified	= m_pGrid->is_Modified();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid_Move::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if( Mode == TOOL_INTERACTIVE_LDOWN )
	{
		m_Down	= ptWorld;
	}
	else if( Mode == TOOL_INTERACTIVE_LUP )
	{
		if( m_Down != ptWorld )
		{
			if( m_pSource == NULL )
			{
				m_pSource	= new CSG_Grid(*m_pGrid);
				m_pSource	->Set_Name(m_pGrid->Get_Name());

				m_Move		= m_Down - ptWorld;
			}
			else
			{
				m_Move	+= m_Down - ptWorld;
			}

			//---------------------------------------------
			int		x, y, ix, iy, dx, dy;

			dx		= (int)(0.5 + m_Move.Get_X() / m_pSource->Get_Cellsize());
			dy		= (int)(0.5 + m_Move.Get_Y() / m_pSource->Get_Cellsize());

			for(y=0, iy=dy; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++, iy++)
			{
				if( iy >= 0 && iy < m_pSource->Get_NY() )
				{
					for(x=0, ix=dx; x<m_pGrid->Get_NX(); x++, ix++)
					{
						if( ix >= 0 && ix < m_pSource->Get_NX() )
						{
							m_pGrid->Set_Value(x, y, m_pSource->asDouble(ix, iy));
						}
						else
						{
							m_pGrid->Set_NoData(x, y);
						}
					}
				}
				else
				{
					for(x=0; x<m_pGrid->Get_NX(); x++)
					{
						m_pGrid->Set_NoData(x, y);
					}
				}
			}

			DataObject_Update(m_pGrid);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGeoref_Grid_Move::On_Execute_Finish(void)
{
	if( m_pSource )
	{
		m_pGrid->Assign(m_pSource);
		m_pGrid->Set_Modified(m_bModified);
		DataObject_Update(m_pGrid);

		if( m_Move.Get_X() == 0.0 && m_Move.Get_Y() == 0.0 )
		{
			Message_Add(_TL("No translation set by user"));
		}
		else if( Message_Dlg_Confirm(_TL("Apply Move"), _TL("Move Grid")) )
		{
			m_pGrid	= new CSG_Grid(m_pSource->Get_Type(), m_pSource->Get_NX(), m_pSource->Get_NY(), m_pSource->Get_Cellsize(),
				m_pSource->Get_XMin() - m_Move.Get_X(),
				m_pSource->Get_YMin() - m_Move.Get_Y()
			);

			m_pGrid->Set_Name   (m_pSource->Get_Name());
			m_pGrid->Set_Unit   (m_pSource->Get_Unit());
			m_pGrid->Set_Scaling(m_pSource->Get_Scaling(), m_pSource->Get_Offset());

			for(int y=0; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++)
			{
				for(int x=0; x<m_pGrid->Get_NX(); x++)
				{
					m_pGrid->Set_Value(x, y, m_pSource->asDouble(x, y));
				}
			}

			Parameters("GRID")->Set_Value(m_pGrid);

			return( true );
		}

		delete(m_pSource);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
