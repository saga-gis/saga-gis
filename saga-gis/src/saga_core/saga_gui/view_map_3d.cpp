
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
#include "res_commands.h"
#include "res_controls.h"
#include "res_dialogs.h"
#include "res_images.h"

#include "helper.h"

#include "wksp_data_manager.h"
#include "wksp_map.h"

#include "view_map_3d.h"
#include "view_map_3d_panel.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	MAP3D_STATUSBAR_ROTATE_X	= 0,
//	MAP3D_STATUSBAR_ROTATE_Y,
	MAP3D_STATUSBAR_ROTATE_Z,
	MAP3D_STATUSBAR_SHIFT_X,
	MAP3D_STATUSBAR_SHIFT_Y,
	MAP3D_STATUSBAR_SHIFT_Z,
	MAP3D_STATUSBAR_EXAGGERATION,
	MAP3D_STATUSBAR_COUNT
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Map_3D, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Map_3D, CVIEW_Base)
	EVT_SIZE(CVIEW_Map_3D::On_Size)

	EVT_MENU           (ID_CMD_VIEW_CLIPBOARD_COPY           , CVIEW_Map_3D::On_Command)
	EVT_UPDATE_UI      (ID_CMD_VIEW_CLIPBOARD_COPY           , CVIEW_Map_3D::On_Command_UI)
	EVT_MENU_RANGE     (ID_CMD_MAP3D_FIRST, ID_CMD_MAP3D_LAST, CVIEW_Map_3D::On_Command)
	EVT_UPDATE_UI_RANGE(ID_CMD_MAP3D_FIRST, ID_CMD_MAP3D_LAST, CVIEW_Map_3D::On_Command_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Map_3D::CVIEW_Map_3D(CWKSP_Map *pMap)
	: CVIEW_Base(pMap, ID_VIEW_MAP_3D, _TL("3D View"), ID_IMG_TB_MAP_3D_SHOW, false)
{
	SetTitle(wxString::Format("%s [%s]", pMap->Get_Name().c_str(), _TL("3D View")));

	CreateStatusBar(MAP3D_STATUSBAR_COUNT);

	m_pPanel = new CVIEW_Map_3DPanel(this, pMap);
	m_pPanel->SetSize(GetClientSize());

	//-----------------------------------------------------
	if( DLG_Parameters(&m_pPanel->m_Parameters) )
	{
		m_pPanel->Update_Parameters(false);

		Do_Show();

		m_pPanel->SetFocus();
	}
	else
	{
		Destroy();
	}
}

//---------------------------------------------------------
CVIEW_Map_3D::~CVIEW_Map_3D(void)
{
	m_pPanel->Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::Do_Update(void)
{
	m_pPanel->Update_View(true);
}

//---------------------------------------------------------
void CVIEW_Map_3D::Update_StatusBar(void)
{
	SetStatusText(wxString::Format("RX %+.1f", m_pPanel->Get_Projector().Get_xRotation() * M_RAD_TO_DEG), MAP3D_STATUSBAR_ROTATE_X    );
//	SetStatusText(wxString::Format("RY %+.1f", m_pPanel->Get_Projector().Get_yRotation() * M_RAD_TO_DEG), MAP3D_STATUSBAR_ROTATE_Y    );
	SetStatusText(wxString::Format("RZ %+.1f", m_pPanel->Get_Projector().Get_zRotation() * M_RAD_TO_DEG), MAP3D_STATUSBAR_ROTATE_Z    );

	SetStatusText(wxString::Format("DX %+.1f", m_pPanel->Get_Projector().Get_xShift   ()               ), MAP3D_STATUSBAR_SHIFT_X     );
	SetStatusText(wxString::Format("DY %+.1f", m_pPanel->Get_Projector().Get_yShift   ()               ), MAP3D_STATUSBAR_SHIFT_Y     );
	SetStatusText(wxString::Format("DZ %+.1f", m_pPanel->Get_Projector().Get_zShift   ()               ), MAP3D_STATUSBAR_SHIFT_Z     );

	SetStatusText(wxString::Format("EX %.1f" , m_pPanel->Get_Projector().Get_zScaling ()               ), MAP3D_STATUSBAR_EXAGGERATION);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::On_Size(wxSizeEvent &event)
{
	m_pPanel->SetSize(GetClientRect());

	event.Skip();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Map_3D::_Create_Menu(void)
{
	wxMenu *pMenu = new wxMenu, *pMenu_Sub;

	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_PARAMETERS);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_USAGE);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_SAVE);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_EXAGGERATE_LESS);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_EXAGGERATE_MORE);

	pMenu->Append(ID_CMD_MAP3D_FIRST, _TL("Rotation"), pMenu_Sub = new wxMenu());
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_X_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_X_MORE);
	pMenu_Sub->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_Z_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_Z_MORE);

	pMenu->Append(ID_CMD_MAP3D_FIRST, _TL("Shift"), pMenu_Sub = new wxMenu());
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_X_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_X_MORE);
	pMenu_Sub->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_Y_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_Y_MORE);
	pMenu_Sub->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_Z_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SHIFT_Z_MORE);

	pMenu->Append(ID_CMD_MAP3D_FIRST, _TL("Sequencer"), pMenu_Sub = new wxMenu());
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_POS_ADD);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_POS_DEL);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_POS_DEL_ALL);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_SEQ_POS_EDIT);
	pMenu_Sub->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Sub,  true, ID_CMD_MAP3D_SEQ_PLAY);
	CMD_Menu_Add_Item(pMenu_Sub,  true, ID_CMD_MAP3D_SEQ_PLAY_LOOP);
	CMD_Menu_Add_Item(pMenu_Sub,  true, ID_CMD_MAP3D_SEQ_SAVE);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu    ,  true, ID_CMD_MAP3D_CENTRAL);
	CMD_Menu_Add_Item(pMenu    ,  true, ID_CMD_MAP3D_ANAGLYPH);
	CMD_Menu_Add_Item(pMenu    ,  true, ID_CMD_MAP3D_STEREO_VIEW);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_VIEW_CLIPBOARD_COPY);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Map_3D::_Create_ToolBar(void)
{
	wxToolBarBase *pToolBar = CMD_ToolBar_Create(ID_TB_VIEW_MAP_3D);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_PARAMETERS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_VIEW_CLIPBOARD_COPY);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar,  true, ID_CMD_MAP3D_ANAGLYPH);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_ROTATE_X_LESS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_ROTATE_X_MORE);
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

	CMD_ToolBar_Add(pToolBar, _TL("3D-View"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::On_Command(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_PARAMETERS     : {
		m_pPanel->Update_Parameters(true);

		if( DLG_Parameters(&m_pPanel->m_Parameters) )
		{
			m_pPanel->Update_Parameters(false); m_pPanel->Update_View();
		}
		return; }

	//-----------------------------------------------------
	case ID_CMD_MAP3D_USAGE          : {
		DLG_Info(_TL("3D View Usage"), m_pPanel->Get_Usage().c_str());
		return; }

	//-----------------------------------------------------
	case ID_CMD_MAP3D_SAVE           : { wxString File; int Type;
		CSG_Parameters P(_TL("Image Resolution"));
		P.Add_Int("", "NX", _TL("Width" ), _TL(""), m_pPanel->GetSize().x, 1, true);
		P.Add_Int("", "NY", _TL("Height"), _TL(""), m_pPanel->GetSize().y, 1, true);

		if( DLG_Image_Save(File, Type) && DLG_Parameters(&P) )
		{
			Set_Buisy_Cursor(true);

			if( P("NX")->asInt() != m_pPanel->GetSize().x
			&&  P("NY")->asInt() != m_pPanel->GetSize().y )
			{
				Freeze(); wxSize oldSize(m_pPanel->GetSize());
				m_pPanel->SetSize(P("NX")->asInt(), P("NY")->asInt());
				m_pPanel->Save_asImage(&File);
				m_pPanel->SetSize(oldSize);
				Thaw();
			}
			else
			{
				m_pPanel->Save_asImage(&File);
			}

			Set_Buisy_Cursor(false);
		}
		return; }

	//-----------------------------------------------------
	case ID_CMD_VIEW_CLIPBOARD_COPY:
		m_pPanel->Save_toClipboard();
		break;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_SEQ_POS_EDIT   : {
		DLG_Table(_TL("Edit 3D-View Sequencer Positions"), m_pPanel->m_Parameters("PLAY")->asTable());
		return; }

	//-----------------------------------------------------
	case ID_CMD_MAP3D_SEQ_POS_ADD    : m_pPanel->Play_Pos_Add(); return;
	case ID_CMD_MAP3D_SEQ_POS_DEL    : m_pPanel->Play_Pos_Del(); return;
	case ID_CMD_MAP3D_SEQ_POS_DEL_ALL: m_pPanel->Play_Pos_Clr(); return;
	case ID_CMD_MAP3D_SEQ_PLAY       : m_pPanel->Play_Once   (); return;
	case ID_CMD_MAP3D_SEQ_PLAY_LOOP  : m_pPanel->Play_Loop   (); return;
	case ID_CMD_MAP3D_SEQ_SAVE       : m_pPanel->Play_Save   (); return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_ROTATE_X_LESS  : m_pPanel->Get_Projector().Inc_xRotation( 4. * M_DEG_TO_RAD); break;
	case ID_CMD_MAP3D_ROTATE_X_MORE  : m_pPanel->Get_Projector().Inc_xRotation(-4. * M_DEG_TO_RAD); break;
	case ID_CMD_MAP3D_ROTATE_Y_LESS  : m_pPanel->Get_Projector().Inc_yRotation( 4. * M_DEG_TO_RAD); break;
	case ID_CMD_MAP3D_ROTATE_Y_MORE  : m_pPanel->Get_Projector().Inc_yRotation(-4. * M_DEG_TO_RAD); break;
	case ID_CMD_MAP3D_ROTATE_Z_LESS  : m_pPanel->Get_Projector().Inc_zRotation( 4. * M_DEG_TO_RAD); break;
	case ID_CMD_MAP3D_ROTATE_Z_MORE  : m_pPanel->Get_Projector().Inc_zRotation(-4. * M_DEG_TO_RAD); break;

	case ID_CMD_MAP3D_SHIFT_X_LESS   : m_pPanel->Get_Projector().Inc_xShift(-0.1); break;
	case ID_CMD_MAP3D_SHIFT_X_MORE   : m_pPanel->Get_Projector().Inc_xShift( 0.1); break;
	case ID_CMD_MAP3D_SHIFT_Y_LESS   : m_pPanel->Get_Projector().Inc_yShift( 0.1); break;
	case ID_CMD_MAP3D_SHIFT_Y_MORE   : m_pPanel->Get_Projector().Inc_yShift(-0.1); break;
	case ID_CMD_MAP3D_SHIFT_Z_LESS   : m_pPanel->Get_Projector().Inc_zShift(-0.1); break;
	case ID_CMD_MAP3D_SHIFT_Z_MORE   : m_pPanel->Get_Projector().Inc_zShift( 0.1); break;

	case ID_CMD_MAP3D_EXAGGERATE_LESS: m_pPanel->Get_Projector().Inc_zScaling(-0.5); break;
	case ID_CMD_MAP3D_EXAGGERATE_MORE: m_pPanel->Get_Projector().Inc_zScaling( 0.5); break;

	case ID_CMD_MAP3D_CENTRAL        : m_pPanel->Get_Projector().do_Central(m_pPanel->Get_Projector().is_Central() == false); break;
	case ID_CMD_MAP3D_CENTRAL_LESS   : m_pPanel->Get_Projector().Inc_Central_Distance( 0.1); break;
	case ID_CMD_MAP3D_CENTRAL_MORE   : m_pPanel->Get_Projector().Inc_Central_Distance(-0.1); break;

	case ID_CMD_MAP3D_ANAGLYPH       : m_pPanel->Set_Stereo_Mode(m_pPanel->m_Parameters("STEREO")->asInt() != 1 ? 1 : 0); break;
	case ID_CMD_MAP3D_STEREO_VIEW    : m_pPanel->Set_Stereo_Mode(m_pPanel->m_Parameters("STEREO")->asInt() != 2 ? 2 : 0); break;
	}

	m_pPanel->Update_Parameters(true); m_pPanel->Update_View();
}

//---------------------------------------------------------
void CVIEW_Map_3D::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_MAP3D_CENTRAL_LESS :
	case ID_CMD_MAP3D_CENTRAL_MORE : event.Enable(m_pPanel->Get_Projector().is_Central()); break;
	case ID_CMD_MAP3D_CENTRAL      : event.Check (m_pPanel->Get_Projector().is_Central()); break;

	case ID_CMD_MAP3D_ANAGLYPH     : event.Check (m_pPanel->m_Parameters("STEREO")->asInt() == 1); break;
	case ID_CMD_MAP3D_STEREO_VIEW  : event.Check (m_pPanel->m_Parameters("STEREO")->asInt() == 2); break;
	case ID_CMD_MAP3D_SEQ_PLAY     : event.Check (m_pPanel->Play_Get_State() == SG_3DVIEW_PLAY_RUN_ONCE); break;
	case ID_CMD_MAP3D_SEQ_PLAY_LOOP: event.Check (m_pPanel->Play_Get_State() == SG_3DVIEW_PLAY_RUN_LOOP); break;
	case ID_CMD_MAP3D_SEQ_SAVE     : event.Check (m_pPanel->Play_Get_State() == SG_3DVIEW_PLAY_RUN_SAVE); break;
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
