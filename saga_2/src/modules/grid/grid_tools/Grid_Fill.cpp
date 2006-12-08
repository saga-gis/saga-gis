
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
//                    Grid_Fill.cpp                      //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                    Andre Ringeler                     //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
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

	Set_Name		(_TL("Change Grid Values - Flood Fill"));

	Set_Author		(_TL("(c) 2005 by A.Ringeler, (c) 2006 by O.Conrad"));

	Set_Description	(_TW(
		"Interactively use the flood fill method to replace a grid's cell values. "
		"If the target is not set, the changes will be stored to the original grid. ")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL	, "GRID_IN"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "GRID_OUT"	, _TL("Changed Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "FILL"		, _TL("Fill Value"),
		_TL(""),
		PARAMETER_TYPE_Double, 1
	); 

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Value to be replaced"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("value at mouse position"),
			_TL("fixed value"),
			_TL("tolerance as absolute values")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "ZFIXED"		, _TL("Fixed value to be replaced"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	); 

	Parameters.Add_Value(
		NULL	, "DZMAX"		, _TL("Upper Tolerance"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	); 

	Parameters.Add_Value(
		NULL	, "DZMIN"		, _TL("Lower Tolerance"),
		_TL(""),
		PARAMETER_TYPE_Double, -1.0
	);
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

	m_Method			= Parameters("METHOD")	->asInt();
	m_zFill				= Parameters("FILL")	->asDouble();
	m_zFixed			= Parameters("ZFIXED")	->asDouble();
	m_zTolerance_Min	= Parameters("DZMIN")	->asDouble();
	m_zTolerance_Max	= Parameters("DZMAX")	->asDouble();

	if( m_zTolerance_Min > m_zTolerance_Max )
	{
		double	z			= m_zTolerance_Min;
		m_zTolerance_Min	= m_zTolerance_Min;
		m_zTolerance_Max	= z;
	}

	return( true );	
}

//---------------------------------------------------------
bool CGrid_Fill::On_Execute_Finish(void)
{
	m_Stack.Clear();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CGrid_Fill::Push(int x, int y)
{
	if( m_iStack >= m_Stack.Get_Count() )
	{
		m_Stack.Set_Count(m_Stack.Get_Count() + 1000);
	}

	m_Stack[m_iStack].x	= x;
	m_Stack[m_iStack].y	= y;

	m_iStack++;
}

//---------------------------------------------------------
inline void CGrid_Fill::Pop(int &x, int &y)
{
	m_iStack--;

	x	= m_Stack[m_iStack].x;
	y	= m_Stack[m_iStack].y;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Fill::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	//-----------------------------------------------------
	if(  Mode == MODULE_INTERACTIVE_LDOWN )
	{
		int		x, y, i, ix, iy, nReplaced;
		double	z, zMin, zMax;

		x	= Get_System()->Get_xWorld_to_Grid(ptWorld.Get_X());
		y	= Get_System()->Get_yWorld_to_Grid(ptWorld.Get_Y());

		if( m_pGrid && m_pGrid->is_InGrid(x, y) )
		{
			Message_Add(_TL("Starting flood fill..."));

			switch( m_Method )
			{
			case 0:	z	= m_pGrid->asDouble(x, y);	break;	// value at mouse position
			case 1:	z	= m_zFixed;					break;	// fixed value
			case 2:	z	= 0.0;						break;	// tolerance as absolute values
			}

			zMin		= z + m_zTolerance_Min;
			zMax		= z + m_zTolerance_Max;

			m_iStack	= 0;
			nReplaced	= 1;

			Push(x, y);

			//---------------------------------------------
			while( m_iStack > 0 && Set_Progress(nReplaced, m_pGrid->Get_NCells()) )
			{
				Pop(x, y);

				for(i=0; i<8; i+=2)
				{
					ix	= Get_xTo(i, x);
					iy	= Get_yTo(i, y);

					if(	m_pGrid->is_InGrid(ix, iy) )
					{
						z	= m_pGrid->asDouble(ix, iy);

						if( z != m_zFill && z >= zMin && z <= zMax )
						{
							Push(ix, iy);

							m_pGrid->Set_Value(ix, iy, m_zFill);

							nReplaced++;
						}
					}
				}
			}

			//---------------------------------------------
			Message_Add(_TL("ready"), false);
			Message_Add(CSG_String::Format(SG_T("%d %s"), nReplaced, _TL("replacements")));

			DataObject_Update(m_pGrid, m_pGrid->Get_ZMin(), m_pGrid->Get_ZMax());

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
