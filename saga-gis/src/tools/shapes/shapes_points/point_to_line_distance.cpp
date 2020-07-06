
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
//               point_to_line_distance.cpp              //
//                                                       //
//                 Copyright (C) 2020 by                 //
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
#include "point_to_line_distance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoint_to_Line_Distance::CPoint_to_Line_Distance(void)
{
	Set_Name		(_TL("Point to Line Distances"));

	Set_Author		("O.Conrad (c) 2020");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes("",
		"RESULT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes("",
		"DISTANCES"	, _TL("Distances"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Table_Field("LINES",
		"LINE_ID"	, _TL("Identifier"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_to_Line_Distance::On_Execute(void)
{
	CSG_Shapes	*pLines	= Parameters("LINES")->asShapes();

	if( !pLines->is_Valid() )
	{
		Error_Set(_TL("Invalid lines layer."));

		return( false );
	}

	int	LineID	= Parameters("LINE_ID")->asInt();

	//-----------------------------------------------------
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	if( Parameters("RESULT")->asShapes() && Parameters("RESULT")->asShapes() != pPoints )
	{
		CSG_Shapes	*pResult	= Parameters("RESULT")->asShapes();
		
		pResult->Create(SHAPE_TYPE_Point);

		pResult->Fmt_Name("%s [%s, %s]", pPoints->Get_Name(), _TL("Distances"), pLines->Get_Name());

		pResult->Add_Field("FID", SG_DATATYPE_Int);

		for(int i=0; i<pPoints->Get_Count(); i++)
		{
			CSG_Shape	*pPoint	= pResult->Add_Shape();

			pPoint->Add_Point(pPoints->Get_Shape(i)->Get_Point(0));
			pPoint->Set_Value(0, i + 1);
		}

		pPoints	= pResult;
	}

	if( !pPoints->is_Valid() )
	{
		Error_Set(_TL("Invalid points layer."));

		return( false );
	}

	//-----------------------------------------------------
	int	offField	= pPoints->Get_Field_Count();

	pPoints->Add_Field("LINE_ID" , LineID < 0 ? SG_DATATYPE_Int : pLines->Get_Field_Type(LineID));
	pPoints->Add_Field("DISTANCE", SG_DATATYPE_Double);
	pPoints->Add_Field("X"       , SG_DATATYPE_Double);
	pPoints->Add_Field("Y"       , SG_DATATYPE_Double);

	//-----------------------------------------------------
	CSG_Shapes	*pDistances	= Parameters("DISTANCES")->asShapes();

	if( pDistances )
	{
		pDistances->Create(SHAPE_TYPE_Line);

		pDistances->Fmt_Name("%s [%s, %s]", pPoints->Get_Name(), _TL("Distances"), pLines->Get_Name());

		pDistances->Add_Field("POINT_ID", SG_DATATYPE_Int);
		pDistances->Add_Field("LINE_ID" , LineID < 0 ? SG_DATATYPE_Int : pLines->Get_Field_Type(LineID));
		pDistances->Add_Field("DISTANCE", SG_DATATYPE_Double);

		pDistances->Set_Count(pPoints->Get_Count());
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int i=0; i<pPoints->Get_Count(); i++)
	{
		if( SG_OMP_Get_Thread_Num() == 0 )
		{
			Set_Progress(i * SG_OMP_Get_Max_Num_Threads(), pPoints->Get_Count());
		}

		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		CSG_Point Point[2]; int Line = Get_Distance(pLines, Point[0] = pPoint->Get_Point(0), Point[1]);

		if( Line < 0 )	// this should actually never happen!
		{
			pPoint->Set_NoData(offField + 0);
			pPoint->Set_NoData(offField + 1);
			pPoint->Set_NoData(offField + 2);
			pPoint->Set_NoData(offField + 3);

			if( pDistances )
			{
				CSG_Shape	*pDistance	= pDistances->Get_Shape(i);

				pDistance->Add_Point(Point[0]);
				pDistance->Set_Value(0, i);
				pDistance->Set_NoData(1);
				pDistance->Set_NoData(2);
			}
		}
		else // if( Line >= 0 )
		{
			if( LineID < 0 )
			{
				pPoint->Set_Value(offField, Line);
			}
			else
			{
				pPoint->Set_Value(offField, pLines->Get_Shape(Line)->asString(LineID));
			}

			pPoint->Set_Value(offField + 1, SG_Get_Distance(Point[0], Point[1]));
			pPoint->Set_Value(offField + 2, Point[1].x);
			pPoint->Set_Value(offField + 3, Point[1].y);

			if( pDistances )
			{
				CSG_Shape	*pDistance	= pDistances->Get_Shape(i);

				pDistance->Add_Point(Point[0]);
				pDistance->Add_Point(Point[1]);
				pDistance->Set_Value(0, i);
				pDistance->Set_Value(1, pPoint->asString(offField));
				pDistance->Set_Value(2, SG_Get_Distance(Point[0], Point[1]));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPoint_to_Line_Distance::Get_Distance(const CSG_Shapes *pLines, const CSG_Point &Point, CSG_Point &PointOnLine)
{
	int	Line	= -1;	double	Distance	= -1.;

	for(int iLine=0; iLine<pLines->Get_Count(); iLine++)
	{
		CSG_Shape_Line	*pLine	= (CSG_Shape_Line *)pLines->Get_Shape(iLine);

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			TSG_Point	A	= pLine->Get_Point(0, iPart);

			for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point	C, B = A; A = pLine->Get_Point(iPoint, iPart);

				double	d	= SG_Get_Nearest_Point_On_Line(Point, A, B, C);

				if( Line < 0 || d < Distance )
				{
					Line = iLine; Distance = d; PointOnLine = C;

					if( Distance == 0. )
					{
						return( Line );
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( Line );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
