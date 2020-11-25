
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
//                 VIEW_Map_Control.cpp                  //
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
#include <wx/image.h>
#include <wx/dcclient.h>

#include <saga_gdi/sgdi_helper.h>

#include "res_images.h"

#include "helper.h"

#include "wksp_map.h"
#include "wksp_map_layer.h"
#include "wksp_tool.h"
#include "wksp_grid.h"
#include "wksp_shapes.h"

#include "view_map.h"
#include "view_map_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_KEYS(e)	\
	((e.LeftIsDown  () ? TOOL_INTERACTIVE_KEY_LEFT   : 0)\
	|(e.MiddleIsDown() ? TOOL_INTERACTIVE_KEY_MIDDLE : 0)\
	|(e.RightIsDown () ? TOOL_INTERACTIVE_KEY_RIGHT  : 0)\
	|(e.ShiftDown   () ? TOOL_INTERACTIVE_KEY_SHIFT  : 0)\
	|(e.AltDown     () ? TOOL_INTERACTIVE_KEY_ALT    : 0)\
	|(e.ControlDown () ? TOOL_INTERACTIVE_KEY_CTRL   : 0))

//---------------------------------------------------------
#define STANDARD_ZOOM_FACTOR	50


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Map_Control, wxPanel)
	EVT_PAINT				(CVIEW_Map_Control::On_Paint)
	EVT_SIZE				(CVIEW_Map_Control::On_Size)
	EVT_KEY_DOWN			(CVIEW_Map_Control::On_Key_Down)

	EVT_MOUSEWHEEL			(CVIEW_Map_Control::On_Mouse_Wheel)
	EVT_MOTION				(CVIEW_Map_Control::On_Mouse_Motion)
	EVT_LEFT_DOWN			(CVIEW_Map_Control::On_Mouse_LDown)
	EVT_LEFT_UP				(CVIEW_Map_Control::On_Mouse_LUp)
	EVT_LEFT_DCLICK			(CVIEW_Map_Control::On_Mouse_LDClick)
	EVT_RIGHT_DOWN			(CVIEW_Map_Control::On_Mouse_RDown)
	EVT_RIGHT_UP			(CVIEW_Map_Control::On_Mouse_RUp)
	EVT_RIGHT_DCLICK		(CVIEW_Map_Control::On_Mouse_RDClick)
	EVT_MIDDLE_DOWN			(CVIEW_Map_Control::On_Mouse_MDown)
	EVT_MIDDLE_UP			(CVIEW_Map_Control::On_Mouse_MUp)
	EVT_MOUSE_CAPTURE_LOST	(CVIEW_Map_Control::On_Mouse_Lost)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Map_Control::CVIEW_Map_Control(CVIEW_Map *pParent, CWKSP_Map *pMap)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	SYS_Set_Color_BG_Window(this);

	m_pParent	= (CVIEW_Map *)pParent;
	m_pMap		= pMap;

	m_Mode		= -1;
	Set_Mode(MAP_MODE_ZOOM);

	m_Drag_Mode	= TOOL_INTERACTIVE_DRAG_NONE;

	m_CrossHair.x = m_CrossHair.y = -1;
}

