
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
//                  ACTIVE_Layers.cpp                    //
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
#include <saga_api/saga_api.h>

#include "res_controls.h"

#include "dc_helper.h"
#include "helper.h"

#include "wksp_map.h"
#include "wksp_layer.h"
#include "wksp_map_layer.h"

#include "active_layers.h"


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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CACTIVE_Layers_Item, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CACTIVE_Layers_Item, wxPanel)
	EVT_PAINT			(CACTIVE_Layers_Item::On_Paint)
END_EVENT_TABLE()

//---------------------------------------------------------
CACTIVE_Layers_Item::CACTIVE_Layers_Item(wxWindow *pParent, int ax, int ay, int dx, int dy, class CWKSP_Layer *pLayer)
	: wxPanel(pParent, -1, wxPoint(ax, ay), wxSize(dx, dy), wxRAISED_BORDER)
{
	m_pLayer	= pLayer;

	SetToolTip(m_pLayer->Get_Name());
}

//---------------------------------------------------------
void CACTIVE_Layers_Item::On_Paint(wxPaintEvent &event)
{
	wxRect		r(wxPoint(0, 0), GetClientSize());
	wxPaintDC	dc(this);

//	Draw_Edge(dc, EDGE_STYLE_SUNKEN, r);

	if( m_pLayer )
	{
		dc.DrawBitmap(m_pLayer->Get_Thumbnail(THUMBNAIL_SIZE, THUMBNAIL_SIZE),
			r.GetLeft(), r.GetTop(), true
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CACTIVE_Layers, wxScrolledWindow)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CACTIVE_Layers, wxScrolledWindow)
	EVT_MOTION			(CACTIVE_Layers::On_Mouse_Motion)
	EVT_KEY_DOWN		(CACTIVE_Layers::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CACTIVE_Layers::CACTIVE_Layers(wxWindow *pParent)
	: wxScrolledWindow(pParent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE)
{
//	SYS_Set_Color_BG(this, wxSYS_COLOUR_WINDOW);

	m_pItem		= NULL;

	m_xScroll	= 0;
	m_yScroll	= 0;

	m_Zoom		= 1.0;

	m_Items	= NULL;
	m_nItems	= 0;
}

//---------------------------------------------------------
CACTIVE_Layers::~CACTIVE_Layers(void)
{
	_Layers_Clear();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Layers::Set_Item(CWKSP_Base_Item *pItem)
{
	m_pItem	= pItem;

	wxSize	s(THUMBNAIL_DIST, THUMBNAIL_DIST);

	_Layers_Clear();

	_Add_Items(s, m_pItem);

	s.x	+= SCROLL_BAR_DX;
	s.y	+= SCROLL_BAR_DY;

	if(	m_xScroll != s.x || m_yScroll != s.y )
	{
		m_xScroll	= s.x;
		m_yScroll	= s.y;

		SetScrollbars(SCROLL_RATE, SCROLL_RATE, m_xScroll / SCROLL_RATE, m_yScroll / SCROLL_RATE);
	}

	Refresh();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Layers::On_Mouse_Motion(wxMouseEvent &event)
{
	event.Skip();
}

//---------------------------------------------------------
void CACTIVE_Layers::On_Key_Down(wxKeyEvent &event)
{
	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Layers::_Layers_Clear(void)
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
}

//---------------------------------------------------------
CACTIVE_Layers_Item * CACTIVE_Layers::_Layers_Add(int ax, int ay, int dx, int dy, CWKSP_Layer *pLayer)
{
	CACTIVE_Layers_Item	*p	= NULL;

	if( pLayer )
	{
		m_Items	= (CACTIVE_Layers_Item **)SG_Realloc(m_Items, (m_nItems + 1) * sizeof(CACTIVE_Layers_Item *));
		m_Items[m_nItems++]	= p	= new CACTIVE_Layers_Item(this, ax, ay, dx, dy, pLayer);
	}

	return( p );
}

//---------------------------------------------------------
CWKSP_Layer * CACTIVE_Layers::_Layers_Get(wxPoint p)
{
	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Layers::OnDraw(wxDC &dc)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CACTIVE_Layers::_Add_Items(wxSize &Size, CWKSP_Base_Item *pItem)
{
	if( pItem )
	{
		int		fSize;

		switch( pItem->Get_Type() )
		{
		default:
			return( false );

		case WKSP_ITEM_Shapes:
		case WKSP_ITEM_TIN:
		case WKSP_ITEM_Grid:
			return( _Add_Item(Size, (CWKSP_Layer *)pItem) );

		case WKSP_ITEM_Map_Layer:
			return( _Add_Item(Size, ((CWKSP_Map_Layer *)pItem)->Get_Layer()) );

		case WKSP_ITEM_Data_Manager:	fSize	= 12;	break;
		case WKSP_ITEM_Grid_Manager:	fSize	= 10;	break;
		case WKSP_ITEM_Grid_System:		fSize	=  8;	break;
		case WKSP_ITEM_Shapes_Manager:	fSize	= 10;	break;
		case WKSP_ITEM_Shapes_Type:		fSize	=  8;	break;
		case WKSP_ITEM_TIN_Manager:		fSize	= 10;	break;
		case WKSP_ITEM_Map_Manager:		fSize	= 10;	break;
		case WKSP_ITEM_Map:				fSize	=  8;	break;
			break;
		}

		int		i, nRows	= GetSize().x / (THUMBNAIL_DIST + THUMBNAIL_SIZE);
//		wxFont	Font(fSize, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
//		dc.SetFont(Font);
//		dc.DrawText(pItem->Get_Name(), 5, Size.y);
		Size.y	+= fSize + THUMBNAIL_DIST;
		Size.x	= THUMBNAIL_DIST;

		for(i=0; i<((CWKSP_Base_Manager *)pItem)->Get_Count(); i++)
		{
			_Add_Items(Size, ((CWKSP_Base_Manager *)pItem)->Get_Item(i));

			if( (i + 1) % nRows )
			{
				Size.x	+= THUMBNAIL_DIST + THUMBNAIL_SIZE;
			}
			else
			{
				Size.x	 = THUMBNAIL_DIST;
				Size.y	+= THUMBNAIL_DIST + THUMBNAIL_SIZE;
			}
		}

		if( i % nRows )
		{
			Size.y	+= THUMBNAIL_DIST + THUMBNAIL_SIZE;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CACTIVE_Layers::_Add_Item(wxSize &s, CWKSP_Layer *pLayer)
{
	if( pLayer )
	{
//		wxRect	r(s.x, s.y, THUMBNAIL_SIZE, THUMBNAIL_SIZE);

//		dc.DrawBitmap(pLayer->Get_Thumbnail(THUMBNAIL_SIZE, THUMBNAIL_SIZE),
//			r.GetLeft(), r.GetTop(), true
//		);

//		Draw_Edge(dc, EDGE_STYLE_RAISED, r);	r.Inflate(1);
//		Draw_Edge(dc, EDGE_STYLE_RAISED, r);	r.Inflate(1);
//		Draw_Edge(dc, EDGE_STYLE_RAISED, r);	r.Inflate(1);

		_Layers_Add(s.x, s.y, THUMBNAIL_SIZE, THUMBNAIL_SIZE, pLayer);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
