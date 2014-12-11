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
#include <wx/tokenzr.h>

#include "helper.h"

#include "res_commands.h"

#include "wksp_module_manager.h"
#include "wksp_module_library.h"
#include "wksp_module_menu.h"
#include "wksp_module.h"


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
	CSG_MetaData	User;

	if( !g_pModules->Get_Parameters()->Get_Parameter("TOOL_MENUS") || !User.Load(g_pModules->Get_Parameters()->Get_Parameter("TOOL_MENUS")->asString()) || !User.Cmp_Name("saga_gui_tool_menus") || SG_Compare_Version(User.Get_Property("saga-version"), "2.2.0") < 0 )
	{
		User.Destroy();
	}

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
				for(int iModule=0; iModule<pLibrary->Get_Count(); iModule++, ID_Menu++)
				{
					pLibrary->Get_Module(iModule)->Set_Menu_ID(ID_Menu);

					if( User.Get_Children_Count() <= 0 || pUser )	// ignore if have user defined menus but no entries defined for this library...
					{
						_Get_SubMenu(pLibrary->Get_Module(iModule), pUser);
					}
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
bool CWKSP_Menu_Modules::_Get_SubMenu(CWKSP_Module *pModule, CSG_MetaData *pUser)
{
	//-----------------------------------------------------
	wxString	Menu	= pModule->Get_Module()->Get_MenuPath(true).c_str();
	wxString	Name	= pModule->Get_Name();

	if( pUser )
	{
		bool	bFound	= false;

		for(int i=0; i<pUser->Get_Children_Count() && !bFound; i++)
		{
			if( pUser->Get_Child(i)->Cmp_Property("id_or_name", pModule->Get_Module()->Get_ID  ())
			||  pUser->Get_Child(i)->Cmp_Property("id_or_name", pModule->Get_Module()->Get_Name()) )
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
		//	pMenu->Append(      ID_CMD_MODULES_FIRST, SubMenu, pSubMenu);
		}

		pMenu	= pSubMenu;
		SubMenu	= Tk.GetNextToken();
	}

	//-----------------------------------------------------
	size_t	iPos;

	for(iPos=0; iPos<pMenu->GetMenuItemCount(); iPos++)
	{
	#if defined(MODULES_MENU_SORT_SIMPLE)
		if( pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(Name) > 0 )
	#else
		if(	pMenu->FindItemByPosition(iPos)->IsSubMenu() == false
		&&	pMenu->FindItemByPosition(iPos)->GetItemLabelText().Cmp(Name) > 0 )
	#endif
		{
			break;
		}
	}

	pMenu->InsertCheckItem(iPos, pModule->Get_Menu_ID(), Name, Name);
//	pMenu->AppendCheckItem(      pModule->Get_Menu_ID(), Name, Name);

	return( true );
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
