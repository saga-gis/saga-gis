
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
	Set_Name		(_TL("Georeferencing - Move Grid"));

	Set_Author		(_TL("(c) 2008 Conrad"));

	Set_Description	(_TW(
		""
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
	Set_Drag_Mode(MODULE_INTERACTIVE_DRAG_LINE);
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
bool CGeoref_Grid_Move::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	if( Mode == MODULE_INTERACTIVE_LDOWN )
	{
		m_Down	= ptWorld;
	}
	else if( Mode == MODULE_INTERACTIVE_LUP )
	{
		if( m_Down != ptWorld )
		{
			if( m_pSource == NULL )
			{
				m_pSource	= new CSG_Grid(*m_pGrid);

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

		if( m_Move.Get_X() != 0.0 || m_Move.Get_Y() != 0.0 )
		{
			m_pGrid	= new CSG_Grid(m_pSource->Get_Type(), m_pSource->Get_NX(), m_pSource->Get_NY(), m_pSource->Get_Cellsize(),
				m_pSource->Get_XMin() - m_Move.Get_X(),
				m_pSource->Get_YMin() - m_Move.Get_Y()
			);

			m_pGrid->Set_Name   (m_pSource->Get_Name());
			m_pGrid->Set_Unit   (m_pSource->Get_Unit());
			m_pGrid->Set_ZFactor(m_pSource->Get_ZFactor());

			for(int y=0; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++)
			{
				for(int x=0; x<m_pGrid->Get_NX(); x++)
				{
					m_pGrid->Set_Value(x, y, m_pSource->asDouble(x, y));
				}
			}

			Parameters("GRID")->Set_Value(m_pGrid);
		}

		delete(m_pSource);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
