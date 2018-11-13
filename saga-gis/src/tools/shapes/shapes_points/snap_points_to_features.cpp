/**********************************************************
 * Version $Id: snap_points_to_features.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_points                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              snap_points_to_features.cpp              //
//                                                       //
//                 Copyright (C) 2012 by                 //
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "snap_points_to_features.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSnap_Points_to_Features::CSnap_Points_to_Features(TSG_Shape_Type Type)
{
	//-----------------------------------------------------
	Set_Name		(Type == SHAPE_TYPE_Point
		? _TL("Snap Points to Points")
		: _TL("Snap Points to Lines")
	);

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "INPUT"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "SNAP"		, _TL("Snap Features"),
		_TL(""),
		PARAMETER_INPUT, Type
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "MOVES"		, _TL("Moves"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Value(
		NULL	, "DISTANCE"	, _TL("Search Distance"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSnap_Points_to_Features::On_Execute(void)
{
	double		Distance;
	CSG_Shapes	*pInput, *pPoints, *pSnap, *pMoves;

	//-----------------------------------------------------
	pInput		= Parameters("INPUT"   )->asShapes();
	pPoints		= Parameters("OUTPUT"  )->asShapes();
	pSnap		= Parameters("SNAP"    )->asShapes();
	pMoves		= Parameters("MOVES"   )->asShapes();
	Distance	= Parameters("DISTANCE")->asDouble();

	//-----------------------------------------------------
	if( !pInput->is_Valid() )
	{
		Error_Set(_TL("invalid points layer"));

		return( false );
	}

	if( !pSnap->is_Valid() || pSnap->Get_Count() <= 0 )
	{
		Error_Set(_TL("invalid snap features"));

		return( false );
	}

	//-----------------------------------------------------
	if( pPoints && pPoints != pInput )
	{
		pPoints->Create(*pInput);
	}
	else
	{
		Parameters("RESULT")->Set_Value(pPoints	= pInput);
	}

	pPoints->Fmt_Name("%s [%s: %s]", pInput->Get_Name(), _TL("snapped"), pSnap->Get_Name());

	if( pMoves )
	{
		pMoves->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s: %s]", pInput->Get_Name(), _TL("snap move"), pSnap->Get_Name()), pPoints);
	}

	//-----------------------------------------------------
	bool	bDistance	= Distance > 0.0;

	if( !bDistance )
	{
		CSG_Rect	r(pPoints->Get_Extent());

		r.Union(pSnap->Get_Extent());

		Distance	= SG_Get_Distance(r.Get_BottomRight(), r.Get_TopLeft());
	}

	//-----------------------------------------------------
	for(int iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		CSG_Shape	*pPoint		= pPoints->Get_Shape(iPoint);
		TSG_Point	Point		= pPoint->Get_Point(0), snap_Point;
		double		snap_Dist	= Distance;

		if( bDistance )
		{
			if( pSnap->Select(CSG_Rect(Point.x - Distance, Point.y - Distance, Point.x + Distance, Point.y + Distance)) )
			{
				for(int i=0; i<pSnap->Get_Selection_Count() && snap_Dist>0.0; i++)
				{
					if( pSnap->Get_Type() == SHAPE_TYPE_Point )
					{
						Snap_To_Point(Point, pSnap->Get_Selection(i), snap_Point, snap_Dist);
					}
					else
					{
						Snap_To_Line (Point, pSnap->Get_Selection(i), snap_Point, snap_Dist);
					}
				}
			}
		}
		else
		{
			for(int i=0; i<pSnap->Get_Count() && snap_Dist>0.0; i++)
			{
				if( pSnap->Get_Type() == SHAPE_TYPE_Point )
				{
					Snap_To_Point(Point, pSnap->Get_Shape(i), snap_Point, snap_Dist);
				}
				else
				{
					Snap_To_Line (Point, pSnap->Get_Shape(i), snap_Point, snap_Dist);
				}
			}
		}

		//-------------------------------------------------
		if( snap_Dist < Distance )
		{
			pPoint->Set_Point(snap_Point, 0);

			if( pMoves )
			{
				CSG_Shape	*pMove	= pMoves->Add_Shape(pPoint, SHAPE_COPY_ATTR);

				pMove->Add_Point(Point);
				pMove->Add_Point(snap_Point);
			}
		}
	}

	//-----------------------------------------------------
	if( bDistance )
	{
		pSnap->Select();	// reset selection
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSnap_Points_to_Features::Snap_To_Point(const TSG_Point &Point, CSG_Shape *pPoint, TSG_Point &snap_Point, double &snap_Dist)
{
	double	d	= SG_Get_Distance(Point, pPoint->Get_Point(0));

	if( d < snap_Dist )
	{
		snap_Dist	= d;
		snap_Point	= pPoint->Get_Point(0);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSnap_Points_to_Features::Snap_To_Line(const TSG_Point &Point, CSG_Shape *pLine, TSG_Point &snap_Point, double &snap_Dist)
{
	CSG_Rect	r(pLine->Get_Extent());

	r.Inflate(snap_Dist, false);

	if( r.Contains(Point) )
	{
		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			TSG_Point	C, B, A	= pLine->Get_Point(0, iPart);

			for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				B	= A;
				A	= pLine->Get_Point(iPoint, iPart);

				r.Assign(A, B);
				r.Inflate(snap_Dist, false);

				if( r.Contains(Point) )
				{
					double	d	= SG_Get_Nearest_Point_On_Line(Point, A, B, C, true);

					if( d >= 0.0 && d < snap_Dist )
					{
						snap_Dist	= d;
						snap_Point	= C;
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
