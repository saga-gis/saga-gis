
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
		"Closes one cell gaps using the arithmetic mean, median, majority or minority "
		"value of the surrounding cell values. "
		"If the target is not set, the changes will be stored to the original grid. "
	));

	Parameters.Add_Grid("",
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT"	, _TL("Changed Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"MODE"		, _TL("Neighbourhood"),
		_TL("Neumann: the four horizontally and vertically neighboured cells; Moore: all eight adjacent cells"),
		CSG_String::Format("%s|%s",
			_TL("Neumann"),
			_TL("Moore")
		), 1
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Value"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("arithmetic mean"),
			_TL("median"),
			_TL("majority"),
			_TL("minority")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Gaps_OneCell::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("INPUT")->asGrid();

	if( pGrid != Parameters("RESULT")->asGrid() && Parameters("RESULT")->asGrid() )
	{
		Process_Set_Text(_TL("Copying original data..."));

		CSG_Grid	*pResult	= Parameters("RESULT")->asGrid();

		pResult->Create(*pGrid);

		pResult->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Close Gaps"));

		DataObject_Set_Parameters(pResult, pGrid);

		pGrid	= pResult;
	}

	//-----------------------------------------------------
	int	iStep	= Parameters("MODE")->asInt() == 0 ? 2 : 1;

	int	Method	= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pGrid->is_NoData(x, y) )
			{
				bool	bClose	= true;	double	Value;

				switch( Method )
				{
				//-----------------------------------------
				default:	// mean
				case  1: {	// median
					CSG_Simple_Statistics	s(Method == 1);

					for(int i=0; i<8 && bClose; i+=iStep)
					{
						int	ix = Get_xTo(i, x), iy = Get_yTo(i, y);

						if( (bClose = pGrid->is_InGrid(ix, iy)) == true )
						{
							s	+= pGrid->asDouble(ix, iy);
						}
					}

					Value	= Method == 1 ? s.Get_Median() : s.Get_Mean();
				}	break;

				//-----------------------------------------
				case  2:	// majority
				case  3: {	// minority
					CSG_Unique_Number_Statistics	s;

					for(int i=0; i<8 && bClose; i+=iStep)
					{
						int	ix = Get_xTo(i, x), iy = Get_yTo(i, y);

						if( (bClose = pGrid->is_InGrid(ix, iy)) == true )
						{
							s	+= pGrid->asDouble(ix, iy);
						}
					}

					bClose	= Method == 3 ? s.Get_Minority(Value) : s.Get_Majority(Value);
				}	break;
				}

				//-----------------------------------------
				if( bClose )
				{
					pGrid->Set_Value(x, y, Value);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( pGrid == Parameters("INPUT")->asGrid() )
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
