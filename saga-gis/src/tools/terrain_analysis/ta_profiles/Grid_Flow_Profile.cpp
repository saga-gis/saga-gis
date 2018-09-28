/**********************************************************
 * Version $Id: Grid_Flow_Profile.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_profiles                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Flow_Profile.cpp                 //
//                                                       //
//                 Copyright (C) 2004 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Grid_Flow_Profile.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define VALUE_OFFSET	6


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Flow_Profile::CGrid_Flow_Profile(void)
{
	Set_Name(_TL("Flow Path Profile"));

	Set_Author		(SG_T("(c) 2004 by O.Conrad"));

	Set_Description	(_TW(
		"Create interactively flow path profiles from a grid based DEM\n"
		"Use a left mouse button click to create a flow profile starting from the clicked point.")
	);

	Parameters.Add_Grid(
		NULL, "DEM"		, _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL, "VALUES"	, _TL("Values"),
		_TL("Additional values that shall be saved to the output table."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL, "POINTS"	, _TL("Profile Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL, "LINE"	, _TL("Profile Line"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Flow_Profile::On_Execute(void)
{
	m_pDEM		= Parameters("DEM"   )->asGrid();
	m_pValues	= Parameters("VALUES")->asGridList();
	m_pPoints	= Parameters("POINTS")->asShapes();
	m_pLines	= Parameters("LINE"  )->asShapes();

	//-----------------------------------------------------
	m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", m_pDEM->Get_Name(), _TL("Profile")));

	m_pPoints->Add_Field("ID"           , SG_DATATYPE_Int);
	m_pPoints->Add_Field(_TL("Distance"), SG_DATATYPE_Double);
	m_pPoints->Add_Field(_TL("Overland"), SG_DATATYPE_Double);
	m_pPoints->Add_Field("X"            , SG_DATATYPE_Double);
	m_pPoints->Add_Field("Y"            , SG_DATATYPE_Double);
	m_pPoints->Add_Field("Z"            , SG_DATATYPE_Double);

	for(int i=0; i<m_pValues->Get_Grid_Count(); i++)
	{
		m_pPoints->Add_Field(m_pValues->Get_Grid(i)->Get_Name(), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	m_pLines->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s]", m_pDEM->Get_Name(), _TL("Profile")));
	m_pLines->Add_Field("ID"	, SG_DATATYPE_Int);
	m_pLine	= m_pLines->Add_Shape();
	m_pLine->Set_Value(0, 1);

	//-----------------------------------------------------
	DataObject_Update(m_pDEM  , SG_UI_DATAOBJECT_SHOW_NEW_MAP );
	DataObject_Update(m_pLines, SG_UI_DATAOBJECT_SHOW_LAST_MAP);

	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_NONE);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Flow_Profile::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
	case TOOL_INTERACTIVE_LDOWN:
	case TOOL_INTERACTIVE_MOVE_LDOWN:
		return( Set_Profile(Get_System().Fit_to_Grid_System(ptWorld)) );

	default:
		return( false );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Flow_Profile::Set_Profile(TSG_Point ptWorld)
{
	int		x, y;

	if( !Get_System().Get_World_to_Grid(x, y, ptWorld) || !m_pDEM->is_InGrid(x, y) )
	{
		return( false );
	}

	m_pPoints->Del_Shapes();
	m_pLine  ->Del_Parts ();

	//-----------------------------------------------------
	int		Direction;

	while( Add_Point(x, y) && (Direction = m_pDEM->Get_Gradient_NeighborDir(x, y)) >= 0 )
	{
		x	+= Get_xTo(Direction);
		y	+= Get_yTo(Direction);
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGrid_Flow_Profile::Add_Point(int x, int y)
{
	if( !m_pDEM->is_InGrid(x, y) )
	{
		return( false );
	}

	TSG_Point	Point	= Get_System().Get_Grid_to_World(x, y);

	double	d, dSurface;

	if( m_pPoints->Get_Count() == 0 )
	{
		d	= dSurface	= 0.0;
	}
	else
	{
		CSG_Shape	*pLast	= m_pPoints->Get_Shape(m_pPoints->Get_Count() - 1);

		d			= SG_Get_Distance(Point, pLast->Get_Point(0));

		dSurface	= pLast->asDouble(5) - m_pDEM->asDouble(x, y);
		dSurface	= sqrt(d*d + dSurface*dSurface);

		d			+= pLast->asDouble(1);
		dSurface	+= pLast->asDouble(2);
	}

	CSG_Shape	*pPoint	= m_pPoints->Add_Shape();

	pPoint->Add_Point(Point);

	pPoint->Set_Value(0, m_pPoints->Get_Count());
	pPoint->Set_Value(1, d);
	pPoint->Set_Value(2, dSurface);
	pPoint->Set_Value(3, Point.x);
	pPoint->Set_Value(4, Point.y);
	pPoint->Set_Value(5, m_pDEM->asDouble(x, y));

	for(int i=0; i<m_pValues->Get_Grid_Count(); i++)
	{
		pPoint->Set_Value(VALUE_OFFSET + i, m_pValues->Get_Grid(i)->asDouble(x, y));
	}

	m_pLine->Add_Point(Point);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
