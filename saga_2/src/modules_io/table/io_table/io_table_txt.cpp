
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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
//						Export							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Export::CTable_Text_Export(void)
{
	Set_Name		(_TL("Export Text Table"));

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL	, "HEADLINE"	, _TL("Save Headline"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Value(
		NULL	, "STRQUOTA"	, _TL("Strings in Quota"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Choice(
		NULL	, "SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("tabulator"),
			_TL(";"),
			_TL(","),
			_TL("other")
		), 0
	);

	Parameters.Add_String(
		NULL	, "SEP_OTHER"	, _TL("Separator (other)"),
		_TL(""),
		SG_T("*")
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s"),
			_TL("Text Files (*.txt)")	, SG_T("*.txt"),
			_TL("CSV Files (*.csv)")	, SG_T("*.csv"),
			_TL("All Files")			, SG_T("*.*")
		), NULL, true
	);
}

//---------------------------------------------------------
bool CTable_Text_Export::On_Execute(void)
{
	CSG_String	StrFormat, Separator, sLine;
	CSG_File	Stream;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable		= Parameters("TABLE")		->asTable();
	StrFormat	= Parameters("STRQUOTA")	->asBool() ? SG_T("\"%s\"") : SG_T("%s");

	switch( Parameters("SEPARATOR")->asInt() )
	{
	case 0:		Separator	= "\t";	break;
	case 1:		Separator	=  ";";	break;
	case 2:		Separator	=  ",";	break;
	default:	Separator	= Parameters("SEP_OTHER")->asString();	break;
	}

	//-----------------------------------------------------
	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_W, false) )
	{
		Message_Add(_TL("file could not be opened."));
	}

	//-----------------------------------------------------
	else
	{
		if( Parameters("HEADLINE")->asBool() )
		{
			sLine.Clear();

			for(int iField=0; iField<pTable->Get_Field_Count(); iField++)
			{
				sLine	+= pTable->Get_Field_Name(iField);
				sLine	+= (iField < pTable->Get_Field_Count() - 1 ? Separator : SG_T("\n"));
			}
		}

		//-------------------------------------------------
		for(int iRecord=0; iRecord<pTable->Get_Record_Count() && Set_Progress(iRecord, pTable->Get_Record_Count()); iRecord++)
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

			sLine.Clear();

			for(int iField=0; iField<pTable->Get_Field_Count(); iField++)
			{
				switch( pTable->Get_Field_Type(iField) )
				{
				default:
				case TABLE_FIELDTYPE_Char:
				case TABLE_FIELDTYPE_String:
				case TABLE_FIELDTYPE_Date:
					Stream.Printf(StrFormat		, pRecord->asString(iField));
					break;

				case TABLE_FIELDTYPE_Short:
				case TABLE_FIELDTYPE_Int:
				case TABLE_FIELDTYPE_Long:
				case TABLE_FIELDTYPE_Color:
					Stream.Printf(SG_T("%d")	, pRecord->asDouble(iField));

				case TABLE_FIELDTYPE_Float:
				case TABLE_FIELDTYPE_Double:
					Stream.Printf(SG_T("%f")	, pRecord->asDouble(iField));
					break;
				}

				sLine	+= (iField < pTable->Get_Field_Count() - 1 ? Separator : SG_T('\n'));
			}
		}

		//-------------------------------------------------
		Stream.Close();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Import::CTable_Text_Import(void)
{
	Set_Name		(_TL("Import Text Table"));

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "HEADLINE"	, _TL("File contains headline"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Choice(
		NULL	, "SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("tabulator"),
			_TL(";"),
			_TL(","),
			_TL("space"),
			_TL("other")
		), 0
	);

	Parameters.Add_String(
		NULL	, "SEP_OTHER"	, _TL("Separator (other)"),
		_TL(""),
		SG_T("*")
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s"),
			_TL("Text Files (*.txt)")	, SG_T("*.txt"),
			_TL("CSV Files (*.csv)")	, SG_T("*.csv"),
			_TL("All Files")			, SG_T("*.*")
		), NULL, false
	);
}

