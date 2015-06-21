/**********************************************************
 * Version $Id$
 *********************************************************/

/*******************************************************************************
    Points_From_Lines.cpp
    Copyright (C) Victor Olaya

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Points_From_Lines.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_From_Lines::CPoints_From_Lines(void)
{
	Set_Name		(_TL("Convert Lines to Points"));

	Set_Author		(_TL("Victor Olaya (c) 2004"));

	Set_Description	(_TW(
		"Converts lines to points. "
		"Optionally inserts additional points in user-defined distances. "
	));

	Parameters.Add_Shapes(
		NULL, "LINES"	, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL, "POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Value(
		NULL, "ADD"		, _TL("Insert Additional Points"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL, "DIST"	, _TL("Insert Distance"),
		_TL(""),
		PARAMETER_TYPE_Double, 100, 0.0, true
	);

	Parameters.Add_Value(
		NULL, "ADD_POINT_ORDER"	, _TL("Add Point Order"),
		_TL("Add point order as additional attribute."),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_From_Lines::On_Execute(void)
{
	int			iLine, iPart, iPoint, jPoint;
	double		dx, dy, dz, dm, dDist, dLine,  A_z, B_z, A_m, B_m;
	TSG_Point	Pt_A, Pt_B;
	CSG_Shapes	*pLines, *pPoints;
	CSG_Shape	*pLine, *pPoint;
	bool		bAddPtOrder;

	//-----------------------------------------------------
	pLines		= Parameters("LINES")	->asShapes();
	pPoints		= Parameters("POINTS")	->asShapes();
	dDist		= Parameters("ADD")		->asBool() ? Parameters("DIST")->asDouble() : -1.0;
	bAddPtOrder	= Parameters("ADD_POINT_ORDER")->asBool();

	pPoints->Create(SHAPE_TYPE_Point, pLines->Get_Name(), pLines, pLines->Get_Vertex_Type());

	if( bAddPtOrder )
		pPoints->Add_Field(_TL("PT_ID"), SG_DATATYPE_Int);

	//-----------------------------------------------------
	for(iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		pLine	= pLines->Get_Shape(iLine);

		int		iPoints = 0;

		for(iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			//---------------------------------------------
			if( dDist > 0.0 )	// insert additional points...
			{
				Pt_B	= pLine->Get_Point(0, iPart);

				if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					B_z	= pLine->Get_Z(0, iPart);

					if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						B_m	= pLine->Get_M(0, iPart);
					}
				}

				for(iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
				{
					Pt_A	= Pt_B;
					Pt_B	= pLine->Get_Point(iPoint, iPart);

					if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
					{
						A_z = B_z;
						B_z	= pLine->Get_Z(iPoint, iPart);

						if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
						{
							A_m = B_m;
							B_m	= pLine->Get_M(iPoint, iPart);
						}
					}

					dx		= Pt_B.x - Pt_A.x;
					dy		= Pt_B.y - Pt_A.y;
					dLine	= sqrt(dx*dx + dy*dy);
					dx		/= dLine;
					dy		/= dLine;

					if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
					{
						dz		= (B_z - A_z) / dLine;

						if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
						{
							dm		= (B_m - A_m) / dLine;
						}
					}

					pPoint	= pPoints->Add_Shape(pLine, SHAPE_COPY_ATTR);
					pPoint	->Add_Point(Pt_A);

					if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
					{
						pPoint->Set_Z(A_z, 0);

						if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
						{
							pPoint->Set_M(A_m, 0);
						}
					}

					if( bAddPtOrder )
					{
						pPoint->Set_Value(pPoints->Get_Field_Count()-1, iPoints);
						iPoints++;
					}

					double dLength = 0.0;

					while( dLength + dDist < dLine )
					{
						Pt_A.x	+= dDist * dx;
						Pt_A.y	+= dDist * dy;

						pPoint	= pPoints->Add_Shape(pLine, SHAPE_COPY_ATTR);
						pPoint	->Add_Point(Pt_A);

						if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
						{
							A_z	+= dDist * dz;
							pPoint->Set_Z(A_z, 0);

							if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
							{
								A_m	+= dDist * dm;
								pPoint->Set_M(A_m, 0);
							}
						}

						if( bAddPtOrder )
						{
							pPoint->Set_Value(pPoints->Get_Field_Count()-1, iPoints);
							iPoints++;
						}

						dLength += dDist;
					}
				}
			}

			//---------------------------------------------
			else				// just copy points...
			{
				for(iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
				{
					pPoint	= pPoints->Add_Shape(pLine, SHAPE_COPY_ATTR);
					pPoint	->Add_Point(pLine->Get_Point(iPoint, iPart));

					if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
					{
						pPoint->Set_Z(pLine->Get_Z(iPoint, iPart), 0);

						if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
						{
							pPoint->Set_M(pLine->Get_M(iPoint, iPart), 0);
						}
					}

					if( bAddPtOrder )
					{
						pPoint->Set_Value(pPoints->Get_Field_Count()-1, iPoints);
						iPoints++;
					}
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
CPoints_From_MultiPoints::CPoints_From_MultiPoints(void)
{
	Set_Name		(_TL("Convert Multipoints to Points"));

	Set_Author		(_TL("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Converts multipoints to points. "
	));

	Parameters.Add_Shapes(
		NULL	, "MULTIPOINTS"	, _TL("Multipoints"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Points
	);

	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_From_MultiPoints::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pMultipoints	= Parameters("MULTIPOINTS")	->asShapes();
	CSG_Shapes	*pPoints		= Parameters("POINTS")		->asShapes();

	pPoints->Create(SHAPE_TYPE_Point, pMultipoints->Get_Name(), pMultipoints, pMultipoints->Get_Vertex_Type());

	//-----------------------------------------------------
	for(int iMultipoint=0; iMultipoint<pMultipoints->Get_Count() && Set_Progress(iMultipoint, pMultipoints->Get_Count()); iMultipoint++)
	{
		CSG_Shape	*pMultipoint	= pMultipoints->Get_Shape(iMultipoint);

		for(int iPart=0; iPart<pMultipoint->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pMultipoint->Get_Point_Count(iPart); iPoint++)
			{
				CSG_Shape	*pPoint	= pPoints->Add_Shape(pMultipoint, SHAPE_COPY_ATTR);

				pPoint->Add_Point(pMultipoint->Get_Point(iPoint, iPart));

				if( pMultipoints->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					pPoint->Set_Z(pMultipoint->Get_Z(iPoint, iPart), 0);

					if( pMultipoints->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						pPoint->Set_M(pMultipoint->Get_M(iPoint, iPart), 0);
					}
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
