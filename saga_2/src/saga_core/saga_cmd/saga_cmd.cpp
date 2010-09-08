
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
#define SAGA_CMD_VERSION	SG_T("2.0.5")

#define SYS_ENV_PATH		SG_T("PATH")

#define FLAG_SILENT		SG_T("s")
#define FLAG_INTERACT		SG_T("i")
#define FLAG_PROJ			SG_T("p")
#define FLAG_LANGUAGE		SG_T("l")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Execute			(const SG_Char *MLB_Path, const SG_Char *FileName, const SG_Char *ModuleName, int argc, char *argv[]);

void		Error_Library	(const SG_Char *MLB_Path);
void		Error_Module	(const SG_Char *MLB_Path, const SG_Char *FileName);

void		Print_Logo		(void);
void		Print_Execution	(const SG_Char *MLB_Path, const SG_Char *FileName, const SG_Char *ModuleName, const SG_Char *Author);
void		Print_Help		(void);

void		Create_Example	(void);


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

	if( !wxInitialize() )
	{
		Print_Error(SG_T("initialisation failed"));

		return( 1 );
	}

	//-----------------------------------------------------
	wxString	Flags, CMD_Path, MLB_Path, ENV_Path;

	if( argc > 1 )
	{
		CSG_String	s(argv[1]);

		if( !s.CmpNoCase(SG_T("-h")) || !s.CmpNoCase(SG_T("--help")) )
		{
			Print_Help();

			return( 0 );
		}

		if( !s.CmpNoCase(SG_T("-b")) || !s.CmpNoCase(SG_T("--batch")) )
		{
			Create_Example();

			return( 0 );
		}

		s	= s.BeforeFirst(SG_T('='));

		if( !s.CmpNoCase(SG_T("-f")) || !s.CmpNoCase(SG_T("--flags")) )
		{
			Flags	= CSG_String(argv[1]).AfterFirst(SG_T('=')).c_str();

			argc--;
			argv++;
		}
	}

	//-----------------------------------------------------
	CMD_Path	= SG_File_Get_Path(SG_UI_Get_Application_Path()).c_str();

	if( !wxGetEnv(SG_T("SAGA_MLB"), &MLB_Path) || MLB_Path.Length() == 0 )
	{
		MLB_Path	= SG_File_Make_Path(CMD_Path, SG_T("modules"))  .c_str();
    #if defined( _SAGA_LINUX)
        MLB_Path = wxT(MODULE_LIBRARY_PATH);
    #endif
	}


	if( wxGetEnv(SYS_ENV_PATH, &ENV_Path) && ENV_Path.Length() > 0 )
	{
		wxSetEnv(SYS_ENV_PATH, wxString::Format(wxT(";%s;%s"), MLB_Path.c_str(), SG_File_Make_Path(CMD_Path, SG_T("dll")).c_str()));
	}
	else
	{
		wxSetEnv(SYS_ENV_PATH, wxString::Format(wxT( "%s;%s"), MLB_Path.c_str(), SG_File_Make_Path(CMD_Path, SG_T("dll")).c_str()));
	}

	//-----------------------------------------------------
	SG_Set_UI_Callback(Get_Callback());

	Set_Silent		(Flags.Find(FLAG_SILENT  ) >= 0 ? true : false);
	Set_Interactive	(Flags.Find(FLAG_INTERACT) >= 0 ? true : false);

	Print_Logo();

	if( Flags.Find(FLAG_LANGUAGE) > 0 )
	{
		SG_Get_Translator() .Create(SG_File_Make_Path(CMD_Path, SG_T("saga"), SG_T("lng")), false);
	}

	if( Flags.Find(FLAG_PROJ) > 0 )
	{
		SG_Get_Projections().Create(
			SG_File_Make_Path(CMD_Path, SG_T("saga_prj"), SG_T("dic")),
			SG_File_Make_Path(CMD_Path, SG_T("saga_prj"), SG_T("srs"))
		);
	}

	//-----------------------------------------------------
	bool	bResult	= false;

	switch( argc )
	{
	case 1: 
		Error_Library		(MLB_Path);
		break;

	case 2:
		Error_Module		(MLB_Path, SG_STR_MBTOSG(argv[1]));
		break;

	default:
		bResult	= Execute	(MLB_Path, SG_STR_MBTOSG(argv[1]), SG_STR_MBTOSG(argv[2]), argc - 2, argv + 2);
		break;
	}

	//-----------------------------------------------------
	if( ENV_Path.Length() > 0 )
	{
		wxSetEnv(SYS_ENV_PATH, ENV_Path);
	}
	else
	{
		wxUnsetEnv(SYS_ENV_PATH);
	}

	wxUninitialize();

