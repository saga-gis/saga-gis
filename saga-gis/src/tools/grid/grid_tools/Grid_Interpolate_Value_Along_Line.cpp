
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//         Grid_Interpolate_Value_Along_Line.cpp         //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Interpolate_Value_Along_Line.h"



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Interpolate_Value_Along_Line::CGrid_Interpolate_Value_Along_Line(void)
{
	Set_Name		(_TL("Interpolate Values Along Line "));

	Set_Author		(SG_T("V. Wichmann (c) 2019"));

	Set_Description	(_TW(
		"The tool allows one to change the values of the input grid along a line. "
		"The values along the line will be interpolated linearly between the "
		"start and the end point of the line. Thus you must assure that these "
		"grid cells contain valid data. Optionally the changes can be limited "
		"to grid cells with No Data only.\n"
		"Once the tool is executed and running, you can use the 'Action' tool to "
		"digitize a line. Use left mouse button clicks to set line vertices. "
		"A right mouse button click will finish the line.\n\n"
	));

	Set_Drag_Mode	(TOOL_INTERACTIVE_DRAG_LINE);

	Parameters.Add_Grid("",
		"GRID"	, _TL("Grid"),
		_TL("The grid to modify."),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"LINE"	, _TL("Line"),
		_TL("The digitized line."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Value("",
		"ONLY_NODATA"	, _TL("Only No Data"),
		_TL("Apply changes only to grid cells with No Data along the line."),
		PARAMETER_TYPE_Bool, false
	);
}

//---------------------------------------------------------
CGrid_Interpolate_Value_Along_Line::~CGrid_Interpolate_Value_Along_Line(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Interpolate_Value_Along_Line::On_Execute(void)
{
	m_pGrid			= Parameters("GRID")->asGrid();
	m_pLine			= Parameters("LINE")->asShapes();
	m_bOnlyNoData	= Parameters("ONLY_NODATA")->asBool();

	m_pPoints		= new CSG_Shapes(SHAPE_TYPE_Point);

	m_bAdd			= false;

	DataObject_Update(m_pGrid, SG_UI_DATAOBJECT_SHOW_NEW_MAP);
	DataObject_Update(m_pLine, SG_UI_DATAOBJECT_SHOW_LAST_MAP);

	return( true );
}

//---------------------------------------------------------
bool CGrid_Interpolate_Value_Along_Line::On_Execute_Finish(void)
{
	if( m_bAdd )
	{
		Set_Line();
	}

	delete( m_pPoints );

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Interpolate_Value_Along_Line::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
	default:
		break;

	case TOOL_INTERACTIVE_LDOWN:
		if( !m_bAdd )
		{
			m_bAdd	= true;
			m_pLine->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("Line [%s]"), m_pGrid->Get_Name()));
			m_pLine->Add_Field("ID"	, SG_DATATYPE_Int);
			m_pLine->Add_Shape()->Set_Value(0, 1);
		}

		m_pLine->Get_Shape(0)->Add_Point(Get_System().Fit_to_Grid_System(ptWorld));

		DataObject_Update(m_pLine);
		break;

	case TOOL_INTERACTIVE_RDOWN:
		Set_Line();
		m_bAdd	= false;
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Interpolate_Value_Along_Line::Set_Line(void)
{
	TSG_Point	A, B;
	int			Ax, Ay, Bx, By, Cx, Cy;
	double		Az, Bz, Cz, Length, tanAlpha;
	CSG_Shape	*pLine;


	//-----------------------------------------------------
	if( (pLine = m_pLine->Get_Shape(0)) == NULL || pLine->Get_Point_Count(0) < 2 )
	{
		return( false );
	}

	A = pLine->Get_Point(0, 0);
	B = pLine->Get_Point(pLine->Get_Point_Count(0) - 1, 0);

	if(	!Get_System().Get_World_to_Grid(Ax, Ay, A) || !m_pGrid->is_InGrid(Ax, Ay)	||
		!Get_System().Get_World_to_Grid(Bx, By, B) || !m_pGrid->is_InGrid(Bx, By) )
	{
		Error_Set(_TL("Please digitize a valid line!"));
		return( false );
	}

	DataObject_Update(m_pLine);

	//-----------------------------------------------------
	m_pPoints->Create(SHAPE_TYPE_Point, SG_T(""));
	m_pPoints->Add_Field("Length", SG_DATATYPE_Double);

	//-----------------------------------------------------
	B	= pLine->Get_Point(0);

	for(int i=1; i<pLine->Get_Point_Count(0); i++)
	{
		A	= B;
		B	= pLine->Get_Point(i);

		Set_Line(A, B);
	}

	//-----------------------------------------------------
	if( m_pPoints->Get_Count() < 2 )
	{
		Error_Set(_TL("Line construction failed!"));
		return( false );
	}

	if( m_pPoints->Get_Count() == 2 )
	{
		Message_Dlg(_TL("Line has only two points on grid cells, nothing to do!"));
		return( true );
	}

	Az			= m_pGrid->asDouble(Ax, Ay);
	Bz			= m_pGrid->asDouble(Bx, By);
	Length		= m_pPoints->Get_Record(m_pPoints->Get_Record_Count() - 1)->asDouble(0);
	tanAlpha	= (Bz - Az) / Length;

	for(int i=1; i<m_pPoints->Get_Count() - 1; i++)
	{
		Cz = Az + tanAlpha * m_pPoints->Get_Shape(i)->asDouble(0);

		Get_System().Get_World_to_Grid(Cx, Cy, m_pPoints->Get_Shape(i)->Get_Point(0));

		if( m_bOnlyNoData && !m_pGrid->is_NoData(Cx, Cy) )
		{
			continue;
		}

		m_pGrid->Set_Value(Cx, Cy, Cz);
	}


	//-----------------------------------------------------
	DataObject_Update(m_pGrid);

	return( true );
}

//---------------------------------------------------------
void CGrid_Interpolate_Value_Along_Line::Set_Line(TSG_Point A, TSG_Point B)
{
	double		dx, dy, d, n;
	TSG_Point	p;

	//-----------------------------------------------------
	dx	= fabs(B.x - A.x);
	dy	= fabs(B.y - A.y);

	if( dx > 0.0 || dy > 0.0 )
	{
		if( dx > dy )
		{
			dx	/= Get_Cellsize();
			n	 = dx;
			dy	/= dx;
			dx	 = Get_Cellsize();
		}
		else
		{
			dy	/= Get_Cellsize();
			n	 = dy;
			dx	/= dy;
			dy	 = Get_Cellsize();
		}

		if( B.x < A.x )
		{
			dx	= -dx;
		}

		if( B.y < A.y )
		{
			dy	= -dy;
		}

		//-------------------------------------------------
		for(d=0.0, p.x=A.x, p.y=A.y; d<=n; d++, p.x+=dx, p.y+=dy)
		{
			Add_Point(p);
		}
	}

	//-----------------------------------------------------
	return;
}

//---------------------------------------------------------
void CGrid_Interpolate_Value_Along_Line::Add_Point(CSG_Point Point)
{
	int			x, y, i;
	double		z, Length;
	CSG_Shape	*pPoint, *pLast;

	if( Get_System().Get_World_to_Grid(x, y, Point) )
	{
		z	= m_pGrid->asDouble(x, y);

		if( m_pPoints->Get_Count() == 0 )
		{
			Length	= 0.0;
		}
		else
		{
			pLast		= m_pPoints->Get_Shape(m_pPoints->Get_Count() - 1);
			Length		= SG_Get_Distance(Point, pLast->Get_Point(0));

			if( Length == 0.0 )
			{
				return;
			}

			Length += pLast->asDouble(0);
		}

		pPoint = m_pPoints->Add_Shape();
		pPoint->Add_Point(Point);
		pPoint->Set_Value(0, Length);
	}

	return;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
