	
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
//                WKSP_Base_Control.cpp                  //
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
#include <wx/window.h>
#include <wx/image.h>
#include <wx/imaglist.h>

#include <saga_api/saga_api.h>

#include "helper.h"

#include "res_commands.h"
#include "res_controls.h"
#include "res_dialogs.h"
#include "res_images.h"

#include "active.h"

#include "wksp.h"

#include "wksp_base_control.h"
#include "wksp_base_manager.h"

#include "wksp_module_control.h"
#include "wksp_module_manager.h"
#include "wksp_module_menu.h"

#include "wksp_data_manager.h"
#include "wksp_data_layers.h"
#include "wksp_table.h"

#include "wksp_map_manager.h"
#include "wksp_map.h"
#include "wksp_map_buttons.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	IMG_NO_ITEMS	= 0,
	IMG_ROOT
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CWKSP_Base_Control, wxTreeCtrl)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP_Base_Control, wxTreeCtrl)
	EVT_TREE_ITEM_RIGHT_CLICK	(ID_WND_WKSP_MODULES	, CWKSP_Base_Control::On_Item_RClick)
	EVT_TREE_DELETE_ITEM		(ID_WND_WKSP_MODULES	, CWKSP_Base_Control::On_Item_Delete)
	EVT_TREE_SEL_CHANGED		(ID_WND_WKSP_MODULES	, CWKSP_Base_Control::On_Item_SelChanged)
	EVT_TREE_KEY_DOWN			(ID_WND_WKSP_MODULES	, CWKSP_Base_Control::On_Item_KeyDown)

	EVT_TREE_ITEM_RIGHT_CLICK	(ID_WND_WKSP_DATA		, CWKSP_Base_Control::On_Item_RClick)
	EVT_TREE_DELETE_ITEM		(ID_WND_WKSP_DATA		, CWKSP_Base_Control::On_Item_Delete)
	EVT_TREE_SEL_CHANGED		(ID_WND_WKSP_DATA		, CWKSP_Base_Control::On_Item_SelChanged)
	EVT_TREE_KEY_DOWN			(ID_WND_WKSP_DATA		, CWKSP_Base_Control::On_Item_KeyDown)

	EVT_TREE_ITEM_RIGHT_CLICK	(ID_WND_WKSP_MAPS		, CWKSP_Base_Control::On_Item_RClick)
	EVT_TREE_DELETE_ITEM		(ID_WND_WKSP_MAPS		, CWKSP_Base_Control::On_Item_Delete)
	EVT_TREE_SEL_CHANGED		(ID_WND_WKSP_MAPS		, CWKSP_Base_Control::On_Item_SelChanged)
	EVT_TREE_KEY_DOWN			(ID_WND_WKSP_MAPS		, CWKSP_Base_Control::On_Item_KeyDown)

	EVT_LEFT_DOWN				(CWKSP_Base_Control::On_Item_LClick)
	EVT_LEFT_DCLICK				(CWKSP_Base_Control::On_Item_LDClick)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Base_Control::CWKSP_Base_Control(wxWindow *pParent, wxWindowID id)
	: wxTreeCtrl(pParent, id, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS)
{
	m_pManager	= NULL;

	AssignImageList(new wxImageList(IMG_SIZE_TREECTRL, IMG_SIZE_TREECTRL, true, 0));
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_NOITEMS);
}

