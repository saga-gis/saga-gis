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
//                       TIN_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TIN_To_Shapes.cpp                   //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "TIN_To_Shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_To_Shapes::CTIN_To_Shapes(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("TIN to Shapes"));

	Set_Author		("O.Conrad (c) 2004");

	Set_Description(_TW(
		"Converts a TIN data set to shapes layers."
	));

	//-----------------------------------------------------
	Parameters.Add_TIN(
		"", "TIN"		, _TL("TIN"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		"", "POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		"", "CENTER"	, _TL("Center of Triangles"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		"", "EDGES"		, _TL("Edges"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		"", "TRIANGLES"	, _TL("Triangles"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		"", "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_To_Shapes::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_TIN	*pTIN = Parameters("TIN")->asTIN();

	//-----------------------------------------------------
	if( Parameters("POINTS")->asShapes() )
	{
		CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

		pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", pTIN->Get_Name(), _TL("TIN")), pTIN);

		for(int i=0; i<pTIN->Get_Node_Count() && Set_Progress(i, pTIN->Get_Node_Count()); i++)
		{
			pPoints->Add_Shape(pTIN->Get_Node(i), SHAPE_COPY_ATTR)->Add_Point(pTIN->Get_Node(i)->Get_Point());
		}
	}

	//-----------------------------------------------------
	if( Parameters("EDGES")->asShapes() )
	{
		CSG_Shapes *pLines	= Parameters("EDGES")->asShapes();

		pLines->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s]", pTIN->Get_Name(), _TL("TIN Edges")));

		pLines->Add_Field("POINT_ID_A", SG_DATATYPE_Int);
		pLines->Add_Field("POINT_ID_B", SG_DATATYPE_Int);

		for(int i=0; i<pTIN->Get_Edge_Count() && Set_Progress(i, pTIN->Get_Edge_Count()); i++)
		{
			CSG_TIN_Edge	*pEdge	= pTIN->Get_Edge(i);

			CSG_Shape	*pLine	= pLines->Add_Shape();

			pLine->Add_Point(pEdge->Get_Node(0)->Get_Point());
			pLine->Add_Point(pEdge->Get_Node(1)->Get_Point());

			pLine->Set_Value(0, pEdge->Get_Node(0)->Get_ID());
			pLine->Set_Value(1, pEdge->Get_Node(1)->Get_ID());
		}
	}

	//-----------------------------------------------------
	if( Parameters("TRIANGLES")->asShapes() )
	{
		CSG_Shapes	*pPolygons	= Parameters("TRIANGLES")->asShapes();

		pPolygons->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s]", pTIN->Get_Name(), _TL("TIN Triangles")));

		pPolygons->Add_Field("POINT_ID_A", SG_DATATYPE_Int);
		pPolygons->Add_Field("POINT_ID_B", SG_DATATYPE_Int);
		pPolygons->Add_Field("POINT_ID_C", SG_DATATYPE_Int);

		for(int i=0; i<pTIN->Get_Triangle_Count() && Set_Progress(i, pTIN->Get_Triangle_Count()); i++)
		{
			CSG_TIN_Triangle	*pTriangle	= pTIN->Get_Triangle(i);

			CSG_Shape	*pPolygon	= pPolygons->Add_Shape();

			pPolygon->Add_Point(pTriangle->Get_Node(0)->Get_Point());
			pPolygon->Add_Point(pTriangle->Get_Node(1)->Get_Point());
			pPolygon->Add_Point(pTriangle->Get_Node(2)->Get_Point());

			pPolygon->Set_Value(0, pTriangle->Get_Node(0)->Get_ID());
			pPolygon->Set_Value(1, pTriangle->Get_Node(1)->Get_ID());
			pPolygon->Set_Value(2, pTriangle->Get_Node(2)->Get_ID());
		}
	}

	//-----------------------------------------------------
	if( Parameters("CENTER")->asShapes() )
	{
		CSG_Shapes	*pPoints	= Parameters("CENTER")->asShapes();

		pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", pTIN->Get_Name(), _TL("TIN Centroids")));

		pPoints->Add_Field("POINT_ID_A", SG_DATATYPE_Int);
		pPoints->Add_Field("POINT_ID_B", SG_DATATYPE_Int);
		pPoints->Add_Field("POINT_ID_C", SG_DATATYPE_Int);

		for(int i=0; i<pTIN->Get_Triangle_Count() && Set_Progress(i, pTIN->Get_Triangle_Count()); i++)
		{
			CSG_TIN_Triangle	*pTriangle	= pTIN->Get_Triangle(i);

			CSG_Shape	*pPoint	= pPoints->Add_Shape();

			pPoint->Add_Point(pTriangle->Get_CircumCircle_Point());

			pPoint->Set_Value(0, pTriangle->Get_Node(0)->Get_Index());
			pPoint->Set_Value(1, pTriangle->Get_Node(1)->Get_Index());
			pPoint->Set_Value(2, pTriangle->Get_Node(2)->Get_Index());
		}
	}

	//-----------------------------------------------------
	if( Parameters("POLYGONS")->asShapes() )
	{
		CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

		pPolygons->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s]", pTIN->Get_Name(), _TL("TIN Voronoi")), pTIN);

		for(int i=0; i<pTIN->Get_Node_Count() && Set_Progress(i, pTIN->Get_Node_Count()); i++)
		{
			CSG_Points	Points;

			if( pTIN->Get_Node(i)->Get_Polygon(Points) )
			{
				CSG_Shape	*pPolygon	= pPolygons->Add_Shape(pTIN->Get_Node(i), SHAPE_COPY_ATTR);

				for(int iPoint=0; iPoint<Points.Get_Count(); iPoint++)
				{
					pPolygon->Add_Point(Points[iPoint]);
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
