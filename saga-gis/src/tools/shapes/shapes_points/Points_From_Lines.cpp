/**********************************************************
 * Version $Id$
 *********************************************************/

/*******************************************************************************
    Points_From_Lines.cpp
    Copyright (C) Victor Olaya, Volker Wichmann

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

	Set_Author		(_TL("V. Olaya, V. Wichmann (c) 2004-2015"));

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

	Parameters.Add_Choice(
		NULL, "METHOD_INSERT"	, _TL("Insertion"),
		_TL("Choose the method how to insert additional points."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("per line segment"),
			_TL("per line"),
			_TL("from line center")
		), 0
	);

	Parameters.Add_Value(
		NULL, "DIST"	, _TL("Insertion Distance"),
		_TL("Point insertion distance [map units]."),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
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
int CPoints_From_Lines::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("ADD") )
	{
		pParameters->Get_Parameter("METHOD_INSERT")		->Set_Enabled(pParameter->asBool());
		pParameters->Get_Parameter("DIST")				->Set_Enabled(pParameter->asBool());
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_From_Lines::On_Execute(void)
{
	CSG_Shapes	*pLines, *pPoints;
	bool		bAddPoints, bAddPtOrder;
	int			iMethod;
	double		dDist;

	//-----------------------------------------------------
	pLines		= Parameters("LINES")			->asShapes();
	pPoints		= Parameters("POINTS")			->asShapes();
	bAddPoints	= Parameters("ADD")				->asBool();
	iMethod		= Parameters("METHOD_INSERT")	->asInt();
	dDist		= Parameters("DIST")			->asDouble();
	bAddPtOrder	= Parameters("ADD_POINT_ORDER")	->asBool();

	pPoints->Create(SHAPE_TYPE_Point, pLines->Get_Name(), pLines, pLines->Get_Vertex_Type());

	if( bAddPtOrder )
		pPoints->Add_Field(_TL("PT_ID"), SG_DATATYPE_Int);

	if( bAddPoints )
	{
		switch (iMethod)
		{
		case 0:
		default:		Convert_Add_Points_Segment(pLines, pPoints, dDist, bAddPtOrder);	break;

		case 1:			Convert_Add_Points_Line(pLines, pPoints, dDist, bAddPtOrder);		break;

		case 2:			Convert_Add_Points_Center(pLines, pPoints, dDist, bAddPtOrder);		break;
		}
	}
	else
	{
		Convert(pLines, pPoints, bAddPtOrder);
	}


	return( true );
}


//---------------------------------------------------------
void CPoints_From_Lines::Convert_Add_Points_Segment(CSG_Shapes *pLines, CSG_Shapes *pPoints, double dDist, bool bAddPtOrder)
{
	CSG_Shape	*pLine, *pPoint;
	TSG_Point	Pt_A, Pt_B;
	double		dx, dy, dz, dm, dLine,  A_z, B_z, A_m, B_m;


	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		pLine	= pLines->Get_Shape(iLine);

		int		iPoints = 0;

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
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

			for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
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
	}

	return;
}


//---------------------------------------------------------
void CPoints_From_Lines::Convert_Add_Points_Line(CSG_Shapes *pLines, CSG_Shapes *pPoints, double dDist, bool bAddPtOrder)
{
	CSG_Shape	*pLine, *pPoint;
	TSG_Point	Pt_A, Pt_B;
	double		dx, dy, dz, dm, dLine,  A_z, B_z, A_m, B_m;


	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		pLine	= pLines->Get_Shape(iLine);

		int		iPoints = 0;
		double	dOffset = 0.0;

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
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


			for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
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

				if ( dOffset > dLine )
				{
					dOffset = dOffset - dLine;
					continue;
				}

				dx		/= dLine;
				dy		/= dLine;

				Pt_A.x	+= dOffset * dx;
				Pt_A.y	+= dOffset * dy;

				if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					dz		= (B_z - A_z) / dLine;
					A_z		+= dOffset * dz;

					if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						dm		= (B_m - A_m) / dLine;
						A_m		+= dOffset * dm;
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

				dLine -= dOffset;

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

				dOffset = (dLength + dDist) - dLine;
			}
		}
	}

	return;
}


//---------------------------------------------------------
void CPoints_From_Lines::Convert_Add_Points_Center(CSG_Shapes *pLines, CSG_Shapes *pPoints, double dDist, bool bAddPtOrder)
{
	CSG_Shape	*pLine;
	TSG_Point	Pt_A, Pt_B, Pt_C;
	double		dx, dy, dz, dm, dLine,  A_z, B_z, C_z, A_m, B_m, C_m;
	int			iSplitPart, iSplitPoint;

	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		pLine	= pLines->Get_Shape(iLine);

		double	dCenter = ((CSG_Shape_Line *)pLine)->Get_Length() / 2.0;
		double	dLength = 0.0;

		//---------------------------------------------------------
		// search for the part we need to split
		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			dLine = ((CSG_Shape_Line *)pLine)->Get_Length(iPart);

			if( dLength + dLine < dCenter )
			{
				dLength += dLine;
			}
			else
			{
				iSplitPart = iPart;
				break;
			}
		}

		double	dOffset = dCenter - dLength;

		Pt_B	= pLine->Get_Point(0, iSplitPart);

		if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
		{
			B_z	= pLine->Get_Z(0, iSplitPart);

			if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
			{
				B_m	= pLine->Get_M(0, iSplitPart);
			}
		}

		dLength = 0.0;

		//---------------------------------------------------------
		// search for the point after which we need to split
		for(int iPoint=1; iPoint<pLine->Get_Point_Count(iSplitPart); iPoint++)
		{
			Pt_A	= Pt_B;
			Pt_B	= pLine->Get_Point(iPoint, iSplitPart);

			if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
			{
				A_z = B_z;
				B_z	= pLine->Get_Z(iPoint, iSplitPart);

				if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
				{
					A_m = B_m;
					B_m	= pLine->Get_M(iPoint, iSplitPart);
				}
			}

			dx		= Pt_B.x - Pt_A.x;
			dy		= Pt_B.y - Pt_A.y;
			dLine	= sqrt(dx*dx + dy*dy);

			if (dLength + dLine < dOffset)
			{
				dLength += dLine;
			}
			else
			{
				iSplitPoint = iPoint - 1;
				dOffset		= dOffset - dLength;
				break;
			}
		}

		//---------------------------------------------------------
		// create center point
		dx		/= dLine;
		dy		/= dLine;

		Pt_A.x	+= dOffset * dx;
		Pt_A.y	+= dOffset * dy;

		Pt_C	= Pt_A;

		if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
		{
			dz		= (B_z - A_z) / dLine;
			A_z		+= dOffset * dz;
			C_z		= A_z;

			if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
			{
				dm		= (B_m - A_m) / dLine;
				A_m		+= dOffset * dm;
				C_m		= A_m;
			}
		}

		//---------------------------------------------------------
		// create line part from center to end of line
		CSG_Shapes	SplitLines;
		CSG_Shape	*pSplitLine;

		SplitLines.Create(pLines->Get_Type(), pLines->Get_Name(), pLines, pLines->Get_Vertex_Type());

		int iPartOffset = 0;
		int iPointOffset;

		pSplitLine	= SplitLines.Add_Shape(pLine, SHAPE_COPY_ATTR);
		pSplitLine	->Add_Point(Pt_C, iPartOffset);

		if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
		{
			pSplitLine->Set_Z(C_z, iPartOffset);

			if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
			{
				pSplitLine->Set_M(C_m, iPartOffset);
			}
		}

		for(int iPart=iSplitPart; iPart<pLine->Get_Part_Count(); iPart++)
		{
			if( iPart == iSplitPart )
			{
				iPointOffset = iSplitPoint + 1;
			}
			else
			{
				iPointOffset = 0;
			}

			for(int iPoint=iPointOffset; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				Pt_A	= pLine->Get_Point(iPoint, iPart);

				if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					A_z	= pLine->Get_Z(iPoint, iPart);

					if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						A_m	= pLine->Get_M(iPoint, iPart);
					}
				}

				pSplitLine	->Add_Point(Pt_A, iPartOffset);

				if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					pSplitLine->Set_Z(A_z, iPartOffset);

					if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						pSplitLine->Set_M(A_m, iPartOffset);
					}
				}
			} // for iPoint

			iPartOffset++;
		} // for iPart;

		Convert_Add_Points_Line(&SplitLines, pPoints, dDist, bAddPtOrder);


		//---------------------------------------------------------
		// create line part from center to start of line

		SplitLines.Create(pLines->Get_Type(), pLines->Get_Name(), pLines, pLines->Get_Vertex_Type());

		iPartOffset = 0;

		pSplitLine	= SplitLines.Add_Shape(pLine, SHAPE_COPY_ATTR);
		pSplitLine	->Add_Point(Pt_C, iPartOffset);

		if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
		{
			pSplitLine->Set_Z(C_z, iPartOffset);

			if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
			{
				pSplitLine->Set_M(C_m, iPartOffset);
			}
		}

		for(int iPart=iSplitPart; iPart>=0; iPart--)
		{
			if( iPart == iSplitPart )
			{
				iPointOffset = iSplitPoint;
			}
			else
			{
				iPointOffset = pLine->Get_Point_Count(iPart);
			}

			for(int iPoint=iPointOffset; iPoint>=0; iPoint--)
			{
				Pt_A	= pLine->Get_Point(iPoint, iPart);

				if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					A_z	= pLine->Get_Z(iPoint, iPart);

					if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						A_m	= pLine->Get_M(iPoint, iPart);
					}
				}

				pSplitLine	->Add_Point(Pt_A, iPartOffset);

				if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					pSplitLine->Set_Z(A_z, iPartOffset);

					if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						pSplitLine->Set_M(A_m, iPartOffset);
					}
				}
			} // for iPoint

			iPartOffset++;
		} // for iPart;

		Convert_Add_Points_Line(&SplitLines, pPoints, dDist, bAddPtOrder);

	} // for iLine


	return;
}


//---------------------------------------------------------
void CPoints_From_Lines::Convert(CSG_Shapes *pLines, CSG_Shapes *pPoints, bool bAddPtOrder)
{
	CSG_Shape	*pLine, *pPoint;

	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		pLine	= pLines->Get_Shape(iLine);

		int		iPoints = 0;

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
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

	return;
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
