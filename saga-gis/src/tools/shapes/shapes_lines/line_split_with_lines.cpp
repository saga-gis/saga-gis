
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
//               line_split_with_lines.cpp               //
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
#include "line_split_with_lines.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Split_with_Lines::CLine_Split_with_Lines(void)
{
	Set_Name		(_TL("Split Lines with Lines"));

	Set_Author		("O. Conrad (c) 2014");

	Set_Description	(_TW(
		"The tool allows one to split lines with lines."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LINES"		, _TL("Lines"),
		_TL("The input lines to split."),
		PARAMETER_INPUT , SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"SPLIT"		, _TL("Split Features"),
		_TL("The lines with which the input lines are to be divided."),
		PARAMETER_INPUT , SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"INTERSECT"	, _TL("Intersection"),
		_TL("The splitted lines."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice("",
		"OUTPUT"	, _TL("Output"),
		_TL("Choose the output line type, either polylines or separate lines."),
		CSG_String::Format("%s|%s",
			_TL("polylines"),
			_TL("separate lines")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Split_with_Lines::On_Execute(void)
{
	CSG_Shapes	*pLines		= Parameters("LINES"    )->asShapes();
	CSG_Shapes	*pSplit		= Parameters("SPLIT"    )->asShapes();
	CSG_Shapes	*pIntersect	= Parameters("INTERSECT")->asShapes();

	if(	!pLines->is_Valid() || pLines->Get_Count() < 1
	||	!pSplit->is_Valid() || pSplit->Get_Count() < 1
	||	pLines->Get_Extent().Intersects(pSplit->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("no lines for splitting"));

		return( false );
	}

	//--------------------------------------------------------
	pIntersect->Create(SHAPE_TYPE_Line,
		CSG_String::Format("%s [%s: %s]", pLines->Get_Name(), _TL("Split"), pSplit->Get_Name()),
		pLines
	);

	//--------------------------------------------------------
	for(sLong iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shape_Line	*pLine	= (CSG_Shape_Line *)pIntersect->Add_Shape(pLines->Get_Shape(iLine), SHAPE_COPY);

		for(sLong iSplit=0; iSplit<pSplit->Get_Count(); iSplit++)
		{
			CSG_Shape_Line	*pSplit_Line	= (CSG_Shape_Line *)pSplit->Get_Shape(iSplit);

			if( pLine->Intersects(pSplit_Line) )
			{
				for(int iPart=0; iPart<pSplit_Line->Get_Part_Count(); iPart++)
				{
					if( pLine->Intersects(pSplit_Line->Get_Extent()) )
					{
						Get_Intersection(pLine, pSplit_Line->Get_Part(iPart));
					}
				}
			}
		}

		if( Parameters("OUTPUT")->asInt() == 1 )
		{
			for(int iPart=pLine->Get_Part_Count()-1; iPart>=1; iPart--)
			{
				CSG_Shape_Line	*pAdd	= (CSG_Shape_Line *)pIntersect->Add_Shape(pLine, SHAPE_COPY_ATTR);	// only attributes

				pAdd->Add_Part(pLine->Get_Part(iPart));

				pLine->Del_Part(iPart);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Split_with_Lines::Get_Intersection(CSG_Shape_Line *pLine, CSG_Shape_Part *pSplit)
{
	CSG_Shapes	New(SHAPE_TYPE_Line);
	CSG_Shape	*pNew	= New.Add_Shape();

	for(int iPart=0, jPart=0; iPart<pLine->Get_Part_Count(); iPart++, jPart++)
	{
		TSG_Point	A[2], B[2], C;

		pNew->Add_Point(A[0] = pLine->Get_Point(0, iPart), jPart);

		for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
		{
			A[1]	= A[0];
			A[0]	= pLine->Get_Point(iPoint, iPart);
			B[0]	= pSplit->Get_Point(0);

			for(int jPoint=1; jPoint<pSplit->Get_Count(); jPoint++)
			{
				B[1]	= B[0];
				B[0]	= pSplit->Get_Point(jPoint);

				if( SG_Get_Crossing(C, A[0], A[1], B[0], B[1], true) )
				{
					pNew->Add_Point(C, jPart++);
					pNew->Add_Point(C, jPart);
				}
			}

			pNew->Add_Point(A[0], jPart);
		}
	}

	if( pNew->Get_Part_Count() > pLine->Get_Part_Count() )
	{
		return( pLine->Assign(pNew, false) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Split_at_Points::CLine_Split_at_Points(void)
{
	Set_Name		(_TL("Split Lines at Points"));

	Set_Author		("O. Conrad, V. Wichmann (c) 2015-2023");

	Set_Description	(_TW(
		"The tool allows one to split lines at certain points. The points must be provided as point shapes.\n"
		"The order in which the input lines are stored is retained, i.e. splitted parts are inserted at their "
		"original location in the dataset. "
		"By using a minimum vertex distance, a splitting close to existing line vertex locations can be avoided."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LINES"		, _TL("Lines"),
		_TL("The input lines to split."),
		PARAMETER_INPUT , SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"SPLIT"		, _TL("Split Features"),
		_TL("The points at which the input lines are to be split."),
		PARAMETER_INPUT , SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes("",
		"INTERSECT"	, _TL("Intersection"),
		_TL("The splitted lines."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice("",
		"OUTPUT"	, _TL("Output"),
		_TL("Choose the output line type, either polylines or separate lines."),
		CSG_String::Format("%s|%s",
			_TL("polylines"),
			_TL("separate lines")
		), 1
	);

	Parameters.Add_Double("",
		"EPSILON"	, _TL("Epsilon"),
		_TL("The tolerance used to find the point-line intersections [map units]."),
		0., true
	);

	Parameters.Add_Double("",
		"MIN_VERTEX_DIST"	, _TL("Minimum Vertex Distance"),
		_TL("The minimum distance of a point to a line vertex in order to split the line at the point's location [map units]."),
		0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Split_at_Points::On_Execute(void)
{
	CSG_Shapes	*pLines		= Parameters("LINES"    )->asShapes();
	CSG_Shapes	*pPoints	= Parameters("SPLIT"    )->asShapes();
	CSG_Shapes	*pIntersect	= Parameters("INTERSECT")->asShapes();

	if(	!pLines ->is_Valid() || pLines ->Get_Count() < 1
	||	!pPoints->is_Valid() || pPoints->Get_Count() < 1
	||	pLines->Get_Extent().Intersects(pPoints->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("no lines for splitting"));

		return( false );
	}

	//--------------------------------------------------------
	pIntersect->Create(SHAPE_TYPE_Line,
		CSG_String::Format("%s [%s: %s]", pLines->Get_Name(), _TL("Split"), pPoints->Get_Name()),
		pLines
	);

	double	Epsilon			= Parameters("EPSILON")->asDouble();
	double  Min_Vertex_Dist	= Parameters("MIN_VERTEX_DIST")->asDouble();

	//--------------------------------------------------------
	for(sLong iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shape_Line	*pLine	= (CSG_Shape_Line *)pLines->Get_Shape(iLine);
		
		CSG_Rect	Extent	= pLine->Get_Extent();	Extent.Inflate(Epsilon, false);

		std::vector<L_PART>	Line_Parts;

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			CSG_Shape_Part	*pPart = pLine->Get_Part(iPart);

			L_PART	part;

			for(int iPoint=0; iPoint<pPart->Get_Count(); iPoint++)
			{
				part.p.push_back(pPart->Get_Point(iPoint));
			}

			Line_Parts.push_back(part);
		}

		for(int iPoint=0; iPoint<pPoints->Get_Count(); iPoint++)
		{
			TSG_Point	Point	= pPoints->Get_Shape(iPoint)->Get_Point(0);

			if( Extent.Contains(Point) )
			{
				Get_Intersection(Line_Parts, Point, Epsilon, Min_Vertex_Dist);
			}
		}

		if( Parameters("OUTPUT")->asInt() == 1 ) // separate lines
		{
			for(size_t iPart=0; iPart<Line_Parts.size(); iPart++)
			{
				CSG_Shape_Line	*pLineOut	= (CSG_Shape_Line *)pIntersect->Add_Shape(pLines->Get_Shape(iLine), SHAPE_COPY_ATTR);

				for(size_t iPoint=0; iPoint<Line_Parts[iPart].p.size(); iPoint++)
				{
					pLineOut->Add_Point(Line_Parts[iPart].p[iPoint]);
				}
			}
		}
		else // polylines
		{
			CSG_Shape_Line	*pLineOut	= (CSG_Shape_Line *)pIntersect->Add_Shape(pLines->Get_Shape(iLine), SHAPE_COPY_ATTR);

			for(int iPart=0; (int)iPart<Line_Parts.size(); iPart++)
			{
				for(size_t iPoint=0; iPoint<Line_Parts[iPart].p.size(); iPoint++)
				{
					pLineOut->Add_Point(Line_Parts[iPart].p[iPoint], iPart);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Split_at_Points::Get_Intersection(std::vector<L_PART> &Line_Parts, TSG_Point Point, double Epsilon, double Min_Vertex_Dist)
{
	for(size_t iPart=0; iPart<Line_Parts.size(); iPart++)
	{
		TSG_Point	min_C, C, B, A	= Line_Parts[iPart].p[0];

		int min_iPoint = 0; double min_Dist = 1. + Epsilon;

		for(int iPoint=1; iPoint<(int)Line_Parts[iPart].p.size(); iPoint++)
		{
			B = A; A = Line_Parts[iPart].p[iPoint];

			double	Dist	= SG_Get_Nearest_Point_On_Line(Point, A, B, C, true);

			if( min_Dist > Dist && Dist >= 0. )
			{
				if( Min_Vertex_Dist > 0. )
				{
					if( SG_Get_Distance(Point, A) < Min_Vertex_Dist || SG_Get_Distance(Point, B) < Min_Vertex_Dist )
					{
						continue;
					}
				}

				min_Dist	= Dist;
				min_iPoint	= iPoint;
				min_C		= C;
			}
		}

		if( min_Dist <= Epsilon )
		{
			L_PART	part;
			
			part.p.push_back(min_C);

			for(size_t iPoint=min_iPoint; iPoint<Line_Parts[iPart].p.size(); iPoint++)
			{
				part.p.push_back(Line_Parts[iPart].p[iPoint]);
			}

			Line_Parts[iPart].p.erase(Line_Parts[iPart].p.begin() + min_iPoint, Line_Parts[iPart].p.end());

			Line_Parts[iPart].p.push_back(min_C);

			iPart++;

			Line_Parts.insert(Line_Parts.begin() + iPart, part);
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
