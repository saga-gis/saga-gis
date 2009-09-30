
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

#include <saga_api/saga_api.h>

#include "callback.h"

#include "module_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SAGA_CMD_VERSION	SG_T("2.0")
#define SAGA_ENV_LIBPATH	SG_T("SAGA_MLB")

#define SYS_ENV_PATH		SG_T("PATH")

#define OPT_CREATE_BATCH	SG_T("-create_batch")
#define OPT_SILENT			SG_T("-silent")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Execute			(const SG_Char *MLB_Path, const SG_Char *FileName, const SG_Char *ModuleName, int argc, char *argv[]);

void		Error_Library	(const SG_Char *MLB_Path);
void		Error_Module	(const SG_Char *MLB_Path, const SG_Char *FileName);

void		Print_Logo		(const SG_Char *MLB_Path);
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

	bool	bResult	= false;

	if( wxInitialize() )
	{
		wxString	MLB_Path, ENV_Path;

		MLB_Path	= wxGetenv(SAGA_ENV_LIBPATH) ? wxGetenv(SAGA_ENV_LIBPATH) : wxGetCwd();

		if( wxGetEnv(SYS_ENV_PATH, &ENV_Path) && ENV_Path.Length() > 0 )
		{
			wxSetEnv(SYS_ENV_PATH, wxString::Format(wxT("%s;%s"), ENV_Path.c_str(), MLB_Path.c_str()));
		}
		else
		{
			wxSetEnv(SYS_ENV_PATH, MLB_Path);
		}

		Print_Logo(MLB_Path);

		SG_Set_UI_Callback(Get_Callback());

		SG_Get_Translator() .Create(SG_File_Make_Path(wxGetCwd(), wxT("saga"), wxT("lng")), false);
		SG_Get_Projections().Create(SG_File_Make_Path(wxGetCwd(), wxT("saga"), wxT("srs")));

		//-------------------------------------------------
		switch( argc )
		{
		case 1: 
			Error_Library		(MLB_Path);
			break;

		case 2:
			Error_Module		(MLB_Path, SG_STR_MBTOSG(argv[1]));
			break;

		default:
 			bResult	= Execute	(MLB_Path, SG_STR_MBTOSG(argv[1]), SG_STR_MBTOSG(argv[2]), argc, argv);
			break;
		}

		//-------------------------------------------------
		if( ENV_Path.Length() > 0 )
		{
			wxSetEnv(SYS_ENV_PATH, ENV_Path);
		}
		else
		{
			wxUnsetEnv(SYS_ENV_PATH);
		}

		wxUninitialize();
	}

//---------------------------------------------------------
#ifdef _DEBUG
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
	bool			bResult	= false;
	int				i;
	CModule_Library	Library;

	if( !Library.Create(FileName, MLB_Path) )
	{
		Error_Library(MLB_Path);
	}
	else if( !Library.Select(ModuleName) )
	{
		Library.Destroy();

		Print_Error(LNG("module not found"), ModuleName);

		Error_Module(MLB_Path, FileName);
	}
	else if( Library.Get_Selected()->is_Interactive() )
	{
		Library.Destroy();

		Print_Error(LNG("cannot execute interactive module"), ModuleName);

		Error_Module(MLB_Path, FileName);
	}
	else
	{
		Print_Execution(MLB_Path, FileName, Library.Get_Selected()->Get_Name(), Library.Get_Selected()->Get_Author());

		if( argc > 3 && !SG_STR_CMP(OPT_SILENT, SG_STR_MBTOSG(argv[3])) )
		{
			i	= 3;

			Set_Silent(true);
		}
		else
		{
			i	= 2;
		}

		Set_Library(&Library);
		bResult	= Library.Execute(argc - i, argv + i);
		Set_Library(NULL);
	}

	return( bResult );
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
			&&	!Dir.GetFirst(&FileName, wxT("*.so" ), wxDIR_FILES|wxDIR_HIDDEN) )
	{
		Print_Error(LNG("no valid module library found in path"), MLB_Path);
	}
	else
	{
		Print_Error(LNG("module library"));

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

	SG_PRINTF(SG_T("\n"));
	SG_PRINTF(LNG("type -h or --help for further information"));
	SG_PRINTF(SG_T("\n"));
}

