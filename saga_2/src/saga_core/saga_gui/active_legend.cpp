
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "res_controls.h"

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
IMPLEMENT_CLASS(CACTIVE_Legend, wxScrolledWindow)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CACTIVE_Legend, wxScrolledWindow)
	EVT_KEY_DOWN		(CACTIVE_Legend::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CACTIVE_Legend::CACTIVE_Legend(wxWindow *pParent)
	: wxScrolledWindow(pParent, ID_WND_ACTIVE_LEGEND, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE)
{
	SYS_Set_Color_BG(this, wxSYS_COLOUR_WINDOW);

	m_pItem		= NULL;

	m_xScroll	= 0;
	m_yScroll	= 0;

	m_Layout	= LEGEND_LAYOUT_VERTICAL;	// LEGEND_LAYOUT_HORIZONTAL;
	m_Zoom		= 1.0;
}

//---------------------------------------------------------
CACTIVE_Legend::~CACTIVE_Legend(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Legend::Set_Item(CWKSP_Base_Item *pItem)
{
	m_pItem	= pItem;

	Refresh();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Legend::On_Key_Down(wxKeyEvent &event)
{
	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Legend::OnDraw(wxDC &dc)
{
	wxPoint	p(5, 5);
	wxSize	s(0, 0);

	if( m_pItem )
	{
		switch( m_pItem->Get_Type() )
		{
		default:
			break;

		case WKSP_ITEM_Grid:
		case WKSP_ITEM_Shapes:
		case WKSP_ITEM_TIN:
			((CWKSP_Layer *)m_pItem)->Get_Legend()->Draw(dc, m_Zoom, 1.0, p, &s, (m_Layout & LEGEND_LAYOUT_VERTICAL) != 0);
			break;

		case WKSP_ITEM_Map:
			((CWKSP_Map   *)m_pItem)->Draw_Legend(dc, m_Zoom, 1.0, p, &s, m_Layout);
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
