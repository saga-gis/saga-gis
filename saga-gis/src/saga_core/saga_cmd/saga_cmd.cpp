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
#include <wx/app.h>
#include <wx/utils.h>
#include <wx/dir.h>
#include <wx/filename.h>

#include <saga_api/saga_api.h>

#include "callback.h"

#include "module_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Check_First		(const CSG_String &Argument);
bool		Check_Flags		(const CSG_String &Argument);

bool		Execute			(int argc, char *argv[]);

bool		Load_Libraries	(void);

void		Print_Libraries	(void);
void		Print_Modules	(CSG_Module_Library *pLibrary);
void		Print_Execution	(CSG_Module_Library *pLibrary, CSG_Module *pModule);

void		Print_Logo		(void);
void		Print_Get_Help	(void);
void		Print_Help		(void);
void		Print_Version	(void);

void		Create_Example	(void);
void		Create_Docs		(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		main	(int argc, char *argv[])
{
//---------------------------------------------------------
#if !defined(_DEBUG) && defined(_SAGA_VC)
#define _MODULE_EXCEPTION
_try 
{
#endif
//---------------------------------------------------------

#if wxCHECK_VERSION(2, 8, 11)
	if( !wxInitialize(argc, argv) )
#else
	if( !wxInitialize() )
#endif
	{
		CMD_Print_Error(SG_T("initialisation failed"));

		return( 1 );
	}

	setlocale(LC_NUMERIC, "C");
	
	SG_Set_UI_Callback(CMD_Get_Callback());

	//-----------------------------------------------------
	if( !Load_Libraries() )
	{
		Print_Get_Help();

		return( 1 );
	}

	//-----------------------------------------------------
	if( argc <= 1 )
	{
		Print_Libraries();

		Print_Get_Help();

		return( 1 );
	}

	//-----------------------------------------------------
	if( Check_First(argv[1]) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	while( argc > 1 && Check_Flags(argv[1]) )
	{
		argc--;	argv++;
	}

	if( argc <= 1 )
	{
		CMD_Print_Error(_TL("no arguments for saga call"));

		Print_Get_Help();

		return( 1 );
	}

	//-----------------------------------------------------
	bool	bResult;

	Print_Logo();

	bResult	= Execute(argc, argv);

	//-----------------------------------------------------
	wxUninitialize();

//---------------------------------------------------------
#ifdef _DEBUG
	CMD_Set_Interactive(true);
	CMD_Get_Pause();
#endif

#ifdef _MODULE_EXCEPTION
}
_except(1)
{
	Print_Error(_TL("access violation"));
}
#endif
//---------------------------------------------------------

	return( bResult ? 0 : 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Check_First		(const CSG_String &Argument)
{
	if( !Argument.CmpNoCase(SG_T("-h")) || !Argument.CmpNoCase(SG_T("--help")) )
	{
		Print_Help();

		return( true );
	}

	if( !Argument.CmpNoCase(SG_T("-v")) || !Argument.CmpNoCase(SG_T("--version")) )
	{
		Print_Version();

		return( true );
	}

	if( !Argument.CmpNoCase(SG_T("-b")) || !Argument.CmpNoCase(SG_T("--batch")) )
	{
		Create_Example();

		return( true );
	}

	if( !Argument.CmpNoCase(SG_T("-d")) || !Argument.CmpNoCase(SG_T("--docs")) )
	{
		Create_Docs();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		Check_Flags		(const CSG_String &Argument)
{
	CSG_String	s(Argument.BeforeFirst(SG_T('=')));

	if( !s.CmpNoCase(SG_T("-f")) || !s.CmpNoCase(SG_T("--flags")) )
	{
		s	= CSG_String(Argument).AfterFirst(SG_T('='));

		CMD_Set_Show_Progress(s.Find('q') < 0 && s.Find('s') < 0);	// q, s: no progress report
		CMD_Set_Show_Messages(s.Find('r') < 0 && s.Find('s') < 0);	// r, s: no messages report
		CMD_Set_Interactive  (s.Find('i') >= 0                  );	// i: allow user interaction
		CMD_Set_XML          (s.Find('x') >= 0                  );	// x: message output as xml

		if( s.Find('l') >= 0 )	// l: load translation dictionary
		{
			SG_Get_Translator() .Create(SG_File_Make_Path(SG_UI_Get_Application_Path(), SG_T("saga"), SG_T("lng")), false);
		}

		if( s.Find('p') >= 0 )	// p: load projections dictionary
		{
			SG_Get_Projections().Create(SG_File_Make_Path(SG_UI_Get_Application_Path(), SG_T("saga_prj"), SG_T("srs")));
		}

		return( true );
	}

	else if( !s.CmpNoCase(SG_T("-c")) || !s.CmpNoCase(SG_T("--cores")) )
	{
		#ifdef _OPENMP
		int	nCores	= 1;

		if( !CSG_String(Argument).AfterFirst(SG_T('=')).asInt(nCores) || nCores > SG_Get_Max_Num_Procs_Omp() )
		{
			nCores	= SG_Get_Max_Num_Procs_Omp();
		}

		SG_Set_Max_Num_Threads_Omp(nCores);
		#endif // _OPENMP

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
bool		Execute(int argc, char *argv[])
{
	CSG_Module_Library	*pLibrary;
	CSG_Module			*pModule;

	if( argc == 1 || (pLibrary = SG_Get_Module_Library_Manager().Get_Library(CSG_String(argv[1]), true)) == NULL )
	{
		Print_Libraries();

		return( false );
	}

	if( argc == 2
	||  (  (pModule = pLibrary->Get_Module(CSG_String(argv[2])        )) == NULL
	    && (pModule = pLibrary->Get_Module(CSG_String(argv[2]).asInt())) == NULL) )
	{
		Print_Modules(pLibrary);

		return( false );
	}

	if( argc == 3 && CMD_Get_XML() )
	{	// Just output module synopsis as XML-tagged text, then return.
		CMD_Print(stderr, pModule->Get_Summary(true, "", "", true));

		return true;
	}

	if( pModule->is_Interactive() )
	{
		CMD_Print_Error(_TL("cannot execute interactive module"), pModule->Get_Name());

		return( false );
	}

	//-----------------------------------------------------
	Print_Execution(pLibrary, pModule);

	CCMD_Module	CMD_Module(pModule);

	return( CMD_Module.Execute(argc - 2, argv + 2) );
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
	int	n	= SG_Get_Module_Library_Manager().Add_Directory(Directory, false);
	CMD_Set_Show_Messages(bShow);

	return( n > 0 );
}

//---------------------------------------------------------
bool		Load_Libraries(void)
{
	wxString	Path, CMD_Path	= SG_File_Get_Path(SG_UI_Get_Application_Path()).c_str();

    #if defined(_SAGA_LINUX)
		Load_Libraries(wxT(MODULE_LIBRARY_PATH));
	#else
		if( wxGetEnv(wxT("PATH"), &Path) && Path.Length() > 0 )
		{
			Path	+= wxT(";");
		}

		Path	+= SG_File_Make_Path(CMD_Path, SG_T("dll"    )).c_str(); Path += wxT(";");
		Path	+= SG_File_Make_Path(CMD_Path, SG_T("modules")).c_str();

		wxSetEnv("PATH"            , Path);
		wxSetEnv("GDAL_DRIVER_PATH", SG_File_Make_Path(CMD_Path, SG_T("dll")).c_str());

		Load_Libraries(SG_File_Make_Path(CMD_Path, SG_T("modules")));
    #endif

	if( wxGetEnv(SG_T("SAGA_MLB"), &Path) && wxDirExists(Path) )
	{
		Load_Libraries(&Path);
	}

	if( SG_Get_Module_Library_Manager().Get_Count() <= 0 )
	{
		CMD_Print_Error(SG_T("could not load any module library"));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Print_Libraries	(void)
{
	CMD_Print_Error(_TL("library"));

	if( CMD_Get_Show_Messages() )
	{
		if( CMD_Get_XML() )
		{
			CMD_Print(SG_Get_Module_Library_Manager().Get_Summary(SG_SUMMARY_FMT_XML_NO_INTERACTIVE));
		}
		else
		{
			CMD_Print(SG_Get_Module_Library_Manager().Get_Summary(SG_SUMMARY_FMT_FLAT_NO_INTERACTIVE));
		}
	}
}

//---------------------------------------------------------
void		Print_Modules	(CSG_Module_Library *pLibrary)
{
	CMD_Print_Error(_TL("module"));

	if( CMD_Get_Show_Messages() )
	{
		if( CMD_Get_XML() )
		{
			CMD_Print(pLibrary->Get_Summary(SG_SUMMARY_FMT_XML_NO_INTERACTIVE));
		}
		else
		{
			CMD_Print(pLibrary->Get_Summary(SG_SUMMARY_FMT_FLAT_NO_INTERACTIVE));

			Print_Get_Help();
		}
	}
}

//---------------------------------------------------------
void		Print_Execution	(CSG_Module_Library *pLibrary, CSG_Module *pModule)
{
	if( CMD_Get_Show_Messages() )
	{
		if( CMD_Get_XML() )
		{
			SG_PRINTF(SG_T("<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\n"));
			SG_PRINTF(SG_T("\n<%s>"), SG_XML_LIBRARY);
			SG_PRINTF(SG_T("\n\t<%s>%s</%s>"), SG_XML_LIBRARY_PATH, pLibrary->Get_File_Name().c_str(), SG_XML_LIBRARY_PATH);
			SG_PRINTF(SG_T("\n\t<%s>%s</%s>"), SG_XML_LIBRARY_NAME, pLibrary->Get_Name()     .c_str(), SG_XML_LIBRARY_NAME);
			SG_PRINTF(SG_T("\n</%s>"), SG_XML_LIBRARY);
		}
		else
		{
			SG_PRINTF(SG_T("%s:\t%s\n"), _TL("library path"), pLibrary->Get_File_Name().c_str());
			SG_PRINTF(SG_T("%s:\t%s\n"), _TL("library name"), pLibrary->Get_Name     ().c_str());
			SG_PRINTF(SG_T("%s:\t%s\n"), _TL("module name "), pModule ->Get_Name     ().c_str());
			SG_PRINTF(SG_T("%s:\t%s\n"), _TL("author      "), pModule ->Get_Author   ().c_str());
			SG_PRINTF(SG_T("_____________________________________________\n"));
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
		CMD_Print("_____________________________________________");
		CMD_Print("  #####   ##   #####    ##");
		CMD_Print(" ###     ###  ##       ###");
		CMD_Print("  ###   # ## ##  #### # ##");
		CMD_Print("   ### ##### ##    # #####");
		CMD_Print("##### #   ##  ##### #   ##");
		CMD_Print("_____________________________________________");
		CMD_Print("");
	}
}

//---------------------------------------------------------
void		Print_Version	(void)
{
	CMD_Print(SG_T("SAGA Version: ") SAGA_VERSION SG_T("\n"));
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

	SG_PRINTF(
		SG_T("Version ") SAGA_VERSION SG_T("\n")
		SG_T("under GNU General Public License (GPL)\n")
		SG_T("\n")
		SG_T("Usage:\n")
		SG_T("\n")
		SG_T("saga_cmd [-h, --help]\n")
		SG_T("saga_cmd [-v, --version]\n")
		SG_T("saga_cmd [-b, --batch]\n")
		SG_T("saga_cmd [-d, --docs]\n")
#ifdef _OPENMP
		SG_T("saga_cmd [-c, --cores][= # of CPU cores] <LIBRARY> <MODULE> <module specific options...>\n")
#endif
		SG_T("saga_cmd [-f, --flags][=qrsilpx] <LIBRARY> <MODULE> <module specific options...>\n")
		SG_T("\n")
		SG_T("[-h], [--help]: help on usage\n")
		SG_T("[-v], [--version]: print version information\n")
		SG_T("[-b], [--batch]: create a batch file example\n")
		SG_T("[-d], [--docs]: create module documentation files in current working directory\n")
#ifdef _OPENMP
		SG_T("[-c], [--cores]: number of physical processors to use for computation\n")
#endif
		SG_T("[-f], [--flags]: various flags for general usage\n")
		SG_T("  q: no progress report\n")
		SG_T("  r: no messages report\n")
		SG_T("  s: silent mode (no progress and no messages report)\n")
		SG_T("  i: allow user interaction\n")
		SG_T("  l: load translation dictionary\n")
		SG_T("  p: load projections dictionary\n")
		SG_T("  x: use XML markups for synopses and messages\n")
		SG_T("<LIBRARY>\t")	SG_T(": file name of the library\n")
		SG_T("<MODULE>\t")	SG_T(": either name or index of the module\n")
		SG_T("\n")
		SG_T("example:\n")
		SG_T("  saga_cmd -f=sp ta_morphometry \n")
		SG_T("           \"Local Morphometry\"\n")
		SG_T("           -ELEVATION c:\\dem.sgrd\n")
		SG_T("           -SLOPE     d:\\slope.sgrd\n")
		SG_T("           -ASPECT    d:\\aspect.sgrd\n")
		SG_T("           -METHOD    1\n")
		SG_T("\n")
		SG_T("_____________________________________________\n")
		SG_T("Module libraries are expected to be in the SAGA installation\n")
		SG_T("directory or its \'modules\' subdirectory. If this is not found\n")
		SG_T("the current working directory will be searched for instead.\n")
		SG_T("Alternatively you can add the environment variable \'SAGA_MLB\'\n")
		SG_T("and let it point to the desired directory.\n")
		SG_T("\n")
		SG_T("SAGA CMD is particularly useful for the automated\n")
		SG_T("execution of a series of analysis steps, because it\n")
		SG_T("allows you to execute modules using batch files.\n")
		SG_T("Calling saga_cmd with the option \'-b\' or \'--batch\'\n")
		SG_T("creates a batch file example. You probably have to edit\n")
		SG_T("the path definitions to make the batch file run on your\n")
		SG_T("computer.\n")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Create_Example	(void)
{
	CSG_String	FileName(CSG_String::Format(SG_T("%s\\saga_cmd_example.bat"), wxGetCwd().wx_str()));
	CSG_File	Stream;

	CMD_Print(_TL("creating batch file example"));

	if( !Stream.Open(FileName, SG_FILE_W, false) )
	{
		CMD_Print(_TL("failed"));

		return;
	}

	Stream.Printf(
		SG_T("@echo off\n")
		SG_T("\n")
		SG_T("set SAGA=.\n")
		SG_T("set SAGA_MLB=%%SAGA%%\\modules\n")
		SG_T("PATH=PATH;%%SAGA%%;%%SAGA_MLB%%\n")
		SG_T("\n")
		SG_T("if exist .\\srtm.asc goto :SRTM\n")
		SG_T("echo _____________________________________________\n")
		SG_T("echo import SRTM-DEM from ESRI ASCII grid\n")
		SG_T("saga_cmd.exe recreations_fractals 5 -GRID .\\dem.sgrd -NX 400 -NY 400 -H 0.75\n")
		SG_T("\n")
		SG_T("goto :GO\n")
		SG_T("\n")
		SG_T(":SRTM\n")
		SG_T("echo _____________________________________________\n")
		SG_T("echo import SRTM-DEM from ESRI ASCII grid\n")
		SG_T("saga_cmd.exe io_grid              1 -FILE .\\srtm.asc -GRID .\\srtm.sgrd\n")
		SG_T("\n")
		SG_T("echo _____________________________________________\n")
		SG_T("echo projection from geodetic to UTM zone 32\n")
		SG_T("\n")
		SG_T("saga_cmd.exe pj_proj4             1 -SOURCE .\\srtm.sgrd -OUT_GRID .\\dem.sgrd -PROJ_TYPE 109 -utm_zone 32 -TARGET_TYPE 1 -GET_AUTOFIT_GRIDSIZE 90.0\n")
		SG_T("\n")
		SG_T(":GO\n")
		SG_T("echo _____________________________________________\n")
		SG_T("echo create contour lines from DEM\n")
		SG_T("\n")
		SG_T("saga_cmd.exe shapes_grid          5 -INPUT .\\dem.sgrd -CONTOUR .\\contour.shp -ZSTEP 25.0\n")
		SG_T("\n")
		SG_T("echo _____________________________________________\n")
		SG_T("echo do some terrain analysis\n")
		SG_T("\n")
		SG_T("saga_cmd.exe ta_preprocessor      1 -DEM .\\dem.sgrd -DEM_PREPROC .\\dem.sgrd\n")
		SG_T("saga_cmd.exe ta_lighting          0 -ELEVATION .\\dem.sgrd -SHADE .\\shade.sgrd -METHOD 0 -AZIMUTH -45 -DECLINATION 45\n")
		SG_T("saga_cmd.exe ta_morphometry       0 -ELEVATION .\\dem.sgrd -SLOPE .\\slope.sgrd -ASPECT .\\aspect.sgrd -HCURV .\\hcurv.sgrd -VCURV .\\vcurv.sgrd\n")
		SG_T("saga_cmd.exe ta_hydrology         0 -ELEVATION .\\dem.sgrd -CAREA .\\carea.sgrd\n")
		SG_T("saga_cmd.exe ta_hydrology        14 -SLOPE .\\slope.sgrd -AREA .\\carea.sgrd -WETNESS .\\wetness.sgrd -STREAMPOW .\\streampow.sgrd -LSFACTOR .\\lsfactor.sgrd\n")
		SG_T("\n")
		SG_T("echo _____________________________________________\n")
		SG_T("echo perform cluster analysis\n")
		SG_T("\n")
		SG_T("saga_cmd.exe grid_discretisation  1 -INPUT .\\dem.sgrd;.\\slope.sgrd;.\\hcurv.sgrd;.\\vcurv.sgrd -RESULT .\\cluster.sgrd -STATISTICS .\\cluster.txt -NORMALISE -NCLUSTER 10\n")
		SG_T("\n")
		SG_T("pause\n")
	);

	CMD_Print(_TL("okay"));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Create_Docs		(void)
{
	Print_Logo();

	if( Load_Libraries() )
	{
		CMD_Print(_TL("creating module documentation files"));

		CMD_Set_Show_Messages(false);

		SG_Get_Module_Library_Manager().Get_Summary(wxGetCwd().wx_str());

		CMD_Print(_TL("okay"));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