//---------------------------------------------------------
#ifdef _DEBUG
	Set_Interactive(true);
	Get_Pause();
#endif

#ifdef _MODULE_EXCEPTION
}
_except(1)
{
	Print_Error(LNG("access violation"));
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
bool		Execute(const SG_Char *MLB_Path, const SG_Char *FileName, const SG_Char *ModuleName, int argc, char *argv[])
{
	CModule_Library	Library;

	if( !Library.Create(FileName, MLB_Path) )
	{
		Error_Library(MLB_Path);
	}
	else if( !Library.Select(ModuleName) )
	{
		Print_Error(LNG("module not found"), ModuleName);

		Error_Module(MLB_Path, FileName);
	}
	else if( Library.Get_Selected()->is_Interactive() )
	{
		Print_Error(LNG("cannot execute interactive module"), ModuleName);

		Error_Module(MLB_Path, FileName);
	}
	else
	{
		Print_Execution(MLB_Path, FileName, Library.Get_Selected()->Get_Name(), Library.Get_Selected()->Get_Author());

		Set_Library(&Library);

		if( Library.Execute(argc, argv) )
		{
			Set_Library(NULL);

			return( true );
		}

		Set_Library(NULL);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Error_Library	(const SG_Char *MLB_Path)
{
	int				nLibraries;
	wxDir			Dir;
	wxString		FileName;
	CModule_Library Library;

	if( !Dir.Open(MLB_Path) )
	{
		Print_Error(LNG("invalid module libraries path"), MLB_Path);
	}
	else if(	!Dir.GetFirst(&FileName, wxT("*.dll"), wxDIR_FILES|wxDIR_HIDDEN)
			&&	!Dir.GetFirst(&FileName, wxT("*.so" ), wxDIR_FILES|wxDIR_HIDDEN) 
			&&	!Dir.GetFirst(&FileName, wxT("*.dylib" ), wxDIR_FILES|wxDIR_HIDDEN))
	{
		Print_Error(LNG("no valid module library found in path"), MLB_Path);
	}
	else
	{
		Print_Error(LNG("module library"));

		if( !Get_Silent() )
		{
			SG_PRINTF(SG_T("\n%s:\n"), LNG("available module libraries"));

			nLibraries	= 0;

			do
			{
				if( Library.Create(FileName, Dir.GetName()) )
				{
					SG_PRINTF(SG_T("- %s\n"), FileName.c_str());
					nLibraries++;
				}
			}
			while( Dir.GetNext(&FileName) );

			SG_PRINTF(SG_T("\n%d %s\n"), nLibraries, LNG("SAGA Module Libraries"));
		}
	}

	if( !Get_Silent() )
	{
		SG_PRINTF(SG_T("\n"));
		SG_PRINTF(LNG("type -h or --help for further information"));
		SG_PRINTF(SG_T("\n"));
	}
}

//---------------------------------------------------------
void		Error_Module	(const SG_Char *MLB_Path, const SG_Char *FileName)
{
	CModule_Library	Library;

	if( !Library.Create(FileName, MLB_Path) )
	{
		Library.Destroy();

		Print_Error(LNG("module library not found"), FileName);

		Error_Library(MLB_Path);

		return;
	}

	Print_Error(LNG("module"));

	if( !Get_Silent() )
	{
		SG_PRINTF(SG_T("\n%s:\n"), LNG("available modules"));

		for(int i=0; i<Library.Get_Count(); i++)
		{
			if( Library.Get_Module(i)->is_Interactive() )
			{
				SG_PRINTF(SG_T("[%d]\t- [%s] %s\n"), i, LNG("interactive"), Library.Get_Module(i)->Get_Name());
			}
			else
			{
				SG_PRINTF(SG_T(" %d\t- %s\n"), i, Library.Get_Module(i)->Get_Name());
			}
		}

		SG_PRINTF(SG_T("\n"));
		SG_PRINTF(LNG("type -h or --help for further information"));
		SG_PRINTF(SG_T("\n"));
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
	if( Get_Silent() )
		return;

	SG_PRINTF(SG_T("_____________________________________________\n"));
	SG_PRINTF(SG_T("  #####   ##   #####    ##\n"));
	SG_PRINTF(SG_T(" ###     ###  ##       ###\n"));
	SG_PRINTF(SG_T("  ###   # ## ##  #### # ##\n"));
	SG_PRINTF(SG_T("   ### ##### ##    # #####\n"));
	SG_PRINTF(SG_T("##### #   ##  ##### #   ##\n"));
	SG_PRINTF(SG_T("_____________________________________________\n"));
	SG_PRINTF(SG_T("\n"));
}

//---------------------------------------------------------
void		Print_Execution	(const SG_Char *MLB_Path, const SG_Char *FileName, const SG_Char *ModuleName, const SG_Char *Author)
{
	if( Get_Silent() )
		return;

	SG_PRINTF(SG_T("%s:\t%s\n"), LNG("library path"), MLB_Path);
	SG_PRINTF(SG_T("%s:\t%s\n"), LNG("library name"), FileName);
	SG_PRINTF(SG_T("%s:\t%s\n"), LNG("module name "), ModuleName);
	SG_PRINTF(SG_T("%s:\t%s\n"), LNG("author      "), Author);
	SG_PRINTF(SG_T("_____________________________________________\n"));
	SG_PRINTF(SG_T("go...\n"));
}

//---------------------------------------------------------
void		Print_Help		(void)
{
	Print_Logo();

	SG_PRINTF(
		SG_T("SAGA API ") SAGA_API_VERSION SG_T("\n")
		SG_T("SAGA CMD ") SAGA_CMD_VERSION SG_T("\n")
		SG_T("under GNU General Public License (GPL)\n")
		SG_T("O.Conrad (C) 2005-10\n")
		SG_T("\n")
		SG_T("Usage:\n")
		SG_T("\n")
		SG_T("saga_cmd [-h, --help]\n")
		SG_T("saga_cmd [-b, --batch]\n")
		SG_T("saga_cmd [-f, --flags][=silp] <LIBRARY> <MODULE> <module specific options...>\n")
		SG_T("\n")
		SG_T("[-h], [--help ]: help on usage\n")
		SG_T("[-b], [--batch]: create a batch file example\n")
		SG_T("[-f], [--flags]: various flags for general usage\n")
		SG_T("  s: silent mode\n")
		SG_T("  i: allow user interaction\n")
		SG_T("  l: load translation dictionary\n")
		SG_T("  p: load projections dictionary\n")
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
		SG_T("and let it point to the desired directory\n")
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Create_Example	(void)
{
	CSG_String	FileName(CSG_String::Format(SG_T("%s\\saga_cmd_example.bat"), wxGetCwd().c_str()));
	CSG_File	Stream;

	SG_PRINTF(SG_T("\n%s...\n"), LNG("creating batch file example"));

	if( !Stream.Open(FileName, SG_FILE_W, false) )
	{
		SG_PRINTF(SG_T("%s\n"), LNG("failed"));

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

	SG_PRINTF(SG_T("%s\n"), LNG("okay"));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
