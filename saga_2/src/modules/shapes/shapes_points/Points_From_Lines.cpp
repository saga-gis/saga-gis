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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 

//---------------------------------------------------------
#include "Points_From_Lines.h"

//---------------------------------------------------------
CPoints_From_Lines::CPoints_From_Lines(void){

	Set_Name		(_TL("Points from Lines"));

	Set_Author		(_TL("Copyrights (c) 2004 by Victor Olaya"));

	Set_Description	(_TL(
		"Converts a line theme to a points theme. "
		"Optionally inserts additional points in user-defined distances. "
	));

	Parameters.Add_Shapes(
		NULL, "POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL, "LINES"	, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT
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
}

//---------------------------------------------------------
CPoints_From_Lines::~CPoints_From_Lines(void)
{}

//---------------------------------------------------------
bool CPoints_From_Lines::On_Execute(void)
{
	int			iLine, iPart, iPoint, jPoint, nPoints;
	double		dx, dy, dDist;
	TGEO_Point	Pt_A, Pt_B;	
	CShapes		*pLines, *pPoints;
	CShape		*pLine, *pPoint;

	//-----------------------------------------------------
	pLines	= Parameters("LINES")	->asShapes();
	pPoints	= Parameters("POINTS")	->asShapes();
	dDist	= Parameters("ADD")		->asBool() ? Parameters("DIST")->asDouble() : -1.0;

	pPoints->Create(SHAPE_TYPE_Point, pLines->Get_Name(), &pLines->Get_Table());

	//-----------------------------------------------------
	for(iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		pLine	= pLines->Get_Shape(iLine);

		for(iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			//---------------------------------------------
			if( dDist > 0.0 )	// insert additional points...
			{
				Pt_B	= pLine->Get_Point(0, iPart);

				for(iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
				{
					Pt_A	= Pt_B;
					Pt_B	= pLine->Get_Point(iPoint, iPart);
					dx		= Pt_B.x - Pt_A.x;
					dy		= Pt_B.y - Pt_A.y;
					nPoints	= 1 + (int)(sqrt(dx*dx + dy*dy) / dDist);
					dx		/= nPoints;
					dy		/= nPoints;

					pPoint	= pPoints->Add_Shape(pLine->Get_Record());
					pPoint	->Add_Point(Pt_A);

					for(jPoint=1; jPoint<nPoints; jPoint++)
					{
						Pt_A.x	+= dx;
						Pt_A.y	+= dy;

						pPoint	= pPoints->Add_Shape(pLine->Get_Record());
						pPoint	->Add_Point(Pt_A);
					}
				}
			}

			//---------------------------------------------
			else				// just copy points...
			{
				for(iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
				{
					pPoint	= pPoints->Add_Shape(pLine->Get_Record());
					pPoint	->Add_Point(pLine->Get_Point(iPoint, iPart));
				}
			}
		}
	}

	return( true );
}
