
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
#include "polygon_split_parts.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Split_Parts::CPolygon_Split_Parts(void)
{
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
	CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();
	CSG_Shapes *pParts    = Parameters("PARTS"   )->asShapes();

	pParts->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("Parts")), pPolygons, pPolygons->Get_Vertex_Type());

	bool bLakes = Parameters("LAKES")->asBool();

	//-----------------------------------------------------
	for(sLong iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon *pPolygon = pPolygons->Get_Shape(iPolygon)->asPolygon();

		for(int iPart=0, iPartOut=0; iPart<pPolygon->Get_Part_Count() && Process_Get_Okay(); iPart++)
		{
			if( bLakes || !pPolygon->is_Lake(iPart) )
			{
				CSG_Shape_Polygon *pPart = pParts->Add_Shape(pPolygon, SHAPE_COPY_ATTR)->asPolygon();

				for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
				{
					pPart->Add_Point(pPolygon->Get_Point(iPoint, iPart));

					if( pPolygons->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
					{
						pPart->Set_Z(pPolygon->Get_Z(iPoint, iPart), iPoint, iPartOut);

						if( pPolygons->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
						{
							pPart->Set_M(pPolygon->Get_M(iPoint, iPart), iPoint, iPartOut);
						}
					}
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

								if( pPolygons->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
								{
									pPart->Set_Z(pPolygon->Get_Z(jPoint, jPart), jPoint, nPart);

									if( pPolygons->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
									{
										pPart->Set_M(pPolygon->Get_M(jPoint, jPart), jPoint, nPart);
									}
								}
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