//---------------------------------------------------------
bool CTable_Text_Import::On_Execute(void)
{
	bool		bHeader;
	CSG_String	Separator;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable	= Parameters("TABLE")		->asTable();
	bHeader	= Parameters("HEADLINE")	->asBool();

	switch( Parameters("SEPARATOR")->asInt() )
	{
	case 0:		Separator	= "\t";	break;
	case 1:		Separator	=  ";";	break;
	case 2:		Separator	=  ",";	break;
	case 3:		Separator	=  " ";	break;
	default:	Separator	= Parameters("SEP_OTHER")->asString();	break;
	}

	//-----------------------------------------------------
	if( !pTable->Create(Parameters("FILENAME")->asString(), bHeader ? TABLE_FILETYPE_Text : TABLE_FILETYPE_Text_NoHeadLine, Separator) )
	{
		Message_Add(_TL("table could not be opened."));
	}

	//-----------------------------------------------------
	else
	{
		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Import_Numbers::CTable_Text_Import_Numbers(void)
{
	Set_Name		(_TL("Import Text Table with Numbers only"));

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "HEADLINE"	, _TL("File contains headline"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);

	Parameters.Add_Choice(
		NULL	, "SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("tabulator"),
			_TL(";"),
			_TL(","),
			_TL("other")
		), 0
	);

	Parameters.Add_String(
		NULL	, "SEP_OTHER"	, _TL("Separator (other)"),
		_TL(""),
		SG_T("*")
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s"),
			_TL("Text Files (*.txt)")	, SG_T("*.txt"),
			_TL("CSV Files (*.csv)")	, SG_T("*.csv"),
			_TL("All Files")			, SG_T("*.*")
		), NULL, false
	);
}

//---------------------------------------------------------
bool CTable_Text_Import_Numbers::On_Execute(void)
{
	bool		bHeader;
	double		Value;
	CSG_String	sLine, sHead, Separator;
	CSG_File	Stream;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable	= Parameters("TABLE")		->asTable();
	bHeader	= Parameters("HEADLINE")	->asBool();

	switch( Parameters("SEPARATOR")->asInt() )
	{
	case 0:		Separator	= "\t";	break;
	case 1:		Separator	=  ";";	break;
	case 2:		Separator	=  ",";	break;
	default:	Separator	= Parameters("SEP_OTHER")->asString();	break;
	}

	//-----------------------------------------------------
	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_R, false) )
	{
		Message_Add(_TL("file could not be opened"));
	}

	else if( !Stream.Read_Line(sLine) || sLine.Length() == 0 )
	{
		Message_Add(_TL("empty or corrupted file"));
	}

	else if( bHeader && (!Stream.Read_Line(sLine) || sLine.Length() == 0) )
	{
		Message_Add(_TL("empty or corrupted file"));
	}

	//-----------------------------------------------------
	else
	{
		pTable->Destroy();
		pTable->Set_Name(SG_File_Get_Name(Parameters("FILENAME")->asString(), false));

		sLine.Replace(Separator, SG_T(" "));
		sHead	= sLine;

		do
		{
			sHead.Trim();

			if( sHead.asDouble(Value) )
			{
				pTable->Add_Field(CSG_String::Format(SG_T("%d"), 1 + pTable->Get_Field_Count()), TABLE_FIELDTYPE_Double);
			}

			sHead	= sHead.AfterFirst(' ');
		}
		while( sHead.Length() > 0 );

		//-------------------------------------------------
		if( pTable->Get_Field_Count() > 0 )
		{
			int		fLength	= Stream.Length();

			do
			{
				sLine.Replace(Separator, SG_T(" "));

				CSG_Table_Record	*pRecord	= pTable->Add_Record();

				for(int i=0; i<pTable->Get_Field_Count(); i++)
				{
					sLine.Trim();

					pRecord->Set_Value(i, sLine.asDouble());

					sLine	= sLine.AfterFirst(' ');
				}
			}
			while( Stream.Read_Line(sLine) && sLine.Length() > 0 && Set_Progress(Stream.Tell(), fLength) );
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
