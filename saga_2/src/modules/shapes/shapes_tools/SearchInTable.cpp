/*******************************************************************************
    SearchInTable.cpp
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
#include <vector>

#include "SearchInTable.h"

//---------------------------------------------------------
#define METHOD_NEW_SEL			0
#define METHOD_ADD_TO_SEL		1
#define METHOD_SELECT_FROM_SEL	2

//---------------------------------------------------------
CSearchInTable::CSearchInTable(void)
{
	Set_Name		(_TL("Search in attributes table"));

	Set_Author		(SG_T("(c) 2004 by Victor Olaya"));

	Set_Description	(_TW(
		"(c) 2004 Victor Olaya. Searches for an expression in the attributes table and selects records where the expression is found"
	));

	Parameters.Add_Shapes(
		NULL, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_String(
		NULL, "EXPRESSION"	, _TL("Expression"),
		_TL(""),
		SG_T("")
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
bool CSearchInTable::On_Execute(void)
{
	bool		*WasSelected;
	int			i, iMethod;
	CSG_String	sExpression;
	CSG_Shapes	*pShapes;

	pShapes		= Parameters("SHAPES")		->asShapes();
	sExpression	= Parameters("EXPRESSION")	->asString();
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
	std::vector <int> m_Selection;

	for(i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(i);

		for(int j=0; j<pShapes->Get_Field_Count(); j++)
		{
			CSG_String	sValue	= pShape->asString(j);

			if( sValue.Find(sExpression) != -1 )
			{
				m_Selection.push_back(i);
				break;
			}
		}
	}

	//-----------------------------------------------------
	for(i=0; i<m_Selection.size() && Set_Progress(i, m_Selection.size()); i++)
	{
		int	iSelection = m_Selection[i];

		if( !pShapes->Get_Record(iSelection)->is_Selected() )
		{
			if( iMethod != METHOD_SELECT_FROM_SEL || WasSelected[iSelection] )
			{
				((CSG_Table *)pShapes)->Select(iSelection, true);
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
