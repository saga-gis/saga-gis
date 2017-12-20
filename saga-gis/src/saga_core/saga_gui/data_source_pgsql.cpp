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
//                 data_source_pgsql.cpp                 //
//                                                       //
//          Copyright (C) 2013 by Olaf Conrad            //
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

#include "data_source_pgsql.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	TYPE_ROOT	= 0,
	TYPE_SERVER,
	TYPE_SOURCE,
	TYPE_TABLE,
	TYPE_SHAPES,
	TYPE_GRIDS,
	TYPE_GRID
};

//---------------------------------------------------------
enum
{
	IMG_ROOT	= 0,
	IMG_SERVER,
	IMG_SRC_CLOSED,
	IMG_SRC_OPENED,
	IMG_TABLE,
	IMG_POINT,
	IMG_POINTS,
	IMG_LINE,
	IMG_POLYGON,
	IMG_GRIDS,
	IMG_GRID
};

//---------------------------------------------------------
enum
{
	DB_PGSQL_Get_Connections	=  0,
	DB_PGSQL_Get_Connection		=  1,
	DB_PGSQL_Del_Connection		=  2,
	DB_PGSQL_Del_Connections	=  3,
	DB_PGSQL_Transaction_Start	=  4,
	DB_PGSQL_Transaction_Stop	=  5,
	DB_PGSQL_Execute_SQL		=  6,

	DB_PGSQL_Table_List			= 10,
	DB_PGSQL_Table_Info			= 11,
	DB_PGSQL_Table_Load			= 12,
	DB_PGSQL_Table_Save			= 13,
	DB_PGSQL_Table_Drop			= 14,
	DB_PGSQL_Table_Query		= 15,

	DB_PGSQL_Shapes_Load		= 20,
	DB_PGSQL_Shapes_Save		= 21,
	DB_PGSQL_Shapes_SRID_Update	= 22,

	DB_PGSQL_Raster_Load		= 30,
	DB_PGSQL_Raster_Load_Band	= 33,
	DB_PGSQL_Raster_Save		= 31,
	DB_PGSQL_Rasters_Save		= 34,
	DB_PGSQL_Raster_SRID_Update	= 32,

	DB_PGSQL_DB_Create			= 35,
	DB_PGSQL_DB_Drop			= 36
};

//---------------------------------------------------------
static	wxString	g_Username	= "postgres";
static	wxString	g_Password	= "postgres";


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RUN_TOOL(TOOL, bManager, CONDITION)	bool bResult = false;\
{\
	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("db_pgsql", TOOL);\
	\
	if(	pTool )\
	{\
		SG_UI_Msg_Lock(true);\
		pTool->On_Before_Execution();\
		pTool->Settings_Push(bManager ? &SG_Get_Data_Manager() : NULL);\
		bResult	= (CONDITION) && pTool->Execute();\
		pTool->Settings_Pop();\
		SG_UI_Msg_Lock(false);\
	}\
}

//---------------------------------------------------------
#define SET_PARAMETER(IDENTIFIER, VALUE)	pTool->Set_Parameter(IDENTIFIER, VALUE)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	PGSQL_Connect			(const CSG_String &Host, const CSG_String &Port, const CSG_String &DBName)
{
	if( PGSQL_is_Connected(Host, Port, DBName) )
	{
		return( true );
	}

	if( !DLG_Login(g_Username, g_Password, wxString::Format("%s: %s [%s:%s]", _TL("Connect to Database"), DBName.c_str(), Host.c_str(), Port.c_str())) )
	{
		return( false );
	}

	RUN_TOOL(DB_PGSQL_Get_Connection, false,	// CGet_Connection
			SET_PARAMETER("PG_HOST",   Host    )
		&&	SET_PARAMETER("PG_PORT",   Port    )
		&&	SET_PARAMETER("PG_NAME",   DBName  )
		&&	SET_PARAMETER("PG_USER", g_Username)
		&&	SET_PARAMETER("PG_PWD" , g_Password)
	);

	return( bResult );
}

//---------------------------------------------------------
bool	PGSQL_is_Connected		(const CSG_String &Host, const CSG_String &Port, const CSG_String &DBName)
{
	return( PGSQL_is_Connected(DBName + " [" + Host + ":" + Port + "]") );
}

