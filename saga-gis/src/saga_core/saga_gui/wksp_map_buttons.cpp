
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
//                 wksp_map_buttons.cpp                  //
//                                                       //
//          Copyright (C) 2006 by Olaf Conrad            //
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
#include <wx/tooltip.h>

#include <saga_api/saga_api.h>
#include <saga_gdi/sgdi_helper.h>

#include "res_controls.h"
#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "active.h"

#include "wksp_map_control.h"
#include "wksp_map_manager.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"

#include "wksp_map_buttons.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CWKSP_Map_Button, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP_Map_Button, wxPanel)
	EVT_PAINT      (CWKSP_Map_Button::On_Paint)
	EVT_KEY_DOWN   (CWKSP_Map_Button::On_Key)
	EVT_LEFT_DOWN  (CWKSP_Map_Button::On_Mouse_LDown)
	EVT_LEFT_DCLICK(CWKSP_Map_Button::On_Mouse_LDClick)
	EVT_RIGHT_DOWN (CWKSP_Map_Button::On_Mouse_RDown)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_Button::CWKSP_Map_Button(wxWindow *pParent, CWKSP_Map *pMap)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER)
{
	m_pMap	= pMap;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_Button::On_Paint(wxPaintEvent &event)
{
	if( g_pMaps->Exists(m_pMap) )
	{
		if( !GetToolTip() || GetToolTip()->GetTip().Cmp(m_pMap->Get_Name()) )
		{
			SetToolTip(m_pMap->Get_Name());
		}

		//-------------------------------------------------
		wxPaintDC	dc(this);

		wxRect		r(GetClientRect());

		dc.DrawBitmap(m_pMap->Get_Thumbnail(r.GetWidth() - 1, r.GetHeight() - 1),
			r.GetLeft(), r.GetTop(), true
		);

		//-------------------------------------------------
		if( m_pMap->is_Selected() )
		{
			dc.SetPen(wxPen(Get_Color_asWX(g_pMaps->Get_Parameter("THUMBNAIL_SELCOLOR")->asColor())));

			Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);	r.Deflate(1);
			Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);	r.Deflate(1);
			Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_Button::On_Key(wxKeyEvent &event)
{
	wxCommandEvent	Command;

	switch( event.GetKeyCode() )
	{
	case WXK_RETURN:
		Command.SetId(ID_CMD_WKSP_ITEM_RETURN);
		g_pMap_Ctrl->On_Command(Command);
		break;

	case WXK_DELETE:
		Command.SetId(ID_CMD_WKSP_ITEM_CLOSE);
		g_pMap_Ctrl->On_Command(Command);
		break;

	default:
		break;
	}
}

//---------------------------------------------------------
void CWKSP_Map_Button::On_Mouse_LDown(wxMouseEvent &event)
{
	_Set_Active();
}

//---------------------------------------------------------
void CWKSP_Map_Button::On_Mouse_LDClick(wxMouseEvent &event)
{
	if( _Set_Active() )
	{
		m_pMap->On_Command(ID_CMD_WKSP_ITEM_RETURN);
	}
}

//---------------------------------------------------------
void CWKSP_Map_Button::On_Mouse_RDown(wxMouseEvent &event)
{
	if( _Set_Active() )
	{
		wxMenu	*pMenu	= m_pMap->Get_Menu();

		if( pMenu )
		{
			GetParent()->PopupMenu(pMenu, GetParent()->ScreenToClient(ClientToScreen(event.GetPosition())));

			delete(pMenu);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Button::_Set_Active(void)
{
	if( g_pMaps->Exists(m_pMap) )
	{
		SetFocus();

		return( g_pMap_Ctrl->Set_Item_Selected(m_pMap) );
	}

	m_pMap	= NULL;

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define THUMBNAIL_DIST	5
#define SCROLL_RATE		5
#define SCROLL_BAR_DX	wxSystemSettings::GetMetric(wxSYS_VSCROLL_X)
#define SCROLL_BAR_DY	wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y)

//---------------------------------------------------------
CWKSP_Map_Buttons	*g_pMap_Buttons	= NULL;


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CWKSP_Map_Buttons, wxScrolledWindow)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP_Map_Buttons, wxScrolledWindow)
	EVT_SIZE      (CWKSP_Map_Buttons::On_Size)
	EVT_LEFT_DOWN (CWKSP_Map_Buttons::On_Mouse_LDown)
	EVT_RIGHT_DOWN(CWKSP_Map_Buttons::On_Mouse_RDown)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_Buttons::CWKSP_Map_Buttons(wxWindow *pParent)
	: wxScrolledWindow(pParent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE)
{
	g_pMap_Buttons	= this;

	m_xScroll		= 0;
	m_yScroll		= 0;

	m_Items			= NULL;
	m_nItems		= 0;
}

//---------------------------------------------------------
CWKSP_Map_Buttons::~CWKSP_Map_Buttons(void)
{
	g_pMap_Buttons	= NULL;

	_Del_Items();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_Buttons::On_Size(wxSizeEvent &event)
{
	_Set_Positions();

	event.Skip();
}

//---------------------------------------------------------
void CWKSP_Map_Buttons::On_Mouse_LDown(wxMouseEvent &event)
{
	g_pMap_Ctrl->Set_Item_Selected(g_pMaps);
}

//---------------------------------------------------------
void CWKSP_Map_Buttons::On_Mouse_RDown(wxMouseEvent &event)
{
	wxMenu	*pMenu	= g_pMaps->Get_Menu();

	if( pMenu )
	{
		PopupMenu(pMenu);

		delete(pMenu);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_Buttons::Update_Buttons(void)
{
	Freeze();

	_Del_Items();

	_Add_Items(g_pMaps);

	Scroll(0, 0);

	_Set_Positions();

	Thaw();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Buttons::_Del_Items(void)
{
	if( m_nItems > 0 )
	{
		for(int i=0; i<m_nItems; i++)
		{
			delete(m_Items[i]);
		}

		SG_Free(m_Items);
	}

	m_Items		= NULL;
	m_nItems	= 0;

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map_Buttons::_Add_Items(CWKSP_Base_Item *pItem)
{
	if( pItem )
	{
		switch( pItem->Get_Type() )
		{
		default:
			return( false );

		case WKSP_ITEM_Map:
			return( _Add_Item((CWKSP_Map *)pItem) );

		case WKSP_ITEM_Map_Manager:
			break;
		}

		for(int i=0; i<((CWKSP_Base_Manager *)pItem)->Get_Count(); i++)
		{
			_Add_Items(((CWKSP_Base_Manager *)pItem)->Get_Item(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Map_Buttons::_Add_Item(CWKSP_Map *pMap)
{
	if( pMap )
	{
		m_Items	= (CWKSP_Map_Button **)SG_Realloc(m_Items, (m_nItems + 1) * sizeof(CWKSP_Map_Button *));
		m_Items[m_nItems++]	= new CWKSP_Map_Button(this, pMap);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_Buttons::_Set_Positions(void)
{
	int		Size, xSize, ySize, xPos, yPos, xAdd, yAdd;

	Size	= g_pMaps->Get_Parameter("THUMBNAIL_SIZE")->asInt();

	xSize	= GetClientSize().x - SCROLL_BAR_DX;

	if( xSize < Size + THUMBNAIL_DIST )
	{
		xSize	= Size + THUMBNAIL_DIST;
	}

	xPos	= THUMBNAIL_DIST;
	yPos	= THUMBNAIL_DIST;
	xAdd	= 0;
	yAdd	= 0;

	//-----------------------------------------------------
	for(int i=0, x, y; i<m_nItems; i++)
	{
		CWKSP_Map_Button	*pItem	= m_Items[i];

		{
			xAdd	= Size;

			if( xPos + xAdd >= xSize )
			{
				xPos	 = THUMBNAIL_DIST;
				yPos	+= yAdd;
				yAdd	 = THUMBNAIL_DIST + Size;
			}

			yAdd	= Size + THUMBNAIL_DIST;

			CalcScrolledPosition(xPos, yPos, &x, &y);
			pItem->SetSize(x, y, Size, Size);

			xPos	+= THUMBNAIL_DIST + xAdd;
		}
	}

	//-----------------------------------------------------
	xSize	+= SCROLL_BAR_DX;
	ySize	 = SCROLL_BAR_DY + yPos + yAdd;

	if(	m_xScroll != xSize || m_yScroll != ySize )
	{
		m_xScroll	= xSize;
		m_yScroll	= ySize;

		SetScrollbars(SCROLL_RATE, SCROLL_RATE, m_xScroll / SCROLL_RATE, m_yScroll / SCROLL_RATE);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
