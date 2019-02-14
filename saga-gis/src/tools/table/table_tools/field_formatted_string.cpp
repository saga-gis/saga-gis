
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     table_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               field_formatted_string.cpp              //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
#include "field_formatted_string.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum EKeys
{
	Key_Index	= 0,
	Key_String,
	Key_Lower,
	Key_Upper,
	Key_Integer,
	Key_Real,
	Key_Count
};

//---------------------------------------------------------
const CSG_String	g_Keys[Key_Count][3]	=
{
	{	"index"  , "index(offset = 0)"     , "record's index"	},
	{	"string" , "string(field)"         , "field's content as it is"	},
	{	"lower"  , "lower(field)"          , "field's content as lower case text"	},
	{	"upper"  , "upper(field)"          , "field's content as upper case text"	},
	{	"integer", "integer(field)"        , "field's content as integer number"	},
	{	"real"   , "real(field, precision)", "field's content as real number with optional precision argument"	},
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CField_Formatted_String_Base::CField_Formatted_String_Base(bool bShapes)
{
	Set_Name		(_TL("Formatted Text"));

	Set_Author		("O.Conrad (c) 2019");

	CSG_String	s	=_TW(
		"With this tool you can create new text field contents from the contents "
		"of other fields. To address other field's contents you have some format options as listed below.\n"
		"Fields are addressed either by their zero based column number or by their name.\n"
		"If the use <i>no-data flag</i> is unchecked and a no-data value appears in "
		"a record's input fields, the result will be an empty text string.\n"
		"Field contents can be combined using the '+' operator. Free text arguments "
		"have to be added in quota.\n"
		"A simple example:\n<i>\"No. \" + index(1) + \": the value of '\" + upper(0) + \"' is \" + number(1, 2)</i>\n"
	);

	s	+= "<table border=\"0\">";

	for(int i=0; i<Key_Count; i++)
	{
		s	+= "<tr><td><b>" + g_Keys[i][1] + "</b></td><td>" + g_Keys[i][2] + "</td></tr>";
	}

	s	+= "</table>";

	Set_Description(s);

	//-----------------------------------------------------
	if( bShapes )
	{
		Set_Name(CSG_String::Format("%s [%s]", Get_Name().c_str(), _TL("Shapes")));

		Parameters.Add_Shapes("", "TABLE" , _TL("Shapes"), _TL(""), PARAMETER_INPUT);
		Parameters.Add_Shapes("", "RESULT", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}
	else
	{
		Parameters.Add_Table ("", "TABLE" , _TL("Table" ), _TL(""), PARAMETER_INPUT);
		Parameters.Add_Table ("", "RESULT", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}

	//-----------------------------------------------------
	Parameters.Add_Table_Field("TABLE",
		"FIELD"		, _TL("Field"),
		_TL(""),
		true
	);

	Parameters.Add_String("TABLE",
		"NAME"		, _TL("Field Name"),
		_TL(""),
		_TL("New Field")
	);

	Parameters.Add_String("",
		"FORMAT"	, _TL("Format"),
		_TL(""),
		"\"Index: \" + index(1)"
	);

	Parameters.Add_Bool("",
		"SELECTION"	, _TL("Selection"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"USE_NODATA", _TL("Use No-Data"),
		_TL(""),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CField_Formatted_String_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_Table	*pTable	= (CSG_Table *)pParameters->Get_Parameter("TABLE")->asDataObject();

	if( pTable )
	{
		CSG_Parameter	*pField	= pParameters->Get_Parameter("FIELD");

		pParameters->Set_Enabled("FIELD"    , true);
		pParameters->Set_Enabled("NAME"     , pField->asInt() < 0);	// not set
		pParameters->Set_Enabled("SELECTION", pTable->Get_Selection_Count() > 0);
	}
	else
	{
		pParameters->Set_Enabled("FIELD"    , false);
		pParameters->Set_Enabled("NAME"     , false);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CField_Formatted_String_Base::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	if( !pTable->is_Valid() || pTable->Get_Field_Count() <= 0 || pTable->Get_Record_Count() <= 0 )
	{
		Error_Set(_TL("invalid table"));

		return( false );
	}

	//-----------------------------------------------------
	if( !Get_Formats(pTable) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asTable() && Parameters("RESULT")->asTable() != pTable )
	{
		pTable	= Parameters("RESULT")->asTable();

		if( pTable->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
		{
			pTable->Create(*Parameters("TABLE")->asShapes());
		}
		else
		{
			pTable->Create(*Parameters("TABLE")->asTable());
		}
	}

	pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());

	//-----------------------------------------------------
	m_Result	= Parameters("FIELD")->asInt();

	if( m_Result < 0 || m_Result >= pTable->Get_Field_Count() )
	{
		m_Result	= pTable->Get_Field_Count();

		pTable->Add_Field(Parameters("NAME")->asString(), SG_DATATYPE_String);
	}
	else if( pTable->Get_Field_Type(m_Result) != SG_DATATYPE_String )
	{
		pTable->Set_Field_Type(m_Result, SG_DATATYPE_String);
	}

	//-----------------------------------------------------
	m_bNoData	= Parameters("USE_NODATA")->asBool();

	if( pTable->Get_Selection_Count() > 0 && Parameters("SELECTION")->asBool() )
	{
		for(size_t i=0; i<pTable->Get_Selection_Count() && Set_Progress(i, pTable->Get_Selection_Count()); i++)
		{
			Set_String(pTable->Get_Selection(i));
		}
	}
	else
	{
		for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
		{
			Set_String(pTable->Get_Record(i));
		}
	}

	//-----------------------------------------------------
	if( pTable == Parameters("TABLE")->asTable() )
	{
		DataObject_Update(pTable);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CField_Formatted_String_Base::Set_String(CSG_Table_Record *pRecord)
{
	CSG_String	Result;

	for(int i=0; i<m_nFormats; i++)
	{
		if( !m_bNoData && m_Formats[i].field >= 0 && pRecord->is_NoData(m_Formats[i].field) )
		{
			pRecord->Set_Value(m_Result, "");

			return( false );
		}

		CSG_String	s;

		switch( m_Formats[i].type )
		{
		default:
			s	= m_Formats[i].format;
			break;

		case Key_Index:
			s	.Printf("%d", m_Formats[i].option + pRecord->Get_Index());
			break;

		case Key_String:
			s	= pRecord->asString(m_Formats[i].field);
			break;

		case Key_Lower:
			s	= pRecord->asString(m_Formats[i].field); s.Make_Lower();
			break;

		case Key_Upper:
			s	= pRecord->asString(m_Formats[i].field); s.Make_Upper();
			break;

		case Key_Integer:
			s	= SG_Get_String(pRecord->asInt(m_Formats[i].field));
			break;

		case Key_Real:
			s	= SG_Get_String(pRecord->asDouble(m_Formats[i].field), m_Formats[i].option);
			break;
		}

		Result	+= s;
	}

	pRecord->Set_Value(m_Result, Result);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CField_Formatted_String_Base::Get_Formats(CSG_Table *pTable)
{
	CSG_String	s	= Parameters("FORMAT")->asString();

	CSG_Strings	Tokens;	Tokens.Add("");

	bool	bQuota	= false;

	for(size_t iCharacter=0; iCharacter<s.Length(); iCharacter++)
	{
		int	i	= Tokens.Get_Count() - 1;

		switch( s[iCharacter] )
		{
		default:
			Tokens[i]	+= s[iCharacter];
			break;

		case '\"':
			bQuota	= !bQuota;
			Tokens[i]	+= s[iCharacter];
			break;

		case '\\':
			if( ++iCharacter < s.Length() )
			{
				Tokens[i]	+= s[iCharacter];
			}
			break;

		case '+':
			if( !bQuota )
			{
				Tokens.Add("");
			}
			break;
		}
	}

	//-----------------------------------------------------
	if( Tokens.Get_Count() < 1 )
	{
		m_nFormats	= 0;
		m_Formats	= NULL;
	}

	m_nFormats	= Tokens.Get_Count();
	m_Formats	= new TFormat[m_nFormats];

	for(int i=0; i<Tokens.Get_Count(); i++)
	{
		Tokens[i].Trim_Both();

		if( Tokens[i].Find('\"') == 0 )
		{
			m_Formats[i].format	= Tokens[i].AfterFirst('\"').BeforeFirst('\"');
			m_Formats[i].type	= Key_Count;
			m_Formats[i].field	= -1;
			m_Formats[i].option	= 0;
		}
		else
		{
			CSG_String	Key	= Tokens[i].BeforeFirst('(');
			CSG_String	Arg	= Tokens[i].AfterFirst ('(').BeforeFirst(')');

			if( !Key.CmpNoCase(g_Keys[Key_Index][0]) )
			{
				int	Offset;

				m_Formats[i].type	= Key_Index;
				m_Formats[i].option	= Arg.asInt(Offset) ? Offset : 0;
			}
			else
			{
				CSG_String	Field	= Arg.BeforeFirst(',');	Field.Trim_Both();

				m_Formats[i].field	= pTable->Get_Field(Field);

				if( m_Formats[i].field < 0 && !Field.asInt(m_Formats[i].field) )
				{
					Error_Fmt("%s [%s]", _TL("field error"), Tokens[i].c_str());

					return( false );
				}

				if( !Key.CmpNoCase(g_Keys[Key_String ][0]) )	{	m_Formats[i].type	= Key_String ;	}
				if( !Key.CmpNoCase(g_Keys[Key_Lower  ][0]) )	{	m_Formats[i].type	= Key_Lower  ;	}
				if( !Key.CmpNoCase(g_Keys[Key_Upper  ][0]) )	{	m_Formats[i].type	= Key_Upper  ;	}
				if( !Key.CmpNoCase(g_Keys[Key_Integer][0]) )	{	m_Formats[i].type	= Key_Integer;	}
				if( !Key.CmpNoCase(g_Keys[Key_Real   ][0]) )	{	m_Formats[i].type	= Key_Real   ;

					int	Precision;

					m_Formats[i].option	= Arg.AfterFirst(',').asInt(Precision) ? Precision : -99;
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
