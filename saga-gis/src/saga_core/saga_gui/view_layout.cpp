
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
//                   VIEW_Layout.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/window.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"

#include "helper.h"

#include "wksp_map.h"

#include "view_ruler.h"

#include "view_layout.h"
#include "view_layout_info.h"
#include "view_layout_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Layout, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Layout, CVIEW_Base)

	EVT_KEY_DOWN (CVIEW_Layout::On_Key_Event)

	EVT_SIZE     (CVIEW_Layout::On_Size)

	EVT_MENU     (ID_CMD_LAYOUT_LOAD            , CVIEW_Layout::On_Load         )
	EVT_MENU     (ID_CMD_LAYOUT_SAVE            , CVIEW_Layout::On_Save         )
	EVT_MENU     (ID_CMD_LAYOUT_PROPERTIES      , CVIEW_Layout::On_Properties   )
	EVT_MENU     (ID_CMD_LAYOUT_PAGE_SETUP      , CVIEW_Layout::On_Page_Setup   )
	EVT_MENU     (ID_CMD_LAYOUT_PRINT_SETUP     , CVIEW_Layout::On_Print_Setup  )
	EVT_MENU     (ID_CMD_LAYOUT_PRINT_PREVIEW   , CVIEW_Layout::On_Print_Preview)
	EVT_MENU     (ID_CMD_LAYOUT_PRINT           , CVIEW_Layout::On_Print        )
	EVT_MENU     (ID_CMD_LAYOUT_EXPORT          , CVIEW_Layout::On_Export       )
	EVT_MENU     (ID_CMD_LAYOUT_TO_CLIPBOARD    , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ZOOM_IN         , CVIEW_Layout::On_Zoom         )
	EVT_MENU     (ID_CMD_LAYOUT_ZOOM_OUT        , CVIEW_Layout::On_Zoom         )
	EVT_MENU     (ID_CMD_LAYOUT_ZOOM_FULL       , CVIEW_Layout::On_Zoom         )
	EVT_MENU     (ID_CMD_LAYOUT_ZOOM_ORIGINAL   , CVIEW_Layout::On_Zoom         )

	EVT_MENU     (ID_CMD_LAYOUT_ITEM_MAP        , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_LEGEND     , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_SCALEBAR   , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_SCALE      , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_LABEL      , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_TEXT       , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_IMAGE      , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_PASTE      , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_PROPERTIES , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_HIDE       , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_DELETE     , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_MOVE_TOP   , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_MOVE_BOTTOM, CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_MOVE_UP    , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_ITEM_MOVE_DOWN  , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_IMAGE_SAVE      , CVIEW_Layout::On_Command      )
	EVT_MENU     (ID_CMD_LAYOUT_IMAGE_RESTORE   , CVIEW_Layout::On_Command      )

	EVT_UPDATE_UI(ID_CMD_LAYOUT_ITEM_MAP        , CVIEW_Layout::On_Command_UI   )
	EVT_UPDATE_UI(ID_CMD_LAYOUT_ITEM_LEGEND     , CVIEW_Layout::On_Command_UI   )
	EVT_UPDATE_UI(ID_CMD_LAYOUT_ITEM_SCALEBAR   , CVIEW_Layout::On_Command_UI   )
	EVT_UPDATE_UI(ID_CMD_LAYOUT_ITEM_SCALE      , CVIEW_Layout::On_Command_UI   )
	EVT_UPDATE_UI(ID_CMD_LAYOUT_ITEM_PASTE      , CVIEW_Layout::On_Command_UI   )
	EVT_UPDATE_UI(ID_CMD_LAYOUT_ITEM_HIDE       , CVIEW_Layout::On_Command_UI   )
	EVT_UPDATE_UI(ID_CMD_LAYOUT_ITEM_MOVE_TOP   , CVIEW_Layout::On_Command_UI   )
	EVT_UPDATE_UI(ID_CMD_LAYOUT_ITEM_MOVE_BOTTOM, CVIEW_Layout::On_Command_UI   )
	EVT_UPDATE_UI(ID_CMD_LAYOUT_ITEM_MOVE_UP    , CVIEW_Layout::On_Command_UI   )
	EVT_UPDATE_UI(ID_CMD_LAYOUT_ITEM_MOVE_DOWN  , CVIEW_Layout::On_Command_UI   )

