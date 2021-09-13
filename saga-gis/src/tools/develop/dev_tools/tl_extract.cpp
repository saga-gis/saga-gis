
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      dev_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    tl_extract.cpp                     //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                     Olaf Conrad                       //
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
//                Institute for Geography                //
//                University of Hamburg                  //
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
#include "tl_extract.h"

//---------------------------------------------------------
#include <wx/dir.h>
#include <wx/filename.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTL_Extract::CTL_Extract(void)
{
	//-----------------------------------------------------
	Set_Name		("Extract Translatable Text Elements from Sources");

	Set_Author		("O. Conrad (c) 2010");

	Set_Description	("");

	//-----------------------------------------------------
	Parameters.Add_Table(
		"", "TARGET"	, "Translatable Elements",
		"",
		PARAMETER_OUTPUT
	);

	CSG_String	Directory;

#ifdef _SAGA_MSW
	if( SG_Get_Environment("SAGA", &Directory) )
	{
		Directory	+= "\\src";
	}
#endif

	Parameters.Add_FilePath(
		"", "DIRECTORY"	, "Sources Directory",
		"",
		NULL, Directory, false, true
	);

	Parameters.Add_Bool(
		"", "LOCATION"	, "Location",
		"",
		false
	);

	Parameters.Add_Bool(
		"", "LONG"		, "Long Texts",
		"",
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTL_Extract::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	Elements;

	Elements.Add_Field("TEXT", SG_DATATYPE_String);
	Elements.Add_Field("FILE", SG_DATATYPE_String);

	int	nFiles	= Read_Directory(Parameters("DIRECTORY")->asString(), Elements);

	if( nFiles <= 0 )
	{
		Error_Set("no source code files found");

		return( false );
	}

	Message_Add(CSG_String::Format("\nnumber of scanned files: %d", nFiles), false);
		
	if( Elements.Get_Count() <= 0 )
	{
		Error_Set("no translatable text elements found");

		return( false );
	}

	Message_Add(CSG_String::Format("\nnumber of translatable elements: %d", Elements.Get_Count()), false);

	//-----------------------------------------------------
	Process_Set_Text("collecting elements");

	CSG_String	Text;

	bool		bLocation	= Parameters("LOCATION")->asBool();

	CSG_Table	*pTarget	= Parameters("TARGET")->asTable();

	pTarget->Destroy();
	pTarget->Set_Name("Translatable Elements");

	pTarget->Add_Field("TEXT"       , SG_DATATYPE_String);
	pTarget->Add_Field("TRANSLATION", SG_DATATYPE_String);

	if( bLocation )
	{
		pTarget->Add_Field("FILE"   , SG_DATATYPE_String);
	}

	Elements.Set_Index(0, TABLE_INDEX_Ascending);

	for(int i=0; i<Elements.Get_Count() && Set_Progress(i, Elements.Get_Count()); i++)
	{
		if( i == 0 || Text.Cmp(Elements.Get_Record_byIndex(i)->asString(0)) )
		{
			Text	= Elements.Get_Record_byIndex(i)->asString(0);

			CSG_Table_Record	*pRecord	= pTarget->Add_Record();

			pRecord->Set_Value(0, Text);

			if( bLocation )
			{
				pRecord->Set_Value(2, Elements.Get_Record_byIndex(i)->asString(1));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTL_Extract::Read_Directory(const SG_Char *Directory, CSG_Table &Elements)
{
	int		nFiles	= 0;

	wxString	Name, s;
	wxFileName	File;
	wxDir		Dir;

	if( !Dir.Open(Directory) )
	{
		return( nFiles );
	}

	File.AssignDir(Directory);

	if(	Dir.GetFirst(&Name, "*.cpp", wxDIR_FILES|wxDIR_HIDDEN) )
	{
		do
		{
			File.SetFullName(Name);

			nFiles	+= Read_File(s = File.GetFullPath(), Elements);
		}
		while( Dir.GetNext(&Name) );
	}

	if(	Dir.GetFirst(&Name, "*.h"  , wxDIR_FILES|wxDIR_HIDDEN) )
	{
		do
		{
			File.SetFullName(Name);

			nFiles	+= Read_File(s = File.GetFullPath(), Elements);
		}
		while( Dir.GetNext(&Name) );
	}

	if(	Dir.GetFirst(&Name, "*.xml", wxDIR_FILES|wxDIR_HIDDEN) )
	{
		do
		{
			File.SetFullName(Name);

			nFiles	+= Read_ToolChain(s = File.GetFullPath(), Elements);
		}
		while( Dir.GetNext(&Name) );
	}

	if(	Dir.GetFirst(&Name, "*"    , wxDIR_DIRS|wxDIR_HIDDEN) )
	{
		do
		{
			File.AssignDir(Directory);
			File.AppendDir(Name);

			nFiles	+= Read_Directory(s = File.GetFullPath(), Elements);
		}
		while( Dir.GetNext(&Name) );
	}

	return( nFiles );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTL_Extract::Read_File(const SG_Char *File, CSG_Table &Elements)
{
	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(File, SG_FILE_R, false) )
	{
		return( 0 );
	}

	Process_Set_Text("file: " + SG_File_Get_Name(File, true));

	//-----------------------------------------------------
	CSG_String	String, Text;

	if( !Stream.Read(String, Stream.Length()) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	const SG_Char	Function[2][4]	= {	SG_T("_TL"), SG_T("_TW") };

	const SG_Char	*p		= String;

	bool			bLong	= Parameters("LONG")->asBool();

	while( *p != '\0' )
	{
		if(	!(p[0] == Function[0][0] && p[1] == Function[0][1] && p[2] == Function[0][2])
		&&	!(p[0] == Function[1][0] && p[1] == Function[1][1] && p[2] == Function[1][2] && bLong) )
		{
			p	++;
		}
		else
		{
			p	+= Read_Text(p, Text);

			if( Text.Length() > 0 )
			{
				CSG_Table_Record	*pRecord	= Elements.Add_Record();

				pRecord->Set_Value(0, Text);
				pRecord->Set_Value(1, File);
			}
		}
	}

	return( 1 );
}

//---------------------------------------------------------
int CTL_Extract::Read_Text(const SG_Char *String, CSG_String &Text)
{
	int			n, Level;

	Text.Clear();

	for(n=0, Level=-1; String[n]!='\0' && Level<2; n++)
	{
		if( Level < 0 )
		{
			if( String[n] == '(' )
			{
				Level	= 0;
			}
			else if( isspace(String[n]) )
			{
				Level	= 2;	// exit !
			}
		}
		else if( Level == 0 )
		{
			switch( String[n] )
			{
			case '\"':
				Level	= 1;
				break;

			case ')':
				Level	= 2;
				break;
			}
		}
		else switch( String[n] )
		{
		case '\"':
			Level	= 0;
			break;

		case '\\':
			Text.Append(String[n++]);
			Text.Append(String[n]);
			break;

		default:
			Text.Append(String[n]);
			break;
		}
	}

	return( n );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_XML_CONTENT(XML, ID)	if( XML(ID) && !XML[ID].Get_Content().is_Empty() ) {\
	CSG_Table_Record	*pRecord	= Elements.Add_Record();\
	pRecord->Set_Value(0, XML[ID].Get_Content());\
	pRecord->Set_Value(1, File);\
}

//---------------------------------------------------------
int CTL_Extract::Read_ToolChain(const SG_Char *File, CSG_Table &Elements)
{
	//-----------------------------------------------------
	CSG_MetaData	Chain;

	if( Chain.Load(File) )
	{
		Process_Set_Text("file: " + SG_File_Get_Name(File, true));

		//-------------------------------------------------
		if( Chain.Cmp_Name("toolchains") )
		{
			GET_XML_CONTENT(Chain, "name"       );
			GET_XML_CONTENT(Chain, "menu"       );
		//	GET_XML_CONTENT(Chain, "description");

			return( 1 );
		}

		//-------------------------------------------------
		if( Chain.Cmp_Name("toolchain") )
		{
			GET_XML_CONTENT(Chain, "name"       );
			GET_XML_CONTENT(Chain, "menu"       );
		//	GET_XML_CONTENT(Chain, "description");

			if( Chain("parameters") )
			{
				for(int i=0; i<Chain["parameters"].Get_Children_Count(); i++)
				{
					const CSG_MetaData	&Parameter	= Chain["parameters"][i];

					GET_XML_CONTENT(Parameter, "name"       );
					GET_XML_CONTENT(Parameter, "description");
				}
			}

			return( 1 );
		}
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
