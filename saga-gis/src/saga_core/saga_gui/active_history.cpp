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
//                  ACTIVE_History.cpp                   //
//                                                       //
//          Copyright (C) 2008 by Olaf Conrad            //
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
#include <wx/imaglist.h>

#include "res_dialogs.h"
#include "res_controls.h"
#include "res_commands.h"
#include "res_images.h"

#include "wksp_table.h"
#include "wksp_layer.h"
#include "wksp_map_layer.h"
#include "wksp_tool_manager.h"

#include "active.h"
#include "active_history.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	IMG_ROOT	= 0,
	IMG_NODE,
	IMG_TOOL,
	IMG_ENTRY,
	IMG_GRID,
	IMG_TABLE,
	IMG_SHAPES,
	IMG_POINTCLOUD,
	IMG_TIN,
	IMG_FILE
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CACTIVE_History, wxTreeCtrl)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CACTIVE_History, wxTreeCtrl)
	EVT_RIGHT_DOWN		(CACTIVE_History::On_Mouse_RDown)

	EVT_MENU			(ID_CMD_DATA_HISTORY_CLEAR           , CACTIVE_History::On_Clear)
	EVT_MENU			(ID_CMD_DATA_HISTORY_TO_MODEL        , CACTIVE_History::On_SaveAs_Model)
	EVT_MENU			(ID_CMD_DATA_HISTORY_OPTIONS_COLLAPSE, CACTIVE_History::On_Options_Expand)
	EVT_MENU			(ID_CMD_DATA_HISTORY_OPTIONS_EXPAND  , CACTIVE_History::On_Options_Expand)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CACTIVE_History::CACTIVE_History(wxWindow *pParent)
	: wxTreeCtrl(pParent, ID_WND_ACTIVE_HISTORY , wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS)
{
	AssignImageList(new wxImageList(IMG_SIZE_TREECTRL, IMG_SIZE_TREECTRL, true, 0));
	IMG_ADD_TO_TREECTRL(ID_IMG_NB_ACTIVE_HISTORY);		// ROOT
	IMG_ADD_TO_TREECTRL(ID_IMG_TB_MAP_ZOOM_NEXT);		// NODE
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TOOL);			// TOOL
	IMG_ADD_TO_TREECTRL(ID_IMG_TB_INFO);				// ENTRY
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_GRID);				// GRID
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TABLE);				// TABLE
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POLYGON);	// SHAPES
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_POINTCLOUD);		// POINTCLOUD
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TIN);				// TIN
	IMG_ADD_TO_TREECTRL(ID_IMG_TB_OPEN);				// FILE

	m_pItem		= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CACTIVE_History::Set_Item(CWKSP_Base_Item *pItem)
{
	m_pItem	= pItem;

	return( _Set_History() );
}

//---------------------------------------------------------
bool CACTIVE_History::_Set_History(void)
{
	Freeze();

	DeleteAllItems();

	CSG_Data_Object	*pObject	= _Get_Object();

	if( pObject == NULL || pObject->Get_History().Get_Children_Count() <= 0 )
	{
		AddRoot(_TL("no history"), IMG_ROOT);
	}
	else if( SG_Compare_Version(pObject->Get_History().Get_Property("saga-version"), "2.1.3") >= 0 )	// new version
	{
		_Add_History(AddRoot(pObject->Get_Name(), IMG_ROOT), pObject->Get_History());
	}
	else
	{
		_Add_History_OLD(AddRoot(pObject->Get_Name(), IMG_ROOT), pObject->Get_History());
	}

	Expand(GetRootItem());

	Thaw();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_History::On_Mouse_RDown(wxMouseEvent &event)
{
	wxMenu	Menu(_TL("History"));

	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DATA_HISTORY_CLEAR);
	Menu.AppendSeparator();
	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DATA_HISTORY_OPTIONS_COLLAPSE);
	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DATA_HISTORY_OPTIONS_EXPAND);

	if( _Get_Object() && SG_Compare_Version(_Get_Object()->Get_History().Get_Property("saga-version"), "2.1.3") >= 0 )	// new version
	{
		Menu.AppendSeparator();
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DATA_HISTORY_TO_MODEL);
	}

	PopupMenu(&Menu, event.GetPosition());

	event.Skip();
}