END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Layout::CVIEW_Layout(CVIEW_Layout_Info *pLayout)
	: CVIEW_Base(pLayout->Get_Map(), ID_VIEW_LAYOUT, _TL("Layout"), ID_IMG_WND_LAYOUT, false)
{
	SetTitle(wxString::Format("%s [%s]", pLayout->Get_Map()->Get_Name().c_str(), _TL("Layout")));

	SYS_Set_Color_BG(this, wxSYS_COLOUR_3DFACE);

	m_pRuler_X	= new CVIEW_Ruler(this, RULER_HORIZONTAL|RULER_EDGE_SUNKEN);
	m_pRuler_Y	= new CVIEW_Ruler(this, RULER_VERTICAL  |RULER_EDGE_SUNKEN);

	m_pLayout	= pLayout;

	m_pControl	= new CVIEW_Layout_Control(this, pLayout);
	m_pControl->SetSize(GetClientSize());

	Do_Show();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Layout::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu, *pSubMenu;

	CMD_Menu_Add_Item(pMenu   , false, ID_CMD_LAYOUT_LOAD);
	CMD_Menu_Add_Item(pMenu   , false, ID_CMD_LAYOUT_SAVE);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu   , false, ID_CMD_LAYOUT_PROPERTIES);
	CMD_Menu_Add_Item(pMenu   , false, ID_CMD_LAYOUT_PAGE_SETUP);
//	CMD_Menu_Add_Item(pMenu   , false, ID_CMD_LAYOUT_PRINT_SETUP);
	CMD_Menu_Add_Item(pMenu   , false, ID_CMD_LAYOUT_PRINT_PREVIEW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu   , false, ID_CMD_LAYOUT_PRINT);
	CMD_Menu_Add_Item(pMenu   , false, ID_CMD_LAYOUT_EXPORT);
	CMD_Menu_Add_Item(pMenu   , false, ID_CMD_LAYOUT_TO_CLIPBOARD);
	pMenu->AppendSeparator();

	pSubMenu	= new wxMenu;
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_LAYOUT_ZOOM_IN);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_LAYOUT_ZOOM_OUT);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_LAYOUT_ZOOM_FULL);
//	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_LAYOUT_ZOOM_ORIGINAL);
	pMenu->AppendSubMenu(pSubMenu, _TL("Zoom"));

	pSubMenu	= new wxMenu;
//	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_LAYOUT_ITEM_MAP);
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_LAYOUT_ITEM_LEGEND);
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_LAYOUT_ITEM_SCALEBAR);
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_LAYOUT_ITEM_SCALE);
	pMenu->AppendSubMenu(pSubMenu, _TL("Show"));

	pSubMenu	= new wxMenu;
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_LAYOUT_ITEM_LABEL);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_LAYOUT_ITEM_TEXT);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_LAYOUT_ITEM_IMAGE);
	pSubMenu->AppendSeparator();
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_LAYOUT_ITEM_PASTE);
	pMenu->AppendSubMenu(pSubMenu, _TL("Add"));

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Layout::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_LAYOUT);

