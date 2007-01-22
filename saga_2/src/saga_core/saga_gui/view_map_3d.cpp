
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
//                   VIEW_Map_3D.cpp                     //
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
#include <wx/wx.h>
#include <wx/window.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_dialogs.h"
#include "res_images.h"

#include "helper.h"

#include "wksp_data_manager.h"
#include "wksp_map.h"

#include "view_map_3d.h"
#include "view_map_3d_image.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	MAP3D_STATUSBAR_ROTATE_X	= 0,
	MAP3D_STATUSBAR_ROTATE_Y,
	MAP3D_STATUSBAR_ROTATE_Z,
	MAP3D_STATUSBAR_SHIFT_X,
	MAP3D_STATUSBAR_SHIFT_Y,
	MAP3D_STATUSBAR_SHIFT_Z,
	MAP3D_STATUSBAR_EXAGGERATION,
	MAP3D_STATUSBAR_CENTRAL,
	MAP3D_STATUSBAR_COUNT
};

//---------------------------------------------------------
enum
{
	PLAY_MODE_NONE	= 0,
	PLAY_MODE_RUN_ONCE,
	PLAY_MODE_RUN_LOOP,
	PLAY_MODE_RUN_SAVE
};

//---------------------------------------------------------
enum
{
	PLAY_REC_ROTATE_X	= 0,
	PLAY_REC_ROTATE_Y,
	PLAY_REC_ROTATE_Z,
	PLAY_REC_SHIFT_X,
	PLAY_REC_SHIFT_Y,
	PLAY_REC_SHIFT_Z,
	PLAY_REC_EXAGGERATION,
	PLAY_REC_CENTRAL,
	PLAY_REC_STEPS
};

//---------------------------------------------------------
#define STEP_ROTATE		( 4.0 * M_DEG_TO_RAD)
#define STEP_SHIFT		(10.0)
#define STEP_EXAGGERATE	( 0.5)
#define STEP_CENTRAL	(10.0)
#define STEP_STEREO		( 0.5 * M_DEG_TO_RAD)
#define STEP_SRC_RES	(50)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Map_3D, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Map_3D, CVIEW_Base)
	EVT_PAINT			(CVIEW_Map_3D::On_Paint)
	EVT_SIZE			(CVIEW_Map_3D::On_Size)
	EVT_KEY_DOWN		(CVIEW_Map_3D::On_Key_Down)
	EVT_LEFT_DOWN		(CVIEW_Map_3D::On_Mouse_LDown)
	EVT_LEFT_UP			(CVIEW_Map_3D::On_Mouse_LUp)
	EVT_RIGHT_DOWN		(CVIEW_Map_3D::On_Mouse_RDown)
	EVT_RIGHT_UP		(CVIEW_Map_3D::On_Mouse_RUp)
	EVT_MOTION			(CVIEW_Map_3D::On_Mouse_Motion)
	EVT_MENU_RANGE		(ID_CMD_MAP3D_FIRST, ID_CMD_MAP3D_LAST, CVIEW_Map_3D::On_Command)
	EVT_UPDATE_UI_RANGE	(ID_CMD_MAP3D_FIRST, ID_CMD_MAP3D_LAST, CVIEW_Map_3D::On_Command_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Map_3D::CVIEW_Map_3D(CWKSP_Map *pMap)
	: CVIEW_Base(ID_VIEW_MAP_3D, wxString::Format(wxT("%s [%s]"), LNG("[CAP] 3D-View"), pMap->Get_Name().c_str()), ID_IMG_WND_MAP3D, CVIEW_Map_3D::_Create_Menu(), LNG("[CAP] 3D-View"))
{
	CreateStatusBar(MAP3D_STATUSBAR_COUNT);

	//-----------------------------------------------------
	m_pMap		= pMap;
	m_pImage	= new CVIEW_Map_3D_Image(this, pMap);

	//-----------------------------------------------------
	m_Play_Mode	= PLAY_MODE_NONE;

	m_Play.Add_Field(LNG("[FLD] Rotate X")			, TABLE_FIELDTYPE_Double);
	m_Play.Add_Field(LNG("[FLD] Rotate Y")			, TABLE_FIELDTYPE_Double);
	m_Play.Add_Field(LNG("[FLD] Rotate Z")			, TABLE_FIELDTYPE_Double);
	m_Play.Add_Field(LNG("[FLD] Shift X")			, TABLE_FIELDTYPE_Double);
	m_Play.Add_Field(LNG("[FLD] Shift Y")			, TABLE_FIELDTYPE_Double);
	m_Play.Add_Field(LNG("[FLD] Shift Z")			, TABLE_FIELDTYPE_Double);
	m_Play.Add_Field(LNG("[FLD] Exaggeration")		, TABLE_FIELDTYPE_Double);
	m_Play.Add_Field(LNG("[FLD] Central Projection"), TABLE_FIELDTYPE_Double);
	m_Play.Add_Field(LNG("[FLD] Steps to Next")		, TABLE_FIELDTYPE_Int);

	//-----------------------------------------------------
	_Parms_Create();

	_Parms_Dlg();
}

//---------------------------------------------------------
CVIEW_Map_3D::~CVIEW_Map_3D(void)
{
	_Play_Stop();

	delete(m_pImage);

	m_pMap->View_Closes(this);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Map_3D::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(), *pMenu_Sub;

	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_PARAMETERS);

	pMenu_Sub	= new wxMenu();
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_X_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_X_MORE);
//	pMenu_Sub->AppendSeparator();
//	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_Y_LESS);
//	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_Y_MORE);
	pMenu_Sub->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_Z_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_Z_MORE);
	pMenu->Append(ID_CMD_MAP3D_FIRST, LNG("[MNU] Rotation"), pMenu_Sub);

	pMenu_Sub	= new wxMenu();
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_X_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_X_MORE);
	pMenu_Sub->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_Y_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_Y_MORE);
	pMenu_Sub->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_Z_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_Z_MORE);
	pMenu->Append(ID_CMD_MAP3D_FIRST, LNG("[MNU] Shift"), pMenu_Sub);

	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_EXAGGERATE_LESS);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_EXAGGERATE_MORE);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu    ,  true, ID_CMD_MAP3D_CENTRAL);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_CENTRAL_LESS);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_CENTRAL_MORE);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu    ,  true, ID_CMD_MAP3D_INTERPOLATED);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu    ,  true, ID_CMD_MAP3D_STEREO);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_STEREO_LESS);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_STEREO_MORE);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_SAVE);

	pMenu_Sub	= new wxMenu();
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_POS_ADD);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_POS_DEL);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_POS_DEL_ALL);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_POS_EDIT);
	pMenu_Sub->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_PLAY);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_PLAY_LOOP);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_SAVE);
	pMenu->Append(ID_CMD_MAP3D_FIRST, LNG("[MNU] Sequencer"), pMenu_Sub);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Map_3D::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_MAP_3D);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_PARAMETERS);
	CMD_ToolBar_Add_Item(pToolBar,  true, ID_CMD_MAP3D_INTERPOLATED);
	CMD_ToolBar_Add_Item(pToolBar,  true, ID_CMD_MAP3D_STEREO);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_ROTATE_X_LESS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_ROTATE_X_MORE);
