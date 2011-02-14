/*******************************************************************************
    SelectByTheme.cpp
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "SelectByTheme.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_Location::CSelect_Location(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Select by Location..."));

	Set_Author		(SG_T("V.Olaya (c) 2004, O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Select by location."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes to Select From"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "LOCATIONS"	, _TL("Locations"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		NULL	, "CONDITION"	, _TL("Condition"), 
		_TL("Select shapes that fulfil this condition"),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("intersect"),
			_TL("are completely within"),
			_TL("completely contain"),
			_TL("have their centroid in"),
			_TL("contain the centeroid of")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("new selection"),
			_TL("add to current selection"),
			_TL("select from current selection"),
			_TL("remove from current selection")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSelect_Location::On_Execute(void)
{
	int		Method, Condition;

	m_pShapes		= Parameters("SHAPES")		->asShapes();
	m_pLocations	= Parameters("LOCATIONS")	->asShapes();
	Condition		= Parameters("CONDITION")	->asInt();
	Method			= Parameters("METHOD")		->asInt();

	//-----------------------------------------------------
	switch( Condition )
	{
	case 0:	// intersect
		if( ((m_pShapes   ->Get_Type() == SHAPE_TYPE_Point || m_pShapes   ->Get_Type() == SHAPE_TYPE_Points) && m_pLocations->Get_Type() != SHAPE_TYPE_Polygon)
		||	((m_pLocations->Get_Type() == SHAPE_TYPE_Point || m_pLocations->Get_Type() == SHAPE_TYPE_Points) && m_pShapes   ->Get_Type() != SHAPE_TYPE_Polygon) )
		{
			Error_Set(_TL("points can only intersect with polygons"));

			return( false );
		}
		break;

	case 1:	// are completely within
	case 3:	// have their centroid in
		if( m_pLocations->Get_Type() != SHAPE_TYPE_Polygon )
		{
			Error_Set(_TL("this operation requires locations to be of type polygon"));

			return( false );
		}
		break;

	case 2:	// completely contain
	case 4:	// contain the centroid of
		if( m_pShapes->Get_Type() != SHAPE_TYPE_Polygon )
		{
			Error_Set(_TL("this operation requires selectable shapes to be of type polygon"));

			return( false );
		}
		break;
	}

	//-----------------------------------------------------
	for(int i=0; i<m_pShapes->Get_Count() && Set_Progress(i, m_pShapes->Get_Count()); i++)
	{
		CSG_Shape	*pShape	= m_pShapes->Get_Shape(i);

		switch( Method )
		{
		case 0:	// New selection
			if( ( pShape->is_Selected() && !Do_Select(pShape, Condition))
			||	(!pShape->is_Selected() &&  Do_Select(pShape, Condition)) )
			{
				m_pShapes->Select(i, true);
			}
			break;

		case 1:	// Add to current selection
			if(  !pShape->is_Selected() &&  Do_Select(pShape, Condition) )
			{
				m_pShapes->Select(i, true);
			}
			break;

		case 2:	// Select from current selection
			if(   pShape->is_Selected() && !Do_Select(pShape, Condition) )
			{
				m_pShapes->Select(i, true);
			}
			break;

		case 3:	// Remove from current selection
			if(   pShape->is_Selected() &&  Do_Select(pShape, Condition) )
			{
				m_pShapes->Select(i, true);
			}
			break;
		}
	}

	//-----------------------------------------------------
	Message_Add(CSG_String::Format(SG_T("%s: %d"), _TL("selected shapes"), m_pShapes->Get_Selection_Count()));

	DataObject_Update(m_pShapes);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSelect_Location::Do_Select(CSG_Shape *pShape, int Condition)
{
	for(int i=0; i<m_pLocations->Get_Count() && Process_Get_Okay(); i++)
	{
		CSG_Shape	*pLocation	= m_pLocations->Get_Shape(i);

		if( pShape->Intersects(pLocation->Get_Extent()) )
		{
			switch( Condition )
			{
			case 0: // intersect
				if( pLocation->Intersects(pShape) )
				{
					return( true );
				}
				break;

			case 1: // are completely within
				if( pLocation->Intersects(pShape) == INTERSECTION_Contains )
				{
					return( true );
				}
				break;

			case 2: // completely contain
				if( pShape->Intersects(pLocation) == INTERSECTION_Contains )
				{
					return( true );
				}
				break;

			case 3: // have their centroid in
				if( ((CSG_Shape_Polygon *)pLocation)->Contains(pShape->Get_Centroid()) )
				{
					return( true );
				}
				break;

			case 4: // contain the centroid of
				if( ((CSG_Shape_Polygon *)pShape)->Contains(pLocation->Get_Centroid()) )
				{
					return( true );
				}
				break;
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
