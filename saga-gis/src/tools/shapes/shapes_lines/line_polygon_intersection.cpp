
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
		"Line-polygon intersection."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		"", "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		"", "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		"", "INTERSECT"	, _TL("Intersection"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		"", "DIFFERENCE", _TL("Difference"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice(
		"", "ATTRIBUTES", _TL("Attributes"),
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
	CSG_Shapes	*pIntersection	= Parameters("INTERSECT" )->asShapes();
	CSG_Shapes	*pDifference	= Parameters("DIFFERENCE")->asShapes();

	int	Attributes	= Parameters("ATTRIBUTES")->asInt();

	pIntersection->Create(SHAPE_TYPE_Line, NULL, Attributes == 0 ? pPolygons : pLines, pLines->Get_Vertex_Type());
	pIntersection->Fmt_Name("%s [%s: %s]", pLines->Get_Name(), _TL("Intersection"), pPolygons->Get_Name());

	if( pDifference )
	{
		pDifference->Create(SHAPE_TYPE_Line, NULL, pLines, pLines->Get_Vertex_Type());
		pDifference->Fmt_Name("%s [%s: %s]", pLines->Get_Name(), _TL("Difference"), pPolygons->Get_Name());
	}

	if( Attributes == 2 )
	{
		for(int iField=0; iField<pPolygons->Get_Field_Count(); iField++)
		{
			pIntersection->Add_Field(pPolygons->Get_Field_Name(iField), pPolygons->Get_Field_Type(iField));
		}
	}

	if( !pLines->Get_Extent().Intersects(pPolygons->Get_Extent()) )	// no intersection >> difference == lines
	{
		return( pDifference ? pDifference->Assign(pLines) : true );
	}

	//-----------------------------------------------------
	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shapes	Intersection(SHAPE_TYPE_Line, NULL, pLines, pLines->Get_Vertex_Type());
		CSG_Shapes	Difference  (SHAPE_TYPE_Line, NULL, pLines, pLines->Get_Vertex_Type());

		CSG_Shape	*pLine	= Difference.Add_Shape(pLines->Get_Shape(iLine));

		for(int iPolygon=0; pLine->Get_Part_Count() > 0 && iPolygon<pPolygons->Get_Count(); iPolygon++)
		{
			CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

			if( Get_Intersection(pPolygon, pLine, Intersection) )
			{
				if( Attributes == 0 )	// inherit attributes from polygon, create one polyline from intersection segments
				{
					CSG_Shape	*pNew	= pIntersection->Add_Shape(pPolygon, SHAPE_COPY_ATTR);

					for(int iSegment=0; iSegment<Intersection.Get_Count(); iSegment++)
					{
						CSG_Shape_Line	*pParts	= (CSG_Shape_Line *)Intersection.Get_Shape(iSegment);

						for(int iPart=0; iPart<pParts->Get_Part_Count(); iPart++)
						{
							pNew->Add_Part(pParts->Get_Part(iPart));
						}
					}
				}
				else					// keep original line attributes
				{
					for(int iSegment=0; iSegment<Intersection.Get_Count(); iSegment++)
					{
						CSG_Shape	*pNew	= pIntersection->Add_Shape(Intersection.Get_Shape(iSegment));

						if( Attributes == 2 )
						{
							for(int iField=0, jField=pLines->Get_Field_Count(); iField<pPolygons->Get_Field_Count(); iField++, jField++)
							{
								*pNew->Get_Value(jField)	= *pPolygon->Get_Value(iField);
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
bool CLine_Polygon_Intersection::Get_Intersection(CSG_Shape_Polygon *pPolygon, CSG_Shape *pLine, CSG_Shapes &Intersection)
{
	if( !pLine->Intersects(pPolygon) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	Segments(SHAPE_TYPE_Line, NULL, (CSG_Table *)0, Intersection.Get_Vertex_Type());

	CSG_Table	Crossings;

	Crossings.Add_Field("X", SG_DATATYPE_Double);
	Crossings.Add_Field("Y", SG_DATATYPE_Double);
	Crossings.Add_Field("D", SG_DATATYPE_Double);

    if( Intersection.Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
    {
        Crossings.Add_Field("Z", SG_DATATYPE_Double);
        Crossings.Add_Field("M", SG_DATATYPE_Double);
    }

    ZM          zmValues;

	//-----------------------------------------------------
	for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
	{
		if( pLine->Get_Point_Count(iPart) < 2 )
		{
			continue;
		}

		TSG_Point	A	= pLine->Get_Point(0, iPart);

        if( pLine->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
        {
            _Get_ZM(pLine, 0, iPart, zmValues.Az, zmValues.Am);
        }

		CSG_Shape	*pSegment	= Segments.Add_Shape();
		
		pSegment->Add_Point(A);

        if( Intersection.Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
        {
            _Set_ZM(pSegment, pSegment->Get_Point_Count() - 1, 0, zmValues.Az, zmValues.Am);
        }

		for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	B = A; A = pLine->Get_Point(iPoint, iPart);

            if( Intersection.Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
            {
                zmValues.Bz = zmValues.Az; zmValues.Az = pLine->Get_Z(iPoint, iPart);

                if( Intersection.Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
                {
                    zmValues.Bm = zmValues.Am; zmValues.Am = pLine->Get_M(iPoint, iPart);
                }
            }

			if( A.x == B.x && A.y == B.y )
			{
				continue;
			}

			if( Get_Crossings(pPolygon, A, B, Crossings, Intersection.Get_Vertex_Type(), zmValues) > 0 )
			{
				for(int iCrossing=0; iCrossing<Crossings.Get_Count(); iCrossing++)
				{
					B.x	= Crossings[iCrossing].asDouble(0);
					B.y	= Crossings[iCrossing].asDouble(1);

					pSegment->Add_Point(B);

                    if( Intersection.Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
                    {
                        double z = Crossings[iCrossing].asDouble(3);
                        double m = Crossings[iCrossing].asDouble(4);

                        _Set_ZM(pSegment, pSegment->Get_Point_Count() - 1, 0, z, m);
                    }

					pSegment = Segments.Add_Shape();
					pSegment->Add_Point(B);

                    if( Intersection.Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
                    {
                        double z = Crossings[iCrossing].asDouble(3);
                        double m = Crossings[iCrossing].asDouble(4);

                        _Set_ZM(pSegment, pSegment->Get_Point_Count() - 1, 0, z, m);
                    }
				}
			}

			pSegment->Add_Point(A);

            if( Intersection.Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
            {
                _Set_ZM(pSegment, pSegment->Get_Point_Count() - 1, 0, zmValues.Az, zmValues.Am);
            }
		}
	}

	//-----------------------------------------------------
	Intersection.Del_Records();

	pLine->Del_Parts();

	for(int iSegment=0; iSegment<Segments.Get_Count(); iSegment++)
	{
		CSG_Shape_Line	*pSegment	= (CSG_Shape_Line *)Segments.Get_Shape(iSegment);

		if( pSegment->Get_Length(0) > 0 )
		{
			TSG_Point	A	= pSegment->Get_Point(0);

			if( pPolygon->is_OnEdge(A) )
			{
				TSG_Point	B	= pSegment->Get_Point(1);

				A.x	+= B.x - A.x;
				A.y	+= B.y - A.y;
			}

			if( pPolygon->Contains(A) )
			{
				Intersection.Add_Shape(pLine, SHAPE_COPY_ATTR)->Add_Part(pSegment->Get_Part(0));
			}
			else
			{
				pLine->Add_Part(pSegment->Get_Part(0));
			}
		}
	}

	//-----------------------------------------------------
	return( Intersection.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLine_Polygon_Intersection::Get_Crossings(CSG_Shape_Polygon *pPolygon, const TSG_Point &a, const TSG_Point &b, CSG_Table &Crossings, TSG_Vertex_Type VertexType, ZM &zmValues)
{
    double dz = 0.0, dm = 0.0;

    if( VertexType != SG_VERTEX_TYPE_XY )
    {
        double dLength = SG_Get_Distance(a, b);

        if( dLength > 0.0 )
        {
            dz = (zmValues.Az - zmValues.Bz) / dLength;

            if( VertexType == SG_VERTEX_TYPE_XYZM )
            {
                dm = (zmValues.Am - zmValues.Bm) / dLength;
            }
        }
    }

	Crossings.Del_Records();

	for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		TSG_Point	A	= pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);

		for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	C, B = A; A = pPolygon->Get_Point(iPoint, iPart);

			if( SG_Get_Crossing(C, A, B, a, b) )
			{
                double dDist = SG_Get_Distance(b, C);

				CSG_Table_Record	*pCrossing	= Crossings.Add_Record();

				pCrossing->Set_Value(0, C.x);
				pCrossing->Set_Value(1, C.y);
				pCrossing->Set_Value(2, dDist);

                if( VertexType != SG_VERTEX_TYPE_XY )
                {
                    pCrossing->Set_Value(3, zmValues.Bz + dDist * dz);

                    if( VertexType == SG_VERTEX_TYPE_XYZM )
                    {
                        pCrossing->Set_Value(4, zmValues.Bm + dDist * dm);
                    }
                }
			}
		}
	}

	Crossings.Set_Index(2, TABLE_INDEX_Ascending);

	return( Crossings.Get_Count() );
}

//---------------------------------------------------------
void CLine_Polygon_Intersection::_Get_ZM(CSG_Shape *pLine, int iPoint, int iPart, double &z, double &m)
{
    z = pLine->Get_Z(iPoint, iPart);

    if( pLine->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
    {
        m = pLine->Get_M(iPoint, iPart);
    }

    return;
}

//---------------------------------------------------------
void CLine_Polygon_Intersection::_Set_ZM(CSG_Shape *pSegment, int iPoint, int iPart, double &z, double &m)
{
    pSegment->Set_Z(z, pSegment->Get_Point_Count() - 1, 0);

    if( pSegment->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
    {
        pSegment->Set_M(m, pSegment->Get_Point_Count() - 1, 0);
    }

    return;
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
