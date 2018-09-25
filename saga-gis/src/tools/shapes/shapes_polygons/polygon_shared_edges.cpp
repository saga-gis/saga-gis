/**********************************************************
 * Version $Id: polygon_shared_edges.cpp 911 2011-02-14 16:38:15Z reklov_w $
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	//-----------------------------------------------------
	Set_Name		(_TL("Shared Polygon Edges"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "ATTRIBUTE"	, _TL("Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Shapes(
		NULL	, "EDGES"		, _TL("Edges"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

//	Parameters.Add_Shapes(
//		NULL	, "NODES"		, _TL("Nodes"),
//		_TL(""),
//		PARAMETER_OUTPUT, SHAPE_TYPE_Point
//	);

	Parameters.Add_Value(
		NULL	, "EPSILON"		, _TL("Tolerance"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "VERTICES"	, _TL("Check Vertices"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "DOUBLE"		, _TL("Double Edges"),
		_TL("give output of an edge twice, i.e. once for each of the two adjacent polygons"),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Shared_Edges::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	m_Field		= Parameters("ATTRIBUTE")->asInt();

	m_pEdges	= Parameters("EDGES")->asShapes();
	m_pEdges->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s [%s]"), pPolygons->Get_Name(), _TL("Edges")));
	m_pEdges->Add_Field("ID_A", m_Field < 0 ? SG_DATATYPE_Int : pPolygons->Get_Field_Type(m_Field));
	m_pEdges->Add_Field("ID_B", m_Field < 0 ? SG_DATATYPE_Int : pPolygons->Get_Field_Type(m_Field));

//	m_pNodes	= Parameters("NODES")->asShapes();
//	m_pNodes->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pPolygons->Get_Name(), _TL("Nodes")));
//	m_pNodes->Add_Field("ID", SG_DATATYPE_Int);

	bool	bVertices	= Parameters("VERTICES")->asBool  ();
	double	Epsilon		= Parameters("EPSILON" )->asDouble();

	int	iPolygon, nAdded	= 0, nRemoved	= 0;

	//-----------------------------------------------------
	if( bVertices )
	{
		for(iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
		{
			CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

			for(int iPart=0; iPart<pPolygon->Get_Part_Count() && Process_Get_Okay(); iPart++)
			{
				CSG_Shape_Part	*pPart	= pPolygon->Get_Part(iPart);

				CSG_Point	A	= pPart->Get_Point(pPart->Get_Count() - 1);

				if( A != pPart->Get_Point(0) )
				{
					pPart->Add_Point(A);
				}

				for(int iPoint=pPart->Get_Count()-2; iPoint>=0; iPoint--)
				{
					CSG_Point	B	= A;	A	= pPart->Get_Point(iPoint);

					if( A == B )
					{
						pPart->Del_Point(iPoint + 1);

						nRemoved--;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	for(iPolygon=0; iPolygon<pPolygons->Get_Count()-1 && Set_Progress(iPolygon, pPolygons->Get_Count()-1); iPolygon++)
	{
		CSG_Shape_Polygon	*pA	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

		for(int jPolygon=iPolygon+1; jPolygon<pPolygons->Get_Count() && Process_Get_Okay(); jPolygon++)
		{
			CSG_Shape_Polygon	*pB	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(jPolygon);

			for(int iPart=0; iPart<pA->Get_Part_Count() && Process_Get_Okay(); iPart++)
			{
				for(int jPart=0; jPart<pB->Get_Part_Count() && Process_Get_Okay(); jPart++)
				{
					if( pA->Get_Part(iPart)->Get_Extent().Intersects(pB->Get_Part(jPart)->Get_Extent()) )
					{
						if( bVertices )
						{
							nAdded	+= Check_Vertices(pA->Get_Part(iPart), pB->Get_Part(jPart), Epsilon);
							nAdded	+= Check_Vertices(pB->Get_Part(jPart), pA->Get_Part(iPart), Epsilon);
						}

						Get_Shared_Edges(pA->Get_Part(iPart), pB->Get_Part(jPart), Epsilon);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Parameters("DOUBLE")->asBool() )
	{
		for(int iEdge=0, nEdges=m_pEdges->Get_Count(); iEdge<nEdges && Set_Progress(iEdge, nEdges); iEdge++)
		{
			CSG_Shape	*pA	= m_pEdges->Get_Shape(iEdge);
			CSG_Shape	*pB	= m_pEdges->Add_Shape(pA);

			*(pB->Get_Value(0))	= *(pA->Get_Value(1));
			*(pB->Get_Value(1))	= *(pA->Get_Value(0));
		}
	}

	//-----------------------------------------------------
	if( nAdded > 0 || nRemoved > 0 )
	{
		Message_Fmt("\n%s: %d %s, %d %s\n", _TL("Vertices"), nAdded, _TL("added"), nRemoved, _TL("removed"));

		DataObject_Update(pPolygons);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Shared_Edges::Get_Shared_Edges(CSG_Shape_Part *pA, CSG_Shape_Part *pB, double Epsilon)
{
	int	Edge_1st	= m_pEdges->Get_Count();

	CSG_Shape	*pEdge	= NULL;

	for(int iPoint=0, jPoint; iPoint<pA->Get_Count(); iPoint++)
	{
		CSG_Point	Point	= pA->Get_Point(iPoint);

		if( !pEdge )
		{
			for(jPoint=0; jPoint<pB->Get_Count(); jPoint++)
			{
				if( Point.is_Equal(pB->Get_Point(jPoint), Epsilon) )
				{
					pEdge	= m_pEdges->Add_Shape();
					pEdge	->Add_Point(Point);

					if( m_Field < 0 )
					{
						pEdge->Set_Value(0, pA->Get_Owner()->Get_Index());
						pEdge->Set_Value(1, pB->Get_Owner()->Get_Index());
					}
					else
					{
						pEdge->Set_Value(0, pA->Get_Owner()->asString(m_Field));
						pEdge->Set_Value(1, pB->Get_Owner()->asString(m_Field));
					}
					break;
				}
			}
		}
		else
		{
			int	j	= jPoint;

			if( Point.is_Equal(pB->Get_Point(jPoint = Get_Next_Vertex(pB, j, false)), Epsilon)
			||  Point.is_Equal(pB->Get_Point(jPoint = Get_Next_Vertex(pB, j, true )), Epsilon) )
			{
				pEdge->Add_Point(Point);
			}
			else
			{
				pEdge	= NULL;
			}
		}
	}

	//-----------------------------------------------------
	if( pEdge )
	{
		CSG_Shape	*pEdge_1st	= m_pEdges->Get_Shape(Edge_1st);

		if( pEdge != pEdge_1st && SG_Is_Equal(pA->Get_Point(0), pEdge_1st->Get_Point(0)) )
		{
			for(int i=0; i<pEdge_1st->Get_Point_Count(0); i++)
			{
				pEdge->Add_Point(pEdge_1st->Get_Point(i));
			}

			m_pEdges->Del_Shape(Edge_1st);
		}
	}

	//-----------------------------------------------------
	for(int iEdge=m_pEdges->Get_Count()-1; iEdge>=Edge_1st; iEdge--)
	{
		if( m_pEdges->Get_Shape(iEdge)->Get_Point_Count() <= 1 )	// touches at point
		{
			m_pEdges->Del_Shape(iEdge);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
int CPolygon_Shared_Edges::Get_Next_Vertex(CSG_Shape_Part *pPoints, int iPoint, bool bAscending)
{
	if( bAscending )
	{
		if( iPoint < pPoints->Get_Count() - 1 )
		{
			iPoint	++;
		}
		else
		{
			iPoint	= 1;
		}
	}
	else
	{
		if( iPoint > 0 )
		{
			iPoint	--;
		}
		else
		{
			iPoint	= pPoints->Get_Count() - 2;
		}
	}

	return( iPoint );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygon_Shared_Edges::Check_Vertices(CSG_Shape_Part *pPolygon, CSG_Shape_Part *pVertices, double Epsilon)
{
	int	nAdded	= 0;

	TSG_Point	A = pPolygon->Get_Point(0, false);

	for(int iPoint=0; iPoint<pPolygon->Get_Count(); iPoint++)
	{
		TSG_Point	B = A; A = pPolygon->Get_Point(iPoint);

		for(int iVertex=0; iVertex<pVertices->Get_Count(); iVertex++)
		{
			TSG_Point	Vertex	= pVertices->Get_Point(iVertex);

			if( (SG_Is_Between(Vertex.x, A.x, B.x, Epsilon) && SG_Is_Between(Vertex.y, A.y, B.y, Epsilon))
			&&  !SG_Is_Equal(Vertex, A, Epsilon) && !SG_Is_Equal(Vertex, B, Epsilon)
			&&  SG_Is_Point_On_Line(Vertex, A, B, true, Epsilon) )
			{
				pPolygon->Ins_Point(Vertex, iPoint++);

				nAdded++;
			}
		}
	}

	return( nAdded );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Vertex_Check::CPolygon_Vertex_Check(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Polygon Vertex Check"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "CHECKED"		, _TL("Checked"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "ADDED"		, _TL("Added"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Value(
		NULL	, "EPSILON"		, _TL("Tolerance"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Vertex_Check::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if( Parameters("CHECKED")->asShapes() && Parameters("CHECKED")->asShapes() != pPolygons )
	{
		CSG_Shapes	*pCopy	= Parameters("CHECKED")->asShapes();

		pCopy->Create(*pPolygons);
		pCopy->Fmt_Name("%s [%s]", pPolygons->Get_Name(), _TL("checked"));

		pPolygons	= pCopy;
	}

	if( (m_pAdded = Parameters("ADDED")->asShapes()) != NULL )
	{
		m_pAdded->Create(SHAPE_TYPE_Point, _TL("Added"));
	}

	double	Epsilon	= Parameters("EPSILON")->asDouble();

	//-----------------------------------------------------
	for(int iPolygon=0; iPolygon<pPolygons->Get_Count()-1 && Set_Progress(iPolygon, pPolygons->Get_Count()-1); iPolygon++)
	{
		CSG_Shape_Polygon	*pA	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

		for(int jPolygon=iPolygon+1; jPolygon<pPolygons->Get_Count() && Process_Get_Okay(); jPolygon++)
		{
			CSG_Shape_Polygon	*pB	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(jPolygon);

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
	TSG_Point	A = pPolygon->Get_Point(0, false);

	for(int iPoint=0; iPoint<pPolygon->Get_Count(); iPoint++)
	{
		TSG_Point	B = A; A = pPolygon->Get_Point(iPoint);

		for(int iVertex=0; iVertex<pVertices->Get_Count(); iVertex++)
		{
			TSG_Point	Vertex	= pVertices->Get_Point(iVertex);

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
