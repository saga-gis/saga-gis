
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
//                   tool_counter.cpp                    //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#include "tool_counter.h"

//---------------------------------------------------------
#include <wx/dir.h>
#include <wx/filename.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTool_Counter::CTool_Counter(void)
{
	//-----------------------------------------------------
	Set_Name		("Count Tools");

	Set_Author		("O. Conrad (c) 2015");

	Set_Description	(
		"Extracts SAGA tool instantiations directly from source by analyzing TLB_Interface.cpp files. "
		"Helps to correctly find the number of tools in a certain SAGA distribution, which became "
		"necessary due to a tool counting bug in the tool library interface function after introducing "
		"the 'skip tool' option for maintenance of tool identifiers. "
	);

	//-----------------------------------------------------
	Parameters.Add_Table(
		"", "TARGET"	, "Tools",
		"",
		PARAMETER_OUTPUT
	);

	CSG_String	Directory;

	#ifdef _SAGA_MSW
	if( SG_Get_Environment("SAGA", &Directory) )
	{
		Directory	+= "\\src\\tools";
	}
	#endif

	Parameters.Add_FilePath(
		"", "DIRECTORY"	, "Sources Directory",
		"",
		NULL, Directory, false, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTool_Counter::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	&Elements	= *Parameters("TARGET")->asTable();

	Elements.Destroy();
	Elements.Set_Name("Tools Instantiations");

	Elements.Add_Field("LIB" , SG_DATATYPE_String);
	Elements.Add_Field("ID"  , SG_DATATYPE_String);
	Elements.Add_Field("TOOL", SG_DATATYPE_String);

	int	nFiles	= Read_Directory(Parameters("DIRECTORY")->asString(), Elements);

	if( nFiles <= 0 )
	{
		Error_Set("no source code files found");

		return( false );
	}

	Message_Fmt("\nnumber of scanned files: %d", nFiles);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTool_Counter::Read_Directory(const SG_Char *Directory, CSG_Table &Elements)
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

	if(	Dir.GetFirst(&Name, "TLB_Interface.cpp", wxDIR_FILES|wxDIR_HIDDEN) )
	{
		do
		{
			File.SetFullName(Name);

			nFiles	+= Read_File(s = File.GetFullPath(), Elements);
		}
		while( Dir.GetNext(&Name) );
	}

	if(	Dir.GetFirst(&Name, "*"    , wxDIR_DIRS |wxDIR_HIDDEN) )
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
int CTool_Counter::Read_File(const SG_Char *File, CSG_Table &Elements)
{
	Process_Set_Text("file: " + SG_File_Get_Name(File, true));

	//-----------------------------------------------------
	CSG_String	Lib	= SG_File_Get_Path(File).BeforeLast('\\').AfterLast('\\');

	if( !Lib.CmpNoCase("dev_tools") || !Lib.CmpNoCase("template") )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(File, SG_FILE_R, false) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	CSG_String	String;

	if( !Stream.Read(String, Stream.Length()) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	int	i;

	i	= String.Find("Create_Tool");	if( i < 0 )	return( false );

	String	= String.Right(String.Length() - i);

	i	= String.Find("switch");	if( i < 0 )	return( false );

	String	= String.Right(String.Length() - i);

	String	= String.AfterFirst('{').BeforeFirst('}');

	while( !String.is_Empty() )
	{
		CSG_String	Text	= String.BeforeFirst(';');

		if( Text.Find("new") >= 0 )
		{
			Text.Replace("\t", " "); Text.Replace("\n", " "); Text.Trim();

			if( Text.Find("case") == 0 )
			{
				Text	= Text.Right(Text.Length() - 4);
				
				CSG_String	ID	= Text.BeforeFirst(':'); ID.Trim();

				Text	= Text.AfterFirst(':');	Text.Trim();

				CSG_Table_Record	*pRecord	= Elements.Add_Record();

				pRecord->Set_Value(0, Lib);
				pRecord->Set_Value(1, ID);
				pRecord->Set_Value(2, Text);
			}
		}

		String	= String.AfterFirst(';');
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTool_Counter::Read_Text(const SG_Char *String, CSG_String &Text)
{
	int	n, Level;

	Text.Clear();

	for(n=0, Level=-1; String[n]!='\0' && Level<2; n++)
	{
		if( Level < 0 )
		{
			if( String[n] == '(' )
			{
				Level	= 0;
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTool_Menus::CTool_Menus(void)
{
	//-----------------------------------------------------
	Set_Name		("Extract Tool and Menu Information");

	Set_Author		("O. Conrad (c) 2015");

	Set_Description	(
		""
	);

	//-----------------------------------------------------
	Parameters.Add_Table(
		"", "TOOLS"		, "Tools",
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(
		"", "MENUS"		, "Menus",
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int(
		"", "LEVEL"		, "Level",
		"",
		0, 0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTool_Menus::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	&Tools	= *Parameters("TOOLS")->asTable();

	Tools.Destroy();
	Tools.Set_Name("Tools");

	Tools.Add_Field("LIB" , SG_DATATYPE_String);
	Tools.Add_Field("TOOL", SG_DATATYPE_String);
	Tools.Add_Field("ID"  , SG_DATATYPE_String);
	Tools.Add_Field("MENU", SG_DATATYPE_String);

	int	Level	= Parameters("LEVEL")->asInt();

	for(int iLibrary=0; iLibrary<SG_Get_Tool_Library_Manager().Get_Count(); iLibrary++)
	{
		CSG_Tool_Library	*pLibrary	= SG_Get_Tool_Library_Manager().Get_Library(iLibrary);

		for(int iTool=0; iTool<pLibrary->Get_Count(); iTool++)
		{
			CSG_Tool	*pTool	= pLibrary->Get_Tool(iTool);

			if( pTool != NULL && pTool != TLB_INTERFACE_SKIP_TOOL )
			{
				CSG_Table_Record	*pRecord	= Tools.Add_Record();

				pRecord->Set_Value(0, pLibrary->Get_Name());
				pRecord->Set_Value(1, pTool   ->Get_Name());
				pRecord->Set_Value(2, pTool   ->Get_ID  ());

				CSG_String	s	= pLibrary->Get_Menu(iTool);

				for(size_t i=0, n=0; i<s.Length() && n<(size_t)Level; i++)
				{
					if( s[i] == '|' )
					{
						if( ++n == Level )
						{
							s	= s.Left(i);
						}
					}
				}

				pRecord->Set_Value(3, s);
			}
		}
	}

	if( !Tools.Get_Count() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	&Menus	= *Parameters("MENUS")->asTable();

	Menus.Destroy();
	Menus.Set_Name("Menus");

	Menus.Add_Field("ID"   , SG_DATATYPE_Int);
	Menus.Add_Field("Menu" , SG_DATATYPE_String);
	Menus.Add_Field("Count", SG_DATATYPE_Int);

	Tools.Set_Index(3, TABLE_INDEX_Ascending);

	CSG_Table_Record	*pMenu	= NULL;

	CSG_String	s;

	for(int i=0; i<Tools.Get_Count(); i++)
	{
		if( !pMenu || s.Cmp(Tools[i].asString(3)) )
		{
			pMenu	= Menus.Add_Record();

			pMenu->Set_Value(0, Menus.Get_Count());
			pMenu->Set_Value(1, s = Tools[i].asString(3));
			pMenu->Set_Value(2, 1);
		}
		else
		{
			pMenu->Add_Value(2, 1);
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
