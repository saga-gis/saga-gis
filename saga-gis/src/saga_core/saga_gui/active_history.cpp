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
#include <wx/imaglist.h>

#include "res_dialogs.h"
#include "res_controls.h"
#include "res_commands.h"
#include "res_images.h"

#include "wksp_table.h"
#include "wksp_layer.h"

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

	EVT_MENU			(ID_CMD_DATA_HISTORY_CLEAR	, CACTIVE_History::On_Clear)
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
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DATA_MANAGER);		// ROOT
	IMG_ADD_TO_TREECTRL(ID_IMG_TB_MAP_ZOOM_NEXT);		// NODE
	IMG_ADD_TO_TREECTRL(ID_IMG_TB_INFO);				// ENTRY
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_GRID);				// GRID
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TABLE);				// TABLE
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POLYGON);	// SHAPES
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_POINTCLOUD);		// POINTCLOUD
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TIN);				// TIN
	IMG_ADD_TO_TREECTRL(ID_IMG_TB_OPEN);				// FILE

	Set_Item(NULL);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_History::On_Mouse_RDown(wxMouseEvent &event)
{
	wxMenu	Menu(_TL("Legend"));

	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DATA_HISTORY_CLEAR);

	PopupMenu(&Menu, event.GetPosition());

	event.Skip();
}

