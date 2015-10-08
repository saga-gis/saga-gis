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
    CData_Source_ODBC_Data(int Type, const CSG_String &Value = SG_T(""), const CSG_String &Server = "")
		: m_Type(Type), m_Value(Value), m_Server(Server)
	{}

	void					Set_Type		(int Type)		{	m_Type	= Type;		}

	int						Get_Type		(void)	const	{	return( m_Type   );	}
	const CSG_String &		Get_Value		(void)	const	{	return( m_Value  );	}
	const CSG_String &		Get_Server		(void)	const	{	return( m_Server );	}


private:

    int						m_Type;

	CSG_String				m_Value, m_Server;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CData_Source_ODBC, wxTreeCtrl)
	EVT_MENU					(ID_CMD_DB_REFRESH				, CData_Source_ODBC::On_Refresh)
	EVT_MENU					(ID_CMD_DB_SOURCE_CLOSE_ALL		, CData_Source_ODBC::On_Source_Close_All)
	EVT_MENU					(ID_CMD_DB_SOURCE_CLOSE			, CData_Source_ODBC::On_Source_Close)
	EVT_MENU					(ID_CMD_DB_SOURCE_OPEN			, CData_Source_ODBC::On_Source_Open)
	EVT_MENU					(ID_CMD_DB_TABLE_OPEN			, CData_Source_ODBC::On_Table_Open)
	EVT_MENU					(ID_CMD_DB_TABLE_DELETE			, CData_Source_ODBC::On_Table_Delete)

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
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_SOURCES);	// IMG_ROOT
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_SOURCE_OFF);	// IMG_SRC_CLOSED
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_SOURCE_ON);	// IMG_SRC_OPENED
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_TABLE);		// IMG_TABLE

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
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_REFRESH);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_CLOSE_ALL);
		break;

	case ODBC_ITEM_TYPE_SOURCE_CLOSED:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_REFRESH);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_OPEN);
		break;

	case ODBC_ITEM_TYPE_SOURCE_OPENED:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_REFRESH);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_CLOSE);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_FROM_QUERY);
		break;

	case ODBC_ITEM_TYPE_TABLE:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_OPEN);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_DELETE);
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
#define RUN_MODULE(MODULE, bRetVal, CONDITION)	{\
	\
	bRetVal	= false;\
	\
	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("db_odbc"), MODULE);\
	\
	if(	pModule )\
	{\
		pModule->Settings_Push(NULL);\
		bRetVal	= pModule->On_Before_Execution() && (CONDITION) && pModule->Execute();\
		pModule->Settings_Pop();\
	}\
}

#define SET_PARAMETER(IDENTIFIER, VALUE)	pModule->Get_Parameters()->Set_Parameter(SG_T(IDENTIFIER), VALUE)

