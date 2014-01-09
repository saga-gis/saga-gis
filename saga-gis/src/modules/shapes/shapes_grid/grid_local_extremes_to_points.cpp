/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//            grid_local_extremes_to_points.cpp          //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "grid_local_extremes_to_points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Local_Extremes_to_Points::CGrid_Local_Extremes_to_Points(void)
{
	Set_Name		(_TL("Local Minima and Maxima"));

	Set_Author		(SG_T("(c) 2009 by O.Conrad"));

	Set_Description	(_TW(
		"Extracts local grid value minima and maxima of to vector points."
	));

	Parameters.Add_Grid(
		NULL, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL, "MINIMA"	, _TL("Minima"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL, "MAXIMA"	, _TL("Maxima"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Local_Extremes_to_Points::On_Execute(void)
{
	bool		bMinimum, bMaximum;
	int			x, y, i, ix, iy;
	double		z, iz;
	CSG_Grid	*pGrid;
	TSG_Point	p;
	CSG_Shape	*pPoint;
	CSG_Shapes	*pMinima, *pMaxima;

	pGrid		= Parameters("GRID")	->asGrid();
	pMinima		= Parameters("MINIMA")	->asShapes();
	pMaxima		= Parameters("MAXIMA")	->asShapes();

	pMinima->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("Local Minima")));
	pMinima->Add_Field(SG_T("GRID_X")	, SG_DATATYPE_Int);
	pMinima->Add_Field(SG_T("GRID_Y")	, SG_DATATYPE_Int);
	pMinima->Add_Field(SG_T("X")		, SG_DATATYPE_Double);
	pMinima->Add_Field(SG_T("Y")		, SG_DATATYPE_Double);
	pMinima->Add_Field(SG_T("Z")		, SG_DATATYPE_Double);

	pMaxima->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pGrid->Get_Name(), _TL("Local Maxima")));
	pMaxima->Add_Field(SG_T("GRID_X")	, SG_DATATYPE_Int);
	pMaxima->Add_Field(SG_T("GRID_Y")	, SG_DATATYPE_Int);
	pMaxima->Add_Field(SG_T("X")		, SG_DATATYPE_Double);
	pMaxima->Add_Field(SG_T("Y")		, SG_DATATYPE_Double);
	pMaxima->Add_Field(SG_T("Z")		, SG_DATATYPE_Double);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			z	= pGrid->asDouble(x, y);

			for(i=0, bMinimum=true, bMaximum=true; i<8 && (bMinimum || bMaximum); i++)
			{
				if( !Get_System()->Get_Neighbor_Pos(i, x, y, ix, iy) || pGrid->is_NoData(ix, iy) )
				{
					bMinimum	= bMaximum	= false;
				}
				else
				{
					iz	= pGrid->asDouble(ix, iy);

					if( iz <= z )
					{
						bMinimum	= false;
					}
					else if( iz >= z )
					{
						bMaximum	= false;
					}
				}
			}

			pPoint	= bMinimum ? pMinima->Add_Shape() : (bMaximum ? pMaxima->Add_Shape() : NULL);

			if( pPoint )
			{
				p		= Get_System()->Get_Grid_to_World(x, y);
				pPoint->Set_Point(p, 0);
				pPoint->Set_Value(0, x);
				pPoint->Set_Value(1, y);
				pPoint->Set_Value(2, p.x);
				pPoint->Set_Value(3, p.y);
				pPoint->Set_Value(4, z);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
