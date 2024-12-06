
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "QueryBuilder.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_Numeric::CSelect_Numeric(void)
{
	Set_Name		(_TL("Select by Attributes... (Numerical Expression)"));

	Set_Author		("V.Olaya (c) 2004, O.Conrad (c) 2011");

	Set_Description(_TW(
		"Selects records for which the expression evaluates to non-zero. "
		"The expression syntax is the same as the one for the table calculator. "
		"If an attribute field is selected, the expression evaluates only "
		"this attribute, which can be addressed with the letter 'x' in the "
		"expression formula. If no attribute is selected, attributes are addressed "
		"by the character 'f' (for 'field') followed by the field number "
		"(i.e.: f1, f2, ..., fn) or by the field in quota (e.g.: \"Field Name\").\n"
		"Examples:<ul>"
		"<li>f1 > f2</li>"
		"<li>eq(\"Population\" * 0.001, \"Area\")</li>"
		"</ul>"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES"    , _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("SHAPES",
		"FIELD"     , _TL("Attribute"),
		_TL("attribute to be evaluated by expression ('x'); if not set all attributes can be addressed (f1, f2, ..., fn)."),
		true
	);

	Parameters.Add_String("",
		"EXPRESSION", _TL("Expression"),
		_TL(""),
		"f1 > 0"
	);

	Parameters.Add_Bool("EXPRESSION",
		"INVERSE"   , _TL("Inverse"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"USE_NODATA", _TL("Use No-Data"),
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
int CSelect_Numeric::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
bool CSelect_Numeric::On_Execute(void)
{
	CSG_Shapes *pShapes = Parameters("SHAPES")->asShapes();

	if( pShapes->Get_Count() < 1 || pShapes->Get_Field_Count() < 1 )
	{
		Error_Set(_TL("empty or invalid shapes layer"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Formula Formula; CSG_Array_Int Fields;

	if( !Formula.Set_Formula(Get_Formula(Parameters("EXPRESSION")->asString(), pShapes, Fields)) )
	{
		CSG_String Message;

		if( Formula.Get_Error(Message) )
		{
			Error_Set(Message);
		}

		return( false );
	}

	//-----------------------------------------------------
	int     Method = Parameters("METHOD"    )->asInt ();
	bool   Inverse = Parameters("INVERSE"   )->asBool();
	bool UseNoData = Parameters("USE_NODATA")->asBool();
	bool    Single = Parameters("FIELD"     )->asInt () >= 0;

	CSG_Vector Values((int)Fields.Get_Size());

	//-----------------------------------------------------
	for(sLong i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
	{
		CSG_Shape *pShape = pShapes->Get_Shape(i);

		bool bOkay = true;

		for(sLong Field=0; bOkay && Field<Fields.Get_Size(); Field++)
		{
			if( (bOkay = UseNoData || !pShape->is_NoData(Fields[Field])) == true )
			{
				Values[Field] = pShape->asDouble(Fields[Field]);
			}
		}

		//-------------------------------------------------
		if( bOkay )
		{
			bool bSelect = (Single ? Formula.Get_Value(Values[0]) : Formula.Get_Value(Values)) ? !Inverse : Inverse;

			switch( Method )
			{
			default: // New selection
				if( ( pShape->is_Selected() && !bSelect)
				||	(!pShape->is_Selected() &&  bSelect) )
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
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s: %lld", _TL("selected records"), pShapes->Get_Selection_Count());

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
CSG_String CSelect_Numeric::Get_Formula(CSG_String Formula, CSG_Table *pShapes, CSG_Array_Int &Fields)
{
	Fields.Destroy();

	if(	Parameters("FIELD")->asInt() >= 0 )
	{
		Fields += Parameters("FIELD")->asInt();

		Formula.Replace_Single_Char('a', 'x');

		return( Formula );
	}

	//---------------------------------------------------------
	const SG_Char vars[27] = SG_T("abcdefghijklmnopqrstuvwxyz");

	for(int Field=pShapes->Get_Field_Count()-1; Field>=0 && Fields.Get_Size()<26; Field--)
	{
		bool bUse = false;

		#define FINDVAR(fmt, val) { CSG_String s; s.Printf(fmt, val); if( Formula.Find(s) >= 0 )\
			{ Formula.Replace(s, CSG_String(vars[Fields.Get_Size()])); bUse = true; }\
		}

		FINDVAR("f%d", Field + 1);
		FINDVAR("F%d", Field + 1);
		FINDVAR( "[%s]" , pShapes->Get_Field_Name(Field));
		FINDVAR("\"%s\"", pShapes->Get_Field_Name(Field));

		if( bUse )
		{
			Fields += Field;
		}
	}

	return( Formula );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
