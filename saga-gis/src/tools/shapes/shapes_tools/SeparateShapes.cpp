
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
	Set_Name		(_TL("Split Shapes Layer Completely"));

	Set_Author		("V.Olaya (c) 2005");

	Set_Description	(_TW(
		"Copies each shape of given layer to a separate target layer."
	));

	Parameters.Add_Shapes("",
		"SHAPES", _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("SHAPES",
		"FIELD"	, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Shapes_List("",
		"LIST"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);
	
	Parameters.Add_Choice("",
		"NAMING", _TL("Name by..."),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("number of order"),
			_TL("attribute")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSeparateShapes::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("NAMING") )
	{
		pParameters->Set_Enabled("FIELD", pParameter->asInt() == 1);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSeparateShapes::On_Execute(void)
{
	CSG_Shapes	&Shapes	= *Parameters("SHAPES")->asShapes();

	if( !Shapes.is_Valid() || Shapes.Get_Count() < 1 )
	{
		return( false );
	}

	CSG_Parameter_Shapes_List	&List	= *Parameters("LIST")->asShapesList();

	int	Naming	= Parameters("NAMING")->asInt();
	int	Field	= Parameters("FIELD" )->asInt();

	for(int i=0; i<Shapes.Get_Count() && Set_Progress(i, Shapes.Get_Count()); i++)
	{
		CSG_Shapes	*pShapes	= SG_Create_Shapes(Shapes.Get_Type(), NULL, &Shapes);

		pShapes->Add_Shape(Shapes.Get_Shape(i));

		switch( Naming )
		{
		default: pShapes->Fmt_Name("%s [%04d]", Shapes.Get_Name(), 1 +    i                 ); break;
		case  1: pShapes->Fmt_Name("%s [%s]"  , Shapes.Get_Name(), Shapes[i].asString(Field)); break;
		}

		List.Add_Item(pShapes);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
