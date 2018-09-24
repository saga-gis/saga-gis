/**********************************************************
 * Version $Id: io_table_txt.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       table_io                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    io_table_txt.cpp                   //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "io_table_txt.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Export::CTable_Text_Export(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Export Text Table"));

	Set_Author		("O. Conrad (c) 2008");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Bool("",
		"HEADLINE"	, _TL("Headline"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"STRQUOTA"	, _TL("Strings in Quota"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format("%s|;|,|%s|%s|",
			_TL("tabulator"),
			_TL("space"),
			_TL("other")
		), 0
	);

	Parameters.Add_String("SEPARATOR",
		"SEP_OTHER"	, _TL("other"),
		_TL(""),
		"*"
	);

	Parameters.Add_FilePath("",
		"FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s|*.txt|%s|*.csv|%s|*.*",
			_TL("Text Files (*.txt)"),
			_TL("CSV Files (*.csv)"),
			_TL("All Files")
		), NULL, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Text_Export::On_Execute(void)
{
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_W, false) )
	{
		Message_Add(_TL("file could not be opened."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable		= Parameters("TABLE")->asTable();

	CSG_String	StrFormat	= Parameters("STRQUOTA")->asBool() ? SG_T("\"%s\"") : SG_T("%s");

	CSG_String	Separator;

	switch( Parameters("SEPARATOR")->asInt() )
	{
	default:	Separator	= Parameters("SEP_OTHER")->asString();	break;
	case  0:	Separator	= "\t";	break;
	case  1:	Separator	=  ";";	break;
	case  2:	Separator	=  ",";	break;
	case  3:	Separator	=  " ";	break;
	}

	//-----------------------------------------------------
	if( Parameters("HEADLINE")->asBool() )
	{
		for(int iField=0; iField<pTable->Get_Field_Count(); iField++)
		{
			Stream.Printf(StrFormat.c_str(), pTable->Get_Field_Name(iField));
			Stream.Printf(iField < pTable->Get_Field_Count() - 1 ? Separator.c_str() : SG_T("\n"));
		}
	}

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		for(int iField=0; iField<pTable->Get_Field_Count(); iField++)
		{
			switch( pTable->Get_Field_Type(iField) )
			{
			case SG_DATATYPE_Char  : default:
			case SG_DATATYPE_String:
			case SG_DATATYPE_Date  :
				Stream.Printf(StrFormat.c_str(), pRecord->asString(iField));
				break;

			case SG_DATATYPE_Short :
			case SG_DATATYPE_Int   :
			case SG_DATATYPE_Color :
				Stream.Printf("%d" , pRecord->asInt(iField));
				break;

			case SG_DATATYPE_Long  :
				Stream.Printf("%ld", (long)pRecord->asDouble(iField));
				break;

			case SG_DATATYPE_ULong :
				Stream.Printf("%lu", (unsigned long)pRecord->asDouble(iField));
				break;

			case SG_DATATYPE_Float :
			case SG_DATATYPE_Double:
				Stream.Printf("%f" , pRecord->asDouble(iField));
				break;
			}

			Stream.Printf(iField < pTable->Get_Field_Count() - 1 ? Separator.c_str() : SG_T("\n"));
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Import::CTable_Text_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Text Table"));

	Set_Author		("O. Conrad (c) 2008");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"HEADLINE"	, _TL("Headline"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format("%s|;|,|%s|%s|",
			_TL("tabulator"),
			_TL("space"),
			_TL("other")
		), 0
	);

	Parameters.Add_String("SEPARATOR",
		"SEP_OTHER"	, _TL("Separator (other)"),
		_TL(""),
		"*"
	);

	Parameters.Add_FilePath("",
		"FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s|*.txt;*.csv|%s|*.txt|%s|*.csv|%s|*.*",
			_TL("Recognized Files"),
			_TL("Text Files (*.txt)"),
			_TL("CSV Files (*.csv)"),
			_TL("All Files")
		), NULL, false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Text_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SEPARATOR") )
	{
		pParameters->Set_Enabled("SEP_OTHER", pParameter->asInt() >= 4);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Text_Import::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	SG_Char	Separator;

	switch( Parameters("SEPARATOR")->asInt() )
	{
	case  0:	Separator	= '\t';	break;
	case  1:	Separator	=  ';';	break;
	case  2:	Separator	=  ',';	break;
	case  3:	Separator	=  ' ';	break;
	default:	Separator	= *Parameters("SEP_OTHER")->asString();	break;
	}

	//-----------------------------------------------------
	if( !pTable->Create(Parameters("FILENAME")->asString(), Parameters("HEADLINE")->asBool() ? TABLE_FILETYPE_Text : TABLE_FILETYPE_Text_NoHeadLine, Separator) )
	{
		Error_Set(_TL("table could not be opened."));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Import_Numbers::CTable_Text_Import_Numbers(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Text Table with Numbers only"));

	Set_Author		("O. Conrad (c) 2008");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table_List("",
		"TABLES"	, _TL("Tables"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int("",
		"SKIP"		, _TL("Skip Leading Lines"),
		_TL(""),
		0, 0, true
	);

	Parameters.Add_Bool("",
		"HEADLINE"	, _TL("Headline"),
		_TL(""),
		false
	);

	Parameters.Add_Choice("",
		"SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format("%s|;|,|%s|%s|",
			_TL("tabulator"),
			_TL("space"),
			_TL("other")
		), 0
	);

	Parameters.Add_String("SEPARATOR",
		"SEP_OTHER"	, _TL("other"),
		_TL(""),
		"*"
	);

	Parameters.Add_FilePath("",
		"FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s|*.txt;*.csv|%s|*.txt|%s|*.csv|%s|*.*",
			_TL("Recognized Files"),
			_TL("Text Files (*.txt)"),
			_TL("CSV Files (*.csv)"),
			_TL("All Files")
		), NULL, false, false, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Text_Import_Numbers::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SEPARATOR") )
	{
		pParameters->Set_Enabled("SEP_OTHER", pParameter->asInt() >= 4);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Text_Import_Numbers::On_Execute(void)
{
	CSG_Strings	Files;

	Parameters("FILENAME")->asFilePath()->Get_FilePaths(Files);

	Parameters("TABLES")->asTableList()->Del_Items();

	for(int i=0; i<Files.Get_Count(); i++)
	{
		if( !Import(Files[i]) )
		{
			Message_Fmt("\n%s: %s", _TL("failed to import table"), Files[i].c_str());
		}
	}

	return( Parameters("TABLES")->asTableList()->Get_Item_Count() > 0 );
}

//---------------------------------------------------------
bool CTable_Text_Import_Numbers::Import(const CSG_String &File)
{
	CSG_String	sHead, sLine, Separator;
	CSG_File	Stream;

	//-----------------------------------------------------
	if( !Stream.Open(File, SG_FILE_R, false) )
	{
		return( false );
	}

	sLong	fLength	= Stream.Length();

	if( Parameters("SKIP")->asInt() > 0 )
	{
		int	i	= Parameters("SKIP")->asInt();

		while( i > 0 && Stream.Read_Line(sLine) )	{ i--; }
	}

	if( !Stream.Read_Line(sHead) || sHead.Length() == 0 )
	{
		return( false );
	}

	if( !Parameters("HEADLINE")->asBool() )
	{
		sLine	= sHead;
	}
	else if( !Stream.Read_Line(sLine) || sLine.Length() == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	switch( Parameters("SEPARATOR")->asInt() )
	{
	case  0:	Separator	= "\t";	break;
	case  1:	Separator	=  ";";	break;
	case  2:	Separator	=  ",";	break;
	case  3:	Separator	=  " ";	break;
	default:	Separator	= Parameters("SEP_OTHER")->asString();	break;
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= SG_Create_Table();

	pTable->Set_Name(SG_File_Get_Name(File, false));

	sHead.Trim(true);
	sHead.Replace(Separator, "\t");

	while( sHead.Length() > 0 )
	{
		sHead.Trim();

		if( Parameters("HEADLINE")->asBool() )
		{
			pTable->Add_Field(sHead.BeforeFirst('\t'), SG_DATATYPE_Double);
		}
		else
		{
			pTable->Add_Field(CSG_String::Format("FIELD%02d", 1 + pTable->Get_Field_Count()), SG_DATATYPE_Double);
		}

		sHead	= sHead.AfterFirst('\t');
	}

	if( pTable->Get_Field_Count() <= 0 )
	{
		delete(pTable);

		return( false );
	}

	//-----------------------------------------------------
	bool	bOkay	= true;

	do
	{
		sLine.Replace(Separator, "\t");

		CSG_Table_Record	*pRecord	= pTable->Add_Record();

		for(int i=0; bOkay && i<pTable->Get_Field_Count(); i++)
		{
			double	Value;

			sLine.Trim();

			if( (bOkay = sLine.asDouble(Value)) == true )
			{
				pRecord->Set_Value(i, Value);

				sLine	= sLine.AfterFirst('\t');
			}
			else
			{
				pTable->Del_Record(pTable->Get_Count() - 1);
			}
		}
	}
	while( bOkay && Stream.Read_Line(sLine) && Set_Progress((double)Stream.Tell(), (double)fLength) );

	//-----------------------------------------------------
	if( pTable->Get_Count() > 0 )
	{
		Parameters("TABLES")->asTableList()->Add_Item(pTable);

		return( true );
	}

	delete(pTable);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Import_Fixed_Cols::CTable_Text_Import_Fixed_Cols(void)
{
	Set_Name		(_TL("Import Text Table (Fixed Column Sizes)"));

	Set_Author		("O. Conrad (c) 2010");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int("",
		"SKIP"		, _TL("Skip Leading Lines"),
		_TL(""),
		0, 0, true
	);

	Parameters.Add_Bool("",
		"HEADLINE"	, _TL("Headline"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"FIELDDEF"	, _TL("Field Definition"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("mark breaks in first line"),
			_TL("specify fields with type"),
			_TL("from list")
		), 2
	);

	Parameters.Add_Int("",
		"NFIELDS"	, _TL("Number of Fields"),
		_TL(""),
		1, 1, true
	);

	CSG_Table	*pList	= Parameters.Add_FixedTable("",
		"LIST"		, _TL("List"),
		_TL("Specify name, size, and type of the attribute fields (type as follows: 0=text, 1=short, 2=long, 3=float, 4=double).")
	)->asTable();

	pList->Add_Field(_TL("Name"), SG_DATATYPE_String);
	pList->Add_Field(_TL("Size"), SG_DATATYPE_Int   );
	pList->Add_Field(_TL("Type"), SG_DATATYPE_Byte  );

	Parameters.Add_FilePath("",
		"FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s|*.txt|%s|*.*",
			_TL("Text Files (*.txt)"),
			_TL("All Files")
		), NULL, false
	);

	Add_Parameters("BREAKS", _TL("Breaks"), _TL(""));
	Add_Parameters("FIELDS", _TL("Fields"), _TL(""));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Text_Import_Fixed_Cols::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("FIELDDEF") )
	{
	//	pParameters->Set_Enabled("BREAKS" , pParameter->asInt() == 0);
	//	pParameters->Set_Enabled("FIELDS" , pParameter->asInt() == 1);
		pParameters->Set_Enabled("NFIELDS", pParameter->asInt() == 1);
		pParameters->Set_Enabled("LIST"   , pParameter->asInt() == 2);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Text_Import_Fixed_Cols::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_R, true) )
	{
		Error_Set(_TL("file could not be opened"));

		return( false );
	}

	sLong	fLength	= Stream.Length();

	//-----------------------------------------------------
	CSG_String	sLine;

	if( Parameters("SKIP")->asInt() > 0 )
	{
		int	i	= Parameters("SKIP")->asInt();

		while( i > 0 && Stream.Read_Line(sLine) )	{ i--; }
	}

	//-----------------------------------------------------
	int		nChars;

	if( !Stream.Read_Line(sLine) || (nChars = (int)sLine.Length()) <= 0 )
	{
		Error_Set(_TL("empty or corrupted file"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	pTable->Destroy();

	pTable->Set_Name(SG_File_Get_Name(Parameters("FILENAME")->asString(), false));

	//-----------------------------------------------------
	bool	bHeader	= Parameters("HEADLINE")->asBool();

	int		i, iField, nFields;

	CSG_Array_Int	First, Length;

	switch( Parameters("FIELDDEF")->asInt() )
	{
	//-----------------------------------------------------
	case 0:
		{
			CSG_Parameters	*pBreaks	= Get_Parameters("BREAKS");

			pBreaks->Del_Parameters();

			for(i=0; i<nChars; i++)
			{
				pBreaks->Add_Bool("",
					CSG_String::Format("%03d", i),
					CSG_String::Format("%03d %c", i + 1, sLine[i]),
					_TL(""), false
				);
			}

			if( !Dlg_Parameters("BREAKS") )
			{
				return( false );
			}

			//-------------------------------------------------
			for(i=0, nFields=1; i<pBreaks->Get_Count(); i++)
			{
				if( pBreaks->Get_Parameter(i)->asBool() )
				{
					nFields++;
				}
			}

			//-------------------------------------------------
			First .Set_Array(nFields);
			Length.Set_Array(nFields);

			First[0]	= 0;

			for(i=0, iField=1; i<pBreaks->Get_Count() && iField<nFields; i++)
			{
				if( pBreaks->Get_Parameter(i)->asBool() )
				{
					First[iField++]	= i + 1;
				}
			}

			//-------------------------------------------------
			for(iField=0; iField<nFields; iField++)
			{
				Length[iField]	= (iField < nFields - 1 ? First[iField + 1] : (int)sLine.Length()) - First[iField];

				pTable->Add_Field(bHeader ? sLine.Mid(First[iField], Length[iField]) : CSG_String::Format("FIELD%03d", iField + 1), SG_DATATYPE_String);
			}
		}
		break;

	//-----------------------------------------------------
	case 1:
		{
			CSG_Parameters	*pFields	= Get_Parameters("FIELDS");

			pFields->Del_Parameters();

			nFields	= Parameters("NFIELDS")->asInt();

			for(iField=0; iField<nFields; iField++)
			{
				CSG_String	s	= CSG_String::Format("%03d", iField);
				pFields->Add_Node("", "NODE" + s, _TL("Field") + s, _TL(""));
				pFields->Add_Int   ("NODE" + s, "LENGTH" + s, _TL("Length"), _TL(""), 1, 1, true);
			//	pFields->Add_Bool  ("NODE" + s, "IMPORT" + s, _TL("Import"), _TL(""), true);
				pFields->Add_Choice("NODE" + s, "TYPE"   + s, _TL("Type"  ), _TL(""), CSG_String::Format("%s|%s|%s|%s|%s|",
					_TL("text"),
					_TL("2 byte integer"),
					_TL("4 byte integer"),
					_TL("4 byte float"),
					_TL("8 byte float")
				));
			}

			if( !Dlg_Parameters("FIELDS") )
			{
				return( false );
			}

			//-------------------------------------------------
			First .Set_Array(nFields);
			Length.Set_Array(nFields);

			First[0]	= 0;

			for(iField=0, i=0; iField<nFields && i<nChars; iField++)
			{
				CSG_String	s	= CSG_String::Format("%03d", iField);

				First [iField]	= i;
				Length[iField]	= pFields->Get_Parameter("LENGTH" + s)->asInt();

				i	+= Length[iField];

				CSG_String	Name	= bHeader ? sLine.Mid(First[iField], Length[iField]) : CSG_String::Format("FIELD%03d", iField + 1);

				switch( pFields->Get_Parameter("TYPE" + s)->asInt() )
				{
				default:	pTable->Add_Field(Name, SG_DATATYPE_String);	break;
				case  1:	pTable->Add_Field(Name, SG_DATATYPE_Short );	break;
				case  2:	pTable->Add_Field(Name, SG_DATATYPE_Int   );	break;
				case  3:	pTable->Add_Field(Name, SG_DATATYPE_Float );	break;
				case  4:	pTable->Add_Field(Name, SG_DATATYPE_Double);	break;
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 2:
		{
			CSG_Table	*pList	= Parameters("LIST")->asTable();

			nFields	= pList->Get_Count();

			if( nFields < 1 )
			{
				Error_Set(_TL("no entries in list"));

				return( false );
			}

			//-------------------------------------------------
			First .Set_Array(nFields);
			Length.Set_Array(nFields);

			First[0]	= 0;

			for(iField=0, i=0; iField<nFields && i<nChars; iField++)
			{
				First [iField]	= i;
				Length[iField]	= pList->Get_Record(iField)->asInt(1);

				i	+= Length[iField];

				CSG_String	Name	= bHeader ? sLine.Mid(First[iField], Length[iField]) : CSG_String(pList->Get_Record(iField)->asString(0));

				switch( pList->Get_Record(iField)->asInt(2) )
				{
				default:	pTable->Add_Field(Name, SG_DATATYPE_String);	break;
				case  1:	pTable->Add_Field(Name, SG_DATATYPE_Short );	break;
				case  2:	pTable->Add_Field(Name, SG_DATATYPE_Int   );	break;
				case  3:	pTable->Add_Field(Name, SG_DATATYPE_Float );	break;
				case  4:	pTable->Add_Field(Name, SG_DATATYPE_Double);	break;
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	if( bHeader )
	{
		Stream.Read_Line(sLine);
	}

	//-----------------------------------------------------
	do
	{
		if( sLine.Length() == nChars )
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			for(iField=0; iField<nFields; iField++)
			{
				pRecord->Set_Value(iField, sLine.Mid(First[iField], Length[iField]));
			}
		}
	}
	while( Stream.Read_Line(sLine) && Set_Progress((double)Stream.Tell(), (double)fLength) );

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
