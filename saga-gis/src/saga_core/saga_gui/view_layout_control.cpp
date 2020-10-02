
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

#include <saga_gdi/sgdi_helper.h>
#include <saga_gdi/sgdi_layout_items.h>

#include "res_commands.h"
#include "res_images.h"

#include "helper.h"

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

	EVT_ERASE_BACKGROUND(CVIEW_Layout_Control::On_EraseBackground)

	EVT_LEFT_DOWN   (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_LEFT_UP     (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_LEFT_DCLICK (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_RIGHT_DOWN  (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_RIGHT_UP    (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_RIGHT_DCLICK(CVIEW_Layout_Control::On_Mouse_Event)
	EVT_MOTION      (CVIEW_Layout_Control::On_Mouse_Event)
	EVT_MOUSEWHEEL  (CVIEW_Layout_Control::On_Mouse_Wheel)

END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Layout_Control::CVIEW_Layout_Control(CVIEW_Layout *pParent, CVIEW_Layout_Info *pLayout)
	: wxScrolledWindow(pParent)
{
	m_Zoom		= 1.;
	m_pLayout	= pLayout;

	m_pLayout->m_Items.Set_Parent(this);

	Set_Scrollbars();
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
#define ZOOM_STEP		1.25

#define SCROLL_RATE		5

#define SCROLL_BAR_DX	wxSystemSettings::GetMetric(wxSYS_VSCROLL_X)
#define SCROLL_BAR_DY	wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y)

//---------------------------------------------------------
bool CVIEW_Layout_Control::Zoom_In(void)
{
	return( Set_Zoom_Centered(ZOOM_STEP / 1.) );
}

bool CVIEW_Layout_Control::Zoom_Out(void)
{
	return( Set_Zoom_Centered(1. / ZOOM_STEP) );
}

bool CVIEW_Layout_Control::Zoom_Original(void)
{
	return( Set_Zoom_Centered(2. / m_Zoom) );
}

bool CVIEW_Layout_Control::Zoom_Full(void)
{
	wxSize	Client(GetClientSize()), Paper(m_pLayout->Get_PaperSize());

	double	dx	= (Client.x - SCROLL_BAR_DX) / (double)Paper.x;
	double	dy	= (Client.y - SCROLL_BAR_DY) / (double)Paper.y;

	return( Set_Zoom(dx < dy ? dx : dy) );
}

//---------------------------------------------------------
bool CVIEW_Layout_Control::Set_Zoom_Centered(double Zooming)
{
	wxPoint	Center(GetClientSize().GetWidth() / 2, GetClientSize().GetHeight() / 2);

	CalcUnscrolledPosition(Center);

	return( Set_Zoom_Centered(Zooming, Center) );
}

//---------------------------------------------------------
bool CVIEW_Layout_Control::Set_Zoom_Centered(double Zooming, wxPoint Center)
{
	int	x, y;	GetViewStart(&x, &y);

	x	= (int)((Zooming * (x * SCROLL_RATE + Center.x) - GetClientSize().x / 2) / SCROLL_RATE);
	y	= (int)((Zooming * (y * SCROLL_RATE + Center.y) - GetClientSize().y / 2) / SCROLL_RATE);

	Freeze();

	if( Set_Zoom(m_Zoom * Zooming) )
	{
		Scroll(x, y);
	}

	Thaw();

	return( true );
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

//---------------------------------------------------------
void CVIEW_Layout_Control::On_EraseBackground(wxEraseEvent &event)
{
	if( event.GetDC() )
	{
		wxDC	&dc	= *event.GetDC();

		dc.SetBrush(SYS_Get_Color(wxSYS_COLOUR_APPWORKSPACE));
		dc.SetPen  (*wxTRANSPARENT_PEN);
		dc.DrawRectangle(dc.GetSize());

		DoPrepareDC(dc);

		m_pLayout->Draw_Paper(dc);
	}
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
		Set_Zoom_Centered(event.GetWheelRotation() > 0 ? ZOOM_STEP : 1. / ZOOM_STEP, event.GetPosition());
	}
	else
	{
		event.Skip();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