//---------------------------------------------------------
void		Error_Module	(const SG_Char *MLB_Path, const SG_Char *FileName)
{
	CModule_Library	Library;

	if( !SG_STR_CMP(SG_T("-h"), FileName) || !SG_STR_CMP(SG_T("--help"), FileName) )
	{
		Print_Help();
	}
	else if( !SG_STR_CMP(OPT_CREATE_BATCH, FileName) )
	{
		Create_Example();
	}
	else 
	{
		if( !Library.Create(FileName, MLB_Path) )
		{
			Library.Destroy();

			Print_Error(LNG("module library not found"), FileName);

			Error_Library(MLB_Path);
		}
		else
		{
			Print_Error(LNG("module"));

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
void		Print_Logo		(const SG_Char *MLB_Path)
{
	SG_PRINTF(SG_T("_____________________________________________\n"));
	SG_PRINTF(SG_T("  #####   ##   #####    ##\n"));
	SG_PRINTF(SG_T(" ###     ###  ##       ###\n"));
	SG_PRINTF(SG_T("  ###   # ## ##  #### # ##\n"));
	SG_PRINTF(SG_T("   ### ##### ##    # #####\n"));
	SG_PRINTF(SG_T("##### #   ##  ##### #   ##\n"));
	SG_PRINTF(SG_T("\n"));
	SG_PRINTF(SG_T("SAGA CMD ") SAGA_API_VERSION SG_T("\n"));
	SG_PRINTF(SG_T("_____________________________________________\n"));
}

//---------------------------------------------------------
void		Print_Execution	(const SG_Char *MLB_Path, const SG_Char *FileName, const SG_Char *ModuleName, const SG_Char *Author)
{
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
	SG_PRINTF(
		SG_T("(C) 2005-08 by O.Conrad\n")
		SG_T("under GNU General Public License (GPL)\n")
		SG_T("\n")
		SG_T("Usage:\n")
		SG_T("\n")
		SG_T("saga_cmd [-h, --help]\n")
		SG_T("saga_cmd [") OPT_CREATE_BATCH SG_T("]\n")
		SG_T("saga_cmd <LIBRARY> <MODULE> [") OPT_SILENT SG_T("] <module specific options...>\n")
		SG_T("\n")
		SG_T("[-h], [--help]\t")				SG_T(": help on usage\n")
		SG_T("[") OPT_CREATE_BATCH SG_T("]\t")	SG_T(": create a batch file example\n")
		SG_T("[") OPT_SILENT SG_T("]\t")		SG_T(": don't wait for user response on errors\n")
		SG_T("<LIBRARY>\t")						SG_T(": file name of the library\n")
		SG_T("<MODULE>\t")						SG_T(": either name or index of the module\n")
		SG_T("\n")
		SG_T("example:\n")
		SG_T("  saga_cmd ta_morphometry \n")
		SG_T("           \"Local Morphometry\"\n")
		SG_T("           -ELEVATION c:\\dem.sgrd\n")
		SG_T("           -SLOPE     d:\\slope.sgrd\n")
		SG_T("           -ASPECT    d:\\aspect.sgrd\n")
		SG_T("           -METHOD    1\n")
		SG_T("\n")
		SG_T("_____________________________________________\n")
		SG_T("Module libraries are expected to be located in\n")
		SG_T("the directory, that is specified by the environment\n")
		SG_T("variable \'") SAGA_ENV_LIBPATH SG_T("\'. If this is not found the\n")
		SG_T("current working directory will be searched for instead.\n")
		SG_T("\n")
		SG_T("SAGA CMD is particularly useful for the automated\n")
		SG_T("execution of a series of analysis steps, because it\n")
		SG_T("allows you to execute modules using batch files.\n")
		SG_T("Calling saga_cmd with the option \'") OPT_CREATE_BATCH SG_T("\'\n")
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

	if( Stream.Open(FileName, SG_FILE_W, false) )
	{
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
	else
	{
		SG_PRINTF(SG_T("%s\n"), LNG("failed"));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
