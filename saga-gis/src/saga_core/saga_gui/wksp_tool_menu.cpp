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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	m_Recent	= (CWKSP_Tool **)SG_Calloc(RECENT_COUNT, sizeof(CWKSP_Tool *));

	m_pMenu		= new wxMenu;
}

//---------------------------------------------------------
CWKSP_Tool_Menu::~CWKSP_Tool_Menu(void)
{
	SG_Free(m_Recent);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool_Menu::Update(void)
{
	//-----------------------------------------------------
	CSG_MetaData	User;

	if( !g_pTools->Get_Parameters()->Get_Parameter("TOOL_MENUS") || !User.Load(g_pTools->Get_Parameters()->Get_Parameter("TOOL_MENUS")->asString()) || !User.Cmp_Name("saga_gui_tool_menus") || SG_Compare_Version(User.Get_Property("saga-version"), "2.2.0") < 0 )
	{
		User.Destroy();
	}

	//-----------------------------------------------------
	wxMenuBar	*pMenuBar	= m_pMenu->GetMenuBar();

	if( pMenuBar )
	{
		pMenuBar->Replace(1, new wxMenu, _TL("Geoprocessing"));
	}

	delete(m_pMenu);	m_pMenu	= new wxMenu;

	//-----------------------------------------------------
	if( g_pTools->Get_Count() > 0 )
	{
		for(int iGroup=0, ID_Menu=ID_CMD_TOOL_MENU_START; iGroup<g_pTools->Get_Count(); iGroup++)
		{
			for(int iLibrary=0; iLibrary<g_pTools->Get_Group(iGroup)->Get_Count(); iLibrary++)
			{
				CWKSP_Tool_Library	*pLibrary	= g_pTools->Get_Group(iGroup)->Get_Library(iLibrary);

				//-----------------------------------------
				CSG_MetaData	*pUser	= NULL;

				if( User.Get_Children_Count() > 0 )
				{
					for(int i=0; i<User.Get_Children_Count() && !pUser; i++)
					{
						if( User[i].Cmp_Property("name", pLibrary->Get_Library()->Get_Library_Name()) )
						{
							pUser	= User(i);
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
	//-----------------------------------------------------
	wxString	Menu	= pTool->Get_Tool()->Get_MenuPath(true).c_str();
	wxString	Name	= pTool->Get_Name();

	if( pUser )
	{
		bool	bFound	= false;

		for(int i=0; i<pUser->Get_Children_Count() && !bFound; i++)
		{
			if( pUser->Get_Child(i)->Cmp_Property("id_or_name", pTool->Get_Tool()->Get_ID  ())
			||  pUser->Get_Child(i)->Cmp_Property("id_or_name", pTool->Get_Tool()->Get_Name()) )
			{
				Menu	= pUser->Get_Child(i)->Get_Content().c_str();

				if( pUser->Get_Child(i)->Get_Property("name") )
				{
					Name	= pUser->Get_Child(i)->Get_Property("name");
				}

				bFound	= true;
			}
		}

		if( !bFound )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	wxStringTokenizer	Tk(Menu, SG_T("|"));

	wxString	SubMenu(Tk.GetNextToken());

	wxMenu	*pMenu	= m_pMenu;

	while( !SubMenu.IsNull() )
	{
		wxMenu	*pSubMenu	= _Get_SubMenu_byToken(pMenu, SubMenu);

		if( !pSubMenu )
		{
			pSubMenu	= new wxMenu();

			size_t	iPos;

			for(iPos=0; iPos<pMenu->GetMenuItemCount(); iPos++)
			{
			#if defined(TOOLS_MENU_SORT_SIMPLE)
				if( pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(SubMenu) > 0 )
			#else
				if(	pMenu->FindItemByPosition(iPos)->IsSubMenu() == false
				||	pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(SubMenu) > 0 )
			#endif
				{
					break;
				}
			}

			pMenu->Insert(iPos, ID_CMD_TOOL_FIRST, SubMenu, pSubMenu);
		//	pMenu->Append(      ID_CMD_TOOL_FIRST, SubMenu, pSubMenu);
		}

		pMenu	= pSubMenu;
		SubMenu	= Tk.GetNextToken();
	}

	//-----------------------------------------------------
	size_t	iPos;

	for(iPos=0; iPos<pMenu->GetMenuItemCount(); iPos++)
	{
	#if defined(TOOLS_MENU_SORT_SIMPLE)
		if( pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(Name) > 0 )
	#else
		if(	pMenu->FindItemByPosition(iPos)->IsSubMenu() == false
		&&	pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(Name) > 0 )
	#endif
		{
			break;
		}
	}

	pMenu->InsertCheckItem(iPos, pTool->Get_Menu_ID(), Name, Name);
//	pMenu->AppendCheckItem(      pTool->Get_Menu_ID(), Name, Name);

	return( true );
}

//---------------------------------------------------------
wxMenu * CWKSP_Tool_Menu::_Get_SubMenu_byToken(wxMenu *pMenu, wxString Token)
{	
	for(size_t i=0; i<pMenu->GetMenuItemCount(); i++)
	{
		wxMenuItem	*pItem	= pMenu->GetMenuItems()[i];

		if( pItem->GetItemLabelText() == Token )
		{
			return( pItem->GetSubMenu() );
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
void CWKSP_Tool_Menu::Set_Recent(CWKSP_Tool *pTool)
{
	CWKSP_Tool	*pLast	= pTool;
	CWKSP_Tool	*pNext	= m_Recent[0];

	for(int i=0; i<RECENT_COUNT && pNext!=pTool; i++)
	{
		pNext		= m_Recent[i];
		m_Recent[i]	= pLast;
		pLast		= pNext;
	}

	Update();
}

//---------------------------------------------------------
void CWKSP_Tool_Menu::_Set_Recent(wxMenu *pMenu)
{
	bool	bRecent;
	int		i, j;

	//-----------------------------------------------------
	for(i=0, j=ID_CMD_TOOL_MENU_RECENT_FIRST, bRecent=false; i<RECENT_COUNT; i++, j++)
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
	for(i=0, j=0; j<RECENT_COUNT; j++)
	{
		if( m_Recent[j] )
		{
			m_Recent[i++]	= m_Recent[j];
		}
	}

	for(; i<RECENT_COUNT; i++)
	{
		m_Recent[i]	= NULL;
	}
}

//---------------------------------------------------------
int CWKSP_Tool_Menu::Get_ID_Translated(int ID)
{
	int		i	= ID - ID_CMD_TOOL_MENU_RECENT_FIRST;

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
