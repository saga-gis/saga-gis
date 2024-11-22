
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
//           polygon_invert_ring_ordering.cpp            //
//                                                       //
//                 Copyright (C) 2024 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "polygon_invert_ring_ordering.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Invert_Ring_Ordering::CPolygon_Invert_Ring_Ordering(void)
{
	Set_Name		(_TL("Invert Ring Ordering"));

	Set_Author		("V.Wichmann (c) 2024");

	Set_Description	(_TW(
		"The tool allows one to invert the ring ordering of polygons, i.e. to change their orientation."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL("The input polygon shapes layer."),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"INVERTED"	, _TL("Inverted Polygons"),
		_TL("The polygon shapes layer with the inverted ring ordering."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Invert_Ring_Ordering::On_Execute(void)
{
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if( !pPolygons->is_Valid() || pPolygons->Get_Count() < 1 )
	{
		Error_Set(_TL("Invalid input polygons."));

		return( false );
	}

	CSG_Shapes	*pInverted	= Parameters("INVERTED")->asShapes();

	pInverted->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("inverted")), pPolygons, pPolygons->Get_Vertex_Type());


	//-----------------------------------------------------
	for(sLong iPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
	{
		CSG_Shape *pIn  = pPolygons->Get_Shape(iPolygon);
		CSG_Shape *pOut = pInverted->Add_Shape(pIn, SHAPE_COPY_ATTR);

		for(int iPart=0; iPart<pIn->Get_Part_Count(); iPart++)
		{
			int iPointOut = 0;

			for(int iPoint=pIn->Get_Point_Count(iPart) - 1; iPoint>=0; iPoint--)
			{
				pOut->Add_Point(pIn->Get_Point(iPoint, iPart), iPart);

				if( pPolygons->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
                {
                    pOut->Set_Z(pIn->Get_Z(iPoint, iPart), iPointOut, iPart);

                    if( pPolygons->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
                    {
                        pOut->Set_M(pIn->Get_M(iPoint, iPart), iPointOut, iPart);
                    }
                }

				iPointOut++;
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
