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
//                WKSP_Module_Library.cpp                //
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
#include <wx/filename.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "helper.h"

#include "wksp_module_library.h"
#include "wksp_module.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if defined(_SAGA_MSW)
	#define ENV_LIB_PATH	wxT("PATH")
	#define ENV_LIB_SEPA	wxT(';')
#elif defined(_SAGA_LINUX)
	#define ENV_LIB_PATH	wxT("LD_LIBRARY_PATH")
	#define ENV_LIB_SEPA	wxT(':')
#else
	#define ENV_LIB_PATH	wxT("PATH")
	#define ENV_LIB_SEPA	wxT(';')
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module_Library::CWKSP_Module_Library(const wxChar *FileName)
{
	TSG_PFNC_MLB_Initialize		MLB_Initialize;
	TSG_PFNC_MLB_Get_Interface	MLB_Get_Interface;

	wxString	sPath;
	wxFileName	fName(FileName);
	CSG_Module	*pModule;

	m_pInterface	= NULL;

	fName.MakeAbsolute();
	m_File_Name		= fName.GetFullPath();

	//-----------------------------------------------------
	if( wxGetEnv(ENV_LIB_PATH, &sPath) && sPath.Length() > 0 )
	{
		wxSetEnv(ENV_LIB_PATH, wxString::Format(wxT("%s%c%s"), sPath.c_str(), ENV_LIB_SEPA, SG_File_Get_Path(FileName).c_str()));
	}
	else
	{
		wxSetEnv(ENV_LIB_PATH, SG_File_Get_Path(FileName).c_str());
	}

	//-----------------------------------------------------
	if( !m_Library.Load(m_File_Name) )
	{
		MSG_Error_Add(wxString::Format(wxT("%s:\n%s"), FileName, LNG("[ERR] Library could not be loaded")), true);
	}
	else
	{
		if( (MLB_Get_Interface	= (TSG_PFNC_MLB_Get_Interface)	m_Library.GetSymbol(SYMBOL_MLB_Get_Interface)) == NULL
		||	(MLB_Initialize		= (TSG_PFNC_MLB_Initialize)		m_Library.GetSymbol(SYMBOL_MLB_Initialize)   ) == NULL
		||	!(m_pInterface = MLB_Get_Interface()) || !MLB_Initialize(m_File_Name) )
		{
			MSG_Error_Add(wxString::Format(wxT("%s:\n%s"), FileName, LNG("[ERR] Invalid library")), true);
		}
		else
		{
			while( (pModule = m_pInterface->Get_Module(Get_Count())) != NULL )
			{
				Add_Item(new CWKSP_Module(pModule, Get_Info(MLB_INFO_Menu_Path)));
			}

			if( Get_Count() == 0 )
			{
				MSG_Error_Add(wxString::Format(wxT("%s:\n%s"), FileName, LNG("[ERR] Library does not contain any modules")), true);
			}
		}
	}

	//-----------------------------------------------------
	if( sPath.Length() > 0 )
	{
		wxSetEnv(ENV_LIB_PATH, sPath);
	}
	else
	{
		wxUnsetEnv(ENV_LIB_PATH);
	}
}

//---------------------------------------------------------
CWKSP_Module_Library::~CWKSP_Module_Library(void)
{
	TSG_PFNC_MLB_Finalize	MLB_Finalize;
	CWKSP_Module			*pItem;

	if( m_Library.IsLoaded() )
	{
		if( is_Valid() )
		{
			MSG_General_Add(wxString::Format(wxT("%s: %s..."), LNG("[MSG] Close Library"), m_File_Name.c_str()), true, true);

			if(	(MLB_Finalize = (TSG_PFNC_MLB_Finalize)m_Library.GetSymbol(SYMBOL_MLB_Finalize)) != NULL )
			{
				MLB_Finalize();
			}

			while( (pItem = Get_Module(0)) != NULL )
			{
				Del_Item(pItem);
				delete(pItem);
			}

			m_Library.Unload();

			MSG_General_Add(LNG("[MSG] okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);
		}
		else
		{
			m_Library.Unload();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Module_Library::Get_Name(void)
{
	return( Get_Info(MLB_INFO_Name) );
}

//---------------------------------------------------------
wxString CWKSP_Module_Library::Get_Description(void)
{
	int			iModule;
	wxString	s, sLine;

	s.Printf(wxT("%s: <b>%s</b><br>%s: <i>%s</i><br>%s: <i>%s</i><br>%s: <i>%s</i><hr>%s"),
		LNG("[CAP] Module Library")	, Get_Info(MLB_INFO_Name),
		LNG("[CAP] Author")			, Get_Info(MLB_INFO_Author),
		LNG("[CAP] Version")		, Get_Info(MLB_INFO_Version),
		LNG("[CAP] File")			, Get_File_Name().c_str(),
		Get_Info(MLB_INFO_Description)
	);

	s.Append(wxString::Format(wxT("<hr><b>%s:<ul>"), LNG("[CAP] Modules")));

	for(iModule=0; iModule<Get_Count(); iModule++)
	{
		sLine.Printf(wxT("<li>%s</li>"), Get_Module(iModule)->Get_Module()->Get_Name());

		s.Append(sLine);
	}

	s.Append(wxT("</ul>"));

	s.Replace(wxT("\n"), wxT("<br>"));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Module_Library::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module_Library::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Module_Library::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Manager::On_Command_UI(event) );

	case ID_CMD_WKSP_ITEM_CLOSE:
		event.Enable(true);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module_Library::Exists(CWKSP_Module *pModule)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pModule	== Get_Module(i) )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
CWKSP_Module * CWKSP_Module_Library::Get_Module_byID(int CMD_ID)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Module(i)->Get_Menu_ID() == CMD_ID )
		{
			return( Get_Module(i) );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module_Library::is_Valid(void)
{
	return( Get_Count() > 0 );
}

//---------------------------------------------------------
wxString & CWKSP_Module_Library::Get_File_Name(void)
{
	return( m_File_Name );
}

//---------------------------------------------------------
const wxChar * CWKSP_Module_Library::Get_Info(int Type)
{
	if( m_pInterface != NULL )
	{
		return( m_pInterface->Get_Info(Type) );
	}

	return( wxT("") );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
