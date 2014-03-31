/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    SeparateShapes.cpp
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
#include "SeparateShapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSeparateShapes::CSeparateShapes(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Split Shapes Layer Completely"));

	Set_Author		(SG_T("Victor Olaya (c) 2005"));

	Set_Description	(_TW(
		"Copies each shape of given layer to a separate target layer."
	));

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"	, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"	, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Shapes_List(
		NULL	, "LIST"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);
	
	Parameters.Add_Choice(
		NULL	, "NAMING"	, _TL("Name by..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("number of order"),
			_TL("attribute")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSeparateShapes::On_Execute(void)
{
	int							Naming, Field;
	CSG_Shapes					*pShapes;
	CSG_Parameter_Shapes_List	*pList;

	pShapes	= Parameters("SHAPES")	->asShapes();
	pList	= Parameters("LIST")	->asShapesList();
	Naming	= Parameters("NAMING")	->asInt();
	Field	= Parameters("FIELD")	->asInt();

	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_String	Name;

		switch( Naming )
		{
		case 0:	default:
			Name.Printf(SG_T("%s [%04d]"), pShapes->Get_Name(), iShape + 1);
			break;

		case 1:
			Name.Printf(SG_T("%s [%s]")  , pShapes->Get_Name(), pShapes->Get_Record(iShape)->asString(Field));
			break;
		}

		CSG_Shapes	*pShape	= SG_Create_Shapes(pShapes->Get_Type(), Name, pShapes);

		pList	->Add_Item(pShape);
		pShape	->Add_Shape(pShapes->Get_Shape(iShape));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