//---------------------------------------------------------
bool	PGSQL_is_Connected		(const CSG_String &Server)
{
	CSG_Table	Connections;

	RUN_TOOL(DB_PGSQL_Get_Connections, false, SET_PARAMETER("CONNECTIONS", &Connections));	// CGet_Connections

	for(int i=0; bResult && i<Connections.Get_Count(); i++)
	{
		if( !Server.Cmp(Connections[i].asString(0)) )
		{
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	PGSQL_Get_Connections	(CSG_Strings &Servers, double vPostGIS)
{
	Servers.Clear();

	CSG_Table	Connections;

	RUN_TOOL(DB_PGSQL_Get_Connections, false, SET_PARAMETER("CONNECTIONS", &Connections));	// CGet_Connections

	for(int i=0; bResult && i<Connections.Get_Count(); i++)
	{
		if( vPostGIS <= 0.0 || vPostGIS <= Connections[i].asDouble("PostGIS") )
		{
			Servers	+= Connections[i].asString(0);
		}
	}

	return( Servers.Get_Count() > 0 );
}

//---------------------------------------------------------
bool	PGSQL_has_Connections	(double vPostGIS)
{
	CSG_Strings	Servers;

	return( PGSQL_Get_Connections(Servers, vPostGIS) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	PGSQL_Save_Table	(CSG_Table *pTable)
{
	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("db_pgsql", DB_PGSQL_Table_Save);

	SG_UI_Msg_Lock(true);

	if(	pTool && pTool->On_Before_Execution() && pTool->Set_Parameter("TABLE", pTable)
	&&  DLG_Parameters(pTool->Get_Parameters()) && pTool->Execute() )
	{
		SG_UI_Msg_Lock(false);

		return( true );
	}

	SG_UI_Msg_Lock(false);

	return( false );
}

//---------------------------------------------------------
bool	PGSQL_Save_Shapes	(CSG_Shapes *pShapes)
{
	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("db_pgsql", DB_PGSQL_Shapes_Save);

	SG_UI_Msg_Lock(true);

	if(	pTool && pTool->On_Before_Execution() && pTool->Set_Parameter("SHAPES", pShapes)
	&&  DLG_Parameters(pTool->Get_Parameters()) && pTool->Execute() )
	{
		SG_UI_Msg_Lock(false);

		return( true );
	}

	SG_UI_Msg_Lock(false);

	return( false );
}

//---------------------------------------------------------
bool	PGSQL_Save_Grid		(CSG_Grid *pGrid)
{
	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("db_pgsql", DB_PGSQL_Raster_Save);

	SG_UI_Msg_Lock(true);

	if(	pTool && pTool->On_Before_Execution() && pTool->Set_Parameter("NAME", pGrid->Get_Name()) )
	{
		pTool->Get_Parameters()->Get_Parameter("GRIDS")->asList()->Del_Items();

		if( pTool->Get_Parameters()->Get_Parameter("GRIDS")->asList()->Add_Item(pGrid)
		&&  DLG_Parameters(pTool->Get_Parameters()) && pTool->Execute() )
		{
			SG_UI_Msg_Lock(false);

			return( true );
		}
	}

	SG_UI_Msg_Lock(false);

	return( false );
}

//---------------------------------------------------------
bool	PGSQL_Save_Grids		(CSG_Grids *pGrids)
{
	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("db_pgsql", DB_PGSQL_Rasters_Save);

	SG_UI_Msg_Lock(true);

	if(	pTool && pTool->On_Before_Execution() && pTool->Set_Parameter("GRIDS", pGrids)
	&&  DLG_Parameters(pTool->Get_Parameters()) && pTool->Execute() )
	{
		SG_UI_Msg_Lock(false);

		return( true );
	}

	SG_UI_Msg_Lock(false);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CData_Source_PgSQL_Data : public wxTreeItemData
{
public:
    CData_Source_PgSQL_Data(int Type, const CSG_String &Value = "", const CSG_String &Server = "", const CSG_String &Username = "", const CSG_String &Password = "")
		: m_Type(Type), m_Value(Value), m_Server(Server), m_Username(Username), m_Password(Password)
	{}

	int						Get_Type		(void)	const	{	return( m_Type     );	}
	const CSG_String &		Get_Value		(void)	const	{	return( m_Value    );	}
	const CSG_String &		Get_Server		(void)	const	{	return( m_Server   );	}

	void					Set_Username	(const SG_Char *Username)	{	m_Username	= Username;	}
	const CSG_String &		Get_Username	(void)	const	{	return( m_Username     );	}
	void					Set_Password	(const SG_Char *Password)	{	m_Password	= Password;	}
	const CSG_String &		Get_Password	(void)	const	{	return( m_Password );	}

	CSG_String				Get_Host		(void)  const	{	return( m_Server.AfterLast ('[').BeforeFirst(':') );	}
	CSG_String				Get_Port		(void)  const	{	return( m_Server.AfterLast (':').BeforeFirst(']') );	}
	CSG_String				Get_DBName		(void)  const	{	CSG_String s(m_Server.BeforeLast('[')); s.Trim(true); return( s );	}

	bool					is_Connected	(void)	const	{	return( PGSQL_is_Connected(m_Server) );	}


private:

    int						m_Type;

	CSG_String				m_Value, m_Server, m_Username, m_Password;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CData_Source_PgSQL, wxTreeCtrl)
	EVT_MENU					(ID_CMD_DB_REFRESH				, CData_Source_PgSQL::On_Refresh)
	EVT_MENU					(ID_CMD_DB_SOURCE_CREATE		, CData_Source_PgSQL::On_Source_Create)
	EVT_MENU					(ID_CMD_DB_SOURCE_DROP			, CData_Source_PgSQL::On_Source_Drop)
	EVT_MENU					(ID_CMD_DB_SOURCE_OPEN			, CData_Source_PgSQL::On_Source_Open)
	EVT_MENU					(ID_CMD_DB_SOURCE_CLOSE			, CData_Source_PgSQL::On_Source_Close)
	EVT_MENU					(ID_CMD_DB_SOURCE_CLOSE_ALL		, CData_Source_PgSQL::On_Sources_Close)
	EVT_MENU					(ID_CMD_DB_SOURCE_DELETE		, CData_Source_PgSQL::On_Source_Delete)
	EVT_MENU					(ID_CMD_DB_TABLE_OPEN			, CData_Source_PgSQL::On_Table_Open)
	EVT_MENU					(ID_CMD_DB_TABLE_FROM_QUERY		, CData_Source_PgSQL::On_Table_From_Query)
	EVT_MENU					(ID_CMD_DB_TABLE_RENAME			, CData_Source_PgSQL::On_Table_Rename)
	EVT_MENU					(ID_CMD_DB_TABLE_INFO			, CData_Source_PgSQL::On_Table_Info)
	EVT_MENU					(ID_CMD_DB_TABLE_DELETE			, CData_Source_PgSQL::On_Table_Drop)

	EVT_TREE_ITEM_ACTIVATED		(ID_WND_DATA_SOURCE_DATABASE	, CData_Source_PgSQL::On_Item_Activated)
	EVT_TREE_ITEM_RIGHT_CLICK	(ID_WND_DATA_SOURCE_DATABASE	, CData_Source_PgSQL::On_Item_RClick)
	EVT_TREE_ITEM_MENU			(ID_WND_DATA_SOURCE_DATABASE	, CData_Source_PgSQL::On_Item_Menu)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CFG_PGSQL_DIR	wxT("/PGSQL")
#define CFG_PGSQL_SRC	wxT("SRC_%03d")

//---------------------------------------------------------
CData_Source_PgSQL::CData_Source_PgSQL(wxWindow *pParent)
	: wxTreeCtrl(pParent, ID_WND_DATA_SOURCE_DATABASE)
{
	AssignImageList(new wxImageList(IMG_SIZE_TREECTRL, IMG_SIZE_TREECTRL, true, 0));
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_SOURCES    );	// IMG_ROOT
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_SOURCES    );	// IMG_SERVER
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_SOURCE_OFF );	// IMG_SRC_CLOSED
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_SOURCE_ON  );	// IMG_SRC_OPENED
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_TABLE      );	// IMG_TABLE
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POINT  );	// IMG_POINT
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POINTS );	// IMG_POINTS
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_LINE   );	// IMG_LINE
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POLYGON);	// IMG_POLYGON
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_GRIDS         );	// IMG_GRIDS
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_GRID          );	// IMG_GRID

	AddRoot(_TL("PostgreSQL Sources"), IMG_ROOT, IMG_ROOT, new CData_Source_PgSQL_Data(TYPE_ROOT));

	//-----------------------------------------------------
	SG_UI_Msg_Lock(true);

	wxString	Server;

	for(int i=0; CONFIG_Read(CFG_PGSQL_DIR, wxString::Format(CFG_PGSQL_SRC, i), Server); i++)
	{
		wxString	User, Password;

		if( Server.Find("|") > 0 )
		{
			User     = Server.AfterFirst ('|').BeforeFirst('|');
			Password = Server.AfterLast  ('|');
			Server   = Server.BeforeFirst('|');
		}

		CData_Source_PgSQL_Data	*pData	= new CData_Source_PgSQL_Data(TYPE_SOURCE, &Server, &Server, &User, &Password);

		Update_Source(AppendItem(Get_Server_Item(Server, true), pData->Get_DBName().c_str(), IMG_SRC_CLOSED, IMG_SRC_CLOSED, pData));
	}

	Update_Sources();

	SG_UI_Msg_Lock(false);
}

