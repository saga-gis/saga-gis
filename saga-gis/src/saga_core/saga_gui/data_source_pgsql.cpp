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
	TYPE_SOURCE,
	TYPE_TABLE,
	TYPE_SHAPES,
	TYPE_GRID
};

//---------------------------------------------------------
enum
{
	IMG_ROOT	= 0,
	IMG_SRC_CLOSED,
	IMG_SRC_OPENED,
	IMG_TABLE,
	IMG_POINT,
	IMG_POINTS,
	IMG_LINE,
	IMG_POLYGON,
	IMG_GRID
};

//---------------------------------------------------------
enum
{
	IO_PGSQL_Get_Connections	= 0,
	IO_PGSQL_Get_Connection		= 1,
	IO_PGSQL_Del_Connection		= 2,
	IO_PGSQL_Del_Connections	= 3,
	IO_PGSQL_Transaction_Start	= 4,
	IO_PGSQL_Transaction_Stop	= 5,
	IO_PGSQL_Execute_SQL		= 6,

	IO_PGSQL_Table_List			= 10,
	IO_PGSQL_Table_Info			= 11,
	IO_PGSQL_Table_Load			= 12,
	IO_PGSQL_Table_Save			= 13,
	IO_PGSQL_Table_Drop			= 14,
	IO_PGSQL_Table_Query		= 15,

	IO_PGSQL_Shapes_Load		= 20,
	IO_PGSQL_Shapes_Save		= 21,
	IO_PGSQL_Shapes_SRID_Update	= 22,

	IO_PGSQL_Raster_Load		= 30,
	IO_PGSQL_Raster_Save		= 31,
	IO_PGSQL_Raster_SRID_Update	= 32
};


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
	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("io_pgsql"), MODULE);\
	\
	if(	pModule )\
	{\
		CSG_Parameters	P; P.Assign(pModule->Get_Parameters());\
		\
		pModule->Set_Manager(NULL);\
		pModule->Get_Parameters()->Set_Callback(true);\
		pModule->On_Before_Execution();\
		\
		if( (CONDITION) && pModule->Execute() )\
		{\
			bRetVal	= true;\
		}\
		\
		pModule->Get_Parameters()->Assign_Values(&P);\
		pModule->Set_Manager(P.Get_Manager());\
	}\
}

//---------------------------------------------------------
#define SET_PARAMETER(IDENTIFIER, VALUE)	pModule->Get_Parameters()->Set_Parameter(SG_T(IDENTIFIER), VALUE)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	is_Connected	(const CSG_String &Server)
{
	bool		bResult;
	CSG_Table	Connections;

	RUN_MODULE(IO_PGSQL_Get_Connections, bResult, SET_PARAMETER("CONNECTIONS", &Connections));	// CGet_Connections

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CData_Source_PgSQL_Data : public wxTreeItemData
{
public:
    CData_Source_PgSQL_Data(int Type, const CSG_String &Value = SG_T(""), const CSG_String &Server = "")
		: m_Type(Type), m_Value(Value), m_Server(Server)
	{}

	int						Get_Type		(void)	const	{	return( m_Type   );	}
	const CSG_String &		Get_Value		(void)	const	{	return( m_Value  );	}
	const CSG_String &		Get_Server		(void)	const	{	return( m_Server );	}

	bool					is_Connected	(void)	const	{	return( ::is_Connected(m_Server) );	}

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
BEGIN_EVENT_TABLE(CData_Source_PgSQL, wxTreeCtrl)
	EVT_MENU					(ID_CMD_DB_REFRESH				, CData_Source_PgSQL::On_Refresh)
	EVT_MENU					(ID_CMD_DB_SOURCE_OPEN			, CData_Source_PgSQL::On_Source_Open)
	EVT_MENU					(ID_CMD_DB_SOURCE_CLOSE			, CData_Source_PgSQL::On_Source_Close)
	EVT_MENU					(ID_CMD_DB_SOURCE_CLOSE_ALL		, CData_Source_PgSQL::On_Sources_Close)
	EVT_MENU					(ID_CMD_DB_SOURCE_DELETE		, CData_Source_PgSQL::On_Source_Delete)
	EVT_MENU					(ID_CMD_DB_TABLE_OPEN			, CData_Source_PgSQL::On_Table_Open)
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
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_SOURCES);		// IMG_ROOT
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_SOURCE_OFF);		// IMG_SRC_CLOSED
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_SOURCE_ON);		// IMG_SRC_OPENED
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DB_TABLE);			// IMG_TABLE
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POINT);		// IMG_POINT
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POINTS);		// IMG_POINTS
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_LINE);		// IMG_LINE
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POLYGON);	// IMG_POLYGON
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_GRID);				// IMG_GRID

	AddRoot(_TL("PostgreSQL Sources"), IMG_ROOT, IMG_ROOT, new CData_Source_PgSQL_Data(TYPE_ROOT));

	//-----------------------------------------------------
	wxString	Server;

	for(int i=0; CONFIG_Read(CFG_PGSQL_DIR, wxString::Format(CFG_PGSQL_SRC, i), Server); i++)
	{
		Update_Source(AppendItem(GetRootItem(), Server.c_str(), IMG_SRC_CLOSED, IMG_SRC_CLOSED,
			new CData_Source_PgSQL_Data(TYPE_SOURCE, &Server, &Server)
		));
	}

	Update_Sources();
}

