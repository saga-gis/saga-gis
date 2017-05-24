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
//                 wksp_tool_library.cpp                 //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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

#include "wksp_tool_control.h"
#include "wksp_tool_manager.h"
#include "wksp_tool_library.h"
#include "wksp_tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Tool_Library::CWKSP_Tool_Library(CSG_Tool_Library *pLibrary)
{
	m_pLibrary	= pLibrary;

	_Add_Tools();
}

//---------------------------------------------------------
CWKSP_Tool_Library::~CWKSP_Tool_Library(void)
{
	_Del_Tools();

	if( MDI_Get_Frame() )	// don't unload library, if gui is closing (i.e. main window == NULL)
	{
		SG_Get_Tool_Library_Manager().Del_Library(m_pLibrary);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool_Library::Update(void)
{
	int		i;

	for(i=Get_Count()-1; i>=0; i--)	// first remove tools that are not available anymore
	{
		CWKSP_Tool	*pItem	= Get_Tool(i);

		if( !m_pLibrary->Get_Tool(pItem->Get_Tool()->Get_Name()) )
		{
			Del_Item(pItem);

			Get_Control()->Delete(pItem->GetId());
		}
	}

	for(i=0; i<m_pLibrary->Get_Count(); i++)	// then add tools that are not yet present in the list
	{
		CSG_Tool	*pTool	= m_pLibrary->Get_Tool(i);

		if( pTool != NULL && pTool != TLB_INTERFACE_SKIP_TOOL )
		{
			for(int j=0; j<Get_Count() && pTool; j++)
			{
				if( pTool == Get_Tool(j)->Get_Tool() )
				{
					Get_Control()->SetItemText(Get_Tool(j)->GetId(), Get_Tool(j)->Get_Name());	// just in case name has changed

					pTool	= NULL;
				}
			}

			if( pTool )
			{
				CWKSP_Tool	*pItem	= new CWKSP_Tool(pTool, m_pLibrary->Get_Menu().w_str());

				Add_Item(pItem);

				g_pTool_Ctrl->Add_Tool(GetId(), pItem);
			}
		}
	}

	Get_Control()->SortChildren(GetId());
}

//---------------------------------------------------------
void CWKSP_Tool_Library::_Add_Tools(void)
{
	for(int i=0; i<m_pLibrary->Get_Count(); i++)
	{
		CSG_Tool	*pTool	= m_pLibrary->Get_Tool(i);

		if( pTool != NULL && pTool != TLB_INTERFACE_SKIP_TOOL )
		{
			CWKSP_Tool	*pItem	= new CWKSP_Tool(pTool, m_pLibrary->Get_Menu().w_str());

			Add_Item(pItem);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Tool_Library::_Del_Tools(void)
{
	for(int i=Get_Count()-1; i>=0; i--)
	{
		CWKSP_Tool	*pItem	= Get_Tool(i);

		Del_Item(pItem);

	//	Get_Control()->Delete(pItem->GetId());
	//	delete(pItem);
	}
}



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Tool_Library::Get_Name(void)
{
	return( m_pLibrary->Get_Name().c_str() );
}

//---------------------------------------------------------
wxString CWKSP_Tool_Library::Get_Description(void)
{
	if( g_pTools->Get_Parameter("HELP_SOURCE")->asInt() == 1 )
	{
		wxString	Description	= Get_Online_Tool_Description(m_pLibrary->Get_File_Name().c_str());

		if( !Description.IsEmpty() )
		{
			return( Description );
		}
	}

	return( m_pLibrary->Get_Summary(SG_SUMMARY_FMT_HTML).c_str() );
}

//---------------------------------------------------------
wxMenu * CWKSP_Tool_Library::Get_Menu(void)
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
bool CWKSP_Tool_Library::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_CLOSE:
		SG_Get_Tool_Library_Manager().Del_Library(m_pLibrary);
		break;

	case ID_CMD_WKSP_ITEM_RETURN:
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Tool_Library::On_Command_UI(wxUpdateUIEvent &event)
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
CWKSP_Tool * CWKSP_Tool_Library::Get_Tool(CWKSP_Tool *pTool)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pTool	== Get_Tool(i) )
		{
			return( pTool );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Tool * CWKSP_Tool_Library::Get_Tool_byID(int CMD_ID)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Tool(i)->Get_Menu_ID() == CMD_ID )
		{
			return( Get_Tool(i) );
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
bool CWKSP_Tool_Library::is_Valid(void)
{
	return( m_pLibrary->is_Valid() );
}

//---------------------------------------------------------
wxString CWKSP_Tool_Library::Get_File_Name(void)
{
	return( m_pLibrary->Get_File_Name().c_str() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