//---------------------------------------------------------
CWKSP_Base_Control::~CWKSP_Base_Control(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Base_Control::_Set_Manager(CWKSP_Base_Manager *pManager)
{
	if( m_pManager == NULL )
	{
		m_pManager	= pManager;

		AddRoot		(m_pManager->Get_Name(), IMG_ROOT, IMG_ROOT, m_pManager);
		AppendItem	(m_pManager->GetId(), LNG("[CAP] [no items]"), IMG_NO_ITEMS, IMG_NO_ITEMS, NULL);
		Expand		(m_pManager->GetId());

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Base_Control::On_Command(wxCommandEvent &event)
{
	//-----------------------------------------------------
	if( event.GetId() == ID_CMD_WKSP_ITEM_CLOSE )
	{
		_Del_Active(false);

		return;
	}

	//-----------------------------------------------------
	if( event.GetId() == ID_CMD_WKSP_ITEM_SHOW )
	{
		_Show_Active();

		return;
	}

	//-----------------------------------------------------
	if( event.GetId() == ID_CMD_WKSP_ITEM_SETTINGS_LOAD && Get_Selection_Count() > 1 )
	{
		_Load_Settings();

		return;
	}

	//-----------------------------------------------------
	if( m_pManager->On_Command(event.GetId()) )
	{
		return;
	}

	//-----------------------------------------------------
	CWKSP_Base_Item	*pItem	= Get_Item_Selected();

	if( pItem )
	{
		pItem->On_Command(event.GetId());
	}
}

//---------------------------------------------------------
void CWKSP_Base_Control::On_Command_UI(wxUpdateUIEvent &event)
{
	//-----------------------------------------------------
	if( m_pManager->On_Command_UI(event) )
	{
		return;
	}

	//-----------------------------------------------------
	CWKSP_Base_Item	*pItem	= Get_Item_Selected();

	if( pItem )
	{
		pItem->On_Command_UI(event);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Base_Control::_Add_Item(CWKSP_Base_Item *pItem, int Image, int selImage, bool bSort)
{
	if( pItem != NULL && pItem->Get_Manager() == m_pManager )
	{
		if( m_pManager->Get_Count() == 1 )
		{
			DeleteChildren(m_pManager->GetId());
		}

		AppendItem(m_pManager->GetId(), pItem->Get_Name(), Image, selImage, pItem);

		if( bSort )
		{
			SortChildren(m_pManager->GetId());
		}

		Expand(m_pManager->GetId());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::_Del_Item(CWKSP_Base_Item *pItem, bool bSilent)
{
	//-----------------------------------------------------
	if( (	pItem == NULL	)
	||	(	pItem->Get_Type()	== WKSP_ITEM_Table
		&&	(	((CWKSP_Table *)pItem)->Get_Owner()->Get_Type() == WKSP_ITEM_Shapes
			||	((CWKSP_Table *)pItem)->Get_Owner()->Get_Type() == WKSP_ITEM_TIN	)	)	)
	{
		return( false );
	}

	//-----------------------------------------------------
	if( pItem == m_pManager )
	{
		if( m_pManager->Get_Count() > 0 && (bSilent || _Del_Item_Confirm(pItem)) )
		{
			Freeze();
			if( g_pData_Buttons )
			{
				g_pData_Buttons->Freeze();
			}

			DeleteChildren	(m_pManager->GetId());
			AppendItem		(m_pManager->GetId(), LNG("[CAP] [no items]"), 0, 0, NULL);
			Expand			(m_pManager->GetId());

			if( g_pModule_Ctrl && m_pManager->Get_Type() == WKSP_ITEM_Module_Manager )
			{
				g_pModules->Get_Modules_Menu()->Update();
			}

			Thaw();

			if( g_pData_Buttons )
			{
				g_pData_Buttons->Thaw();
				g_pData_Buttons->Update_Buttons();
			}

			if( g_pMap_Buttons )
			{
				g_pMap_Buttons->Update_Buttons();
			}

			return( true );
		}
	}

	//-----------------------------------------------------
	else
	{
		if( bSilent || _Del_Item_Confirm(pItem) )
		{
			CWKSP_Base_Manager	*pItem_Manager	= pItem->Get_Manager();

			Freeze();

			Delete(pItem->GetId());

			if( pItem_Manager != NULL && pItem_Manager->Get_Count() == 0 )
			{
				Thaw();

				if( m_pManager->Get_Type() == WKSP_ITEM_Data_Manager )
				{
					g_pData_Buttons->Update_Buttons();
				}

				if( m_pManager->Get_Type() == WKSP_ITEM_Map_Manager )
				{
					g_pMap_Buttons->Update_Buttons();
				}

				return( _Del_Item(pItem_Manager, true) );
			}

			if( m_pManager->Get_Type() == WKSP_ITEM_Module_Manager )
			{
				g_pModules->Get_Modules_Menu()->Update();
			}

			if( pItem_Manager != NULL && pItem_Manager->Get_Type() == WKSP_ITEM_Map )
			{
				((CWKSP_Map *)pItem_Manager)->View_Refresh(false);
			}

			Thaw();

			Refresh();

			if( m_pManager->Get_Type() == WKSP_ITEM_Data_Manager )
			{
				g_pData_Buttons->Update_Buttons();
			}

			if( m_pManager->Get_Type() == WKSP_ITEM_Map_Manager )
			{
				g_pMap_Buttons->Update_Buttons();
			}

			return( true );
		}
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::_Del_Item_Confirm(CWKSP_Base_Item *pItem)
{
	if( DLG_Message_Confirm(ID_DLG_DELETE) )
	{
		if( m_pManager->Get_Type() == WKSP_ITEM_Data_Manager )
		{
			return( g_pData->Save_Modified(pItem) );
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Base_Item * CWKSP_Base_Control::Get_Item_Selected(void)
{
	wxTreeItemId	ID;

	if( GetWindowStyle() & wxTR_MULTIPLE )
	{
		wxArrayTreeItemIds	IDs;

		if( GetSelections(IDs) == 1 )
		{
			ID	= IDs[0];
		}
	}
	else
	{
		ID	= GetSelection();
	}

	return( ID.IsOk() ? (CWKSP_Base_Item *)GetItemData(ID) : NULL );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::Set_Item_Selected(CWKSP_Base_Item *pItem, bool bKeepMultipleSelection)
{
	if( pItem && pItem->Get_Control() == this && pItem->GetId().IsOk() )
	{
		bool	bSelect	= bKeepMultipleSelection ? !IsSelected(pItem->GetId()) : true;

		if( !bKeepMultipleSelection )
		{
			UnselectAll();
		}

		SelectItem(pItem->GetId(), bSelect);

		Refresh();

		_Set_Active();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CWKSP_Base_Control::Get_Selection_Count(void)
{
	if( GetWindowStyle() & wxTR_MULTIPLE )
	{
		wxArrayTreeItemIds	IDs;

		return( GetSelections(IDs) );
	}
	else
	{
		wxTreeItemId	ID	= GetSelection();

		return( ID.IsOk() ? 1 : 0 );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_Base_Control::Get_Context_Menu(void)
{
	wxMenu			*pMenu	= NULL;
	CWKSP_Base_Item	*pItem	= Get_Item_Selected();

	if( pItem )
	{
		pMenu	= pItem->Get_Menu();
	}
	else if( GetWindowStyle() & wxTR_MULTIPLE )
	{
		wxArrayTreeItemIds	IDs;

		if( GetSelections(IDs) > 0 )
		{
			pMenu	= new wxMenu;

			CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SHOW);
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SETTINGS_LOAD);
		}
	}

	if( pMenu == NULL )
	{
		pMenu	= new wxMenu;

		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	}

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Base_Control::_Set_Active(void)
{
	if( g_pACTIVE )
	{
		g_pACTIVE->Set_Active(Get_Item_Selected());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::_Del_Active(bool bSilent)
{
	wxTreeItemId	ID;

	if( GetWindowStyle() & wxTR_MULTIPLE )
	{
		wxArrayTreeItemIds	IDs;

		if( GetSelections(IDs) > 0 && ((CWKSP_Base_Item *)GetItemData(IDs[0]))->Get_Control() == this )
		{
			if( DLG_Message_Confirm(ID_DLG_DELETE)
			&&	(m_pManager->Get_Type() != WKSP_ITEM_Data_Manager || g_pData->Save_Modified_Sel()) )
			{
				for(size_t i=0; i<IDs.GetCount(); i++)
				{
					if( IDs[i].IsOk() )
					{
						_Del_Item((CWKSP_Base_Item *)GetItemData(IDs[i]), true);
					}
				}
			}
		}
	}
	else
	{
		ID	= GetSelection();

		if( ID.IsOk() && ((CWKSP_Base_Item *)GetItemData(ID))->Get_Control() == this )
		{
			_Del_Item((CWKSP_Base_Item *)GetItemData(ID), bSilent);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::_Show_Active(void)
{
	wxArrayTreeItemIds	IDs;

	if( (GetWindowStyle() & wxTR_MULTIPLE) != 0 && GetSelections(IDs) > 0 && ((CWKSP_Base_Item *)GetItemData(IDs[0]))->Get_Control() == this )
	{
		int				iMap;
		size_t			iItem;
		CWKSP_Base_Item	*pItem;

		for(iItem=0, iMap=0; iItem<IDs.GetCount(); iItem++)
		{
			if( IDs[iItem].IsOk() )
			{
				pItem	= (CWKSP_Base_Item *)GetItemData(IDs[iItem]);

				switch( pItem->Get_Type() )
				{
				case WKSP_ITEM_Grid:
				case WKSP_ITEM_Shapes:
				case WKSP_ITEM_TIN:
					iMap	= 1;
					break;

				case WKSP_ITEM_Table:
					((CWKSP_Table *)pItem)->Set_View(true);
					break;

				default:
					break;
				}
			}
		}

		if( iMap && (iMap = DLG_Maps_Add()) >= 0 )
		{
			for(iItem=0; iItem<IDs.GetCount(); iItem++)
			{
				if( IDs[iItem].IsOk() )
				{
					pItem	= (CWKSP_Base_Item *)GetItemData(IDs[iItem]);

					switch( pItem->Get_Type() )
					{
					case WKSP_ITEM_Grid:
					case WKSP_ITEM_Shapes:
					case WKSP_ITEM_TIN:
						g_pMaps->Add((CWKSP_Layer *)pItem, g_pMaps->Get_Map(iMap));
						break;

					default:
						break;
					}
				}
			}

			g_pMaps->Get_Map(iMap)->View_Show(true);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Base_Control::_Load_Settings(void)
{
	wxString	File_Path;
	CSG_File	Stream;

	if( Get_Selection_Count() > 0 && DLG_Open(File_Path, ID_DLG_PARAMETERS_OPEN) && Stream.Open(CSG_String( File_Path.mb_str()), SG_FILE_R, true) )
	{
		if(	GetWindowStyle() & wxTR_MULTIPLE )
		{
			wxArrayTreeItemIds	IDs;

			if( GetSelections(IDs) > 0 )
			{
				for(size_t i=0; i<IDs.GetCount(); i++)
				{
					Stream.Seek_Start();

					_Load_Settings(&Stream, (CWKSP_Base_Item *)GetItemData(IDs[i]));
				}

				return( true );
			}
		}
		else
		{
			return( _Load_Settings(&Stream, Get_Item_Selected()) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::_Load_Settings(CSG_File *pStream, CWKSP_Base_Item *pItem)
{
	if( pItem && pItem->Get_Parameters() )
	{
		if( pItem->Get_Parameters()->Serialize(*pStream, false) )
		{
			pItem->Parameters_Changed();
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Base_Control::On_Item_LClick(wxMouseEvent &event)
{
	_Set_Active();

	event.Skip();
}

//---------------------------------------------------------
void CWKSP_Base_Control::On_Item_LDClick(wxMouseEvent &event)
{
	CWKSP_Base_Item	*pItem;

	if( (pItem = Get_Item_Selected()) != NULL )
	{
		pItem->On_Command(ID_CMD_WKSP_ITEM_RETURN);
	}
}

//---------------------------------------------------------
void CWKSP_Base_Control::On_Item_RClick(wxTreeEvent &event)
{
	if( Get_Selection_Count() <= 1 )
	{
	//	SelectItem(event.GetItem());

		_Set_Active();
	}

	wxMenu	*pMenu	= Get_Context_Menu();

	if( pMenu )
	{
		PopupMenu(pMenu);

		delete(pMenu);
	}

	event.Skip();
}

//---------------------------------------------------------
void CWKSP_Base_Control::On_Item_KeyDown(wxTreeEvent &event)
{
	CWKSP_Base_Item	*pItem;

	if( event.GetKeyCode() == WXK_DELETE )
	{
		_Del_Active(false);
	}
	else if( (pItem = Get_Item_Selected()) != NULL )
	{
		switch( event.GetKeyCode() )
		{
		default:
			pItem->On_Command(event.GetKeyCode());
			break;

		case WXK_RETURN:
			pItem->On_Command(ID_CMD_WKSP_ITEM_RETURN);
			break;
		}
	}
}

//---------------------------------------------------------
void CWKSP_Base_Control::On_Item_SelChanged(wxTreeEvent &event)
{
	_Set_Active();

	event.Skip();
}

//---------------------------------------------------------
void CWKSP_Base_Control::On_Item_Delete(wxTreeEvent &event)
{
	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
