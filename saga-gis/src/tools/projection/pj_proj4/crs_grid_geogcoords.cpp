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
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 crs_grid_geogcoords.cpp               //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
#include "crs_grid_geogcoords.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Grid_GeogCoords::CCRS_Grid_GeogCoords(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Geographic Coordinate Grids"));

	Set_Author		("O. Conrad (c) 2014");

	Set_Description	(_TW(
		"Creates for a given grid geographic coordinate information, "
		"i.e. two grids specifying the longitude and latitude for each cell. "
		"The coordinate system of the input grid has to be defined. "
	));

	Set_Description	(Get_Description() + "\n" + CSG_CRSProjector::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "LON"		, _TL("Longitude"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "LAT"		, _TL("Latitude"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Grid_GeogCoords::On_Execute(void)
{
	CSG_CRSProjector	Projector;

	if( !Projector.Set_Source(Parameters("GRID")->asGrid()->Get_Projection()) )
	{
		Error_Set(_TL("Could not initialize grid projection."));

		return( false );
	}

	Projector.Set_Target(CSG_Projection("+proj=longlat +ellps=WGS84 +datum=WGS84", SG_PROJ_FMT_Proj4));

	//-----------------------------------------------------
	CSG_Grid	*pLon	= Parameters("LON")->asGrid();
	CSG_Grid	*pLat	= Parameters("LAT")->asGrid();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	yWorld	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Point	p(Get_XMin() + x * Get_Cellsize(), yWorld);

			if( Projector.Get_Projection(p) )
			{
				pLon->Set_Value(x, y, p.Get_X());
				pLat->Set_Value(x, y, p.Get_Y());
			}
			else
			{
				pLon->Set_NoData(x, y);
				pLat->Set_NoData(x, y);
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
