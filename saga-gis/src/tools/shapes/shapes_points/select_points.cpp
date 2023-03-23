
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

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("", "POINTS"   , _TL("Points"                  ), _TL(""), PARAMETER_INPUT , SHAPE_TYPE_Point);
	Parameters.Add_Shapes("", "SELECTION", _TL("Selection"               ), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Point);
	Parameters.Add_Double("", "RADIUS"   , _TL("Radius"                  ), _TL("ignored if zero"), 1000., 0., true);
	Parameters.Add_Int   ("", "MAXNUM"   , _TL("Maximum Number of Points"), _TL("ignored if zero"),    0 , 0 , true);
	Parameters.Add_Bool  ("", "MULTIPLE" , _TL("Multiple Selections"     ), _TL(""), false);
	Parameters.Add_Bool  ("", "ADDCENTER", _TL("Add Center Point"        ), _TL(""), false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSelect_Points::On_Execute(void)
{
	m_pPoints    = Parameters("POINTS"   )->asShapes();
	m_pSelection = Parameters("SELECTION")->asShapes();
	m_Radius     = Parameters("RADIUS"   )->asDouble();
	m_MaxPoints  = Parameters("MAXNUM"   )->asInt();
	m_bMultiple  = Parameters("MULTIPLE" )->asBool();
	m_bAddCenter = Parameters("ADDCENTER")->asBool();

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

	if( m_MaxPoints <= 0 && m_Radius <= 0. )
	{
		Error_Set(_TL("either maximum point number or radius have to be greater zero"));

		return( false );
	}

	if( !m_Search.Create(m_pPoints) )
	{
		Error_Set(_TL("failed to initialise search engine"));

		return( false );
	}

	//-----------------------------------------------------
	m_pSelection->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s]", m_pPoints->Get_Name(), _TL("Selection")), m_pPoints);

	m_pSelection->Add_Field(_TL("Order"   ), SG_DATATYPE_Int   );
	m_pSelection->Add_Field(_TL("Distance"), SG_DATATYPE_Double);

	return( true );
}

//---------------------------------------------------------
bool CSelect_Points::On_Execute_Finish(void)
{
	m_Search.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSelect_Points::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
	case TOOL_INTERACTIVE_LUP:
		m_pSelection->Del_Records();

		if( m_bMultiple == false )
		{
			m_pSelection->Del_Shapes();
		}

		if( m_bAddCenter )
		{
			m_pSelection->Add_Shape()->Add_Point(ptWorld);
		}

		if( m_Search.Get_Nearest_Points(ptWorld.x, ptWorld.y, m_MaxPoints, m_Radius) )
		{
			for(size_t i=0; i<m_Search.Get_Match_Count(); i++)
			{
				CSG_Shape &Point = *m_pSelection->Add_Shape(m_pPoints->Get_Shape(m_Search.Get_Match_Index(i)));

				Point.Set_Value(m_pSelection->Get_Field_Count() - 2, i + 1);
				Point.Set_Value(m_pSelection->Get_Field_Count() - 1, m_Search.Get_Match_Distance(i));
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
