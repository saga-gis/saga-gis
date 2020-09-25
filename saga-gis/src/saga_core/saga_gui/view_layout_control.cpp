
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

	EVT_MENU        (ID_CMD_LAYOUT_ITEM_PROPERTIES , CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_ITEM_HIDE       , CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_ITEM_DELETE     , CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_ITEM_MOVE_TOP   , CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_ITEM_MOVE_BOTTOM, CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_ITEM_MOVE_UP    , CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_ITEM_MOVE_DOWN  , CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_IMAGE_SAVE      , CVIEW_Layout_Control::On_Item_Menu)
	EVT_MENU        (ID_CMD_LAYOUT_IMAGE_RESTORE   , CVIEW_Layout_Control::On_Item_Menu)

	EVT_UPDATE_UI   (ID_CMD_LAYOUT_ITEM_HIDE       , CVIEW_Layout_Control::On_Item_Menu_UI)
	EVT_UPDATE_UI   (ID_CMD_LAYOUT_ITEM_MOVE_TOP   , CVIEW_Layout_Control::On_Item_Menu_UI)
	EVT_UPDATE_UI   (ID_CMD_LAYOUT_ITEM_MOVE_BOTTOM, CVIEW_Layout_Control::On_Item_Menu_UI)
	EVT_UPDATE_UI   (ID_CMD_LAYOUT_ITEM_MOVE_UP    , CVIEW_Layout_Control::On_Item_Menu_UI)
	EVT_UPDATE_UI   (ID_CMD_LAYOUT_ITEM_MOVE_DOWN  , CVIEW_Layout_Control::On_Item_Menu_UI)

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
	Do_Destroy();
}

//---------------------------------------------------------
bool CVIEW_Layout_Control::Do_Destroy(void)
{
	if( m_pLayout )
	{
		m_pLayout->Set_Zoom(1.);

		m_pLayout->m_Items.Set_Parent(NULL);

		m_pLayout	= NULL;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SCROLL_RATE		5

#define SCROLL_BAR_DX	wxSystemSettings::GetMetric(wxSYS_VSCROLL_X)
#define SCROLL_BAR_DY	wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y)

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
	int	x, y;	GetViewStart(&x, &y);

	x	= (int)((Zoom * (x * SCROLL_RATE + Center.x) - GetClientSize().x / 2) / SCROLL_RATE);
	y	= (int)((Zoom * (y * SCROLL_RATE + Center.y) - GetClientSize().y / 2) / SCROLL_RATE);

	Freeze();

	if( Set_Zoom(m_Zoom * Zoom) )
	{
		Scroll(x, y);
	}

	Thaw();
}

//---------------------------------------------------------
bool CVIEW_Layout_Control::Set_Scrollbars(void)
{
	wxRect	Size(m_pLayout->Get_PaperSize());

	int	w	= (int)(0.5 + (m_Zoom * Size.GetWidth () + SCROLL_BAR_DX) / SCROLL_RATE);
	int	h	= (int)(0.5 + (m_Zoom * Size.GetHeight() + SCROLL_BAR_DY) / SCROLL_RATE);

	SetScrollbars(SCROLL_RATE, SCROLL_RATE, w, h);

	Set_Rulers();

	Refresh();

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
	Set_Rulers();

	m_pLayout->Draw(dc);

	//wxBitmap	Bmp(GetVirtualSize());
	//wxMemoryDC	dcBmp(Bmp);

	//dcBmp.SetBackground(wxBrush(SYS_Get_Color(wxSYS_COLOUR_APPWORKSPACE))); dcBmp.Clear();
	//m_pLayout->Draw(dcBmp);
	//dcBmp.SelectObject(wxNullBitmap);

	//dc.DrawBitmap(Bmp, 0, 0);
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
	if( event.Moving() || event.Dragging() )
	{
		wxPoint	Position(event.GetPosition());

		((CVIEW_Layout *)GetParent())->Ruler_Set_Position(Position.x, Position.y);
	}

	m_pLayout->m_Items.On_Mouse_Event(event);

	if( event.RightUp() )
	{
		wxMenu	*pMenu	= m_pLayout->Menu_Get_Active();

		if( pMenu )
		{
			PopupMenu(pMenu, event.GetPosition());

			delete(pMenu);
		}
	}
}

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Mouse_Wheel(wxMouseEvent &event)
{
	if( event.ControlDown() )
	{
		Set_Zoom_Centered(event.GetWheelRotation() > 0 ? 1.25 / 1. : 1. / 1.25, event.GetPosition());
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
	m_pLayout->Menu_On_Command(event);
}

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Item_Menu_UI(wxUpdateUIEvent &event)
{
	m_pLayout->Menu_On_Command_UI(event);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
