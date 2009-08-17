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

//---------------------------------------------------------
#include "Polygon_Clipper.h"

#include "SelectByTheme.h"

//---------------------------------------------------------
#define METHOD_NEW_SEL			0
#define METHOD_ADD_TO_SEL		1
#define METHOD_SELECT_FROM_SEL	2

//---------------------------------------------------------
CSelectByTheme::CSelectByTheme(void){

	Set_Name		(_TL("Select by theme"));

	Set_Author		(SG_T("(c) 2004 by Victor Olaya"));

	Set_Description	(_TW(
		"Select by Theme. Currently both input layers have to be of type polygon!"
	));

	Parameters.Add_Shapes(
		NULL, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL, "SHAPES2"		, _TL("Shapes 2"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL, "CONDITION"	, _TL("Condition"), 
		_TL("Select features in Shapes 1 that fulfil this condition"),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Intersect"),
			_TL("Are completely within"),
			_TL("Completely contain"),
			_TL("Have their center in"),
			_TL("Contain the center of")
		), 0
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("New selection"),
			_TL("Add to current selection"),
			_TL("Select from current selection")
		), 0
	);
}

//---------------------------------------------------------
bool CSelectByTheme::On_Execute(void)
{
	bool		*WasSelected;
	int			i, iMethod, iCondition;
	CSG_Shapes	*pShapes, *pShapes2;

	pShapes		= Parameters("SHAPES")		->asShapes();
	pShapes2	= Parameters("SHAPES2")		->asShapes();
	iCondition	= Parameters("CONDITION")	->asInt();
	iMethod		= Parameters("METHOD")		->asInt();

	//-----------------------------------------------------
	if( iMethod == METHOD_SELECT_FROM_SEL )
	{
		WasSelected	= new bool[pShapes->Get_Count()];

		for(i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
		{
			WasSelected[i]	= pShapes->Get_Record(i)->is_Selected();
		}
	}

	if( iMethod != METHOD_ADD_TO_SEL )
	{
		pShapes->Select();
	}

	//-----------------------------------------------------
	if( Select(pShapes, pShapes2, iCondition, false) )
	{
		for(i=0; i<m_Selection.size() && Set_Progress(i, m_Selection.size()); i++)
		{
			int	iSelection	= m_Selection[i];

			if( !pShapes->Get_Record(iSelection)->is_Selected() )
			{
				if( iMethod != METHOD_SELECT_FROM_SEL || WasSelected[iSelection] )
				{
					((CSG_Table *)pShapes)->Select(iSelection, true);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( iMethod == METHOD_SELECT_FROM_SEL )
	{
		delete(WasSelected);
	}

	Message_Add(CSG_String::Format(SG_T("%s: %d"), _TL("selected shapes"), m_Selection.size()));

	DataObject_Update(pShapes);

	return( true );
}

//---------------------------------------------------------
bool CSelectByTheme::Select(CSG_Shapes *pShapes, CSG_Shapes *pShapes2, int iCondition, bool bFromSelection)
{
	CSG_Shapes			Intersect(SHAPE_TYPE_Polygon);
	CSG_Shape_Polygon	*pIntersect	= (CSG_Shape_Polygon *)Intersect.Add_Shape();

	m_Selection.clear();

	for(int i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
	{
		CSG_Shape_Polygon	*pShape	= (CSG_Shape_Polygon *)pShapes->Get_Shape(i);

		bool	bSelect	= false;

		for(int j=0; !bSelect && j<pShapes2->Get_Count(); j++)
		{
			if( !bFromSelection || pShapes2->Get_Record(j)->is_Selected() )
			{
				CSG_Shape_Polygon	*pShape2 = (CSG_Shape_Polygon *)pShapes2->Get_Shape(j);

				switch( iCondition )
				{
				case 0: //intersect
					if( GPC_Intersection(pShape, pShape2, pIntersect) )
					{
						bSelect = true;
					}
					break;

				case 1: //are completely within
					if( GPC_Intersection(pShape, pShape2, pIntersect)
					&&  pIntersect->Get_Area() == pShape->Get_Area() )
					{
						bSelect = true;
					}
					break;

				case 2: //Completely contain
					if( GPC_Intersection(pShape, pShape2, pIntersect)
					&&	pIntersect->Get_Area() == pShape2->Get_Area() )
					{
						bSelect = true;
					}
					break;

				case 3: //have their center in
					if( pShape2->is_Containing(pShape->Get_Centroid()) )
					{
						bSelect = true;
					}
					break;

				case 4: //contain center of
					if( pShape->is_Containing(pShape2->Get_Centroid()) )
					{
						bSelect = true;
					}
					break;
				}

				if( bSelect )
				{
					m_Selection.push_back(i);
				}
			}
		}
	}

	return( m_Selection.size() > 0 );
}
