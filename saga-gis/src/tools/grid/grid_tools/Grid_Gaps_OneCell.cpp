/**********************************************************
 * Version $Id$
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
//                 Grid_Gaps_OneCell.cpp                 //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#include "Grid_Gaps_OneCell.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Gaps_OneCell::CGrid_Gaps_OneCell(void)
{
	Set_Name		(_TL("Close One Cell Gaps"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Closes one cell gaps using the mean value of the surrounding cell values. "
		"If the target is not set, the changes will be stored to the original grid. "
	));

	Parameters.Add_Grid(NULL,
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"RESULT"	, _TL("Changed Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(NULL,
		"MODE"		, _TL("Neighbourhood"),
		_TL("Neumann: the four horizontally and vertically neighboured cells; Moore: all eight adjacent cells"),
		CSG_String::Format("%s|%s|",
			_TL("Neumann"),
			_TL("Moore")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Gaps_OneCell::On_Execute(void)
{
	CSG_Grid	*pInput		= Parameters("INPUT" )->asGrid();
	CSG_Grid	*pResult	= Parameters("RESULT")->asGrid();

	DataObject_Set_Parameters(pResult, pInput);

	pResult->Set_Name(CSG_String::Format("%s [%s]", pInput->Get_Name(), _TL("Close Gaps")));

	int	iStep	= Parameters("MODE")->asInt() == 0 ? 2 : 1;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pInput->is_NoData(x, y) )
			{
				pResult->Set_Value(x, y, pInput->asDouble(x, y));
			}
			else
			{
				bool	bClose	= true;

				CSG_Simple_Statistics	s;

				for(int i=0; i<8 && bClose; i+=iStep)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( (bClose = pInput->is_InGrid(ix, iy)) == true )
					{
						s	+= pInput->asDouble(ix, iy);
					}
				}

				if( bClose )
				{
					pResult->Set_Value(x, y, s.Get_Mean());
				}
				else
				{
					pResult->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
