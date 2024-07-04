
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
//                   3d_view_dialog.cpp                  //
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
#include <wx/menu.h>

#include "3d_view.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSG_3DView_Dialog, CSGDI_Dialog)
	EVT_CLOSE     (CSG_3DView_Dialog::On_Close)

	EVT_BUTTON    (wxID_ANY	, CSG_3DView_Dialog::On_Button)
	EVT_CHECKBOX  (wxID_ANY	, CSG_3DView_Dialog::On_Update_Control)
	EVT_TEXT_ENTER(wxID_ANY	, CSG_3DView_Dialog::On_Update_Control)
	EVT_SLIDER    (wxID_ANY	, CSG_3DView_Dialog::On_Update_Control)
	EVT_CHOICE    (wxID_ANY	, CSG_3DView_Dialog::On_Update_Choices)

	EVT_MENU_RANGE     (MENU_FIRST, MENU_LAST, CSG_3DView_Dialog::On_Menu)
	EVT_UPDATE_UI_RANGE(MENU_FIRST, MENU_LAST, CSG_3DView_Dialog::On_Menu_UI)
END_EVENT_TABLE()

//---------------------------------------------------------
CSG_3DView_Dialog::CSG_3DView_Dialog(const CSG_String &Caption, int Style)
	: CSGDI_Dialog(Caption.c_str(), Style)
{
	SetWindowStyle(wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE);

	m_pPanel = NULL;
}

//---------------------------------------------------------
bool CSG_3DView_Dialog::Create(CSG_3DView_Panel *pPanel)
{
	m_pPanel    = pPanel;

	m_pCommands = Add_Button  (_TL("Commands"    ), wxID_ANY);

//	Add_Spacer();
//	m_pBox      = Add_CheckBox(_TL("Bounding Box"), m_pPanel->m_Parameters("BOX"   )->asBool());
//	m_pRotate   = Add_Slider  (_TL("Rotation"    ), m_pPanel->Get_Projector().Get_xRotation() * M_RAD_TO_DEG, -180., 180.);

	Add_Output(m_pPanel);

	return( true );
}

//---------------------------------------------------------
void CSG_3DView_Dialog::On_Close(wxCloseEvent &event)
{
	if( m_pPanel )
	{
		m_pPanel->Play_Stop();
	}

	event.Skip();
}

//---------------------------------------------------------
void CSG_3DView_Dialog::On_Update_Control(wxCommandEvent &event)
{
//	CHECKBOX_UPDATE(m_pBox, "BOX");

//	if( event.GetEventObject() == m_pRotate )
//	{
//		m_pPanel->Get_Projector().Set_xRotation(m_pRotate_X->Get_Value() * M_DEG_TO_RAD); m_pPanel->Update_View();
//	}
}

//---------------------------------------------------------
void CSG_3DView_Dialog::On_Update_Choices(wxCommandEvent &event)
{
//	if( event.GetEventObject() == m_pField_Z )
//	{
//		m_pPanel->m_zField	= m_pField_Z->GetSelection();
//		m_pPanel->Update_View();
//	}
}

