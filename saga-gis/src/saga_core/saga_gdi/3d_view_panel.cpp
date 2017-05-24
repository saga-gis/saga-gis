/**********************************************************
 * Version $Id: 3d_view_panel.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   3d_view_panel.cpp                   //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dcclient.h>
#include <wx/clipbrd.h>

#include "3d_view.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	PLAY_REC_ROTATE_X	= 0,
	PLAY_REC_ROTATE_Y,
	PLAY_REC_ROTATE_Z,
	PLAY_REC_SHIFT_X,
	PLAY_REC_SHIFT_Y,
	PLAY_REC_SHIFT_Z,
	PLAY_REC_SCALE_Z,
	PLAY_REC_CENTRAL,
	PLAY_REC_STEPS
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSG_3DView_Panel, wxPanel)
	EVT_SIZE		(CSG_3DView_Panel::On_Size)
	EVT_PAINT		(CSG_3DView_Panel::On_Paint)
	EVT_KEY_DOWN	(CSG_3DView_Panel::On_Key_Down)
	EVT_LEFT_DOWN	(CSG_3DView_Panel::On_Mouse_LDown)
	EVT_LEFT_UP		(CSG_3DView_Panel::On_Mouse_LUp)
	EVT_RIGHT_DOWN	(CSG_3DView_Panel::On_Mouse_RDown)
	EVT_RIGHT_UP	(CSG_3DView_Panel::On_Mouse_RUp)
	EVT_MIDDLE_DOWN	(CSG_3DView_Panel::On_Mouse_MDown)
	EVT_MIDDLE_UP	(CSG_3DView_Panel::On_Mouse_MUp)
	EVT_MOTION		(CSG_3DView_Panel::On_Mouse_Motion)
	EVT_MOUSEWHEEL	(CSG_3DView_Panel::On_Mouse_Wheel)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_3DView_Panel::CSG_3DView_Panel(wxWindow *pParent, CSG_Grid *pDrape)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE)
{
	CSG_Parameter	*pNode, *pNode_1;

	m_Parameters.Create(this, _TL("Properties"), _TL(""));

	m_Parameters.Set_Callback_On_Parameter_Changed(_On_Parameter_Changed);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(
		NULL	, "NODE_GENERAL"	, _TL("General"),
		_TL("")
	);

	pNode_1	= m_Parameters.Add_Choice(
		pNode	, "CENTRAL"			, _TL("Projection"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("parallel"),
			_TL("central")
		), 1
	);

	m_Parameters.Add_Value(
		pNode_1	, "CENTRAL_DIST"	, _TL("Central Projection Distance"),
		_TL(""),
		PARAMETER_TYPE_Double, m_Projector.Get_Central_Distance(), 1.0, true
	);

	m_Parameters.Add_Value(
		pNode	, "BGCOLOR"			, _TL("Background Color"),
		_TL(""),
		PARAMETER_TYPE_Color, SG_COLOR_WHITE
	);

	m_Parameters.Add_Value(
		pNode	, "DRAW_BOX"		, _TL("Draw Box"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	pNode_1	= m_Parameters.Add_Value(
		pNode	, "STEREO"			, _TL("Stereo Anaglyph"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_Value(
		pNode_1	, "STEREO_DIST"		, _TL("Stereo Eye Distance [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	//-----------------------------------------------------
	m_pDrape	= pDrape;

	if( m_pDrape )
	{
		pNode_1	= m_Parameters.Add_Value(
			pNode	, "DO_DRAPE"	, _TL("Map Draping"),
			_TL(""),
			PARAMETER_TYPE_Bool, true
		);

		m_Parameters.Add_Choice(
			pNode_1	, "DRAPE_MODE"	, _TL("Map Drape Resampling"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s|",
				_TL("Nearest Neighbour"),
				_TL("Bilinear Interpolation"),
				_TL("Bicubic Spline Interpolation"),
				_TL("B-Spline Interpolation")
			), 0
		);
	}

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(
		NULL	, "NODE_PLAYER"		, _TL("Sequencer"),
		_TL("")
	);

	m_Parameters.Add_FilePath(
		pNode	, "PLAY_FILE"		, _TL("Image File"),
		_TL("file path, name and type used to save frames to image files"),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s"),
			_TL("Windows or OS/2 Bitmap (*.bmp)")				, SG_T("*.bmp"),
			_TL("JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)")	, SG_T("*.jpg;*.jif;*.jpeg"),
			_TL("Zsoft Paintbrush (*.pcx)")						, SG_T("*.pcx"),
			_TL("Portable Network Graphics (*.png)")			, SG_T("*.png"),
			_TL("Tagged Image File Format (*.tif, *.tiff)")		, SG_T("*.tif;*.tiff")
		), NULL, true
	);

	m_pPlay	= m_Parameters.Add_FixedTable(
		pNode	, "PLAY"			, _TL("Sequencer Positions"),
		_TL("")
		)->asTable();

	m_pPlay->Add_Field(_TL("Rotate X"        ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Rotate Y"        ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Rotate Z"        ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Shift X"         ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Shift Y"         ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Shift Z"         ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Exaggeration Z"  ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Central Distance"), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Steps to Next"   ), SG_DATATYPE_Int);

	m_Play_State	= SG_3DVIEW_PLAY_STOP;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_3DView_Panel::_On_Parameter_Changed(CSG_Parameter *pParameter, int Flags)
{
	if( pParameter && pParameter->Get_Owner() && pParameter->Get_Owner()->Get_Owner() )
	{
		if( Flags & PARAMETER_CHECK_VALUES )
		{
			((CSG_3DView_Panel *)pParameter->Get_Owner()->Get_Owner())->
				On_Parameter_Changed(pParameter->Get_Owner(), pParameter);
		}

		if( Flags & PARAMETER_CHECK_ENABLE )
		{
			((CSG_3DView_Panel *)pParameter->Get_Owner()->Get_Owner())->
				On_Parameters_Enable(pParameter->Get_Owner(), pParameter);
		}

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_3DView_Panel::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( 1 );
}

//---------------------------------------------------------
int CSG_3DView_Panel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "CENTRAL") )
	{
		pParameters->Set_Enabled("CENTRAL_DIST", pParameter->asInt() == 1);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "DO_DRAPE") )
	{
		pParameters->Set_Enabled("DRAPE_MODE"  , pParameter->asBool());
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "STEREO") )
	{
		pParameters->Set_Enabled("STEREO_DIST" , pParameter->asBool());
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Panel::Update_Statistics(void)
{}

//---------------------------------------------------------
void CSG_3DView_Panel::Update_Parent(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Panel::On_Key_Down(wxKeyEvent &event)
{
	//-----------------------------------------------------
	if( event.ControlDown() )
	{
		switch( event.GetKeyCode() )
		{
		case 'A': Play_Pos_Add();	return;
		case 'D': Play_Pos_Del();	return;
		case 'X': Play_Pos_Clr();	return;

		case 'P': Play_Once   ();	return;
		case 'L': Play_Loop   ();	return;
		case 'S': Play_Save   ();	return;

		case 'C': Save_toClipboard();	return;
		}

		event.Skip();
	}

	//-----------------------------------------------------
	else
	{
		switch( event.GetKeyCode() )
		{
		default:
			event.Skip();
			return;

		case WXK_ESCAPE:
			if( !m_Play_State )
			{
				return;
			}

			Play_Stop();
			break;

		case WXK_DOWN: case WXK_NUMPAD_DOWN: case WXK_ADD: case WXK_NUMPAD_ADD:
			m_Projector.Set_xRotation(m_Projector.Get_xRotation() - 4.0 * M_DEG_TO_RAD);
			break;

		case WXK_UP: case WXK_NUMPAD_UP: case WXK_SUBTRACT: case WXK_NUMPAD_SUBTRACT:
			m_Projector.Set_xRotation(m_Projector.Get_xRotation() + 4.0 * M_DEG_TO_RAD);
			break;

		case WXK_F3:
			m_Projector.Set_yRotation(m_Projector.Get_yRotation() - 4.0 * M_DEG_TO_RAD);
			break;

		case WXK_F4:
			m_Projector.Set_yRotation(m_Projector.Get_yRotation() + 4.0 * M_DEG_TO_RAD);
			break;

		case WXK_RIGHT: case WXK_NUMPAD_RIGHT: case WXK_MULTIPLY: case WXK_NUMPAD_MULTIPLY:
			m_Projector.Set_zRotation(m_Projector.Get_zRotation() - 4.0 * M_DEG_TO_RAD);
			break;

		case WXK_LEFT: case WXK_NUMPAD_LEFT: case WXK_DIVIDE: case WXK_NUMPAD_DIVIDE:
			m_Projector.Set_zRotation(m_Projector.Get_zRotation() + 4.0 * M_DEG_TO_RAD);
			break;

		case WXK_INSERT:
			m_Projector.Set_xShift   (m_Projector.Get_xShift   () - 10.0);
			break;

		case WXK_DELETE:
			m_Projector.Set_xShift   (m_Projector.Get_xShift   () + 10.0);
			break;

		case WXK_HOME:
			m_Projector.Set_yShift   (m_Projector.Get_yShift   () - 10.0);
			break;

		case WXK_END:
			m_Projector.Set_yShift   (m_Projector.Get_yShift   () + 10.0);
			break;

		case WXK_PAGEUP:
			m_Projector.Set_zShift   (m_Projector.Get_zShift   () - 10.0);
			break;

		case WXK_PAGEDOWN:
			m_Projector.Set_zShift   (m_Projector.Get_zShift   () + 10.0);
			break;

		case 'B':
			m_Parameters("DRAW_BOX")->Set_Value(m_Parameters("DRAW_BOX")->asBool() == false);
			break;

		case 'S':
			m_Parameters("STEREO"  )->Set_Value(m_Parameters("STEREO"  )->asBool() == false);
			break;
		}

		Update_Parent();
		Update_View();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_MOUSE_X_RELDIFF	((double)(m_Down_Screen.x - event.GetX()) / (double)GetClientSize().x)
#define GET_MOUSE_Y_RELDIFF	((double)(m_Down_Screen.y - event.GetY()) / (double)GetClientSize().y)

//---------------------------------------------------------
void CSG_3DView_Panel::On_Mouse_LDown(wxMouseEvent &event)
{
	SetFocus();

	m_Down_Screen	= event.GetPosition();
	m_Down_Value.x	= m_Projector.Get_zRotation();
	m_Down_Value.y	= m_Projector.Get_xRotation();

	CaptureMouse();
}

void CSG_3DView_Panel::On_Mouse_LUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Down_Screen.x != event.GetX() || m_Down_Screen.y != event.GetY() )
	{
		m_Projector.Set_zRotation(m_Down_Value.x + GET_MOUSE_X_RELDIFF * M_PI_180);
		m_Projector.Set_xRotation(m_Down_Value.y + GET_MOUSE_Y_RELDIFF * M_PI_180);

		Update_View();
		Update_Parent();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::On_Mouse_RDown(wxMouseEvent &event)
{
	SetFocus();

	m_Down_Screen	= event.GetPosition();
	m_Down_Value.x	= m_Projector.Get_xShift();
	m_Down_Value.y	= m_Projector.Get_yShift();

	CaptureMouse();
}

void CSG_3DView_Panel::On_Mouse_RUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Down_Screen.x != event.GetX() || m_Down_Screen.y != event.GetY() )
	{
		m_Projector.Set_xShift(m_Down_Value.x - (m_Down_Screen.x - event.GetX()));
		m_Projector.Set_yShift(m_Down_Value.y - (m_Down_Screen.y - event.GetY()));

		Update_View();
		Update_Parent();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::On_Mouse_MDown(wxMouseEvent &event)
{
	SetFocus();

	m_Down_Screen	= event.GetPosition();
	m_Down_Value.x	= m_Projector.Get_yRotation();
	m_Down_Value.y	= m_Projector.Get_zShift();

	CaptureMouse();
}

void CSG_3DView_Panel::On_Mouse_MUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Down_Screen.x != event.GetX() || m_Down_Screen.y != event.GetY() )
	{
		m_Projector.Set_yRotation(m_Down_Value.x + GET_MOUSE_X_RELDIFF * M_PI_180);
		m_Projector.Set_zShift   (m_Down_Value.y + (m_Down_Screen.y - event.GetY()));

		Update_View();
		Update_Parent();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::On_Mouse_Motion(wxMouseEvent &event)
{
	if( HasCapture() && event.Dragging() )
	{
		if( event.LeftIsDown() )
		{
			m_Projector.Set_zRotation(m_Down_Value.x + GET_MOUSE_X_RELDIFF * M_PI_180);
			m_Projector.Set_xRotation(m_Down_Value.y + GET_MOUSE_Y_RELDIFF * M_PI_180);
		}
		else if( event.RightIsDown() )
		{
			m_Projector.Set_xShift   (m_Down_Value.x - (m_Down_Screen.x - event.GetX()));
			m_Projector.Set_yShift   (m_Down_Value.y - (m_Down_Screen.y - event.GetY()));
		}
		else if( event.MiddleIsDown() )
		{
			m_Projector.Set_yRotation(m_Down_Value.x + GET_MOUSE_X_RELDIFF * M_PI_180);
			m_Projector.Set_zShift   (m_Down_Value.y + (m_Down_Screen.y - event.GetY()));
		}
		else
		{
			return;
		}

		Update_View();
		Update_Parent();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::On_Mouse_Wheel(wxMouseEvent &event)
{
	if( event.GetWheelRotation() )
	{
		m_Projector.Set_zShift(m_Projector.Get_zShift()	+ event.GetWheelRotation());

		Update_View();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Panel::On_Size(wxSizeEvent &event)
{
	Update_View();

	event.Skip();
}

//---------------------------------------------------------
void CSG_3DView_Panel::On_Paint(wxPaintEvent &WXUNUSED(event))
{
	if( m_Image.IsOk() && m_Image.GetWidth() > 0 && m_Image.GetHeight() > 0 )
	{
		wxPaintDC	dc(this);

		dc.DrawBitmap(wxBitmap(m_Image), 0, 0, false);
	}
}

//---------------------------------------------------------
bool CSG_3DView_Panel::Update_View(bool bStatistics)
{
	if( bStatistics )
	{
		Update_Statistics();
	}

	//-----------------------------------------------------
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_bgColor	= m_Parameters("BGCOLOR"    )->asColor ();
		m_bBox		= m_Parameters("DRAW_BOX"   )->asBool  ();
		m_bStereo	= m_Parameters("STEREO"     )->asBool  ();
		m_dStereo	= m_Parameters("STEREO_DIST")->asDouble();

		switch( m_Parameters("DRAPE_MODE") ? m_Parameters("DRAPE_MODE")->asInt() : 0 )
		{
		default:	m_Drape_Mode	= GRID_RESAMPLING_NearestNeighbour;	break;
		case  1:	m_Drape_Mode	= GRID_RESAMPLING_Bilinear;			break;
		case  2:	m_Drape_Mode	= GRID_RESAMPLING_BicubicSpline;	break;
		case  3:	m_Drape_Mode	= GRID_RESAMPLING_BSpline;			break;
		}

		m_Projector.do_Central          (m_Parameters("CENTRAL")->asInt() == 1);
		m_Projector.Set_Central_Distance(m_Parameters("CENTRAL_DIST")->asDouble());
	}

	//-----------------------------------------------------
	wxSize	Size	= GetClientSize();

	if( Size.x < 1 || Size.y < 1 )
	{
		return( false );
	}

	if( !m_Image.IsOk() || Size.x != m_Image.GetWidth() || Size.y != m_Image.GetHeight() )
	{
		if( !m_Image.Create(Size.x, Size.y) )
		{
			return( false );
		}
	}

	Set_Image(m_Image.GetData(), m_Image.GetWidth(), m_Image.GetHeight() );

	//-----------------------------------------------------
	CSG_Grid	*pDrape	= m_pDrape;	if( m_pDrape && !m_Parameters("DO_DRAPE")->asBool() )	m_pDrape	= NULL;

	if( Draw() )
	{
		wxClientDC	dc(this);

		dc.DrawBitmap(wxBitmap(m_Image), 0, 0, false);
	}

	m_pDrape	= pDrape;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_3DView_Panel::Save_asImage(const CSG_String &FileName)
{
	return( m_Image.SaveFile(FileName.c_str()) );
}

//---------------------------------------------------------
bool CSG_3DView_Panel::Save_toClipboard(void)
{
	if( m_Image.IsOk() && m_Image.GetWidth() > 0 && m_Image.GetHeight() > 0 && wxTheClipboard->Open() )
	{
		wxBitmapDataObject	*pBMP	= new wxBitmapDataObject;
		pBMP->SetBitmap(m_Image);
		wxTheClipboard->SetData(pBMP);
		wxTheClipboard->Close();

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
void CSG_3DView_Panel::Play_Pos_Add(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )	
	{
		CSG_Table_Record	*pRecord	= m_pPlay->Add_Record();

		pRecord->Set_Value(PLAY_REC_ROTATE_X, m_Projector.Get_xRotation       ());
		pRecord->Set_Value(PLAY_REC_ROTATE_Y, m_Projector.Get_yRotation       ());
		pRecord->Set_Value(PLAY_REC_ROTATE_Z, m_Projector.Get_zRotation       ());
		pRecord->Set_Value(PLAY_REC_SHIFT_X , m_Projector.Get_xShift          ());
		pRecord->Set_Value(PLAY_REC_SHIFT_Y , m_Projector.Get_yShift          ());
		pRecord->Set_Value(PLAY_REC_SHIFT_Z , m_Projector.Get_zShift          ());
		pRecord->Set_Value(PLAY_REC_SCALE_Z , m_Projector.Get_zScaling        ());
		pRecord->Set_Value(PLAY_REC_CENTRAL , m_Projector.Get_Central_Distance());
		pRecord->Set_Value(PLAY_REC_STEPS   , 10);
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Pos_Del(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_pPlay->Del_Record(m_pPlay->Get_Count() - 1);
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Pos_Clr(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_pPlay->Del_Records();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Once(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Play_State	= SG_3DVIEW_PLAY_RUN_ONCE;

		_Play();

		m_Play_State	= SG_3DVIEW_PLAY_STOP;
	}
	else if( m_Play_State == SG_3DVIEW_PLAY_RUN_ONCE )
	{
		Play_Stop();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Loop(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Play_State	= SG_3DVIEW_PLAY_RUN_LOOP;

		while( _Play() );
	}
	else if( m_Play_State == SG_3DVIEW_PLAY_RUN_LOOP )
	{
		Play_Stop();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Save(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Play_State	= SG_3DVIEW_PLAY_RUN_SAVE;

		_Play();

		m_Play_State	= SG_3DVIEW_PLAY_STOP;
	}
	else if( m_Play_State == SG_3DVIEW_PLAY_RUN_SAVE )
	{
		Play_Stop();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Stop(void)
{
	if( m_Play_State != SG_3DVIEW_PLAY_STOP )
	{
		m_Play_State	= SG_3DVIEW_PLAY_STOP;
	}
}

//---------------------------------------------------------
#define PLAYER_READ(iRecord)	{	CSG_Table_Record *pRecord = m_pPlay->Get_Record(iRecord); if( pRecord ) {\
	for(int i=0; i<=PLAY_REC_STEPS; i++)\
	{\
		Position[i][0]	= Position[i][1];\
		Position[i][1]	= pRecord->asDouble(i);\
	}\
}}

//---------------------------------------------------------
double	SG_Get_Short_Angle	(double Angle)
{
	Angle	= fmod(Angle, M_PI_360);

	if( fabs(Angle) > M_PI_180 )
	{
		Angle	+= Angle < 0.0 ? M_PI_360 : -M_PI_360;
	}

	return( Angle );
}


//---------------------------------------------------------
#define PLAYER_GET_VAL(Value)	(Value[0] + d * (Value[1] - Value[0]))
#define PLAYER_GET_ROT(Value)	(Value[0] + d * SG_Get_Short_Angle(Value[1] - Value[0]))

//---------------------------------------------------------
bool CSG_3DView_Panel::_Play(void)
{
	//-----------------------------------------------------
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		return( false );
	}

	if( m_pPlay->Get_Count() < 2 )
	{
		m_Play_State	= SG_3DVIEW_PLAY_STOP;

		return( false );
	}

	//-----------------------------------------------------
	if( m_Play_State == SG_3DVIEW_PLAY_RUN_SAVE && *m_Parameters("PLAY_FILE")->asString() == '\0' )
	{
		SG_UI_Dlg_Error(_TL("invalid image file path"), _TL("3D View Sequencer"));

		m_Play_State	= SG_3DVIEW_PLAY_STOP;

		return( false );
	}

	//-----------------------------------------------------
	CSG_Matrix	Position(2, 9);

	PLAYER_READ(0);

	int	nPositions	= (int)m_pPlay->Get_Count() + (m_Play_State == SG_3DVIEW_PLAY_RUN_LOOP ? 1 : 0);

	for(int iRecord=1, iFrame=0; iRecord<nPositions && m_Play_State!=SG_3DVIEW_PLAY_STOP; iRecord++)
	{
		PLAYER_READ(iRecord % m_pPlay->Get_Count());

		for(int iStep=0; iStep<(int)Position[PLAY_REC_STEPS][0] && m_Play_State!=SG_3DVIEW_PLAY_STOP; iStep++, iFrame++)
		{
			double	d	= iStep / Position[PLAY_REC_STEPS][0];

			m_Projector.Set_xRotation       (PLAYER_GET_ROT(Position[PLAY_REC_ROTATE_X]));
			m_Projector.Set_yRotation       (PLAYER_GET_ROT(Position[PLAY_REC_ROTATE_Y]));
			m_Projector.Set_zRotation       (PLAYER_GET_ROT(Position[PLAY_REC_ROTATE_Z]));
			m_Projector.Set_xShift          (PLAYER_GET_VAL(Position[PLAY_REC_SHIFT_X ]));
			m_Projector.Set_yShift          (PLAYER_GET_VAL(Position[PLAY_REC_SHIFT_Y ]));
			m_Projector.Set_zShift          (PLAYER_GET_VAL(Position[PLAY_REC_SHIFT_Z ]));
			m_Projector.Set_zScaling        (PLAYER_GET_VAL(Position[PLAY_REC_SCALE_Z ]));
			m_Projector.Set_Central_Distance(PLAYER_GET_VAL(Position[PLAY_REC_CENTRAL ]));

			Update_View();

			if( m_Play_State == SG_3DVIEW_PLAY_RUN_SAVE )
			{
				m_Image.SaveFile(SG_File_Make_Path(
					SG_File_Get_Path     (m_Parameters("PLAY_FILE")->asString()),
					SG_File_Get_Name     (m_Parameters("PLAY_FILE")->asString(), false) + CSG_String::Format(SG_T("%03d"), iFrame),
					SG_File_Get_Extension(m_Parameters("PLAY_FILE")->asString())
				).c_str());
			}

			SG_UI_Process_Get_Okay();
		}
	}

	if( m_Play_State != SG_3DVIEW_PLAY_RUN_LOOP )
	{
		m_Play_State	= SG_3DVIEW_PLAY_STOP;

		Update_Parent();
		Update_View();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
