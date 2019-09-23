
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
//                   active_legend.cpp                   //
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
#include <wx/clipbrd.h>

#include "res_controls.h"
#include "res_commands.h"

#include "helper.h"

#include "wksp_map.h"
#include "wksp_layer.h"
#include "wksp_layer_legend.h"

#include "active_legend.h"


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
IMPLEMENT_CLASS(CActive_Legend, wxScrolledWindow)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CActive_Legend, wxScrolledWindow)
	EVT_KEY_DOWN  (CActive_Legend::On_Key_Down)
	EVT_RIGHT_DOWN(CActive_Legend::On_Mouse_RDown)

	EVT_MENU     (ID_CMD_DATA_LEGEND_COPY             , CActive_Legend::On_Copy       )
	EVT_MENU     (ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND, CActive_Legend::On_Copy       )

	EVT_MENU     (ID_CMD_DATA_LEGEND_SIZE_INC         , CActive_Legend::On_Size_Inc   )
	EVT_MENU     (ID_CMD_DATA_LEGEND_SIZE_DEC         , CActive_Legend::On_Size_Dec   )

	EVT_MENU     (ID_CMD_DATA_LEGEND_BG_BLACK         , CActive_Legend::On_BG_Black   )
	EVT_UPDATE_UI(ID_CMD_DATA_LEGEND_BG_BLACK         , CActive_Legend::On_BG_Black_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool   CActive_Legend::m_BG_Black = false;

//---------------------------------------------------------
double CActive_Legend::m_Zoom     = 1.;


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CActive_Legend::CActive_Legend(wxWindow *pParent)
	: wxScrolledWindow(pParent, ID_WND_ACTIVE_LEGEND, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE)
{
	SYS_Set_Color_BG(this, wxSYS_COLOUR_WINDOW);

	m_pItem		= NULL;

	m_xScroll	= 0;
	m_yScroll	= 0;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Legend::Set_Item(CWKSP_Base_Item *pItem)
{
	m_pItem	= pItem;

	Refresh();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Legend::On_Key_Down(wxKeyEvent &event)
{
	event.Skip();
}

//---------------------------------------------------------
void CActive_Legend::On_Mouse_RDown(wxMouseEvent &event)
{
	if( !m_pItem || !m_pItem->GetId().IsOk() )
	{
		return;
	}

	wxMenu	Menu(_TL("Legend"));

	CMD_Menu_Add_Item(&Menu, false, m_pItem->Get_Type() == WKSP_ITEM_Map ? ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND : ID_CMD_DATA_LEGEND_COPY);
	Menu.AppendSeparator();
	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DATA_LEGEND_SIZE_INC);
	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DATA_LEGEND_SIZE_DEC);
	Menu.AppendSeparator();
	CMD_Menu_Add_Item(&Menu,  true, ID_CMD_DATA_LEGEND_BG_BLACK);

	PopupMenu(&Menu, event.GetPosition());

	event.Skip();
}

//---------------------------------------------------------
void CActive_Legend::On_Copy(wxCommandEvent &event)
{
	if( !m_pItem || !m_pItem->GetId().IsOk() )
	{
		return;
	}

	if( event.GetId() == ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND )
	{
		((CWKSP_Map   *)m_pItem)->SaveAs_Image_Clipboard(true);

		return;
	}

	if(	m_pItem->Get_Type() != WKSP_ITEM_Grid
	&&	m_pItem->Get_Type() != WKSP_ITEM_Grids
	&&	m_pItem->Get_Type() != WKSP_ITEM_Shapes
	&&	m_pItem->Get_Type() != WKSP_ITEM_TIN
	&&	m_pItem->Get_Type() != WKSP_ITEM_PointCloud )
	{
		return;
	}

	wxPoint		p(5, 5);
	wxSize		s(0, 0);
	wxBitmap	BMP;
	wxMemoryDC	dc;
	
	((CWKSP_Layer *)m_pItem)->Get_Legend()->Draw(dc, m_Zoom, 1.0, p, &s);

	BMP.Create(s.GetWidth() + p.x, s.GetHeight() + p.y);
	dc.SelectObject(BMP);

	if( m_BG_Black )
	{
		dc.SetTextForeground(*wxWHITE      );
		dc.SetBackground    (*wxBLACK_BRUSH);
	}
	else
	{
		dc.SetTextForeground(*wxBLACK      );
		dc.SetBackground    (*wxWHITE_BRUSH);
	}

	dc.Clear();

	((CWKSP_Layer *)m_pItem)->Get_Legend()->Draw(dc, m_Zoom, 1.0, p);

	dc.SelectObject(wxNullBitmap);

	if( wxTheClipboard->Open() )
	{
		wxBitmapDataObject	*pBMP	= new wxBitmapDataObject;
		pBMP->SetBitmap(BMP);
		wxTheClipboard->SetData(pBMP);
		wxTheClipboard->Close();
	}
}

//---------------------------------------------------------
void CActive_Legend::On_Size_Inc(wxCommandEvent &event)
{
	m_Zoom	*= 1.25;

	Refresh();
}

//---------------------------------------------------------
void CActive_Legend::On_Size_Dec(wxCommandEvent &event)
{
	m_Zoom	/= 1.25;

	Refresh();
}

//---------------------------------------------------------
void CActive_Legend::On_BG_Black(wxCommandEvent &event)
{
	m_BG_Black = !m_BG_Black;

	if( m_BG_Black )
	{
		SetBackgroundColour(*wxBLACK);
	}
	else
	{
		SYS_Set_Color_BG(this, wxSYS_COLOUR_WINDOW);
	}

	Refresh();
}

void CActive_Legend::On_BG_Black_UI(wxUpdateUIEvent &event)
{
	event.Check(m_BG_Black);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Legend::OnDraw(wxDC &dc)
{
	wxPoint	p(5, 5);
	wxSize	s(0, 0);

	if( m_pItem && m_pItem->GetId().IsOk() )
	{
		if( m_BG_Black )
		{
			dc.SetTextForeground(*wxWHITE);
		}

		switch( m_pItem->Get_Type() )
		{
		case WKSP_ITEM_Grid      :
		case WKSP_ITEM_Grids     :
		case WKSP_ITEM_Shapes    :
		case WKSP_ITEM_TIN       :
		case WKSP_ITEM_PointCloud:
			((CWKSP_Layer *)m_pItem)->Get_Legend()->Draw(dc, m_Zoom, 1.0, p, &s);
			break;

		case WKSP_ITEM_Map       :
			((CWKSP_Map   *)m_pItem)->Draw_Legend(dc, 1.0, m_Zoom, p, &s);
			break;

		default:
			break;
		}
	}

	s.x	+= p.x + SCROLL_BAR_DX;
	s.y	+= p.y + SCROLL_BAR_DY;

	if(	m_xScroll != s.x || m_yScroll != s.y )
	{
		m_xScroll	= s.x;
		m_yScroll	= s.y;

		SetScrollbars(SCROLL_RATE, SCROLL_RATE, m_xScroll / SCROLL_RATE, m_yScroll / SCROLL_RATE);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
