
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
#define SAGA_GUI_VERSION		wxT("2.0.6")
#define SAGA_GUI_BUILD			wxT("20100719")

//---------------------------------------------------------
const wxChar *	SAGA_GUI_Get_Version(void)
{
	return( SAGA_GUI_VERSION );
}

//---------------------------------------------------------
const wxChar *	SAGA_GUI_Get_Build(void)
{
	return( SAGA_GUI_BUILD );
}


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

	SetVendorName		(wxT("SAGA-GIS.org"));
	SetAppName			(wxT("SAGA"));

	wxInitAllImageHandlers();

	m_App_Path			= wxFileName(argv[0]).GetPath();

	_Init_Config();

	//-----------------------------------------------------
	long			lValue;

	m_Process_bContinue	= true;
	m_Process_Frequency	= CONFIG_Read(wxT("/MODULES"), wxT("PROC_FREQ"), lValue) ? lValue : 0;

	//-----------------------------------------------------
	long			iLogo;
	wxSplashScreen	*pLogo;

	iLogo	= CONFIG_Read(wxT("/MODULES"), wxT("START_LOGO"), iLogo) ? iLogo : 1;

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

	if( wxGetEnv(wxT("PATH"), &Path) && Path.Length() > 0 )
	{
		wxSetEnv(wxT("PATH"), wxString::Format(wxT("%s;%s\\dll"), Path.c_str(), Get_App_Path().c_str()));
	}
	else
	{
		wxSetEnv(wxT("PATH"), wxString::Format(wxT("%s\\dll"), Get_App_Path().c_str()));
	}
#endif // defined(_SAGA_MSW)

	//-----------------------------------------------------
	wxString	File;

	//-----------------------------------------------------
	if( !CONFIG_Read(wxT("/MODULES"), wxT("LNG_FILE_DIC"), File) || !SG_File_Exists(File) )
	{
		File	= SG_File_Make_Path(Get_App_Path(), wxT("saga"), wxT("lng")).c_str();
	}

	SG_Get_Translator().Create(File.c_str(), false);

	//-----------------------------------------------------
	if( !CONFIG_Read(wxT("/MODULES"), wxT("CRS_FILE_DIC"), File) || !SG_File_Exists(File) )
	{
		File	= SG_File_Make_Path(Get_App_Path(), wxT("saga_prj"), wxT("dic")).c_str();
	}

	SG_Get_Projections().Load_Dictionary(File.c_str());

	//-----------------------------------------------------
	if( !CONFIG_Read(wxT("/MODULES"), wxT("CRS_FILE_SRS"), File) || !SG_File_Exists(File) )
	{
		File	= SG_File_Make_Path(Get_App_Path(), wxT("saga_prj"), wxT("srs")).c_str();
	}

	SG_Get_Projections().Load_DB(File.c_str());

	//-----------------------------------------------------
	SetTopWindow(new CSAGA_Frame());

	//-----------------------------------------------------
	if( argc > 1 )
	{
		for(int i=1; i<argc; i++)
		{
			g_pWKSP->Open(argv[i]);
		}
	}

	//-----------------------------------------------------
	if( pLogo && iLogo == 1 )
	{
		pLogo->Destroy();

		wxYield();
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
void CSAGA::_Init_Config(void)
{
	wxConfigBase	*pConfig;

#if defined(_SAGA_MSW)
	wxFileName	fConfig(Get_App_Path(), wxT("saga_gui"), wxT("ini"));

	if(	( fConfig.FileExists() && (!fConfig.IsFileReadable() || !fConfig.IsFileWritable()))
	||	(!fConfig.FileExists() && (!fConfig.IsDirReadable () || !fConfig.IsDirWritable ())) )
	{
		fConfig.Assign(wxGetHomeDir(), wxT("saga_gui"), wxT("ini"));
		//fConfig.Assign(wxFileName::GetTempDir(), wxT("saga_gui"), wxT("ini"));
	}

	pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, fConfig.GetFullPath(), fConfig.GetFullPath(), wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE|wxCONFIG_USE_RELATIVE_PATH);
#else
	pConfig	= new wxConfig;
#endif

	wxConfigBase::Set(pConfig);

	//-----------------------------------------------------
	wxString	s;

	if( !CONFIG_Read(wxT("Version"), wxT("Build"), s) || s.Cmp(SAGA_GUI_Get_Build()) )
	{
		long	l;

		pConfig->SetPath(wxT("/"));

		while( pConfig->GetFirstGroup(s, l) )
		{
			pConfig->DeleteGroup(s);
		}

		pConfig->Flush();

		CONFIG_Write(wxT("Version"), wxT("Build"), SAGA_GUI_Get_Build());
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

	if( !bYield && (bEnforce || m_Process_Frequency <= 0 || (wxDateTime::Now() - tYield).GetMilliseconds() > m_Process_Frequency) )
	{
		bYield	= true;

		//	Yield();
		//	wxSafeYield(g_pSAGA_Frame);

		while( Pending() )
		{
			Dispatch();
		}

		bYield	= false;
		tYield	= wxDateTime::UNow();
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
