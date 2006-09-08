
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
#define SAGA_CMD_VERSION	"2.0"
#define SAGA_ENV_LIBPATH	"SAGA_MLB"

#define OPT_CREATE_BATCH	"-create_batch"
#define OPT_SILENT			"-silent"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		Execute			(const char *MLB_Path, const char *FileName, const char *ModuleName, int argc, char *argv[]);

void		Error_Library	(const char *MLB_Path);
void		Error_Module	(const char *MLB_Path, const char *FileName);

void		Print_Logo		(const char *MLB_Path);
void		Print_Execution	(const char *MLB_Path, const char *FileName, const char *ModuleName);
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

		if( wxGetEnv("PATH", &ENV_Path) && ENV_Path.Length() > 0 )
		{
			wxSetEnv("PATH", wxString::Format("%s;%s", ENV_Path.c_str(), MLB_Path.c_str()));
		}
		else
		{
			wxSetEnv("PATH", MLB_Path);
		}

		Print_Logo(MLB_Path);

		SG_Set_UI_Callback(Get_Callback());
		SG_Get_Translator()->Create(SG_File_Make_Path(wxGetCwd(), "saga", "lng"), false);

		//-------------------------------------------------
		switch( argc )
		{
		case 1: 
			Error_Library		(MLB_Path);
			break;

		case 2:
			Error_Module		(MLB_Path, argv[1]);
			break;

		default:
 			bResult	= Execute	(MLB_Path, argv[1], argv[2], argc, argv);
			break;
		}

		//-------------------------------------------------
		if( ENV_Path.Length() > 0 )
		{
			wxSetEnv("PATH", ENV_Path);
		}
		else
		{
			wxUnsetEnv("PATH");
		}

		wxBell();
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
bool		Execute(const char *MLB_Path, const char *FileName, const char *ModuleName, int argc, char *argv[])
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
		Print_Execution(MLB_Path, FileName, Library.Get_Selected()->Get_Name());

		if( argc > 3 && !strcmp(OPT_SILENT, argv[3]) )
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
void		Error_Library	(const char *MLB_Path)
{
	int				nLibraries;
	wxDir			Dir;
	wxString		FileName;
	CModule_Library Library;

	if( !Dir.Open(MLB_Path) )
	{
		Print_Error(LNG("invalid module libraries path"), MLB_Path);
	}
	else if(	!Dir.GetFirst(&FileName, "*.dll", wxDIR_FILES|wxDIR_HIDDEN)
			&&	!Dir.GetFirst(&FileName, "*.so" , wxDIR_FILES|wxDIR_HIDDEN) )
	{
		Print_Error(LNG("no valid module library found in path"), MLB_Path);
	}
	else
	{
		Print_Error(LNG("module library"));

		printf("\n%s:\n", LNG("available module libraries"));

		nLibraries	= 0;

		do
		{
			if( Library.Create(FileName, Dir.GetName()) )
			{
				printf("- %s\n", FileName.c_str());
				nLibraries++;
			}
		}
		while( Dir.GetNext(&FileName) );

		printf("\n%d %s", nLibraries, LNG("saga module libraries"));
	}
}

