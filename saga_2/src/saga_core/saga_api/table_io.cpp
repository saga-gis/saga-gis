
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
bool CSG_Table::_Load(const char *File_Name, int Format, char Separator)
{
	bool		bResult;
	CSG_String	fName;

	_Destroy();

	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", LNG("[MSG] Load table"), File_Name), true);

	//-----------------------------------------------------
	if( Format <= TABLE_FILETYPE_Undefined || Format > TABLE_FILETYPE_DBase )
	{
		if( SG_File_Cmp_Extension(File_Name, "dbf") )
		{
			Format	= TABLE_FILETYPE_DBase;
		}
		else if( SG_File_Cmp_Extension(File_Name, "csv") )
		{
			Format	= TABLE_FILETYPE_Text;
			Separator	= ';';
		}
		else //if( SG_File_Cmp_Extension(File_Name, "txt") )
		{
			Format	= TABLE_FILETYPE_Text;
		}
	}

	//-----------------------------------------------------
	switch( Format )
	{
	case TABLE_FILETYPE_Text:
		bResult	= _Load_Text (File_Name, true , Separator);
		break;

	case TABLE_FILETYPE_Text_NoHeadLine:
		bResult	= _Load_Text (File_Name, false, Separator);
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

		Set_File_Name(File_Name);

		if( !Get_History().Load(File_Name, HISTORY_EXT_TABLE) )
		{
			Get_History().Add_Entry(LNG("[HST] Loaded from file"), File_Name);
		}

		SG_UI_Msg_Add(LNG("[MSG] okay"), false);

		return( true );
	}

	SG_UI_Msg_Add(LNG("[MSG] failed"), false);

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::Save(const char *File_Name, int Format)
{
	return( Save(File_Name, Format, '\t') );
}

//---------------------------------------------------------
bool CSG_Table::Save(const char *File_Name, int Format, char Separator)
{
	bool	bResult;

	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", LNG("[MSG] Save table"), File_Name), true);

	//-----------------------------------------------------
	if( Format <= TABLE_FILETYPE_Undefined || Format > TABLE_FILETYPE_DBase )
	{
		if( SG_File_Cmp_Extension(File_Name, "dbf") )
		{
			Format	= TABLE_FILETYPE_DBase;
		}
		else if( SG_File_Cmp_Extension(File_Name, "csv") )
		{
			Format	= TABLE_FILETYPE_Text;
			Separator	= ';';
		}
		else //if( SG_File_Cmp_Extension(File_Name, "txt") )
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

		Set_File_Type(Format);

		Set_File_Name(File_Name);

		Get_History().Save(File_Name, HISTORY_EXT_TABLE);

		SG_UI_Msg_Add(LNG("[MSG] okay"), false);

		return( true );
	}

	SG_UI_Msg_Add(LNG("[MSG] failed"), false);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Text							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table::_Load_Text(const char *File_Name, bool bHeadline, char Separator)
{
	bool			bContinue, bNumeric, bFloat;
	int				i, iField, iRecord, fLength;
	double			Value;
	FILE			*Stream;
	CSG_Table_Record	*pRecord;
	CSG_Table			newTable;
	CSG_String		sLine, sField;

	//-----------------------------------------------------
	if( (Stream = fopen(File_Name, "r")) != NULL )
	{
		fseek(Stream, 0, SEEK_END);
		fLength	= ftell(Stream);
		fseek(Stream, 0, SEEK_SET);

		if( SG_Read_Line(Stream, sLine) )
		{
			while( (i = sLine.Find(Separator)) >= 0 )
			{
				sField.Printf(bHeadline ? sLine.Left(i) : "FIELD_%02d", newTable.Get_Field_Count() + 1);

				newTable.Add_Field(sField, TABLE_FIELDTYPE_String);

				sLine.Remove(0, i + 1);
			}

			sField.Printf(bHeadline ? sLine : "FIELD_%02d", newTable.Get_Field_Count() + 1);

			newTable.Add_Field(sField, TABLE_FIELDTYPE_String);
		}

		//-------------------------------------------------
		if( newTable.Get_Field_Count() > 0 )
		{
			if( !bHeadline )
			{
				fseek(Stream, 0, SEEK_SET);
			}

			bContinue	= true;

			while( bContinue && SG_Read_Line(Stream, sLine) && SG_UI_Process_Set_Progress(ftell(Stream), fLength) )
			{
				sLine.Append(Separator);

				pRecord	= newTable._Add_Record();

				for(iField=0; iField<newTable.Get_Field_Count() && bContinue; iField++)
				{
					if( (i = sLine.Find(Separator)) >= 0 )
					{
						sField.Printf(sLine.Left(i));

						pRecord->Set_Value(iField, sField);

						sLine.Remove(0, i + 1);
					}
					else
					{
						bContinue	= false;
					}
				}

				if( !bContinue )
				{
					newTable._Del_Record(newTable.Get_Record_Count() - 1);
				}
			}

			//---------------------------------------------
			if( newTable.Get_Record_Count() > 0 )
			{
				for(iField=0; iField<newTable.Get_Field_Count(); iField++)
				{
					for(iRecord=0, bNumeric=true, bFloat=false; iRecord<newTable.Get_Record_Count() && bNumeric; iRecord++)
					{
						if( sscanf(newTable.Get_Record(iRecord)->asString(iField), "%lf", &Value) != 1 )
						{
							bNumeric	= false;
						}
						else if( !bFloat && Value - (int)Value != 0.0 )
						{
							bFloat		= true;
						}
					}

					if( !bNumeric )
					{
						Add_Field(newTable.Get_Field_Name(iField), TABLE_FIELDTYPE_String);
					}
					else if( !bFloat )
					{
						Add_Field(newTable.Get_Field_Name(iField), TABLE_FIELDTYPE_Int);
					}
					else
					{
						Add_Field(newTable.Get_Field_Name(iField), TABLE_FIELDTYPE_Double);
					}
				}

				for(iRecord=0; iRecord<newTable.Get_Record_Count() && SG_UI_Process_Set_Progress(iRecord, newTable.Get_Record_Count()); iRecord++)
				{
					pRecord	= _Add_Record();

					for(iField=0; iField<Get_Field_Count(); iField++)
					{
						switch( Get_Field_Type(iField) )
						{
						default:
							pRecord->Set_Value(iField,      newTable.Get_Record(iRecord)->asString(iField) );
							break;

						case TABLE_FIELDTYPE_Int:
							pRecord->Set_Value(iField, atoi(newTable.Get_Record(iRecord)->asString(iField)));
							break;

						case TABLE_FIELDTYPE_Double:
							pRecord->Set_Value(iField, atof(newTable.Get_Record(iRecord)->asString(iField)));
							break;
						}
					}
				}
			}
		}

		SG_UI_Process_Set_Ready();

		fclose(Stream);
	}

	return( Get_Field_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Table::_Save_Text(const char *File_Name, bool bHeadline, char Separator)
{
	int		iField, iRecord;
	FILE	*Stream;

	if( Get_Field_Count() > 0 )
	{
		if( (Stream = fopen(File_Name, "w")) != NULL )
		{
			for(iField=0; iField<Get_Field_Count(); iField++)
			{
				fprintf(Stream, "%s%c", Get_Field_Name(iField), iField < Get_Field_Count() - 1 ? Separator : '\n');
			}

			for(iRecord=0; iRecord<Get_Record_Count() && SG_UI_Process_Set_Progress(iRecord, Get_Record_Count()); iRecord++)
			{
				for(iField=0; iField<Get_Field_Count(); iField++)
				{
					fprintf(Stream, "%s", Get_Record(iRecord)->asString(iField));
					fprintf(Stream, "%c", iField < Get_Field_Count() - 1 ? Separator : '\n');
				}
			}

			SG_UI_Process_Set_Ready();

			fclose(Stream);

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
bool CSG_Table::_Load_DBase(const char *File_Name)
{
	int				iField;
	CSG_Table_DBase	dbf;
	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	if( dbf.Open(File_Name) )
	{
		Destroy();

		for(iField=0; iField<dbf.Get_FieldCount(); iField++)
		{
			switch( dbf.Get_FieldType(iField) )
			{
			case DBF_FT_LOGICAL:
				Add_Field(dbf.Get_FieldName(iField), TABLE_FIELDTYPE_Char);
				break;

			case DBF_FT_CHARACTER:
				Add_Field(dbf.Get_FieldName(iField), TABLE_FIELDTYPE_String);
				break;

			case DBF_FT_DATE:
				Add_Field(dbf.Get_FieldName(iField), TABLE_FIELDTYPE_Date);
				break;

			case DBF_FT_NUMERIC:
				if( dbf.Get_FieldDecimals(iField) > 0 )
					Add_Field(dbf.Get_FieldName(iField), TABLE_FIELDTYPE_Double);
				else
					Add_Field(dbf.Get_FieldName(iField), TABLE_FIELDTYPE_Long);
				break;

			default:
				Add_Field(dbf.Get_FieldName(iField), TABLE_FIELDTYPE_String);
				break;
			}
		}

		//-------------------------------------------------
		if( dbf.Move_First() )
		{
			do
			{
				pRecord	= _Add_Record();

				for(iField=0; iField<Get_Field_Count(); iField++)
				{
					switch( Get_Field_Type(iField) )
					{
					case TABLE_FIELDTYPE_Char:
						pRecord->Set_Value(iField, dbf.asString	(iField) );
						break;

					case TABLE_FIELDTYPE_String:	default:
						pRecord->Set_Value(iField, dbf.asString	(iField) );
						break;

					case TABLE_FIELDTYPE_Long:
						pRecord->Set_Value(iField, dbf.asInt	(iField) );
						break;

					case TABLE_FIELDTYPE_Double:
						pRecord->Set_Value(iField, dbf.asDouble	(iField) );
						break;
					}
				}
			}
			while( dbf.Move_Next() && SG_UI_Process_Set_Progress(dbf.Get_File_Position(), dbf.Get_File_Length()) );

			SG_UI_Process_Set_Ready();

			_Range_Invalidate();
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table::_Save_DBase(const char *File_Name)
{
	int							iField, iRecord;
	CSG_Table_Record				*pRecord;
	CSG_Table_DBase				dbf;
	CSG_Table_DBase::TFieldDesc	*dbfFieldDesc;

	//-----------------------------------------------------
	dbfFieldDesc	= (CSG_Table_DBase::TFieldDesc *)SG_Calloc(Get_Field_Count(), sizeof(CSG_Table_DBase::TFieldDesc));

	for(iField=0; iField<Get_Field_Count(); iField++)
	{
		strncpy(dbfFieldDesc[iField].Name, Get_Field_Name(iField), 11);

		switch( Get_Field_Type(iField) )
		{
		default:
		case TABLE_FIELDTYPE_String:
			dbfFieldDesc[iField].Type		= DBF_FT_CHARACTER;
			dbfFieldDesc[iField].Width		= (char)64;
			break;

		case TABLE_FIELDTYPE_Date:
			dbfFieldDesc[iField].Type		= DBF_FT_DATE;
			dbfFieldDesc[iField].Width		= (char)12;
			break;

		case TABLE_FIELDTYPE_Char:
			dbfFieldDesc[iField].Type		= DBF_FT_CHARACTER;
			dbfFieldDesc[iField].Width		= (char)1;
			break;

		case TABLE_FIELDTYPE_Short:
		case TABLE_FIELDTYPE_Int:
		case TABLE_FIELDTYPE_Long:
		case TABLE_FIELDTYPE_Color:
			dbfFieldDesc[iField].Type		= DBF_FT_NUMERIC;
			dbfFieldDesc[iField].Width		= (char)16;
			dbfFieldDesc[iField].Decimals	= (char)0;
			break;

		case TABLE_FIELDTYPE_Float:
		case TABLE_FIELDTYPE_Double:
			dbfFieldDesc[iField].Type		= DBF_FT_NUMERIC;
			dbfFieldDesc[iField].Width		= (char)16;
			dbfFieldDesc[iField].Decimals	= (char)8;
			break;
		}
	}

	dbf.Open(File_Name, Get_Field_Count(), dbfFieldDesc);

	SG_Free(dbfFieldDesc);

	//-----------------------------------------------------
	for(iRecord=0; iRecord<Get_Record_Count() && SG_UI_Process_Set_Progress(iRecord, Get_Record_Count()); iRecord++)
	{
		pRecord	= Get_Record(iRecord);

		dbf.Add_Record();

		for(iField=0; iField<Get_Field_Count(); iField++)
		{
			switch( dbf.Get_FieldType(iField) )
			{
			case DBF_FT_CHARACTER:
				dbf.Set_Value(iField, pRecord->asString(iField));
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
bool CSG_Table::Serialize(FILE *Stream, bool bSave)
{
	char			Separator	= '\t';
	int				iField, nFields, iRecord, nRecords, FieldType;
	CSG_Table_Record	*pRecord;
	CSG_String		s, sLine;

	//-----------------------------------------------------
	if( bSave )
	{
		fprintf(Stream, "%d %d\n", m_nFields, m_nRecords);

		for(iField=0; iField<m_nFields; iField++)
		{
			fprintf(Stream, "%d \"%s\"\n", Get_Field_Type(iField), Get_Field_Name(iField));
		}

		for(iRecord=0; iRecord<m_nRecords; iRecord++)
		{
			for(iField=0; iField<m_nFields; iField++)
			{
				fprintf(Stream, "%s%c", Get_Record(iRecord)->asString(iField), iField < m_nFields - 1 ? Separator : '\n');
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	else if( SG_Read_Line(Stream, sLine) && sscanf(sLine, "%d %d", &nFields, &nRecords) == 2 && nFields > 0 )
	{
		_Destroy();

		for(iField=0; iField<nFields; iField++)
		{
			if( SG_Read_Line(Stream, sLine) && sscanf(sLine, "%d", &FieldType) == 1 )
			{
				Add_Field(sLine.AfterFirst('\"').BeforeFirst('\"'), (TSG_Table_Field_Type)FieldType);
			}
		}

		for(iRecord=0; iRecord<nRecords; iRecord++)
		{
			if( SG_Read_Line(Stream, sLine) )
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
