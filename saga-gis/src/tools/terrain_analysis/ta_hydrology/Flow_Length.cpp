/**********************************************************
 * Version $Id: Flow_Length.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Flow_Length.cpp                    //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
//                University of Hamburg                  //
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
#include "Flow_Length.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_Length::CFlow_Length(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Maximum Flow Path Length"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool calculates the maximum upstream or downstream distance "
		"or weighted distance along the flow path for each cell based on "
		"\'Deterministic 8 (D8)\' (O'Callaghan and Mark 1984) flow directions."
	));

	Add_Reference("O'Callaghan, J.F. & Mark, D.M.", "1984",
		"The extraction of drainage networks from digital elevation data",
		"Computer Vision, Graphics and Image Processing, 28:323-344."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"WEIGHTS"	, _TL("Weights"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"DISTANCE"	, _TL("Flow Path Length"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"DIRECTION"	, _TL("Direction of Measurement"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("downstream"),
			_TL("upstream")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Length::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pDEM		= Parameters("ELEVATION")->asGrid();
	CSG_Grid	*pWeights	= Parameters("WEIGHTS"  )->asGrid();

	if( !pDEM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pDistance	= Parameters("DISTANCE")->asGrid();

	pDistance->Set_NoData_Value(-1.0);
	pDistance->Assign_NoData();

	int		x, y, i;

	//-----------------------------------------------------
	if( Parameters("DIRECTION")->asInt() == 0 )	// downstream
	{
		pDistance->Set_Name("%s [%s]", _TL("Flow Path Length"), _TL("downstream"));

		DataObject_Set_Colors(pDistance, 11, SG_COLORS_RAINBOW);

		for(sLong iCell=0; iCell<Get_NCells() && Set_Progress_NCells(iCell); iCell++)
		{
			if( pDEM->Get_Sorted(iCell, x, y, false) )
			{
				double	Distance	= 0.0;

				if( (i = pDEM->Get_Gradient_NeighborDir(x, y, true, false)) >= 0 )
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( pDistance->is_InGrid(ix, iy) )
					{
						Distance	= pDistance->asDouble(ix, iy) + Get_Length(i) * (pWeights ? pWeights->asDouble(ix, iy) : 1.0);
					}
				}

				pDistance->Set_Value(x, y, Distance);
			}
		}
	}

	//-----------------------------------------------------
	else	// upstream
	{
		pDistance->Set_Name("%s [%s]", _TL("Flow Path Length"), _TL("upstream"));

		DataObject_Set_Colors(pDistance, 11, SG_COLORS_WHITE_BLUE);

		for(sLong iCell=0; iCell<Get_NCells() && Set_Progress_NCells(iCell); iCell++)
		{
			if( pDEM->Get_Sorted(iCell, x, y, true) )
			{
				if( pDistance->is_NoData(x, y) )
				{
					pDistance->Set_Value(x, y, 0.0);
				}

				if( (i = pDEM->Get_Gradient_NeighborDir(x, y, true, false)) >= 0 )
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( pDEM->is_InGrid(ix, iy) )
					{
						double	Distance	= pDistance->asDouble(x, y) + Get_Length(i) * (pWeights ? pWeights->asDouble(x, y) : 1.0);

						if( pDistance->is_NoData(ix, iy) || pDistance->asDouble(ix, iy) < Distance )
						{
							pDistance->Set_Value(ix, iy, Distance);
						}
					}
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
