
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
//                  wksp_tool_menu.cpp                   //
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
#include <wx/tokenzr.h>

#include "helper.h"

#include "res_commands.h"

#include "saga_frame.h"

#include "wksp_tool_manager.h"
#include "wksp_tool_library.h"
#include "wksp_tool_menu.h"
#include "wksp_tool.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Tool_Menu::CWKSP_Tool_Menu(void)
{
	m_Recent = (CWKSP_Tool **)SG_Calloc(MAX_COUNT_RECENT_TOOLS, sizeof(CWKSP_Tool *));

	m_pMenu  = new wxMenu;
}

//---------------------------------------------------------
CWKSP_Tool_Menu::~CWKSP_Tool_Menu(void)
{
	SG_Free(m_Recent);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool_Menu::Update(void)
{
	wxMenuBar *pMenuBar = m_pMenu->GetMenuBar();

	if( pMenuBar )
	{
		pMenuBar->Freeze();
	}

	while( m_pMenu->GetMenuItemCount() )
	{
		m_pMenu->Destroy(m_pMenu->GetMenuItems()[0]);
	}

	m_Recent_Start = -1;

	//-----------------------------------------------------
	if( g_pTools->Get_Count() < 1 )
	{
		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_TOOL_OPEN  );
		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_TOOL_RELOAD);
	}

	//-----------------------------------------------------
	else
	{
		CSG_MetaData User;

		if( !g_pTools->Get_Parameters()->Get_Parameter("TOOL_MENUS") || !User.Load(g_pTools->Get_Parameters()->Get_Parameter("TOOL_MENUS")->asString()) || !User.Cmp_Name("saga_gui_tool_menus") || SG_Compare_Version(User.Get_Property("saga-version"), "2.2.0") < 0 )
		{
			User.Destroy();
		}

		//-------------------------------------------------
		for(int iGroup=0, ID_Menu=ID_CMD_TOOL_MENU_FIRST; iGroup<g_pTools->Get_Count(); iGroup++)
		{
			for(int iLibrary=0; iLibrary<g_pTools->Get_Group(iGroup)->Get_Count(); iLibrary++)
			{
				CWKSP_Tool_Library *pLibrary = g_pTools->Get_Group(iGroup)->Get_Library(iLibrary);

				//-----------------------------------------
				CSG_MetaData *pUser = NULL;

				if( User.Get_Children_Count() > 0 )
				{
					for(int i=0; i<User.Get_Children_Count() && !pUser; i++)
					{
						if( User[i].Cmp_Property("name", pLibrary->Get_Library()->Get_Library_Name()) )
						{
							pUser = User(i);
						}
					}
				}

				//-----------------------------------------
				for(int iTool=0; iTool<pLibrary->Get_Count(); iTool++, ID_Menu++)
				{
					pLibrary->Get_Tool(iTool)->Set_Menu_ID(ID_Menu);

					if( User.Get_Children_Count() <= 0 || pUser ) // ignore if have user defined menus but no entries defined for this library...
					{
						_Get_SubMenu(pLibrary->Get_Tool(iTool), pUser);
					}
				}
			}
		}

		m_pMenu->InsertSeparator(0);

		CMD_Menu_Ins_Item(m_pMenu, false, ID_CMD_TOOL_SEARCH, 0);

		m_Recent_Start = m_pMenu->GetMenuItemCount();

		_Set_Recent();
	}

	//-----------------------------------------------------
	if( pMenuBar )
	{
		pMenuBar->Thaw();
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Tool_Menu::_Get_SubMenu(CWKSP_Tool *pTool, CSG_MetaData *pUser)
{
	CSG_Strings Menu; wxString Tool = pTool->Get_Name();

	if( pUser )
	{
		for(int i=0; i<pUser->Get_Children_Count() && !Menu.Get_Count(); i++)
		{
			if( pUser->Get_Child(i)->Cmp_Property("id_or_name", pTool->Get_Tool()->Get_ID  ())
			||  pUser->Get_Child(i)->Cmp_Property("id_or_name", pTool->Get_Tool()->Get_Name()) )
			{
				Menu += pUser->Get_Child(i)->Get_Content();

				if( pUser->Get_Child(i)->Get_Property("name") )
				{
					Tool = pUser->Get_Child(i)->Get_Property("name");
				}
			}
		}

		if( !Menu.Get_Count() )
		{
			return( false );
		}
	}
	else
	{
		Menu = SG_String_Tokenize(pTool->Get_Tool()->Get_MenuPath(true), ";");
	}

	//-----------------------------------------------------
	for(int i=0; i<Menu.Get_Count(); i++)
	{
		wxMenu *pMenu = m_pMenu;

		CSG_Strings SubMenus = SG_String_Tokenize(Menu[i].c_str(), "|");

		for(int i=0; i<SubMenus.Get_Count(); i++)
		{
			CSG_String SubMenu(SG_Translate(SubMenus[i])); wxMenu *pSubMenu = _Get_SubMenu(pMenu, SubMenu.c_str());

			if( !pSubMenu )
			{
				pSubMenu = new wxMenu();

				size_t iPos;

				for(iPos=0; iPos<pMenu->GetMenuItemCount(); iPos++)
				{
				#if defined(TOOLS_MENU_SORT_SIMPLE)
					if( pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(SubMenu) > 0 )
				#else
					if(	pMenu->FindItemByPosition(iPos)->IsSubMenu() == false
					||	pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(SubMenu.c_str()) > 0 )
				#endif
					{
						break;
					}
				}

				pMenu->Insert(iPos, ID_CMD_TOOL_FIRST, SubMenu.c_str(), pSubMenu);
			//	pMenu->Append(      ID_CMD_TOOL_FIRST, SubMenu.c_str(), pSubMenu);
			}

			pMenu = pSubMenu;
		}

		//-----------------------------------------------------
		size_t iPos;

		for(iPos=0; iPos<pMenu->GetMenuItemCount(); iPos++)
		{
		#if defined(TOOLS_MENU_SORT_SIMPLE)
			if( pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(Tool) > 0 )
		#else
			if(	pMenu->FindItemByPosition(iPos)->IsSubMenu() == false
			&&	pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(Tool) > 0 )
		#endif
			{
				break;
			}
		}

		pMenu->InsertCheckItem(iPos, pTool->Get_Menu_ID(), Tool, Tool);
	//	pMenu->AppendCheckItem(      pTool->Get_Menu_ID(), Tool, Tool);
	}

	return( true );
}

//---------------------------------------------------------
wxMenu * CWKSP_Tool_Menu::_Get_SubMenu(wxMenu *pMenu, const wxString &Name)
{	
	for(size_t i=0; i<pMenu->GetMenuItemCount(); i++)
	{
		wxMenuItem *pItem = pMenu->GetMenuItems()[i];

		if( pItem->GetItemLabelText() == Name )
		{
			return( pItem->GetSubMenu() );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Tool_Menu::Get_ID_Translated(int ID)
{
	int i = ID - ID_CMD_TOOL_RECENT_FIRST;

	if( i >= 0 && i < MAX_COUNT_RECENT_TOOLS && m_Recent[i] != NULL )
	{
		return( m_Recent[i]->Get_Menu_ID() );
	}

	return( ID );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool_Menu::Add_Recent(CWKSP_Tool *pTool)
{
	if( pTool )
	{
		CWKSP_Tool *pLast = pTool, *pNext = m_Recent[0];

		for(int i=0; i<MAX_COUNT_RECENT_TOOLS && pNext!=pTool; i++)
		{
			pNext = m_Recent[i]; m_Recent[i] = pLast; pLast = pNext;
		}

		_Set_Recent();
	}
}

//---------------------------------------------------------
bool CWKSP_Tool_Menu::_Set_Recent(void)
{
	int Listing = g_pTools->Get_Parameter("RECENT_LIST")->asInt();

	if( Listing == 2 ) // do not list
	{
		return( false );
	}

	//-----------------------------------------------------
	int nRecents = 0;

	for(int i=0; m_Recent[i] && i<MAX_COUNT_RECENT_TOOLS; i++)
	{
		if( m_Recent[i] && g_pTools->Exists(m_Recent[i]) )
		{
			m_Recent[nRecents++] = m_Recent[i];
		}
		else
		{
			m_Recent[i] = NULL;
		}
	}

	//-----------------------------------------------------
	if( Listing == 0 ) // append to top-level menu
	{
		for(int i=m_pMenu->GetMenuItemCount()-1; i>=m_Recent_Start; i--)
		{
			m_pMenu->Destroy(m_pMenu->GetMenuItems()[i]);
		}

		if( nRecents > 0 )
		{
			m_pMenu->AppendSeparator();
		}

		for(int i=0, id=ID_CMD_TOOL_RECENT_FIRST; i<nRecents; i++, id++)
		{
			m_pMenu->AppendCheckItem(id, m_Recent[i]->Get_Name(), m_Recent[i]->Get_Name());
		}
	}

	//-----------------------------------------------------
	else // if( Listing == 1 ) // list in sub menu
	{
		wxMenu *pMenu = m_pMenu->GetMenuItemCount() > 1 && m_pMenu->GetMenuItems()[1]->IsSubMenu() ? m_pMenu->GetMenuItems()[1]->GetSubMenu() : NULL;
		
		if( !pMenu || m_pMenu->GetMenuItems()[1]->GetName().Cmp(_TL("Recent Tools")) )
		{
			m_pMenu->Insert(1, wxID_ANY, _TL("Recent Tools"), pMenu = new wxMenu);
		}
		else while( pMenu->GetMenuItemCount() )
		{
			pMenu->Destroy(pMenu->GetMenuItems()[0]);
		}

		m_pMenu->GetMenuItems()[1]->Enable(nRecents > 0);

		for(int i=0, id=ID_CMD_TOOL_RECENT_FIRST; i<nRecents; i++, id++)
		{
			pMenu->AppendCheckItem(id, m_Recent[i]->Get_Name(), m_Recent[i]->Get_Name());
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Tool_Menu::Load_Recent(void)
{
	int i = 0; wxString Tool;

	for(int i=0, j=0; i<=MAX_COUNT_RECENT_TOOLS; i++)
	{
		if( CONFIG_Read("RECENT_TOOLS", wxString::Format("TOOL_%02d", i), Tool) )
		{
			if( (m_Recent[j] = g_pTools->Get_Tool(Tool.BeforeFirst('|'), Tool.AfterFirst('|'))) != NULL )
			{
				j++;
			}
		}
	}

	return( _Set_Recent() );
}

//---------------------------------------------------------
bool CWKSP_Tool_Menu::Save_Recent(void)
{
	CONFIG_Delete("RECENT_TOOLS");

	for(int i=0, j=0; i<=MAX_COUNT_RECENT_TOOLS; i++)
	{
		if( m_Recent[i] && g_pTools->Exists(m_Recent[i]) )
		{
			CSG_Tool *pTool = m_Recent[i]->Get_Tool();

			CONFIG_Write("RECENT_TOOLS", wxString::Format("TOOL_%02d", j++), wxString::Format("%s|%s", pTool->Get_Library().c_str(), pTool->Get_ID().c_str()));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
