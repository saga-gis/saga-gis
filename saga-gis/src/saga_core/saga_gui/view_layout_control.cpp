
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
//                VIEW_Layout_Control.cpp                //
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
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/window.h>
#include <wx/dcmemory.h>
#include <wx/image.h>

#include <saga_gdi/sgdi_layout_items.h>

#include "res_commands.h"
#include "res_images.h"

#include "helper.h"
#include "dc_helper.h"

#include "wksp_map.h"

#include "view_layout.h"
#include "view_layout_info.h"
#include "view_layout_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SCROLL_RATE		5

#define SCROLL_BAR_DX	wxSystemSettings::GetMetric(wxSYS_VSCROLL_X)
#define SCROLL_BAR_DY	wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Layout_Control, wxScrolledWindow)
	EVT_TRACKER_CHANGED(wxID_ANY, CVIEW_Layout_Control::On_Tracker_Changed)

	EVT_LEFT_DOWN   (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_LEFT_UP     (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_LEFT_DCLICK (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_RIGHT_DOWN  (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_RIGHT_UP    (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_RIGHT_DCLICK(CVIEW_Layout_Control::On_Mouse_Event)
	EVT_MOTION      (CVIEW_Layout_Control::On_Mouse_Event)

	EVT_MOUSEWHEEL  (CVIEW_Layout_Control::On_Mouse_Wheel)

	EVT_MENU        (ID_CMD_LAYOUT_PROPERTIES , CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_DELETE     , CVIEW_Layout_Control::On_Item_Menu)

	EVT_MENU        (ID_CMD_LAYOUT_MOVE_TOP   , CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_MOVE_BOTTOM, CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_MOVE_UP    , CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_MOVE_DOWN  , CVIEW_Layout_Control::On_Item_Menu)

	EVT_UPDATE_UI   (ID_CMD_LAYOUT_MOVE_TOP   , CVIEW_Layout_Control::On_Item_Menu_UI)
	EVT_UPDATE_UI   (ID_CMD_LAYOUT_MOVE_BOTTOM, CVIEW_Layout_Control::On_Item_Menu_UI)
	EVT_UPDATE_UI   (ID_CMD_LAYOUT_MOVE_UP    , CVIEW_Layout_Control::On_Item_Menu_UI)
	EVT_UPDATE_UI   (ID_CMD_LAYOUT_MOVE_DOWN  , CVIEW_Layout_Control::On_Item_Menu_UI)

END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Layout_Control::CVIEW_Layout_Control(CVIEW_Layout *pParent, CVIEW_Layout_Info *pLayout)
	: wxScrolledWindow(pParent)
{
	SYS_Set_Color_BG(this, wxSYS_COLOUR_APPWORKSPACE);

	m_Zoom		= 1.;

	m_pLayout	= pLayout;
	m_pLayout->m_Items.Set_Parent(this);

	Set_Scrollbars();

//	SetCursor(IMG_Get_Cursor(ID_IMG_CRS_MAGNIFIER));
}

//---------------------------------------------------------
CVIEW_Layout_Control::~CVIEW_Layout_Control(void)
{
	m_pLayout->Set_Zoom(1.);

	m_pLayout->m_Items.Set_Parent(NULL);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Control::Fit_To_Size(int x, int y)
{
	wxSize	Size(m_pLayout->Get_PaperSize());

	double	dx	= (x - SCROLL_BAR_DX) / (double)Size.x;
	double	dy	= (y - SCROLL_BAR_DY) / (double)Size.y;

	return( Set_Zoom(dx < dy ? dx : dy) );
}

//---------------------------------------------------------
bool CVIEW_Layout_Control::Set_Zoom(double Zoom)
{
	if( Zoom > 0.4 && Zoom != m_Zoom )
	{
		m_pLayout->Set_Zoom(m_Zoom = Zoom);

		return( Set_Scrollbars() );
	}

	return( false );
}

//---------------------------------------------------------
void CVIEW_Layout_Control::Set_Zoom_Centered(double Zoom, wxPoint Center)
{
	int		x, y;

	GetViewStart(&x, &y);

	x	= (int)((Zoom * (x * SCROLL_RATE + Center.x) - GetClientSize().x / 2) / SCROLL_RATE);
	y	= (int)((Zoom * (y * SCROLL_RATE + Center.y) - GetClientSize().y / 2) / SCROLL_RATE);

	Set_Zoom(m_Zoom * Zoom);

	Scroll(x, y);
}

//---------------------------------------------------------
bool CVIEW_Layout_Control::Set_Scrollbars(void)
{
	wxRect	Size(m_pLayout->Get_PaperSize());

	int	w	= (int)(0.5 + (m_Zoom * Size.GetWidth () + SCROLL_BAR_DX) / SCROLL_RATE);
	int	h	= (int)(0.5 + (m_Zoom * Size.GetHeight() + SCROLL_BAR_DY) / SCROLL_RATE);

	SetScrollbars(SCROLL_RATE, SCROLL_RATE, w, h);

	return( true );
}

//---------------------------------------------------------
void CVIEW_Layout_Control::Set_Rulers(void)
{
	double	d	= 1. / m_Zoom;
	double	dx	= d * GetSize().x;
	double	dy	= d * GetSize().y;

	int		ix, iy;

	GetViewStart(&ix, &iy);

	double	ax	= d * ix * SCROLL_RATE;
	double	ay	= d * iy * SCROLL_RATE;

	((CVIEW_Layout *)GetParent())->Ruler_Refresh(ax, ax + dx, ay, ay + dy);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout_Control::OnDraw(wxDC &dc)
{
	wxRect	r(m_pLayout->Get_PaperSize());

	r.width  *= m_Zoom;
	r.height *= m_Zoom;

	dc.SetBrush(*wxWHITE_BRUSH);
	dc.DrawRectangle(r);

	m_pLayout->Draw(dc);

	Set_Rulers();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Tracker_Changed(wxCommandEvent &event)
{
	m_pLayout->m_Items.On_Tracker_Changed();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Mouse_Event(wxMouseEvent &event)
{
	if( event.LeftDown() || event.RightDown() )
	{
		m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();
	}

	if( event.Moving() || event.Dragging() )
	{
		m_Mouse_Move	= event.GetPosition();

		((CVIEW_Layout *)GetParent())->Ruler_Set_Position(m_Mouse_Move.x, m_Mouse_Move.y);
	}

	m_pLayout->m_Items.On_Mouse_Event(event);

	if( event.RightUp() )
	{
		if( m_pLayout->m_Items.Get_Active() && m_pLayout->m_Items.Get_Count() > 1 )
		{
			wxMenu	Menu;

			if( m_pLayout->Can_Delete() )
			{
				CMD_Menu_Add_Item(&Menu, false, ID_CMD_LAYOUT_DELETE);
				Menu.AppendSeparator();
			}

			CMD_Menu_Add_Item(&Menu, false, ID_CMD_LAYOUT_MOVE_TOP   );
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_LAYOUT_MOVE_BOTTOM);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_LAYOUT_MOVE_UP    );
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_LAYOUT_MOVE_DOWN  );
			Menu.AppendSeparator();
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_LAYOUT_PROPERTIES );

			PopupMenu(&Menu, event.GetPosition());
		}
	}
}

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Mouse_Wheel(wxMouseEvent &event)
{
	if( event.ControlDown() )
	{
		Set_Zoom_Centered(event.GetWheelRotation() > 0 ? 1.2 / 1. : 1. / 1.2, event.GetPosition());
	}
	else
	{
		event.Skip();
	}
}



///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Item_Menu(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_LAYOUT_MOVE_TOP   : m_pLayout->m_Items.Active_Move_Top   (); break;
	case ID_CMD_LAYOUT_MOVE_BOTTOM: m_pLayout->m_Items.Active_Move_Bottom(); break;
	case ID_CMD_LAYOUT_MOVE_UP    : m_pLayout->m_Items.Active_Move_Up    (); break;
	case ID_CMD_LAYOUT_MOVE_DOWN  : m_pLayout->m_Items.Active_Move_Down  (); break;

	case ID_CMD_LAYOUT_PROPERTIES : m_pLayout->m_Items.Active_Properties (); break;
	case ID_CMD_LAYOUT_DELETE     :
		if( m_pLayout->m_Items.Del(m_pLayout->m_Items.Get_Active()) )
		{
			Refresh(false);
		}
		break;
	}
}

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Item_Menu_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_LAYOUT_MOVE_TOP   : event.Enable(!m_pLayout->m_Items.Active_is_Top   ()); break;
	case ID_CMD_LAYOUT_MOVE_BOTTOM: event.Enable(!m_pLayout->m_Items.Active_is_Bottom()); break;
	case ID_CMD_LAYOUT_MOVE_UP    : event.Enable(!m_pLayout->m_Items.Active_is_Top   ()); break;
	case ID_CMD_LAYOUT_MOVE_DOWN  : event.Enable(!m_pLayout->m_Items.Active_is_Bottom()); break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
