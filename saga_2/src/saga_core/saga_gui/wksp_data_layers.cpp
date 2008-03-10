
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
//                 WKSP_Data_Layers.cpp                  //
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
#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"
#include "dc_helper.h"

#include "active.h"

#include "wksp_data_control.h"
#include "wksp_data_manager.h"
#include "wksp_layer.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"

#include "wksp_data_layers.h"


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
CWKSP_Data_Buttons	*g_pData_Buttons	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CWKSP_Data_Button, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP_Data_Button, wxPanel)
	EVT_PAINT			(CWKSP_Data_Button::On_Paint)
	EVT_LEFT_DOWN		(CWKSP_Data_Button::On_Mouse_LDown)
	EVT_LEFT_DCLICK		(CWKSP_Data_Button::On_Mouse_LDClick)
	EVT_RIGHT_DOWN		(CWKSP_Data_Button::On_Mouse_RDown)
END_EVENT_TABLE()

//---------------------------------------------------------
CWKSP_Data_Button::CWKSP_Data_Button(wxWindow *pParent, class CWKSP_Layer *pLayer)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER)
{
	m_pLayer	= pLayer;
	m_pObject	= pLayer->Get_Object();
	m_Title		= pLayer->Get_Name();
}

//---------------------------------------------------------
CWKSP_Data_Button::CWKSP_Data_Button(wxWindow *pParent, const wxChar *Title)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize, 0)
{
	m_pLayer	= NULL;
	m_pObject	= NULL;
	m_Title		= Title;

	int			x, y, d, e;
	wxClientDC	dc(this);
	wxFont		Font(TITLE_FONT);
	dc.GetTextExtent(m_Title, &x, &y, &d, &e, &Font);
	SetSize(-1, -1, x + 4, y + d + e + 4);
}

