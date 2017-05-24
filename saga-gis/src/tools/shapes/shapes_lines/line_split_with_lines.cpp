/**********************************************************
 * Version $Id: line_split_with_lines.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	//-----------------------------------------------------
	Set_Name		(_TL("Split Lines with Lines"));

	Set_Author		(SG_T("O. Conrad (c) 2014"));

	Set_Description	(_TW(
		"Split Lines with Lines."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "SPLIT"		, _TL("Split Features"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "INTERSECT"	, _TL("Intersection"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
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
	CSG_Shapes	*pLines, *pSplit, *pIntersect;

	pLines		= Parameters("LINES"    )->asShapes();
	pSplit		= Parameters("SPLIT"    )->asShapes();
	pIntersect	= Parameters("INTERSECT")->asShapes();

	if(	!pLines->is_Valid() || pLines->Get_Count() < 1
	||	!pSplit->is_Valid() || pSplit->Get_Count() < 1
	||	pLines->Get_Extent().Intersects(pSplit->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("no lines for splitting"));

		return( false );
	}

	//--------------------------------------------------------
	pIntersect->Create(SHAPE_TYPE_Line,
		CSG_String::Format(SG_T("%s [%s: %s]"), pLines->Get_Name(), _TL("Split"), pSplit->Get_Name()),
		pLines
	);

	//--------------------------------------------------------
	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shape	*pLine	= pIntersect->Add_Shape(pLines->Get_Shape(iLine), SHAPE_COPY);

		for(int iSplit=0; iSplit<pSplit->Get_Count(); iSplit++)
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
			while( pLine->Get_Part_Count() > 1 )
			{
				CSG_Shape_Line	*pAdd	= (CSG_Shape_Line *)pIntersect->Add_Shape(pLine, SHAPE_COPY_ATTR);	// only attributes

				for(int iPoint=0; iPoint<pLine->Get_Point_Count(1); iPoint++)
				{
					pAdd->Add_Point(pLine->Get_Point(iPoint, 1));
				}

				pLine->Del_Part(1);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Split_with_Lines::Get_Intersection(CSG_Shape *pLine, CSG_Shape_Part *pSplit)
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
	//-----------------------------------------------------
	Set_Name		(_TL("Split Lines at Points"));

	Set_Author		(SG_T("O. Conrad (c) 2015"));

	Set_Description	(_TW(
		"Split Lines at Points."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "SPLIT"		, _TL("Split Features"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "INTERSECT"	, _TL("Intersection"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("polylines"),
			_TL("separate lines")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "EPSILON"		, _TL("Epsilon"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Split_at_Points::On_Execute(void)
{
	CSG_Shapes	*pLines, *pSplit, *pIntersect;

	pLines		= Parameters("LINES"    )->asShapes();
	pSplit		= Parameters("SPLIT"    )->asShapes();
	pIntersect	= Parameters("INTERSECT")->asShapes();

	if(	!pLines->is_Valid() || pLines->Get_Count() < 1
	||	!pSplit->is_Valid() || pSplit->Get_Count() < 1
	||	pLines->Get_Extent().Intersects(pSplit->Get_Extent()) == INTERSECTION_None )
	{
		Error_Set(_TL("no lines for splitting"));

		return( false );
	}

	//--------------------------------------------------------
	pIntersect->Create(SHAPE_TYPE_Line,
		CSG_String::Format(SG_T("%s [%s: %s]"), pLines->Get_Name(), _TL("Split"), pSplit->Get_Name()),
		pLines
	);

	double	Epsilon	= Parameters("EPSILON")->asDouble();

	//--------------------------------------------------------
	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shape	*pLine	= pIntersect->Add_Shape(pLines->Get_Shape(iLine), SHAPE_COPY);

		CSG_Rect	Extent	= pLine->Get_Extent();	Extent.Inflate(Epsilon, false);

		for(int iSplit=0; iSplit<pSplit->Get_Count(); iSplit++)
		{
			TSG_Point	Point	= pSplit->Get_Shape(iSplit)->Get_Point(0);

			if( Extent.Contains(Point) )
			{
				Get_Intersection(pLine, Point, Epsilon);
			}
		}

		if( Parameters("OUTPUT")->asInt() == 1 )
		{
			while( pLine->Get_Part_Count() > 1 )
			{
				CSG_Shape_Line	*pAdd	= (CSG_Shape_Line *)pIntersect->Add_Shape(pLine, SHAPE_COPY_ATTR);	// only attributes

				for(int iPoint=0; iPoint<pLine->Get_Point_Count(1); iPoint++)
				{
					pAdd->Add_Point(pLine->Get_Point(iPoint, 1));
				}

				pLine->Del_Part(1);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Split_at_Points::Get_Intersection(CSG_Shape *pLine, TSG_Point Point, double Epsilon)
{
	int			min_iPart, min_iPoint;
	double		min_Dist	= 1.1 * Epsilon;
	TSG_Point	min_C;

	for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
	{
		TSG_Point	C, B, A	= pLine->Get_Point(0, iPart);

		for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
		{
			B	= A;	A	= pLine->Get_Point(iPoint, iPart);

			double	Dist	= SG_Get_Nearest_Point_On_Line(Point, A, B, C, true);

			if( Dist < min_Dist )
			{
				min_Dist	= Dist;
				min_C		= C;
				min_iPart	= iPart;
				min_iPoint	= iPoint;
			}
		}
	}

	if( min_Dist <= Epsilon )
	{
		int		iPoint;

		CSG_Shape_Part	*pPart	= ((CSG_Shape_Line *)pLine)->Get_Part(min_iPart);

		pLine->Add_Point(min_C, min_iPart = pLine->Get_Part_Count());

		for(iPoint=min_iPoint; iPoint<pPart->Get_Count(); iPoint++)
		{
			pLine->Add_Point(pPart->Get_Point(iPoint), min_iPart);
		}

		for(iPoint=pPart->Get_Count()-1; iPoint>=min_iPoint; iPoint--)
		{
			pPart->Del_Point(iPoint);
		}

		pPart->Add_Point(min_C);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
