
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
//                      config.cpp                       //
//                                                       //
//          Copyright (C) 2017 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "config.h"
#include "callback.h"

//---------------------------------------------------------
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/wfstream.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include <wx/app.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Config_Read(wxConfigBase *pConfig, const wxString &Group, const wxString &Key, bool &Value)
{
	pConfig->SetPath("/" + Group);	return( pConfig->Read(Key, &Value) );
}

bool	Config_Read(wxConfigBase *pConfig, const wxString &Group, const wxString &Key, int &Value)
{
	pConfig->SetPath("/" + Group);	return( pConfig->Read(Key, &Value) );
}

bool	Config_Read(wxConfigBase *pConfig, const wxString &Group, const wxString &Key, double &Value)
{
	pConfig->SetPath("/" + Group);	return( pConfig->Read(Key, &Value) );
}

bool	Config_Read(wxConfigBase *pConfig, const wxString &Group, const wxString &Key, wxString &Value)
{
	pConfig->SetPath("/" + Group);	return( pConfig->Read(Key, &Value) );
}

//---------------------------------------------------------
bool	Config_Write(wxConfigBase *pConfig, const wxString &Group, const wxString &Key, bool Value)
{
	pConfig->SetPath("/" + Group);	return( pConfig->Write(Key, Value) ? pConfig->Flush() : false );
}

bool	Config_Write(wxConfigBase *pConfig, const wxString &Group, const wxString &Key, int Value)
{
	pConfig->SetPath("/" + Group);	return( pConfig->Write(Key, Value) ? pConfig->Flush() : false );
}

bool	Config_Write(wxConfigBase *pConfig, const wxString &Group, const wxString &Key, double Value)
{
	pConfig->SetPath("/" + Group);	return( pConfig->Write(Key, Value) ? pConfig->Flush() : false );
}

bool	Config_Write(wxConfigBase *pConfig, const wxString &Group, const wxString &Key, const wxString &Value)
{
	pConfig->SetPath("/" + Group);	return( pConfig->Write(Key, Value) ? pConfig->Flush() : false );
}

