/**********************************************************
 * Version $Id: Data_Source.cpp 911 2011-02-14 16:38:15Z reklov_w $
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
//                 data_source_odbc.cpp                  //
//                                                       //
//          Copyright (C) 2012 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'MicroCity: Spatial Analysis and //
// Simulation Framework'. MicroCity is free software;you //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// MicroCity is distributed in the hope that it will be  //
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
//                University of Hamburg                  //
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
#include <wx/menu.h>

#include "saga_odbc/saga_odbc.h"

#include "helper.h"

#include "res_controls.h"
#include "res_commands.h"
#include "res_dialogs.h"
#include "res_images.h"

#include "wksp.h"
#include "wksp_data_manager.h"

#include "data_source_odbc.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	ODBC_ITEM_TYPE_ROOT	= 0,
	ODBC_ITEM_TYPE_SOURCE_CLOSED,
	ODBC_ITEM_TYPE_SOURCE_OPENED,
	ODBC_ITEM_TYPE_TABLE
};

//---------------------------------------------------------
enum
{
	IMG_ROOT	= 0,
	IMG_SRC_CLOSED,
	IMG_SRC_OPENED,
	IMG_TABLE
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CData_Source_ODBC_Data : public wxTreeItemData
{
public:
    CData_Source_ODBC_Data(int Type, const CSG_String &Value = SG_T(""), CSG_ODBC_Connection *pConnection = NULL)
		: m_Type(Type), m_Value(Value), m_pConnection(pConnection)
	{}

	void					Set_Type		(int Type)							{	m_Type			= Type;			}
	void					Set_Connection	(CSG_ODBC_Connection *pConnection)	{	m_pConnection	= pConnection;	}

	int						Get_Type		(void)	const	{	return( m_Type        );	}
	const CSG_String &		Get_Value		(void)	const	{	return( m_Value       );	}
	CSG_ODBC_Connection *	Get_Connection	(void)	const	{	return( m_pConnection );	}


private:

    int						m_Type;

	CSG_String				m_Value;

	CSG_ODBC_Connection		*m_pConnection;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CData_Source_ODBC, wxTreeCtrl)
	EVT_MENU					(ID_CMD_ODBC_REFRESH			, CData_Source_ODBC::On_Refresh)
	EVT_MENU					(ID_CMD_ODBC_SOURCE_CLOSE_ALL	, CData_Source_ODBC::On_Source_Close_All)
	EVT_MENU					(ID_CMD_ODBC_SOURCE_CLOSE		, CData_Source_ODBC::On_Source_Close)
	EVT_MENU					(ID_CMD_ODBC_SOURCE_OPEN		, CData_Source_ODBC::On_Source_Open)
	EVT_MENU					(ID_CMD_ODBC_TABLE_OPEN			, CData_Source_ODBC::On_Table_Open)
	EVT_MENU					(ID_CMD_ODBC_TABLE_DELETE		, CData_Source_ODBC::On_Table_Delete)

	EVT_TREE_ITEM_ACTIVATED		(ID_WND_DATA_SOURCE_DATABASE	, CData_Source_ODBC::On_Item_Activated)
	EVT_TREE_ITEM_RIGHT_CLICK	(ID_WND_DATA_SOURCE_DATABASE	, CData_Source_ODBC::On_Item_RClick)
	EVT_TREE_ITEM_MENU			(ID_WND_DATA_SOURCE_DATABASE	, CData_Source_ODBC::On_Item_Menu)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CData_Source_ODBC::CData_Source_ODBC(wxWindow *pParent)
	: wxTreeCtrl(pParent, ID_WND_DATA_SOURCE_DATABASE)
{
	AssignImageList(new wxImageList(IMG_SIZE_TREECTRL, IMG_SIZE_TREECTRL, true, 0));
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_ODBC_SOURCES);		// IMG_ROOT
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_ODBC_SOURCE_OFF);	// IMG_SRC_CLOSED
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_ODBC_SOURCE_ON);	// IMG_SRC_OPENED
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_ODBC_TABLE);		// IMG_TABLE

	AddRoot(_TL("ODBC Sources"), IMG_ROOT, IMG_ROOT, new CData_Source_ODBC_Data(ODBC_ITEM_TYPE_ROOT));

//	Update_Sources();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_ODBC::On_Refresh(wxCommandEvent &WXUNUSED(event))
{
	Update_Item(GetSelection());
}

//---------------------------------------------------------
void CData_Source_ODBC::On_Source_Close_All(wxCommandEvent &WXUNUSED(event))
{
	Source_Close_All();
}

//---------------------------------------------------------
void CData_Source_ODBC::On_Source_Close(wxCommandEvent &WXUNUSED(event))
{
	Source_Close(GetSelection());
}

//---------------------------------------------------------
void CData_Source_ODBC::On_Source_Open(wxCommandEvent &WXUNUSED(event))
{
	Source_Open(GetSelection());
}

//---------------------------------------------------------
void CData_Source_ODBC::On_Table_Open(wxCommandEvent &WXUNUSED(event))
{
	Table_Open(GetSelection());
}

//---------------------------------------------------------
void CData_Source_ODBC::On_Table_Delete(wxCommandEvent &WXUNUSED(event))
{
	Table_Delete(GetSelection());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_ODBC::On_Item_Activated(wxTreeEvent &event)
{
	CData_Source_ODBC_Data	*pData	= event.GetItem().IsOk() ? (CData_Source_ODBC_Data *)GetItemData(event.GetItem()) : NULL; if( pData == NULL )	return;

	switch( pData->Get_Type() )
	{
	case ODBC_ITEM_TYPE_ROOT:			Update_Sources();	break;
	case ODBC_ITEM_TYPE_SOURCE_CLOSED:	Source_Open (event.GetItem());	break;
	case ODBC_ITEM_TYPE_TABLE:			Table_Open  (event.GetItem());	break;
	}
}

//---------------------------------------------------------
void CData_Source_ODBC::On_Item_RClick(wxTreeEvent &event)
{
	SelectItem(event.GetItem());

	event.Skip();
}

//---------------------------------------------------------
void CData_Source_ODBC::On_Item_Menu(wxTreeEvent &event)
{
	CData_Source_ODBC_Data	*pData	= event.GetItem().IsOk() ? (CData_Source_ODBC_Data *)GetItemData(event.GetItem()) : NULL; if( pData == NULL )	return;

	wxMenu	Menu;

	switch( pData->Get_Type() )
	{
	case ODBC_ITEM_TYPE_ROOT:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_ODBC_REFRESH);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_ODBC_SOURCE_CLOSE_ALL);
		break;

	case ODBC_ITEM_TYPE_SOURCE_CLOSED:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_ODBC_REFRESH);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_ODBC_SOURCE_OPEN);
		break;

	case ODBC_ITEM_TYPE_SOURCE_OPENED:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_ODBC_REFRESH);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_ODBC_SOURCE_CLOSE);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_ODBC_TABLE_FROM_QUERY);
		break;

	case ODBC_ITEM_TYPE_TABLE:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_ODBC_TABLE_OPEN);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_ODBC_TABLE_DELETE);
		break;
	}

	if( Menu.GetMenuItemCount() > 0 )
	{
		PopupMenu(&Menu);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_ODBC::Update_Item(const wxTreeItemId &Item)
{
	CData_Source_ODBC_Data	*pData	= Item.IsOk() ? (CData_Source_ODBC_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	switch( pData->Get_Type() )
	{
	case ODBC_ITEM_TYPE_ROOT:
		Update_Sources();
		break;

	case ODBC_ITEM_TYPE_SOURCE_CLOSED:
	case ODBC_ITEM_TYPE_SOURCE_OPENED:
		Update_Source(Item);
		break;
	}
}

//---------------------------------------------------------
void CData_Source_ODBC::Update_Source(const wxTreeItemId &Item)
{
	CData_Source_ODBC_Data	*pData	= Item.IsOk() ? (CData_Source_ODBC_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	if( pData->Get_Type() != ODBC_ITEM_TYPE_SOURCE_CLOSED
	&&  pData->Get_Type() != ODBC_ITEM_TYPE_SOURCE_OPENED )
	{
		return;
	}

	pData->Set_Connection(SG_ODBC_Get_Connection_Manager().Get_Connection(pData->Get_Value()));

	Freeze();

	DeleteChildren(Item);

	if( pData->Get_Connection() == NULL )
	{
		pData->Set_Type(ODBC_ITEM_TYPE_SOURCE_CLOSED);
		SetItemImage(Item, IMG_SRC_CLOSED, wxTreeItemIcon_Normal);
		SetItemImage(Item, IMG_SRC_CLOSED, wxTreeItemIcon_Selected);
	}
	else
	{
		pData->Set_Type(ODBC_ITEM_TYPE_SOURCE_OPENED);
		SetItemImage(Item, IMG_SRC_OPENED, wxTreeItemIcon_Normal);
		SetItemImage(Item, IMG_SRC_OPENED, wxTreeItemIcon_Selected);

		CSG_Strings	Tables;

		pData->Get_Connection()->Get_Tables(Tables);

		for(int i=0; i<Tables.Get_Count(); i++)
		{
			AppendItem(Item, Tables[i].c_str(), IMG_TABLE, IMG_TABLE, new CData_Source_ODBC_Data(ODBC_ITEM_TYPE_TABLE, Tables[i], pData->Get_Connection()));
		}

		SortChildren(Item);
		Expand      (Item);
	}

	Thaw();
}

//---------------------------------------------------------
void CData_Source_ODBC::Update_Source(const wxString &Server)
{
	wxTreeItemIdValue	Cookie;
	wxTreeItemId		Item	= GetFirstChild(GetRootItem(), Cookie);

	while( Item.IsOk() )
	{
		if( Server.Cmp(GetItemText(Item)) == 0 )
		{
			Update_Source(Item);
		}

		Item	= GetNextChild(Item, Cookie);
	}
}

//---------------------------------------------------------
void CData_Source_ODBC::Update_Sources(void)
{
	Freeze();

	DeleteChildren(GetRootItem());

	//-----------------------------------------------------
	CSG_Strings	Servers;
	
	SG_ODBC_Get_Connection_Manager().Get_Servers(Servers);

	for(int i=0; i<Servers.Get_Count(); i++)
	{
		Update_Source(AppendItem(GetRootItem(), Servers[i].c_str(), IMG_SRC_CLOSED, IMG_SRC_CLOSED,
			new CData_Source_ODBC_Data(ODBC_ITEM_TYPE_SOURCE_CLOSED, Servers[i]))
		);
	}

	//-----------------------------------------------------
	SortChildren(GetRootItem());
	Expand      (GetRootItem());

	Thaw();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_ODBC::Source_Close_All(void)
{
	CSG_ODBC_Connections	&Manager	= SG_ODBC_Get_Connection_Manager();

	while( Manager.Get_Count() > 0 )
	{
		Manager.Del_Connection(0, true);
	}

	Update_Sources();
}

//---------------------------------------------------------
void CData_Source_ODBC::Source_Close(const wxTreeItemId &Item)
{
	CData_Source_ODBC_Data	*pData	= Item.IsOk() ? (CData_Source_ODBC_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	SG_ODBC_Get_Connection_Manager().Del_Connection(pData->Get_Connection(), true);

	Update_Source(Item);
}

//---------------------------------------------------------
void CData_Source_ODBC::Source_Open	(const wxTreeItemId &Item)
{
	CData_Source_ODBC_Data	*pData	= Item.IsOk() ? (CData_Source_ODBC_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	wxString	Username, Password;

	if( DLG_Login(Username, Password) )
	{
		MSG_General_Add(wxString::Format(wxT("%s: %s..."), _TL("Open Database Connection"), pData->Get_Value().c_str()), true, true);

		if( SG_ODBC_Get_Connection_Manager().Add_Connection(pData->Get_Value(), &Username, &Password) )
		{
			MSG_General_Add(_TL("okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);
		}
		else
		{
			MSG_General_Add(_TL("failed"), false, false, SG_UI_MSG_STYLE_FAILURE);
		}

		Update_Source(Item);
	}
}

//---------------------------------------------------------
void CData_Source_ODBC::Table_Open(const wxTreeItemId &Item)
{
	CData_Source_ODBC_Data	*pData	= Item.IsOk() ? (CData_Source_ODBC_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	CSG_Table	*pTable	= SG_Create_Table();

	MSG_General_Add(wxString::Format(wxT("%s: [%s] %s..."), _TL("Load table"), pData->Get_Connection()->Get_Server().c_str(), pData->Get_Value().c_str()), true, true);

	if( pData->Get_Connection()->Table_Load(*pTable, pData->Get_Value()) )
	{
		SG_Get_Data_Manager().Add(pTable);

		g_pData->Show(pTable, 0);

		MSG_General_Add(_TL("okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);
	}
	else
	{
		delete(pTable);

		MSG_General_Add(_TL("failed"), false, false, SG_UI_MSG_STYLE_FAILURE);
	}
}

//---------------------------------------------------------
void CData_Source_ODBC::Table_Delete(const wxTreeItemId &Item)
{
	CData_Source_ODBC_Data	*pData	= Item.IsOk() ? (CData_Source_ODBC_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	if( DLG_Message_Confirm(wxString::Format(wxT("%s [%s]"), _TL("Do you really want to delete the table"), pData->Get_Value().c_str()), _TL("Database Table Deletion")) )
	{
		MSG_General_Add(wxString::Format(wxT("%s: [%s] %s..."), _TL("Deleting table"), pData->Get_Connection()->Get_Server().c_str(), pData->Get_Value().c_str()), true, true);

		if( pData->Get_Connection()->Table_Drop(pData->Get_Value(), false) )
		{
			Delete(Item);

			MSG_General_Add(_TL("okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);
		}
		else
		{
			MSG_General_Add(_TL("failed"), false, false, SG_UI_MSG_STYLE_FAILURE);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
