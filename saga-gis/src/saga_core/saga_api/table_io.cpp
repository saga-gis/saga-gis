
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
//														 //
//														 //
//														 //
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
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Set_File_Encoding(int Encoding)
{
	if( Encoding >= 0 && Encoding < SG_FILE_ENCODING_UNDEFINED )
	{
		m_Encoding	= Encoding;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
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
		Format	= SG_File_Cmp_Extension(FileName, "dbf") ? TABLE_FILETYPE_DBase : TABLE_FILETYPE_Text;
	}

	if( Separator == '\0' )
	{
		Separator	= SG_File_Cmp_Extension(FileName, "csv") ? ',' : '\t';	// comma separated values or tab spaced text
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

	CSG_MetaData	*pFields	= Get_MetaData_DB().Get_Child("FIELDS");

	if( pFields && pFields->Get_Children_Count() == Get_Field_Count() )
	{
		for(int iField=0; iField<Get_Field_Count(); iField++)
		{
			Set_Field_Name(iField, pFields->Get_Content(iField));
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
	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Save table"), FileName.c_str()), true);

	Set_File_Encoding(Encoding);

	//-----------------------------------------------------
	if( Format <= TABLE_FILETYPE_Undefined || Format > TABLE_FILETYPE_DBase )
	{
		if( SG_File_Cmp_Extension(FileName, "dbf") )
		{
			Format	= TABLE_FILETYPE_DBase;
		}
		else
		{
			Format	= TABLE_FILETYPE_Text;

			if( Separator == '\0' )
			{
				Separator	= SG_File_Cmp_Extension(FileName, "csv") ? ',' : '\t';	// comma separated values or tab spaced text
			}
		}
	}

	//-----------------------------------------------------
	bool	bResult	= false;

	switch( Format )
	{
	case TABLE_FILETYPE_Text:   default:	bResult	= _Save_Text (FileName, true , Separator);	break;
	case TABLE_FILETYPE_Text_NoHeadLine:	bResult	= _Save_Text (FileName, false, Separator);	break;
	case TABLE_FILETYPE_DBase          :	bResult	= _Save_DBase(FileName                  );	break;
	}

	//-----------------------------------------------------
	CSG_MetaData	*pFields	= Get_MetaData_DB().Get_Child("FIELDS");

	if( !pFields )
	{
		pFields	= Get_MetaData_DB().Add_Child("FIELDS");
	}

	pFields->Del_Children();

	for(int iField=0; iField<Get_Field_Count(); iField++)
	{
		pFields->Add_Child("FIELD", Get_Field_Name(iField))->Add_Property("TYPE", gSG_Data_Type_Identifier[Get_Field_Type(iField)]);
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
//														 //
//						Text							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_Table::_Load_Text_Trim(CSG_String &s, const SG_Char Separator)
{
	for(size_t i=0; i<s.Length(); i++)
	{
		SG_Char	c	= s[i];

		if( c == Separator || (c != ' ' && c != '\t' && c != '\n' && c != '\v' && c != '\f' && c != '\r') )
		{
			if( i > 0 )
			{
				s	= s.Right(s.Length() - i);
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
		bool	bInQuotes	= true;

		for(size_t i=1; i<s.Length(); i++)
		{
			if( bInQuotes )
			{
				if( s[i] == '\"' )
				{
					bInQuotes	= false;
				}
			}
			else if( s[i] == '\"' )
			{
				bInQuotes	= true;
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
bool CSG_Table::_Load_Text(const CSG_String &FileName, bool bHeadline, const SG_Char Separator)
{
	int			iField;
	sLong		fLength;
	CSG_String	sLine, sField;
	CSG_File	Stream;
	CSG_Table	Table;

	//-----------------------------------------------------
	if( Stream.Open(FileName, SG_FILE_R, false, m_Encoding) == false )
	{
		return( false );
	}

	if( (fLength = Stream.Length()) <= 0 )
	{
		return( false );
	}

	if( !Stream.Read_Line(sLine) )
	{
		return( false );
	}

	//-----------------------------------------------------
	_Load_Text_Trim(sLine, Separator);

	while( !sLine.is_Empty() )
	{
		if( sLine[0] == '\"' )	// value in quotas
		{
			sField	= sLine.AfterFirst('\"').BeforeFirst('\"');
			sLine	= sLine.AfterFirst('\"').AfterFirst ('\"');
		}
		else
		{
			sField	= sLine.BeforeFirst(Separator);
		}

		sLine	= sLine.AfterFirst(Separator);	_Load_Text_Trim(sLine, Separator);

		if( !bHeadline || sField.Length() == 0 )
		{
			sField.Printf("F%02d", Table.Get_Field_Count() + 1);
		}

		Table.Add_Field(sField, SG_DATATYPE_String);
	}

	//-----------------------------------------------------
	TSG_Data_Type	*Type	= new TSG_Data_Type[Table.Get_Field_Count()];

	for(iField=0; iField<Table.Get_Field_Count(); iField++)
	{
		Type[iField]	= SG_DATATYPE_Int;
	}

	if( !bHeadline )
	{
		Stream.Seek_Start();
	}

	while( Stream.Read_Line(sLine) && SG_UI_Process_Set_Progress((double)Stream.Tell(), (double)fLength) )
	{
		if( sLine.Length() < 1 )
		{
			continue;
		}

		CSG_Table_Record	*pRecord	= Table.Add_Record();

		_Load_Text_Trim(sLine, Separator);

		for(iField=0; iField<Table.Get_Field_Count() && !sLine.is_Empty(); iField++)
		{
			size_t	Position	= _Load_Text_EndQuote(sLine, Separator);

			if( Position > 0 )	// value in quotas !!!
			{
				if( Position - 2 > 0 )
				{
					sField	= sLine.Mid(1, Position - 2);
				}
				else
				{
					sField.Clear();
				}

				sLine	= sLine.Right(sLine.Length() - Position);

				Type[iField]	= SG_DATATYPE_String;
			}
			else
			{
				sField	= sLine.BeforeFirst(Separator);
			}

			sLine	= sLine.AfterFirst(Separator);	_Load_Text_Trim(sLine, Separator);

			//---------------------------------------------
			if( Type[iField] != SG_DATATYPE_String && !sField.is_Empty() )
			{
				double	Value;

				if( sField.asDouble(Value) == false || sField.Find('.', false) < sField.Find('.', true) )
				{
					Type[iField]	= SG_DATATYPE_String;
				}
				else if( Type[iField] != SG_DATATYPE_Double && Value - (int)Value != 0.0 )
				{
					Type[iField]	= SG_DATATYPE_Double;
				}
			}

			pRecord->Set_Value(iField, sField);
		}
	}

	//-----------------------------------------------------
	if( Table.Get_Field_Count() > 0 )
	{
		for(iField=0; iField<Table.Get_Field_Count(); iField++)
		{
			Add_Field(Table.Get_Field_Name(iField), Type[iField]);
		}

		for(int iRecord=0; iRecord<Table.Get_Count() && SG_UI_Process_Set_Progress(iRecord, Table.Get_Count()); iRecord++)
		{
			CSG_Table_Record	*pRecord	= Add_Record();

			for(iField=0; iField<Get_Field_Count(); iField++)
			{
				if( *Table[iRecord].asString(iField) )
				{
					switch( Get_Field_Type(iField) )
					{
					default                :	pRecord->Set_Value(iField, Table[iRecord].asString(iField));	break;
					case SG_DATATYPE_Int   :	pRecord->Set_Value(iField, Table[iRecord].asInt   (iField));	break;
					case SG_DATATYPE_Double:	pRecord->Set_Value(iField, Table[iRecord].asDouble(iField));	break;
					}
				}
				else
				{
					pRecord->Set_NoData(iField);
				}
			}
		}
	}

	delete[](Type);

	SG_UI_Process_Set_Ready();

	return( Get_Field_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Table::_Save_Text(const CSG_String &FileName, bool bHeadline, const SG_Char Separator)
{
	CSG_File	Stream;

	if( Get_Field_Count() <= 0 || Stream.Open(FileName, SG_FILE_W, false, m_Encoding) == false )
	{
		return( false );
	}

	//-----------------------------------------------------
	{
		for(int iField=0; iField<Get_Field_Count(); iField++)
		{
			Stream.Printf("%s%c", Get_Field_Name(iField), iField < Get_Field_Count() - 1 ? Separator : '\n');
		}
	}

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<Get_Record_Count() && SG_UI_Process_Set_Progress(iRecord, Get_Record_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= Get_Record_byIndex(iRecord);

		for(int iField=0; iField<Get_Field_Count(); iField++)
		{
			switch( Get_Field_Type(iField) )
			{
			case SG_DATATYPE_String:
			case SG_DATATYPE_Date:
				if( !pRecord->is_NoData(iField) )
				{
					Stream.Printf("\"%s\"", pRecord->asString(iField));
				}
				else
				{
					Stream.Printf("\"\"");
				}
				break;

			default:
				if( !pRecord->is_NoData(iField) )
				{
					Stream.Printf("%s", pRecord->asString(iField));
				}
				break;
			}

			Stream.Printf("%c", iField < Get_Field_Count() - 1 ? Separator : '\n');
		}
	}

	//-----------------------------------------------------
	SG_UI_Process_Set_Ready();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						DBase							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::_Load_DBase(const CSG_String &FileName)
{
	CSG_Table_DBase		dbf(m_Encoding);

	return( dbf.Open_Read(FileName, this) );
}

//---------------------------------------------------------
bool CSG_Table::_Save_DBase(const CSG_String &FileName)
{
	CSG_Table_DBase		dbf(m_Encoding);

	return( dbf.Open_Write(FileName, this) );
}


///////////////////////////////////////////////////////////
//														 //
//						Serialize						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::Serialize(CSG_File &Stream, bool bSave)
{
	SG_Char				Separator	= SG_T('\t');
	int					iField, nFields, iRecord, nRecords, FieldType;
	CSG_Table_Record	*pRecord;
	CSG_String			s, sLine;

	//-----------------------------------------------------
	if( bSave )
	{
		Stream.Printf("%d %d\n", m_nFields, m_nRecords);

		for(iField=0; iField<m_nFields; iField++)
		{
			Stream.Printf("%d \"%s\"\n", Get_Field_Type(iField), Get_Field_Name(iField));
		}

		for(iRecord=0; iRecord<m_nRecords; iRecord++)
		{
			for(iField=0; iField<m_nFields; iField++)
			{
				Stream.Printf("%s%c", Get_Record(iRecord)->asString(iField), iField < m_nFields - 1 ? Separator : '\n');
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	else if( Stream.Read_Line(sLine) && SG_SSCANF(sLine, SG_T("%d %d"), &nFields, &nRecords) == 2 && nFields > 0 )
	{
		Destroy();

		for(iField=0; iField<nFields; iField++)
		{
			if( Stream.Read_Line(sLine) && SG_SSCANF(sLine, SG_T("%d"), &FieldType) == 1 )
			{
				Add_Field(sLine.AfterFirst('\"').BeforeFirst('\"'), (TSG_Data_Type)FieldType);
			}
		}

		for(iRecord=0; iRecord<nRecords; iRecord++)
		{
			if( Stream.Read_Line(sLine) )
			{
				pRecord	= Add_Record();

				for(iField=0; iField<m_nFields; iField++)
				{
					pRecord->Set_Value(iField, sLine.BeforeFirst(Separator));

					sLine	= sLine.AfterFirst(Separator);
				}
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
