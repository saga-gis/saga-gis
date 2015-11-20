/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      SAGA.cpp                         //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/image.h>
#include <wx/splash.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include <saga_api/saga_api.h>

#include "helper.h"

#include "res_images.h"

#include "saga.h"
#include "saga_frame.h"

#include "wksp.h"
#include "wksp_data_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSAGA	*g_pSAGA	= NULL;

//---------------------------------------------------------
IMPLEMENT_APP(CSAGA)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSAGA, wxApp)
	EVT_KEY_DOWN		(CSAGA::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SAGA_GUI_BUILD	"20140214"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSAGA::CSAGA(void)
{}

//---------------------------------------------------------
CSAGA::~CSAGA(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSAGA::OnInit(void)
{
	//-----------------------------------------------------
	g_pSAGA				= this;

	SetVendorName		("SAGA-GIS.org");
	SetAppName			("SAGA");

	wxInitAllImageHandlers();

	wxFileName			App_Path(argv[0]);	App_Path.MakeAbsolute();

	m_App_Path			= App_Path.GetPath();

#if !defined(_DEBUG)
	wxSetAssertHandler(NULL);		// disable all wx asserts in SAGA release builds
#endif

	/* workaround: wxwidgets 2.9.3 is complaining about setlocale
	 * mismatch between c setlocale and wxLocale. since saga has its own
	 * translation system, we use english as default. this assures
	 * using . as decimal separator in printf like formatting of
	 * floating point values. wxXLocale is currently not fully
	 * implemented (wxPrintf_l and similar still missing). */
	//setlocale(LC_NUMERIC, "C");
	m_wxLocale.Init(wxLANGUAGE_ENGLISH);


	_Init_Config();

	//-----------------------------------------------------
	long			lValue;

	m_Process_bContinue	= true;
	m_Process_Frequency	= CONFIG_Read("/MODULES", "PROCESS_UPDATE", lValue) ? lValue : 0;

	//-----------------------------------------------------
	wxSplashScreen	*pLogo;

	long	iLogo		= CONFIG_Read("/MODULES", "START_LOGO"    , iLogo ) ? iLogo : 1;

	switch( iLogo )
	{
	default:
		pLogo	= NULL;
		break;

	case 1:
	case 3:
		pLogo	= new wxSplashScreen(IMG_Get_Bitmap(ID_IMG_SAGA_SPLASH), wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT, 0, NULL, -1);
		break;

	case 2:
		pLogo	= new wxSplashScreen(IMG_Get_Bitmap(ID_IMG_SAGA_SPLASH), wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT, 20000, NULL, -1);
		break;
	}

	wxYield();

	//-----------------------------------------------------
#if defined(_SAGA_MSW)
	wxString	Path;

	if( wxGetEnv("PATH", &Path) && Path.Length() > 0 )
	{
		wxSetEnv("PATH", Get_App_Path() + "\\dll;" + Path);
	}
	else
	{
		wxSetEnv("PATH", Get_App_Path() + "\\dll");
	}

	wxSetEnv("GDAL_DRIVER_PATH", Get_App_Path() + "\\dll");
	wxSetEnv("GDAL_DATA"       , Get_App_Path() + "\\dll\\gdal-data");
#endif // defined(_SAGA_MSW)

	//-----------------------------------------------------
	wxString	File;

	//-----------------------------------------------------
	if( !CONFIG_Read("/MODULES", "LNG_FILE_DIC", File) || !wxFileExists(File) )
	{
		File	= wxFileName(Get_App_Path(), "saga", "lng").GetFullPath();
	}

	SG_Get_Translator().Create(&File, false);

	//-----------------------------------------------------
	long oldstyle; if( CONFIG_Read("/MODULES", "LNG_OLDSTYLE", oldstyle) && oldstyle ) SG_Set_OldStyle_Naming();
	//-----------------------------------------------------

	//-----------------------------------------------------
	if( !CONFIG_Read("/MODULES", "CRS_FILE_DIC", File) || !wxFileExists(File) )
	{
#if defined(_SAGA_LINUX)
		File	= wxFileName(SHARE_PATH, "saga_prj", "dic").GetFullPath();
#endif
#if defined(_SAGA_MSW)
		File	= wxFileName(Get_App_Path(), "saga_prj", "dic").GetFullPath();
#endif
	}

	SG_Get_Projections().Load_Dictionary(&File);

	//-----------------------------------------------------
	if( !CONFIG_Read("/MODULES", "CRS_FILE_SRS", File) || !wxFileExists(File) )
	{
#if defined(_SAGA_LINUX)
		File	= wxFileName(SHARE_PATH, "saga_prj", "srs").GetFullPath();
#endif
#if defined(_SAGA_MSW)
		File	= wxFileName(Get_App_Path(), "saga_prj", "srs").GetFullPath();
#endif
	}

	SG_Get_Projections().Load_DB(&File);

	//-----------------------------------------------------
	SetTopWindow(new CSAGA_Frame());

	//-----------------------------------------------------
	if( pLogo && (iLogo == 1 || argc > 1) )
	{
		pLogo->Destroy();

		wxYield();
	}

	//-----------------------------------------------------
	if( argc > 1 )
	{
		for(int i=1; i<argc; i++)
		{
			g_pWKSP->Open(argv[i]);
		}
	}
	else
	{
		g_pData->Initialise();
	}

	//-----------------------------------------------------
	g_pSAGA_Frame->Show_Tips(false);

	return( true );
}

//---------------------------------------------------------
int CSAGA::OnExit(void)
{
	delete(wxConfigBase::Set((wxConfigBase *)NULL));

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wfstream.h>

//---------------------------------------------------------
void CSAGA::_Init_Config(void)
{
	wxConfigBase	*pConfig;

#if defined(_SAGA_MSW)
	wxFileName	fLocal(Get_App_Path(), "saga_gui", "ini");

	if( ( fLocal.FileExists() && (!fLocal.IsFileReadable() || !fLocal.IsFileWritable()))
	||  (!fLocal.FileExists() && (!fLocal.IsDirReadable () || !fLocal.IsDirWritable ())) )
	{
		wxFileName	fUser (wxGetHomeDir(), "saga_gui", "ini");
	//	wxFileName	fUser (wxStandardPaths::Get().GetUserConfigDir(), "saga_gui", "ini");

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
		pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, fLocal.GetFullPath(), fLocal.GetFullPath(), wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE|wxCONFIG_USE_RELATIVE_PATH);
	}
	else
	{
		pConfig	= new wxConfig;	// this might go to registry
	}
#else
	pConfig	= new wxConfig;
#endif

	wxConfigBase::Set(pConfig);

	//-----------------------------------------------------
	wxString	s;

	if( !CONFIG_Read("Version", "Build", s) || s.Cmp(SAGA_GUI_BUILD) )
	{
		long	l;

		pConfig->SetPath("/");

		while( pConfig->GetFirstGroup(s, l) )
		{
			pConfig->DeleteGroup(s);
		}

		pConfig->Flush();

		CONFIG_Write("Version", "Build", SAGA_GUI_BUILD);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:
		event.Skip();
		break;

	case WXK_ESCAPE:
		m_Process_bContinue	= false;
		break;
	}
}

//---------------------------------------------------------
bool CSAGA::Process_Wait(bool bEnforce)
{
	static bool			bYield	= false;
	static wxDateTime	tYield	= wxDateTime::UNow();

	if( !bYield && (bEnforce || m_Process_Frequency <= 0 || m_Process_Frequency <= (wxDateTime::UNow() - tYield).GetMilliseconds()) )
	{
		bYield	= true;

	//	Yield();
	//	wxSafeYield(g_pSAGA_Frame);

		while( Pending() && Dispatch() );

		if( m_Process_Frequency > 0 )
		{
			tYield	= wxDateTime::UNow();
		}

		bYield	= false;
	}

	return( true );
}

//---------------------------------------------------------
bool CSAGA::Process_Set_Okay(bool bOkay)
{
	m_Process_bContinue	= bOkay;

	return( m_Process_bContinue );
}

//---------------------------------------------------------
bool CSAGA::Process_Get_Okay(void)
{
	Process_Wait();

	return( m_Process_bContinue );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
