
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
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <string.h>

#include "table.h"
#include "table_dbase.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::_Load(const CSG_String &File_Name, TSG_Table_File_Type Format, const SG_Char *Separator)
{
	if( !::SG_File_Exists(File_Name) )
	{
		return( false );
	}

	bool		bResult;
	CSG_String	fName, sSeparator(Separator);

	_Destroy();

	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), LNG("[MSG] Load table"), File_Name.c_str()), true);

	//-----------------------------------------------------
	if( Format == TABLE_FILETYPE_Undefined )
	{
		if( SG_File_Cmp_Extension(File_Name, SG_T("dbf")) )
		{
			Format	= TABLE_FILETYPE_DBase;
		}
		else if( SG_File_Cmp_Extension(File_Name, SG_T("csv")) )
		{
			Format	= TABLE_FILETYPE_Text;
			sSeparator	= ";";
		}
		else //if( SG_File_Cmp_Extension(File_Name, SG_T("txt")) )
		{
			Format	= TABLE_FILETYPE_Text;
		}
	}

	//-----------------------------------------------------
	switch( Format )
	{
	case TABLE_FILETYPE_Text:
		bResult	= _Load_Text (File_Name, true , sSeparator);
		break;

	case TABLE_FILETYPE_Text_NoHeadLine:
		bResult	= _Load_Text (File_Name, false, sSeparator);
		break;

	case TABLE_FILETYPE_DBase:
		bResult	= _Load_DBase(File_Name);
		break;

	default:
		bResult	= false;
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Set_Modified(false);

		Set_Update_Flag();

		Set_File_Name(File_Name);

		Load_MetaData(File_Name);

		SG_UI_Msg_Add(LNG("[MSG] okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Save(const CSG_String &File_Name, int Format)
{
	return( Save(File_Name, Format, SG_T("\t")) );
}

//---------------------------------------------------------
bool CSG_Table::Save(const CSG_String &File_Name, int Format, const SG_Char *Separator)
{
	bool		bResult;
	CSG_String	sSeparator(Separator);

	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), LNG("[MSG] Save table"), File_Name.c_str()), true);

	//-----------------------------------------------------
	if( Format <= TABLE_FILETYPE_Undefined || Format > TABLE_FILETYPE_DBase )
	{
		if( SG_File_Cmp_Extension(File_Name, SG_T("dbf")) )
		{
			Format	= TABLE_FILETYPE_DBase;
		}
		else if( SG_File_Cmp_Extension(File_Name, SG_T("csv")) )
		{
			Format	= TABLE_FILETYPE_Text;
			sSeparator	= ';';
		}
		else //if( SG_File_Cmp_Extension(File_Name, SG_T("txt")) )
		{
			Format	= TABLE_FILETYPE_Text;
		}
	}

	switch( Format )
	{
	case TABLE_FILETYPE_Text:
		bResult	= _Save_Text (File_Name, true , Separator);
		break;

	case TABLE_FILETYPE_Text_NoHeadLine:
		bResult	= _Save_Text (File_Name, false, Separator);
		break;

	case TABLE_FILETYPE_DBase:
		bResult	= _Save_DBase(File_Name);
		break;
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Set_Modified(false);

		Set_Update_Flag();

		Set_File_Type(Format);

		Set_File_Name(File_Name);

		Save_MetaData(File_Name);

		SG_UI_Msg_Add(LNG("[MSG] okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Text							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::_Load_Text(const CSG_String &File_Name, bool bHeadline, const SG_Char *Separator)
{
	int			i, iField, fLength;
	CSG_String	sLine, sField;
	CSG_File	Stream;
	CSG_Table	Table;

	//-----------------------------------------------------
	if( Stream.Open(File_Name, SG_FILE_R, false) == false )
	{
		return( false );
	}

	if( !Stream.Read_Line(sLine) )
	{
		return( false );
	}

	//-----------------------------------------------------
	sLine	+= Separator;

	while( (i = sLine.Find(Separator)) >= 0 )
	{
		sField.Printf(bHeadline ? sLine.Left(i) : SG_T("FIELD_%02d"), Table.Get_Field_Count() + 1);

		if( sField[0] == SG_T('\"') && sField[(int)(sField.Length() - 1)] == SG_T('\"') )	// remove quota
		{
			sField	= sField.AfterFirst('\"').BeforeLast('\"');
		}

		Table.Add_Field(sField, SG_DATATYPE_String);

		sLine.Remove(0, i + 1);
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

	fLength	= Stream.Length();

	while( Stream.Read_Line(sLine) && sLine.Length() > 0 && SG_UI_Process_Set_Progress(Stream.Tell(), fLength) )
	{
		CSG_Table_Record	*pRecord	= Table._Add_Record();

		sLine	+= Separator;

		for(iField=0; iField<Table.Get_Field_Count(); iField++)
		{
			if( (i = sLine.Find(Separator)) >= 0 )
			{
				sField.Printf(sLine.Left(i));

				if( sField[0] == SG_T('\"') && sField[(int)(sField.Length() - 1)] == SG_T('\"') )	// remove quota
				{
					sField	= sField.AfterFirst('\"').BeforeLast('\"');
				}

				if( Type[iField] != SG_DATATYPE_String )
				{
					double	Value;

					if( SG_SSCANF(sField, SG_T("%lf"), &Value) != 1 )
					{
						Type[iField]	= SG_DATATYPE_String;
					}
					else if( Type[iField] != SG_DATATYPE_Double && Value - (int)Value != 0.0 )
					{
						Type[iField]	= SG_DATATYPE_Double;
					}
				}

				pRecord->Set_Value(iField, sField);

				sLine.Remove(0, i + 1);
			}
			else
			{
				break;
			}
		}
	}

	//-----------------------------------------------------
	if( Table.Get_Count() > 0 )
	{
		for(iField=0; iField<Table.Get_Field_Count(); iField++)
		{
			Add_Field(Table.Get_Field_Name(iField), Type[iField]);
		}

		for(int iRecord=0; iRecord<Table.Get_Count() && SG_UI_Process_Set_Progress(iRecord, Table.Get_Count()); iRecord++)
		{
			CSG_Table_Record	*pRecord	= _Add_Record();

			for(iField=0; iField<Get_Field_Count(); iField++)
			{
				switch( Get_Field_Type(iField) )
				{
				default:						pRecord->Set_Value(iField, Table[iRecord].asString(iField));	break;
				case SG_DATATYPE_Int:		pRecord->Set_Value(iField, Table[iRecord].asInt   (iField));	break;
				case SG_DATATYPE_Double:	pRecord->Set_Value(iField, Table[iRecord].asDouble(iField));	break;
				}
			}
		}
	}

	delete[](Type);

	SG_UI_Process_Set_Ready();

	return( Get_Field_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Table::_Save_Text(const CSG_String &File_Name, bool bHeadline, const SG_Char *Separator)
{
	int			iField, iRecord;
	CSG_File	Stream;

	if( Get_Field_Count() > 0 )
	{
		if( Stream.Open(File_Name, SG_FILE_W, false) )
		{
			for(iField=0; iField<Get_Field_Count(); iField++)
			{
				Stream.Printf(SG_T("%s%s"), Get_Field_Name(iField), iField < Get_Field_Count() - 1 ? Separator : SG_T("\n"));
			}

			for(iRecord=0; iRecord<Get_Record_Count() && SG_UI_Process_Set_Progress(iRecord, Get_Record_Count()); iRecord++)
			{
				for(iField=0; iField<Get_Field_Count(); iField++)
				{
					Stream.Printf(SG_T("%s"), Get_Record(iRecord)->asString(iField));
					Stream.Printf(SG_T("%s"), iField < Get_Field_Count() - 1 ? Separator : SG_T("\n"));
				}
			}

			SG_UI_Process_Set_Ready();

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						DBase							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::_Load_DBase(const CSG_String &File_Name)
{
	int					iField;
	CSG_Table_DBase		dbf;
	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	if( dbf.Open(File_Name) )
	{
		Destroy();

		for(iField=0; iField<dbf.Get_FieldCount(); iField++)
		{
			TSG_Data_Type	Type;

			switch( dbf.Get_FieldType(iField) )
			{
			case DBF_FT_LOGICAL:
				Type	= SG_DATATYPE_Char;
				break;

			case DBF_FT_CHARACTER:	default:
				Type	= SG_DATATYPE_String;
				break;

			case DBF_FT_DATE:
				Type	= SG_DATATYPE_Date;
				break;

			case DBF_FT_NUMERIC:
				Type	= dbf.Get_FieldDecimals(iField) > 0
						? SG_DATATYPE_Double
						: SG_DATATYPE_Long;
				break;
			}

			Add_Field(SG_STR_MBTOSG(dbf.Get_FieldName(iField)), Type);
		}

		//-------------------------------------------------
		if( dbf.Move_First() && dbf.Get_Record_Count() > 0 )
		{
			m_nRecords		= m_nBuffer	= dbf.Get_Record_Count();
			m_Records		= (CSG_Table_Record **)SG_Malloc(m_nRecords * sizeof(CSG_Table_Record *));

			for(int iRecord=0; iRecord<m_nRecords && SG_UI_Process_Set_Progress(iRecord, m_nRecords); iRecord++)
			{
				m_Records[iRecord]	= pRecord	= _Get_New_Record(iRecord);

				for(iField=0; iField<Get_Field_Count(); iField++)
				{
					switch( Get_Field_Type(iField) )
					{
					case SG_DATATYPE_Char:
						pRecord->Set_Value(iField, SG_STR_MBTOSG(dbf.asString(iField)) );
						break;

					case SG_DATATYPE_String:	default:
						pRecord->Set_Value(iField, SG_STR_MBTOSG(dbf.asString(iField)) );
						break;

					case SG_DATATYPE_Date:
						pRecord->Set_Value(iField, dbf.asDouble(iField) );
						break;

					case SG_DATATYPE_Long:
						pRecord->Set_Value(iField, dbf.asInt(iField) );
						break;

					case SG_DATATYPE_Double:
						pRecord->Set_Value(iField, dbf.asDouble(iField) );
						break;
					}
				}

				dbf.Move_Next();
			}

			SG_UI_Process_Set_Ready();

			Set_Modified(false);

			Set_Update_Flag();

			_Stats_Invalidate();
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::_Save_DBase(const CSG_String &File_Name)
{
	int				iField, iRecord, nBytes;
	CSG_Table_DBase	dbf;

	//-----------------------------------------------------
	CSG_Table_DBase::TFieldDesc	*dbfFields	= new CSG_Table_DBase::TFieldDesc[Get_Field_Count()];

	for(iField=0; iField<Get_Field_Count(); iField++)
	{
		strncpy(dbfFields[iField].Name, SG_STR_SGTOMB(Get_Field_Name(iField)), 11);

		switch( Get_Field_Type(iField) )
		{
		case SG_DATATYPE_String: default:
			dbfFields[iField].Type		= DBF_FT_CHARACTER;
			dbfFields[iField].Width		= (BYTE)((nBytes = Get_Field_Length(iField)) > 255 ? 255 : nBytes);
			break;

		case SG_DATATYPE_Date:
			dbfFields[iField].Type		= DBF_FT_DATE;
			dbfFields[iField].Width		= (BYTE)8;
			break;

		case SG_DATATYPE_Char:
			dbfFields[iField].Type		= DBF_FT_CHARACTER;
			dbfFields[iField].Width		= (BYTE)1;
			break;

		case SG_DATATYPE_Short:
		case SG_DATATYPE_Int:
		case SG_DATATYPE_Long:
		case SG_DATATYPE_Color:
			dbfFields[iField].Type		= DBF_FT_NUMERIC;
			dbfFields[iField].Width		= (BYTE)16;
			dbfFields[iField].Decimals	= (BYTE)0;
			break;

		case SG_DATATYPE_Float:
		case SG_DATATYPE_Double:
			dbfFields[iField].Type		= DBF_FT_NUMERIC;
			dbfFields[iField].Width		= (BYTE)16;
			dbfFields[iField].Decimals	= (BYTE)8;
			break;
		}
	}

	if( !dbf.Open(File_Name, Get_Field_Count(), dbfFields) )
	{
		delete[](dbfFields);

		SG_UI_Msg_Add_Error(LNG("[ERR] dbase file could not be opened"));

		return( false );
	}

	delete[](dbfFields);

	//-----------------------------------------------------
	for(iRecord=0; iRecord<Get_Record_Count() && SG_UI_Process_Set_Progress(iRecord, Get_Record_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= Get_Record(iRecord);

		dbf.Add_Record();

		for(iField=0; iField<Get_Field_Count(); iField++)
		{
			switch( dbf.Get_FieldType(iField) )
			{
			case DBF_FT_DATE:
			case DBF_FT_CHARACTER:
				dbf.Set_Value(iField, SG_STR_SGTOMB(pRecord->asString(iField)));
				break;

			case DBF_FT_NUMERIC:
				dbf.Set_Value(iField, pRecord->asDouble(iField));
				break;
			}
		}

		dbf.Flush_Record();
	}

	SG_UI_Process_Set_Ready();

	return( true );
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
		Stream.Printf(SG_T("%d %d\n"), m_nFields, m_nRecords);

		for(iField=0; iField<m_nFields; iField++)
		{
			Stream.Printf(SG_T("%d \"%s\"\n"), Get_Field_Type(iField), Get_Field_Name(iField));
		}

		for(iRecord=0; iRecord<m_nRecords; iRecord++)
		{
			for(iField=0; iField<m_nFields; iField++)
			{
				Stream.Printf(SG_T("%s%c"), Get_Record(iRecord)->asString(iField), iField < m_nFields - 1 ? Separator : '\n');
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	else if( Stream.Read_Line(sLine) && SG_SSCANF(sLine, SG_T("%d %d"), &nFields, &nRecords) == 2 && nFields > 0 )
	{
		_Destroy();

		for(iField=0; iField<nFields; iField++)
		{
			if( Stream.Read_Line(sLine) && SG_SSCANF(sLine, SG_T("%d"), &FieldType) == 1 )
			{
				Add_Field(sLine.AfterFirst(SG_T('\"')).BeforeFirst(SG_T('\"')), (TSG_Data_Type)FieldType);
			}
		}

		for(iRecord=0; iRecord<nRecords; iRecord++)
		{
			if( Stream.Read_Line(sLine) )
			{
				pRecord	= _Add_Record();

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
