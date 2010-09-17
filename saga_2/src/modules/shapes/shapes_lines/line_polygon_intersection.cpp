
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
#include "line_polygon_intersection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Polygon_Intersection::CLine_Polygon_Intersection(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Line-Polygon Intersection"));

	Set_Author		(SG_T("O. Conrad (c) 2010"));

	Set_Description	(_TW(
		"Line-polygon intersection."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "INTERSECT"	, _TL("Intersection"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Output"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("one multi-line per polygon"),
			_TL("keep original line attributes")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Polygon_Intersection::On_Execute(void)
{
	int			Method;
	CSG_Shapes	*pLines, *pPolygons, *pNew_Lines, New_Lines;

	pLines		= Parameters("LINES")		->asShapes();
	pPolygons	= Parameters("POLYGONS")	->asShapes();
	pNew_Lines	= Parameters("INTERSECT")	->asShapes();

	Method		= Parameters("METHOD")		->asInt();

	if(	!pLines   ->is_Valid() || pLines   ->Get_Count() < 1
	||	!pPolygons->is_Valid() || pPolygons->Get_Count() < 1
	||	pLines->Get_Extent().Intersects(pPolygons->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("no shapes for intersection found"));

		return( false );
	}

	//--------------------------------------------------------
	switch( Method )
	{
	case 0:
		pNew_Lines->Create(SHAPE_TYPE_Line,
			CSG_String::Format(SG_T("%s [%s: %s]"), pLines->Get_Name(), _TL("Intersection"), pPolygons->Get_Name()),
			pPolygons
		);
		break;

	case 1: default:
		pNew_Lines->Create(SHAPE_TYPE_Line,
			CSG_String::Format(SG_T("%s [%s: %s]"), pLines->Get_Name(), _TL("Intersection"), pPolygons->Get_Name()),
			pLines
		);
		break;
	}

	New_Lines.Create(SHAPE_TYPE_Line, NULL, pLines);

	//--------------------------------------------------------
	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		if( Get_Intersection((CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon), pLines, New_Lines) )
		{
			switch( Method )
			{
			case 0:
				{
					CSG_Shape	*pNew_Line	= pNew_Lines->Add_Shape(pPolygons->Get_Shape(iPolygon), SHAPE_COPY_ATTR);

					for(int iLine=0, jPart=0; iLine<New_Lines.Get_Count(); iLine++, jPart++)
					{
						CSG_Shape	*pLine	= New_Lines.Get_Shape(iLine);

						for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++, jPart++)
						{
							for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
							{
								pNew_Line->Add_Point(pLine->Get_Point(iPoint), jPart);
							}
						}
					}
				}
				break;

			case 1:
				{
					for(int iLine=0; iLine<New_Lines.Get_Count(); iLine++)
					{
						pNew_Lines->Add_Shape(New_Lines.Get_Shape(iLine));
					}
				}
				break;
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Polygon_Intersection::Get_Intersection(CSG_Shape_Polygon *pPolygon, CSG_Shapes *pLines, CSG_Shapes &New_Lines)
{
	if( !pLines->Select(pPolygon->Get_Extent()) )
	{
		return( false );
	}

	New_Lines.Del_Records();

	for(int iSelection=0; iSelection<pLines->Get_Selection_Count(); iSelection++)
	{
		CSG_Shape	*pNew_Line, *pLine	= pLines->Get_Selection(iSelection);

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			TSG_Point	Point	= pLine->Get_Point(0, iPart);

			if( pPolygon->is_Containing(Point) )
			{
				pNew_Line	= New_Lines.Add_Shape(pLine, SHAPE_COPY_ATTR);
				pNew_Line	->Add_Point(Point, iPart);
			}
			else
			{
				pNew_Line	= NULL;
			}

			for(int iPoint=1; iPoint<pLine->Get_Point_Count(); iPoint++)
			{
				Point	= pLine->Get_Point(iPoint, iPart);

				if( pNew_Line )
				{
					if( pPolygon->is_Containing(Point) )
					{
						pNew_Line	->Add_Point(Point, iPart);
					}
					else
					{
						Point		= Get_Intersection(pPolygon, Point, pLine->Get_Point(iPoint - 1, iPart));
						pNew_Line	->Add_Point(Point, iPart);
						pNew_Line	= NULL;
					}
				}
				else if( pPolygon->is_Containing(Point) )
				{
					Point		= Get_Intersection(pPolygon, Point, pLine->Get_Point(iPoint - 1, iPart));
					pNew_Line	= New_Lines.Add_Shape(pLine, SHAPE_COPY_ATTR);
					pNew_Line	->Add_Point(Point, iPart);
				}
			}
		}
	}

	return( New_Lines.Get_Count() > 0 );
}

//---------------------------------------------------------
TSG_Point CLine_Polygon_Intersection::Get_Intersection(CSG_Shape_Polygon *pPolygon, const TSG_Point &a, const TSG_Point &b)
{
	TSG_Point	c	= a;

	for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		TSG_Point	A, B;

		B	= pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);

		for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
		{
			A	= B;
			B	= pPolygon->Get_Point(iPoint, iPart);

			if( SG_Get_Crossing(c, A, B, a, b) )
			{
				return( c );
			}
		}
	}

	return( c );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