//---------------------------------------------------------
CData_Source_PgSQL::~CData_Source_PgSQL(void)
{
	long Reopen	= 0;

	CONFIG_Read("/DATA", "PROJECT_DB_REOPEN", Reopen);

	CONFIG_Delete(CFG_PGSQL_DIR);

	wxTreeItemIdValue srvCookie; wxTreeItemId srvItem = GetFirstChild(GetRootItem(), srvCookie);

	for(int i=0; srvItem.IsOk(); )
	{
		wxTreeItemIdValue Cookie; wxTreeItemId Item = GetFirstChild(srvItem, Cookie);

		while( Item.IsOk() )
		{
			CData_Source_PgSQL_Data	*pData	= (CData_Source_PgSQL_Data *)GetItemData(Item);

			if( pData && pData->Get_Type() == TYPE_SOURCE )
			{
				CSG_String	Connection	= pData->Get_Server().c_str();

				if( Reopen != 0 && pData->is_Connected() && !pData->Get_Username().is_Empty() )	// store user and password
				{
					Connection	+= "|" + pData->Get_Username() + "|" + pData->Get_Password();
				}

				CONFIG_Write(CFG_PGSQL_DIR, wxString::Format(CFG_PGSQL_SRC, i++), Connection.c_str());
			}

			Item	= GetNextChild(Item, Cookie);
		}

		srvItem	= GetNextChild(srvItem, srvCookie);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_PgSQL::Autoconnect(void)
{
	long Reopen	= 0;

	CONFIG_Read("/DATA", "PROJECT_DB_REOPEN", Reopen);

	if( Reopen != 0 )
	{
		wxTreeItemIdValue srvCookie; wxTreeItemId srvItem = GetFirstChild(GetRootItem(), srvCookie);

		while( srvItem.IsOk() )
		{
			wxTreeItemIdValue Cookie; wxTreeItemId Item = GetFirstChild(srvItem, Cookie);

			while( Item.IsOk() )
			{
				CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

				if( pData->Get_Type() == TYPE_SOURCE && !pData->Get_Username().is_Empty() )
				{
					Source_Open(pData, false);
				}

				Item	= GetNextChild(Item, Cookie);
			}

			srvItem	= GetNextChild(srvItem, srvCookie);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_PgSQL::On_Refresh(wxCommandEvent &WXUNUSED(event))
{
	Update_Item(GetSelection());
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Source_Create(wxCommandEvent &WXUNUSED(event))
{
	Source_Create(GetSelection());
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Source_Drop(wxCommandEvent &WXUNUSED(event))
{
	Source_Drop(GetSelection());
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Source_Open(wxCommandEvent &WXUNUSED(event))
{
	Source_Open(GetSelection());
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Source_Close(wxCommandEvent &WXUNUSED(event))
{
	Source_Close(GetSelection(), false);
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Sources_Close(wxCommandEvent &WXUNUSED(event))
{
	Sources_Close();
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Source_Delete(wxCommandEvent &WXUNUSED(event))
{
	Source_Close(GetSelection(), true);
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Table_Open(wxCommandEvent &WXUNUSED(event))
{
	Table_Open(GetSelection());
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Table_From_Query(wxCommandEvent &WXUNUSED(event))
{
	Table_From_Query(GetSelection());
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Table_Rename(wxCommandEvent &WXUNUSED(event))
{
	Table_Rename(GetSelection());
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Table_Info(wxCommandEvent &WXUNUSED(event))
{
	Table_Info(GetSelection());
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Table_Drop(wxCommandEvent &WXUNUSED(event))
{
	Table_Drop(GetSelection());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_PgSQL::On_Item_Activated(wxTreeEvent &event)
{
	CData_Source_PgSQL_Data	*pData	= event.GetItem().IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(event.GetItem()) : NULL; if( pData == NULL )	return;

	switch( pData->Get_Type() )
	{
	case TYPE_ROOT:
		Update_Sources();
		break;

	case TYPE_SERVER:
		Update_Sources(event.GetItem());
		break;

	case TYPE_SOURCE:
		Source_Open(event.GetItem());
		break;

	case TYPE_TABLE:
	case TYPE_SHAPES:
	case TYPE_GRIDS:
	case TYPE_GRID:
		Table_Open(event.GetItem());
		break;
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Item_RClick(wxTreeEvent &event)
{
	SelectItem(event.GetItem());

	event.Skip();
}

//---------------------------------------------------------
void CData_Source_PgSQL::On_Item_Menu(wxTreeEvent &event)
{
	CData_Source_PgSQL_Data	*pData	= event.GetItem().IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(event.GetItem()) : NULL; if( pData == NULL )	return;

	wxMenu	Menu;

	switch( pData->Get_Type() )
	{
	case TYPE_ROOT:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_REFRESH);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_CREATE);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_OPEN);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_CLOSE_ALL);
		break;

	case TYPE_SERVER:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_REFRESH);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_CREATE);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_OPEN);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_CLOSE_ALL);
		break;

	case TYPE_SOURCE:
		if( !pData->is_Connected() )
		{
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_REFRESH);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_OPEN);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_DELETE);
		}
		else
		{
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_REFRESH);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_DROP);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_CLOSE);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_DELETE);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_FROM_QUERY);
		}
		break;

	case TYPE_TABLE:
	case TYPE_SHAPES:
	case TYPE_GRIDS:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_OPEN);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_RENAME);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_DELETE);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_INFO);
		break;

	case TYPE_GRID:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_OPEN);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_RENAME);
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
wxTreeItemId CData_Source_PgSQL::Get_Server_Item(const wxString &Server, bool bCreate)
{
	wxString	Name	= Server.AfterFirst('[').BeforeFirst(']');

	wxTreeItemIdValue Cookie; wxTreeItemId Item = GetFirstChild(GetRootItem(), Cookie);

	while( Item.IsOk() )
	{
		if( !Name.Cmp(GetItemText(Item)) )
		{
			return( Item );
		}

		Item	= GetNextChild(Item, Cookie);
	}

	if( bCreate )
	{
		Item	= AppendItem(GetRootItem(), Name, IMG_SERVER, IMG_SERVER, new CData_Source_PgSQL_Data(TYPE_SERVER, &Name, &Name));

		SortChildren(GetRootItem());
		Expand      (GetRootItem());
	}

	return( Item );
}

