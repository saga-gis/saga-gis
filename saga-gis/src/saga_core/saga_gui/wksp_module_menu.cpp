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
//                 WKSP_Module_Menu.cpp                  //
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
#include "res_commands.h"

#include "wksp_module_manager.h"
#include "wksp_module_library.h"
#include "wksp_module_menu.h"
#include "wksp_module.h"
#include <wx/tokenzr.h>

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module_Menu::CWKSP_Module_Menu(void)
{
	m_Recent	= (CWKSP_Module **)SG_Calloc(RECENT_COUNT, sizeof(CWKSP_Module *));

	m_Menus		= NULL;
	m_nMenus	= 0;
}

//---------------------------------------------------------
CWKSP_Module_Menu::~CWKSP_Module_Menu(void)
{
	SG_Free(m_Recent);

	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module_Menu::Destroy(void)
{
	if( m_Menus )
	{
		SG_Free(m_Menus);
		m_Menus		= NULL;
		m_nMenus	= 0;
	}
}

//---------------------------------------------------------
void CWKSP_Module_Menu::Add(wxMenu *pMenu)
{
	_Update(pMenu);

	m_Menus	= (wxMenu **)SG_Realloc(m_Menus, (m_nMenus + 1) * sizeof(wxMenu *));
	m_Menus[m_nMenus++]	= pMenu;
}

//---------------------------------------------------------
void CWKSP_Module_Menu::Del(wxMenu *pMenu)
{
	for(int i=0; i<m_nMenus; i++)
	{
		if( m_Menus[i] == pMenu )
		{
			m_nMenus--;

			for( ; i<m_nMenus; i++)
			{
				m_Menus[i]	= m_Menus[i + 1];
			}

			m_Menus	= (wxMenu **)SG_Realloc(m_Menus, m_nMenus * sizeof(wxMenu *));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module_Menu::Update(void)
{
	for(int i=0; i<m_nMenus; i++)
	{
		_Update(m_Menus[i]);
	}
}

//---------------------------------------------------------
void CWKSP_Module_Menu::_Update(wxMenu *pMenu)
{
	int						iLibrary, iModule, ID_Menu;
	wxMenu					*pSubMenu;
	CWKSP_Module_Library	*pLibrary;
	CWKSP_Module			*pModule;

	//-----------------------------------------------------
	while( (ID_Menu = pMenu->GetMenuItemCount()) > 0 )
	{
		pMenu->Destroy(pMenu->GetMenuItems().Item(ID_Menu - 1)->GetData());
	}

	//-----------------------------------------------------
	if( g_pModules->Get_Count() > 0 )
	{
		for(iLibrary=0, ID_Menu=ID_CMD_MODULE_START; iLibrary<g_pModules->Get_Count(); iLibrary++)
		{
			pLibrary	= g_pModules->Get_Library(iLibrary);

			for(iModule=0; iModule<pLibrary->Get_Count(); iModule++, ID_Menu++)
			{
				pModule		= pLibrary->Get_Module(iModule);
				pModule		->Set_Menu_ID(ID_Menu);
				pSubMenu	= _Get_SubMenu(pMenu, pModule->Get_Menu_Path());

				size_t	iPos;

				for(iPos=0; iPos<pSubMenu->GetMenuItemCount(); iPos++)
				{
#if defined(MODULES_MENU_SORT_SIMPLE)
					if( pSubMenu->FindItemByPosition(iPos)->GetLabel().Cmp(pModule->Get_Name()) > 0 )
#else
					if(	pSubMenu->FindItemByPosition(iPos)->IsSubMenu() == false
					&&	pSubMenu->FindItemByPosition(iPos)->GetLabel().Cmp(pModule->Get_Name()) > 0 )
#endif
						break;
				}

				pSubMenu->InsertCheckItem(iPos, ID_Menu, pModule->Get_Name(), pModule->Get_Name());
			//	pSubMenu->AppendCheckItem(ID_Menu, pModule->Get_Name(), pModule->Get_Name());
			}
		}

		pMenu->InsertSeparator(0);
		CMD_Menu_Ins_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE, 0);
		CMD_Menu_Ins_Item(pMenu, false, ID_CMD_MODULES_OPEN	, 0);

		_Set_Recent(pMenu);
	}
	else
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MODULES_OPEN);
	}
}

//---------------------------------------------------------
wxMenu * CWKSP_Module_Menu::_Get_SubMenu(wxMenu *pMenu, wxString Menu_Path)
{
	wxStringTokenizer	*tk;
	wxString			sSubMenu;
	wxMenu				*pSubMenu;

	tk			= new wxStringTokenizer(Menu_Path, SG_T("|"));
	sSubMenu	= tk->GetNextToken();

	while( ! sSubMenu.IsNull() )
	{
		pSubMenu	= _Find_SubMenu_For_Token(pMenu, sSubMenu);

		if( ! pSubMenu )
		{
			pSubMenu	= new wxMenu();

			size_t	iPos;

			for(iPos=0; iPos<pMenu->GetMenuItemCount(); iPos++)
			{
#if defined(MODULES_MENU_SORT_SIMPLE)
				if( pMenu->FindItemByPosition(iPos)->GetLabel().Cmp(sSubMenu) > 0 )
#else
				if(	pMenu->FindItemByPosition(iPos)->IsSubMenu() == false
				||	pMenu->FindItemByPosition(iPos)->GetLabel().Cmp(sSubMenu) > 0 )
#endif
					break;
			}

			pMenu->Insert(iPos, ID_CMD_MODULES_FIRST, sSubMenu, pSubMenu);
		//	pMenu->Append(ID_CMD_MODULES_FIRST, sSubMenu, pSubMenu);
		}

		pMenu		= pSubMenu;
		sSubMenu	= tk->GetNextToken();
	}

	delete tk;

	return pMenu;
}

//---------------------------------------------------------
wxMenu * CWKSP_Module_Menu::_Find_SubMenu_For_Token( wxMenu* menu, wxString token ) {
	
	wxMenuItem* item;
	wxMenu* result = NULL;
	
	for( size_t i = 0; i < menu->GetMenuItemCount(); i++ ) {
		item = menu->GetMenuItems()[ i ];
		if ( item->GetLabel() == token ) {
			result = item->GetSubMenu();
			break;
		}
	}
	return result;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module_Menu::Set_Recent(CWKSP_Module *pModule)
{
	int				i;
	CWKSP_Module	*pNext, *pLast;

	//-----------------------------------------------------
	pLast	= pModule;
	pNext	= m_Recent[0];

	for(i=0; i<RECENT_COUNT && pNext!=pModule; i++)
	{
		pNext		= m_Recent[i];
		m_Recent[i]	= pLast;
		pLast		= pNext;
	}

	//-----------------------------------------------------
	for(i=0; i<m_nMenus; i++)
	{
		_Update(m_Menus[i]);
	}
}

//---------------------------------------------------------
void CWKSP_Module_Menu::_Set_Recent(wxMenu *pMenu)
{
	bool	bRecent;
	int		i, j;

	//-----------------------------------------------------
	for(i=0, j=ID_CMD_MODULE_RECENT_FIRST, bRecent=false; i<RECENT_COUNT; i++, j++)
	{
		if( m_Recent[i] && g_pModules->Exists(m_Recent[i]) )
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
int CWKSP_Module_Menu::Get_ID_Translated(int ID)
{
	int		i	= ID - ID_CMD_MODULE_RECENT_FIRST;

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