//---------------------------------------------------------
bool CData_Source_ODBC::is_Connected(const CSG_String &Server)
{
	bool		bResult;
	CSG_Table	Tables;

	RUN_MODULE(9, bResult, SET_PARAMETER("SERVERS", &Tables) && SET_PARAMETER("CONNECTED", true));

	for(int i=0; bResult && i<Tables.Get_Count(); i++)
	{
		if( !Server.Cmp(Tables[i].asString(0)) )
		{
			return( Tables[i].asInt(1) == 1 );
		}
	}

	return( false );
}

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
void CData_Source_ODBC::Update_Sources(void)
{
	Freeze();

	DeleteChildren(GetRootItem());

	//-----------------------------------------------------
	bool		bResult;
	CSG_Table	Servers;

	RUN_MODULE(9, bResult, SET_PARAMETER("SERVERS", &Servers));

	//-----------------------------------------------------
	for(int i=0; i<Servers.Get_Count(); i++)
	{
		CSG_String	Server	= Servers[i].asString(0);
		int			Image	= Servers[i].asInt(1) == 1 ? IMG_SRC_OPENED : IMG_SRC_CLOSED;
		int			Type	= Servers[i].asInt(1) == 1 ? ODBC_ITEM_TYPE_SOURCE_OPENED : ODBC_ITEM_TYPE_SOURCE_CLOSED;

		Update_Source(AppendItem(GetRootItem(), Server.c_str(), Image, Image,
			new CData_Source_ODBC_Data(Type, Server, Server)
		));
	}

	//-----------------------------------------------------
	SortChildren(GetRootItem());
	Expand      (GetRootItem());

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
void CData_Source_ODBC::Update_Source(const wxTreeItemId &Item)
{
	CData_Source_ODBC_Data	*pData	= Item.IsOk() ? (CData_Source_ODBC_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	if( pData->Get_Type() != ODBC_ITEM_TYPE_SOURCE_CLOSED
	&&  pData->Get_Type() != ODBC_ITEM_TYPE_SOURCE_OPENED )
	{
		return;
	}

	//-----------------------------------------------------
	Freeze();

	DeleteChildren(Item);

	if( is_Connected(pData->Get_Value()) )
	{
		bool		bResult;
		CSG_Table	Tables;

		RUN_MODULE(10, bResult,
				SET_PARAMETER("CONNECTION", pData->Get_Value())
			&&	SET_PARAMETER("TABLES"    , &Tables)
		);

		pData->Set_Type(ODBC_ITEM_TYPE_SOURCE_OPENED);
		SetItemImage(Item, IMG_SRC_OPENED, wxTreeItemIcon_Normal);
		SetItemImage(Item, IMG_SRC_OPENED, wxTreeItemIcon_Selected);

		for(int i=0; i<Tables.Get_Count(); i++)
		{
			AppendItem(Item, Tables[i].asString(0), IMG_TABLE, IMG_TABLE,
				new CData_Source_ODBC_Data(ODBC_ITEM_TYPE_TABLE, Tables[i].asString(0), pData->Get_Server())
			);
		}

		SortChildren(Item);
		Expand      (Item);
	}
	else
	{
		pData->Set_Type(ODBC_ITEM_TYPE_SOURCE_CLOSED);
		SetItemImage(Item, IMG_SRC_CLOSED, wxTreeItemIcon_Normal);
		SetItemImage(Item, IMG_SRC_CLOSED, wxTreeItemIcon_Selected);
	}

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
	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("db_odbc"), 11);

	if( pModule )
	{
		pModule->Execute();
	}
}

//---------------------------------------------------------
void CData_Source_ODBC::Source_Close(const wxTreeItemId &Item)
{
	CData_Source_ODBC_Data	*pData	= Item.IsOk() ? (CData_Source_ODBC_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("db_odbc"), 1);

	if( pModule )
	{
		pModule->On_Before_Execution();
		pModule->Get_Parameters()->Set_Parameter("SERVERS", pData->Get_Value());
		pModule->Execute();
	}
}

//---------------------------------------------------------
void CData_Source_ODBC::Source_Open(const wxTreeItemId &Item)
{
	CData_Source_ODBC_Data	*pData	= Item.IsOk() ? (CData_Source_ODBC_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	wxString	Username, Password;

	if( DLG_Login(Username, Password) )
	{
		MSG_General_Add(wxString::Format(wxT("%s: %s..."), _TL("Open Database Connection"), pData->Get_Value().c_str()), true, true);

		bool	bResult;

		RUN_MODULE(0, bResult,
				SET_PARAMETER("SERVER"  , pData->Get_Value())
			&&	SET_PARAMETER("USERNAME", Username)
			&&	SET_PARAMETER("PASSWORD", Password)
		);

		if( bResult )
		{
			MSG_General_Add(_TL("okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);

			Update_Source(Item);
		}
		else
		{
			MSG_General_Add(_TL("failed"), false, false, SG_UI_MSG_STYLE_FAILURE);
		}
	}
}

//---------------------------------------------------------
void CData_Source_ODBC::Table_Open(const wxTreeItemId &Item)
{
	CData_Source_ODBC_Data	*pData	= Item.IsOk() ? (CData_Source_ODBC_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	CSG_Table	*pTable	= SG_Create_Table();

	MSG_General_Add(wxString::Format(wxT("%s: [%s] %s..."), _TL("Load table"), pData->Get_Server().c_str(), pData->Get_Value().c_str()), true, true);

	bool	bResult;

	RUN_MODULE(5, bResult,
			SET_PARAMETER("CONNECTION", pData->Get_Server())
		&&	SET_PARAMETER("TABLES"    , pData->Get_Value())
		&&	SET_PARAMETER("TABLE"     , pTable)
	);

	if( bResult )
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

	if( DLG_Message_Confirm(wxString::Format(wxT("%s [%s]"), _TL("Do you really want to delete the table"), pData->Get_Value().c_str()), _TL("Table Deletion")) )
	{
		MSG_General_Add(wxString::Format(wxT("%s: [%s] %s..."), _TL("Deleting table"), pData->Get_Server().c_str(), pData->Get_Value().c_str()), true, true);

		bool	bResult;

		RUN_MODULE(7, bResult,
				SET_PARAMETER("CONNECTION", pData->Get_Server())
			&&	SET_PARAMETER("TABLES"    , pData->Get_Value())
		);

		if( bResult )
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
