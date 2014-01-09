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
#include <wx/filename.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "helper.h"

#include "wksp_module_manager.h"
#include "wksp_module_library.h"
#include "wksp_module.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module_Library::CWKSP_Module_Library(CSG_Module_Library *pLibrary)
{
	m_pLibrary	= pLibrary;

	for(int i=0; i<m_pLibrary->Get_Count(); i++)
	{
		CSG_Module	*pModule	= m_pLibrary->Get_Module(i);

		if( pModule != NULL && pModule != MLB_INTERFACE_SKIP_MODULE )
		{
			Add_Item(new CWKSP_Module(pModule, m_pLibrary->Get_Menu().w_str()));
		}
	}
}

//---------------------------------------------------------
CWKSP_Module_Library::~CWKSP_Module_Library(void)
{
	for(int i=Get_Count()-1; i>=0; i--)
	{
		CWKSP_Module	*pItem	= Get_Module(i);
		Del_Item(pItem);
		delete(pItem);
	}

	SG_Get_Module_Library_Manager().Del_Library(m_pLibrary);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Module_Library::Get_Name(void)
{
	return( m_pLibrary->Get_Name().c_str() );
}

//---------------------------------------------------------
wxString CWKSP_Module_Library::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format(wxT("<b>%s</b>"), _TL("Module Library"));

	s	+= wxT("<table border=\"0\">");

	DESC_ADD_STR(_TL("Name")	, m_pLibrary->Get_Name     ().c_str());
	DESC_ADD_STR(_TL("Author")	, m_pLibrary->Get_Author   ().c_str());
	DESC_ADD_STR(_TL("Version")	, m_pLibrary->Get_Version  ().c_str());
	DESC_ADD_STR(_TL("File")	, m_pLibrary->Get_File_Name().c_str());

	s	+= wxT("</table><hr>");

	//-----------------------------------------------------
	s	+= wxString::Format(wxT("<b>%s</b><br>"), _TL("Description"));

	wxString	sDesc;

	if( g_pModules->Get_Parameter("HELP_SOURCE")->asInt() == 1 )
	{
		sDesc	= Get_Online_Module_Description(m_pLibrary->Get_File_Name().c_str());
	}

	s	+= sDesc.Length() > 0 ? sDesc.c_str() : m_pLibrary->Get_Description().c_str();

	//-----------------------------------------------------
	s	+= wxString::Format(wxT("<hr><b>%s:<ul>"), _TL("Modules"));

	for(int iModule=0; iModule<Get_Count(); iModule++)
	{
		s	+= wxString::Format(wxT("<li>[%d] %s</li>"),
				Get_Module(iModule)->Get_Module()->Get_ID(),
				Get_Module(iModule)->Get_Module()->Get_Name().w_str()
			);
	}

	s	+= wxT("</ul>");

	//-----------------------------------------------------
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
CWKSP_Module * CWKSP_Module_Library::Get_Module(CWKSP_Module *pModule)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pModule	== Get_Module(i) )
		{
			return( pModule );
		}
	}

	return( NULL );
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
	return( m_pLibrary->is_Valid() );
}

//---------------------------------------------------------
wxString CWKSP_Module_Library::Get_File_Name(void)
{
	return( m_pLibrary->Get_File_Name().c_str() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
