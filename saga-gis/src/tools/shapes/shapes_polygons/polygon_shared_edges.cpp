
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
//                polygon_shared_edges.cpp               //
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
#include "polygon_shared_edges.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Shared_Edges::CPolygon_Shared_Edges(void)
{
	Set_Name		(_TL("Shared Polygon Edges"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field("POLYGONS",
		"ATTRIBUTE"	, _TL("Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Shapes("",
		"EDGES"		, _TL("Edges"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Double("",
		"EPSILON"	, _TL("Tolerance"),
		_TL(""),
		0.00001, 0., true
	);

	Parameters.Add_Bool("",
		"DOUBLE"	, _TL("Double Edges"),
		_TL("give output of an edge twice, i.e. once for each of the two adjacent polygons"),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Shared_Edges::On_Execute(void)
{
	CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();

	int Field = Parameters("ATTRIBUTE")->asInt();

	CSG_Shapes *pEdges = Parameters("EDGES")->asShapes();
	pEdges->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("Edges")));
	pEdges->Add_Field("ID_A", Field < 0 ? SG_DATATYPE_Int : pPolygons->Get_Field_Type(Field));
	pEdges->Add_Field("ID_B", Field < 0 ? SG_DATATYPE_Int : pPolygons->Get_Field_Type(Field));

	double Epsilon = Parameters("EPSILON")->asDouble();

	//-----------------------------------------------------
	for(sLong iPolygon=0; iPolygon<pPolygons->Get_Count()-1 && Set_Progress(iPolygon, pPolygons->Get_Count()-1); iPolygon++)
	{
		CSG_Shape_Polygon *pA = pPolygons->Get_Shape(iPolygon)->asPolygon();

		for(sLong jPolygon=iPolygon+1; jPolygon<pPolygons->Get_Count() && Process_Get_Okay(); jPolygon++)
		{
			CSG_Shape_Polygon *pB = pPolygons->Get_Shape(jPolygon)->asPolygon();

			CSG_Lines Edges = pA->Get_Shared_Edges(pB, Epsilon);

			for(sLong iEdge=0; iEdge<Edges.Get_Count(); iEdge++)
			{
				if( Edges[iEdge].Get_Count() > 1 )
				{
					CSG_Shape *pEdge = pEdges->Add_Shape();

					if( Field < 0 )
					{
						pEdge->Set_Value(0, pA->Get_Index());
						pEdge->Set_Value(1, pB->Get_Index());
					}
					else
					{
						pEdge->Set_Value(0, pA->asString(Field));
						pEdge->Set_Value(1, pB->asString(Field));
					}

					for(sLong iPoint=0; iPoint<Edges[iEdge].Get_Count(); iPoint++)
					{
						pEdge->Add_Point(Edges[iEdge][iPoint]);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Parameters("DOUBLE")->asBool() )
	{
		for(sLong iEdge=0, nEdges=pEdges->Get_Count(); iEdge<nEdges && Set_Progress(iEdge, nEdges); iEdge++)
		{
			CSG_Shape *pA = pEdges->Get_Shape(iEdge);
			CSG_Shape *pB = pEdges->Add_Shape(pA);

			*(pB->Get_Value(0)) = *(pA->Get_Value(1));
			*(pB->Get_Value(1)) = *(pA->Get_Value(0));
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
CPolygon_Vertex_Check::CPolygon_Vertex_Check(void)
{
	Set_Name		(_TL("Polygon Vertex Check"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"CHECKED"		, _TL("Checked"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"ADDED"		, _TL("Added"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Double("",
		"EPSILON"		, _TL("Tolerance"),
		_TL(""),
		0., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Vertex_Check::On_Execute(void)
{
	CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();

	if( Parameters("CHECKED")->asShapes() && Parameters("CHECKED")->asShapes() != pPolygons )
	{
		CSG_Shapes *pCopy = Parameters("CHECKED")->asShapes();

		pCopy->Create(*pPolygons);
		pCopy->Fmt_Name("%s [%s]", pPolygons->Get_Name(), _TL("checked"));

		pPolygons = pCopy;
	}

	if( (m_pAdded = Parameters("ADDED")->asShapes()) != NULL )
	{
		m_pAdded->Create(SHAPE_TYPE_Point, _TL("Added"));
	}

	double Epsilon = Parameters("EPSILON")->asDouble();

	//-----------------------------------------------------
	for(sLong iPolygon=0; iPolygon<pPolygons->Get_Count()-1 && Set_Progress(iPolygon, pPolygons->Get_Count()-1); iPolygon++)
	{
		CSG_Shape_Polygon *pA = pPolygons->Get_Shape(iPolygon)->asPolygon();

		for(sLong jPolygon=iPolygon+1; jPolygon<pPolygons->Get_Count() && Process_Get_Okay(); jPolygon++)
		{
			CSG_Shape_Polygon *pB = pPolygons->Get_Shape(jPolygon)->asPolygon();

			for(int iPart=0; iPart<pA->Get_Part_Count() && Process_Get_Okay(); iPart++)
			{
				for(int jPart=0; jPart<pB->Get_Part_Count() && Process_Get_Okay(); jPart++)
				{
					if( pA->Get_Part(iPart)->Get_Extent().Intersects(pB->Get_Part(jPart)->Get_Extent()) )
					{
						Check_Vertices(pA->Get_Part(iPart), pB->Get_Part(jPart), Epsilon);
						Check_Vertices(pB->Get_Part(jPart), pA->Get_Part(iPart), Epsilon);
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Vertex_Check::Check_Vertices(CSG_Shape_Part *pPolygon, CSG_Shape_Part *pVertices, double Epsilon)
{
	TSG_Point A = pPolygon->Get_Point(0, false);

	for(int iPoint=0; iPoint<pPolygon->Get_Count(); iPoint++)
	{
		TSG_Point B = A; A = pPolygon->Get_Point(iPoint);

		for(int iVertex=0; iVertex<pVertices->Get_Count(); iVertex++)
		{
			TSG_Point Vertex = pVertices->Get_Point(iVertex);

			if( (SG_Is_Between(Vertex.x, A.x, B.x, Epsilon) && SG_Is_Between(Vertex.y, A.y, B.y, Epsilon))
			&&  !SG_Is_Equal(Vertex, A, Epsilon) && !SG_Is_Equal(Vertex, B, Epsilon)
			&&  SG_Is_Point_On_Line(Vertex, A, B, true, Epsilon) )
			{
				pPolygon->Ins_Point(Vertex, iPoint++);

				if( m_pAdded )
				{
					m_pAdded->Add_Shape()->Add_Point(Vertex);
				}
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