//---------------------------------------------------------
void CSG_3DView_Dialog::On_Button(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pCommands )
	{
		wxMenu Menu, *pMenu;

		//-------------------------------------------------
		Menu.Append(MENU_PROPERTIES  , _TL("Properties"));
		Menu.Append(MENU_USAGE       , _TL("Usage"));
		Menu.Append(MENU_TO_CLIPBOARD, wxString::Format("%s [Ctrl+C]", _TL("Copy to Clipboard" )));

		Menu.AppendSeparator();

		//-------------------------------------------------
		Menu.AppendSubMenu(pMenu = new wxMenu, _TL("Display"));

		pMenu->AppendCheckItem(MENU_BOX          , wxString::Format("%s [B]"        , _TL("Bounding Box"      )));
		pMenu->AppendCheckItem(MENU_LABELS       , wxString::Format("%s [L]"        , _TL("Axis Labeling"     )));
		pMenu->AppendCheckItem(MENU_NORTH        , wxString::Format("%s [N]"        , _TL("North Arrow"       )));
		pMenu->AppendCheckItem(MENU_ANAGLYPH     , wxString::Format("%s [A]"        , _TL("Anaglyph"          )));
		pMenu->AppendCheckItem(MENU_STEREO_VIEW  , wxString::Format("%s [T]"        , _TL("Stereo View"       )));
		pMenu->AppendCheckItem(MENU_CENTRAL      , wxString::Format("%s [C]"        , _TL("Central Projection")));

		pMenu->AppendSeparator();

		//-------------------------------------------------
		Menu.AppendSubMenu(pMenu = new wxMenu, _TL("Rotation"));

		pMenu->Append         (MENU_ROTATE_Z_DEC , wxString::Format("%s [1]"        , _TL("Left"    )));
		pMenu->Append         (MENU_ROTATE_Z_INC , wxString::Format("%s [2]"        , _TL("Right"   )));
		pMenu->AppendSeparator();
		pMenu->Append         (MENU_ROTATE_X_DEC , wxString::Format("%s [3]"        , _TL("Up"      )));
		pMenu->Append         (MENU_ROTATE_X_INC , wxString::Format("%s [4]"        , _TL("Down"    )));

		//-------------------------------------------------
		Menu.AppendSubMenu(pMenu = new wxMenu, _TL("Shift"));

		pMenu->Append         (MENU_SHIFT_X_DEC  , wxString::Format("%s [Ins]"      , _TL("Left"    )));
		pMenu->Append         (MENU_SHIFT_X_INC  , wxString::Format("%s [Del]"      , _TL("Right"   )));
		pMenu->AppendSeparator();
		pMenu->Append         (MENU_SHIFT_Y_DEC  , wxString::Format("%s [Home]"     , _TL("Up"      )));
		pMenu->Append         (MENU_SHIFT_Y_INC  , wxString::Format("%s [End]"      , _TL("Down"    )));
		pMenu->AppendSeparator();
		pMenu->Append         (MENU_SHIFT_Z_DEC  , wxString::Format("%s [Page Up]"  , _TL("Forward" )));
		pMenu->Append         (MENU_SHIFT_Z_INC  , wxString::Format("%s [Page Down]", _TL("Backward")));

		//-------------------------------------------------
		Menu.AppendSubMenu(pMenu = new wxMenu, _TL("Sequencer"));

		pMenu->Append         (MENU_PLAY_POS_ADD , wxString::Format("%s [Ctrl+A]", _TL("Add Position"          )));
		pMenu->Append         (MENU_PLAY_POS_DEL , wxString::Format("%s [Ctrl+D]", _TL("Delete Last Position"  )));
		pMenu->Append         (MENU_PLAY_POS_CLR , wxString::Format("%s [Ctrl+X]", _TL("Delete All Positions"  )));
		pMenu->AppendSeparator();
		pMenu->Append         (MENU_PLAY_RUN_ONCE, wxString::Format("%s [Ctrl+P]", _TL("Play Once"             )));
		pMenu->AppendCheckItem(MENU_PLAY_RUN_LOOP, wxString::Format("%s [Ctrl+L]", _TL("Play Loop"             )));
		pMenu->Append         (MENU_PLAY_RUN_SAVE, wxString::Format("%s [Ctrl+S]", _TL("Play and Save to Image")));

		//-------------------------------------------------
		Set_Menu(Menu);

		Menu.AppendSeparator();
		Menu.Append(MENU_CLOSE, _TL("Close"));

		m_pCommands->PopupMenu(&Menu, m_pCommands->GetRect().GetBottomLeft());

		return;
	}

	event.Skip();
}

