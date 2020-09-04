
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
	EVT_MOTION			(CVIEW_Layout_Control::On_Mouse_Motion)
	EVT_LEFT_DOWN		(CVIEW_Layout_Control::On_Mouse_LDown)
	EVT_LEFT_UP			(CVIEW_Layout_Control::On_Mouse_LUp)
	EVT_LEFT_DCLICK		(CVIEW_Layout_Control::On_Mouse_LDClick)
	EVT_RIGHT_DOWN		(CVIEW_Layout_Control::On_Mouse_RDown)
	EVT_RIGHT_UP		(CVIEW_Layout_Control::On_Mouse_RUp)
	EVT_RIGHT_DCLICK	(CVIEW_Layout_Control::On_Mouse_RDClick)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Layout_Control::CVIEW_Layout_Control(CVIEW_Layout *pParent)
	: wxScrolledWindow(pParent)
{
	SYS_Set_Color_BG(this, wxSYS_COLOUR_APPWORKSPACE);

	m_pLayout		= pParent;
	m_Zoom			= 1.0;

	SetCursor(IMG_Get_Cursor(ID_IMG_CRS_MAGNIFIER));

	Set_Dimensions();
}

//---------------------------------------------------------
CVIEW_Layout_Control::~CVIEW_Layout_Control(void)
{
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout_Control::OnDraw(wxDC &dc)
{
	dc.SetUserScale(m_Zoom, m_Zoom);
	dc.DrawBitmap(wxBitmap(m_Image), 0, 0, true);

	_Set_Rulers();
}

//---------------------------------------------------------
bool CVIEW_Layout_Control::Refresh_Layout(void)
{
	return( _Draw_Layout(false) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Control::Set_Dimensions(void)
{
	int		w, h, r	= m_pLayout->Get_Info()->Get_Map()->Get_Print_Resolution();
	wxSize	Size(m_pLayout->Get_Info()->Get_PaperSize());

	w	= r * Size.GetWidth();
	h	= r * Size.GetHeight();

	m_Image.Create(w, h);

	w	= (int)(w * m_Zoom) + SCROLL_BAR_DX;
	h	= (int)(h * m_Zoom) + SCROLL_BAR_DY;

	SetScrollbars(SCROLL_RATE, SCROLL_RATE, w / SCROLL_RATE, h / SCROLL_RATE);

	_Draw_Layout(true);

	return( true );
}

//---------------------------------------------------------
bool CVIEW_Layout_Control::_Set_Zoom(double Zoom)
{
	if( Zoom > 0.0 && Zoom != m_Zoom )
	{
		m_Zoom	= Zoom;

		return( Set_Dimensions() );
	}

	return( false );
}

//---------------------------------------------------------
void CVIEW_Layout_Control::_Set_Zoom_Centered(double Zoom, wxPoint Center)
{
	int		x, y;

	GetViewStart(&x, &y);

	x	= (int)((Zoom * (x * SCROLL_RATE + Center.x) - GetClientSize().x / 2) / SCROLL_RATE);
	y	= (int)((Zoom * (y * SCROLL_RATE + Center.y) - GetClientSize().y / 2) / SCROLL_RATE);

	_Set_Zoom(m_Zoom * Zoom);

	Scroll(x, y);
}

//---------------------------------------------------------
bool CVIEW_Layout_Control::Fit_To_Size(int x, int y)
{
	double	dx, dy, r	= m_pLayout->Get_Info()->Get_Map()->Get_Print_Resolution();
	wxSize	sPage(m_pLayout->Get_Info()->Get_PaperSize());

	dx		= (x - SCROLL_BAR_DX) / (r * sPage.x);
	dy		= (y - SCROLL_BAR_DY) / (r * sPage.y);

	return( _Set_Zoom(dx < dy ? dx : dy) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout_Control::_Draw_Layout(bool bEraseBkgrd)
{
	wxBitmap	dc_BMP(m_Image.GetWidth(), m_Image.GetHeight());
	wxMemoryDC	dc;

	dc.SelectObject(dc_BMP);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

	m_pLayout->Get_Info()->Draw(dc);

	dc.SelectObject(wxNullBitmap);

	m_Image	= dc_BMP.ConvertToImage();

	Refresh(bEraseBkgrd);

	return( true );
}

//---------------------------------------------------------
void CVIEW_Layout_Control::_Set_Rulers(void)
{
	int		ix, iy;
	double	d, dx, dy, ax, ay;
	wxSize	sPaper(m_pLayout->Get_Info()->Get_PaperSize());

	d	= (double)sPaper.x / (m_Image.GetWidth()  * m_Zoom);

	dx	= d * GetSize().x;
	dy	= d * GetSize().y;

	GetViewStart(&ix, &iy);

	ax	= d * ix * SCROLL_RATE;
	ay	= d * iy * SCROLL_RATE;

	m_pLayout->Ruler_Refresh(ax, ax + dx, ay, ay + dy);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Mouse_LDown(wxMouseEvent &event)
{
	m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();
}

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Mouse_LUp(wxMouseEvent &event)
{
	_Set_Zoom_Centered(1.5, event.GetPosition());
}

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Mouse_LDClick(wxMouseEvent &event)
{
}

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Mouse_RDown(wxMouseEvent &event)
{
	m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();
}

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Mouse_RUp(wxMouseEvent &event)
{
	_Set_Zoom_Centered(1.0 / 1.5, event.GetPosition());
}

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Mouse_RDClick(wxMouseEvent &event)
{
}

//---------------------------------------------------------
void CVIEW_Layout_Control::On_Mouse_Motion(wxMouseEvent &event)
{
	m_Mouse_Move	= event.GetPosition();

	m_pLayout->Ruler_Set_Position(m_Mouse_Move.x, m_Mouse_Move.y);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