bool	Config_Write(wxConfigBase *pConfig, const wxString &Group, const wxString &Key, const SG_Char *Value)
{
	pConfig->SetPath("/" + Group);	return( pConfig->Write(Key, Value) ? pConfig->Flush() : false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Config_Create	(wxConfigBase *pConfig)
{
	Config_Write(pConfig,   "CMD", "NO_PROGRESS"         , false   );	// q, s: no progress report
	Config_Write(pConfig,   "CMD", "NO_MESSAGES"         , false   );	// r, s: no messages report
	Config_Write(pConfig,   "CMD", "INTERACTIVE"         , false   );	// i: allow user interaction
	Config_Write(pConfig,   "CMD", "XML_MESSAGE"         , false   );	// x: message output as xml

	Config_Write(pConfig, "TOOLS", "LNG_FILE_DIC"        , SG_T(""));	// translation dictionary
	Config_Write(pConfig, "TOOLS", "CRS_CODE_DB"         , SG_Get_Projections().Get_UseInternalDB() ? 0 : 1);
	Config_Write(pConfig, "TOOLS", "OMP_THREADS_MAX"     , SG_OMP_Get_Max_Num_Procs());
	Config_Write(pConfig, "TOOLS", "ADD_LIB_PATHS"       , SG_T(""));	// additional tool library paths (aka SAGA_TLB)

	Config_Write(pConfig,  "DATA", "GRID_CACHE_TMPDIR"   , SG_Grid_Cache_Get_Directory   ());
	Config_Write(pConfig,  "DATA", "GRID_CACHE_MODE"     , SG_Grid_Cache_Get_Mode        ());
	Config_Write(pConfig,  "DATA", "GRID_CACHE_THRESHLOD", SG_Grid_Cache_Get_Threshold_MB());
	Config_Write(pConfig,  "DATA", "GRID_COORD_PRECISION", CSG_Grid_System::Get_Precision());
	Config_Write(pConfig,  "DATA", "HISTORY_DEPTH"       , SG_Get_History_Depth());
	Config_Write(pConfig,  "DATA", "HISTORY_LISTS"       , SG_Get_History_Ignore_Lists() != 0);

	return( true );
}

//---------------------------------------------------------
bool	Config_Load		(wxConfigBase *pConfig)
{
#if   defined(_SAGA_LINUX)
	CSG_String	Path_Shared	= SHARE_PATH;
#elif defined(_SAGA_MSW)
	CSG_String	Path_Shared	= SG_UI_Get_Application_Path(true);
#endif

	bool bValue; int iValue; double dValue; wxString sValue;

	//-----------------------------------------------------
	Config_Read(pConfig, "CMD", "NO_PROGRESS", bValue = false); CMD_Set_Show_Progress(bValue == false);	// q, s: no progress report
	Config_Read(pConfig, "CMD", "NO_MESSAGES", bValue = false); CMD_Set_Show_Messages(bValue == false);	// r, s: no messages report
	Config_Read(pConfig, "CMD", "INTERACTIVE", bValue = false); CMD_Set_Interactive  (bValue ==  true);	// i: allow user interaction
	Config_Read(pConfig, "CMD", "XML_MESSAGE", bValue = false); CMD_Set_XML          (bValue ==  true);	// x: message output as xml

	//-----------------------------------------------------
	if( Config_Read(pConfig, "TOOLS", "LNG_FILE_DIC", sValue) && wxFileExists(sValue) )	// load translation dictionary
	{
		CMD_Print(CSG_String::Format("\n%s:", _TL("loading translation dictionary")));
		CMD_Print(CSG_String::Format("\n%s.",
		//	SG_Get_Translator().Create(SG_File_Make_Path(Path_Shared, SG_T("saga"), SG_T("lng")), false)
			SG_Get_Translator().Create(&sValue, false)
			? _TL("success") : _TL("failed")
		));
	}

	if( Config_Read(pConfig, "TOOLS", "CRS_CODE_DB"         , iValue) )	{	SG_Get_Projections().Set_UseInternalDB(iValue == 0);	}

	if( Config_Read(pConfig, "TOOLS", "OMP_THREADS_MAX"     , iValue) )	{	SG_OMP_Set_Max_Num_Threads(iValue);	}

	if( Config_Read(pConfig, "TOOLS", "ADD_LIB_PATHS"       , sValue) && !sValue.IsEmpty() )
	{
		wxString	Path;

		if( (wxGetEnv("SAGA_TLB", &Path) || wxGetEnv("SAGA_MLB", &Path)) && !Path.IsEmpty() )
		{
			sValue	+= ";" + Path;
		}

		wxSetEnv("SAGA_TLB", sValue);
	}

	//-----------------------------------------------------
	if( Config_Read(pConfig,  "DATA", "GRID_CACHE_TMPDIR"   , sValue) )	{	SG_Grid_Cache_Set_Directory   (sValue);	}
	if( Config_Read(pConfig,  "DATA", "GRID_CACHE_MODE"     , iValue) )	{	SG_Grid_Cache_Set_Mode        (iValue);	}
	if( Config_Read(pConfig,  "DATA", "GRID_CACHE_THRESHLOD", dValue) )	{	SG_Grid_Cache_Set_Threshold_MB(dValue);	}

	if( Config_Read(pConfig,  "DATA", "GRID_COORD_PRECISION", iValue) )	{	CSG_Grid_System::Set_Precision(iValue);	}

	if( Config_Read(pConfig,  "DATA", "HISTORY_DEPTH"       , iValue) )	{	SG_Set_History_Depth       (iValue     );	}
	if( Config_Read(pConfig,  "DATA", "HISTORY_LISTS"       , iValue) )	{	SG_Set_History_Ignore_Lists(iValue != 0);	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxConfigBase *	Config_Default(bool bCreate)
{
#if defined(_SAGA_MSW)
	wxFileName	fLocal(SG_UI_Get_Application_Path(false).c_str()); fLocal.SetExt("ini");

	if( !fLocal.FileExists() && !bCreate )
	{
		return( NULL );
	}

	if( ( fLocal.FileExists() && (!fLocal.IsFileReadable() || !fLocal.IsFileWritable()))
	||  (!fLocal.FileExists() && (!fLocal.IsDirReadable () || !fLocal.IsDirWritable ())) )
	{
		wxFileName	fUser (wxGetHomeDir(), "saga_cmd", "ini");
	//	wxFileName	fUser (wxStandardPaths::Get().GetUserConfigDir(), "saga_cmd", "ini");

		if(	fLocal.FileExists() && fLocal.IsFileReadable() && !fUser.FileExists() )	// create a copy in user's home directory
		{
			wxFileInputStream	is(fLocal.GetFullPath());
			wxFileOutputStream	os(fUser .GetFullPath());
			wxFileConfig		ic(is);	ic.Save(os);
		}

		fLocal	= fUser;
	}

	if( (fLocal.FileExists() && fLocal.IsFileWritable()) || (!fLocal.FileExists() && fLocal.IsDirWritable()) )
	{
		return( new wxFileConfig(wxEmptyString, wxEmptyString, fLocal.GetFullPath(), fLocal.GetFullPath(), wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE|wxCONFIG_USE_RELATIVE_PATH) );
	}
#endif

	return( new wxConfig );	// this might go to registry
}

//---------------------------------------------------------
bool	Config_Create	(const CSG_String &File)
{
	wxConfigBase *pConfig;

	if( File.is_Empty() )
	{
		SG_UI_Console_Print_StdOut(CSG_String::Format("\n%s...", _TL("creating default configuration")), '\0');

		pConfig = Config_Default(true);
	}
	else
	{
		SG_UI_Console_Print_StdOut(CSG_String::Format("\n%s\n>>%s\n...", _TL("creating default configuration"), File.c_str()), '\0');

		pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, File.c_str(), File.c_str(), wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE|wxCONFIG_USE_RELATIVE_PATH);
	}

	Config_Create(pConfig);

	delete(pConfig);

	SG_UI_Console_Print_StdOut(CSG_String::Format("%s", _TL("ready")));

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Config_Load		(const CSG_String &File)
{
	wxConfigBase *pConfig = NULL;

	if( File.is_Empty() )
	{
		pConfig = Config_Default(false);
	}
	else if( SG_File_Exists(File) )
	{
		pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, File.c_str(), File.c_str(), wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE|wxCONFIG_USE_RELATIVE_PATH);
	}

	if( pConfig )
	{
		Config_Load(pConfig);

		delete(pConfig);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Config_Libraries	(CSG_Strings &Libraries, const CSG_String &File)
{
	wxConfigBase *pConfig = NULL;

	if( File.is_Empty() )
	{
		pConfig = Config_Default(false);
	}
	else if( SG_File_Exists(File) )
	{
		pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, File.c_str(), File.c_str(), wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE|wxCONFIG_USE_RELATIVE_PATH);
	}

	if( pConfig == NULL )
	{
		return( false );
	}

	if( pConfig->HasGroup("/LIBS") == false )
	{
		delete(pConfig);

		return( false );
	}

	//--------------------------------------------------------
	// group is there, return true and fill the list of
	// libraries/tool-chains to be loaded (might be empty!)

	pConfig->SetPath("/LIBS");

	long Index; wxString Entry;

	if( pConfig->GetFirstEntry(Entry, Index) )
	{
		#ifdef _SAGA_MSW
			wxString Default_Path(SG_UI_Get_Application_Path(true).c_str());
		#else
			wxString Default_Path(TOOLS_PATH);
		#endif

		Libraries.Clear();

		//-----------------------------------------------------
		do
		{
			wxString Library = pConfig->Read(Entry);

			if( !wxFileExists(Library) )
			{
				wxFileName FileName(Library);

				FileName.MakeAbsolute(Default_Path);

				Library	= FileName.GetFullPath();
			}

			if( wxFileExists(Library) )
			{
				Libraries += &Library;
			}
		}
		while( pConfig->GetNextEntry(Entry, Index) );
	}

	delete(pConfig);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
