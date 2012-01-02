/**********************************************************
 * Version $Id$
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
#include <wx/tooltip.h>

#include <saga_api/saga_api.h>

#include "res_controls.h"
#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"
#include "dc_helper.h"

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
#define SCROLL_RATE		5

#define SCROLL_BAR_DX	wxSystemSettings::GetMetric(wxSYS_VSCROLL_X)
#define SCROLL_BAR_DY	wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y)

//---------------------------------------------------------
#define THUMBNAIL_SIZE	75
#define THUMBNAIL_DIST	5

#define TITLE_FONT		wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_Buttons	*g_pMap_Buttons	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CWKSP_Map_Button, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP_Map_Button, wxPanel)
	EVT_PAINT			(CWKSP_Map_Button::On_Paint)
	EVT_LEFT_DOWN		(CWKSP_Map_Button::On_Mouse_LDown)
	EVT_LEFT_DCLICK		(CWKSP_Map_Button::On_Mouse_LDClick)
	EVT_RIGHT_DOWN		(CWKSP_Map_Button::On_Mouse_RDown)
END_EVENT_TABLE()

//---------------------------------------------------------
CWKSP_Map_Button::CWKSP_Map_Button(wxWindow *pParent, class CWKSP_Map *pMap)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER)
{
	m_pMap		= pMap;
	m_Title		= pMap->Get_Name();
}

//---------------------------------------------------------
CWKSP_Map_Button::CWKSP_Map_Button(wxWindow *pParent, const wxString &Title)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize, 0)
{
	m_pMap		= NULL;
	m_Title		= Title;

	int			x, y, d, e;
	wxClientDC	dc(this);
	wxFont		Font(TITLE_FONT);
	dc.GetTextExtent(m_Title, &x, &y, &d, &e, &Font);
	SetSize(-1, -1, x + 4, y + d + e + 4);
}

//---------------------------------------------------------
void CWKSP_Map_Button::On_Paint(wxPaintEvent &event)
{
	wxPaintDC	dc(this);
	wxRect		r(wxPoint(0, 0), GetClientSize());

	if( m_pMap )
	{
		if( g_pMaps->Exists(m_pMap) )
		{
			if( !GetToolTip() || GetToolTip()->GetTip().Cmp(m_pMap->Get_Name()) )
			{
				SetToolTip(m_pMap->Get_Name());
			}

			dc.DrawBitmap(m_pMap->Get_Thumbnail(r.GetWidth() - 1, r.GetHeight() - 1), r.GetLeft(), r.GetTop(), true);

			if( g_pACTIVE->Get_Item() == m_pMap )
			{
				dc.SetPen(wxPen(((CWKSP_Map_Buttons *)GetParent())->Get_Active_Color()));
				Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);	r.Deflate(1);
				Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);	r.Deflate(1);
				Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);
			}
		}
	}
	else
	{
		dc.DrawLine(0, 0, r.GetWidth(), 0);
		dc.DrawLine(0, 1, r.GetWidth(), 1);
		dc.SetFont(TITLE_FONT);
		dc.DrawText(m_Title, 2, 2);
		dc.DrawLine(0, r.GetBottom(), GetClientSize().x, r.GetBottom());
	}
}

//---------------------------------------------------------
bool CWKSP_Map_Button::_Set_Layer_Active(void)
{
	if( m_pMap && g_pMaps->Exists(m_pMap) )
	{
		g_pMap_Ctrl->Set_Item_Selected(m_pMap);

		return( true );
	}

	m_pMap	= NULL;

	return( false );
}

//---------------------------------------------------------
void CWKSP_Map_Button::On_Mouse_LDown(wxMouseEvent &event)
{
	_Set_Layer_Active();

	event.Skip();
}

//---------------------------------------------------------
void CWKSP_Map_Button::On_Mouse_LDClick(wxMouseEvent &event)
{
	if( _Set_Layer_Active() )
	{
		m_pMap->On_Command(ID_CMD_WKSP_ITEM_RETURN);
	}

	event.Skip();
}

//---------------------------------------------------------
void CWKSP_Map_Button::On_Mouse_RDown(wxMouseEvent &event)
{
	if( _Set_Layer_Active() )
	{
		wxMenu	*pMenu;

		if( (pMenu = m_pMap->Get_Menu()) != NULL )
		{
			GetParent()->PopupMenu(pMenu, GetParent()->ScreenToClient(ClientToScreen(event.GetPosition())));

			delete(pMenu);

			return;
		}
	}

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CWKSP_Map_Buttons, wxScrolledWindow)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP_Map_Buttons, wxScrolledWindow)
	EVT_RIGHT_DOWN		(CWKSP_Map_Buttons::On_Mouse_RDown)
	EVT_SIZE			(CWKSP_Map_Buttons::On_Size)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
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

	//-----------------------------------------------------
	long	lValue;

	m_Size			= CONFIG_Read(wxT("/BUTTONS_MAPS"), wxT("SIZE")		, lValue) ? (int)lValue : 75;
	m_Active_Color	= CONFIG_Read(wxT("/BUTTONS_MAPS"), wxT("SELCOLOR")	, lValue) ?      lValue : Get_Color_asInt(SYS_Get_Color(wxSYS_COLOUR_BTNSHADOW));

	//-----------------------------------------------------
	m_Parameters.Create(this, _TL("Options for Map Thumbnails"), _TL(""));

	m_Parameters.Add_Value(
		NULL, "SIZE"		, _TL("Thumbnail Size"),
		_TL(""),
		PARAMETER_TYPE_Int, m_Size, 10, true
	);

	m_Parameters.Add_Value(
		NULL, "SELCOLOR"	, _TL("Selection Color"),
		_TL(""),
		PARAMETER_TYPE_Color, m_Active_Color
	);
}

//---------------------------------------------------------
CWKSP_Map_Buttons::~CWKSP_Map_Buttons(void)
{
	CONFIG_Write(wxT("/BUTTONS_MAPS"), wxT("SIZE")    , (long)m_Parameters("SIZE")    ->asInt());
	CONFIG_Write(wxT("/BUTTONS_MAPS"), wxT("SELCOLOR"),       m_Parameters("SELCOLOR")->asColor());

	_Del_Items();

	g_pMap_Buttons	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_Buttons::On_Mouse_RDown(wxMouseEvent &event)
{
	if( DLG_Parameters(&m_Parameters) )
	{
		m_Size			= m_Parameters("SIZE")		->asInt();
		m_Active_Color	= m_Parameters("SELCOLOR")	->asColor();

		Update_Buttons();
	}

	event.Skip();
}

//---------------------------------------------------------
void CWKSP_Map_Buttons::On_Size(wxSizeEvent &event)
{
	_Set_Positions();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_Buttons::_Set_Positions(void)
{
	int		xSize, ySize, xPos, yPos, xAdd, yAdd, i, x, y;

	xSize	= GetClientSize().x - SCROLL_BAR_DX;

	if( xSize < m_Size + THUMBNAIL_DIST )
	{
		xSize	= m_Size + THUMBNAIL_DIST;
	}

	xPos	= THUMBNAIL_DIST;
	yPos	= THUMBNAIL_DIST;
	xAdd	= 0;
	yAdd	= 0;

	for(i=0; i<m_nItems; i++)
	{
		CWKSP_Map_Button	*pItem	= m_Items[i];

		if( pItem->is_Title() )
		{
			xPos	 = THUMBNAIL_DIST;
			yPos	+= THUMBNAIL_DIST + yAdd;

			CalcScrolledPosition(0, yPos, &x, &y);
			pItem->SetSize(x, y, xSize + SCROLL_BAR_DX, -1);

			yPos	+= THUMBNAIL_DIST + pItem->GetSize().y;
			yAdd	 = 0;
		}
		else
		{
			xAdd	= m_Size;

			if( xPos + xAdd >= xSize )
			{
				xPos	 = THUMBNAIL_DIST;
				yPos	+= yAdd;
				yAdd	 = THUMBNAIL_DIST + m_Size;
			}

			yAdd	= m_Size + THUMBNAIL_DIST;

			CalcScrolledPosition(xPos, yPos, &x, &y);
			pItem->SetSize(x, y, m_Size, m_Size);

			xPos	+= THUMBNAIL_DIST + xAdd;
		}
	}

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

//---------------------------------------------------------
bool CWKSP_Map_Buttons::_Add_Item(const wxString &Title)
{
	if( Title.Length() > 0 )
	{
		m_Items	= (CWKSP_Map_Button **)SG_Realloc(m_Items, (m_nItems + 1) * sizeof(CWKSP_Map_Button *));
		m_Items[m_nItems++]	= new CWKSP_Map_Button(this, Title);

		return( true );
	}

	return( false );
}

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