//---------------------------------------------------------
void CSG_3DView_Dialog::On_Menu(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	default: return;

	case MENU_CLOSE        : Close(); return;

	case MENU_PROPERTIES   : m_pPanel->Parameters_Dialog(); return;

	case MENU_USAGE        : SG_UI_Dlg_Info(m_pPanel->Get_Usage(), _TL("Usage")); return;

	case MENU_TO_CLIPBOARD : m_pPanel->Save_toClipboard(); return;

	case MENU_BOX          : m_pPanel->Parameter_Value_Toggle("BOX"   ); return;
	case MENU_LABELS       : m_pPanel->Parameter_Value_Toggle("LABELS"); return;
	case MENU_NORTH        : m_pPanel->Parameter_Value_Toggle("NORTH" ); return;

	case MENU_ANAGLYPH     : m_pPanel->Set_Stereo_Mode(m_pPanel->m_Parameters("STEREO")->asInt() != 1 ? 1 : 0); break;
	case MENU_STEREO_VIEW  : m_pPanel->Set_Stereo_Mode(m_pPanel->m_Parameters("STEREO")->asInt() != 2 ? 2 : 0); break;

	case MENU_ROTATE_X_DEC : m_pPanel->Get_Projector().Inc_xRotation(-4., true); break;
	case MENU_ROTATE_X_INC : m_pPanel->Get_Projector().Inc_xRotation( 4., true); break;
	case MENU_ROTATE_Y_DEC : m_pPanel->Get_Projector().Inc_yRotation(-4., true); break;
	case MENU_ROTATE_Y_INC : m_pPanel->Get_Projector().Inc_yRotation( 4., true); break;
	case MENU_ROTATE_Z_DEC : m_pPanel->Get_Projector().Inc_zRotation(-4., true); break;
	case MENU_ROTATE_Z_INC : m_pPanel->Get_Projector().Inc_zRotation( 4., true); break;

	case MENU_SHIFT_X_DEC  : m_pPanel->Get_Projector().Inc_xShift(-10.); break;
	case MENU_SHIFT_X_INC  : m_pPanel->Get_Projector().Inc_xShift( 10.); break;
	case MENU_SHIFT_Y_DEC  : m_pPanel->Get_Projector().Inc_yShift(-10.); break;
	case MENU_SHIFT_Y_INC  : m_pPanel->Get_Projector().Inc_yShift( 10.); break;
	case MENU_SHIFT_Z_DEC  : m_pPanel->Get_Projector().Inc_zShift(-10.); break;
	case MENU_SHIFT_Z_INC  : m_pPanel->Get_Projector().Inc_zShift( 10.); break;

	case MENU_CENTRAL      : m_pPanel->Get_Projector().do_Central(!m_pPanel->Get_Projector().is_Central()); break;

	case MENU_PLAY_POS_ADD : m_pPanel->Play_Pos_Add(); return;
	case MENU_PLAY_POS_DEL : m_pPanel->Play_Pos_Del(); return;
	case MENU_PLAY_POS_CLR : m_pPanel->Play_Pos_Clr(); return;
	case MENU_PLAY_RUN_ONCE: m_pPanel->Play_Once   (); return;
	case MENU_PLAY_RUN_LOOP: m_pPanel->Play_Loop   (); return;
	case MENU_PLAY_RUN_SAVE: m_pPanel->Play_Save   (); return;
	}

	m_pPanel->Update_Parameters(true); m_pPanel->Update_View();
}

//---------------------------------------------------------
void CSG_3DView_Dialog::On_Menu_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	case MENU_BOX          : event.Check(m_pPanel->m_Parameters("BOX"   )->asBool()    ); break;
	case MENU_NORTH        : event.Check(m_pPanel->m_Parameters("NORTH" )->asBool()    ); break;
	case MENU_ANAGLYPH     : event.Check(m_pPanel->m_Parameters("STEREO")->asInt() == 1); break;
	case MENU_STEREO_VIEW  : event.Check(m_pPanel->m_Parameters("STEREO")->asInt() == 2); break;
	case MENU_LABELS       : event.Check(m_pPanel->m_Parameters("LABELS")->asInt() != 2); break;

	case MENU_CENTRAL      : event.Check(m_pPanel->Get_Projector().is_Central()); break;

	case MENU_PLAY_RUN_LOOP: event.Check(m_pPanel->Play_Get_State() == SG_3DVIEW_PLAY_RUN_LOOP); break;
	}
}

//---------------------------------------------------------
void CSG_3DView_Dialog::Update_Controls(void)
{
//	m_pBox->SetValue(m_pPanel->m_Parameters("BOX")->asBool());

//	double d = fmod(M_RAD_TO_DEG * m_pPanel->Get_Projector().Get_xRotation(), 360.);
//	if( d < -180. ) d += 360.; else if( d > 180. ) d -= 360.;
//	m_pRotate->Set_Value(d);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