//	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_PROPERTIES);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_PAGE_SETUP);
//	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_PRINT_SETUP);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_PRINT_PREVIEW);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_PRINT);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_EXPORT);
//	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_TO_CLIPBOARD);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_ZOOM_IN);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_ZOOM_OUT);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_ZOOM_FULL);
//	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_ZOOM_ORIGINAL);

	CMD_ToolBar_Add(pToolBar, _TL("Layout"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout::Do_Destroy(void)
{
	m_pControl->Do_Destroy();

	CVIEW_Base::Do_Destroy();
}

//---------------------------------------------------------
void CVIEW_Layout::Do_Update(void)
{
	m_pControl->Refresh(true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout::On_Key_Event(wxKeyEvent &event)
{
	if( event.GetKeyCode() == WXK_F5 )
	{
		m_pControl->Refresh();
	}
	else if( event.GetKeyCode() == WXK_DELETE )
	{
		if( m_pLayout->Can_Delete() && m_pLayout->m_Items.Del(m_pLayout->m_Items.Get_Active()) )
		{
			m_pControl->Refresh(false);
		}
	}
	else if( (event.ControlDown() && event.GetUnicodeKey() == 'V')
		||   (event.ShiftDown  () && event.GetKeyCode   () == WXK_INSERT) )
	{
		m_pLayout->Clipboard_Paste();
	}
	else
	{
		m_pLayout->m_Items.On_Key_Event(event);
	}
}

//---------------------------------------------------------
void CVIEW_Layout::On_Size(wxSizeEvent &event)
{
	int	A	= 1;
	int	B	= 20;
	int	d	= B - 4 * A;
	int	dX	= GetClientSize().x - B;
	int	dY	= GetClientSize().y - B;

	Freeze();

	m_pRuler_Y->SetSize(wxRect(A, B, d , dY));
	m_pRuler_X->SetSize(wxRect(B, A, dX, d ));
	m_pControl->SetSize(wxRect(B, B, dX, dY));

	m_pControl->Zoom_Full();

	Thaw();

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout::Ruler_Set_Position(int x, int y)
{
	m_pRuler_X->Set_Position(x);
	m_pRuler_Y->Set_Position(y);
}

//---------------------------------------------------------
void CVIEW_Layout::Ruler_Refresh(double xMin, double xMax, double yMin, double yMax)
{
	wxRect	r(m_pLayout->Get_Margins());

	m_pRuler_X->Set_Range_Core(r.GetLeft(), r.GetLeft() + r.GetWidth ());
	m_pRuler_Y->Set_Range_Core(r.GetTop (), r.GetTop () + r.GetHeight());

	m_pRuler_X->Set_Range(xMin, xMax);
	m_pRuler_Y->Set_Range(yMin, yMax);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout::On_Load(wxCommandEvent &event)
{
	m_pLayout->Load();
}

//---------------------------------------------------------
void CVIEW_Layout::On_Save(wxCommandEvent &event)
{
	m_pLayout->Save();
}

//---------------------------------------------------------
void CVIEW_Layout::On_Properties(wxCommandEvent &event)
{
	if( m_pLayout->Properties() )
	{
		m_pControl->Refresh();
	}
}

//---------------------------------------------------------
void CVIEW_Layout::On_Page_Setup(wxCommandEvent &event)
{
	if( m_pLayout->Page_Setup() )
	{
		m_pControl->Set_Scrollbars();
	}
}

//---------------------------------------------------------
void CVIEW_Layout::On_Print_Setup(wxCommandEvent &event)
{
	if( m_pLayout->Print_Setup() )
	{
		m_pControl->Set_Scrollbars();
	}
}

//---------------------------------------------------------
void CVIEW_Layout::On_Print_Preview(wxCommandEvent &event)
{
	m_pLayout->Print_Preview();
}

//---------------------------------------------------------
void CVIEW_Layout::On_Print(wxCommandEvent &event)
{
	m_pLayout->Print();
}

//---------------------------------------------------------
void CVIEW_Layout::On_Export(wxCommandEvent &event)
{
	m_pLayout->Export();
}

//---------------------------------------------------------
void CVIEW_Layout::On_Zoom(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_LAYOUT_ZOOM_IN      : m_pControl->Zoom_In      (); break;
	case ID_CMD_LAYOUT_ZOOM_OUT     : m_pControl->Zoom_Out     (); break;
	case ID_CMD_LAYOUT_ZOOM_FULL    : m_pControl->Zoom_Full    (); break;
	case ID_CMD_LAYOUT_ZOOM_ORIGINAL: m_pControl->Zoom_Original(); break;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout::On_Command(wxCommandEvent &event)
{
	m_pLayout->Menu_On_Command(event);
}

//---------------------------------------------------------
void CVIEW_Layout::On_Command_UI(wxUpdateUIEvent &event)
{
	m_pLayout->Menu_On_Command_UI(event);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
