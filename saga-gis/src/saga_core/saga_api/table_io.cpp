
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    table_io.cpp                       //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table.h"
#include "table_dbase.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::On_Reload(void)
{
	return( Create(Get_File_Name(false)) );
}

//---------------------------------------------------------
bool CSG_Table::On_Delete(void)
{
	return( SG_File_Delete(Get_File_Name(false)) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Set_File_Encoding(int Encoding)
{
	if( Encoding >= 0 && Encoding < SG_FILE_ENCODING_UNDEFINED )
	{
		m_Encoding = Encoding;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Load(const CSG_String &FileName, int Format, SG_Char Separator, int Encoding)
{
	Set_File_Encoding(Encoding);

	if( !SG_File_Exists(FileName) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Format == TABLE_FILETYPE_Undefined )
	{
		Format = SG_File_Cmp_Extension(FileName, "dbf") ? TABLE_FILETYPE_DBase : TABLE_FILETYPE_Text;
	}

	//-----------------------------------------------------
	Destroy();

	switch( Format )
	{
	case TABLE_FILETYPE_Text:   default: if( !_Load_Text (FileName, true , Separator) ) return( false ); break;
	case TABLE_FILETYPE_Text_NoHeadLine: if( !_Load_Text (FileName, false, Separator) ) return( false ); break;
	case TABLE_FILETYPE_DBase          : if( !_Load_DBase(FileName                  ) ) return( false ); break;
	}

	//-----------------------------------------------------
	Set_Name(SG_File_Get_Name(FileName, false));

	Load_MetaData(FileName);

	CSG_MetaData *pFields = Get_MetaData_DB().Get_Child("FIELDS");

	if( pFields && pFields->Get_Children_Count() == Get_Field_Count() )
	{
		for(int Field=0; Field<Get_Field_Count(); Field++)
		{
			Set_Field_Name(Field, pFields->Get_Content(Field));
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSG_Table::Save(const CSG_String &FileName, int Format)
{
	return( Save(FileName, Format, '\0', m_Encoding) );
}

//---------------------------------------------------------
bool CSG_Table::Save(const CSG_String &FileName, int Format, SG_Char Separator, int Encoding)
{
	SG_UI_Msg_Add(CSG_String::Format("%s %s: %s...", _TL("Saving"), _TL("table"), FileName.c_str()), true);

	Set_File_Encoding(Encoding);

	//-----------------------------------------------------
	if( Format <= TABLE_FILETYPE_Undefined || Format > TABLE_FILETYPE_DBase )
	{
		if( SG_File_Cmp_Extension(FileName, "dbf") )
		{
			Format = TABLE_FILETYPE_DBase;
		}
		else
		{
			Format = TABLE_FILETYPE_Text;

			if( Separator == '\0' )
			{
				Separator = SG_File_Cmp_Extension(FileName, "csv") ? ',' : '\t'; // comma separated values or tab spaced text
			}
		}
	}

	//-----------------------------------------------------
	bool bResult = false;

	switch( Format )
	{
	case TABLE_FILETYPE_Text:   default: bResult = _Save_Text (FileName, true , Separator); break;
	case TABLE_FILETYPE_Text_NoHeadLine: bResult = _Save_Text (FileName, false, Separator); break;
	case TABLE_FILETYPE_DBase          : bResult = _Save_DBase(FileName                  ); break;
	}

	//-----------------------------------------------------
	CSG_MetaData *pFields = Get_MetaData_DB().Get_Child("FIELDS");

	if( !pFields )
	{
		pFields = Get_MetaData_DB().Add_Child("FIELDS");
	}

	pFields->Del_Children();

	for(int Field=0; Field<Get_Field_Count(); Field++)
	{
		pFields->Add_Child("FIELD", Get_Field_Name(Field))->Add_Property("TYPE", gSG_Data_Type_Identifier[Get_Field_Type(Field)]);
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Set_Modified(false);

		Set_Update_Flag();

		Set_File_Type(Format);

		Set_File_Name(FileName, true);

		Save_MetaData(FileName);

		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                         Text                          //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_Table::_Load_Text_Trim(CSG_String &s, const SG_Char Separator)
{
	for(size_t i=0; i<s.Length(); i++)
	{
		SG_Char c = s[i];

		if( c == Separator || (c != ' ' && c != '\t' && c != '\n' && c != '\v' && c != '\f' && c != '\r') )
		{
			if( i > 0 )
			{
				s = s.Right(s.Length() - i);
			}

			return( i );
		}
	}

	return( 0 );
}

//---------------------------------------------------------
size_t	CSG_Table::_Load_Text_EndQuote(const CSG_String &s, const SG_Char Separator)
{
	if( s.Length() > 1 && s[0] == '\"' )
	{
		bool bInQuotes = true;

		for(size_t i=1; i<s.Length(); i++)
		{
			if( bInQuotes )
			{
				if( s[i] == '\"' )
				{
					bInQuotes = false;
				}
			}
			else if( s[i] == '\"' )
			{
				bInQuotes = true;
			}
			else if( s[i] == Separator )
			{
				return( i );
			}
		}

		if( s[s.Length() - 1] == '\"' )
		{
			return( s.Length() );
		}
	}

	return( 0 );
}

//---------------------------------------------------------
bool CSG_Table::_Load_Text(const CSG_String &FileName, bool bHeadline, const SG_Char _Separator)
{
	CSG_File Stream;

	if( Stream.Open(FileName, SG_FILE_R, false, m_Encoding) == false )
	{
		return( false );
	}

	sLong fLength = Stream.Length();

	if( fLength < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_String Line;

	if( !Stream.Read_Line(Line) ) // end-of-file !
	{
		return( false );
	}

	if( Line[0] == 65279 ) // 65279 => '\uFEFF' => BOM => zero-width no-break space ! ...to do: recognizing other 'magic' first characters !
	{
		Line.Remove(0, 1);
	}

	bool bCSV = SG_File_Cmp_Extension(FileName, "csv");

	while( Line.is_Empty() || (bCSV && Line[0] == '#') ) // empty or comment
	{
		if( !Stream.Read_Line(Line) ) // end-of-file !
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	SG_Char Separator = _Separator;

	if( Separator == '\0' )
	{
		if( bCSV ) // comma separated values
		{
			Separator = Line.Find(';') >= 0 ? ';' : ','; // assume semicolon as value separator, comma as decimal separator!
		}
		else // assume tab spaced text table
		{
			Separator = '\t';
		}
	}

	bool bComma2Point = bCSV && Separator == ';';

	//-----------------------------------------------------
	CSG_Table Table;

	_Load_Text_Trim(Line, Separator);

	while( !Line.is_Empty() )
	{
		CSG_String Value;

		if( Line[0] == '\"' ) // value in quotas
		{
			Value = Line.AfterFirst('\"').BeforeFirst('\"');
			Line  = Line.AfterFirst('\"').AfterFirst ('\"');
		}
		else
		{
			Value = Line.BeforeFirst(Separator);
		}

		Line = Line.AfterFirst(Separator); _Load_Text_Trim(Line, Separator);

		if( !bHeadline || Value.Length() == 0 )
		{
			Value.Printf("F%02d", Table.Get_Field_Count() + 1);
		}

		Table.Add_Field(Value, SG_DATATYPE_String);
	}

	//-----------------------------------------------------
	TSG_Data_Type *Types = new TSG_Data_Type[Table.Get_Field_Count()];

	for(int Field=0; Field<Table.Get_Field_Count(); Field++)
	{
		Types[Field] = SG_DATATYPE_Int;
	}

	if( !bHeadline )
	{
		Stream.Seek_Start();
	}

	while( Stream.Read_Line(Line) && SG_UI_Process_Set_Progress((double)Stream.Tell(), (double)fLength) )
	{
		if( Line.is_Empty() || (bCSV && Line[0] == '#') ) // empty or comment
		{
			continue;
		}

		CSG_Table_Record &Record = *Table.Add_Record();

		_Load_Text_Trim(Line, Separator);

		for(int Field=0; Field<Table.Get_Field_Count() && !Line.is_Empty(); Field++)
		{
			size_t Position = _Load_Text_EndQuote(Line, Separator); CSG_String Value;

			if( Position > 0 ) // value in quotas !!!
			{
				if( Position - 2 > 0 )
				{
					Value = Line.Mid(1, Position - 2);
				}
				else
				{
					Value.Clear();
				}

				Line = Line.Right(Line.Length() - Position);

				Types[Field] = SG_DATATYPE_String;
			}
			else
			{
				Value = Line.BeforeFirst(Separator);

				if( bComma2Point )
				{
					Value.Replace(",", ".");
				}
			}

			Line = Line.AfterFirst(Separator); _Load_Text_Trim(Line, Separator);

			//---------------------------------------------
			if( Types[Field] != SG_DATATYPE_String && !Value.is_Empty() )
			{
				if( Value[0] == '0' && Value[1] != '.' ) // keep leading zero(s) => don't interpret as number !
				{
					Types[Field] = SG_DATATYPE_String;
				}
				else try
				{
					size_t pos; double number = std::stod(Value.to_StdString(), &pos);

					if( pos < Value.Length() )
					{
						Types[Field] = SG_DATATYPE_String;
					}
					else if( Types[Field] != SG_DATATYPE_Double && (number - (int)number != 0. || Value.Find('.') >= 0) )
					{
						Types[Field] = SG_DATATYPE_Double;
					}
				}
				catch(...)
				{
					Types[Field] = SG_DATATYPE_String;
				}
			}

			Record.Set_Value(Field, Value);
		}
	}

	//-----------------------------------------------------
	if( Table.Get_Field_Count() > 0 )
	{
		for(int Field=0; Field<Table.Get_Field_Count(); Field++)
		{
			Add_Field(Table.Get_Field_Name(Field), Types[Field]);
		}

		for(sLong i=0; i<Table.Get_Count() && SG_UI_Process_Set_Progress(i, Table.Get_Count()); i++)
		{
			CSG_Table_Record &Record = *Add_Record();

			for(int Field=0; Field<Get_Field_Count(); Field++)
			{
				if( *Table[i].asString(Field) )
				{
					Record.Set_Value(Field, Table[i].asString(Field));
				}
				else
				{
					Record.Set_NoData(Field);
				}
			}
		}
	}

	delete[](Types);

	SG_UI_Process_Set_Ready();

	return( Get_Field_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Table::_Save_Text(const CSG_String &FileName, bool bHeadline, const SG_Char Separator)
{
	CSG_File Stream;

	if( Get_Field_Count() <= 0 || Stream.Open(FileName, SG_FILE_W, false, m_Encoding) == false )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int Field=0; Field<Get_Field_Count(); Field++)
	{
		Stream.Printf("%s%c", Get_Field_Name(Field), Field < Get_Field_Count() - 1 ? Separator : '\n');
	}

	//-----------------------------------------------------
	for(sLong i=0; i<Get_Count() && SG_UI_Process_Set_Progress(i, Get_Count()); i++)
	{
		CSG_Table_Record &Record = *Get_Record_byIndex(i);

		for(int Field=0; Field<Get_Field_Count(); Field++)
		{
			switch( Get_Field_Type(Field) )
			{
			case SG_DATATYPE_String:
			case SG_DATATYPE_Date  :
				if( !Record.is_NoData(Field) )
				{
					Stream.Printf("\"%s\"", Record.asString(Field));
				}
				else
				{
					Stream.Printf("\"\"");
				}
				break;

			default:
				if( !Record.is_NoData(Field) )
				{
					Stream.Printf("%s", Record.asString(Field));
				}
				break;
			}

			Stream.Printf("%c", Field < Get_Field_Count() - 1 ? Separator : '\n');
		}
	}

	//-----------------------------------------------------
	SG_UI_Process_Set_Ready();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                        DBase                          //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::_Load_DBase(const CSG_String &FileName)
{
	CSG_Table_DBase dbf(m_Encoding);

	return( dbf.Open_Read(FileName, this) );
}

//---------------------------------------------------------
bool CSG_Table::_Save_DBase(const CSG_String &FileName)
{
	CSG_Table_DBase dbf(m_Encoding);

	return( dbf.Open_Write(FileName, this) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                     From/To Text                      //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::from_Text(const CSG_String &Text)
{
	if( Text.is_Empty() )
	{
		return( false );
	}

	Destroy(); Set_Name(_TL("New Table"));

	//-----------------------------------------------------
	CSG_Strings Values, Records(SG_String_Tokenize(Text, "\r\n"));

	Values = (SG_String_Tokenize(Records[0], "\t"));

	TSG_Data_Type *Types = new TSG_Data_Type[Values.Get_Count()];

	for(int Field=0; Field<Values.Get_Count(); Field++)
	{
		Add_Field(Values[Field], SG_DATATYPE_String); Types[Field] = SG_DATATYPE_Int;
	}

	//-----------------------------------------------------
	for(int i=1; i<Records.Get_Count(); i++)
	{
		CSG_Table_Record &Record = *Add_Record();

		Values = (SG_String_Tokenize(Records[i], "\t"));

		for(int Field=0; Field<Values.Get_Count() && Field<Get_Field_Count(); Field++)
		{
			CSG_String Value(Values[Field]);

			if( Value.Length() >= 2 && Value[0] == '\"' && Value[Value.Length() - 1] == '\"' ) // is string, remove quota
			{
				Types[Field] = SG_DATATYPE_String; Value = Value.Mid(1, Value.Length() - 2);
			}

			Record.Set_Value(Field, Value);

			if( Types[Field] != SG_DATATYPE_String && !Value.is_Empty() )
			{
				if( Value[0] == '0' && Value[1] != '.' ) // keep leading zero(s) => don't interpret as number !
				{
					Types[Field] = SG_DATATYPE_String;
				}
				else try
				{
					size_t pos; double number = std::stod(Value.to_StdString(), &pos);

					if( pos < Value.Length() )
					{
						Types[Field] = SG_DATATYPE_String;
					}
					else if( Types[Field] != SG_DATATYPE_Double && (number - (int)number != 0. || Value.Find('.') >= 0) )
					{
						Types[Field] = SG_DATATYPE_Double;
					}
				}
				catch(...)
				{
					Types[Field] = SG_DATATYPE_String;
				}
			}
		}
	}

	//-----------------------------------------------------
	for(int Field=0; Field<Get_Field_Count(); Field++)
	{
		Set_Field_Type(Field, Types[Field]);
	}

	delete[](Types);

	return( Get_Field_Count() > 1 || Get_Count() > 0 );
}

//---------------------------------------------------------
CSG_String CSG_Table::to_Text(bool Selection) const
{
	CSG_String Text;

	for(int Field=0; Field<Get_Field_Count(); Field++)
	{
		Text += Get_Field_Name(Field); Text += 1 + Field < Get_Field_Count() ? '\t' : '\n';
	}

	sLong n = Selection ? Get_Selection_Count() : Get_Count();

	for(sLong i=0; i<n; i++)
	{
		CSG_Table_Record &Record = Selection ? *Get_Selection(i) : *Get_Record_byIndex(i);

		for(int Field=0; Field<Get_Field_Count(); Field++)
		{
			Text += Record.asString(Field); Text += 1 + Field < Get_Field_Count() ? '\t' : '\n';
		}
	}

	return( Text );
}


///////////////////////////////////////////////////////////
//                                                       //
//                      Serialize                        //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Serialize(CSG_File &Stream, bool bSave)
{
	const SG_Char Separator = SG_T('\t');

	//-----------------------------------------------------
	if( bSave )
	{
		Stream.Printf("%d %d\n", m_nFields, m_nRecords);

		for(int Field=0; Field<m_nFields; Field++)
		{
			Stream.Printf("%d \"%s\"\n", Get_Field_Type(Field), Get_Field_Name(Field));
		}

		for(sLong i=0; i<m_nRecords; i++)
		{
			for(int Field=0; Field<m_nFields; Field++)
			{
				Stream.Printf("%s%c", Get_Record(i)->asString(Field), Field < m_nFields - 1 ? Separator : '\n');
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	CSG_String sLine; int nFields, FieldType; sLong nRecords;

	if( Stream.Read_Line(sLine) && SG_SSCANF(sLine, SG_T("%d %lld"), &nFields, &nRecords) == 2 && nFields > 0 )
	{
		Destroy();

		for(int Field=0; Field<nFields; Field++)
		{
			if( Stream.Read_Line(sLine) && SG_SSCANF(sLine, SG_T("%d"), &FieldType) == 1 )
			{
				Add_Field(sLine.AfterFirst('\"').BeforeFirst('\"'), (TSG_Data_Type)FieldType);
			}
		}

		for(sLong i=0; i<nRecords; i++)
		{
			if( Stream.Read_Line(sLine) )
			{
				CSG_Table_Record &Record = *Add_Record();

				for(int Field=0; Field<m_nFields; Field++)
				{
					Record.Set_Value(Field, sLine.BeforeFirst(Separator));

					sLine = sLine.AfterFirst(Separator);
				}
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
