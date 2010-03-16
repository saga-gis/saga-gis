
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Grid_Proximity.cpp                  //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#include "Grid_Proximity.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Proximity::CGrid_Proximity(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Proximity Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Calculates a grid with euclidean distance to feature cells (not no-data cells)."
	));


	//-----------------------------------------------------
	// 2. Standard in- and output...

	Parameters.Add_Grid(
		NULL	, "FEATURES"		, _TL("Features"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "DISTANCE"		, _TL("Distance"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "DIRECTION"		, _TL("Direction"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "ALLOCATION"		, _TL("Allocation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Proximity::On_Execute(void)
{
	int				x, y;
	double			z, d;
	TSG_Point		p;
	CSG_Grid		*pFeatures, *pDistance, *pDirection, *pAllocation;
	CSG_PRQuadTree	Search;

	//-----------------------------------------------------
	pFeatures	= Parameters("FEATURES")	->asGrid();
	pDistance	= Parameters("DISTANCE")	->asGrid();
	pDirection	= Parameters("DIRECTION")	->asGrid();
	pAllocation	= Parameters("ALLOCATION")	->asGrid();

	//-----------------------------------------------------
	Process_Set_Text(_TL("preparing..."));

	Search.Create(CSG_Rect(-1, -1, Get_NX(), Get_NY()));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !pFeatures->is_NoData(x, y) )
			{
				Search.Add_Point(x, y, pFeatures->asDouble(x, y));
			}
		}
	}

	if( !Search.is_Okay() )
	{
		Message_Add(_TL("no features to buffer."));

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("distance calculation..."));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( Search.Get_Nearest_Point(x, y, p, z, d) )
			{
				pDistance->Set_Value(x, y, d * Get_Cellsize());

				if( pDirection )
				{
					if( d > 0.0 )
					{
						pDirection->Set_Value(x, y, SG_Get_Angle_Of_Direction(x, y, p.x, p.y) * M_RAD_TO_DEG);
					}
					else
					{
						pDirection->Set_NoData(x, y);
					}
				}

				if( pAllocation )
				{
					pAllocation->Set_Value(x, y, z);
				}
			}
			else
			{
				pDistance->Set_NoData(x, y);

				if( pDirection )
				{
					pDirection->Set_NoData(x, y);
				}

				if( pAllocation )
				{
					pAllocation->Set_NoData(x, y);
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