//---------------------------------------------------------
CData_Source_PgSQL::~CData_Source_PgSQL(void)
{
	wxTreeItemIdValue	Cookie;
	wxTreeItemId		Item	= GetFirstChild(GetRootItem(), Cookie);

	CONFIG_Delete(CFG_PGSQL_DIR);

	for(int i=0; Item.IsOk(); )
	{
		CData_Source_PgSQL_Data	*pData	= (CData_Source_PgSQL_Data *)GetItemData(Item);

		if( pData && pData->Get_Type() == TYPE_SOURCE )
		{
			CONFIG_Write(CFG_PGSQL_DIR, wxString::Format(CFG_PGSQL_SRC, i++), pData->Get_Server().c_str());
		}

		Item	= GetNextChild(Item, Cookie);
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

	case TYPE_SOURCE:
		Source_Open(event.GetItem());
		break;

	case TYPE_TABLE:
	case TYPE_SHAPES:
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
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_CLOSE);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_SOURCE_DELETE);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_FROM_QUERY);
		}
		break;

	case TYPE_TABLE:
	case TYPE_SHAPES:
	case TYPE_GRID:
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_OPEN);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_RENAME);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_DELETE);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DB_TABLE_INFO);
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
wxTreeItemId CData_Source_PgSQL::Find_Source(const wxString &Server)
{
	wxTreeItemIdValue	Cookie;

	wxTreeItemId	Item	= GetFirstChild(GetRootItem(), Cookie);

	while( Item.IsOk() && Server.Cmp(GetItemText(Item)) )
	{
		Item	= GetNextChild(Item, Cookie);
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
	case TYPE_SOURCE:	Update_Source(Item);	break;
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Update_Sources(void)
{
	Freeze();

	//-----------------------------------------------------
	wxTreeItemIdValue	Cookie;

	wxTreeItemId	Item	= GetFirstChild(GetRootItem(), Cookie);

	while( Item.IsOk() )
	{
		Update_Source(Item);

		Item	= GetNextChild(Item, Cookie);
	}

	//-----------------------------------------------------
	bool		bResult;
	CSG_Table	Connections;

	RUN_MODULE(IO_PGSQL_Get_Connections, bResult, SET_PARAMETER("CONNECTIONS", &Connections));	// CGet_Connections

	for(int i=0; i<Connections.Get_Count(); i++)
	{
		if( !Find_Source(Connections[i].asString(0)) )
		{
			Update_Source(Connections[i].asString(0));
		}
	}

	//-----------------------------------------------------
	SortChildren(GetRootItem());
	Expand      (GetRootItem());

	Thaw();
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

	if( !Item.IsOk() && is_Connected(&Server) )
	{
		Item	= AppendItem(GetRootItem(), Server.c_str(), IMG_SRC_OPENED, IMG_SRC_OPENED,
			new CData_Source_PgSQL_Data(TYPE_SOURCE, &Server, &Server)
		);
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

		bool		bResult;
		CSG_Table	Tables;

		RUN_MODULE(IO_PGSQL_Table_List, bResult,	// CTable_List
				SET_PARAMETER("CONNECTION", pData->Get_Value())
			&&	SET_PARAMETER("TABLES"    , &Tables)
		);

		Tables.Set_Index(1, TABLE_INDEX_Ascending, 0, TABLE_INDEX_Ascending);

		for(int i=0; i<Tables.Get_Count(); i++)
		{
			CSG_String	s(Tables[i].asString(1));

			TSG_Shape_Type	Shape;
			TSG_Vertex_Type	Vertex;

			if( CSG_Shapes_OGIS_Converter::to_ShapeType(s, Shape, Vertex) )
			{
				switch( Shape )
				{
				case SHAPE_TYPE_Point:   Append_Table(Item, Tables[i].asString(0), TYPE_SHAPES, IMG_POINT  ); break;
				case SHAPE_TYPE_Points:  Append_Table(Item, Tables[i].asString(0), TYPE_SHAPES, IMG_POINTS ); break;
				case SHAPE_TYPE_Line:    Append_Table(Item, Tables[i].asString(0), TYPE_SHAPES, IMG_LINE   ); break;
				case SHAPE_TYPE_Polygon: Append_Table(Item, Tables[i].asString(0), TYPE_SHAPES, IMG_POLYGON); break;
				}
			}
			else if( !s.Cmp("RASTER" ) ) Append_Table(Item, Tables[i].asString(0), TYPE_GRID  , IMG_GRID   );
			else if( !s.Cmp("TABLE"  ) ) Append_Table(Item, Tables[i].asString(0), TYPE_TABLE , IMG_TABLE  );
		}

		Expand      (Item);
	}

	Thaw();
}

//---------------------------------------------------------
void CData_Source_PgSQL::Append_Table(const wxTreeItemId &Parent, const SG_Char *Name, int Type, int Image)
{
	CData_Source_PgSQL_Data	*pData	= Parent.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Parent) : NULL; if( pData == NULL )	return;

	AppendItem(Parent, Name, Image, Image, new CData_Source_PgSQL_Data(Type, Name, pData->Get_Server()));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_PgSQL::Source_Open(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	if( pData->Get_Type() == TYPE_ROOT )
	{
		CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("io_pgsql"), IO_PGSQL_Get_Connection);	// CGet_Connection

		if(	pModule && pModule->On_Before_Execution() && DLG_Parameters(pModule->Get_Parameters()) )
		{
			pModule->Execute();
		}
	}
	else if( pData->is_Connected() )
	{
		Update_Source(Item);
	}
	else
	{
		static	wxString	Username = "postgres", Password = "postgres";

		if( DLG_Login(Username, Password) )
		{
			bool	bResult;

			CSG_String	Host	= pData->Get_Server().AfterLast ('[').BeforeFirst(':');
			CSG_String	Port	= pData->Get_Server().AfterLast (':').BeforeFirst(']');
			CSG_String	Name	= pData->Get_Server().BeforeLast('['); Name.Trim(true);

			RUN_MODULE(IO_PGSQL_Get_Connection, bResult,	// CGet_Connection
					SET_PARAMETER("PG_HOST", Host)
				&&	SET_PARAMETER("PG_PORT", Port)
				&&	SET_PARAMETER("PG_NAME", Name)
				&&	SET_PARAMETER("PG_USER", Username)
				&&	SET_PARAMETER("PG_PWD" , Password)
			);

			if( !bResult )
			{
				DLG_Message_Show_Error(_TL("Could not connect to data source."), _TL("Connect to PostgreSQL"));
			}
		}
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Source_Close(const wxTreeItemId &Item, bool bDelete)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	if( pData->is_Connected() )
	{
		CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("io_pgsql"), IO_PGSQL_Del_Connection);	// CDel_Connection

		if( pModule )
		{
			pModule->On_Before_Execution();
			pModule->Get_Parameters()->Set_Parameter("CONNECTION", pData->Get_Server());
			pModule->Execute();
		}
	}

	if( bDelete )
	{
		Delete(Item);
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Sources_Close(void)
{
	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("io_pgsql"), IO_PGSQL_Del_Connections);	// CDel_Connections

	if( pModule )
	{
		pModule->Execute();
	}
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

		bool	bResult;

		RUN_MODULE(IO_PGSQL_Table_Load, bResult,	// CTable_Load
				SET_PARAMETER("CONNECTION", pData->Get_Server())
			&&	SET_PARAMETER("TABLES"    , pData->Get_Value())
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
		CSG_Shapes	*pShapes	= SG_Create_Shapes();

		bool	bResult;

		RUN_MODULE(IO_PGSQL_Shapes_Load, bResult,	// CPGIS_Shapes_Load
				SET_PARAMETER("CONNECTION", pData->Get_Server())
			&&	SET_PARAMETER("TABLES"    , pData->Get_Value())
			&&	SET_PARAMETER("SHAPES"    , pShapes)
		);

		if( bResult )
		{
			SG_Get_Data_Manager().Add(pShapes);

		//	g_pData->Show(pShapes, SG_UI_DATAOBJECT_SHOW_NEW_MAP);
		}
		else
		{
			delete(pShapes);
		}
	}

	//-----------------------------------------------------
	if( pData->Get_Type() == TYPE_GRID )
	{
		CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("io_pgsql"), IO_PGSQL_Raster_Load);	// CPGIS_Raster_Load

		if( pModule )
		{
			pModule->On_Before_Execution();
			pModule->Get_Parameters()->Set_Parameter("CONNECTION", pData->Get_Server());
			pModule->Get_Parameters()->Set_Parameter("TABLES"    , pData->Get_Value ());
			pModule->Execute();
		}
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Table_Rename(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	wxString	Name	= pData->Get_Value().c_str();

	if( DLG_Get_Text(Name, _TL("Rename Table"), _TL("Name")) )
	{
		CSG_String	SQL	= "ALTER TABLE \"" + pData->Get_Value() + "\" RENAME TO \"" + CSG_String(&Name) + "\";";

		bool	bResult;

		RUN_MODULE(IO_PGSQL_Execute_SQL, bResult,
				SET_PARAMETER("CONNECTION", pData->Get_Server())
			&&	SET_PARAMETER("SQL"       , SQL)
		);

		if( bResult )
		{
			SetItemText(Item, Name);
		}
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Table_Info(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("io_pgsql"), IO_PGSQL_Table_Info);

	if( pModule )
	{
		pModule->On_Before_Execution();
		pModule->Get_Parameters()->Set_Parameter("CONNECTION", pData->Get_Server());
		pModule->Get_Parameters()->Set_Parameter("TABLES"    , pData->Get_Value ());
		pModule->Get_Parameters()->Set_Parameter("TABLE"     , DATAOBJECT_NOTSET  );
		
		if( pModule->Execute() )
		{
			g_pData->Show(pModule->Get_Parameters()->Get_Parameter("TABLE")->asTable());
		}
	}
}

//---------------------------------------------------------
void CData_Source_PgSQL::Table_Drop(const wxTreeItemId &Item)
{
	CData_Source_PgSQL_Data	*pData	= Item.IsOk() ? (CData_Source_PgSQL_Data *)GetItemData(Item) : NULL; if( pData == NULL )	return;

	if( DLG_Message_Confirm(wxString::Format(wxT("%s [%s]"), _TL("Do you really want to delete the table"), pData->Get_Value().c_str()), _TL("Table Deletion")) )
	{
		MSG_General_Add(wxString::Format(wxT("%s: [%s] %s..."), _TL("Deleting table"), pData->Get_Server().c_str(), pData->Get_Value().c_str()), true, true);

		bool	bResult;

		RUN_MODULE(IO_PGSQL_Table_Drop, bResult,	// CTable_Drop
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
