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

	Set_Description(_TW(
		"Selects records for which the expression evaluates to non-zero. "
		"The expression syntax is the same as the one for the table calculator. "
		"If an attribute field is selected, the expression evaluates only "
		"this attribute, which can be addressed with the letter 'a' in the "
		"expression formula. If no attribute is selected, attributes are addressed "
		"by the character 'f' (for 'field') followed by the field number "
		"(i.e.: f1, f2, ..., fn) or by the field name in square brackets "
		"(e.g.: [Field Name]).\n"
		"Examples:\n"
		"- f1 > f2\n"
		"- eq([Population] * 2, [Area])\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("SHAPES",
		"FIELD"		, _TL("Attribute"),
		_TL("attribute to be evaluated by expression ('a'); if not set all attributes can be addressed (f1, f2, ..., fn)."),
		true
	);

	Parameters.Add_String("",
		"EXPRESSION", _TL("Expression"),
		_TL(""),
		"a > 0"
	);

	Parameters.Add_Bool("",
		"USE_NODATA", _TL("Use No-Data"),
		_TL(""),
		false
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
	CSG_Table	*pTable	= Parameters("SHAPES")->asShapes();

	if( pTable->Get_Count() < 1 || pTable->Get_Field_Count() < 1 )
	{
		Error_Set(_TL("empty or invalid shapes layer"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Array_Int	Fields;

	CSG_Formula	Formula;

	if( !Formula.Set_Formula(Get_Formula(Parameters("EXPRESSION")->asString(), pTable, Fields)) )
	{
		CSG_String	Message;

		if( Formula.Get_Error(Message) )
		{
			Error_Set(Message);
		}

		return( false );
	}

	//-----------------------------------------------------
	int	Method	= Parameters("METHOD")->asInt();

	bool	bUseNoData	= Parameters("USE_NODATA")->asBool();

	CSG_Vector	Values((int)Fields.Get_Size());

	//-----------------------------------------------------
	for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(i);

		bool	bOkay	= true;

		for(size_t Field=0; bOkay && Field<Fields.Get_Size(); Field++)
		{
			if( (bOkay = bUseNoData || !pRecord->is_NoData(Fields[Field])) == true )
			{
				Values[Field]	= pRecord->asDouble(Fields[Field]);
			}
		}

		//-------------------------------------------------
		if( bOkay )
		{
			switch( Method )
			{
			default:	// New selection
				if( ( pRecord->is_Selected() && !Formula.Get_Value(Values))
				||	(!pRecord->is_Selected() &&  Formula.Get_Value(Values)) )
				{
					pTable->Select(i, true);
				}
				break;

			case  1:	// Add to current selection
				if(  !pRecord->is_Selected() &&  Formula.Get_Value(Values) )
				{
					pTable->Select(i, true);
				}
				break;

			case  2:	// Select from current selection
				if(   pRecord->is_Selected() && !Formula.Get_Value(Values) )
				{
					pTable->Select(i, true);
				}
				break;

			case  3:	// Remove from current selection
				if(   pRecord->is_Selected() &&  Formula.Get_Value(Values) )
				{
					pTable->Select(i, true);
				}
				break;
			}
		}
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s: %d", _TL("selected shapes"), pTable->Get_Selection_Count());

	DataObject_Update(pTable);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSelect_Numeric::Get_Formula(CSG_String Formula, CSG_Table *pTable, CSG_Array_Int &Fields)
{
	const SG_Char	vars[27]	= SG_T("abcdefghijklmnopqrstuvwxyz");

	Fields.Destroy();

	int	Field	= Parameters("FIELD")->asInt();

	if(	Field >= 0 )
	{
	//	Formula.Replace("a", CSG_String(vars[Fields.Get_Size()]));

		Fields	+= Field;

		return( Formula );
	}

	//---------------------------------------------------------
	for(Field=pTable->Get_Field_Count()-1; Field>=0 && Fields.Get_Size()<26; Field--)
	{
		bool	bUse	= false;

		CSG_String	s;

		s.Printf("f%d", Field + 1);

		if( Formula.Find(s) >= 0 )
		{
			Formula.Replace(s, CSG_String(vars[Fields.Get_Size()]));

			bUse	= true;
		}

		s.Printf("F%d", Field + 1);

		if( Formula.Find(s) >= 0 )
		{
			Formula.Replace(s, CSG_String(vars[Fields.Get_Size()]));

			bUse	= true;
		}

		s.Printf("[%s]", pTable->Get_Field_Name(Field));

		if( Formula.Find(s) >= 0 )
		{
			Formula.Replace(s, CSG_String(vars[Fields.Get_Size()]));

			bUse	= true;
		}

		if( bUse )
		{
			Fields	+= Field;
		}
	}

	return( Formula );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
