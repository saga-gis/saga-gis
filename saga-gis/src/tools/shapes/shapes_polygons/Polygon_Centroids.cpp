
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    shapes_polygons                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Polygon_Centroids.cpp                 //
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
#include "Polygon_Centroids.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Centroids::CPolygon_Centroids(void)
{
	Set_Name		(_TL("Polygon Centroids"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Creates a points layer containing the centroids of the input polygon layer."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		"", "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		"", "CENTROIDS"	, _TL("Centroids"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Bool(
		"", "METHOD"	, _TL("Centroids for each part"),
		_TL(""),
		false
	);

	Parameters.Add_Bool(
		"", "INSIDE"	, _TL("Force Inside"),
		_TL("If a centroid falls outside its polygon, then move it to the closest boundary."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Centroids::On_Execute(void)
{
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if( !pPolygons->is_Valid() || pPolygons->Get_Count() < 1 || pPolygons->Get_Type() != SHAPE_TYPE_Polygon )
	{
		Error_Set(_TL("Invalid input polygons."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pCentroids	= Parameters("CENTROIDS")	->asShapes();

	pCentroids->Create(SHAPE_TYPE_Point, pPolygons->Get_Name(), pPolygons);

	bool	bParts	= Parameters("METHOD")->asBool();
	bool	bInside	= Parameters("INSIDE")->asBool();

	//-----------------------------------------------------
	for(int iPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

		if( !bParts )
		{
			TSG_Point	Centroid	= pPolygon->Get_Centroid();

			if( bInside ) { Force_Inside(Centroid, pPolygon); }

			pCentroids->Add_Shape(pPolygon, SHAPE_COPY_ATTR)->Add_Point(Centroid);
		}
		else for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			TSG_Point	Centroid	= pPolygon->Get_Centroid(iPart);

			if( bInside ) { Force_Inside(Centroid, pPolygon); }

			pCentroids->Add_Shape(pPolygon, SHAPE_COPY_ATTR)->Add_Point(Centroid);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Centroids::Force_Inside(TSG_Point &Centroid, CSG_Shape_Polygon *pPolygon)
{
	TSG_Point	cMin;	double	dMin	= -1.;

	for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		TSG_Point	A	= pPolygon->Get_Point(0, iPart, false);

		for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	C, B = A; A = pPolygon->Get_Point(iPoint, iPart);

			double	d	= SG_Get_Nearest_Point_On_Line(Centroid, A, B, C, true);

			if( d >= 0. && (dMin < 0. || d < dMin) )
			{
				dMin	= d;
				cMin	= C;
			}
		}
	}

	//-----------------------------------------------------
	if( dMin < 0. )
	{
		return( false );
	}

	Centroid	= cMin;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