//---------------------------------------------------------
void CACTIVE_History::On_Clear(wxCommandEvent &event)
{
	CSG_Data_Object	*pObject	= _Get_Object();

	int	Depth	= 0;

	if( pObject && DLG_Get_Number(Depth, _TL("Delete History Entries"), _TL("Depth")) )
	{
		pObject->Get_History().Del_Children(Depth, Depth > 0 ? SG_T("TOOL") : SG_T(""));
		pObject->Set_Modified(true);

		Set_Item(g_pACTIVE->Get_Active());
	}
}

//---------------------------------------------------------
void CACTIVE_History::On_SaveAs_Model(wxCommandEvent &event)
{
	const wxString	Filter	= wxString::Format("%s|*.xml|%s|*.*",
		_TL("XML Files"), _TL("All Files")
	);

	wxString	File;

	CSG_Data_Object	*pObject	= _Get_Object();

	if( pObject && pObject->Get_History().Get_Children_Count() > 0
	&&  DLG_Save(File, _TL("Save History as Model"), Filter)
	&&  pObject->Save_History_to_Model(&File) )
	{
		g_pTools->Open(File);
	}
}

//---------------------------------------------------------
void CACTIVE_History::On_Options_Expand(wxCommandEvent &event)
{
	_Expand(GetRootItem(), "Options", event.GetId() == ID_CMD_DATA_HISTORY_OPTIONS_EXPAND);
}