//---------------------------------------------------------
CVIEW_Map_Control::~CVIEW_Map_Control(void)
{
	_Distance_Reset();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Map_Control::Set_Mode(int Mode)
{
	if( m_Mode != Mode )
	{
		if( m_Mode == MAP_MODE_DISTANCE )
		{
			_Distance_Reset();
		}

		m_Mode	= Mode;

		switch( m_Mode )
		{
		case MAP_MODE_ZOOM: default:
			SetCursor(IMG_Get_Cursor(ID_IMG_CRS_SELECT));
			break;

		case MAP_MODE_PAN:
			SetCursor(IMG_Get_Cursor(ID_IMG_CRS_HAND));
			break;

		case MAP_MODE_PAN_DOWN:
			SetCursor(IMG_Get_Cursor(ID_IMG_CRS_HAND_GRAP));
			break;

		case MAP_MODE_SELECT:
			SetCursor(IMG_Get_Cursor(ID_IMG_CRS_INFO));
			break;

		case MAP_MODE_DISTANCE:
			SetCursor(IMG_Get_Cursor(ID_IMG_CRS_SELECT));
			_Distance_Reset();
			break;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline void CVIEW_Map_Control::_Set_StatusBar(const TSG_Point &Point)
{
	static bool	bBuisy	= false;

	if( bBuisy == false )
	{
		bBuisy	= true;

		if( m_pMap->Get_Parameter("GCS_POSITION")->asBool() && m_pMap->Get_Projection().is_Okay() )
		{
			CSG_Projection	GCS; GCS.Set_GCS_WGS84(); TSG_Point Position = Point;

			if( SG_Get_Projected(m_pMap->Get_Projection(), GCS, Position) )
			{
				STATUSBAR_Set_Text(wxString::Format("X %s", SG_Double_To_Degree(Position.x).c_str()), STATUSBAR_VIEW_X);
				STATUSBAR_Set_Text(wxString::Format("Y %s", SG_Double_To_Degree(Position.y).c_str()), STATUSBAR_VIEW_Y);
			}
		}
		else
		{
			STATUSBAR_Set_Text(wxString::Format("X %f", Point.x), STATUSBAR_VIEW_X);
			STATUSBAR_Set_Text(wxString::Format("Y %f", Point.y), STATUSBAR_VIEW_Y);
		}

		if( m_Mode == MAP_MODE_DISTANCE )
		{
			STATUSBAR_Set_Text(wxString::Format("D %f", m_Distance + m_Distance_Move), STATUSBAR_VIEW_Z);
		}
		else if( Get_Active_Layer() )
		{
			TSG_Point	p(Point);	double epsilon	= _Get_Client2World(2.); // 2 pixel tolerance...

			CWKSP_Map_Layer *pMapLayer = m_pMap->Find_Layer(Get_Active_Layer());

			if( !pMapLayer || pMapLayer->is_Projecting() )
			{
				TSG_Point	e(Point); e.x += epsilon; e.y += epsilon;

				if( SG_Get_Projected(m_pMap->Get_Projection(), Get_Active_Layer()->Get_Object()->Get_Projection(), p)
				&&  SG_Get_Projected(m_pMap->Get_Projection(), Get_Active_Layer()->Get_Object()->Get_Projection(), e) )
				{
					epsilon	= (fabs(e.x - e.x) + fabs(e.y - e.y)) / 2.;
				}
			}

			STATUSBAR_Set_Text(wxString::Format("Z %s", Get_Active_Layer()->Get_Value(p, epsilon).c_str()), STATUSBAR_VIEW_Z);
		}
		else
		{
			STATUSBAR_Set_Text("Z", STATUSBAR_VIEW_Z);
		}

		bBuisy	= false;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline wxPoint CVIEW_Map_Control::_Get_World2Client(const TSG_Point &Point)
{
	wxRect		rClient(GetClientSize());
	CSG_Rect	rWorld(m_pMap->Get_World(rClient));
	double		World2Client	= (double)rClient.GetWidth() / rWorld.Get_XRange();

	return( wxPoint(          (int)(0.5 + (Point.x - rWorld.Get_XMin()) * World2Client),
		rClient.GetHeight() - (int)(0.5 + (Point.y - rWorld.Get_YMin()) * World2Client)
	));
}

//---------------------------------------------------------
inline CSG_Point CVIEW_Map_Control::_Get_Client2World(const wxPoint &Point, bool bToActive)
{
	TSG_Point	pWorld	= m_pMap->Get_World(wxRect(GetClientSize()), Point);

	if( bToActive && m_pMap->Find_Active() )
	{
		SG_Get_Projected(m_pMap->Get_Projection(), Get_Active_Layer()->Get_Object()->Get_Projection(), pWorld);
	}

	return( pWorld );
}

//---------------------------------------------------------
inline double CVIEW_Map_Control::_Get_Client2World(double Length, bool bToActive)
{
	wxRect	rClient(GetClientSize());	CSG_Rect	rWorld(m_pMap->Get_World(rClient));

	if( bToActive && m_pMap->Find_Active() )
	{
		SG_Get_Projected(m_pMap->Get_Projection(), Get_Active_Layer()->Get_Object()->Get_Projection(), rWorld.m_rect);
	}

	return( Length * rWorld.Get_XRange() / (double)rClient.GetWidth() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::Set_CrossHair(const TSG_Point &Point)
{
	_Draw_CrossHair(m_CrossHair);
	_Draw_CrossHair(m_CrossHair = _Get_World2Client(Point));
}

//---------------------------------------------------------
void CVIEW_Map_Control::Set_CrossHair_Off(void)
{
	_Draw_CrossHair(m_CrossHair);

	m_CrossHair.x = m_CrossHair.y = -1;
}

//---------------------------------------------------------
bool CVIEW_Map_Control::_Draw_CrossHair(const wxPoint &Point)
{
	wxRect	r(GetClientSize());

	if( r.Contains(Point) )
	{
		wxClientDC	dc(this);

		dc.SetLogicalFunction(wxINVERT);

	//	dc.CrossHair(Point);

		dc.DrawLine(Point.x, r.GetTop(), Point.x, r.GetBottom());
		dc.DrawLine(r.GetLeft(), Point.y, r.GetRight(), Point.y);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::_Draw_Inverse(const wxPoint &A, const wxPoint &B)
{
	if( m_Drag_Mode != TOOL_INTERACTIVE_DRAG_NONE )
	{
		wxClientDC	dc(this);

		dc.SetLogicalFunction(wxINVERT);

		switch( m_Drag_Mode )
		{
		case TOOL_INTERACTIVE_DRAG_LINE:
			dc.DrawLine     (A.x, A.y, B.x, B.y);
			break;

		case TOOL_INTERACTIVE_DRAG_BOX:
			dc.DrawRectangle(A.x, A.y, B.x - A.x, B.y - A.y);
			break;

		case TOOL_INTERACTIVE_DRAG_CIRCLE:
			dc.DrawCircle   (A.x, A.y, (int)SG_Get_Distance(A.x, A.y, B.x, B.y));
			break;
		}
	}
}

//---------------------------------------------------------
void CVIEW_Map_Control::_Draw_Inverse(const wxPoint &A, const wxPoint &B_Old, const wxPoint &B_New)
{
	_Draw_Inverse(A, B_Old);
	_Draw_Inverse(A, B_New);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Map_Control::_Zoom(const wxPoint &A, const wxPoint &B)
{
	if( A.x == B.x && A.y == B.y )
	{
		return( _Zoom(_Get_Client2World(A), true) );
	}

	m_pMap->Set_Extent(CSG_Rect(_Get_Client2World(A), _Get_Client2World(B)));

	return( true );
}

//---------------------------------------------------------
bool CVIEW_Map_Control::_Zoom(const CSG_Point &Center, bool bZoomIn)
{
	CSG_Rect	rWorld(m_pMap->Get_Extent());

	rWorld.Move(Center - rWorld.Get_Center());

	if( bZoomIn )
	{
		rWorld.Deflate(STANDARD_ZOOM_FACTOR);
	}
	else
	{
		rWorld.Inflate(STANDARD_ZOOM_FACTOR);
	}

	m_pMap->Set_Extent(rWorld);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Map_Control::_Pan(const wxPoint &A, const wxPoint &B)
{
	if( A.x != B.x || A.y != B.y )
	{
		wxClientDC	dc(this);

		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();
		dc.DrawBitmap(m_Bitmap, B.x - A.x, B.y - A.y, false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Map_Control::_Move(wxPoint &A, const wxPoint &B)
{
	if( A.x != B.x || A.y != B.y )
	{
		CSG_Rect	rWorld;

		rWorld.Assign(m_pMap->Get_Extent());
		rWorld.Move(_Get_Client2World(A) - _Get_Client2World(B));
		m_pMap->Set_Extent(rWorld);

		A	= B;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Map_Control::_Move(const wxPoint &Move)
{
	wxPoint	A(Move), B(0, 0);

	return( _Move(A, B) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::_Distance_Reset(void)
{
	m_Distance_Pts.Clear();

	m_Distance		= 0.0;
	m_Distance_Move	= 0.0;

	Refresh(false);
}

//---------------------------------------------------------
void CVIEW_Map_Control::_Distance_Add(const wxPoint &Point)
{
	int		n	= m_Distance_Pts.Get_Count();

	m_Distance_Pts.Add(_Get_Client2World(Point));

	if( n > 0 )
	{
		m_Distance	+= SG_Get_Distance(m_Distance_Pts[n], m_Distance_Pts[n - 1]);
	}

	m_Distance_Move	= 0.0;

	wxClientDC	dc(this);

	_Distance_Draw(dc);
}

//---------------------------------------------------------
void CVIEW_Map_Control::_Distance_Draw(wxDC &dc)
{
	int		n	= m_Distance_Pts.Get_Count();

	if( m_Mode == MAP_MODE_DISTANCE && n > 0 )
	{
		int		i;
		wxPoint	A, B;

		dc.SetPen(wxPen(*wxWHITE, 4));
		for(i=1, A=_Get_World2Client(m_Distance_Pts[0]); i<n; i++)
		{
			B	= A;
			A	= _Get_World2Client(m_Distance_Pts[i]);
			dc.DrawLine(A.x, A.y, B.x, B.y);
		}

		dc.SetPen(wxPen(*wxBLACK, 2));
		for(i=1, A=_Get_World2Client(m_Distance_Pts[0]); i<n; i++)
		{
			B	= A;
			A	= _Get_World2Client(m_Distance_Pts[i]);
			dc.DrawLine(A.x, A.y, B.x, B.y);
		}

		dc.SetPen(wxNullPen);
		dc.SetLogicalFunction(wxINVERT);
		A	= _Get_World2Client(m_Distance_Pts[n - 1]);
		dc.DrawLine(A.x, A.y, m_Mouse_Move.x, m_Mouse_Move.y);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Paint(wxPaintEvent &event)
{
	if( m_Bitmap.Ok() )
	{
		wxPaintDC	dc(this);

		dc.DrawBitmap(m_Bitmap, 0, 0, false);

		_Distance_Draw(dc);

		m_CrossHair.x = m_CrossHair.y = -1;
	}
}

//---------------------------------------------------------
void CVIEW_Map_Control::On_Size(wxSizeEvent &event)
{
	wxRect	r(GetClientSize());

	if( !m_Bitmap.Ok() || m_Bitmap.GetWidth() != r.GetWidth() || m_Bitmap.GetHeight() != r.GetHeight() )
	{
		m_Bitmap.Create(r.GetWidth(), r.GetHeight());

		Refresh_Map();
	}
}

//---------------------------------------------------------
void CVIEW_Map_Control::Refresh_Map(void)
{
	if( m_Bitmap.Ok() )
	{
		wxMemoryDC	dc;

		dc.SelectObject(m_Bitmap);
		m_pMap->Draw_Map(dc, 1.0, GetClientSize());
		dc.SelectObject(wxNullBitmap);

		m_pParent->Ruler_Refresh();

		Refresh(false);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Key_Down(wxKeyEvent &event)
{
	if( m_pMap->Find_Active(true) && Get_Active_Layer()->Edit_On_Key_Down(event.GetKeyCode()) )
	{
		return;
	}

	switch( event.GetKeyCode() )
	{
	default:
		event.Skip();
		break;

	case WXK_PAGEUP:
		_Zoom(m_pMap->Get_Extent().Get_Center(), true);
		break;

	case WXK_PAGEDOWN:
		_Zoom(m_pMap->Get_Extent().Get_Center(), false);
		break;

	case WXK_LEFT:
		_Move(wxPoint(-10,   0));
		break;

	case WXK_RIGHT:
		_Move(wxPoint( 10,   0));
		break;

	case WXK_UP:
		_Move(wxPoint(  0, -10));
		break;

	case WXK_DOWN:
		_Move(wxPoint(  0,  10));
		break;

	case WXK_F2:
		m_pMap->SaveAs_Image();
		break;

	case 'C':
		if( event.GetModifiers() == wxMOD_CONTROL )
		{
			m_pMap->SaveAs_Image_Clipboard(false);
		}
		else if( event.GetModifiers() == wxMOD_ALT )
		{
			m_pMap->SaveAs_Image_Clipboard(GetClientSize().x, GetClientSize().y, -1);
		}
		break;

	case 'L':
		if( event.GetModifiers() == wxMOD_CONTROL )
		{
			m_pMap->SaveAs_Image_Clipboard(true);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_LDown(wxMouseEvent &event)
{
	m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();

	//-----------------------------------------------------
	if( m_Mode != MAP_MODE_SELECT )	// clipboard copy ?
	{
		if( event.ControlDown() )
		{
			m_pMap->SaveAs_Image_Clipboard(false);

			return;
		}

		if( event.AltDown() )
		{
			m_pMap->SaveAs_Image_Clipboard(GetClientSize().x, GetClientSize().y, -1);

			return;
		}
	}

	//-----------------------------------------------------
	bool	bCaptureMouse	= true;

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pTool && g_pTool->is_Interactive() )
		{
			m_Drag_Mode		= ((CSG_Tool_Interactive *)g_pTool->Get_Tool())->Get_Drag_Mode();
			bCaptureMouse	= !g_pTool->Execute(_Get_Client2World(event.GetPosition()), TOOL_INTERACTIVE_LDOWN, GET_KEYS(event));
		}
		else if( m_pMap->Find_Active(false) )
		{
			switch(	Get_Active_Layer()->Get_Type() )
			{
			default                  : m_Drag_Mode = TOOL_INTERACTIVE_DRAG_NONE; break;
			case WKSP_ITEM_Grid      :
			case WKSP_ITEM_PointCloud: m_Drag_Mode = TOOL_INTERACTIVE_DRAG_BOX ; break;
			case WKSP_ITEM_Shapes    : m_Drag_Mode = ((CWKSP_Shapes *)Get_Active_Layer())->is_Editing()
												   ? TOOL_INTERACTIVE_DRAG_NONE
												   : TOOL_INTERACTIVE_DRAG_BOX ; break;
			}

			if( m_pMap->Find_Active(true) )
			{
				Get_Active_Layer()->Edit_On_Mouse_Down(
					_Get_Client2World(event.GetPosition()), _Get_Client2World(1.), GET_KEYS(event)
				);
			}
		}
		break;

	//-----------------------------------------------------
	case MAP_MODE_DISTANCE:
		m_Drag_Mode		= TOOL_INTERACTIVE_DRAG_NONE;
		break;

	//-----------------------------------------------------
	case MAP_MODE_ZOOM:
		m_Drag_Mode		= TOOL_INTERACTIVE_DRAG_BOX;
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN:
		m_Drag_Mode		= TOOL_INTERACTIVE_DRAG_NONE;
		Set_Mode(MAP_MODE_PAN_DOWN);
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN_DOWN:
		m_Drag_Mode		= TOOL_INTERACTIVE_DRAG_NONE;
		break;
	}

	//-----------------------------------------------------
	if( bCaptureMouse && !HasCapture() )
	{
		CaptureMouse();
	}

	event.Skip();
}

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_LUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	//-----------------------------------------------------
	if( m_Mode != MAP_MODE_SELECT && (event.ControlDown() || event.AltDown()) )	// clipboard copy ?
	{
		return;
	}

	//-----------------------------------------------------
	_Draw_Inverse(m_Mouse_Down, event.GetPosition());
	m_Drag_Mode	= TOOL_INTERACTIVE_DRAG_NONE;

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pTool )
		{
			g_pTool->Execute(_Get_Client2World(event.GetPosition()), TOOL_INTERACTIVE_LUP, GET_KEYS(event));
		}
		else if( m_pMap->Find_Active(true) )
		{
			Get_Active_Layer()->Edit_On_Mouse_Up(
				_Get_Client2World(event.GetPosition()), _Get_Client2World(1.), GET_KEYS(event)|TOOL_INTERACTIVE_KEY_LEFT
			);
		}
		else if( m_pMap->Find_Active() )	// on-the-fly projected layer !
		{
			double	d	= _Get_Client2World(1., true);

			Get_Active_Layer()->Edit_On_Mouse_Down(_Get_Client2World(m_Mouse_Down       , true), d, GET_KEYS(event));
			Get_Active_Layer()->Edit_On_Mouse_Up  (_Get_Client2World(event.GetPosition(), true), d, GET_KEYS(event)|TOOL_INTERACTIVE_KEY_LEFT);
		}
		break;

	//-----------------------------------------------------
	case MAP_MODE_DISTANCE:
		_Distance_Add(event.GetPosition());
		break;

	//-----------------------------------------------------
	case MAP_MODE_ZOOM:
		_Zoom(m_Mouse_Down, event.GetPosition());
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN:
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN_DOWN:
		Set_Mode(MAP_MODE_PAN);
		_Move(m_Mouse_Down, event.GetPosition());
		break;
	}
}

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_LDClick(wxMouseEvent &event)
{
	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pTool )
		{
			g_pTool->Execute(_Get_Client2World(event.GetPosition()), TOOL_INTERACTIVE_LDCLICK, GET_KEYS(event));
		}
		break;

	//-----------------------------------------------------
	default:
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_RDown(wxMouseEvent &event)
{
	m_pParent->Activate();

	m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();

	_Draw_Inverse(m_Mouse_Down, event.GetPosition());
	m_Drag_Mode	= TOOL_INTERACTIVE_DRAG_NONE;

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pTool )
		{
			g_pTool->Execute(_Get_Client2World(event.GetPosition()), TOOL_INTERACTIVE_RDOWN, GET_KEYS(event));
		}
		else if( m_pMap->Find_Active(true) )
		{
			Get_Active_Layer()->Edit_On_Mouse_Down(
				_Get_Client2World(event.GetPosition()), _Get_Client2World(1.), GET_KEYS(event)
			);
		}
		break;

	//-----------------------------------------------------
	default:
		break;
	}
}

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_RUp(wxMouseEvent &event)
{
	wxMenu	*pMenu	= NULL;

	m_Drag_Mode		= TOOL_INTERACTIVE_DRAG_NONE;

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pTool )
		{
			g_pTool->Execute(_Get_Client2World(event.GetPosition()), TOOL_INTERACTIVE_RUP, GET_KEYS(event));
		}
		else if( m_pMap->Find_Active(true) && !Get_Active_Layer()->Edit_On_Mouse_Up(
			_Get_Client2World(event.GetPosition()), _Get_Client2World(1.), GET_KEYS(event)|TOOL_INTERACTIVE_KEY_RIGHT) )
		{
			pMenu	= Get_Active_Layer()->Edit_Get_Menu();
		}
		break;

	//-----------------------------------------------------
	case MAP_MODE_DISTANCE:
		if( event.ControlDown() )	// context menu
		{
			pMenu	= m_pParent->_Create_Menu();
		}
		else	// reset
		{
			_Distance_Reset();
		}
		break;

	case MAP_MODE_ZOOM:
		if( event.ControlDown() )	// context menu
		{
			pMenu	= m_pParent->_Create_Menu();
		}
		else	// zoom out
		{
			_Zoom(_Get_Client2World(event.GetPosition()), false);
		}
		break;

	//-----------------------------------------------------
	default:
		pMenu	= m_pParent->_Create_Menu();
		break;
	}

	//-----------------------------------------------------
	if( pMenu != NULL )
	{
		PopupMenu(pMenu, event.GetPosition());

		delete(pMenu);
	}
}

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_RDClick(wxMouseEvent &event)
{
	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pTool )
		{
			g_pTool->Execute(_Get_Client2World(event.GetPosition()), TOOL_INTERACTIVE_RDCLICK, GET_KEYS(event));
		}
		break;

	//-----------------------------------------------------
	case MAP_MODE_DISTANCE:
		break;

	//-----------------------------------------------------
	case MAP_MODE_ZOOM:
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN:
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN_DOWN:
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_MDown(wxMouseEvent &event)
{
	bool	bCaptureMouse	= true;

	m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pTool )
		{
			g_pTool->Execute(_Get_Client2World(event.GetPosition()), TOOL_INTERACTIVE_MDOWN, GET_KEYS(event));
		}
		break;

	//-----------------------------------------------------
	case MAP_MODE_DISTANCE:
		break;

	//-----------------------------------------------------
	case MAP_MODE_ZOOM:
		m_Drag_Mode		= TOOL_INTERACTIVE_DRAG_NONE;
		Set_Mode(MAP_MODE_PAN_DOWN);
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN:
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN_DOWN:
		m_Drag_Mode		= TOOL_INTERACTIVE_DRAG_NONE;
		break;
	}

	//-----------------------------------------------------
	if( bCaptureMouse && !HasCapture() )
	{
		CaptureMouse();
	}

	event.Skip();
}

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_MUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	_Draw_Inverse(m_Mouse_Down, event.GetPosition());
	m_Drag_Mode	= TOOL_INTERACTIVE_DRAG_NONE;

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		break;

	//-----------------------------------------------------
	case MAP_MODE_DISTANCE:
		break;

	//-----------------------------------------------------
	case MAP_MODE_ZOOM:
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN:
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN_DOWN:
		Set_Mode(MAP_MODE_ZOOM);
		_Move(m_Mouse_Down, event.GetPosition());
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_Motion(wxMouseEvent &event)
{
	wxPoint	Point	= event.GetPosition();

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pTool )
		{
			TSG_Tool_Interactive_Mode	iMode
				= event.LeftIsDown  () ? TOOL_INTERACTIVE_MOVE_LDOWN
				: event.MiddleIsDown() ? TOOL_INTERACTIVE_MOVE_MDOWN
				: event.RightIsDown () ? TOOL_INTERACTIVE_MOVE_RDOWN : TOOL_INTERACTIVE_MOVE;

			g_pTool->Execute(_Get_Client2World(Point), iMode, GET_KEYS(event));
		}
		else if( m_pMap->Find_Active(true) )
		{
			Get_Active_Layer()->Edit_On_Mouse_Move(
				this, m_pMap->Get_World(GetClientSize()),
				Point, m_Mouse_Move,
				GET_KEYS(event)
			);
		}
		break;

	//-----------------------------------------------------
	case MAP_MODE_DISTANCE:
		if( m_Distance_Pts.Get_Count() > 0 )
		{
			int			n	= m_Distance_Pts.Get_Count();
			wxClientDC	dc(this);
			wxPoint		Last(_Get_World2Client(m_Distance_Pts[n - 1]));
			dc.SetLogicalFunction(wxINVERT);
			dc.DrawLine(Last.x, Last.y, m_Mouse_Move.x, m_Mouse_Move.y);
			dc.DrawLine(Last.x, Last.y,        Point.x,        Point.y);
			m_Distance_Move	= SG_Get_Distance(m_Distance_Pts[n - 1], _Get_Client2World(Point));
		}
		break;

	//-----------------------------------------------------
	case MAP_MODE_ZOOM:
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN:
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN_DOWN:
	//	_Move(m_Mouse_Down, Point);
		_Pan(m_Mouse_Down, Point);
		break;
	}

	//-----------------------------------------------------
	if( m_Mode != MAP_MODE_PAN_DOWN )
	{
		m_pParent->Ruler_Set_Position(Point.x, Point.y);

		m_pMap->Set_Mouse_Position(_Get_Client2World(Point));

		_Set_StatusBar(_Get_Client2World(Point));
	}

	//-----------------------------------------------------
	_Draw_Inverse(m_Mouse_Down, m_Mouse_Move, Point);

	m_Mouse_Move	= Point;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_Wheel(wxMouseEvent &event)
{
	if( event.GetWheelRotation() < 0 )
	{
		_Zoom(_Get_Client2World(event.GetPosition()), false);
	}
	else if( event.GetWheelRotation() > 0 )
	{
		_Zoom(_Get_Client2World(event.GetPosition()), true);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_Lost(wxMouseCaptureLostEvent &event)
{
	Set_CrossHair_Off();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