//---------------------------------------------------------
void CWKSP_Data_Button::On_Paint(wxPaintEvent &event)
{
	wxPaintDC	dc(this);
	wxRect		r(wxPoint(0, 0), GetClientSize());

	if( m_pLayer && m_pLayer->Get_Object() && m_pLayer->GetId().IsOk() )
	{
		if( g_pData->Exists(m_pObject) )
		{
			SetToolTip(m_pLayer->Get_Name());

			dc.DrawBitmap(m_pLayer->Get_Thumbnail(r.GetWidth() - 1, r.GetHeight() - 1), r.GetLeft(), r.GetTop(), true);

			if( m_pLayer->is_Selected() )
			{
				dc.SetPen(wxPen(((CWKSP_Data_Buttons *)GetParent())->Get_Active_Color()));
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
bool CWKSP_Data_Button::_Select(bool bKeepOthers)
{
	if( m_pLayer && g_pData->Exists(m_pObject) )
	{
		g_pData_Ctrl->Set_Item_Selected(m_pLayer, bKeepOthers);

		GetParent()->Refresh();

		return( true );
	}

	m_pObject	= NULL;

	return( false );
}

//---------------------------------------------------------
void CWKSP_Data_Button::On_Mouse_LDown(wxMouseEvent &event)
{
	_Select(event.ShiftDown() || event.ControlDown());

	event.Skip();
}

//---------------------------------------------------------
void CWKSP_Data_Button::On_Mouse_LDClick(wxMouseEvent &event)
{
	if( _Select(false) )
	{
		m_pLayer->On_Command(ID_CMD_WKSP_ITEM_RETURN);
	}

	event.Skip();
}

//---------------------------------------------------------
void CWKSP_Data_Button::On_Mouse_RDown(wxMouseEvent &event)
{
	if( _Select(event.ShiftDown() || event.ControlDown()) )
	{
		wxMenu	*pMenu;

		if( (pMenu = m_pLayer->Get_Menu()) != NULL )
		{
			PopupMenu(pMenu, event.GetPosition());
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
IMPLEMENT_CLASS(CWKSP_Data_Buttons, wxScrolledWindow)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP_Data_Buttons, wxScrolledWindow)
	EVT_RIGHT_DOWN		(CWKSP_Data_Buttons::On_Mouse_RDown)
	EVT_SIZE			(CWKSP_Data_Buttons::On_Size)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Buttons::CWKSP_Data_Buttons(wxWindow *pParent)
	: wxScrolledWindow(pParent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE)
{
	g_pData_Buttons	= this;

	m_xScroll		= 0;
	m_yScroll		= 0;

	m_Items			= NULL;
	m_nItems		= 0;

	//-----------------------------------------------------
	bool	bValue;
	long	lValue;

	m_Size			= CONFIG_Read(wxT("/BUTTONS_DATA"), wxT("SIZE")		, lValue) ? (int)lValue : 75;
	m_bCategorised	= CONFIG_Read(wxT("/BUTTONS_DATA"), wxT("CATEGORY")	, bValue) ?      bValue : true;
	m_Active_Color	= CONFIG_Read(wxT("/BUTTONS_DATA"), wxT("SELCOLOR")	, lValue) ?      lValue : Get_Color_asInt(SYS_Get_Color(wxSYS_COLOUR_BTNSHADOW));

	//-----------------------------------------------------
	m_Parameters.Create(this, LNG("Options for Data Thumbnails"), LNG(""));

	m_Parameters.Add_Value(
		NULL, "SIZE"		, LNG("Thumbnail Size"),
		LNG(""),
		PARAMETER_TYPE_Int, m_Size, 10, true
	);

	m_Parameters.Add_Value(
		NULL, "CATEGORY"	, LNG("Show Categories"),
		LNG(""),
		PARAMETER_TYPE_Bool, m_bCategorised
	);

	m_Parameters.Add_Value(
		NULL, "SELCOLOR"	, LNG("Selection Color"),
		LNG(""),
		PARAMETER_TYPE_Color, m_Active_Color
	);
}

//---------------------------------------------------------
CWKSP_Data_Buttons::~CWKSP_Data_Buttons(void)
{
	CONFIG_Write(wxT("/BUTTONS_DATA"), wxT("SIZE")    , (long)m_Parameters("SIZE")	  ->asInt());
	CONFIG_Write(wxT("/BUTTONS_DATA"), wxT("CATEGORY"),       m_Parameters("CATEGORY")->asBool());
	CONFIG_Write(wxT("/BUTTONS_DATA"), wxT("SELCOLOR"),       m_Parameters("SELCOLOR")->asColor());

	_Del_Items();

	g_pData_Buttons	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Data_Buttons::On_Mouse_RDown(wxMouseEvent &event)
{
	if( DLG_Parameters(&m_Parameters) )
	{
		m_Size			= m_Parameters("SIZE")		->asInt();
		m_bCategorised	= m_Parameters("CATEGORY")	->asBool();
		m_Active_Color	= m_Parameters("SELCOLOR")	->asColor();

		Update_Buttons();
	}

	event.Skip();
}

//---------------------------------------------------------
void CWKSP_Data_Buttons::On_Size(wxSizeEvent &event)
{
	_Set_Positions();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Data_Buttons::Update_Buttons(void)
{
	Freeze();

	_Del_Items();
	_Add_Items(g_pData);

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
void CWKSP_Data_Buttons::_Set_Positions(void)
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
		CWKSP_Data_Button	*pItem	= m_Items[i];

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
bool CWKSP_Data_Buttons::_Add_Items(CWKSP_Base_Item *pItem)
{
	if( pItem )
	{
		switch( pItem->Get_Type() )
		{
		default:
			return( false );

		case WKSP_ITEM_Shapes:
		case WKSP_ITEM_TIN:
		case WKSP_ITEM_Grid:
			return( _Add_Item((CWKSP_Layer *)pItem) );

		case WKSP_ITEM_Data_Manager:
		case WKSP_ITEM_Grid_Manager:
		case WKSP_ITEM_Shapes_Manager:
			break;

		case WKSP_ITEM_Grid_System:
		case WKSP_ITEM_Shapes_Type:
		case WKSP_ITEM_TIN_Manager:
			if( m_bCategorised )
			{
				_Add_Item(pItem->Get_Name());
			}
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
bool CWKSP_Data_Buttons::_Add_Item(CWKSP_Layer *pLayer)
{
	if( pLayer )
	{
		m_Items	= (CWKSP_Data_Button **)SG_Realloc(m_Items, (m_nItems + 1) * sizeof(CWKSP_Data_Button *));
		m_Items[m_nItems++]	= new CWKSP_Data_Button(this, pLayer);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Buttons::_Add_Item(const wxChar *Title)
{
	if( Title )
	{
		m_Items	= (CWKSP_Data_Button **)SG_Realloc(m_Items, (m_nItems + 1) * sizeof(CWKSP_Data_Button *));
		m_Items[m_nItems++]	= new CWKSP_Data_Button(this, Title);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Buttons::_Del_Items(void)
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
