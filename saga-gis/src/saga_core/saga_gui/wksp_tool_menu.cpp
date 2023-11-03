
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

#include "wksp_tool_manager.h"
#include "wksp_tool_library.h"
#include "wksp_tool_menu.h"
#include "wksp_tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Tool_Menu::CWKSP_Tool_Menu(void)
{
	m_Recent = (CWKSP_Tool **)SG_Calloc(RECENT_COUNT, sizeof(CWKSP_Tool *));

	m_pMenu  = new wxMenu;
}

//---------------------------------------------------------
CWKSP_Tool_Menu::~CWKSP_Tool_Menu(void)
{
	SG_Free(m_Recent);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool_Menu::Update(void)
{
	CSG_MetaData User;

	if( !g_pTools->Get_Parameters()->Get_Parameter("TOOL_MENUS") || !User.Load(g_pTools->Get_Parameters()->Get_Parameter("TOOL_MENUS")->asString()) || !User.Cmp_Name("saga_gui_tool_menus") || SG_Compare_Version(User.Get_Property("saga-version"), "2.2.0") < 0 )
	{
		User.Destroy();
	}

	//-----------------------------------------------------
	wxMenuBar *pMenuBar = m_pMenu->GetMenuBar();

	if( pMenuBar )
	{
		pMenuBar->Replace(1, new wxMenu, _TL("Geoprocessing"));
	}

	delete(m_pMenu); m_pMenu = new wxMenu;

	//-----------------------------------------------------
	if( g_pTools->Get_Count() > 0 )
	{
		for(int iGroup=0, ID_Menu=ID_CMD_TOOL_MENU_START; iGroup<g_pTools->Get_Count(); iGroup++)
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

					if( User.Get_Children_Count() <= 0 || pUser )	// ignore if have user defined menus but no entries defined for this library...
					{
						_Get_SubMenu(pLibrary->Get_Tool(iTool), pUser);
					}
				}
			}
		}

		m_pMenu->InsertSeparator(0);

		CMD_Menu_Ins_Item(m_pMenu, false, ID_CMD_TOOL_OPEN  , 0);
		CMD_Menu_Ins_Item(m_pMenu, false, ID_CMD_TOOL_SEARCH, 1);

		_Set_Recent(m_pMenu);
	}
	else
	{
		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_TOOL_OPEN);
	}

	if( pMenuBar )
	{
		delete(pMenuBar->Replace(1, m_pMenu, _TL("Geoprocessing")));
	}
}

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
			CSG_String SubMenu(SG_Translate(SubMenus[i]));
			wxMenu *pSubMenu = _Get_SubMenu(pMenu, SubMenu.c_str());

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
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool_Menu::Set_Recent(CWKSP_Tool *pTool)
{
	CWKSP_Tool *pLast = pTool, *pNext = m_Recent[0];

	for(int i=0; i<RECENT_COUNT && pNext!=pTool; i++)
	{
		pNext = m_Recent[i]; m_Recent[i] = pLast; pLast = pNext;
	}

	Update();
}

//---------------------------------------------------------
void CWKSP_Tool_Menu::_Set_Recent(wxMenu *pMenu)
{
	bool bRecent = false;

	for(int i=0, j=ID_CMD_TOOL_MENU_RECENT_FIRST; i<RECENT_COUNT; i++, j++)
	{
		if( m_Recent[i] && g_pTools->Exists(m_Recent[i]) )
		{
			if( !bRecent )
			{
				bRecent	= true;

				pMenu->AppendSeparator();
			}

			pMenu->AppendCheckItem(j, m_Recent[i]->Get_Name(), m_Recent[i]->Get_Name());
		}
		else
		{
			m_Recent[i]	= NULL;
		}
	}

	//-----------------------------------------------------
	int i = 0;

	for(int j=0; j<RECENT_COUNT; j++)
	{
		if( m_Recent[j] )
		{
			m_Recent[i++] = m_Recent[j];
		}
	}

	for(; i<RECENT_COUNT; i++)
	{
		m_Recent[i] = NULL;
	}
}

//---------------------------------------------------------
int CWKSP_Tool_Menu::Get_ID_Translated(int ID)
{
	int i = ID - ID_CMD_TOOL_MENU_RECENT_FIRST;

	if( i >= 0 && i < RECENT_COUNT && m_Recent[i] != NULL )
	{
		return( m_Recent[i]->Get_Menu_ID() );
	}

	return( ID );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
