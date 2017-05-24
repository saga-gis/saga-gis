
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
//                   select_points.cpp                   //
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
#include "select_points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_Points::CSelect_Points(void)
{
	Set_Name		(_TL("Select Points"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "SELECTION"	, _TL("Selection"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "MAXNUM"		, _TL("Maximum Number of Points"),
		_TL(""),
		PARAMETER_TYPE_Int, 0.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "QUADRANT"	, _TL("Quadrant"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("all quadrants"),
			_TL("1. quadrant (upper right)"),
			_TL("2. quadrant (lower right)"),
			_TL("3. quadrant (lower left)"),
			_TL("4. quadrant (upper left)")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "ADDCENTER"	, _TL("Add Center"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSelect_Points::On_Execute(void)
{
	//-----------------------------------------------------
	m_pPoints		= Parameters("POINTS")		->asShapes();
	m_pSelection	= Parameters("SELECTION")	->asShapes();
	m_Radius		= Parameters("RADIUS")		->asDouble();
	m_MaxPoints		= Parameters("MAXNUM")		->asInt();
	m_Quadrant		= Parameters("QUADRANT")	->asInt() - 1;
	m_bAddCenter	= Parameters("ADDCENTER")	->asBool();

	//-----------------------------------------------------
	if( !m_pPoints->is_Valid() )
	{
		Error_Set(_TL("invalid points layer"));

		return( false );
	}

	if( m_pPoints->Get_Count() <= 0 )
	{
		Error_Set(_TL("no points in layer"));

		return( false );
	}

	if( !m_Search.Create(m_pPoints, -1) )
	{
		Error_Set(_TL("failed to initialise search engine"));

		return( false );
	}

	//-----------------------------------------------------
	m_pSelection->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), m_pPoints->Get_Name(), _TL("Selection")), m_pPoints);

	m_pSelection->Add_Field(_TL("Order")	, SG_DATATYPE_Int);
	m_pSelection->Add_Field(_TL("Distance")	, SG_DATATYPE_Double);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSelect_Points::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
	case TOOL_INTERACTIVE_LUP:
		m_pSelection->Del_Records();

		if( m_bAddCenter )
		{
			m_pSelection->Add_Shape()->Add_Point(ptWorld);
		}

		if( m_Search.Select_Nearest_Points(ptWorld.Get_X(), ptWorld.Get_Y(), m_MaxPoints, m_Radius, m_Quadrant) )
		{
			for(int i=0; i<m_Search.Get_Selected_Count(); i++)
			{
				CSG_Shape	*pPoint	= m_pSelection->Add_Shape(m_pPoints->Get_Shape((int)m_Search.Get_Selected_Z(i)));

				pPoint->Set_Value(m_pSelection->Get_Field_Count() - 2, i + 1);
				pPoint->Set_Value(m_pSelection->Get_Field_Count() - 1, m_Search.Get_Selected_Distance(i));
			}
		}

		DataObject_Update(m_pSelection);

		break;

	default:
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
