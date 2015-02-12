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

#include "wksp_data_manager.h"
#include "wksp_data_layers.h"
#include "wksp_table.h"

#include "wksp_map_manager.h"
#include "wksp_map.h"
#include "wksp_map_buttons.h"
#include "wksp_layer.h"


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
	m_bUpdating	= false;

	AssignImageList(new wxImageList(IMG_SIZE_TREECTRL, IMG_SIZE_TREECTRL, true, 0));
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_NOITEMS);
}

//---------------------------------------------------------
CWKSP_Base_Control::~CWKSP_Base_Control(void)
{}


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
		AppendItem	(m_pManager->GetId(), _TL("<no items>"), IMG_NO_ITEMS, IMG_NO_ITEMS, NULL);
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
	switch( event.GetId() )
	{
	case ID_CMD_WKSP_ITEM_CLOSE:
		_Del_Active(false);
		break;

	case ID_CMD_WKSP_ITEM_SHOW:
		_Show_Active();
		break;

	case ID_CMD_WKSP_ITEM_SETTINGS_LOAD:
		_Load_Settings();
		break;

	case ID_CMD_WKSP_ITEM_SETTINGS_COPY:
		_Copy_Settings();
		break;

	case ID_CMD_WKSP_ITEM_SEARCH:
		_Search_Item();
		break;

	//-----------------------------------------------------
	default:
		if( !m_pManager->On_Command(event.GetId()) )
		{
			CWKSP_Base_Item	*pItem	= Get_Item_Selected();

			if( pItem )
			{
				pItem->On_Command(event.GetId());
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Base_Control::On_Command_UI(wxUpdateUIEvent &event)
{
	if( !m_pManager->On_Command_UI(event) )
	{
		CWKSP_Base_Item	*pItem	= Get_Item_Selected();

		if( pItem )
		{
			pItem->On_Command_UI(event);
		}
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
	if( pItem == NULL || pItem->GetId().IsOk() == false || pItem->Get_Control() != this )
	{
		return( false );
	}

	if( pItem->Get_Type() == WKSP_ITEM_Table &&	((CWKSP_Table *)pItem)->Get_Object()->Get_ObjectType() != DATAOBJECT_TYPE_Table )
	{
		return( false );
	}

	if( !bSilent && !_Del_Item_Confirm(pItem) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( pItem == m_pManager )
	{
		if( m_pManager->Get_Count() > 0 )
		{
			Freeze();

			if( m_pManager == g_pData || m_pManager == g_pMaps )
			{
				if( g_pData_Buttons )	{	g_pData_Buttons->Freeze();	}
				if( g_pMap_Buttons  )	{	g_pMap_Buttons ->Freeze();	}
			}

			//---------------------------------------------
			DeleteChildren	(m_pManager->GetId());
			AppendItem		(m_pManager->GetId(), _TL("<no items>"), 0, 0, NULL);
			Expand			(m_pManager->GetId());

			if( g_pModules == m_pManager )
			{
				g_pModules->Update();
			}

			//---------------------------------------------
			Thaw();

			if( m_pManager == g_pData || m_pManager == g_pMaps )
			{
				if( g_pData_Buttons )	{	g_pData_Buttons->Thaw();	g_pData_Buttons->Update_Buttons();	}
				if( g_pMap_Buttons  )	{	g_pMap_Buttons ->Thaw();	g_pMap_Buttons ->Update_Buttons();	}
			}

			return( true );
		}
	}

	//-----------------------------------------------------
	else
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
			g_pModules->Update();
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
		if( GetWindowStyle() & wxTR_MULTIPLE )
		{
			if( bKeepMultipleSelection )
			{
				ToggleItemSelection(pItem->GetId());
			}
			else
			{
				m_bUpdating	= true;
				SelectItem(pItem->GetId());
				m_bUpdating	= false;

				wxArrayTreeItemIds	IDs;
				
				if( GetSelections(IDs) > 1 )
				{
					for(size_t i=0; i<IDs.Count(); i++)
					{
						if( IDs[i] != pItem->GetId() )
						{
							UnselectItem(IDs[i]);
						}
					}
				}
			}
		}
		else
		{
			SelectItem(pItem->GetId());
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
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SETTINGS_COPY);
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
bool CWKSP_Base_Control::_Del_Active(bool bSilent)
{
	if( m_pManager->Get_Type() == WKSP_ITEM_Data_Manager && PROCESS_is_Executing() )	// never allow data deletion during module execution!
	{
		return( false );
	}

	if( GetWindowStyle() & wxTR_MULTIPLE )
	{
		wxArrayTreeItemIds	IDs;

		if( GetSelections(IDs) > 0 && (bSilent || DLG_Message_Confirm(ID_DLG_DELETE)) && (m_pManager->Get_Type() != WKSP_ITEM_Data_Manager || g_pData->Save_Modified_Sel()) )
		{
			UnselectAll();

			for(size_t i=0; i<IDs.GetCount(); i++)
			{
				if( IDs[i].IsOk() )
				{
					_Del_Item((CWKSP_Base_Item *)GetItemData(IDs[i]), true);
				}
			}

			SetFocus();
		}
	}
	else
	{
		wxTreeItemId	ID	= GetSelection();

		if( ID.IsOk() )
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
				case WKSP_ITEM_PointCloud:
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
					case WKSP_ITEM_PointCloud:
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
	wxString		File_Path;
	CSG_MetaData	Data;

	if( Get_Selection_Count() > 0 && DLG_Open(File_Path, ID_DLG_PARAMETERS_OPEN) && Data.Load(&File_Path) )
	{
		if(	GetWindowStyle() & wxTR_MULTIPLE )
		{
			wxArrayTreeItemIds	IDs;

			if( GetSelections(IDs) > 0 )
			{
				for(size_t i=0; i<IDs.GetCount(); i++)
				{
					_Load_Settings(&Data, (CWKSP_Base_Item *)GetItemData(IDs[i]));
				}
			}
		}
		else
		{
			_Load_Settings(&Data, Get_Item_Selected());
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::_Load_Settings(CSG_MetaData *pData, CWKSP_Base_Item *pItem)
{
	if( pData && pItem && pItem->Get_Parameters() )
	{
		if( pItem->Get_Parameters()->Serialize(*pData, false) )
		{
			pItem->Parameters_Changed();
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void	DLG_Copy_Settings(CSG_Table &List, CWKSP_Base_Item *pItem)
{
	if( pItem )
	{
		if( pItem->is_Manager() )
		{
			for(int i=0; i<((CWKSP_Base_Manager *)pItem)->Get_Count(); i++)
			{
				DLG_Copy_Settings(List, ((CWKSP_Base_Manager *)pItem)->Get_Item(i));
			}
		}
		else if( pItem->Get_Parameters() )
		{
			CSG_Table_Record	*pEntry	= List.Add_Record();

			pEntry->Set_Value(0, CSG_String(wxString::Format(SG_T("[%s] %s"), pItem->Get_Manager()->Get_Name(), pItem->Get_Name()).wc_str()));
			pEntry->Set_Value(1, (long)pItem->Get_Parameters());
		}
	}
}

CSG_Parameters *	DLG_Copy_Settings(void)
{
	CSG_Table	List;

	List.Add_Field(SG_T("NAME"), SG_DATATYPE_String);
	List.Add_Field(SG_T("PRMS"), SG_DATATYPE_Int);

	DLG_Copy_Settings(List, (CWKSP_Base_Item *)g_pData->Get_Grids());
	DLG_Copy_Settings(List, (CWKSP_Base_Item *)g_pData->Get_Shapes());
	DLG_Copy_Settings(List, (CWKSP_Base_Item *)g_pData->Get_TINs());
	DLG_Copy_Settings(List, (CWKSP_Base_Item *)g_pData->Get_PointClouds());

	if( List.Get_Count() > 0 )
	{
		wxArrayString	Items;

		for(int i=0; i<List.Get_Count(); i++)
		{
			Items.Add(List.Get_Record(i)->asString(0));
		}

		wxSingleChoiceDialog	dlg(MDI_Get_Top_Window(),
			_TL("Copy Settings from..."),
			_TL("Select a layer to copy settings from it."),
			Items
		);

		bool	bOk	= dlg.ShowModal() == wxID_OK;

		if( bOk )
		{
			return( (CSG_Parameters *)List.Get_Record(dlg.GetSelection())->asInt(1) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::_Copy_Settings(CSG_Parameters *pParameters, CWKSP_Base_Item *pItem)
{
	if( pParameters && pItem && pParameters != pItem->Get_Parameters() )
	{
		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			CSG_Parameter	*pSource	= pParameters->Get_Parameter(i);

			if(	SG_STR_CMP(pSource->Get_Identifier(), SG_T("OBJECT_NAME")) )
		//	&&	SG_STR_CMP(pSource->Get_Identifier(), SG_T("LUT_ATTRIB"))
		//	&&	SG_STR_CMP(pSource->Get_Identifier(), SG_T("METRIC_ATTRIB"))
		//	&&	SG_STR_CMP(pSource->Get_Identifier(), SG_T("LABEL_ATTRIB"))
		//	&&	SG_STR_CMP(pSource->Get_Identifier(), SG_T("LABEL_ATTRIB_SIZE_BY"))	)
			{
				CSG_Parameter	*pTarget	= pItem->Get_Parameter(pSource->Get_Identifier());

				if( pTarget && pTarget->Get_Type() == pSource->Get_Type() )
				{
					pTarget->Set_Value(pSource);
				}
			}
		}

		pItem->Parameters_Changed();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::_Copy_Settings(void)
{
	wxArrayTreeItemIds	IDs;
	CSG_Parameters		*pParameters;

	if( Get_Selection_Count() > 0 && (pParameters = DLG_Copy_Settings()) != NULL )
	{
		if(	(GetWindowStyle() & wxTR_MULTIPLE) && GetSelections(IDs) > 0 )
		{
			for(size_t i=0; i<IDs.GetCount(); i++)
			{
				_Copy_Settings(pParameters, (CWKSP_Base_Item *)GetItemData(IDs[i]));
			}

			return( true );
		}
		else
		{
			return( _Copy_Settings(pParameters, Get_Item_Selected()) );
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
bool CWKSP_Base_Control::_Search_Compare(wxString A, wxString B, bool bCase)
{
	return( bCase ? B.Find(A) != wxNOT_FOUND : B.MakeUpper().Find(A.MakeUpper().c_str()) != wxNOT_FOUND );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::_Search_Get_List(CSG_Table *pList, CWKSP_Base_Item *pItem, const wxString &String, bool bName, bool bDesc, bool bCase, TWKSP_Item Type)
{
	if( pItem == NULL )
	{
		return( false );
	}

	if(	Type == WKSP_ITEM_Undefined || Type == pItem->Get_Type() )
	{
		if( (bName && _Search_Compare(String, pItem->Get_Name       (), bCase))
		||  (bDesc && _Search_Compare(String, pItem->Get_Description(), bCase)) )
		{
			CSG_Table_Record	*pRecord	= pList->Add_Record();

			pRecord->Set_Value(0, pItem->Get_Name().wx_str());
			pRecord->Set_Value(1, pItem->Get_Type_Name(pItem->Get_Type()).wx_str());
			pRecord->Set_Value(2, (long)pItem);
		}
	}

	if( pItem->is_Manager() )
	{
		for(int i=0; i<((CWKSP_Base_Manager *)pItem)->Get_Count(); i++)
		{
			_Search_Get_List(pList, ((CWKSP_Base_Manager *)pItem)->Get_Item(i), String, bName, bDesc, bCase, Type);
		}
	}

	return( true );
}

//---------------------------------------------------------
CWKSP_Base_Item * CWKSP_Base_Control::Search_Item(const wxString &Caption, TWKSP_Item Type)
{
	static CSG_Parameters	Search(NULL, Caption, _TL(""));

	if( Search.Get_Count() == 0 )
	{
		Search.Add_String	(NULL, "STRING"	, _TL("Search for...")	, _TL(""), SG_T(""));
		Search.Add_Value	(NULL, "NAME"	, _TL("Name")			, _TL(""), PARAMETER_TYPE_Bool, true);
		Search.Add_Value	(NULL, "DESC"	, _TL("Description")	, _TL(""), PARAMETER_TYPE_Bool, false);
		Search.Add_Value	(NULL, "CASE"	, _TL("Case Sensitive")	, _TL(""), PARAMETER_TYPE_Bool, false);
	}

	if( !DLG_Parameters(&Search) )
	{
		return( NULL );
	}

	//-----------------------------------------------------
	CSG_Table	List;

	List.Add_Field(_TL("NAME")	, SG_DATATYPE_String);
	List.Add_Field(_TL("TYPE")	, SG_DATATYPE_String);
	List.Add_Field(_TL("ADDR")	, SG_DATATYPE_Long);

	_Search_Get_List(&List, m_pManager, Search("STRING")->asString(), Search("NAME")->asBool(), Search("DESC")->asBool(), Search("CASE")->asBool(), Type);

	if( List.Get_Count() <= 0 )
	{
		wxMessageBox(_TL("Search text not found"), _TL("Search for..."), wxOK|wxICON_EXCLAMATION);

		return( NULL );
	}

	//-----------------------------------------------------
	List.Set_Index(1, TABLE_INDEX_Ascending, 0, TABLE_INDEX_Ascending);

	wxArrayString	Items;

	for(int i=0; i<List.Get_Count(); i++)
	{
		if( Type == WKSP_ITEM_Undefined )
		{
			Items.Add(wxString::Format(wxT("[%s] %s"), List[i].asString(1), List[i].asString(0)));
		}
		else
		{
			Items.Add(List[i].asString(0));
		}
	}

	wxSingleChoiceDialog	dlg(MDI_Get_Top_Window(),
		wxString::Format(wxT("%s: '%s'"), _TL("Search Result"), Search("STRING")->asString()),
		Caption, Items
	);

	if( dlg.ShowModal() != wxID_OK )
	{
		return( NULL );
	}

	//-----------------------------------------------------
	return( (CWKSP_Base_Item *)List[dlg.GetSelection()].asInt(2) );
}

//---------------------------------------------------------
bool CWKSP_Base_Control::_Search_Item(void)
{
	CWKSP_Base_Item	*pItem	= Search_Item(_TL("Locate..."));

	if( pItem && pItem->GetId().IsOk() )
	{
		EnsureVisible	(pItem->GetId());
		SelectItem		(pItem->GetId());
		ScrollTo		(pItem->GetId());

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
	g_pACTIVE->Set_Active(Get_Item_Selected());

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

		g_pACTIVE->Set_Active(Get_Item_Selected());
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
	if( g_pACTIVE && (!m_bUpdating || Get_Item_Selected()) )
	{
		g_pACTIVE->Set_Active(Get_Item_Selected());
	}

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
