
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
#include <wx/image.h>
#include <wx/splash.h>

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
const char *	SAGA_GUI_Get_Version(void)
{
	return( "SAGA Graphical User Interface - Version: " SAGA_GUI_VERSION );
}

//---------------------------------------------------------
const char *	SAGA_GUI_Get_Build(void)
{
	return( "20060825" );
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

	m_Process_bContinue	= true;

	wxInitAllImageHandlers();

	_Init_Config();

	//-----------------------------------------------------
	long			iLogo;
	wxSplashScreen	*pLogo;

	iLogo	= CONFIG_Read("/DATA", "START_LOGO", iLogo) ? iLogo : 2;

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
		pLogo	= new wxSplashScreen(IMG_Get_Bitmap(ID_IMG_SAGA_SPLASH), wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT, 30000, NULL, -1);
		break;
	}

	wxYield();

	//-----------------------------------------------------
	SG_Get_Translator()->Create(SG_File_Make_Path(wxGetCwd(), "saga", "lng"), false);

	SetTopWindow(new CSAGA_Frame());

	//-----------------------------------------------------
	if( pLogo && iLogo == 1 )
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

	//-----------------------------------------------------
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
	long			l;
	wxString		s;
	wxConfigBase	*pConfig;

	//-----------------------------------------------------
	SetVendorName("SAGA-GIS.org");

	#if   defined(__VISUALC__) && defined(_DEBUG)
		SetAppName(CSG_String::Format("SAGA VC%d [Debug]", __VISUALC__).c_str());
	#elif defined(__VISUALC__)
		SetAppName(CSG_String::Format("SAGA VC%d"        , __VISUALC__).c_str());
	#elif defined(__MINGW32__)
		SetAppName("SAGA MinGW");
	#else
		SetAppName("SAGA");
	#endif

	pConfig	= wxConfigBase::Get();

	//-----------------------------------------------------
	if( !CONFIG_Read("Version", "Build", s) || s.Cmp(SAGA_GUI_Get_Build()) )
	{
		pConfig->SetPath("/");

		while( pConfig->GetFirstGroup(s, l) )
		{
			pConfig->DeleteGroup(s);
		}

		pConfig->Flush();

		CONFIG_Write("Version", "Build", SAGA_GUI_Get_Build());
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CSAGA::Get_App_Path(void)
{
	return( SG_File_Get_Path(argv[0]).c_str() );
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
bool CSAGA::Process_Wait(void)
{
	while( Pending() )
	{
		Dispatch();
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
	static bool	bYield	= false;

	if( !bYield )
	{
		bYield	= true;

	//	Yield();
	//	wxSafeYield(g_pSAGA_Frame);
		Process_Wait();

		bYield	= false;
	}

	return( m_Process_bContinue );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
