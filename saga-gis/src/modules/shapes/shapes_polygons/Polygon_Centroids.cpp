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
#include "Polygon_Centroids.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Centroids::CPolygon_Centroids(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Centroids"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"Creates a points layer containing the centroids of the input polygon layer."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "CENTROIDS"	, _TL("Centroids"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Value(
		NULL	, "METHOD"		, _TL("Centroids for each part"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	//-----------------------------------------------------
}

//---------------------------------------------------------
CPolygon_Centroids::~CPolygon_Centroids(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Centroids::On_Execute(void)
{
	bool			bPart;
	int				iShape, iPart;
	CSG_Shape			*pCentroid;
	CSG_Shape_Polygon	*pPolygon;
	CSG_Shapes			*pPolygons, *pCentroids;

	pPolygons	= Parameters("POLYGONS")	->asShapes();
	pCentroids	= Parameters("CENTROIDS")	->asShapes();
	bPart		= Parameters("METHOD")		->asBool();

	if(	pPolygons->Get_Type() == SHAPE_TYPE_Polygon && pPolygons->Get_Count() > 0 )
	{
		pCentroids->Create(SHAPE_TYPE_Point, pPolygons->Get_Name(), pPolygons);

		//-------------------------------------------------
		for(iShape=0; iShape<pPolygons->Get_Count(); iShape++)
		{
			pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iShape);

			if( bPart )
			{
				for(iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
				{
					pCentroid	= pCentroids->Add_Shape(pPolygon, SHAPE_COPY_ATTR);
					pCentroid->Add_Point(pPolygon->Get_Centroid(iPart));
				}
			}
			else
			{
				pCentroid	= pCentroids->Add_Shape(pPolygon, SHAPE_COPY_ATTR);
				pCentroid->Add_Point(pPolygon->Get_Centroid());
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
