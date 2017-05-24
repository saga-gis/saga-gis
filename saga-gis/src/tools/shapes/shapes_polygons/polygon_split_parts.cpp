/**********************************************************
 * Version $Id: polygon_split_parts.cpp 915 2011-02-15 08:43:36Z oconrad $
 *********************************************************/

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
//                polygon_split_parts.cpp                //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#include "polygon_split_parts.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Split_Parts::CPolygon_Split_Parts(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Parts to Separate Polygons"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Splits parts of multipart polygons into separate polygons. "
		"This can be done only for islands (outer rings) or for all "
		"parts (inner and outer rings) by checking the 'lakes' option."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"PARTS"		, _TL("Polygon Parts"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Bool("",
		"LAKES"		, _TL("Lakes"),
		_TL(""),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Split_Parts::On_Execute(void)
{
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();
	CSG_Shapes	*pParts		= Parameters("PARTS"   )->asShapes();

	pParts->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("Parts")), pPolygons);

	bool	bLakes	= Parameters("LAKES")->asBool();

	//-----------------------------------------------------
	for(int iShape=0; iShape<pPolygons->Get_Count() && Set_Progress(iShape, pPolygons->Get_Count()); iShape++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iShape);

		for(int iPart=0; iPart<pPolygon->Get_Part_Count() && Process_Get_Okay(); iPart++)
		{
			if( bLakes || !pPolygon->is_Lake(iPart) )
			{
				CSG_Shape_Polygon	*pPart	= (CSG_Shape_Polygon *)pParts->Add_Shape(pPolygon, SHAPE_COPY_ATTR);

				for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
				{
					pPart->Add_Point(pPolygon->Get_Point(iPoint, iPart));
				}

				if( !bLakes )
				{
					for(int jPart=0; jPart<pPolygon->Get_Part_Count(); jPart++)
					{
						if(	pPolygon->is_Lake(jPart) && pPart->Contains(pPolygon->Get_Point(0, jPart)) )
						{
							for(int jPoint=0, nPart=pPart->Get_Part_Count(); jPoint<pPolygon->Get_Point_Count(jPart); jPoint++)
							{
								pPart->Add_Point(pPolygon->Get_Point(jPoint, jPart), nPart);
							}
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
