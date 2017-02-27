
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
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
bool	Config_Create	(const CSG_String &File)
{
	wxString	_File	= File.c_str();

	if( File.is_Empty() )
	{
		wxFileName	fLocal(SG_UI_Get_Application_Path().c_str());
		
		fLocal.SetExt("ini");

		_File	= fLocal.GetFullPath();
	}

	SG_Printf(CSG_String::Format("\n%s...\n", _TL("writing configuration file")));

	wxConfigBase	*pConfig	= new wxFileConfig(wxEmptyString, wxEmptyString, _File, _File, wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE|wxCONFIG_USE_RELATIVE_PATH);

	//-----------------------------------------------------
	pConfig->Write("CMD_NO_PROGRESS"     , false);	// q, s: no progress report
	pConfig->Write("CMD_NO_MESSAGES"     , false);	// r, s: no messages report
	pConfig->Write("CMD_INTERACTIVE"     , false);	// i: allow user interaction
	pConfig->Write("CMD_XML_MESSAGE"     , false);	// x: message output as xml

	pConfig->Write("TRANSLATIONS"        , false);	// load translation dictionary
	pConfig->Write("PROJECTIONS"         , false);	// load projections dictionary

	pConfig->Write("CORES"               , SG_OMP_Get_Max_Num_Procs      ());	// number of cores
	pConfig->Write("HISTORY_DEPTH"       , SG_Get_History_Depth          ());	// history
	pConfig->Write("GRID_COORD_PRECISION", CSG_Grid_System::Get_Precision());	// grid system coordinate precision

	pConfig->Write("ADD_LIB_PATHS"       , ";"  );	// additional tool library paths (aka SAGA_MLB)

	pConfig->Write("OLDSTYLE"            , false);	// load old style naming, has no effect if l-flag is set.

	//-----------------------------------------------------
	delete(pConfig);

	return( true );
}

//---------------------------------------------------------
bool	Config_Load		(wxConfigBase *pConfig)
{
#if   defined(_SAGA_LINUX)
	CSG_String	Path_Shared	= SHARE_PATH;
#elif defined(_SAGA_MSW)
	CSG_String	Path_Shared	= SG_File_Get_Path(SG_UI_Get_Application_Path());
#endif

	bool		bValue;
	long		lValue;
//	double		dValue;
	wxString	sValue;

	//-----------------------------------------------------
	pConfig->Read("CMD_NO_PROGRESS", &bValue, false); CMD_Set_Show_Progress(bValue == false);	// q, s: no progress report
	pConfig->Read("CMD_NO_MESSAGES", &bValue, false); CMD_Set_Show_Messages(bValue == false);	// r, s: no messages report
	pConfig->Read("CMD_INTERACTIVE", &bValue, false); CMD_Set_Interactive  (bValue ==  true);	// i: allow user interaction
	pConfig->Read("CMD_XML_MESSAGE", &bValue, false); CMD_Set_XML          (bValue ==  true);	// x: message output as xml

	//-----------------------------------------------------
	if( pConfig->Read("TRANSLATIONS", &bValue) && bValue == true )	// load translation dictionary
	{
		SG_Printf(CSG_String::Format("\n%s:", _TL("loading translation dictionary")));
		SG_Printf(CSG_String::Format("\n%s.\n",
			SG_Get_Translator().Create(SG_File_Make_Path(Path_Shared, SG_T("saga"), SG_T("lng")), false)
			? _TL("success") : _TL("failed")
		));
	}

	if( pConfig->Read("PROJECTIONS", &bValue) && bValue == true )	// load projections dictionary
	{
		SG_Printf(CSG_String::Format("\n%s:", _TL("loading spatial reference system database")));
		SG_Printf(CSG_String::Format("\n%s.\n",
			SG_Get_Projections().Create(SG_File_Make_Path(Path_Shared, SG_T("saga_prj"), SG_T("srs")))
			? _TL("success") : _TL("failed")
		));
	}

	//-----------------------------------------------------
	if( pConfig->Read("CORES", &lValue) && lValue > 0 )	// number of cores
	{
		SG_OMP_Set_Max_Num_Threads(M_GET_MIN(lValue, SG_OMP_Get_Max_Num_Procs()));
	}

	if( pConfig->Read("HISTORY_DEPTH", &lValue) )	// history
	{
		SG_Set_History_Depth(lValue);
	}

	if( pConfig->Read("GRID_COORD_PRECISION", &lValue) && lValue >= 0 )	// grid system coordinate precision
	{
		CSG_Grid_System::Set_Precision((int)lValue);
	}

	//-----------------------------------------------------
	if( pConfig->Read("ADD_LIB_PATHS", &sValue) )	// additional tool library paths (aka SAGA_MLB)
	{
		wxSetEnv("SAGA_MLB", sValue);
	}

	//-----------------------------------------------------
	if( pConfig->Read("OLDSTYLE", &bValue) && bValue == true )	// load old style naming, has no effect if l-flag is set.
	{
		SG_Set_OldStyle_Naming();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxConfigBase *	Config_Default(void)
{
	wxConfigBase	*pConfig;

#if defined(_SAGA_MSW)
	wxFileName	fLocal(SG_UI_Get_Application_Path().c_str());

	fLocal.SetExt("ini");

	if( (fLocal.FileExists() && fLocal.IsFileWritable()) || (!fLocal.FileExists() && fLocal.IsDirWritable()) )
	{
		pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, fLocal.GetFullPath(), fLocal.GetFullPath(), wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE|wxCONFIG_USE_RELATIVE_PATH);
	}
	else
	{
		pConfig	= new wxConfig;	// this might go to registry
	}
#else
	pConfig	= new wxConfig;
#endif

	return(pConfig);
}

//---------------------------------------------------------
bool	Config_Load		(void)
{
	wxConfigBase	*pConfig	= Config_Default();

	bool	bResult	= Config_Load(pConfig);

	delete(pConfig);

	return( bResult );
}

//---------------------------------------------------------
bool	Config_Load		(const CSG_String &File)
{
	wxConfigBase	*pConfig	= new wxFileConfig(wxEmptyString, wxEmptyString, File.c_str(), File.c_str(), wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE|wxCONFIG_USE_RELATIVE_PATH);

	bool	bResult	= Config_Load(pConfig);

	delete(pConfig);

	return( bResult );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
