
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             line_polygon_intersection.cpp             //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#include "line_polygon_intersection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Polygon_Intersection::CLine_Polygon_Intersection(void)
{
	Set_Name		(_TL("Line-Polygon Intersection"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Line-polygon intersection. Splits lines with polygon arcs. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("", "LINES"     , _TL("Lines"       ), _TL(""), PARAMETER_INPUT , SHAPE_TYPE_Line   );
	Parameters.Add_Shapes("", "POLYGONS"  , _TL("Polygons"    ), _TL(""), PARAMETER_INPUT , SHAPE_TYPE_Polygon);

	Parameters.Add_Shapes("", "INTERSECT" , _TL("Intersection"), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Line   );
	Parameters.Add_Shapes("", "DIFFERENCE", _TL("Difference"  ), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Line   );

	Parameters.Add_Choice("",
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("intersection"),
			_TL("difference"),
			_TL("intersection and difference")
		), 2
	);

	Parameters.Add_Choice("",
		"ATTRIBUTES", _TL("Attributes"),
		_TL("attributes inherited to intersection result"),
		CSG_String::Format("%s|%s|%s",
			_TL("polygon"),
			_TL("line"),
			_TL("line and polygon")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLine_Polygon_Intersection::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("OUTPUT") )
	{
		pParameters->Set_Enabled("INTERSECT" , pParameter->asInt() == 2 || pParameter->asInt() == 0);
		pParameters->Set_Enabled("ATTRIBUTES", pParameter->asInt() == 2 || pParameter->asInt() == 0);

		pParameters->Set_Enabled("DIFFERENCE", pParameter->asInt() == 2 || pParameter->asInt() == 1);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Polygon_Intersection::On_Execute(void)
{
	CSG_Shapes	*pLines	= Parameters("LINES")->asShapes();

	if(	pLines->Get_Count() < 1 )
	{
		Error_Set(_TL("no features in lines layer"));

		return( false );
	}

	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if(	pPolygons->Get_Count() < 1 )
	{
		Error_Set(_TL("no features in polygons layer"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes *pIntersection = Parameters("INTERSECT")->is_Enabled() ? Parameters("INTERSECT")->asShapes() : NULL;

	int	Attributes	= Parameters("ATTRIBUTES")->asInt();

	if( pIntersection )
	{
		pIntersection->Create(SHAPE_TYPE_Line, NULL, Attributes == 0 ? pPolygons : pLines, pLines->Get_Vertex_Type());
		pIntersection->Fmt_Name("%s [%s: %s]", pLines->Get_Name(), _TL("Intersection"), pPolygons->Get_Name());

		if( Attributes == 2 )
		{
			for(int iField=0; iField<pPolygons->Get_Field_Count(); iField++)
			{
				pIntersection->Add_Field(pPolygons->Get_Field_Name(iField), pPolygons->Get_Field_Type(iField));
			}
		}
	}

	//-----------------------------------------------------
	CSG_Shapes *pDifference = Parameters("DIFFERENCE")->is_Enabled() ? Parameters("DIFFERENCE")->asShapes() : NULL;

	if( pDifference )
	{
		pDifference->Create(SHAPE_TYPE_Line, NULL, pLines, pLines->Get_Vertex_Type());
		pDifference->Fmt_Name("%s [%s: %s]", pLines->Get_Name(), _TL("Difference"), pPolygons->Get_Name());
	}

	if( !pLines->Get_Extent().Intersects(pPolygons->Get_Extent()) )	// no intersection >> difference == lines
	{
		return( pDifference ? pDifference->Assign(pLines) : true );
	}

	//-----------------------------------------------------
	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shapes Intersection(SHAPE_TYPE_Line, NULL, pLines, pLines->Get_Vertex_Type());
		CSG_Shapes Difference  (SHAPE_TYPE_Line, NULL, pLines, pLines->Get_Vertex_Type());

		CSG_Shape *pLine = Difference.Add_Shape(pLines->Get_Shape(iLine));

		for(int iPolygon=0; pLine->Get_Part_Count() > 0 && iPolygon<pPolygons->Get_Count(); iPolygon++)
		{
			CSG_Shape_Polygon *pPolygon = pPolygons->Get_Shape(iPolygon)->asPolygon();

			if( Get_Intersection(pPolygon, pLine, Intersection) && pIntersection )
			{
				if( Attributes == 0 ) // inherit attributes from polygon only, create one polyline from intersection segments
				{
					CSG_Shape *pNew = pIntersection->Add_Shape(pPolygon, SHAPE_COPY_ATTR);

					for(int iSegment=0; iSegment<Intersection.Get_Count(); iSegment++)
					{
						CSG_Shape_Line *pParts = (CSG_Shape_Line *)Intersection.Get_Shape(iSegment);

						for(int iPart=0; iPart<pParts->Get_Part_Count(); iPart++)
						{
							pNew->Add_Part(pParts->Get_Part(iPart));
						}
					}
				}
				else                  // keep original line attributes
				{
					for(int iSegment=0; iSegment<Intersection.Get_Count(); iSegment++)
					{
						CSG_Shape *pNew = pIntersection->Add_Shape(Intersection.Get_Shape(iSegment));

						for(int iField=0; iField<pLines->Get_Field_Count(); iField++)
						{
							*pNew->Get_Value(iField) = *pLine->Get_Value(iField);
						}

						if( Attributes == 2 ) // ...and add polygon attributes
						{
							for(int iField=0, jField=pLines->Get_Field_Count(); iField<pPolygons->Get_Field_Count(); iField++, jField++)
							{
								*pNew->Get_Value(jField) = *pPolygon->Get_Value(iField);
							}
						}
					}
				}
			}
		}

		if( pDifference && pLine->is_Valid() )
		{
			pDifference->Add_Shape(pLine);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define Add_Vertex(p, d, c) { CSG_Shape &v = *Vertices.Add_Shape();\
	v.Add_Point(p); v.Set_Value(0, d); v.Set_Value(1, c);\
}

//---------------------------------------------------------
bool CLine_Polygon_Intersection::Get_Intersection(CSG_Shape_Polygon *pPolygon, CSG_Shape *pLine, CSG_Shapes &Intersection)
{
	if( !pLine->Intersects(pPolygon) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes Difference(SHAPE_TYPE_Line , NULL, NULL, Intersection.Get_Vertex_Type());
	CSG_Shapes Vertices  (SHAPE_TYPE_Point, NULL, NULL, Intersection.Get_Vertex_Type());

	Vertices.Add_Field("DISTANCE", SG_DATATYPE_Double);
	Vertices.Add_Field("CROSSING", SG_DATATYPE_Char  );

	Intersection.Del_Shapes();

	//-----------------------------------------------------
	for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
	{
		CSG_Shape_Part *pPart = pLine->Get_Part(iPart);

		if( pPart->Get_Count() < 2 )
		{
			continue;
		}

		//-------------------------------------------------
		TSG_Point_ZM Segment[2]; Segment[1] = pPart->Get_Point_ZM(0);
		
		double Distance = 0.; bool bCrossings = false;

		Add_Vertex(Segment[1], Distance, 0);

		for(int iPoint=1; iPoint<pPart->Get_Count(); iPoint++)
		{
			Segment[0] = Segment[1]; Segment[1] = pPart->Get_Point_ZM(iPoint);

			if( Segment[0].x != Segment[1].x || Segment[0].y != Segment[1].y )
			{
				if( Add_Crossings(pPolygon, Segment, Distance, Vertices) )
				{
					bCrossings = true;
				}

				Distance += SG_Get_Distance(Segment[0].x, Segment[0].y, Segment[1].x, Segment[1].y);

				Add_Vertex(Segment[1], Distance, 0);
			}
		}

		//-------------------------------------------------
		if( bCrossings == false )
		{
			if( pPolygon->Contains(pPart->Get_Point(0)) )
			{
				Intersection.Add_Shape()->Add_Part(pPart); // completely in
			}
			else
			{
				Difference  .Add_Shape()->Add_Part(pPart); // completely out
			}
		}
		else // if( bCrossings == true )
		{
			Vertices.Set_Index(0, TABLE_INDEX_Ascending);

			//---------------------------------------------
			for(int i=1, j=0; i<Vertices.Get_Count(); i++, j++) // check for vertices at polygon edges/vertices (vertex + one or two crossings)
			{
				CSG_Point a(Vertices.Get_Shape_byIndex(i)->Get_Point(0));
				CSG_Point b(Vertices.Get_Shape_byIndex(j)->Get_Point(0));

				if( a == b )
				{
					Vertices.Get_Shape_byIndex(j)->Set_Value(1, Vertices.Get_Shape_byIndex(i)->asInt(1)); // copy crossing flag
					Vertices.Get_Shape_byIndex(i)->Set_Value(1, -1); // invalidate vertex
				}
			}

			//---------------------------------------------
			bool bInterior = pPolygon->Contains(Vertices.Get_Shape_byIndex(0)->Get_Point(0));

			if( bInterior && Vertices.Get_Shape_byIndex(0)->asInt(1) ) // starts with crossing => first vertex is on polygon edge/vertex
			{
				CSG_Point First(Vertices.Get_Shape_byIndex(0)->Get_Point(0));

				for(int i=1; i<Vertices.Get_Count(); i++)
				{
					CSG_Point Next(Vertices.Get_Shape_byIndex(i)->Get_Point(0));

					if( First != Next )
					{
						bInterior = pPolygon->Contains(Next);

						break;
					}
				}
			}

			//---------------------------------------------
			CSG_Shape *pIntersection = Intersection.Add_Shape();
			CSG_Shape *pDifference   = Difference  .Add_Shape();

			for(int i=0, iIntersection=0, iDifference=0; i<Vertices.Get_Count(); i++)
			{
				CSG_Shape &Vertex = *Vertices.Get_Shape_byIndex(i);

				if( Vertex.asInt(1) < 0 ) // skip invalidated vertex
				{
					continue;
				}

				if( bInterior )
				{
					pIntersection->Add_Point(Vertex.Get_Point_ZM(0), iIntersection);

					if( Vertex.asInt(1) ) // Crossing
					{
						bInterior = false;

						if( pDifference->Get_Point_Count(iDifference) > 1 )
						{
							iDifference++;
						}
						else
						{
							pDifference->Del_Part(iDifference);
						}

						pDifference->Add_Point(Vertex.Get_Point_ZM(0), iDifference);
					}
				}
				else // if( bInterior == false )
				{
					pDifference->Add_Point(Vertex.Get_Point_ZM(0), iDifference);

					if( Vertex.asInt(1) ) // Crossing
					{
						bInterior = true;

						if( pIntersection->Get_Point_Count(iIntersection) > 1 )
						{
							iIntersection++;
						}
						else
						{
							pIntersection->Del_Part(iIntersection);
						}

						pIntersection->Add_Point(Vertex.Get_Point_ZM(0), iIntersection);
					}
				}
			}
		}

		Vertices.Del_Shapes();
	}

	//-----------------------------------------------------
	if( Intersection.Get_Count() > 0 )
	{
		pLine->Del_Parts();

		for(int i=0; i<Difference.Get_Count(); i++)
		{
			CSG_Shape_Line *pDifference = (CSG_Shape_Line *)Difference.Get_Shape(i);

			for(int iPart=0; iPart<pDifference->Get_Part_Count(); iPart++)
			{
				if( pDifference->Get_Point_Count(iPart) > 1 && pDifference->Get_Length(iPart) > 0. )
				{
					pLine->Add_Part(pDifference->Get_Part(iPart));
				}
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Polygon_Intersection::Add_Crossings(CSG_Shape_Polygon *pPolygon, const TSG_Point_ZM Segment[2], double Distance, CSG_Shapes &Vertices)
{
	CSG_Point a(Segment[0].x, Segment[0].y);
	CSG_Point b(Segment[1].x, Segment[1].y);

	double Length = SG_Get_Distance(a, b);

	if( Length > 0. )
	{
		bool bCrossings = false;

		for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			TSG_Point A = pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);

			for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point C, B = A; A = pPolygon->Get_Point(iPoint, iPart);

				if( SG_Get_Crossing(C, A, B, a, b) )
				{
					bCrossings = true;

					TSG_Point_ZM c; double d = SG_Get_Distance(a, C);

					c.x = C.x;
					c.y = C.y;
					c.z = Segment[0].z + d * (Segment[1].z - Segment[0].z) / Length;
					c.m = Segment[0].m + d * (Segment[1].m - Segment[0].m) / Length;

					Add_Vertex(c, Distance + d, 1);
				}
			}
		}

		return( bCrossings );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
