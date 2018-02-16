/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                Command Line Interface                 //
//                                                       //
//                   Program: SAGA_CMD                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     SAGA_CMD.cpp                      //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <locale.h>

#include <wx/app.h>
#include <wx/utils.h>

#include "config.h"
#include "callback.h"
#include "tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Run				(int argc, char *argv[]);

bool		Execute			(int argc, char *argv[]);
bool		Execute_Script	(const CSG_String &Script);

bool		Load_Libraries	(void);

bool		Check_First		(const CSG_String &Argument, int argc, char *argv[]);
bool		Check_Flags		(const CSG_String &Argument);

void		Print_Libraries	(void);
void		Print_Tools		(const CSG_String &Library);
void		Print_Execution	(CSG_Tool *pTool);

void		Print_Logo		(void);
void		Print_Get_Help	(void);
void		Print_Help		(void);
void		Print_Help		(const CSG_String &Library);
void		Print_Help		(const CSG_String &Library, const CSG_String &Tool);
void		Print_Version	(void);

void		Create_Batch	(const CSG_String &File);
void		Create_Docs		(const CSG_String &Directory);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		main	(int argc, char *argv[])
{
	if( !wxInitialize() )
	{
		CMD_Print_Error("initialisation failed");

		return( 1 );
	}

	wxTheApp->SetVendorName("www.saga-gis.org");
	wxTheApp->SetAppName   ("saga_cmd");

#if !defined(_DEBUG)
	wxSetAssertHandler(NULL);		// disable all wx asserts in SAGA release builds
#endif

//---------------------------------------------------------
#if !defined(_DEBUG) && defined(_SAGA_VC)
#define _TOOL_EXCEPTION
_try
{
#endif
//---------------------------------------------------------

	bool bResult	= Run(argc, argv);

//---------------------------------------------------------
#ifdef _DEBUG
	CMD_Set_Interactive(true);
	CMD_Get_Pause();
#endif

#ifdef _TOOL_EXCEPTION
}
_except(1)
{
	Print_Error(_TL("access violation"));
}
#endif
//---------------------------------------------------------

	fflush(stdout);
	fflush(stderr);

	wxUninitialize();

	return( bResult ? 0 : 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Run(int argc, char *argv[])
{
	setlocale(LC_NUMERIC, "C");

	SG_Set_UI_Callback(CMD_Get_Callback());

	Config_Load();	// first load default configuration (if available). can be modified subsequently by flags

	//-----------------------------------------------------
	if( Check_First(argv[1], argc - 2, argv + 2) )
	{
		return( true );
	}

	//-----------------------------------------------------
	while( argc > 1 && Check_Flags(argv[1]) )
	{
		argc--;	argv++;
	}

	Print_Logo();

	//-----------------------------------------------------
	if( !Load_Libraries() )
	{
		Print_Get_Help();

		return( false );
	}

	//-----------------------------------------------------
	if( argc == 2 && SG_File_Exists(CSG_String(argv[1])) )
	{
		return( Execute_Script(argv[1]) );
	}

	return( Execute(argc, argv) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Execute(int argc, char *argv[])
{
	//-----------------------------------------------------
	CSG_String	Library	= argc <= 1 ? "" : argv[1];

	if( SG_Get_Tool_Library_Manager().Get_Library(Library, true) == NULL )
	{
		Print_Libraries();

		return( false );
	}

	//-----------------------------------------------------
	CSG_Tool	*pTool	= argc <= 2 ? NULL : SG_Get_Tool_Library_Manager().Get_Tool(Library, argv[2]);

	if( pTool == NULL )
	{
		Print_Tools(Library);

		return( false );
	}

	//-----------------------------------------------------
	if( argc == 3 && CMD_Get_XML() )
	{	// Just output tool synopsis as XML-tagged text, then return.
		SG_PRINTF(pTool->Get_Summary(true, "", "", SG_SUMMARY_FMT_XML).c_str());

		return( true );
	}

	//-----------------------------------------------------
	if( pTool->needs_GUI() )
	{
		CMD_Print_Error(_TL("tool needs graphical user interface"), pTool->Get_Name());

		return( false );
	}

	//-----------------------------------------------------
	Print_Execution(pTool);

	CCMD_Tool	CMD_Tool(pTool);

	return( CMD_Tool.Execute(argc - 2, argv + 2) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Set_Environment(CSG_String &Command)
{
	wxString	Value, Key	= Command.AfterFirst('%').BeforeFirst('%').c_str();

	if( Key.IsEmpty() )
	{
		return( true );
	}

	if( !wxGetEnv(Key, &Value) || Command.Replace(&Key.Prepend("%").Append("%"), &Value) < 1 )
	{
		return( false );
	}

	return( Set_Environment(Command) );
}

//---------------------------------------------------------
bool		Execute(CSG_String Command)
{
	Command.Trim();

	if( Command.is_Empty() )
	{
		return( true );
	}

	if( !Command.Left(3).CmpNoCase("REM") || Command[0] == '#' )
	{
		return( true );
	}

	if( !Command.Left(4).CmpNoCase("ECHO") )
	{
		CMD_Print(Command.AfterFirst(' '));

		return( true );
	}

	//-----------------------------------------------------
	int		argc	= 1;
	char	**argv	= NULL;

	while( Command.Length() > 0 )
	{
		CSG_String	s	= Command[0] == '\"' ? Command.AfterFirst('\"').BeforeFirst('\"') : Command.BeforeFirst(' ');

		argv		= (char **)SG_Realloc(argv, (argc + 1) * sizeof(char *));
		argv[argc]	= (char  *)SG_Calloc(1 + s.Length(), sizeof(char));

		memcpy(argv[argc++], s.b_str(), s.Length() * sizeof(char));

		Command	= Command.AfterFirst(' ');	Command.Trim();
	}

	//-----------------------------------------------------
	bool	bResult	= Execute(argc, argv);

	for(int i=1; i<argc; i++)
	{
		SG_FREE_SAFE(argv[i]);
	}

	SG_FREE_SAFE(argv);

	return( bResult );
}

//---------------------------------------------------------
bool		Execute_Script(const CSG_String &Script)
{
	if( CMD_Get_Show_Messages() )
	{
		CMD_Print(CSG_String::Format("%s: %s", _TL("Running Script"), Script.c_str()));
	}

	CSG_File	Stream;

	if( !Stream.Open(Script, SG_FILE_R, false) )
	{
		CMD_Print_Error(_TL("could not open file"), Script);

		return( false );
	}

	CSG_String	Command;

	while( Stream.Read_Line(Command) )
	{
		Set_Environment(Command);

		if( !Execute(Command) )
		{
			CMD_Print_Error(_TL("invalid command"), Command);

			return( false );
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
bool		Load_Libraries(const CSG_String &Directory)
{
	bool	bShow	= CMD_Get_Show_Messages();

	CMD_Set_Show_Messages(false);
	int	n	= SG_Get_Tool_Library_Manager().Add_Directory(Directory, false);
	CMD_Set_Show_Messages(bShow);

	return( n > 0 );
}

//---------------------------------------------------------
bool		Load_Libraries(void)
{
	wxString	Path, CMD_Path	= SG_File_Get_Path(SG_UI_Get_Application_Path()).c_str();

    #if defined(_SAGA_LINUX)
		Load_Libraries(MODULE_LIBRARY_PATH);
		Load_Libraries(SG_File_Make_Path(CSG_String(SHARE_PATH), SG_T("toolchains")));	// look for tool chains
	#else
		wxString	DLL_Path	= SG_File_Make_Path(&CMD_Path, "dll").c_str();

		if( wxGetEnv("PATH", &Path) && Path.Length() > 0 )
		{
			wxSetEnv("PATH", DLL_Path + wxT(";") + Path);
		}
		else
		{
			wxSetEnv("PATH", DLL_Path);
		}

		wxSetEnv("GDAL_DRIVER_PATH", DLL_Path);
		wxSetEnv("GDAL_DATA"       , DLL_Path + "\\gdal-data");

		Load_Libraries(SG_File_Make_Path(&CMD_Path, "tools"));
    #endif

	if( wxGetEnv("SAGA_MLB", &Path) )
	{
		CSG_String_Tokenizer	Paths(&Path, ";");

		while( Paths.Has_More_Tokens() )
		{
			Load_Libraries(Paths.Get_Next_Token());
		}
	}

	if( SG_Get_Tool_Library_Manager().Get_Count() <= 0 )
	{
		CMD_Print_Error("could not load any tool library");

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
bool		Check_First		(const CSG_String &Argument, int argc, char *argv[])
{
	if( !Argument.Cmp("-h") || !Argument.Cmp("--help") )
	{
		switch( argc )
		{
		default:	Print_Help(                );	break;
		case  1:	Print_Help(argv[0]         );	break;
		case  2:	Print_Help(argv[0], argv[1]);	break;
		}

		return( true );
	}

	if( !Argument.Cmp("-v") || !Argument.Cmp("--version") )
	{
		Print_Version();

		return( true );
	}

	if( !Argument.Find("--create-config") )
	{
		Config_Create(Argument.AfterFirst('='));

		return( true );
	}

	if( !Argument.Find("--create-batch") )
	{
		Create_Batch(Argument.AfterFirst('='));

		return( true );
	}

	if( !Argument.Find("--create-docs") )
	{
		Create_Docs(Argument.AfterFirst('='));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		Check_Flags		(const CSG_String &Argument)
{
#if   defined(_SAGA_LINUX)
	CSG_String	Path_Shared	= SHARE_PATH;
#elif defined(_SAGA_MSW)
	CSG_String	Path_Shared	= SG_File_Get_Path(SG_UI_Get_Application_Path());
#endif

	//-----------------------------------------------------
	CSG_String	s(Argument.BeforeFirst('='));

	if( !s.Cmp("-f") || !s.Cmp("--flags") )
	{
		s	= CSG_String(Argument).AfterFirst('=');

		CMD_Set_Show_Progress(s.Find('q') < 0 && s.Find('s') < 0);	// q, s: no progress report
		CMD_Set_Show_Messages(s.Find('r') < 0 && s.Find('s') < 0);	// r, s: no messages report
		CMD_Set_Interactive  (s.Find('i') >= 0                  );	// i: allow user interaction
		CMD_Set_XML          (s.Find('x') >= 0                  );	// x: message output as xml

		if( s.Find('l') >= 0 )	// l: load translation dictionary
		{
			SG_Printf(CSG_String::Format("\n%s:", _TL("loading translation dictionary")));
			SG_Printf(CSG_String::Format("\n%s.\n",
				SG_Get_Translator().Create(SG_File_Make_Path(Path_Shared, SG_T("saga"), SG_T("lng")), false)
				? _TL("success") : _TL("failed")
			));
		}

		if( s.Find('p') >= 0 )	// p: load projections dictionary
		{
			SG_Printf(CSG_String::Format("\n%s:", _TL("loading spatial reference system database")));
			SG_Printf(CSG_String::Format("\n%s.\n",
				SG_Get_Projections().Create(SG_File_Make_Path(Path_Shared, SG_T("saga_prj"), SG_T("srs")))
				? _TL("success") : _TL("failed")
			));
		}

		if( s.Find('o') >= 0 )	// o: load old style naming, has no effect if l-flag is set.
		{
			SG_Set_OldStyle_Naming();
		}

		return( true );
	}

	//-----------------------------------------------------
	else if( !s.Cmp("-c") || !s.Cmp("--cores") )
	{
		int	nCores	= 1;

		if( CSG_String(Argument).AfterFirst('=').asInt(nCores) )
		{
			SG_OMP_Set_Max_Num_Threads(nCores);
		}

		return( true );
	}

	//-----------------------------------------------------
	else if( !s.Cmp("-s") || !s.Cmp("--story") )
	{
		int	Depth;

		if( CSG_String(Argument).AfterFirst('=').asInt(Depth) )
		{
			SG_Set_History_Depth(Depth);
		}

		return( true );
	}

	//-----------------------------------------------------
	else if( !s.Cmp("-C") || !s.Cmp("--config") )
	{
		Config_Load(CSG_String(Argument).AfterFirst('='));

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
void		Print_Libraries	(void)
{
	if( CMD_Get_Show_Messages() )
	{
		if( CMD_Get_XML() )
		{
			SG_PRINTF(SG_Get_Tool_Library_Manager().Get_Summary(SG_SUMMARY_FMT_XML).c_str());
		}
		else
		{
			CMD_Print(SG_Get_Tool_Library_Manager().Get_Summary(SG_SUMMARY_FMT_FLAT));

			CMD_Print_Error(_TL("select a library"));

			Print_Get_Help();
		}
	}
}

//---------------------------------------------------------
void		Print_Tools		(const CSG_String &Library)
{
	if( CMD_Get_Show_Messages() )
	{
		if( CMD_Get_XML() )
		{
			for(int i=0; i<SG_Get_Tool_Library_Manager().Get_Count(); i++)
			{
				CSG_Tool_Library	*pLibrary	= SG_Get_Tool_Library_Manager().Get_Library(i);

				if( !pLibrary->Get_Library_Name().Cmp(Library) )
				{
					SG_PRINTF(pLibrary->Get_Summary(SG_SUMMARY_FMT_XML , false).c_str());
				}
			}
		}
		else
		{
			for(int i=0; i<SG_Get_Tool_Library_Manager().Get_Count(); i++)
			{
				CSG_Tool_Library	*pLibrary	= SG_Get_Tool_Library_Manager().Get_Library(i);

				if( !pLibrary->Get_Library_Name().Cmp(Library) )
				{
					CMD_Print(pLibrary->Get_Summary(SG_SUMMARY_FMT_FLAT, false));
				}
			}

			CMD_Print_Error(_TL("select a tool"));

			Print_Get_Help();
		}
	}
}

//---------------------------------------------------------
void		Print_Execution	(CSG_Tool *pTool)
{
	if( CMD_Get_Show_Messages() )
	{
		if( CMD_Get_XML() )
		{
			SG_PRINTF(SG_T("<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\n"));
			SG_PRINTF(SG_T("<%s>\n"), SG_XML_LIBRARY);
			SG_PRINTF(SG_T("\t<%s>%s</%s>\n"), SG_XML_LIBRARY_PATH, pTool->Get_File_Name().c_str(), SG_XML_LIBRARY_PATH);
			SG_PRINTF(SG_T("\t<%s>%s</%s>\n"), SG_XML_LIBRARY_NAME, pTool->Get_Library  ().c_str(), SG_XML_LIBRARY_NAME);
			SG_PRINTF(SG_T("</%s>\n"), SG_XML_LIBRARY);
		}
		else
		{
			SG_PRINTF(SG_T("____________________________\n"));
			SG_PRINTF(SG_T("%s: %s\n"), _TL("library path"), SG_File_Get_Path(pTool->Get_File_Name()       ).c_str());
			SG_PRINTF(SG_T("%s: %s\n"), _TL("library name"), SG_File_Get_Name(pTool->Get_File_Name(), false).c_str());
			SG_PRINTF(SG_T("%s: %s\n"), _TL("library     "), pTool->Get_Library().c_str());
			SG_PRINTF(SG_T("%s: %s\n"), _TL("tool        "), pTool->Get_Name   ().c_str());
			SG_PRINTF(SG_T("%s: %s\n"), _TL("identifier  "), pTool->Get_ID     ().c_str());
			SG_PRINTF(SG_T("%s: %s\n"), _TL("author      "), pTool->Get_Author ().c_str());
		#ifdef _OPENMP
			SG_PRINTF(SG_T("%s: %d [%d]\n"), _TL("processors  "), SG_OMP_Get_Max_Num_Threads(), SG_OMP_Get_Max_Num_Procs());
		#endif // _OPENMP
			SG_PRINTF(SG_T("____________________________\n\n"));
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Print_Logo		(void)
{
	if( CMD_Get_Show_Messages() )
	{
		CMD_Print(
			"____________________________\n"
			"\n"
			"   #####   ##   #####    ##\n"
			"  ###     ###  ##       ###\n"
			"   ###   # ## ##  #### # ##\n"
			"    ### ##### ##    # #####\n"
			" ##### #   ##  ##### #   ##\n"
			"____________________________\n"
		);

		Print_Version();
	}
}

//---------------------------------------------------------
void		Print_Version	(void)
{
#if defined(_SAGA_MSW) && defined(_WIN64)
	CMD_Print(SG_T("SAGA Version: ") SAGA_VERSION SG_T(" (64 bit)\n"));
#elif defined(_SAGA_MSW)
	CMD_Print(SG_T("SAGA Version: ") SAGA_VERSION SG_T(" (32 bit)\n"));
#else
	CMD_Print(SG_T("SAGA Version: ") SAGA_VERSION SG_T("\n"));
#endif
}

//---------------------------------------------------------
void		Print_Get_Help	(void)
{
	if( CMD_Get_Show_Messages() )
	{
		CMD_Print(_TL("type -h or --help for further information"));
		CMD_Print("");
	}
}

//---------------------------------------------------------
void		Print_Help		(void)
{
	Print_Logo();

	CMD_Print(
		"under GNU General Public License (GPL)\n"
		"\n"
		"_____________________________________________________________________________\n"
		"\n"
		"Usage:\n"
		"\n"
		"saga_cmd [-h, --help]\n"
		"saga_cmd [-v, --version]\n"
#ifdef _OPENMP
		"saga_cmd [-C, --config][=#][-s, --story][=#][-c, --cores][=#][-f, --flags][=#]\n"
		"  <LIBRARY> <TOOL> <OPTIONS>\n"
		"saga_cmd [-C, --config][=#][-s, --story][=#][-c, --cores][=#][-f, --flags][=#]\n"
		"  <SCRIPT>\n"
#else
		"saga_cmd [-C, --config][=#][-s, --story][=#][-f, --flags][=#]\n"
		"  <LIBRARY> <TOOL> <OPTIONS>\n"
		"saga_cmd [-C, --config][=#][-s, --story][=#][-f, --flags][=#]\n"
		"  <SCRIPT>\n"
#endif
		"\n"
		"[-h], [--help]   : help on usage\n"
		"[-v], [--version]: version information\n"
		"[-s], [--story]  : maximum data history depth (default is unlimited)\n"
		"[-C], [--config] : configuration file (default is 'saga_cmd.ini')\n"
#ifdef _OPENMP
		"[-c], [--cores]  : number of physical processors to use for computation\n"
#endif
		"[-f], [--flags]  : various flags for general usage [qrsilpxo]\n"
		"  q              : no progress report\n"
		"  r              : no messages report\n"
		"  s              : silent mode (no progress and no messages report)\n"
		"  i              : allow user interaction\n"
		"  l              : load translation dictionary\n"
		"  p              : load projections dictionary\n"
		"  x              : use XML markups for synopses and messages\n"
		"  o              : load old style naming\n"
		"<LIBRARY>        : name of the library\n"
		"<TOOL>           : either name or index of the tool\n"
		"<OPTIONS>        : tool specific options\n"
		"<SCRIPT>         : saga cmd script file with one or more tool calls\n"
		"\n"
		"saga_cmd --create-config[=file]\n"
		"   creates a default configuration file. If no file name is specified\n"
		"   it will use \'saga_cmd.ini\'.\n"
		"\n"
		"saga_cmd --create-batch[=file]\n"
		"   creates a batch script file example.\n"
		"\n"
		"saga_cmd --create-docs[=directory]\n"
		"   creates tool documentation in current working directory, if no other\n"
		"   directory is given.\n"
		"\n"
		"_____________________________________________________________________________\n"
		"\n"
		"Example:\n"
		"\n"
		"  saga_cmd ta_lighting 0 -ELEVATION=c:\\dem.sgrd -SHADE=c:\\shade.sgrd\n"
		"\n"
		"_____________________________________________________________________________\n"
		"\n"
		"Tool libraries in the \'tools\' subdirectory of the SAGA installation\n"
		"will be loaded automatically. Additional directories can be specified\n"
		"by adding the environment variable \'SAGA_MLB\' and let it point to one\n"
		"or more directories, just the way it is done with the DOS \'PATH\' variable.\n"
		"\n"
		"A more convenient way to set various saga_cmd options is to edit a\n"
		"configuration file. The default configuration file \'saga_cmd.ini\' will be\n"
		"loaded automatically, if present. You can specify a different configuration "
		"file with the \'-C\' or \'--config\' option. Use \'--create-config\' to\n"
		"generate such a file with default options and edit it to fit your purposes.\n"
		"\n"
		"The SAGA command line interpreter is particularly useful for the processing\n"
		"of complex work flows by defining a series of subsequent tool calls in a\n"
		"script file. Calling saga_cmd with the option \'--create-batch\' will\n"
		"create an example of a DOS batch script file, which might be a good starting\n"
		"point for the implementation of your own specific work flows.\n"
		"\n"
		"_____________________________________________________________________________\n"
		"\n"
		"Please provide the following reference in your work if you are using SAGA:\n"
		"\n"
		"Conrad, O., Bechtel, B., Bock, M., Dietrich, H., Fischer, E., Gerlitz, L.,\n"
		"  Wehberg, J., Wichmann, V., and Boehner, J. (2015):\n"
		"  System for Automated Geoscientific Analyses (SAGA) v. 2.1.4.\n"
		"  Geosci. Model Dev., 8, 1991-2007, doi:10.5194/gmd-8-1991-2015.\n"
		"\n"
		"  http://www.geosci-model-dev.net/8/1991/2015/gmd-8-1991-2015.html\n"
		"\n"
		"_____________________________________________________________________________\n"
	);
}

//---------------------------------------------------------
void		Print_Help		(const CSG_String &Library)
{
	Print_Logo();

	CSG_Tool_Library	*pLibrary;

	if( !Load_Libraries() || !(pLibrary = SG_Get_Tool_Library_Manager().Get_Library(Library, true)) )
	{
		Print_Libraries();
	}
	else
	{
		CMD_Print(pLibrary->Get_Description());
		CMD_Print(pLibrary->Get_Summary(SG_SUMMARY_FMT_FLAT, false));
	}
}

//---------------------------------------------------------
void		Print_Help		(const CSG_String &Library, const CSG_String &Tool)
{
	Print_Logo();

	if( !Load_Libraries() || !SG_Get_Tool_Library_Manager().Get_Library(Library, true) )
	{
		Print_Libraries();
	}
	else
	{
		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool(Library, Tool);

		if( !pTool )
		{
			Print_Tools(Library);
		}
		else
		{
			Print_Execution(pTool);

			CMD_Print(pTool->Get_Summary(true, "", "", SG_SUMMARY_FMT_FLAT));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Create_Batch	(const CSG_String &File)
{
	Print_Logo();

	CSG_File	Stream;

	CSG_String	_File(File.is_Empty() ? SG_File_Make_Path(SG_Dir_Get_Current(), SG_T("saga_cmd"), SG_T("bat")) : File);

	CMD_Print(CSG_String::Format("%s [%s]", _TL("creating batch file example"), _File.c_str()));

	if( !Stream.Open(_File, SG_FILE_W, false) )
	{
		CMD_Print(_TL("failed"));

		return;
	}

	SG_File_Set_Extension(_File, SG_T("txt"));

	//-----------------------------------------------------
	Stream.Printf(
		"@ECHO OFF\n"
		"\n"
		"SET FLAGS=-f=s\n"
		"REM SET SAGA=.\n"
		"REM SET SAGA_MLB=%%SAGA%%\\_private\n"
		"REM PATH=PATH;%%SAGA%%\n"
		"\n"
		"IF EXIST dem.sgrd GOTO :GO\n"
		"IF EXIST srtm.tif GOTO :SRTM\n"
		"\n"
		"ECHO ____________________________\n"
		"ECHO create a Gaussian landscape\n"
		"saga_cmd %%FLAGS%% garden_fractals 5 -GRID=dem.sgrd -NX=400 -NY=400 -H=0.75\n"
		"GOTO :GO\n"
		"\n"
		":SRTM\n"
		"ECHO ____________________________\n"
		"ECHO import and project srtm (geotiff)\n"
		"saga_cmd %%FLAGS%% io_gdal              0 -FILES=srtm.tif -GRIDS=srtm -TRANSFORM=true\n"
		"saga_cmd %%FLAGS%% pj_proj4             7 -SOURCE=srtm.sgrd -GET_USER_GRID=dem.sgrd -GET_USER_SIZE=1000.0 -SOURCE_PROJ=\"+proj=longlat +datum=WGS84\" -TARGET_PROJ=\"+proj=cea +datum=WGS84 +lat_ts=0\"\n"
		"REM alternative method 4 might need to load projection data base with EPSG codes (use --flags=p)\n"
		"REM saga_cmd -f=qp pj_proj4             4 -SOURCE=srtm.sgrd -GET_USER_GRID=dem.sgrd -GET_USER_SIZE=1000.0 -CRS_PROJ4=\"+proj=cea +datum=WGS84 +lat_ts=0\"\n"
		"\n"
		":GO\n"
		"ECHO ____________________________\n"
		"ECHO create contour lines from DEM\n"
		"saga_cmd %%FLAGS%% shapes_grid          5 -GRID=dem.sgrd -CONTOUR=contour.shp -ZSTEP=100.0\n"
		"\n"
		"ECHO ____________________________\n"
		"ECHO do some terrain analysis\n"
		"saga_cmd %%FLAGS%% ta_preprocessor      2 -DEM=dem.sgrd -DEM_PREPROC=dem.sgrd\n"
		"saga_cmd %%FLAGS%% ta_lighting          0 -ELEVATION=dem.sgrd -SHADE=shade.sgrd -METHOD=0 -AZIMUTH=-45 -DECLINATION=45\n"
		"saga_cmd %%FLAGS%% ta_morphometry       0 -ELEVATION=dem.sgrd -SLOPE=slope.sgrd -ASPECT=aspect.sgrd -C_CROS=hcurv.sgrd -C_LONG=vcurv.sgrd\n"
		"saga_cmd %%FLAGS%% ta_hydrology         0 -ELEVATION=dem.sgrd -FLOW=flow.sgrd\n"
		"\n"
		"ECHO ____________________________\n"
		"ECHO run saga cmd script\n"
		"SET INPUT=dem.sgrd;slope.sgrd;hcurv.sgrd;vcurv.sgrd\n"
		"saga_cmd %%FLAGS%% %s\n"
		"\n"
		"PAUSE\n", _File.c_str()
	);

	//-----------------------------------------------------
	CMD_Print(CSG_String::Format("%s [%s]", _TL("creating sub script example"), _File.c_str()));

	if( !Stream.Open(_File, SG_FILE_W, false) )
	{
		CMD_Print(_TL("failed"));

		return;
	}

	Stream.Printf(
		"REM \'REM\' or \'#\' can be used for comments, \'ECHO\' for message output.\n"
		"REM environment variables can be accessed using the ms-dos/window style\n"
		"\n"
		"ECHO ____________________________\n"
		"ECHO cluster analysis and vectorisation\n"
		"\n"
		"# cluster analysis\n"
		"imagery_classification 1 -GRIDS=%%INPUT%% -CLUSTER=cluster.sgrd -NORMALISE=true -NCLUSTER=5\n"
		"\n"
		"# majority filter\n"
		"grid_filter            6 -INPUT=cluster.sgrd -RADIUS=3\n"
		"\n"
		"# vectorization\n"
		"shapes_grid            6 -GRID=cluster.sgrd -POLYGONS=cluster.shp -CLASS_ALL=1\n"
		"\n"
		"# select cluster class 1\n"
		"shapes_tools           3 -SHAPES=cluster.shp -FIELD=ID -EXPRESSION=\"a = 1\"\n"
		"\n"
		"# save selectione\n"
		"shapes_tools           6 -INPUT=cluster.shp -OUTPUT=cluster_class1.shp\n"
		"\n"
		"ECHO ____________________________\n"
		"ECHO\n"
	);

	//-----------------------------------------------------
	CMD_Print(_TL("okay"));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Create_Docs		(const CSG_String &Directory)
{
	Print_Logo();

	CMD_Print(_TL("creating tool documentation files"));

	CSG_String	_Directory(Directory.is_Empty() ? SG_Dir_Get_Current() : Directory);

	if( !SG_Dir_Exists(_Directory) )
	{
		CMD_Print(_TL("directory does not exist"));

		return;
	}

	if( Load_Libraries() )
	{
		CMD_Set_Show_Messages(false);

		SG_Get_Tool_Library_Manager().Get_Summary(_Directory);

		CMD_Print(_TL("okay"));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