//---------------------------------------------------------
void CACTIVE_History::On_Clear(wxCommandEvent &event)
{
	CWKSP_Base_Item	*pItem	= g_pACTIVE->Get_Item();

	if( !pItem || !pItem->GetId().IsOk() )
	{
		return;
	}

	CSG_Data_Object	*pObject	= NULL;

	if( pItem )
	{
		switch( pItem->Get_Type() )
		{
		default:					pObject	= NULL;									break;

		case WKSP_ITEM_Table:		pObject	= ((CWKSP_Table *)pItem)->Get_Table();	break;

		case WKSP_ITEM_Shapes:
		case WKSP_ITEM_TIN:
		case WKSP_ITEM_PointCloud:
		case WKSP_ITEM_Grid:		pObject	= ((CWKSP_Layer *)pItem)->Get_Object();	break;
		}
	}

	int	Depth	= 0;

	if( pObject && DLG_Get_Number(Depth, _TL("Delete History Entries"), _TL("Depth")) )
	{
		pObject->Get_History().Del_Children(Depth);

		Set_Item(pItem);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CACTIVE_History::Set_Item(CWKSP_Base_Item *pItem)
{
	CSG_Data_Object	*pObject	= NULL;

	if( pItem )
	{
		switch( pItem->Get_Type() )
		{
		default:					pObject	= NULL;									break;

		case WKSP_ITEM_Table:		pObject	= ((CWKSP_Table *)pItem)->Get_Table();	break;

		case WKSP_ITEM_Shapes:
		case WKSP_ITEM_TIN:
		case WKSP_ITEM_PointCloud:
		case WKSP_ITEM_Grid:		pObject	= ((CWKSP_Layer *)pItem)->Get_Object();	break;
		}
	}

	Freeze();

	DeleteAllItems();

	if( pObject == NULL || pObject->Get_History().Get_Children_Count() <= 0 )
	{
		AddRoot(_TL("No history available"), IMG_ROOT);
	}
	else
	{
		_Add_History(AddRoot(pObject->Get_Name(), IMG_ROOT), pObject->Get_History());

		Expand(GetRootItem());
	}

	Thaw();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

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

//---------------------------------------------------------
bool CACTIVE_History::_Add_History(wxTreeItemId Parent, CSG_MetaData &History)
{
	if( !Parent.IsOk() || History.Get_Children_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int					i;
	TSG_Parameter_Type	Type;
	CSG_String			Name;
	CSG_MetaData		*pEntry;
	wxTreeItemId		Node;

	//-----------------------------------------------------
	if( (pEntry = History.Get_Child("MODULE")) == NULL )
	{
		for(i=0; i<History.Get_Children_Count(); i++)
		{
			pEntry	= History.Get_Child(i);

			if( pEntry->Get_Children_Count() > 0 )
			{
				_Add_History(AppendItem(Parent, wxString::Format(wxT("%s"), pEntry->Get_Name().c_str()), IMG_NODE), *pEntry);
			}
			else
			{
				if( !pEntry->Get_Name().Cmp(SG_T("FILE")) )
				{
					AppendItem(Parent, wxString::Format(wxT("%s"), pEntry->Get_Content().c_str()), IMG_FILE);
				}
				else
				{
					AppendItem(Parent, wxString::Format(wxT("[%s] %s"), pEntry->Get_Name().c_str(), pEntry->Get_Content().c_str()), IMG_ENTRY);
				}
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		Node	= AppendItem(Parent, _TL("Module"), IMG_NODE);

		AppendItem(Node, pEntry->Get_Content().c_str(), IMG_ENTRY);

		Expand(Node);

		//-------------------------------------------------
		Node	= AppendItem(Parent, _TL("Options"), IMG_NODE);

		for(i=0; i<History.Get_Children_Count(); i++)
		{
			pEntry	= History.Get_Child(i);
			Name	= pEntry->Get_Property(SG_T("name"));
			Type	= SG_Parameter_Type_Get_Type(pEntry->Get_Property(SG_T("type")));

			if( !pEntry->Get_Name().Cmp(SG_T("OPTION")) )
			{
				switch( Type )
				{
				case PARAMETER_TYPE_Bool:
				case PARAMETER_TYPE_Int:
				case PARAMETER_TYPE_Double:
				case PARAMETER_TYPE_Degree:
				case PARAMETER_TYPE_Choice:
				case PARAMETER_TYPE_Range:
				case PARAMETER_TYPE_Table_Field:
				case PARAMETER_TYPE_String:
				case PARAMETER_TYPE_Text:
				case PARAMETER_TYPE_FilePath:
					AppendItem(Node, wxString::Format(wxT("%s [%s: %s]"),
						Name.c_str(), SG_Parameter_Type_Get_Name(Type).c_str(),
						pEntry->Get_Content().c_str()),
						IMG_ENTRY
					);
					break;

				case PARAMETER_TYPE_Grid_System:
					break;

				case PARAMETER_TYPE_FixedTable:
				case PARAMETER_TYPE_Parameters:
					break;

				default:
					break;
				}
			}
		}

		if( GetChildrenCount(Node, false) == 0 )
		{
			AppendItem(Node, _TL("no options"), IMG_ENTRY);
		}

		Expand(Node);

		//-------------------------------------------------
		Node	= AppendItem(Parent, _TL("Input Data"), IMG_NODE);

		for(i=0; i<History.Get_Children_Count(); i++)
		{
			pEntry	= History.Get_Child(i);
			Name	= pEntry->Get_Property(SG_T("name"));
			Type	= SG_Parameter_Type_Get_Type(pEntry->Get_Property(SG_T("type")));

			if( !pEntry->Get_Name().Cmp(SG_T("DATA")) )
			{
				_Add_History(AppendItem(Node, wxString::Format(wxT("%s"), Name.c_str()), _Get_Image(Type)), *pEntry);
			}
			else if( !pEntry->Get_Name().Cmp(SG_T("DATA_LIST")) )
			{
				wxTreeItemId	List	= AppendItem(Node, wxString::Format(wxT("%s %s"), Name.c_str(), _TL("List")), _Get_Image(Type));

				for(int j=0; j<pEntry->Get_Children_Count(); j++)
				{
					_Add_History(AppendItem(List, wxString::Format(wxT("%d. %s"), j + 1, _TL("Item")), _Get_Image(Type)), *pEntry->Get_Child(j));
				}

				Expand(List);
			}
		}

		if( GetChildrenCount(Node, false) == 0 )
		{
			AppendItem(Node, _TL("no input data"), IMG_ENTRY);
		}

		Expand(Node);
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