//	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_ROTATE_Y_LESS);
//	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_ROTATE_Y_MORE);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_ROTATE_Z_LESS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_ROTATE_Z_MORE);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_SHIFT_X_LESS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_SHIFT_X_MORE);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_SHIFT_Y_LESS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_SHIFT_Y_MORE);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_SHIFT_Z_LESS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_SHIFT_Z_MORE);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_EXAGGERATE_LESS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_EXAGGERATE_MORE);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_CENTRAL_LESS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_CENTRAL_MORE);

	CMD_ToolBar_Add(pToolBar, LNG("[CAP] 3D-View"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::On_Size(wxSizeEvent &event)
{
	m_pImage->Set_Image(GetClientSize().x, GetClientSize().y);

	event.Skip();
}

//---------------------------------------------------------
void CVIEW_Map_3D::On_Paint(wxPaintEvent &event)
{
	wxPaintDC	dc(this);

	_Paint(dc);
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Paint(void)
{
	wxClientDC	dc(this);

	_Paint(dc);
}

void CVIEW_Map_3D::_Paint(wxDC &dc)
{
	if( m_pImage->Get_Image().Ok() )
	{
		dc.DrawBitmap(wxBitmap(m_pImage->Get_Image()), 0, 0, true);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:
		event.Skip();
		break;

	case WXK_F10:		_Parms_Command(ID_CMD_MAP3D_PARAMETERS);		break;

	case WXK_UP:		_Parms_Command(ID_CMD_MAP3D_ROTATE_X_LESS);		break;
	case WXK_DOWN:		_Parms_Command(ID_CMD_MAP3D_ROTATE_X_MORE);		break;

	case WXK_F3:		_Parms_Command(ID_CMD_MAP3D_ROTATE_Y_LESS);		break;
	case WXK_F4:		_Parms_Command(ID_CMD_MAP3D_ROTATE_Y_MORE);		break;

	case WXK_LEFT:		_Parms_Command(ID_CMD_MAP3D_ROTATE_Z_LESS);		break;
	case WXK_RIGHT:		_Parms_Command(ID_CMD_MAP3D_ROTATE_Z_MORE);		break;

	case WXK_INSERT:	_Parms_Command(ID_CMD_MAP3D_SHIFT_X_LESS);		break;
	case WXK_DELETE:	_Parms_Command(ID_CMD_MAP3D_SHIFT_X_MORE);		break;

	case WXK_HOME:		_Parms_Command(ID_CMD_MAP3D_SHIFT_Y_LESS);		break;
	case WXK_END:		_Parms_Command(ID_CMD_MAP3D_SHIFT_Y_MORE);		break;

	case WXK_PRIOR:		_Parms_Command(ID_CMD_MAP3D_SHIFT_Z_LESS);		break;
	case WXK_NEXT:		_Parms_Command(ID_CMD_MAP3D_SHIFT_Z_MORE);		break;

	case WXK_F1:		_Parms_Command(ID_CMD_MAP3D_EXAGGERATE_LESS);	break;
	case WXK_F2:		_Parms_Command(ID_CMD_MAP3D_EXAGGERATE_MORE);	break;

	case 'C':			_Parms_Command(ID_CMD_MAP3D_CENTRAL);			break;
	case WXK_F5:		_Parms_Command(ID_CMD_MAP3D_CENTRAL_LESS);		break;
	case WXK_F6:		_Parms_Command(ID_CMD_MAP3D_CENTRAL_MORE);		break;

	case 'S':			_Parms_Command(ID_CMD_MAP3D_STEREO);			break;
//	case WXK_F5:		_Parms_Command(ID_CMD_MAP3D_STEREO_LESS);		break;
//	case WXK_F6:		_Parms_Command(ID_CMD_MAP3D_STEREO_MORE);		break;

	case 'I':			_Parms_Command(ID_CMD_MAP3D_INTERPOLATED);		break;

	case WXK_F7:		_Parms_Command(ID_CMD_MAP3D_SRC_RES_LESS);		break;
	case WXK_F8:		_Parms_Command(ID_CMD_MAP3D_SRC_RES_MORE);		break;

	case 'A':			_Parms_Command(ID_CMD_MAP3D_SEQ_POS_ADD);		break;
	case 'D':			_Parms_Command(ID_CMD_MAP3D_SEQ_POS_DEL);		break;
	case 'P':			_Parms_Command(ID_CMD_MAP3D_SEQ_PLAY);			break;
	case 'L':			_Parms_Command(ID_CMD_MAP3D_SEQ_PLAY_LOOP);		break;
	case WXK_ESCAPE:	_Play_Stop();	event.Skip();					break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_MOUSE_X_RELDIFF	((double)(m_Mouse_Down.x - event.GetX()) / (double)GetClientSize().x)
#define GET_MOUSE_Y_RELDIFF	((double)(m_Mouse_Down.y - event.GetY()) / (double)GetClientSize().y)

//---------------------------------------------------------
void CVIEW_Map_3D::On_Mouse_LDown(wxMouseEvent &event)
{
	m_Mouse_Down	= event.GetPosition();
	m_xDown			= m_pImage->m_zRotate;
	m_yDown			= m_pImage->m_xRotate;

	CaptureMouse();
}

//---------------------------------------------------------
void CVIEW_Map_3D::On_Mouse_LUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Mouse_Down.x != event.GetX() || m_Mouse_Down.y != event.GetY() )
	{
		m_pImage->m_zRotate	= m_xDown - GET_MOUSE_X_RELDIFF * M_PI_180;
		m_pImage->m_xRotate	= m_yDown - GET_MOUSE_Y_RELDIFF * M_PI_180;

		_Parms_Changed();
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D::On_Mouse_RDown(wxMouseEvent &event)
{
	m_Mouse_Down	= event.GetPosition();
	m_xDown			= m_pImage->m_yShift;
	m_yDown			= m_pImage->m_zShift;

	CaptureMouse();
}

//---------------------------------------------------------
void CVIEW_Map_3D::On_Mouse_RUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Mouse_Down.x != event.GetX() || m_Mouse_Down.y != event.GetY() )
	{
		m_pImage->m_yShift	= m_xDown - GET_MOUSE_X_RELDIFF * 100.0;
		m_pImage->m_zShift	= m_yDown + GET_MOUSE_Y_RELDIFF * 100.0;

		_Parms_Changed();
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D::On_Mouse_Motion(wxMouseEvent &event)
{
	if( HasCapture() && event.Dragging() )
	{
		if( event.LeftIsDown() )
		{
			m_pImage->m_zRotate	= m_xDown - GET_MOUSE_X_RELDIFF * M_PI_180;
			m_pImage->m_xRotate	= m_yDown - GET_MOUSE_Y_RELDIFF * M_PI_180;

			_Parms_Changed();
		}
		else if( event.RightIsDown() )
		{
			m_pImage->m_yShift	= m_xDown - GET_MOUSE_X_RELDIFF * 100.0;
			m_pImage->m_zShift	= m_yDown + GET_MOUSE_Y_RELDIFF * 100.0;

			_Parms_Changed();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::On_Command(wxCommandEvent &event)
{
	_Parms_Command(event.GetId());
}

//---------------------------------------------------------
void CVIEW_Map_3D::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_MAP3D_INTERPOLATED:
		event.Check(m_pImage->m_bInterpol);
		break;

	case ID_CMD_MAP3D_CENTRAL:
		event.Check(m_pImage->m_bCentral);
		break;

	case ID_CMD_MAP3D_STEREO:
		event.Check(m_pImage->m_bStereo);
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::On_Source_Changed(void)
{
	m_pImage->m_Src_bUpdate	= true;

	m_pImage->Set_Source();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::_Parms_Create(void)
{
	CSG_Parameter	*pNode;

	m_Parameters.Create(NULL, LNG("[CAP] 3D-View"), LNG(""));

	//-----------------------------------------------------
	m_Parameters.Add_Grid(
		NULL	, "ELEVATION"		, LNG("[CAP] Elevation"),
		LNG(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "ROTATION", LNG("[CAP] Rotation"), LNG(""));

	m_Parameters.Add_Value(
		pNode	, "ROTATION_X"		, LNG("X"),
		LNG(""),
		PARAMETER_TYPE_Double, -45.0, -360.0, true, 360.0, true
	);

	m_Parameters.Add_Value(
		pNode	, "ROTATION_Y"		, LNG("Y"),
		LNG(""),
		PARAMETER_TYPE_Double,   0.0, -360.0, true, 360.0, true
	);

	m_Parameters.Add_Value(
		pNode	, "ROTATION_Z"		, LNG("Z"),
		LNG(""),
		PARAMETER_TYPE_Double,  45.0, -360.0, true, 360.0, true
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "SHIFT", LNG("[CAP] Shift"), LNG(""));

	m_Parameters.Add_Value(
		pNode	, "SHIFT_X"			, LNG("Left/Right"),
		LNG(""),
		PARAMETER_TYPE_Double, 0.0
	);

	m_Parameters.Add_Value(
		pNode	, "SHIFT_Y"			, LNG("Up/Down"),
		LNG(""),
		PARAMETER_TYPE_Double, 0.0
	);

	m_Parameters.Add_Value(
		pNode	, "SHIFT_Z"			, LNG("In/Out"),
		LNG(""),
		PARAMETER_TYPE_Double, 200.0
	);

	//-----------------------------------------------------
	m_Parameters.Add_Value(
		NULL	, "EXAGGERATION"	, LNG("[CAP] Exaggeration"),
		LNG(""),
		PARAMETER_TYPE_Double, 1.0
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_PROJECTION", LNG("[CAP] Projection"), LNG(""));

	m_Parameters.Add_Choice(
		pNode	, "CENTRAL"			, LNG("[CAP] Projection"),
		LNG(""),

		CSG_String::Format(wxT("%s|%s|"),
			LNG("parellel"),
			LNG("central")
		), 1
	);

	m_Parameters.Add_Value(
		pNode	, "CENTRAL_DIST"	, LNG("[CAP] Perspectivic Distance"),
		LNG(""),
		PARAMETER_TYPE_Double, 200, 1, true
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_FIGURE", LNG("[CAP] Figure"), LNG(""));

	m_Parameters.Add_Choice(
		pNode	, "FIGURE"			, LNG("[CAP] Figure"),
		LNG(""),

		CSG_String::Format(wxT("%s|%s|%s|%s|"),
			LNG("plain"),
			LNG("cylinder"),
			LNG("ball"),
			LNG("panorama")
		), 0
	);

	m_Parameters.Add_Value(
		pNode	, "FIGURE_WEIGHT"	, LNG("[CAP] Weight"),
		LNG(""),
		PARAMETER_TYPE_Double, 1.0
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_STEREO", LNG("[CAP] Anaglyph"), LNG(""));

	m_Parameters.Add_Value(
		pNode	, "STEREO"			, LNG("[CAP] Anaglyph"),
		LNG(""),
		PARAMETER_TYPE_Bool, 0.0
	);

	m_Parameters.Add_Value(
		pNode	, "STEREO_DIST"		, LNG("[CAP] Eye Distance [Degree]"),
		LNG(""),
		PARAMETER_TYPE_Double, 2.0, 0, true, 180, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Value(
		NULL	, "INTERPOLATED"	, LNG("[CAP] Interpolated"),
		LNG(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Value(
		NULL	, "BKGRD_COLOR"		, LNG("[CAP] Background Color"),
		LNG(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(0, 0, 0)
	);

	m_Parameters.Add_Value(
		NULL	, "SRC_RESOLUTION"	, LNG("Resolution"),
		LNG(""),
		PARAMETER_TYPE_Int, 200, 10, true
	);
}

//---------------------------------------------------------
bool CVIEW_Map_3D::_Parms_Update(bool bGet)
{
	return( _Parms_Update(bGet, m_pImage) );
}

bool CVIEW_Map_3D::_Parms_Update(bool bGet, CVIEW_Map_3D_Image *pImage)
{
	//-----------------------------------------------------
	if( bGet )
	{
		pImage->m_xShift		= m_Parameters("SHIFT_X")		->asDouble();
		pImage->m_yShift		= m_Parameters("SHIFT_Y")		->asDouble();
		pImage->m_zShift		= m_Parameters("SHIFT_Z")		->asDouble();

		pImage->m_xRotate		= m_Parameters("ROTATION_X")	->asDouble() * M_DEG_TO_RAD;
		pImage->m_yRotate		= m_Parameters("ROTATION_Y")	->asDouble() * M_DEG_TO_RAD;
		pImage->m_zRotate		= m_Parameters("ROTATION_Z")	->asDouble() * M_DEG_TO_RAD;

		pImage->m_Exaggeration	= m_Parameters("EXAGGERATION")	->asDouble();

		pImage->m_bCentral		= m_Parameters("CENTRAL")		->asInt() == 1;
		pImage->m_Central		= m_Parameters("CENTRAL_DIST")	->asDouble();

		pImage->m_Figure		= m_Parameters("FIGURE")		->asInt();
		pImage->m_Figure_Weight	= m_Parameters("FIGURE_WEIGHT")	->asDouble();

		pImage->m_bStereo		= m_Parameters("STEREO")		->asBool();
		pImage->m_Stereo		= m_Parameters("STEREO_DIST")	->asDouble() * M_DEG_TO_RAD;

		pImage->m_BkColor		= m_Parameters("BKGRD_COLOR")	->asColor();

		_Parms_StatusBar();

		//-------------------------------------------------
		CSG_Grid	*pDEM			= m_Parameters("ELEVATION")		->asGrid();

		if( !g_pData->Exists(pDEM, DATAOBJECT_TYPE_Grid) )
		{
			pDEM				= NULL;
		}

		pImage->m_Src_bUpdate	= pImage->m_pDEM != pDEM ? true : pImage->m_Src_bUpdate;
		pImage->m_pDEM			= pDEM;

		//-------------------------------------------------
		if( pImage->m_bInterpol != m_Parameters("INTERPOLATED")	->asBool() )
		{
			pImage->m_Src_bUpdate	= true;
			pImage->m_bInterpol		= !pImage->m_bInterpol;
		}

		//-------------------------------------------------
		pImage->Set_Source(m_Parameters("SRC_RESOLUTION")->asInt());

		return( pImage->m_pDEM != NULL );
	}

	//-----------------------------------------------------
	else
	{
		m_Parameters("SHIFT_X")			->Set_Value(pImage->m_xShift);
		m_Parameters("SHIFT_Y")			->Set_Value(pImage->m_yShift);
		m_Parameters("SHIFT_Z")			->Set_Value(pImage->m_zShift);

		m_Parameters("ROTATION_X")		->Set_Value(fmod(pImage->m_xRotate * M_RAD_TO_DEG, 360.0));
		m_Parameters("ROTATION_Y")		->Set_Value(fmod(pImage->m_yRotate * M_RAD_TO_DEG, 360.0));
		m_Parameters("ROTATION_Z")		->Set_Value(fmod(pImage->m_zRotate * M_RAD_TO_DEG, 360.0));

		m_Parameters("EXAGGERATION")	->Set_Value(pImage->m_Exaggeration);

		m_Parameters("CENTRAL")			->Set_Value(pImage->m_bCentral ? 1 : 0);
		m_Parameters("CENTRAL_DIST")	->Set_Value(pImage->m_Central);

		m_Parameters("STEREO")			->Set_Value(pImage->m_bStereo);
		m_Parameters("STEREO_DIST")		->Set_Value(pImage->m_Stereo * M_RAD_TO_DEG);

		_Parms_StatusBar();

		return( true );
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Parms_StatusBar(void)
{
	SetStatusText(wxString::Format(wxT("X %+.1f\xb0"), m_pImage->m_xRotate * M_RAD_TO_DEG)	, MAP3D_STATUSBAR_ROTATE_X);
	SetStatusText(wxString::Format(wxT("Y %+.1f\xb0"), m_pImage->m_yRotate * M_RAD_TO_DEG)	, MAP3D_STATUSBAR_ROTATE_Y);
	SetStatusText(wxString::Format(wxT("Z %+.1f\xb0"), m_pImage->m_zRotate * M_RAD_TO_DEG)	, MAP3D_STATUSBAR_ROTATE_Z);

	SetStatusText(wxString::Format(wxT("X %+.1f" ), m_pImage->m_xShift)					, MAP3D_STATUSBAR_SHIFT_X);
	SetStatusText(wxString::Format(wxT("Y %+.1f" ), m_pImage->m_yShift)					, MAP3D_STATUSBAR_SHIFT_Y);
	SetStatusText(wxString::Format(wxT("Z %+.1f" ), m_pImage->m_zShift)					, MAP3D_STATUSBAR_SHIFT_Z);

	SetStatusText(wxString::Format(wxT("E %.1f"  ), m_pImage->m_Exaggeration)			, MAP3D_STATUSBAR_EXAGGERATION);
	SetStatusText(wxString::Format(wxT("C %.1f"  ), m_pImage->m_bCentral ? m_pImage->m_Central : 0.0)	, MAP3D_STATUSBAR_CENTRAL);
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Parms_Command(int Command)
{
	switch( Command )
	{
	default:
		return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_SEQ_POS_ADD:		_Play_Pos_Add();	return;
	case ID_CMD_MAP3D_SEQ_POS_DEL:		_Play_Pos_Del();	return;
	case ID_CMD_MAP3D_SEQ_POS_DEL_ALL:	_Play_Pos_Clr();	return;
	case ID_CMD_MAP3D_SEQ_POS_EDIT:		_Play_Pos_Edit();	return;
	case ID_CMD_MAP3D_SEQ_PLAY:			_Play_Once();		return;
	case ID_CMD_MAP3D_SEQ_PLAY_LOOP:	_Play_Loop();		return;
	case ID_CMD_MAP3D_SEQ_SAVE:			_Play_Save();		return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_PARAMETERS:		_Parms_Dlg();		return;

	case ID_CMD_MAP3D_SAVE:				m_pImage->Save();	return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_INTERPOLATED:
		m_Parameters("INTERPOLATED")	->Set_Value(!m_pImage->m_bInterpol);

		_Parms_Update(true);
		return;

	case ID_CMD_MAP3D_SRC_RES_LESS:
		m_Parameters("SRC_RESOLUTION")	->Set_Value(m_pImage->m_Resolution - STEP_SRC_RES);

		_Parms_Update(true);
		return;

	case ID_CMD_MAP3D_SRC_RES_MORE:
		m_Parameters("SRC_RESOLUTION")	->Set_Value(m_pImage->m_Resolution + STEP_SRC_RES);

		_Parms_Update(true);
		return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_ROTATE_X_LESS:	m_pImage->m_xRotate			-= STEP_ROTATE;				break;
	case ID_CMD_MAP3D_ROTATE_X_MORE:	m_pImage->m_xRotate			+= STEP_ROTATE;				break;
	case ID_CMD_MAP3D_ROTATE_Y_LESS:	m_pImage->m_yRotate			-= STEP_ROTATE;				break;
	case ID_CMD_MAP3D_ROTATE_Y_MORE:	m_pImage->m_yRotate			+= STEP_ROTATE;				break;
	case ID_CMD_MAP3D_ROTATE_Z_LESS:	m_pImage->m_zRotate			-= STEP_ROTATE;				break;
	case ID_CMD_MAP3D_ROTATE_Z_MORE:	m_pImage->m_zRotate			+= STEP_ROTATE;				break;

	case ID_CMD_MAP3D_SHIFT_X_LESS:		m_pImage->m_xShift			-= STEP_SHIFT;				break;
	case ID_CMD_MAP3D_SHIFT_X_MORE:		m_pImage->m_xShift			+= STEP_SHIFT;				break;
	case ID_CMD_MAP3D_SHIFT_Y_LESS:		m_pImage->m_yShift			-= STEP_SHIFT;				break;
	case ID_CMD_MAP3D_SHIFT_Y_MORE:		m_pImage->m_yShift			+= STEP_SHIFT;				break;
	case ID_CMD_MAP3D_SHIFT_Z_LESS:		m_pImage->m_zShift			-= STEP_SHIFT;				break;
	case ID_CMD_MAP3D_SHIFT_Z_MORE:		m_pImage->m_zShift			+= STEP_SHIFT;				break;

	case ID_CMD_MAP3D_EXAGGERATE_LESS:	m_pImage->m_Exaggeration	-= STEP_EXAGGERATE;			break;
	case ID_CMD_MAP3D_EXAGGERATE_MORE:	m_pImage->m_Exaggeration	+= STEP_EXAGGERATE;			break;

	case ID_CMD_MAP3D_CENTRAL_LESS:		m_pImage->m_Central			-= STEP_CENTRAL;			break;
	case ID_CMD_MAP3D_CENTRAL_MORE:		m_pImage->m_Central			+= STEP_CENTRAL;			break;
	case ID_CMD_MAP3D_CENTRAL:			m_pImage->m_bCentral		 = !m_pImage->m_bCentral;	break;

	case ID_CMD_MAP3D_STEREO_LESS:		m_pImage->m_Stereo			-= STEP_STEREO;				break;
	case ID_CMD_MAP3D_STEREO_MORE:		m_pImage->m_Stereo			+= STEP_STEREO;				break;
	case ID_CMD_MAP3D_STEREO:			m_pImage->m_bStereo			 = !m_pImage->m_bStereo;	break;
	}

	_Parms_Changed();
}

//---------------------------------------------------------
bool CVIEW_Map_3D::_Parms_Dlg(void)
{
	if( DLG_Parameters(&m_Parameters) )
	{
		_Parms_Update(true);

		m_pImage->Set_Image();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Parms_Changed(void)
{
	_Parms_Update(false);

	m_pImage->Set_Image();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::_Play_Pos_Add(void)
{
	CSG_Table_Record	*pRecord;

	if( m_Play_Mode == PLAY_MODE_NONE )
	{
		pRecord	= m_Play.Add_Record();

		pRecord->Set_Value(PLAY_REC_ROTATE_X		, m_pImage->m_xRotate);
		pRecord->Set_Value(PLAY_REC_ROTATE_Y		, m_pImage->m_yRotate);
		pRecord->Set_Value(PLAY_REC_ROTATE_Z		, m_pImage->m_zRotate);
		pRecord->Set_Value(PLAY_REC_SHIFT_X			, m_pImage->m_xShift);
		pRecord->Set_Value(PLAY_REC_SHIFT_Y			, m_pImage->m_yShift);
		pRecord->Set_Value(PLAY_REC_SHIFT_Z			, m_pImage->m_zShift);
		pRecord->Set_Value(PLAY_REC_EXAGGERATION	, m_pImage->m_Exaggeration);
		pRecord->Set_Value(PLAY_REC_CENTRAL			, m_pImage->m_Central);
		pRecord->Set_Value(PLAY_REC_STEPS			, 10);
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Play_Pos_Del(void)
{
	if( m_Play_Mode == PLAY_MODE_NONE )
	{
		m_Play.Del_Record(m_Play.Get_Record_Count() - 1);
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Play_Pos_Clr(void)
{
	if( m_Play_Mode == PLAY_MODE_NONE )
	{
		m_Play.Del_Records();
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Play_Pos_Edit(void)
{
	if( m_Play_Mode == PLAY_MODE_NONE )
	{
		DLG_Table(LNG("[CAP] 3D-View: Player Sequence"), &m_Play);
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Play_Once(void)
{
	if( m_Play_Mode == PLAY_MODE_NONE )
	{
		m_Play_Mode	= PLAY_MODE_RUN_ONCE;

		_Play();

		m_Play_Mode	= PLAY_MODE_NONE;
	}
	else if( m_Play_Mode == PLAY_MODE_RUN_ONCE )
	{
		_Play_Stop();
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Play_Loop(void)
{
	if( m_Play_Mode == PLAY_MODE_NONE )
	{
		m_Play_Mode	= PLAY_MODE_RUN_LOOP;

		while( _Play() );
	}
	else if( m_Play_Mode == PLAY_MODE_RUN_LOOP )
	{
		_Play_Stop();
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Play_Save(void)
{
	if( m_Play_Mode == PLAY_MODE_NONE )
	{
		m_Play_Mode	= PLAY_MODE_RUN_SAVE;

		_Play();

		m_Play_Mode	= PLAY_MODE_NONE;
	}
	else if( m_Play_Mode == PLAY_MODE_RUN_SAVE )
	{
		_Play_Stop();
	}
}

//---------------------------------------------------------
void CVIEW_Map_3D::_Play_Stop(void)
{
	if( m_Play_Mode != PLAY_MODE_NONE )
	{
		m_Play_Mode	= PLAY_MODE_NONE;
	}
}

//---------------------------------------------------------
#define PLAYER_READ(iRec)	if( (pRecord = m_Play.Get_Record(iRec)) != NULL )			\
							{	xRotate[1]	= pRecord->asDouble(PLAY_REC_ROTATE_X);		\
								yRotate[1]	= pRecord->asDouble(PLAY_REC_ROTATE_Y);		\
								zRotate[1]	= pRecord->asDouble(PLAY_REC_ROTATE_Z);		\
								xShift [1]	= pRecord->asDouble(PLAY_REC_SHIFT_X);		\
								yShift [1]	= pRecord->asDouble(PLAY_REC_SHIFT_Y);		\
								zShift [1]	= pRecord->asDouble(PLAY_REC_SHIFT_Z);		\
								Exagg  [1]	= pRecord->asDouble(PLAY_REC_EXAGGERATION);	\
								Central[1]	= pRecord->asDouble(PLAY_REC_CENTRAL);		\
								Steps  [1]	= pRecord->asDouble(PLAY_REC_STEPS);		}

#define PLAYER_SWAP			{	xRotate[0]	= xRotate[1];	yRotate[0]	= yRotate[1];	zRotate[0]	= zRotate[1];	\
								xShift [0]	= xShift [1];	yShift [0]	= yShift [1];	zShift [0]	= zShift [1];	\
								Exagg  [0]	= Exagg  [1];	Central[0]	= Central[1];	Steps  [0]	= Steps  [1];	}

#define PLAYER_SET(Value)	(Value[0] + d * (Value[1] - Value[0]))

//---------------------------------------------------------
bool CVIEW_Map_3D::_Play(void)
{
	int				iRecord, iStep, iFrame	= 0, type;

	double			xRotate[2], yRotate[2], zRotate[2],
					xShift [2], yShift [2], zShift [2],
					Exagg  [2], Central[2], Steps  [2],
					d;

	wxString		file;

	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	if( m_Play.Get_Record_Count() > 1 && m_Play_Mode != PLAY_MODE_NONE )
	{
		if( m_Play_Mode == PLAY_MODE_RUN_SAVE )
		{
			if( !DLG_Image_Save(file, type) )
			{
				m_Play_Mode	= PLAY_MODE_NONE;

				return( false );
			}

			iFrame	= 0;
		}

		//-------------------------------------------------
		PLAYER_READ(0);

		for(iRecord=1; iRecord<m_Play.Get_Record_Count() && m_Play_Mode!=PLAY_MODE_NONE; iRecord++)
		{
			PLAYER_SWAP;
			PLAYER_READ(iRecord);

			for(iStep=0; iStep<Steps[0] && m_Play_Mode!=PLAY_MODE_NONE; iStep++)
			{
				d							= iStep / (double)Steps[0];
				m_pImage->m_xRotate			= PLAYER_SET(xRotate);
				m_pImage->m_yRotate			= PLAYER_SET(yRotate);
				m_pImage->m_zRotate			= PLAYER_SET(zRotate);
				m_pImage->m_xShift			= PLAYER_SET(xShift );
				m_pImage->m_yShift			= PLAYER_SET(yShift );
				m_pImage->m_zShift			= PLAYER_SET(zShift );
				m_pImage->m_xShift			= PLAYER_SET(xShift );
				m_pImage->m_Exaggeration	= PLAYER_SET(Exagg  );
				m_pImage->m_Central			= PLAYER_SET(Central);

				_Parms_Changed();

				if( m_Play_Mode == PLAY_MODE_RUN_SAVE )
				{
					m_pImage->Save(wxString::Format(wxT("%s_%03d"), file.c_str(), iFrame++), type);
				}

				PROCESS_Wait();
			}
		}

		return( m_Play_Mode != PLAY_MODE_NONE );
	}

	m_Play_Mode	= PLAY_MODE_NONE;

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
