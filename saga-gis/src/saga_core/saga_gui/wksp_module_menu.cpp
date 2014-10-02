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
CWKSP_Menu_Modules::CWKSP_Menu_Modules(void)
{
	m_Recent	= (CWKSP_Module **)SG_Calloc(RECENT_COUNT, sizeof(CWKSP_Module *));

	m_pMenu		= new wxMenu;
}

//---------------------------------------------------------
CWKSP_Menu_Modules::~CWKSP_Menu_Modules(void)
{
	SG_Free(m_Recent);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Menu_Modules::Update(void)
{
	//-----------------------------------------------------
	int		ID_Menu;

	while( (ID_Menu = m_pMenu->GetMenuItemCount()) > 0 )
	{
		m_pMenu->Destroy(m_pMenu->GetMenuItems().Item(ID_Menu - 1)->GetData());
	}

	//-----------------------------------------------------
	if( g_pModules->Get_Count() > 0 )
	{
		ID_Menu	= ID_CMD_MODULE_START;

		for(int iGroup=0; iGroup<g_pModules->Get_Count(); iGroup++)
		{
			for(int iLibrary=0; iLibrary<g_pModules->Get_Group(iGroup)->Get_Count(); iLibrary++)
			{
				CWKSP_Module_Library	*pLibrary	= g_pModules->Get_Group(iGroup)->Get_Library(iLibrary);

				for(int iModule=0; iModule<pLibrary->Get_Count(); iModule++, ID_Menu++)
				{
					CWKSP_Module	*pModule	= pLibrary->Get_Module(iModule);
					wxMenu			*pSubMenu	= _Get_SubMenu(m_pMenu, pModule->Get_Menu_Path());

					pModule->Set_Menu_ID(ID_Menu);

					size_t	iPos;

					for(iPos=0; iPos<pSubMenu->GetMenuItemCount(); iPos++)
					{
					#if defined(MODULES_MENU_SORT_SIMPLE)
						if( pSubMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(pModule->Get_Name()) > 0 )
					#else
						if(	pSubMenu->FindItemByPosition(iPos)->IsSubMenu() == false
						&&	pSubMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(pModule->Get_Name()) > 0 )
					#endif
						{
							break;
						}
					}

					pSubMenu->InsertCheckItem(iPos, ID_Menu, pModule->Get_Name(), pModule->Get_Name());
				//	pSubMenu->AppendCheckItem(ID_Menu, pModule->Get_Name(), pModule->Get_Name());
				}
			}
		}

		m_pMenu->InsertSeparator(0);
		CMD_Menu_Ins_Item(m_pMenu, false, ID_CMD_MODULES_OPEN	, 0);
		CMD_Menu_Ins_Item(m_pMenu, false, ID_CMD_MODULES_SEARCH	, 1);

		_Set_Recent(m_pMenu);
	}
	else
	{
		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_MODULES_OPEN);
	}
}

//---------------------------------------------------------
wxMenu * CWKSP_Menu_Modules::_Get_SubMenu(wxMenu *pMenu, wxString Menu_Path)
{
	wxStringTokenizer	Tk(Menu_Path, SG_T("|"));

	wxString	SubMenu(Tk.GetNextToken());

	while( !SubMenu.IsNull() )
	{
		wxMenu	*pSubMenu	= _Get_SubMenu_byToken(pMenu, SubMenu);

		if( !pSubMenu )
		{
			pSubMenu	= new wxMenu();

			size_t	iPos;

			for(iPos=0; iPos<pMenu->GetMenuItemCount(); iPos++)
			{
			#if defined(MODULES_MENU_SORT_SIMPLE)
				if( pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(SubMenu) > 0 )
			#else
				if(	pMenu->FindItemByPosition(iPos)->IsSubMenu() == false
				||	pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(SubMenu) > 0 )
			#endif
				{
					break;
				}
			}

			pMenu->Insert(iPos, ID_CMD_MODULES_FIRST, SubMenu, pSubMenu);
		//	pMenu->Append(ID_CMD_MODULES_FIRST, SubMenu, pSubMenu);
		}

		pMenu	= pSubMenu;
		SubMenu	= Tk.GetNextToken();
	}

	return( pMenu );
}

//---------------------------------------------------------
wxMenu * CWKSP_Menu_Modules::_Get_SubMenu_byToken(wxMenu *pMenu, wxString Token)
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
void CWKSP_Menu_Modules::Set_Recent(CWKSP_Module *pModule)
{
	CWKSP_Module	*pLast	= pModule;
	CWKSP_Module	*pNext	= m_Recent[0];

	for(int i=0; i<RECENT_COUNT && pNext!=pModule; i++)
	{
		pNext		= m_Recent[i];
		m_Recent[i]	= pLast;
		pLast		= pNext;
	}

	Update();
}

//---------------------------------------------------------
void CWKSP_Menu_Modules::_Set_Recent(wxMenu *pMenu)
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
int CWKSP_Menu_Modules::Get_ID_Translated(int ID)
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