//---------------------------------------------------------
wxTreeItemId CData_Source_PgSQL::Find_Source(const wxString &Server)
{
	wxTreeItemId	Item	= Get_Server_Item(Server, false);

	if( Item.IsOk() )
	{
		wxTreeItemIdValue Cookie; Item = GetFirstChild(Item, Cookie);

		wxString	Name	= Server.BeforeLast('['); Name.Trim(true);

		while( Item.IsOk() && Name.Cmp(GetItemText(Item)) )
		{
			Item	= GetNextChild(Item, Cookie);
		}
	}

	return( Item );
}

//---------------------------------------------------------
void CData_Source_PgSQL::Update_Item(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	switch( pData->Get_Type() )
	{
	case TYPE_ROOT:		Update_Sources();		break;
	case TYPE_SERVER:	Update_Sources(Item);	break;
	case TYPE_SOURCE:	Update_Source (Item);	break;
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Update_Sources(const wxTreeItemId &Root)
{
	Freeze();

	//-----------------------------------------------------
	wxTreeItemIdValue Cookie; wxTreeItemId Item = GetFirstChild(Root, Cookie);

	while( Item.IsOk() )
	{
		Update_Source(Item);

		Item	= GetNextChild(Item, Cookie);
	}

	//-----------------------------------------------------
	CSG_Table	Connections;

	RUN_TOOL(DB_PGSQL_Get_Connections, false, SET_PARAMETER("CONNECTIONS", &Connections));	// CGet_Connections

	for(int i=0; i<Connections.Get_Count(); i++)
	{
		if( !Find_Source(Connections[i].asString(0)) )
		{
			Update_Source(Connections[i].asString(0));
		}
	}

	//-----------------------------------------------------
	SortChildren(Root);
	Expand      (Root);

	Thaw();
}

//---------------------------------------------------------
void CData_Source_PgSQL::Update_Sources(void)
{
	wxTreeItemIdValue Cookie; wxTreeItemId Item = GetFirstChild(GetRootItem(), Cookie);

	while( Item.IsOk() )
	{
		Update_Sources(Item);

		Item	= GetNextChild(Item, Cookie);
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Update_Source(const wxString &Server)
{
	if( Server.IsEmpty() )
	{
		Update_Sources();

		return;
	}

	wxTreeItemId	Item	= Find_Source(Server);

	if( !Item.IsOk() && PGSQL_is_Connected(&Server) )
	{
		CData_Source_PgSQL_Data	*pData	= new CData_Source_PgSQL_Data(TYPE_SOURCE, &Server, &Server);

		Item	= AppendItem(Get_Server_Item(Server, true), pData->Get_DBName().c_str(), IMG_SRC_OPENED, IMG_SRC_OPENED, pData);
	}

	Update_Source(Item);
}

//---------------------------------------------------------
void CData_Source_PgSQL::Update_Source(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	if( pData->Get_Type() != TYPE_SOURCE )
	{
		return;
	}

	Freeze();

	DeleteChildren(Item);

	//-----------------------------------------------------
	if( !pData->is_Connected() )
	{
		SetItemImage(Item, IMG_SRC_CLOSED, wxTreeItemIcon_Normal);
		SetItemImage(Item, IMG_SRC_CLOSED, wxTreeItemIcon_Selected);
	}
	else
	{
		SetItemImage(Item, IMG_SRC_OPENED, wxTreeItemIcon_Normal);
		SetItemImage(Item, IMG_SRC_OPENED, wxTreeItemIcon_Selected);

		CSG_Table	Tables;

		RUN_TOOL(DB_PGSQL_Table_List, false,	// CTable_List
				SET_PARAMETER("CONNECTION", pData->Get_Value())
			&&	SET_PARAMETER("TABLES"    , &Tables)
		);

		Tables.Set_Index(1, TABLE_INDEX_Ascending, 0, TABLE_INDEX_Ascending);

		bool	bSkipPostGISTables	= true;

		for(int i=0; i<Tables.Get_Count(); i++)
		{
			CSG_String	s(Tables[i].asString(0));

			if( bSkipPostGISTables == false
			|| (s.CmpNoCase("geography_columns")
			&&  s.CmpNoCase("geometry_columns" )
			&&  s.CmpNoCase("raster_columns"   )
			&&  s.CmpNoCase("raster_overviews" )
			&&  s.CmpNoCase("spatial_ref_sys"  )) )
			{
				s	= Tables[i].asString(1);

				TSG_Shape_Type	Shape;
				TSG_Vertex_Type	Vertex;

				if( CSG_Shapes_OGIS_Converter::to_ShapeType(s, Shape, Vertex) )
				{
					switch( Shape )
					{
					case SHAPE_TYPE_Point  : Append_Table(Item, Tables[i].asString(0), TYPE_SHAPES, IMG_POINT  ); break;
					case SHAPE_TYPE_Points : Append_Table(Item, Tables[i].asString(0), TYPE_SHAPES, IMG_POINTS ); break;
					case SHAPE_TYPE_Line   : Append_Table(Item, Tables[i].asString(0), TYPE_SHAPES, IMG_LINE   ); break;
					case SHAPE_TYPE_Polygon: Append_Table(Item, Tables[i].asString(0), TYPE_SHAPES, IMG_POLYGON); break;
					}
				}
				else if( !s.Cmp("RASTER" ) ) Append_Table(Item, Tables[i].asString(0), TYPE_GRIDS , IMG_GRIDS  );
				else if( !s.Cmp("TABLE"  ) ) Append_Table(Item, Tables[i].asString(0), TYPE_TABLE , IMG_TABLE  );
			}
		}

		Expand(Item);
	}

	Thaw();
}

//---------------------------------------------------------
void CData_Source_PgSQL::Append_Table(const wxTreeItemId &Parent, const SG_Char *Name, int Type, int Image)
{
	CData_Source_PgSQL_Data	*pData	= Parent.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Parent) : NULL; if( pData == NULL )	return;

	wxTreeItemId	Item	= AppendItem(Parent, Name, Image, Image, new CData_Source_PgSQL_Data(Type, Name, pData->Get_Server()));

	if( Type == TYPE_GRIDS )
	{
		CSG_Table	Grids;

		RUN_TOOL(DB_PGSQL_Table_Query, false,	// CTable_Query
				SET_PARAMETER("CONNECTION", pData->Get_Server())
			&&	SET_PARAMETER("TABLES"    , Name)
			&&	SET_PARAMETER("TABLE"     , &Grids)
			&&  SET_PARAMETER("FIELDS"    , SG_T("rid, name"))
		);

		if( bResult )
		{
			for(int i=0; i<Grids.Get_Count(); i++)
			{
				AppendItem(Item, Grids[i].asString(1), IMG_GRID, IMG_GRID,
					new CData_Source_PgSQL_Data(TYPE_GRID, CSG_String::Format("%s:rid=%d", Name, Grids[i].asInt(0)), pData->Get_Server())
				);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CData_Source_PgSQL::Source_Create(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return( false );

	if( pData->Get_Type() == TYPE_ROOT
	||  pData->Get_Type() == TYPE_SERVER )
	{
		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("db_pgsql", DB_PGSQL_DB_Create);

		SG_UI_Msg_Lock(true);

		if(	pTool && pTool->On_Before_Execution() )
		{
			if( pData->Get_Type() == TYPE_SERVER )
			{
				pTool->Set_Parameter("PG_HOST", pData->Get_Host());
				pTool->Set_Parameter("PG_PORT", pData->Get_Port());
			}

			if( DLG_Parameters(pTool->Get_Parameters()) )
			{
				pTool->Execute();
			}
		}

		SG_UI_Msg_Lock(false);
	}

	return( true );
}

//---------------------------------------------------------
bool CData_Source_PgSQL::Source_Drop(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return( false );

	if( !DLG_Login(g_Username, g_Password, _TL("Drop Database")) )
	{
		return( false );
	}

	pData->Set_Username(g_Username);
	pData->Set_Password(g_Password);

	if( pData->Get_Type() == TYPE_SOURCE && pData->is_Connected() )
	{
		RUN_TOOL(DB_PGSQL_DB_Drop, true,	// CDatabase_Drop
				SET_PARAMETER("PG_HOST", pData->Get_Host    ())
			&&	SET_PARAMETER("PG_PORT", pData->Get_Port    ())
			&&	SET_PARAMETER("PG_NAME", pData->Get_DBName  ())
			&&	SET_PARAMETER("PG_USER", pData->Get_Username())
			&&	SET_PARAMETER("PG_PWD" , pData->Get_Password())
		);

		if( bResult )
		{
			Delete(Item);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CData_Source_PgSQL::Source_Open(CData_Source_PgSQL_Data *pData, bool bDialog)
{
	if( bDialog )
	{
		if( !DLG_Login(g_Username, g_Password) )
		{
			return( false );
		}

		pData->Set_Username(g_Username);
		pData->Set_Password(g_Password);
	}

	//-----------------------------------------------------
	MSG_General_Add(wxString::Format("%s: %s...", _TL("Connect to Database"), pData->Get_Server().c_str()), true, true);

	RUN_TOOL(DB_PGSQL_Get_Connection, false,	// CGet_Connection
			SET_PARAMETER("PG_HOST", pData->Get_Host    ())
		&&	SET_PARAMETER("PG_PORT", pData->Get_Port    ())
		&&	SET_PARAMETER("PG_USER", pData->Get_Username())
		&&	SET_PARAMETER("PG_PWD" , pData->Get_Password())
		&&	SET_PARAMETER("PG_LIST", pData->Get_DBName  ())
		&&	SET_PARAMETER("PG_NAME", pData->Get_DBName  ())
	);

	if( bResult )
	{
		MSG_General_Add(_TL("okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	MSG_General_Add(_TL("failed"), false, false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}

//---------------------------------------------------------
void CData_Source_PgSQL::Source_Open(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	if( pData->Get_Type() == TYPE_ROOT
	||  pData->Get_Type() == TYPE_SERVER )
	{
		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("db_pgsql", DB_PGSQL_Get_Connection);	// CGet_Connection

		SG_UI_Msg_Lock(true);

		if(	pTool && pTool->On_Before_Execution() )
		{
			if( pData->Get_Type() == TYPE_SERVER )
			{
				pTool->Set_Parameter("PG_HOST", pData->Get_Host());
				pTool->Set_Parameter("PG_PORT", pData->Get_Port());
			}

			if( DLG_Parameters(pTool->Get_Parameters()) )
			{
				pTool->Execute();
			}
		}

		SG_UI_Msg_Lock(false);
	}
	else if( pData->is_Connected() )
	{
		Update_Source(Item);
	}
	else if( !Source_Open(pData, true) )
	{
		DLG_Message_Show_Error(_TL("Could not connect to data source."), _TL("Connect to PostgreSQL"));
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Source_Close(const wxTreeItemId &Item, bool bDelete)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	if( pData->is_Connected() )
	{
		RUN_TOOL(DB_PGSQL_Del_Connection, true, SET_PARAMETER("CONNECTION", pData->Get_Server()));
	}

	if( bDelete )
	{
		Delete(Item);
	}
	else
	{
		pData->Set_Username(SG_T(""));
		pData->Set_Password(SG_T(""));
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Sources_Close(void)
{
	RUN_TOOL(DB_PGSQL_Del_Connections, true, true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_PgSQL::Table_Open(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	//-----------------------------------------------------
	if( pData->Get_Type() == TYPE_TABLE )
	{
		CSG_Table	*pTable	= SG_Create_Table();

		RUN_TOOL(DB_PGSQL_Table_Load, false,	// CTable_Load
				SET_PARAMETER("CONNECTION", pData->Get_Server())
			&&	SET_PARAMETER("TABLES"    , pData->Get_Value ())
			&&	SET_PARAMETER("TABLE"     , pTable)
		);

		if( bResult )
		{
			SG_Get_Data_Manager().Add(pTable);
			g_pData->Show(pTable, 0);
		}
		else
		{
			delete(pTable);
		}
	}

	//-----------------------------------------------------
	if( pData->Get_Type() == TYPE_SHAPES )
	{
		RUN_TOOL(DB_PGSQL_Shapes_Load, true,	// CPGIS_Shapes_Load
				SET_PARAMETER("CONNECTION", pData->Get_Server())
			&&	SET_PARAMETER("TABLES"    , pData->Get_Value ())
		);
	}

	//-----------------------------------------------------
	if( pData->Get_Type() == TYPE_GRIDS )
	{
		RUN_TOOL(DB_PGSQL_Raster_Load, true,
				SET_PARAMETER("CONNECTION", pData->Get_Server())
			&&	SET_PARAMETER("TABLES"    , pData->Get_Value ())
		);
	}

	//-----------------------------------------------------
	if( pData->Get_Type() == TYPE_GRID )
	{
		RUN_TOOL(DB_PGSQL_Raster_Load, true,
				SET_PARAMETER("CONNECTION", pData->Get_Server())
			&&	SET_PARAMETER("TABLES"    , pData->Get_Value ().BeforeFirst(':'))
			&&	SET_PARAMETER("WHERE"     , pData->Get_Value ().AfterFirst (':'))
		);
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Table_From_Query(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("db_pgsql", DB_PGSQL_Table_Query);

	SG_UI_Msg_Lock(true);

	if(	pTool && pTool->On_Before_Execution() )
	{
		if( pData->Get_Type() == TYPE_SERVER )
		{
			pTool->Set_Parameter("PG_HOST", pData->Get_Host());
			pTool->Set_Parameter("PG_PORT", pData->Get_Port());
		}

		if( DLG_Parameters(pTool->Get_Parameters()) )
		{
			pTool->Execute();
		}
	}

	SG_UI_Msg_Lock(false);
}

//---------------------------------------------------------
void CData_Source_PgSQL::Table_Rename(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	wxString	Name	= GetItemText(Item);

	switch( pData->Get_Type() )
	{
	//-----------------------------------------------------
	case TYPE_GRID:
		if( DLG_Get_Text(Name, _TL("Rename Raster Band"), _TL("Name")) )
		{
			CSG_String	Table	= pData->Get_Value().BeforeFirst(':');
			CSG_String	rid		= pData->Get_Value().AfterFirst (':');
			CSG_String	SQL	= "UPDATE \"" + Table + "\" SET name='" + CSG_String(&Name) + "' WHERE " + rid + ";";

			RUN_TOOL(DB_PGSQL_Execute_SQL, false,
					SET_PARAMETER("CONNECTION", pData->Get_Server())
				&&	SET_PARAMETER("OUTPUT"    , 0)	// none
				&&	SET_PARAMETER("SQL"       , SQL)
			);

			if( bResult )
			{
				SetItemText(Item, Name);
			}
		}
		break;

	//-----------------------------------------------------
	default:
		if( DLG_Get_Text(Name, _TL("Rename Table"), _TL("Name")) )
		{
			CSG_String	SQL	= "ALTER TABLE \"" + pData->Get_Value() + "\" RENAME TO \"" + CSG_String(&Name) + "\";";

			RUN_TOOL(DB_PGSQL_Execute_SQL, false,
					SET_PARAMETER("CONNECTION", pData->Get_Server())
				&&	SET_PARAMETER("OUTPUT"    , 0)	// none
				&&	SET_PARAMETER("SQL"       , SQL)
			);

			if( bResult )
			{
				SetItemText(Item, Name);
			}
		}
		break;
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Table_Info(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	CSG_Table	*pTable	= SG_Create_Table();

	RUN_TOOL(DB_PGSQL_Table_Info, false,	// CTable_Info
			SET_PARAMETER("CONNECTION", pData->Get_Server())
		&&	SET_PARAMETER("TABLES"    , pData->Get_Value ())
		&&	SET_PARAMETER("TABLE"     , pTable)
		&&	SET_PARAMETER("VERBOSE"   , true)
	);

	if( bResult )
	{
		SG_Get_Data_Manager().Add(pTable);
		g_pData->Show(pTable, 0);
	}
	else
	{
		delete(pTable);
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Table_Drop(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	wxString	Name	= GetItemText(Item);

	switch( pData->Get_Type() )
	{
	//-----------------------------------------------------
	case TYPE_GRID:
		if( DLG_Message_Confirm(wxString::Format("%s [%s]", _TL("Do you really want to delete this raster band"), Name.c_str()), _TL("Raster Band Deletion")) )
		{
			MSG_General_Add(wxString::Format("%s: [%s] %s...", _TL("Deleting raster band"), pData->Get_Server().c_str(), Name.c_str()), true, true);

			CSG_String	Table	= pData->Get_Value().BeforeFirst(':');
			CSG_String	rid		= pData->Get_Value().AfterFirst (':');
			CSG_String	SQL	= "DELETE FROM \"" + Table + "\" WHERE " + rid + ";";

			RUN_TOOL(DB_PGSQL_Execute_SQL, false,
					SET_PARAMETER("CONNECTION", pData->Get_Server())
				&&	SET_PARAMETER("OUTPUT"    , 0)	// none
				&&	SET_PARAMETER("SQL"       , SQL)
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
		break;

	//-----------------------------------------------------
	default:
		if( DLG_Message_Confirm(wxString::Format("%s [%s]", _TL("Do you really want to delete the table"), pData->Get_Value().c_str()), _TL("Table Deletion")) )
		{
			MSG_General_Add(wxString::Format("%s: [%s] %s...", _TL("Deleting table"), pData->Get_Server().c_str(), pData->Get_Value().c_str()), true, true);

			RUN_TOOL(DB_PGSQL_Table_Drop, false,	// CTable_Drop
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
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