//---------------------------------------------------------
void CACTIVE_History::_Expand(wxTreeItemId Node, const wxString &Name, bool bExpand)
{
	if( !GetItemText(Node).Cmp(Name) )
	{
		if( bExpand )
		{
			Expand(Node);
		}
		else
		{
			Collapse(Node);
		}
	}
	else
	{
		wxTreeItemIdValue	Cookie;

		wxTreeItemId	Child	= GetFirstChild(Node, Cookie);

		while( Child.IsOk() )
		{
			_Expand(Child, Name, bExpand);

			Child	= GetNextChild(Node, Cookie);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Object * CACTIVE_History::_Get_Object(void)
{
	if( m_pItem && m_pItem->GetId().IsOk() )
	{
		if( m_pItem->Get_Type() == WKSP_ITEM_Table
		||  m_pItem->Get_Type() == WKSP_ITEM_TIN
		||  m_pItem->Get_Type() == WKSP_ITEM_PointCloud
		||  m_pItem->Get_Type() == WKSP_ITEM_Shapes
		||  m_pItem->Get_Type() == WKSP_ITEM_Grid
		||  m_pItem->Get_Type() == WKSP_ITEM_Grids )
		{
			return( ((CWKSP_Data_Item *)m_pItem)->Get_Object() );
		}

		if( m_pItem->Get_Type() == WKSP_ITEM_Map_Layer )
		{
			return( ((CWKSP_Map_Layer *)m_pItem)->Get_Layer()->Get_Object() );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
int CACTIVE_History::_Get_Image(TSG_Parameter_Type Type)
{
	switch( Type )
	{
	case PARAMETER_TYPE_Grid:				return( IMG_GRID		);
	case PARAMETER_TYPE_Table:				return( IMG_TABLE		);
	case PARAMETER_TYPE_Shapes:				return( IMG_SHAPES		);
	case PARAMETER_TYPE_PointCloud:			return( IMG_POINTCLOUD	);
	case PARAMETER_TYPE_TIN:				return( IMG_TIN			);

	case PARAMETER_TYPE_Grid_List:			return( IMG_GRID		);
	case PARAMETER_TYPE_Table_List:			return( IMG_TABLE		);
	case PARAMETER_TYPE_Shapes_List:		return( IMG_SHAPES		);
	case PARAMETER_TYPE_PointCloud_List:	return( IMG_POINTCLOUD	);
	case PARAMETER_TYPE_TIN_List:			return( IMG_TIN			);

	default:								return( IMG_NODE );
	}
}

//---------------------------------------------------------
int CACTIVE_History::_Get_Image(const CSG_String &Type)
{
	return( _Get_Image(SG_Parameter_Type_Get_Type(Type)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CACTIVE_History::_Add_History(wxTreeItemId Parent, CSG_MetaData &Data)
{
	if( !Parent.IsOk() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	*pTool	= Data("TOOL") ? Data("TOOL") : Data("MODULE");

	if( !pTool || !pTool->Get_Child("OUTPUT") )
	{
		if( Data("FILE") )
		{
			AppendItem(Parent, Data["FILE"].Get_Content().c_str(), IMG_FILE);
		}
		else
		{
			AppendItem(Parent, Data.Get_Name().c_str(), IMG_FILE);
		}
	}

	//-----------------------------------------------------
	else
	{
		int	i;

		wxTreeItemId	Tool	= AppendItem(Parent, wxString::Format("%s [%s]",
			pTool->Get_Child("OUTPUT")->Get_Property("name"), pTool->Get_Property("name")
			), IMG_TOOL
		);

		//-------------------------------------------------
		wxTreeItemId	Options	= AppendItem(Tool, _TL("Options"), IMG_ENTRY);

		for(i=0; i<pTool->Get_Children_Count(); i++)	// Options
		{
			CSG_MetaData		*pEntry	= pTool->Get_Child(i);
			CSG_String			Name	= pEntry->Get_Property("name");
			TSG_Parameter_Type	Type	= SG_Parameter_Type_Get_Type(pEntry->Get_Property("type"));

			if( !pEntry->Get_Name().Cmp("OPTION") )
			{
				switch( Type )
				{
				case PARAMETER_TYPE_Bool:
				case PARAMETER_TYPE_Int:
				case PARAMETER_TYPE_Double:
				case PARAMETER_TYPE_Degree:
				case PARAMETER_TYPE_Date:
				case PARAMETER_TYPE_Choice:
				case PARAMETER_TYPE_Range:
				case PARAMETER_TYPE_Table_Field:
				case PARAMETER_TYPE_Table_Fields:
				case PARAMETER_TYPE_String:
				case PARAMETER_TYPE_Text:
				case PARAMETER_TYPE_FilePath:
				//	AppendItem(Options, wxString::Format("%s [%s: %s]", Name.c_str(), SG_Parameter_Type_Get_Name(Type).c_str(), pEntry->Get_Content().c_str()), IMG_ENTRY);
					AppendItem(Options, wxString::Format("%s [%s]", Name.c_str(), pEntry->Get_Content().c_str()), IMG_ENTRY);
					break;

				case PARAMETER_TYPE_Grid_System:
					if( pEntry->Get_Children_Count() == 0 )
					{
					//	AppendItem(Options, wxString::Format("%s [%s: %s]", Name.c_str(), SG_Parameter_Type_Get_Name(Type).c_str(), pEntry->Get_Content().c_str()), IMG_ENTRY);
						AppendItem(Options, wxString::Format("%s [%s]", Name.c_str(), pEntry->Get_Content().c_str()), IMG_ENTRY);
					}
					break;

				case PARAMETER_TYPE_FixedTable:
				case PARAMETER_TYPE_Parameters:
					break;

				default:
					break;
				}
			}
		}

		if( ItemHasChildren(Options) )
		{
			Expand(Options);
		}
		else
		{
		//	Delete(Options);
			SetItemText(Options, _TL("No Options"));
		}

		//-------------------------------------------------
		for(i=0; i<pTool->Get_Children_Count(); i++)	// Input Data
		{
			CSG_MetaData		*pEntry	= pTool->Get_Child(i);
			CSG_String			Name	= pEntry->Get_Property("name");
			TSG_Parameter_Type	Type	= SG_Parameter_Type_Get_Type(pEntry->Get_Property("type"));

			if( !pEntry->Get_Name().Cmp("INPUT") )
			{
				_Add_History(AppendItem(Tool, wxString::Format("%s", Name.c_str()), _Get_Image(Type)), *pEntry);
			}
			else if( !pEntry->Get_Name().Cmp("INPUT_LIST") )
			{
				if( pEntry->Get_Children_Count() > 0 )
				{
					wxTreeItemId	List	= AppendItem(Tool, wxString::Format("%s %s", Name.c_str(), _TL("List")), _Get_Image(Type));

					for(int j=0; j<pEntry->Get_Children_Count(); j++)
					{
						_Add_History(List, *pEntry->Get_Child(j));
					}

					Expand(List);
				}
			}
		}

		Expand(Tool);
	}

	Expand(Parent);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Compatibility						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CACTIVE_History::_Add_History_OLD(wxTreeItemId Parent, CSG_MetaData &Data)
{
	if( !Parent.IsOk() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	*pTool	= Data("MODULE");

	if( !pTool )
	{
		if( Data.Get_Children_Count() > 0 )
		{
			for(int i=0; i<Data.Get_Children_Count(); i++)
			{
				CSG_MetaData	*pEntry	= Data.Get_Child(i);

				if( pEntry->Get_Children_Count() > 0 )
				{
					_Add_History_OLD(AppendItem(Parent, wxString::Format("%s", pEntry->Get_Name().c_str()), IMG_NODE), *pEntry);
				}
				else if( !pEntry->Get_Name().Cmp(SG_T("FILE")) )
				{
					AppendItem(Parent, wxString::Format("%s", pEntry->Get_Content().c_str()), IMG_FILE);
				}
				else
				{
					AppendItem(Parent, wxString::Format("[%s] %s", pEntry->Get_Name().c_str(), pEntry->Get_Content().c_str()), IMG_ENTRY);
				}
			}
		}
		else
		{
			AppendItem(Parent, Data.Get_Name().c_str(), IMG_FILE);
		}
	}

	//-----------------------------------------------------
	else
	{
		int	i;

		wxTreeItemId	Tool	= AppendItem(Parent, pTool->Get_Content().c_str(), IMG_TOOL);

		//-------------------------------------------------
		wxTreeItemId	Options	= AppendItem(Tool, _TL("Options"), IMG_ENTRY);

		for(i=0; i<Data.Get_Children_Count(); i++)	// Options
		{
			CSG_MetaData		*pEntry	= Data.Get_Child(i);
			CSG_String			Name	= pEntry->Get_Property("name");
			TSG_Parameter_Type	Type	= SG_Parameter_Type_Get_Type(pEntry->Get_Property("type"));

			if( !pEntry->Get_Name().Cmp("OPTION") )
			{
				switch( Type )
				{
				case PARAMETER_TYPE_Bool:
				case PARAMETER_TYPE_Int:
				case PARAMETER_TYPE_Double:
				case PARAMETER_TYPE_Degree:
				case PARAMETER_TYPE_Date:
				case PARAMETER_TYPE_Choice:
				case PARAMETER_TYPE_Range:
				case PARAMETER_TYPE_Table_Field:
				case PARAMETER_TYPE_Table_Fields:
				case PARAMETER_TYPE_String:
				case PARAMETER_TYPE_Text:
				case PARAMETER_TYPE_FilePath:
					AppendItem(Options, wxString::Format("%s [%s]", Name.c_str(), pEntry->Get_Content().c_str()), IMG_ENTRY);
					break;

				case PARAMETER_TYPE_Grid_System:
					if( pEntry->Get_Children_Count() == 0 )
					{
						AppendItem(Options, wxString::Format("%s [%s]", Name.c_str(), pEntry->Get_Content().c_str()), IMG_ENTRY);
					}
					break;

				case PARAMETER_TYPE_FixedTable:
				case PARAMETER_TYPE_Parameters:
					break;

				default:
					break;
				}
			}
		}

		if( ItemHasChildren(Options) )
		{
			Expand(Options);
		}
		else
		{
		//	Delete(Options);
			SetItemText(Options, _TL("No Options"));
		}

		//-------------------------------------------------
		for(i=0; i<Data.Get_Children_Count(); i++)	// Input Data
		{
			CSG_MetaData		*pEntry	= Data.Get_Child(i);
			CSG_String			Name	= pEntry->Get_Property("name");
			TSG_Parameter_Type	Type	= SG_Parameter_Type_Get_Type(pEntry->Get_Property("type"));

			if( !pEntry->Get_Name().Cmp("DATA") )
			{
				_Add_History_OLD(AppendItem(Tool, wxString::Format("%s", Name.c_str()), _Get_Image(Type)), *pEntry);
			}
			else if( !pEntry->Get_Name().Cmp("DATA_LIST") )
			{
				if( pEntry->Get_Children_Count() > 0 )
				{
					wxTreeItemId	List	= AppendItem(Tool, wxString::Format("%s %s", Name.c_str(), _TL("List")), _Get_Image(Type));

					for(int j=0; j<pEntry->Get_Children_Count(); j++)
					{
						_Add_History_OLD(List, *pEntry->Get_Child(j));
					}

					Expand(List);
				}
			}
		}

		Expand(Tool);
	}

	Expand(Parent);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
