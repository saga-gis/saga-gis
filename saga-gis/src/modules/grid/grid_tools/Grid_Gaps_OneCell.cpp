
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
#include "Grid_Gaps_OneCell.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Gaps_OneCell::CGrid_Gaps_OneCell(void)
{
	Set_Name(_TL("Close One Cell Gaps"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW(
		"Closes one cell gaps using the mean value of the surrounding cell values. "
		"If the target is not set, the changes will be stored to the original grid. ")
	);

	Parameters.Add_Grid(
		NULL, "INPUT"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"	, _TL("Changed Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
CGrid_Gaps_OneCell::~CGrid_Gaps_OneCell(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Gaps_OneCell::On_Execute(void)
{
	bool	bCloseGap;
	int		x, y, i, ix, iy;
	double	Sum;
	CSG_Grid	*pInput, *pResult;

	pInput	= Parameters("INPUT")->asGrid();
	pResult	= Parameters("RESULT")->asGrid();

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pInput->is_NoData(x, y) )
			{
				for(i=0, bCloseGap=true, Sum=0.0; i<8 && bCloseGap; i++)
				{
					Get_System()->Get_Neighbor_Pos(i, x, y, ix, iy);

					if( !pInput->is_InGrid(ix, iy) )
					{
						bCloseGap	= false;
					}
					else
					{
						Sum			+= pInput->asDouble(ix, iy);
					}
				}

				if( bCloseGap )
				{
					pResult->Set_Value(x, y, Sum / 8.0);
				}
				else
				{
					pResult->Set_NoData(x, y);
				}
			}
			else
			{
				pResult->Set_Value(x, y, pInput->asDouble(x, y));
			}
		}
	}

	return( true );
}
