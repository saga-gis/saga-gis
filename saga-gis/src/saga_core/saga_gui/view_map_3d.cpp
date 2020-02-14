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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	MAP3D_STATUSBAR_COUNT
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Map_3D, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Map_3D, CVIEW_Base)
	EVT_SIZE			(CVIEW_Map_3D::On_Size)
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
	: CVIEW_Base(pMap, ID_VIEW_MAP_3D, _TL("3D View"), ID_IMG_WND_MAP3D, false)
{
	SetTitle(wxString::Format("%s [%s]", pMap->Get_Name().c_str(), _TL("3D View")));

	CreateStatusBar(MAP3D_STATUSBAR_COUNT);

	m_pPanel	= new CVIEW_Map_3DPanel(this, pMap);
	m_pPanel->SetSize(GetClientSize());

	//-----------------------------------------------------
	Parameters_Create();

	if( DLG_Parameters(&m_Parameters) )
	{
		Parameters_Update(false);

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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Map_3D::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu, *pMenu_Sub;

	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_PARAMETERS);

	pMenu->Append(ID_CMD_MAP3D_FIRST, _TL("Rotation"), pMenu_Sub = new wxMenu());
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_X_LESS);
	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_X_MORE);
//	pMenu_Sub->AppendSeparator();
//	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_Y_LESS);
//	CMD_Menu_Add_Item(pMenu_Sub, false, ID_CMD_MAP3D_ROTATE_Y_MORE);
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

	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_EXAGGERATE_LESS);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_EXAGGERATE_MORE);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu    ,  true, ID_CMD_MAP3D_CENTRAL);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_CENTRAL_LESS);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_CENTRAL_MORE);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu    ,  true, ID_CMD_MAP3D_STEREO);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_STEREO_LESS);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_STEREO_MORE);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu    ,  true, ID_CMD_MAP3D_INTERPOLATED);
	CMD_Menu_Add_Item(pMenu    , false, ID_CMD_MAP3D_SAVE);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Map_3D::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_MAP_3D);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_MAP3D_PARAMETERS);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar,  true, ID_CMD_MAP3D_STEREO);
	CMD_ToolBar_Add_Item(pToolBar,  true, ID_CMD_MAP3D_INTERPOLATED);
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

	CMD_ToolBar_Add(pToolBar, _TL("3D-View"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::Do_Update(void)
{
	m_pPanel->Update_View(true);
}

//---------------------------------------------------------
void CVIEW_Map_3D::Update_StatusBar(void)
{
	if( m_pPanel )
	{
		SetStatusText(wxString::Format("X %+.1f", m_pPanel->Get_Projector().Get_xRotation() * M_RAD_TO_DEG), MAP3D_STATUSBAR_ROTATE_X);
		SetStatusText(wxString::Format("Y %+.1f", m_pPanel->Get_Projector().Get_yRotation() * M_RAD_TO_DEG), MAP3D_STATUSBAR_ROTATE_Y);
		SetStatusText(wxString::Format("Z %+.1f", m_pPanel->Get_Projector().Get_zRotation() * M_RAD_TO_DEG), MAP3D_STATUSBAR_ROTATE_Z);

		SetStatusText(wxString::Format("X %+.1f"    , m_pPanel->Get_Projector().Get_xShift()), MAP3D_STATUSBAR_SHIFT_X);
		SetStatusText(wxString::Format("Y %+.1f"    , m_pPanel->Get_Projector().Get_yShift()), MAP3D_STATUSBAR_SHIFT_Y);
		SetStatusText(wxString::Format("Z %+.1f"    , m_pPanel->Get_Projector().Get_zShift()), MAP3D_STATUSBAR_SHIFT_Z);

		SetStatusText(wxString::Format("E %.1f"     , m_pPanel->m_zScale), MAP3D_STATUSBAR_EXAGGERATION);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::On_Size(wxSizeEvent &event)
{
	m_pPanel->SetSize(GetClientRect());

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::On_Command(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	default:	return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_PARAMETERS:
		{
			Parameters_Update(true);

			if( DLG_Parameters(&m_Parameters) )
			{
				Parameters_Update(false);
			}
		}
		return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_SAVE:
		{
			int				FileType;
			wxString		FileName;
			CSG_Parameters	P(_TL("Image Resolution"));

			P.Add_Int("", "NX", _TL("Width" ), _TL(""), m_pPanel->GetSize().x, 1, true);
			P.Add_Int("", "NY", _TL("Height"), _TL(""), m_pPanel->GetSize().y, 1, true);

			if( DLG_Image_Save(FileName, FileType) && DLG_Parameters(&P) )
			{
				Set_Buisy_Cursor(true);

				if( P("NX")->asInt() == m_pPanel->GetSize().x
				&&  P("NY")->asInt() == m_pPanel->GetSize().y )
				{
					m_pPanel->Save_asImage(&FileName);
				}
				else
				{
					wxSize	Size(m_pPanel->GetSize());

					Freeze();
					m_pPanel->SetSize(P("NX")->asInt(), P("NY")->asInt());
					m_pPanel->Save_asImage(&FileName);
					m_pPanel->SetSize(Size);
					Thaw();
				}

				Set_Buisy_Cursor(false);
			}
		}
		return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_SEQ_POS_EDIT:
		{
			DLG_Table(_TL("Edit 3D-View Sequencer Positions"), m_pPanel->m_Parameters("PLAY")->asTable());
		}
		return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_SEQ_POS_ADD    :	m_pPanel->Play_Pos_Add();	return;
	case ID_CMD_MAP3D_SEQ_POS_DEL    :	m_pPanel->Play_Pos_Del();	return;
	case ID_CMD_MAP3D_SEQ_POS_DEL_ALL:	m_pPanel->Play_Pos_Clr();	return;
	case ID_CMD_MAP3D_SEQ_PLAY       :	m_pPanel->Play_Once   ();	return;
	case ID_CMD_MAP3D_SEQ_PLAY_LOOP  :	m_pPanel->Play_Loop   ();	return;
	case ID_CMD_MAP3D_SEQ_SAVE       :
		{
			int			Type;
			wxString	File;

			if( DLG_Image_Save(File, Type) )
			{
				m_pPanel->m_Parameters("PLAY_FILE")->Set_Value(File.wc_str());
				m_pPanel->Play_Save();
			}
		}
		return;

	//-----------------------------------------------------
	case ID_CMD_MAP3D_ROTATE_X_LESS  :	m_pPanel->Get_Projector().Inc_xRotation( 4.0 * M_DEG_TO_RAD);	break;
	case ID_CMD_MAP3D_ROTATE_X_MORE  :	m_pPanel->Get_Projector().Inc_xRotation(-4.0 * M_DEG_TO_RAD);	break;
	case ID_CMD_MAP3D_ROTATE_Y_LESS  :	m_pPanel->Get_Projector().Inc_yRotation( 4.0 * M_DEG_TO_RAD);	break;
	case ID_CMD_MAP3D_ROTATE_Y_MORE  :	m_pPanel->Get_Projector().Inc_yRotation(-4.0 * M_DEG_TO_RAD);	break;
	case ID_CMD_MAP3D_ROTATE_Z_LESS  :	m_pPanel->Get_Projector().Inc_zRotation( 4.0 * M_DEG_TO_RAD);	break;
	case ID_CMD_MAP3D_ROTATE_Z_MORE  :	m_pPanel->Get_Projector().Inc_zRotation(-4.0 * M_DEG_TO_RAD);	break;

	case ID_CMD_MAP3D_SHIFT_X_LESS   :	m_pPanel->Get_Projector().Inc_xShift(-10.0);	break;
	case ID_CMD_MAP3D_SHIFT_X_MORE   :	m_pPanel->Get_Projector().Inc_xShift( 10.0);	break;
	case ID_CMD_MAP3D_SHIFT_Y_LESS   :	m_pPanel->Get_Projector().Inc_yShift( 10.0);	break;
	case ID_CMD_MAP3D_SHIFT_Y_MORE   :	m_pPanel->Get_Projector().Inc_yShift(-10.0);	break;
	case ID_CMD_MAP3D_SHIFT_Z_LESS   :	m_pPanel->Get_Projector().Inc_zShift(-10.0);	break;
	case ID_CMD_MAP3D_SHIFT_Z_MORE   :	m_pPanel->Get_Projector().Inc_zShift( 10.0);	break;

	case ID_CMD_MAP3D_EXAGGERATE_LESS:	m_pPanel->m_zScale	-= 0.5;	break;
	case ID_CMD_MAP3D_EXAGGERATE_MORE:	m_pPanel->m_zScale	+= 0.5;	break;

	case ID_CMD_MAP3D_CENTRAL        :	m_pPanel->Get_Projector().do_Central(m_pPanel->Get_Projector().is_Central() == false);	break;
	case ID_CMD_MAP3D_CENTRAL_LESS   :	m_pPanel->Get_Projector().Inc_Central_Distance( 50);	break;
	case ID_CMD_MAP3D_CENTRAL_MORE   :	m_pPanel->Get_Projector().Inc_Central_Distance(-50);	break;

//	case ID_CMD_MAP3D_DRAW_BOX       :	m_pPanel->m_Parameters("DRAW_BOX"    )->Set_Value(m_pPanel->m_Parameters("DRAW_BOX"    )->asBool() ==  true    );	break;
	case ID_CMD_MAP3D_STEREO         :	m_pPanel->m_Parameters("STEREO"      )->Set_Value(m_pPanel->m_Parameters("STEREO"      )->asBool() == false    );	break;
	case ID_CMD_MAP3D_STEREO_LESS    :	m_pPanel->m_Parameters("STEREO_DIST" )->Set_Value(m_pPanel->m_Parameters("STEREO_DIST" )->asDouble() - 2       );	break;
	case ID_CMD_MAP3D_STEREO_MORE    :	m_pPanel->m_Parameters("STEREO_DIST" )->Set_Value(m_pPanel->m_Parameters("STEREO_DIST" )->asDouble() + 2       );	break;
	case ID_CMD_MAP3D_INTERPOLATED   :	m_pPanel->m_Parameters("DRAPE_MODE"  )->Set_Value(m_pPanel->m_Parameters("DRAPE_MODE"  )->asInt() == 0 ? 1 : 0 );	break;

//	case ID_CMD_MAP3D_SRC_RES_LESS   :	m_pPanel->;	break;
//	case ID_CMD_MAP3D_SRC_RES_MORE   :	m_pPanel->;	break;
	}

	Parameters_Update(true);
	m_pPanel->Update_View();
}

//---------------------------------------------------------
void CVIEW_Map_3D::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_MAP3D_CENTRAL:
		event.Check(m_pPanel->Get_Projector().is_Central());
		break;

	case ID_CMD_MAP3D_CENTRAL_LESS:
	case ID_CMD_MAP3D_CENTRAL_MORE:
		event.Enable(m_pPanel->Get_Projector().is_Central());
		break;

	case ID_CMD_MAP3D_INTERPOLATED:
		event.Check(m_pPanel->m_Parameters("DRAPE_MODE")->asInt() != 0);
		break;

	case ID_CMD_MAP3D_STEREO:
		event.Check(m_pPanel->m_Parameters("STEREO")->asBool());
		break;

	case ID_CMD_MAP3D_SEQ_PLAY:
		event.Check(m_pPanel->Play_Get_State() == SG_3DVIEW_PLAY_RUN_ONCE);
		break;

	case ID_CMD_MAP3D_SEQ_PLAY_LOOP:
		event.Check(m_pPanel->Play_Get_State() == SG_3DVIEW_PLAY_RUN_LOOP);
		break;

	case ID_CMD_MAP3D_SEQ_SAVE:
		event.Check(m_pPanel->Play_Get_State() == SG_3DVIEW_PLAY_RUN_SAVE);
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3D::Parameters_Update(bool bFromPanel)
{
	if( bFromPanel )
	{
		m_Parameters("ROTATION_X"  )->Set_Value(m_pPanel->Get_Projector().Get_xRotation() * M_RAD_TO_DEG);
		m_Parameters("ROTATION_Y"  )->Set_Value(m_pPanel->Get_Projector().Get_yRotation() * M_RAD_TO_DEG);
		m_Parameters("ROTATION_Z"  )->Set_Value(m_pPanel->Get_Projector().Get_zRotation() * M_RAD_TO_DEG);

		m_Parameters("SHIFT_X"     )->Set_Value(m_pPanel->Get_Projector().Get_xShift());
		m_Parameters("SHIFT_Y"     )->Set_Value(m_pPanel->Get_Projector().Get_yShift());
		m_Parameters("SHIFT_Z"     )->Set_Value(m_pPanel->Get_Projector().Get_zShift());

		m_Parameters("CENTRAL"     )->Set_Value(m_pPanel->Get_Projector().is_Central() ? 1 : 0);
		m_Parameters("CENTRAL_DIST")->Set_Value(m_pPanel->Get_Projector().Get_Central_Distance());

		m_Parameters("BGCOLOR"     )->Set_Value(m_pPanel->m_Parameters("BGCOLOR"    ));
		m_Parameters("STEREO"      )->Set_Value(m_pPanel->m_Parameters("STEREO"     ));
		m_Parameters("STEREO_DIST" )->Set_Value(m_pPanel->m_Parameters("STEREO_DIST"));
		m_Parameters("DRAW_BOX"    )->Set_Value(m_pPanel->m_Parameters("DRAW_BOX"   ));

		m_Parameters("Z_SCALE"     )->Set_Value(m_pPanel->m_zScale);
		m_Parameters("DEM_RES"     )->Set_Value(m_pPanel->Get_DEM_Res());
		m_Parameters("MAP_RES"     )->Set_Value(m_pPanel->Get_Map_Res());
	}

	//-----------------------------------------------------
	else
	{
		m_pPanel->Get_Projector().Set_Rotation(
			m_Parameters("ROTATION_X")->asDouble() * M_DEG_TO_RAD,
			m_Parameters("ROTATION_Y")->asDouble() * M_DEG_TO_RAD,
			m_Parameters("ROTATION_Z")->asDouble() * M_DEG_TO_RAD
		);

		m_pPanel->Get_Projector().Set_Shift(
			m_Parameters("SHIFT_X"   )->asDouble(),
			m_Parameters("SHIFT_Y"   )->asDouble(),
			m_Parameters("SHIFT_Z"   )->asDouble()
		);

		m_pPanel->Get_Projector().do_Central          (m_Parameters("CENTRAL"     )->asBool  ());
		m_pPanel->Get_Projector().Set_Central_Distance(m_Parameters("CENTRAL_DIST")->asDouble());

		m_pPanel->m_Parameters("BGCOLOR"    )->Set_Value(m_Parameters("BGCOLOR"    ));
		m_pPanel->m_Parameters("STEREO"     )->Set_Value(m_Parameters("STEREO"     ));
		m_pPanel->m_Parameters("STEREO_DIST")->Set_Value(m_Parameters("STEREO_DIST"));
		m_pPanel->m_Parameters("DRAPE_MODE" )->Set_Value(m_Parameters("DRAPE_MODE" ));
		m_pPanel->m_Parameters("DRAW_BOX"   )->Set_Value(m_Parameters("DRAW_BOX"   ));

		m_pPanel->m_zScale	= m_Parameters("Z_SCALE")->asDouble();

		//-------------------------------------------------
		CSG_Grid	*pDEM	= m_Parameters("DEM")->asGrid();

		if( !m_pPanel->Set_Options(
			SG_Get_Data_Manager().Exists(pDEM) ? pDEM : NULL,
			m_Parameters("DEM_RES")->asInt(),
			m_Parameters("MAP_RES")->asInt()) )
		{
			m_pPanel->Update_View();
		}
	}

	Update_StatusBar();
}

//---------------------------------------------------------
void CVIEW_Map_3D::Parameters_Create(void)
{
	m_Parameters.Create(_TL("3D-View"));

	m_Parameters.Add_Grid  (""   , "DEM"     , _TL("Elevation"   ), _TL(""), PARAMETER_INPUT);
	m_Parameters.Add_Int   ("DEM", "DEM_RES" , _TL("Resolution"  ), _TL(""), 100, 2, true);
	m_Parameters.Add_Double("DEM", "Z_SCALE" , _TL("Exaggeration"), _TL(""), 1.0);
	m_Parameters.Add_Bool  ("DEM", "DRAW_BOX", _TL("Bounding Box"), _TL(""), false);

	m_Parameters.Add_Node  (""   , "MAP"       , _TL("Map"                      ), _TL(""));
	m_Parameters.Add_Int   ("MAP", "MAP_RES"   , _TL("Resolution"               ), _TL(""), 1000, 2, true);
	m_Parameters.Add_Choice("MAP", "DRAPE_MODE", _TL("Map Draping Interpolation"), _TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s",
			_TL("None"),
			_TL("Bilinear"),
			_TL("Inverse Distance"),
			_TL("Bicubic Spline"),
			_TL("B-Spline")
		), 0
	);

	m_Parameters.Add_Node  (""        , "ROTATION"  , _TL("Rotation"  ), _TL(""));
	m_Parameters.Add_Double("ROTATION", "ROTATION_X", _TL("X"         ), _TL(""),  55.0, -360.0, true, 360.0, true);
	m_Parameters.Add_Double("ROTATION", "ROTATION_Y", _TL("Y"         ), _TL(""),   0.0, -360.0, true, 360.0, true);
	m_Parameters.Add_Double("ROTATION", "ROTATION_Z", _TL("Z"         ), _TL(""), -45.0, -360.0, true, 360.0, true);

	m_Parameters.Add_Node  (""        , "SHIFT"     , _TL("Shift"     ), _TL(""));
	m_Parameters.Add_Double("SHIFT"   , "SHIFT_X"   , _TL("Left/Right"), _TL(""),    0.0);
	m_Parameters.Add_Double("SHIFT"   , "SHIFT_Y"   , _TL("Up/Down"   ), _TL(""),    0.0);
	m_Parameters.Add_Double("SHIFT"   , "SHIFT_Z"   , _TL("In/Out"    ), _TL(""), 1500.0);

	m_Parameters.Add_Choice(""        , "CENTRAL"     , _TL("Projection"           ), _TL(""), CSG_String::Format("%s|%s|", _TL("parallel"), _TL("central")), 1);
	m_Parameters.Add_Double("CENTRAL" , "CENTRAL_DIST", _TL("Perspectivic Distance"), _TL(""), 1500, 1, true);

	m_Parameters.Add_Bool  (""        , "STEREO"     , _TL("Anaglyph"             ), _TL(""), false);
	m_Parameters.Add_Double("STEREO"  , "STEREO_DIST", _TL("Eye Distance [Degree]"), _TL(""), 2.0, 0, true, 180, true);

	m_Parameters.Add_Color("", "BGCOLOR", _TL("Background Color"), _TL(""), SG_GET_RGB(255, 255, 255));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
