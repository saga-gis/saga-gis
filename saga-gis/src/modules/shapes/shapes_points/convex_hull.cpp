/**********************************************************
 * Version $Id: remove_duplicates.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_points                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    convex_hull.cpp                    //
//                                                       //
//                 Copyright (C) 2011 by                 //
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "convex_hull.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CConvex_Hull::CConvex_Hull(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Convex Hull"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Implementation of 'Andrew's Monotone Chain Algorithm' for convex hull construction. "
		"\nReferences:\n"
		"Algorithmist (2011): <a target=\"_blank\" href=\"http://www.algorithmist.com/index.php/Monotone_Chain_Convex_Hull.cpp\">Monotone Chain Convex Hull</a>. algorithmist.com.\n"
		"Andrew, A.M. (1979): Another Efficient Algorithm for Convex Hulls in Two Dimensions. Info. Proc. Letters 9, pp.216-219.\n"
		"Sunday, D. (2001-2006): <a target=\"_blank\" href=\"http://www.softsurfer.com/Archive/algorithm_0109\">The Convex Hull of a 2D Point Set or Polygon</a>. Softsurfer.com.\n"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "HULLS"		, _TL("Convex Hull"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "BOXES"		, _TL("Minimum Bounding Box"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL	, "POLYPOINTS"	, _TL("Hull Construction"),
		_TL("This option does not apply to simple point layers."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("one hull for all shapes"),
			_TL("one hull per shape"),
			_TL("one hull per shape part")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConvex_Hull::On_Execute(void)
{
	CSG_Shapes	*pShapes, *pHulls, *pBoxes;

	//-----------------------------------------------------
	pShapes	= Parameters("SHAPES")	->asShapes();
	pHulls	= Parameters("HULLS")	->asShapes();
	pBoxes	= Parameters("BOXES")	->asShapes();

	//-----------------------------------------------------
	pHulls->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Convex Hull")));
	pHulls->Add_Field(_TL("ID")			, SG_DATATYPE_Int);
	pHulls->Add_Field(_TL("AREA")		, SG_DATATYPE_Double);
	pHulls->Add_Field(_TL("PERIMETER")	, SG_DATATYPE_Double);

	//-----------------------------------------------------
	int	nOkay	= 0;

	//-----------------------------------------------------
	if( pShapes->Get_Type() == SHAPE_TYPE_Point )
	{
		nOkay	= Get_Chain_Hull(pShapes, pHulls) ? 1 : 0;
	}

	//-----------------------------------------------------
	else
	{
		CSG_Shapes	Points(SHAPE_TYPE_Point);

		int	Construction	= Parameters("POLYPOINTS")->asInt();

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Points.Add_Shape()->Add_Point(pShape->Get_Point(iPoint, iPart));
				}

				if( Construction == 2 )	// one hull per shape part
				{
					if( Get_Chain_Hull(&Points, pHulls) )	nOkay++;	Points.Del_Records();
				}
			}

			if( Construction == 1 )	// one hull per shape
			{
				if( Get_Chain_Hull(&Points, pHulls) )	nOkay++;	Points.Del_Records();
			}
		}

		if( Construction == 0 )	// one hull for all shapes
		{
			if( Get_Chain_Hull(&Points, pHulls) )	nOkay++;	Points.Del_Records();
		}
	}

	if( nOkay <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( pBoxes )
	{
		pBoxes->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Bounding Box")));
		pBoxes->Add_Field(_TL("ID")			, SG_DATATYPE_Int);
		pBoxes->Add_Field(_TL("AREA")		, SG_DATATYPE_Double);
		pBoxes->Add_Field(_TL("PERIMETER")	, SG_DATATYPE_Double);

		for(int iHull=0; iHull<pHulls->Get_Count() && Set_Progress(iHull, pHulls->Get_Count()); iHull++)
		{
			Get_Bounding_Box(pHulls->Get_Shape(iHull), pBoxes->Add_Shape());
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
bool CConvex_Hull::Get_Bounding_Box(CSG_Shape *pHull, CSG_Shape *pBox)
{
	int			i, iBox;
	double		a, aBox, d, dBox, dSin, dCos;
	TSG_Rect	r, rBox;
	TSG_Point	A, B;

	B	= pHull->Get_Point(pHull->Get_Point_Count() - 1);

	for(i=0; i<pHull->Get_Point_Count(); i++)
	{
		A		= B;
		B		= pHull->Get_Point(i);
		d		= SG_Get_Angle_Of_Direction(A, B);
		dSin	= sin(-d);
		dCos	= cos(-d);

		for(int j=0; j<pHull->Get_Point_Count(); j++)
		{
			TSG_Point	q, p	= pHull->Get_Point(j);

			q.x	= p.x * dCos - p.y * dSin;
			q.y	= p.x * dSin + p.y * dCos;

			if( j == 0 )
			{
				r.xMin	= r.xMax	= q.x;
				r.yMin	= r.yMax	= q.y;
			}
			else
			{
				if( r.xMin > q.x ) r.xMin = q.x; else if( r.xMax < q.x ) r.xMax = q.x;
				if( r.yMin > q.y ) r.yMin = q.y; else if( r.yMax < q.y ) r.yMax = q.y;
			}
		}

		a	= (r.xMax - r.xMin) * (r.yMax - r.yMin);

		if( i == 0 || a < aBox )
		{
			iBox	= i;
			aBox	= a;
			dBox	= d;
			rBox	= r;
		}
	}

	//-----------------------------------------------------
	dSin	= sin(dBox);
	dCos	= cos(dBox);

	pBox->Add_Point(rBox.xMin * dCos - rBox.yMin * dSin, rBox.xMin * dSin + rBox.yMin * dCos);
	pBox->Add_Point(rBox.xMin * dCos - rBox.yMax * dSin, rBox.xMin * dSin + rBox.yMax * dCos);
	pBox->Add_Point(rBox.xMax * dCos - rBox.yMax * dSin, rBox.xMax * dSin + rBox.yMax * dCos);
	pBox->Add_Point(rBox.xMax * dCos - rBox.yMin * dSin, rBox.xMax * dSin + rBox.yMin * dCos);
	pBox->Add_Point(pBox->Get_Point(0));

	pBox->Set_Value(0, pBox->Get_Index());
	pBox->Set_Value(1, aBox);
	pBox->Set_Value(2, 2.0 * ((rBox.xMax - rBox.xMin) + (rBox.yMax - rBox.yMin)));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CConvex_Hull::Get_Chain_Hull(CSG_Shapes *pPoints, CSG_Shapes *pHulls)
{
	int			i, n;
	CSG_Points	Points, Hull;

	//-----------------------------------------------------
	if( pPoints->Get_Count() < 3 )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pPoints	= pPoints;

	CSG_Index	Index(m_pPoints->Get_Count(), (TSG_PFNC_Compare)CConvex_Hull::Compare);

	m_pPoints	= NULL;

	if( !Index.is_Okay() )
	{
		return( false );
	}

	for(i=0; i<pPoints->Get_Count(); i++)
	{
		Points.Add(pPoints->Get_Shape(Index[i])->Get_Point(0));
	}

	//-----------------------------------------------------
	if( (n = Get_Chain_Hull(Points, Hull)) < 3 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shape	*pHull	= pHulls->Add_Shape();

	for(i=0; i<n && Process_Get_Okay(); i++)
	{
		pHull->Add_Point(Hull[i]);
	}

	pHull->Set_Value(0, pHull->Get_Index());
	pHull->Set_Value(1, ((CSG_Shape_Polygon *)pHull)->Get_Area());
	pHull->Set_Value(2, ((CSG_Shape_Polygon *)pHull)->Get_Perimeter());

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes * CConvex_Hull::m_pPoints	= NULL;

//---------------------------------------------------------
int CConvex_Hull::Compare(const int iElement_1, const int iElement_2)
{
	TSG_Point	a	= m_pPoints->Get_Shape(iElement_1)->Get_Point(0);
	TSG_Point	b	= m_pPoints->Get_Shape(iElement_2)->Get_Point(0);

	if( a.x < b.x )	{	return( -1 );	}
	if( a.x > b.x )	{	return(  1 );	}

	if( a.y < b.y )	{	return( -1 );	}
	if( a.y > b.y )	{	return(  1 );	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
//---------------------------------------------------------

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////
//---------------------------------------------------------
// is_Left(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points Line_A, Line_B, and Point
//    Return: >0 for Point left of the line through P0 and P1
//            =0 for Point on the line
//            <0 for Point right of the line
//    See: the January 2001 Algorithm on Area of Triangles
//---------------------------------------------------------
inline double CConvex_Hull::is_Left(const TSG_Point &Line_A, const TSG_Point &Line_B, const TSG_Point &Point)
{
	return( (Line_B.x - Line_A.x) * (Point.y - Line_A.y) - (Point.x - Line_A.x) * (Line_B.y - Line_A.y) );
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////
//---------------------------------------------------------
// chainHull_2D(): Andrew's monotone chain 2D convex hull algorithm
//     Input:  P[] = an array of 2D points
//                   presorted by increasing x- and y-coordinates
//             n = the number of points in P[]
//     Output: H[] = an array of the convex hull vertices (max is n)
//     Return: the number of points in H[]
//---------------------------------------------------------
int CConvex_Hull::Get_Chain_Hull(CSG_Points &P, CSG_Points &H)
{
	int		i, n, bot, top, minmin, minmax, maxmin, maxmax;
	double	xmin, xmax;

	//-----------------------------------------------------
    // the output array H[] will be used as the stack

	n	= P.Get_Count();

	H.Set_Count(2 * n);

	bot	=  0;	// index for bottom of the stack
	top	= -1;	// index for top of the stack

	//-----------------------------------------------------
	// Get the indices of points with min x-coord and min|max y-coord

	minmin	= 0;
	xmin	= P[0].x;

	for(i=1; i<n; i++)
	{
		if( P[i].x != xmin )
		{
			break;
		}
	}

	minmax	= i - 1;

	if( minmax == n - 1 )	// degenerate case: all x-coords == xmin
	{
		H[++top]	= P[minmin];

		if( P[minmax].y != P[minmin].y )	// a nontrivial segment
		{
			H[++top]	= P[minmax];
		}

		H[++top]	= P[minmin];			// add polygon endpoint

		return( top + 1 );
	}

	//-----------------------------------------------------
	// Get the indices of points with max x-coord and min|max y-coord
	maxmax	= n-1;
	xmax	= P[n - 1].x;

	for(i=n-2; i>=0; i--)
	{
		if( P[i].x != xmax )
		{
			break;
		}
	}

	maxmin	= i + 1;

	//-----------------------------------------------------
	// Compute the lower hull on the stack H
	H[++top]	= P[minmin];		// push minmin point onto stack
	i			= minmax;

	while( ++i <= maxmin && Process_Get_Okay() )
	{
		// the lower line joins P[minmin] with P[maxmin]
		if( is_Left(P[minmin], P[maxmin], P[i]) >= 0 && i < maxmin )
			continue;				// ignore P[i] above or on the lower line

		while( top > 0 )			// there are at least 2 points on the stack
		{
			// test if P[i] is left of the line at the stack top
			if( is_Left(H[top-1], H[top], P[i]) > 0 )
				break;				// P[i] is a new hull vertex
			else
				top--;				// pop top point off stack
		}

		H[++top]	= P[i];			// push P[i] onto stack
	}

	//-----------------------------------------------------
	// Next, compute the upper hull on the stack H above the bottom hull
	if( maxmax != maxmin )			// if distinct xmax points
	{
		H[++top]	= P[maxmax];	// push maxmax point onto stack
	}

	bot		= top;					// the bottom point of the upper hull stack
	i		= maxmin;

	while( --i >= minmax && Process_Get_Okay() )
	{
		// the upper line joins P[maxmax] with P[minmax]
		if( is_Left(P[maxmax], P[minmax], P[i]) >= 0 && i > minmax )
			continue;				// ignore P[i] below or on the upper line

		while( top > bot )			// at least 2 points on the upper stack
		{
			// test if P[i] is left of the line at the stack top
			if( is_Left(H[top-1], H[top], P[i]) > 0 )
				break;				// P[i] is a new hull vertex
			else
				top--;				// pop top point off stack
		}

		H[++top]	= P[i];			// push P[i] onto stack
	}

	if( minmax != minmin )
	{
		H[++top]	= P[minmin];	// push joining endpoint onto stack
	}

	return( top + 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
