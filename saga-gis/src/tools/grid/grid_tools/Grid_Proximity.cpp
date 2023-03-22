
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#include "Grid_Proximity.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Proximity::CGrid_Proximity(void)
{
	Set_Name		(_TL("Proximity Grid"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Calculates a grid with euclidean distance to feature cells (not no-data cells)."
	));

	Parameters.Add_Grid("", "FEATURES"  , _TL("Features"  ), _TL(""), PARAMETER_INPUT          );
	Parameters.Add_Grid("", "DISTANCE"  , _TL("Distance"  ), _TL(""), PARAMETER_OUTPUT         );
	Parameters.Add_Grid("", "DIRECTION" , _TL("Direction" ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "ALLOCATION", _TL("Allocation"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Proximity::On_Execute(void)
{
	CSG_Grid *pFeatures   = Parameters("FEATURES"  )->asGrid();
	CSG_Grid *pDistance   = Parameters("DISTANCE"  )->asGrid();
	CSG_Grid *pDirection  = Parameters("DIRECTION" )->asGrid();
	CSG_Grid *pAllocation = Parameters("ALLOCATION")->asGrid();

	//-----------------------------------------------------
	Process_Set_Text(_TL("preparing distance calculation..."));

	CSG_Shapes Points(SHAPE_TYPE_Point, NULL, NULL, SG_VERTEX_TYPE_XYZ);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pFeatures->is_NoData(x, y) )
			{
				pDistance->Set_Value(x, y, -1.);
			}
			else
			{
				pDistance->Set_Value(x, y,  0.);

				if( pDirection )
				{
					pDirection->Set_NoData(x, y);
				}

				if( pAllocation )
				{
					pAllocation->Set_Value(x, y, pFeatures->asDouble(x, y));
				}

				//-----------------------------------------
				bool bBorder = false;

				for(int i=0; !bBorder && i<8; i++)
				{
					int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

					bBorder = is_InGrid(ix, iy) && pFeatures->is_NoData(ix, iy);
				}

				if( bBorder )
				{
					Points.Add_Shape()->Add_Point(CSG_Point_3D(x, y, pFeatures->asDouble(x, y)));
				}
			}
		}
	}

	if( Points.Get_Count() < 1 )
	{
		Message_Add(_TL("no features to allocate."));

		return( false );
	}

	CSG_KDTree_2D Search(&Points);

	//-----------------------------------------------------
	Process_Set_Text(_TL("performing distance calculation..."));

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pDistance->asDouble(x, y) < 0. )
			{
				size_t Index; double Distance;

				if( Search.Get_Nearest_Point(x, y, Index, Distance) )
				{
					pDistance->Set_Value(x, y, Distance * Get_Cellsize());

					CSG_Shape &Point = *Points.Get_Shape((sLong)Index);

					if( pDirection )
					{
						if( Distance > 0. )
						{
							TSG_Point p = Point.Get_Point(0);

							pDirection->Set_Value(x, y, SG_Get_Angle_Of_Direction(x, y, p.x, p.y) * M_RAD_TO_DEG);
						}
						else
						{
							pDirection->Set_NoData(x, y);
						}
					}

					if( pAllocation )
					{
						pAllocation->Set_Value(x, y, Point.Get_Z(0));
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
