/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    QueryBuilder.cpp
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "QueryBuilder.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_Numeric::CSelect_Numeric(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Select by Attributes... (Numerical Expression)"));

	Set_Author		("V.Olaya (c) 2004, O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Selects records for which the expression is true."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("SHAPES",
		"FIELD"		, _TL("Attribute"),
		_TL("attribute to be searched; if not set all attributes will be searched"),
		true
	);

	Parameters.Add_String("",
		"EXPRESSION", _TL("Expression"),
		_TL(""),
		"a > 0"
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("new selection"),
			_TL("add to current selection"),
			_TL("select from current selection"),
			_TL("remove from current selection")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSelect_Numeric::On_Execute(void)
{
	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

	int	Field	= Parameters("FIELD" )->asInt();
	int	Method	= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	CSG_Formula	Formula;

	if( !Formula.Set_Formula(Parameters("EXPRESSION")->asString()) )
	{
		CSG_String	Message;

		if( Formula.Get_Error(Message) )
		{
			Error_Set(Message);
		}

		return( false );
	}

	//-----------------------------------------------------
	double	*Values	= new double[pShapes->Get_Field_Count()];

	for(int i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(i);

		if( Field >= pShapes->Get_Field_Count() )
		{
			for(int j=0; j<pShapes->Get_Field_Count(); j++)
			{
				Values[j]	= pShape->asDouble(j);
			}
		}
		else
		{
			Values[0]	= pShape->asDouble(Field);
		}

		switch( Method )
		{
		case 0:	// New selection
			if( ( pShape->is_Selected() && !Formula.Get_Value(Values, pShapes->Get_Field_Count()))
			||	(!pShape->is_Selected() &&  Formula.Get_Value(Values, pShapes->Get_Field_Count())) )
			{
				pShapes->Select(i, true);
			}
			break;

		case 1:	// Add to current selection
			if(  !pShape->is_Selected() &&  Formula.Get_Value(Values, pShapes->Get_Field_Count()) )
			{
				pShapes->Select(i, true);
			}
			break;

		case 2:	// Select from current selection
			if(   pShape->is_Selected() && !Formula.Get_Value(Values, pShapes->Get_Field_Count()) )
			{
				pShapes->Select(i, true);
			}
			break;

		case 3:	// Remove from current selection
			if(   pShape->is_Selected() &&  Formula.Get_Value(Values, pShapes->Get_Field_Count()) )
			{
				pShapes->Select(i, true);
			}
			break;
		}
	}

	delete[](Values);

	//-----------------------------------------------------
	Message_Fmt("\n%s: %d", _TL("selected shapes"), pShapes->Get_Selection_Count());

	DataObject_Update(pShapes);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
