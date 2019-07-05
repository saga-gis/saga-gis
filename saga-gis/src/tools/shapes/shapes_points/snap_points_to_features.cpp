
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
	switch (Type)
	{
	default                : Set_Name(_TL("Snap Points to Points"  )); break;
	case SHAPE_TYPE_Line   : Set_Name(_TL("Snap Points to Lines"   )); break;
	case SHAPE_TYPE_Polygon: Set_Name(_TL("Snap Points to Polygons")); break;
	}

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		"", "INPUT"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		"", "SNAP"		, _TL("Snap Features"),
		_TL(""),
		PARAMETER_INPUT, Type
	);

	Parameters.Add_Shapes(
		"", "OUTPUT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		"", "MOVES"		, _TL("Moves"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Double(
		"", "DISTANCE"	, _TL("Search Distance"),
		_TL(""),
		0., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSnap_Points_to_Features::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pFeatures	= Parameters("SNAP")->asShapes();

	if( !pFeatures->is_Valid() || pFeatures->Get_Count() <= 1 )
	{
		Error_Set(_TL("invalid snap features"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pPoints	= Parameters("INPUT")->asShapes();

	if( !pPoints->is_Valid() || pPoints->Get_Count() < 1 )
	{
		Error_Set(_TL("invalid points layer"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pMoves	= Parameters("MOVES")->asShapes();

	if( pMoves )
	{
		pMoves->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s]", pPoints->Get_Name(), _TL("snap move")), pPoints);
	}

	//-----------------------------------------------------
	if( Parameters("OUTPUT")->asShapes() && Parameters("OUTPUT")->asShapes() != pPoints )
	{
		CSG_Shapes	*pOutput	= Parameters("OUTPUT")->asShapes();

		pOutput->Create(*pPoints);

		pOutput->Fmt_Name("%s [%s]", pPoints->Get_Name(), _TL("snapped"));

		pPoints	= pOutput;
	}

	//-----------------------------------------------------
	double	Distance	= Parameters("DISTANCE")->asDouble();

	bool	bDistance	= Distance > 0.0;

	if( !bDistance )
	{
		CSG_Rect	r(pPoints->Get_Extent());

		r.Union(pFeatures->Get_Extent());

		Distance	= SG_Get_Distance(r.Get_BottomRight(), r.Get_TopLeft());
	}

	//-----------------------------------------------------
	for(int iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		CSG_Shape	*pPoint		= pPoints->Get_Shape(iPoint);
		TSG_Point	Point		= pPoint->Get_Point(0), snap_Point;
		double		snap_Dist	= Distance;

		if( !bDistance || pFeatures->Select(CSG_Rect(Point.x - Distance, Point.y - Distance, Point.x + Distance, Point.y + Distance)) )
		{
			for(int i=0, n=bDistance?pFeatures->Get_Selection_Count():pFeatures->Get_Count(); i<n && 0.<snap_Dist; i++)
			{
				CSG_Shape	*pFeature	= bDistance ? pFeatures->Get_Selection(i) : pFeatures->Get_Shape(i);

				switch( pFeatures->Get_Type() )
				{
				default                : Snap_To_Point  (Point, pFeature, snap_Point, snap_Dist); break;
				case SHAPE_TYPE_Line   : Snap_To_Line   (Point, pFeature, snap_Point, snap_Dist); break;
				case SHAPE_TYPE_Polygon: Snap_To_Polygon(Point, pFeature, snap_Point, snap_Dist); break;
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

				pMove->Add_Point(     Point);
				pMove->Add_Point(snap_Point);
			}
		}
	}

	//-----------------------------------------------------
	if( bDistance )
	{
		pFeatures->Select();	// reset selection
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSnap_Points_to_Features::Snap_To_Point(const TSG_Point &Point, CSG_Shape *pFeature, TSG_Point &snap_Point, double &snap_Dist)
{
	double	d	= SG_Get_Distance(Point, pFeature->Get_Point(0));

	if( d < snap_Dist )
	{
		snap_Dist	= d;
		snap_Point	= pFeature->Get_Point(0);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSnap_Points_to_Features::Snap_To_Line(const TSG_Point &Point, CSG_Shape *pFeature, TSG_Point &snap_Point, double &snap_Dist)
{
	CSG_Rect	r(pFeature->Get_Extent());

	r.Inflate(snap_Dist, false);

	if( r.Contains(Point) )
	{
		for(int iPart=0; iPart<pFeature->Get_Part_Count(); iPart++)
		{
			TSG_Point	C, B, A	= pFeature->Get_Point(0, iPart);

			for(int iPoint=1; iPoint<pFeature->Get_Point_Count(iPart); iPoint++)
			{
				B	= A;
				A	= pFeature->Get_Point(iPoint, iPart);

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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSnap_Points_to_Features::Snap_To_Polygon(const TSG_Point &Point, CSG_Shape *pFeature, TSG_Point &snap_Point, double &snap_Dist)
{
	CSG_Rect	r(pFeature->Get_Extent());

	r.Inflate(snap_Dist, false);

	if( r.Contains(Point) )
	{
		for(int iPart=0; iPart<pFeature->Get_Part_Count(); iPart++)
		{
			TSG_Point	C, B, A	= pFeature->Get_Point(0, iPart, false);

			for(int iPoint=0; iPoint<pFeature->Get_Point_Count(iPart); iPoint++)
			{
				B	= A;
				A	= pFeature->Get_Point(iPoint, iPart);

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