//---------------------------------------------------------
void		Error_Module	(const char *MLB_Path, const char *FileName)
{
	CModule_Library	Library;

	if( !strcmp("-h", FileName) || !strcmp("--help", FileName) )
	{
		Print_Help();
	}
	else if( !strcmp(OPT_CREATE_BATCH, FileName) )
	{
		Create_Example();
	}
	else if( !Library.Create(FileName, MLB_Path) )
	{
		Library.Destroy();

		Print_Error(LNG("module library not found"), FileName);

		Error_Library(MLB_Path);
	}
	else
	{
		Print_Error(LNG("module"));

		printf("\n%s:\n", LNG("available modules"));

		for(int i=0; i<Library.Get_Count(); i++)
		{
			if( Library.Get_Module(i)->is_Interactive() )
			{
				printf("[%d]\t- [%s] %s\n", i, LNG("interactive"), Library.Get_Module(i)->Get_Name());
			}
			else
			{
				printf(" %d\t- %s\n", i, Library.Get_Module(i)->Get_Name());
			}
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Print_Logo		(const char *MLB_Path)
{
	printf("_____________________________________________\n");
	printf("  #####   ##   #####    ##\n");
	printf(" ###     ###  ##       ###\n");
	printf("  ###   # ## ##  #### # ##\n");
	printf("   ### ##### ##    # #####\n");
	printf("##### #   ##  ##### #   ##\n");
	printf("\n");
	printf("SAGA CMD "SAGA_CMD_VERSION"\n");
	printf("\n");
	printf("Copyright (C) 2005 by Olaf Conrad\n");
	printf(LNG("under GNU General Public License (GPL)\n"));
	printf(LNG("type -h or --help for further information\n"));
	printf("_____________________________________________\n");
}

//---------------------------------------------------------
void		Print_Execution	(const char *MLB_Path, const char *FileName, const char *ModuleName)
{
	printf("%s:\t%s\n", LNG("library path"), MLB_Path);
	printf("%s:\t%s\n", LNG("library name"), FileName);
	printf("%s:\t%s\n", LNG("module name"), ModuleName);
	printf("_____________________________________________\n");
	printf("go...\n");
}

//---------------------------------------------------------
void		Print_Help		(void)
{
	printf(
		"Usage:\n"
		"\n"
		"saga_cmd [-h, --help]\n"
		"saga_cmd ["OPT_CREATE_BATCH"]\n"
		"saga_cmd <LIBRARY> <MODULE> ["OPT_SILENT"] <module specific options...>\n"
		"\n"
		"[-h], [--help]\t"			": help on usage\n"
		"["OPT_CREATE_BATCH"]\t"	": create a batch file example\n"
		"["OPT_SILENT"]\t"			": don't wait for user response on errors\n"
		"<LIBRARY>\t"				": file name of the library\n"
		"<MODULE>\t"				": either name or index of the module\n"
		"\n"
		"example:\n"
		"  saga_cmd Terrain_Analysis_Morphometry \n"
		"           \"Local Morphometry\"\n"
		"           -ELEVATION c:\\dem.sgrd\n"
		"           -SLOPE     d:\\slope.sgrd\n"
		"           -ASPECT    d:\\aspect.sgrd\n"
		"           -METHOD    1\n"
		"\n"
		"_____________________________________________\n"
		"Module libraries are expected to be located in\n"
		"the directory, that is specified by the environment\n"
		"variable \'"SAGA_ENV_LIBPATH"\'. If this is not found the\n"
		"current working directory will be searched for instead.\n"
		"\n"
		"SAGA CMD is particularly useful for the automated\n"
		"execution of a series of analysis steps, because it\n"
		"allows you to execute modules using batch files.\n"
		"Calling saga_cmd with the option \'"OPT_CREATE_BATCH"\'\n"
		"creates a batch file example. You probably have to edit\n"
		"the path definitions to make the batch file run on your\n"
		"computer.\n"
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
	FILE		*Stream;
	wxString	FileName(wxString::Format("%s\\saga_cmd_example.bat", wxGetCwd().c_str()));

	printf("\n%s...\n", LNG("creating batch file example"));

	if( (Stream = fopen(FileName, "w")) != NULL )
	{
		fprintf(Stream,
			"@echo off\n"
			"\n"
			"set SAGA_MLB=D:\\DEVELOP\\SAGA_2.0\\bin\\saga_vc\\modules\n"
			"path=D:\\DEVELOP\\SAGA_2.0\\bin\\saga_vc;D:\\DEVELOP\\SAGA_2.0\\bin\\saga_vc\\modules\n"
			"\n"
			"echo _____________________________________________\n"
			"echo SAGASAGASAGASAGASAGASAGASAGASAGASAGASAGASAGAS\n"
			"echo some simple examples\n"
			"echo ;-)\n"
			"\n"
			"saga_cmd.exe Grid_Shapes                  0 -INPUT .\\dem.sgrd -CONTOUR .\\contour.shp -ZSTEP 25.0\n"
			"saga_cmd.exe Terrain_Analysis_Morphometry 3 -ELEVATION .\\dem.sgrd -SLOPE .\\slope.sgrd -ASPECT .\\aspect.sgrd -HCURV .\\hcurv.sgrd -VCURV .\\vcurv.sgrd\n"
			"saga_cmd.exe Terrain_Analysis_Flow        0 -ELEVATION .\\dem.sgrd -CAREA .\\carea.sgrd\n"
			"saga_cmd.exe Terrain_Analysis_Indices     1 -SLOPE .\\slope.sgrd -AREA .\\carea.sgrd -WETNESS .\\wetness.sgrd -STREAMPOW .\\streampow.sgrd -LSFACTOR .\\lsfactor.sgrd\n"
			"saga_cmd.exe Terrain_Analysis_Lighting    0 -ELEVATION .\\dem.sgrd -SHADE .\\shade.sgrd -METHOD 0 -AZIMUTH -45 -DECLINATION 45\n"
			"\n"
			"echo _____________________________________________\n"
			"echo SAGASAGASAGASAGASAGASAGASAGASAGASAGASAGASAGAS\n"
			"echo using data object lists with semicolon (;)\n"
			"echo separated file names\n"
			"echo ;-)\n"
			"\n"
			"saga_cmd.exe Grid_Discretisation          0 -INPUT .\\dem.sgrd;.\\slope.sgrd;.\\aspect.sgrd;.\\hcurv.sgrd;.\\vcurv.sgrd -RESULT .\\cluster.sgrd -STATISTICS cluster.txt -NORMALISE -NCLUSTER 10\n"
			"\n"
			"echo _____________________________________________\n"
			"echo SAGASAGASAGASAGASAGASAGASAGASAGASAGASAGASAGAS\n"
			"echo project from geodetic to gauss-krueger 3\n"
			"echo ;-)\n"
			"\n"
			"saga_cmd.exe Projection_Proj4             0 -PROJ_TYPE 98 -SOURCE .\\latlon.shp -TARGET .\\gk3.shp    -LON_0 9 -X_0 3500000\n"
			"\n"
			"echo _____________________________________________\n"
			"echo SAGASAGASAGASAGASAGASAGASAGASAGASAGASAGASAGAS\n"
			"echo using extra parameter lists\n"
			"echo (-LIST_NAME \"-OPTION1 value1 -OPTION1 value2\")\n"
			"echo ;-)\n"
			"\n"
			"saga_cmd.exe Projection_Proj4             0 -PROJ_TYPE 0  -SOURCE .\\latlon.shp -TARGET .\\albers.shp -LON_0 9 -aea \"-lat_1 25 -lat_2 40\"\n"
			"\n"
			"pause\n"
		);

		fclose(Stream);

		printf("%s\n", LNG("okay"));
	}
	else
	{
		printf("%s\n", LNG("failed"));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
