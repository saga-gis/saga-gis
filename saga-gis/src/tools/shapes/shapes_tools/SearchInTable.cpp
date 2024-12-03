
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "SearchInTable.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_String::CSelect_String(void)
{
	Set_Name       (_TL("Select by Attributes... (String Expression)"));

	Set_Author     ("V.Olaya (c) 2004, O.Conrad (c) 2011");

	Set_Description(_TW(
		"Searches for an character string expression in the attributes table and selects records where the expression is found."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES"    , _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("SHAPES",
		"FIELD"     , _TL("Attribute"),
		_TL("attribute to be searched; if not set all attributes will be searched"),
		true
	);

	Parameters.Add_String("",
		"EXPRESSION", _TL("Expression"),
		_TL(""),
		""
	);

	Parameters.Add_Bool("",
		"CASE"      , _TL("Case Sensitive"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"COMPARE"   , _TL("Select if..."),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("attribute is identical with search expression"),
			_TL("attribute contains search expression"),
			_TL("attribute is contained in search expression")
		), 1
	);

	Parameters.Add_Bool("COMPARE",
		"INVERSE"   , _TL("Inverse"),
		_TL(""),
		false
	);

	Parameters.Add_Choice("",
		"METHOD"    , _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("new selection"),
			_TL("add to current selection"),
			_TL("select from current selection"),
			_TL("remove from current selection")
		), 0
	);

	Parameters.Add_Choice("",
		"POSTJOB"   , _TL("Post Job"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("none"),
			_TL("copy"),
			_TL("move"),
			_TL("delete")
		), 0
	);

	Parameters.Add_Shapes("",
		"COPY"      , _TL("Copy"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSelect_String::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POSTJOB") )
	{
		pParameters->Set_Enabled("COPY", pParameter->asInt() == 1 || pParameter->asInt() == 2);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSelect_String::On_Execute(void)
{
	CSG_Shapes *pShapes = Parameters("SHAPES")->asShapes();

	m_Field      = Parameters("FIELD"     )->asInt   ();
	m_Expression = Parameters("EXPRESSION")->asString();
	m_Case       = Parameters("CASE"      )->asBool  ();
	m_Compare    = Parameters("COMPARE"   )->asInt   ();
	int	  Method = Parameters("METHOD"    )->asInt   ();
	bool Inverse = Parameters("INVERSE"   )->asBool  ();

	//-----------------------------------------------------
	if( m_Case == false )
	{
		m_Expression.Make_Upper();
	}

	//-----------------------------------------------------
	for(sLong i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
	{
		CSG_Shape *pShape = pShapes->Get_Shape(i);

		bool bSelect = Do_Select(pShape) ? !Inverse : Inverse;

		switch( Method )
		{
		default: // New selection
			if( ( pShape->is_Selected() && !bSelect)
			||  (!pShape->is_Selected() &&  bSelect) )
			{
				pShapes->Select(i, true);
			}
			break;

		case  1: // Add to current selection
			if(  !pShape->is_Selected() &&  bSelect )
			{
				pShapes->Select(i, true);
			}
			break;

		case  2: // Select from current selection
			if(   pShape->is_Selected() && !bSelect )
			{
				pShapes->Select(i, true);
			}
			break;

		case  3: // Remove from current selection
			if(   pShape->is_Selected() &&  bSelect )
			{
				pShapes->Select(i, true);
			}
			break;
		}
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s: %lld", _TL("selected shapes"), pShapes->Get_Selection_Count());

	if( Parameters("POSTJOB")->asInt() == 1 || Parameters("POSTJOB")->asInt() == 2 ) // copy
	{
		CSG_Shapes *pCopy = Parameters("COPY")->asShapes(); pCopy->Create(pShapes->Get_Type(), CSG_String::Format("%s [%s]", pShapes->Get_Name(), _TL("Selection")), pShapes, pShapes->Get_Vertex_Type());

		for(sLong i=0; i<pShapes->Get_Selection_Count() && Set_Progress(i, pShapes->Get_Selection_Count()); i++)
		{
			pCopy->Add_Shape(pShapes->Get_Selection(i));
		}
	}

	if( Parameters("POSTJOB")->asInt() == 2 || Parameters("POSTJOB")->asInt() == 3 ) // delete
	{
		pShapes->Del_Selection();
	}

	DataObject_Update(pShapes);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSelect_String::Do_Compare(const SG_Char *Value)
{
	CSG_String s(Value);

	if( m_Case == false )
	{
		s.Make_Upper();
	}

	switch( m_Compare )
	{
	case 0:	// identical
		return( m_Expression.Cmp(s) == 0 );

	case 1:	// contains
		return( s.Find(m_Expression) >= 0 );

	case 2:	// contained
		return( m_Expression.Find(s) >= 0 );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CSelect_String::Do_Select(CSG_Shape *pShape)
{
	if( m_Field >= 0 && m_Field < pShape->Get_Table()->Get_Field_Count() )
	{
		return( Do_Compare(pShape->asString(m_Field)) );
	}

	for(int i=0; i<pShape->Get_Table()->Get_Field_Count(); i++)
	{
		if( Do_Compare(pShape->asString(i)) )
		{
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
