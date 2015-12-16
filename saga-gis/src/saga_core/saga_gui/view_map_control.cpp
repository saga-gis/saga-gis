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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include <wx/window.h>
#include <wx/image.h>
#include <wx/dcclient.h>

#include "res_images.h"

#include "helper.h"
#include "dc_helper.h"

#include "wksp_map.h"
#include "wksp_module.h"
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
#define GET_KEYS(e)	((e.LeftIsDown()   ? MODULE_INTERACTIVE_KEY_LEFT   : 0)\
					|(e.MiddleIsDown() ? MODULE_INTERACTIVE_KEY_MIDDLE : 0)\
					|(e.RightIsDown()  ? MODULE_INTERACTIVE_KEY_RIGHT  : 0)\
					|(e.ShiftDown()    ? MODULE_INTERACTIVE_KEY_SHIFT  : 0)\
					|(e.AltDown()      ? MODULE_INTERACTIVE_KEY_ALT    : 0)\
					|(e.ControlDown()  ? MODULE_INTERACTIVE_KEY_CTRL   : 0))

//---------------------------------------------------------
#define STANDARD_ZOOM_FACTOR	50


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
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

	m_Drag_Mode	= MODULE_INTERACTIVE_DRAG_NONE;
}

//---------------------------------------------------------
CVIEW_Map_Control::~CVIEW_Map_Control(void)
{
	_Distance_Reset();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
inline void CVIEW_Map_Control::_Set_StatusBar(CSG_Point ptWorld)
{
	static bool	bBuisy	= false;

	if( bBuisy == false )
	{
		bBuisy	= true;

		CSG_Module	*pProjector	= NULL;

		if( m_pMap->Get_Parameter("GCS_POSITION")->asBool() && m_pMap->Get_Projection().is_Okay() && (pProjector = SG_Get_Module_Library_Manager().Get_Module("pj_proj4", 2)) != NULL )	// Coordinate Transformation (Shapes)
		{
			if( pProjector->is_Executing() )
			{
				pProjector	= NULL;
			}
			else
			{
				SG_UI_Progress_Lock(true);
				SG_UI_Msg_Lock     (true);

				CSG_Shapes	prj(SHAPE_TYPE_Point), gcs(SHAPE_TYPE_Point); prj.Add_Shape()->Add_Point(ptWorld); prj.Get_Projection().Assign(m_pMap->Get_Projection());

				pProjector->Settings_Push(NULL);

				if( pProjector->Set_Parameter("CRS_PROJ4", SG_T("+proj=longlat +ellps=WGS84 +datum=WGS84"))
				&&  pProjector->Set_Parameter("SOURCE"   , &prj)
				&&  pProjector->Set_Parameter("TARGET"   , &gcs)
				&&  pProjector->Execute() )
				{
					ptWorld	= gcs.Get_Shape(0)->Get_Point(0);

					STATUSBAR_Set_Text(wxString::Format("X %s", SG_Double_To_Degree(ptWorld.Get_X()).c_str()), STATUSBAR_VIEW_X);
					STATUSBAR_Set_Text(wxString::Format("Y %s", SG_Double_To_Degree(ptWorld.Get_Y()).c_str()), STATUSBAR_VIEW_Y);

					pProjector->Settings_Pop();
				}
				else
				{
					pProjector->Settings_Pop();		pProjector	= NULL;
				}

				SG_UI_Progress_Lock(false);
				SG_UI_Msg_Lock     (false);
			}
		}

		if( !pProjector )
		{
			STATUSBAR_Set_Text(wxString::Format("X %f", ptWorld.Get_X()), STATUSBAR_VIEW_X);
			STATUSBAR_Set_Text(wxString::Format("Y %f", ptWorld.Get_Y()), STATUSBAR_VIEW_Y);
		}

		if( m_Mode == MAP_MODE_DISTANCE )
		{
			STATUSBAR_Set_Text(wxString::Format(wxT("D %f"), m_Distance + m_Distance_Move), STATUSBAR_VIEW_Z);
		}
		else if( Get_Active_Layer() )
		{
			STATUSBAR_Set_Text(wxString::Format(wxT("Z %s"), Get_Active_Layer()->Get_Value(ptWorld, _Get_World(2.0)).c_str()), STATUSBAR_VIEW_Z);
		}
		else
		{
			STATUSBAR_Set_Text(wxT("Z"), STATUSBAR_VIEW_Z);
		}

		bBuisy	= false;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline wxRect CVIEW_Map_Control::_Get_Client(void)
{
	return( wxRect(wxPoint(0, 0), GetClientSize()) );
}

//---------------------------------------------------------
inline wxPoint CVIEW_Map_Control::_Get_Client(TSG_Point Point)
{
	wxRect		rClient(_Get_Client());
	CSG_Rect	rWorld(m_pMap->Get_World(rClient));
	double		World2DC	= (double)rClient.GetWidth() / rWorld.Get_XRange();

	return( wxPoint(          (int)(0.5 + (Point.x - rWorld.Get_XMin()) * World2DC),
		rClient.GetHeight() - (int)(0.5 + (Point.y - rWorld.Get_YMin()) * World2DC)
	));
}

//---------------------------------------------------------
inline CSG_Point CVIEW_Map_Control::_Get_World(wxPoint ptClient)
{
	return( m_pMap->Get_World(_Get_Client(), ptClient) );
}

//---------------------------------------------------------
inline double CVIEW_Map_Control::_Get_World(double xClient)
{
	wxRect		rClient(_Get_Client());
	CSG_Rect	rWorld(m_pMap->Get_World(rClient));

	return( xClient * rWorld.Get_XRange() / (double)rClient.GetWidth() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::_Draw_Inverse(wxPoint ptA, wxPoint ptB)
{
	if( m_Drag_Mode != MODULE_INTERACTIVE_DRAG_NONE )
	{
		wxClientDC	dc(this);

		dc.SetLogicalFunction(wxINVERT);

		switch( m_Drag_Mode )
		{
		case MODULE_INTERACTIVE_DRAG_LINE:
			dc.DrawLine			(ptA.x, ptA.y, ptB.x, ptB.y);
			break;

		case MODULE_INTERACTIVE_DRAG_BOX:
			dc.DrawRectangle	(ptA.x, ptA.y, ptB.x - ptA.x, ptB.y - ptA.y);
			break;

		case MODULE_INTERACTIVE_DRAG_CIRCLE:
			dc.DrawCircle		(ptA.x, ptA.y, (int)SG_Get_Distance(ptA.x, ptA.y, ptB.x, ptB.y));
			break;
		}
	}
}

//---------------------------------------------------------
void CVIEW_Map_Control::_Draw_Inverse(wxPoint ptA, wxPoint ptB_Old, wxPoint ptB_New)
{
	if( m_Drag_Mode != MODULE_INTERACTIVE_DRAG_NONE )
	{
		wxClientDC	dc(this);

		dc.SetLogicalFunction(wxINVERT);

		switch( m_Drag_Mode )
		{
		case MODULE_INTERACTIVE_DRAG_LINE:
			dc.DrawLine			(ptA.x, ptA.y, ptB_Old.x, ptB_Old.y);
			dc.DrawLine			(ptA.x, ptA.y, ptB_New.x, ptB_New.y);
			break;

		case MODULE_INTERACTIVE_DRAG_BOX:
			dc.DrawRectangle	(ptA.x, ptA.y, ptB_Old.x - ptA.x, ptB_Old.y - ptA.y);
			dc.DrawRectangle	(ptA.x, ptA.y, ptB_New.x - ptA.x, ptB_New.y - ptA.y);
			break;

		case MODULE_INTERACTIVE_DRAG_CIRCLE:
			dc.DrawCircle		(ptA.x, ptA.y, (int)SG_Get_Distance(ptA.x, ptA.y, ptB_Old.x, ptB_Old.y));
			dc.DrawCircle		(ptA.x, ptA.y, (int)SG_Get_Distance(ptA.x, ptA.y, ptB_New.x, ptB_New.y));
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Map_Control::_Zoom(wxPoint A, wxPoint B)
{
	CSG_Rect	rWorld;

	if( A.x == B.x && A.y == B.y )
	{
		return( _Zoom(_Get_World(A), true) );
	}

	rWorld.Assign(_Get_World(A), _Get_World(B));

	m_pMap->Set_Extent(rWorld);

	return( true );
}

bool CVIEW_Map_Control::_Zoom(CSG_Point ptCenter, bool bZoomIn)
{
	CSG_Rect	rWorld(m_pMap->Get_Extent());

	rWorld.Move(ptCenter - rWorld.Get_Center());

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Map_Control::_Pan(wxPoint A, wxPoint B)
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
bool CVIEW_Map_Control::_Move(wxPoint &A, wxPoint B)
{
	if( A.x != B.x || A.y != B.y )
	{
		CSG_Rect	rWorld;

		rWorld.Assign(m_pMap->Get_Extent());
		rWorld.Move(_Get_World(A) - _Get_World(B));
		m_pMap->Set_Extent(rWorld);

		A	= B;

		return( true );
	}

	return( false );
}

bool CVIEW_Map_Control::_Move(wxPoint ptMove)
{
	return( _Move(ptMove, wxPoint(0, 0)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
void CVIEW_Map_Control::_Distance_Add(wxPoint Point)
{
	int		n	= m_Distance_Pts.Get_Count();

	m_Distance_Pts.Add(_Get_World(Point));

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
		for(i=1, A=_Get_Client(m_Distance_Pts[0]); i<n; i++)
		{
			B	= A;
			A	= _Get_Client(m_Distance_Pts[i]);
			dc.DrawLine(A.x, A.y, B.x, B.y);
		}

		dc.SetPen(wxPen(*wxBLACK, 2));
		for(i=1, A=_Get_Client(m_Distance_Pts[0]); i<n; i++)
		{
			B	= A;
			A	= _Get_Client(m_Distance_Pts[i]);
			dc.DrawLine(A.x, A.y, B.x, B.y);
		}

		dc.SetPen(wxNullPen);
		dc.SetLogicalFunction(wxINVERT);
		A	= _Get_Client(m_Distance_Pts[n - 1]);
		dc.DrawLine(A.x, A.y, m_Mouse_Move.x, m_Mouse_Move.y);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
	}
}

//---------------------------------------------------------
void CVIEW_Map_Control::On_Size(wxSizeEvent &event)
{
	wxRect	r(_Get_Client());

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
		m_pMap->Draw_Map(dc, 1.0, _Get_Client());
		dc.SelectObject(wxNullBitmap);

		m_pParent->Ruler_Refresh();

		Refresh(false);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Key_Down(wxKeyEvent &event)
{
	if( m_pMap->Find_Layer(Get_Active_Layer()) && Get_Active_Layer()->Edit_On_Key_Down(event.GetKeyCode()) )
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
//														 //
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
		if( g_pModule && g_pModule->is_Interactive() )
		{
			m_Drag_Mode		= ((CSG_Module_Interactive *)g_pModule->Get_Module())->Get_Drag_Mode();
			bCaptureMouse	= !g_pModule->Execute(_Get_World(event.GetPosition()), MODULE_INTERACTIVE_LDOWN, GET_KEYS(event));
		}
		else if( m_pMap->Find_Layer(Get_Active_Layer()) )
		{
			switch(	Get_Active_Layer()->Get_Type() )
			{
			default:
				m_Drag_Mode		= MODULE_INTERACTIVE_DRAG_NONE;
				break;

			case WKSP_ITEM_Grid:
			case WKSP_ITEM_PointCloud:
				m_Drag_Mode		= MODULE_INTERACTIVE_DRAG_BOX;
				break;

			case WKSP_ITEM_Shapes:
				m_Drag_Mode		= ((CWKSP_Shapes *)Get_Active_Layer())->is_Editing()
								? MODULE_INTERACTIVE_DRAG_NONE
								: MODULE_INTERACTIVE_DRAG_BOX;
				break;
			}

			Get_Active_Layer()->Edit_On_Mouse_Down(_Get_World(event.GetPosition()), _Get_World(1.0), GET_KEYS(event));
		}
		break;

	//-----------------------------------------------------
	case MAP_MODE_DISTANCE:
		m_Drag_Mode		= MODULE_INTERACTIVE_DRAG_NONE;
		break;

	//-----------------------------------------------------
	case MAP_MODE_ZOOM:
		m_Drag_Mode		= MODULE_INTERACTIVE_DRAG_BOX;
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN:
		m_Drag_Mode		= MODULE_INTERACTIVE_DRAG_NONE;
		Set_Mode(MAP_MODE_PAN_DOWN);
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN_DOWN:
		m_Drag_Mode		= MODULE_INTERACTIVE_DRAG_NONE;
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
	m_Drag_Mode	= MODULE_INTERACTIVE_DRAG_NONE;

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pModule )
		{
			g_pModule->Execute(_Get_World(event.GetPosition()), MODULE_INTERACTIVE_LUP, GET_KEYS(event));
		}
		else if( m_pMap->Find_Layer(Get_Active_Layer()) )
		{
			Get_Active_Layer()->Edit_On_Mouse_Up(_Get_World(event.GetPosition()), _Get_World(1.0), GET_KEYS(event)|MODULE_INTERACTIVE_KEY_LEFT);
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
		if( g_pModule )
		{
			g_pModule->Execute(_Get_World(event.GetPosition()), MODULE_INTERACTIVE_LDCLICK, GET_KEYS(event));
		}
		break;

	//-----------------------------------------------------
	default:
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_RDown(wxMouseEvent &event)
{
	m_pParent->Activate();

	m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();

	_Draw_Inverse(m_Mouse_Down, event.GetPosition());
	m_Drag_Mode	= MODULE_INTERACTIVE_DRAG_NONE;

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pModule )
		{
			g_pModule->Execute(_Get_World(event.GetPosition()), MODULE_INTERACTIVE_RDOWN, GET_KEYS(event));
		}
		else if( m_pMap->Find_Layer(Get_Active_Layer()) )
		{
			Get_Active_Layer()->Edit_On_Mouse_Down(_Get_World(event.GetPosition()), _Get_World(1.0), GET_KEYS(event));
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

	m_Drag_Mode		= MODULE_INTERACTIVE_DRAG_NONE;

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pModule )
		{
			g_pModule->Execute(_Get_World(event.GetPosition()), MODULE_INTERACTIVE_RUP, GET_KEYS(event));
		}
		else if( m_pMap->Find_Layer(Get_Active_Layer()) && !Get_Active_Layer()->Edit_On_Mouse_Up(_Get_World(event.GetPosition()), _Get_World(1.0), GET_KEYS(event)|MODULE_INTERACTIVE_KEY_RIGHT) )
		{
			pMenu	= Get_Active_Layer()->Edit_Get_Menu();
		}
		else
		{
			pMenu	= m_pParent->_Create_Menu();
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
			_Zoom(_Get_World(event.GetPosition()), false);
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
		if( g_pModule )
		{
			g_pModule->Execute(_Get_World(event.GetPosition()), MODULE_INTERACTIVE_RDCLICK, GET_KEYS(event));
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
//														 //
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
		if( g_pModule )
		{
			g_pModule->Execute(_Get_World(event.GetPosition()), MODULE_INTERACTIVE_MDOWN, GET_KEYS(event));
		}
		break;

	//-----------------------------------------------------
	case MAP_MODE_DISTANCE:
		break;

	//-----------------------------------------------------
	case MAP_MODE_ZOOM:
		m_Drag_Mode		= MODULE_INTERACTIVE_DRAG_NONE;
		Set_Mode(MAP_MODE_PAN_DOWN);
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN:
		break;

	//-----------------------------------------------------
	case MAP_MODE_PAN_DOWN:
		m_Drag_Mode		= MODULE_INTERACTIVE_DRAG_NONE;
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
	m_Drag_Mode	= MODULE_INTERACTIVE_DRAG_NONE;

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_Motion(wxMouseEvent &event)
{
	if( m_Mode != MAP_MODE_PAN_DOWN )
	{
		_Set_StatusBar(_Get_World(event.GetPosition()));

		m_pParent->Ruler_Set_Position(event.GetPosition().x, event.GetPosition().y);
	}

	switch( m_Mode )
	{
	//-----------------------------------------------------
	case MAP_MODE_SELECT:
		if( g_pModule )
		{
			TSG_Module_Interactive_Mode	iMode
				= event.LeftIsDown()	? MODULE_INTERACTIVE_MOVE_LDOWN
				: event.MiddleIsDown()	? MODULE_INTERACTIVE_MOVE_MDOWN
				: event.RightIsDown()	? MODULE_INTERACTIVE_MOVE_RDOWN
										: MODULE_INTERACTIVE_MOVE;

			g_pModule->Execute(_Get_World(event.GetPosition()), iMode, GET_KEYS(event));
		}
		else if( m_pMap->Find_Layer(Get_Active_Layer()) )
		{
			Get_Active_Layer()->Edit_On_Mouse_Move(
				this, m_pMap->Get_World(_Get_Client()),
				event.GetPosition(), m_Mouse_Move,
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
			wxPoint		Last(_Get_Client(m_Distance_Pts[n - 1]));
			dc.SetLogicalFunction(wxINVERT);
			dc.DrawLine(Last.x, Last.y, m_Mouse_Move       .x, m_Mouse_Move       .y);
			dc.DrawLine(Last.x, Last.y, event.GetPosition().x, event.GetPosition().y);
			m_Distance_Move	= SG_Get_Distance(m_Distance_Pts[n - 1], _Get_World(event.GetPosition()));
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
	//	_Move(m_Mouse_Down, event.GetPosition());
		_Pan(m_Mouse_Down, event.GetPosition());
		break;
	}

	//-----------------------------------------------------
	_Draw_Inverse(m_Mouse_Down, m_Mouse_Move, event.GetPosition());

	//-----------------------------------------------------
	m_Mouse_Move	= event.GetPosition();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_Wheel(wxMouseEvent &event)
{
	if( event.GetWheelRotation() < 0 )
	{
		_Zoom(_Get_World(event.GetPosition()), false);
	}
	else if( event.GetWheelRotation() > 0 )
	{
		_Zoom(_Get_World(event.GetPosition()), true);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_Control::On_Mouse_Lost(wxMouseCaptureLostEvent &event)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
