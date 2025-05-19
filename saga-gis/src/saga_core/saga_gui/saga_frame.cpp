
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
//                    SAGA_Frame.cpp                     //
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
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/filename.h>
#include <wx/statusbr.h>
#include <wx/icon.h>
#include <wx/gauge.h>
#include <wx/choicdlg.h>
#include <wx/toolbar.h>
#include <wx/tipdlg.h>
#include <wx/aui/aui.h>
#include <wx/display.h>

#include <saga_api/saga_api.h>

#include "callback.h"

#include "res_commands.h"
#include "res_controls.h"
#include "res_dialogs.h"
#include "res_images.h"

#include "helper.h"

#include "saga.h"
#include "saga_frame.h"
#include "saga_frame_droptarget.h"

#include "info.h"
#include "data_source.h"
#include "active.h"

#include "wksp.h"
#include "wksp_base_control.h"
#include "wksp_tool_manager.h"
#include "wksp_tool.h"
#include "wksp_data_control.h"
#include "wksp_data_manager.h"
#include "wksp_data_menu_files.h"
#include "wksp_map_manager.h"

#include "view_map.h"
#include "view_map_3d.h"
#include "view_table.h"
#include "view_table_diagram.h"
#include "view_histogram.h"
#include "view_scatterplot.h"
#include "view_layout.h"

#include "dlg_about.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if defined(_DEBUG)
	#if defined(_SAGA_MSW) && !defined(_WIN64)
		#define SAGA_CAPTION	wxString::Format("DEBUG|32-Bit|SAGA %s", SAGA_VERSION)
	#else
		#define SAGA_CAPTION	wxString::Format("DEBUG|SAGA %s", SAGA_VERSION)
	#endif
#else
	#if defined(_SAGA_MSW) && !defined(_WIN64)
		#define SAGA_CAPTION	wxString::Format("32-Bit|SAGA %s", SAGA_VERSION)
	#else
		#define SAGA_CAPTION	wxString::Format("SAGA %s", SAGA_VERSION)
	#endif
#endif


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSAGA_Frame_StatusBar : public wxStatusBar
{
public:
	CSAGA_Frame_StatusBar(wxWindow *parent, wxWindowID id, long style = wxST_SIZEGRIP, const wxString &name = "StatusBar")
		: wxStatusBar(parent, id, style, name)
	{
		m_pProgressBar = new wxGauge(this, ID_WND_PROGRESSBAR, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH);
	}

	void		On_Size		(wxSizeEvent &event)
	{
		wxRect r;

		if( m_pProgressBar && GetFieldRect(STATUSBAR_PROGRESS, r) )
		{
			m_pProgressBar->SetSize(r);
		}
	}

	wxGauge *m_pProgressBar;

	DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSAGA_Frame_StatusBar, wxStatusBar)
	EVT_SIZE(CSAGA_Frame_StatusBar::On_Size)
END_EVENT_TABLE()

//---------------------------------------------------------
wxStatusBar * CSAGA_Frame::OnCreateStatusBar(int number, long style, wxWindowID id, const wxString& name)
{
	CSAGA_Frame_StatusBar *StatusBar = new CSAGA_Frame_StatusBar(this, id, style, name);

	StatusBar->SetFieldsCount(number);

	return( StatusBar );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSAGA_Frame *g_pSAGA_Frame = NULL;


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSAGA_Frame, MDI_ParentFrame)
	EVT_CLOSE(CSAGA_Frame::On_Close)
	EVT_SIZE (CSAGA_Frame::On_Size)

	EVT_MENU     (ID_CMD_FRAME_QUIT            , CSAGA_Frame::On_Quit)
	EVT_MENU     (ID_CMD_FRAME_HELP            , CSAGA_Frame::On_Help)
	EVT_MENU     (ID_CMD_FRAME_ABOUT           , CSAGA_Frame::On_About)
	EVT_MENU     (ID_CMD_FRAME_TIPS            , CSAGA_Frame::On_Tips)

	EVT_MENU     (ID_CMD_FRAME_CASCADE         , CSAGA_Frame::On_Frame_Cascade)
	EVT_UPDATE_UI(ID_CMD_FRAME_CASCADE         , CSAGA_Frame::On_Frame_Cascade_UI)
	EVT_MENU     (ID_CMD_FRAME_TILE_HORZ       , CSAGA_Frame::On_Frame_hTile)
	EVT_UPDATE_UI(ID_CMD_FRAME_TILE_HORZ       , CSAGA_Frame::On_Frame_hTile_UI)
	EVT_MENU     (ID_CMD_FRAME_TILE_VERT       , CSAGA_Frame::On_Frame_vTile)
	EVT_UPDATE_UI(ID_CMD_FRAME_TILE_VERT       , CSAGA_Frame::On_Frame_vTile_UI)
	EVT_MENU     (ID_CMD_FRAME_ARRANGEICONS    , CSAGA_Frame::On_Frame_ArrangeIcons)
	EVT_UPDATE_UI(ID_CMD_FRAME_ARRANGEICONS    , CSAGA_Frame::On_Frame_ArrangeIcons_UI)
	EVT_MENU     (ID_CMD_FRAME_UNSPLIT         , CSAGA_Frame::On_Frame_Unsplit)
	EVT_UPDATE_UI(ID_CMD_FRAME_UNSPLIT         , CSAGA_Frame::On_Frame_Unsplit_UI)
	EVT_MENU     (ID_CMD_FRAME_SPLIT_LEFT      , CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI(ID_CMD_FRAME_SPLIT_LEFT      , CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU     (ID_CMD_FRAME_SPLIT_RIGHT     , CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI(ID_CMD_FRAME_SPLIT_RIGHT     , CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU     (ID_CMD_FRAME_SPLIT_TOP       , CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI(ID_CMD_FRAME_SPLIT_TOP       , CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU     (ID_CMD_FRAME_SPLIT_BOTTOM    , CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI(ID_CMD_FRAME_SPLIT_BOTTOM    , CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU     (ID_CMD_FRAME_SPLIT_ALL_HORZ  , CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI(ID_CMD_FRAME_SPLIT_ALL_HORZ  , CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU     (ID_CMD_FRAME_SPLIT_ALL_VERT  , CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI(ID_CMD_FRAME_SPLIT_ALL_VERT  , CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU     (ID_CMD_FRAME_FIND            , CSAGA_Frame::On_Frame_Find)
	EVT_UPDATE_UI(ID_CMD_FRAME_FIND            , CSAGA_Frame::On_Frame_Find_UI)
	EVT_MENU     (ID_CMD_FRAME_NEXT            , CSAGA_Frame::On_Frame_Next)
	EVT_UPDATE_UI(ID_CMD_FRAME_NEXT            , CSAGA_Frame::On_Frame_Next_UI)
	EVT_MENU     (ID_CMD_FRAME_PREVIOUS        , CSAGA_Frame::On_Frame_Previous)
	EVT_UPDATE_UI(ID_CMD_FRAME_PREVIOUS        , CSAGA_Frame::On_Frame_Previous_UI)
	EVT_MENU     (ID_CMD_FRAME_CLOSE           , CSAGA_Frame::On_Frame_Close)
	EVT_UPDATE_UI(ID_CMD_FRAME_CLOSE           , CSAGA_Frame::On_Frame_Close_UI)
	EVT_MENU     (ID_CMD_FRAME_CLOSE_ALL       , CSAGA_Frame::On_Frame_Close_All)
	EVT_UPDATE_UI(ID_CMD_FRAME_CLOSE_ALL       , CSAGA_Frame::On_Frame_Close_All_UI)

	EVT_MENU     (ID_CMD_FRAME_WKSP_SHOW       , CSAGA_Frame::On_WKSP_Show)
	EVT_UPDATE_UI(ID_CMD_FRAME_WKSP_SHOW       , CSAGA_Frame::On_WKSP_Show_UI)
	EVT_MENU     (ID_CMD_FRAME_ACTIVE_SHOW     , CSAGA_Frame::On_Active_Show)
	EVT_UPDATE_UI(ID_CMD_FRAME_ACTIVE_SHOW     , CSAGA_Frame::On_Active_Show_UI)
	EVT_MENU     (ID_CMD_FRAME_DATA_SOURCE_SHOW, CSAGA_Frame::On_Data_Source_Show)
	EVT_UPDATE_UI(ID_CMD_FRAME_DATA_SOURCE_SHOW, CSAGA_Frame::On_Data_Source_Show_UI)
	EVT_MENU     (ID_CMD_FRAME_INFO_SHOW       , CSAGA_Frame::On_INFO_Show)
	EVT_UPDATE_UI(ID_CMD_FRAME_INFO_SHOW       , CSAGA_Frame::On_INFO_Show_UI)

	EVT_MENU_RANGE		(ID_CMD_WKSP_FIRST     , ID_CMD_WKSP_LAST     , CSAGA_Frame::On_Command_Workspace)
	EVT_UPDATE_UI_RANGE	(ID_CMD_WKSP_FIRST     , ID_CMD_WKSP_LAST     , CSAGA_Frame::On_Command_Workspace_UI)
	EVT_MENU_RANGE		(ID_CMD_DATA_FIRST     , ID_CMD_DATA_LAST     , CSAGA_Frame::On_Command_Data)
	EVT_UPDATE_UI_RANGE	(ID_CMD_DATA_FIRST     , ID_CMD_DATA_LAST     , CSAGA_Frame::On_Command_Data_UI)
	EVT_MENU_RANGE		(ID_CMD_TOOL_FIRST     , ID_CMD_TOOL_LAST     , CSAGA_Frame::On_Command_Tool)
	EVT_UPDATE_UI_RANGE	(ID_CMD_TOOL_FIRST     , ID_CMD_TOOL_LAST     , CSAGA_Frame::On_Command_Tool_UI)

	EVT_MENU_RANGE		(ID_CMD_CHILD_FIRST    , ID_CMD_CHILD_LAST    , CSAGA_Frame::On_Command_Child)
	EVT_UPDATE_UI_RANGE	(ID_CMD_MAP_FIRST      , ID_CMD_MAP_LAST      , CSAGA_Frame::On_Command_Child_UI)
	EVT_UPDATE_UI_RANGE	(ID_CMD_HISTOGRAM_FIRST, ID_CMD_HISTOGRAM_LAST, CSAGA_Frame::On_Command_Child_UI)
	EVT_UPDATE_UI_RANGE	(ID_CMD_DIAGRAM_FIRST  , ID_CMD_DIAGRAM_LAST  , CSAGA_Frame::On_Command_Child_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef MDI_TABBED
#define MDI_PARENT_FRAME_STYLE	wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE|wxFRAME_NO_WINDOW_MENU
#else
#define MDI_PARENT_FRAME_STYLE	wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE|wxFRAME_NO_WINDOW_MENU|wxHSCROLL|wxVSCROLL
#endif

//---------------------------------------------------------
CSAGA_Frame::CSAGA_Frame(void)
	: MDI_ParentFrame(NULL, ID_WND_MAIN, SAGA_CAPTION, wxDefaultPosition, wxDefaultSize, MDI_PARENT_FRAME_STYLE)
{
	g_pSAGA_Frame  = this;

	m_nTopWindows  = 0;
	m_pTopWindows  = NULL;

	m_pINFO        = NULL;
	m_pData_Source = NULL;
	m_pActive      = NULL;
	m_pWKSP        = NULL;

	SG_Set_UI_Callback(Get_Callback());

#ifndef WITH_WXBMPBUNDLE
	SetIcon (IMG_Get_Icon (ID_IMG_SAGA_ICON_32));
#else
	SetIcons(IMG_Get_Icons(ID_IMG_SAGA_ICON));
#endif

	SetDropTarget(new CSAGA_Frame_DropTarget);

	//-----------------------------------------------------
	int STATUSBAR_Sizes[STATUSBAR_COUNT] = { -1, 125, 125, 125, -1 };

	CreateStatusBar   (STATUSBAR_COUNT);
	SetStatusWidths   (STATUSBAR_COUNT, STATUSBAR_Sizes);
	SetStatusBarPane  (STATUSBAR_DEFAULT);
	StatusBar_Set_Text(_TL("ready"));

	m_pProgressBar = ((CSAGA_Frame_StatusBar *)GetStatusBar())->m_pProgressBar;

	//-----------------------------------------------------
	m_pLayout = new wxAuiManager(this);

	m_pLayout->GetArtProvider()->SetColor	(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR,
		wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION)
	);

	m_pLayout->GetArtProvider()->SetColor	(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR,
		wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE)
	);

	m_pLayout->GetArtProvider()->SetMetric	(wxAUI_DOCKART_GRADIENT_TYPE	, wxAUI_GRADIENT_NONE);
	m_pLayout->GetArtProvider()->SetMetric	(wxAUI_DOCKART_CAPTION_SIZE		, 14);

	m_pLayout->SetFlags(m_pLayout->GetFlags() ^ wxAUI_MGR_TRANSPARENT_DRAG);
//	m_pLayout->SetFlags(m_pLayout->GetFlags() ^ wxAUI_MGR_ALLOW_ACTIVE_PANE);

	m_pLayout->GetPane(GetClientWindow()).Show().Center();

	//-----------------------------------------------------
	_Bar_Add(m_pINFO        = new CINFO       (this), 0, 0); m_pINFO       ->Add_Pages();
	_Bar_Add(m_pWKSP        = new CWKSP       (this), 2, 1); m_pWKSP       ->Add_Pages();
	_Bar_Add(m_pData_Source = new CData_Source(this), 2, 1); m_pData_Source->Add_Pages();
	_Bar_Add(m_pActive      = new CActive     (this), 2, 0); m_pActive     ->Add_Pages();

	//-----------------------------------------------------
	_Create_MenuBar();

	//-----------------------------------------------------
	wxString s;

	if( CONFIG_Read("/FL", "MANAGER", s) )
	{
		m_pLayout->LoadPerspective(s, false);
	}

	Set_Pane_Caption(m_pINFO       , m_pINFO       ->GetName());	// captions might have been modified by perspective, so update again...
	Set_Pane_Caption(m_pWKSP       , m_pWKSP       ->GetName());
	Set_Pane_Caption(m_pData_Source, m_pData_Source->GetName());
	Set_Pane_Caption(m_pActive     , m_pActive     ->GetName());

	//-----------------------------------------------------
	m_pTB_Main        =                      _Create_ToolBar();
	m_pTB_Map         = CVIEW_Map          ::_Create_ToolBar();
	m_pTB_Map_3D      = CVIEW_Map_3D       ::_Create_ToolBar();
	m_pTB_Layout      = CVIEW_Layout       ::_Create_ToolBar();
	m_pTB_Table       = CVIEW_Table        ::_Create_ToolBar();
	m_pTB_Diagram     = CVIEW_Table_Diagram::_Create_ToolBar();
	m_pTB_Histogram   = CVIEW_Histogram    ::_Create_ToolBar();
	m_pTB_ScatterPlot = CVIEW_ScatterPlot  ::_Create_ToolBar();
	m_pTB_Data        = NULL;

	_Bar_Show(m_pTB_Main, true);

	m_pLayout->Update();

	//-----------------------------------------------------
	long l; wxRect r, rDefault = wxGetClientDisplayRect();

	rDefault.Deflate((int)(0.1 * rDefault.GetWidth()), (int)(0.1 * rDefault.GetHeight()));

	r.SetLeft  (CONFIG_Read("/FL", "X" , l) ? l : rDefault.GetLeft  ());
	r.SetTop   (CONFIG_Read("/FL", "Y" , l) ? l : rDefault.GetTop   ());
	r.SetWidth (CONFIG_Read("/FL", "DX", l) ? l : rDefault.GetWidth ());
	r.SetHeight(CONFIG_Read("/FL", "DY", l) ? l : rDefault.GetHeight());

	if( wxDisplay::GetFromPoint(r.GetTopLeft    ()) == wxNOT_FOUND
	&&  wxDisplay::GetFromPoint(r.GetTopRight   ()) == wxNOT_FOUND
	&&  wxDisplay::GetFromPoint(r.GetBottomLeft ()) == wxNOT_FOUND
	&&  wxDisplay::GetFromPoint(r.GetBottomRight()) == wxNOT_FOUND	)
	{
		r = rDefault;
	}

	SetSize(r);

	if( !(CONFIG_Read("/FL", "STATE", l) && l == 0) )
	{
		Maximize();
	}

	//-----------------------------------------------------
	Show(true);

	Update();

	//-----------------------------------------------------
	m_pData_Source->Autoconnect_DB();
}

//---------------------------------------------------------
CSAGA_Frame::~CSAGA_Frame(void)
{
	if( IsIconized() )
	{
		Iconize(false);
	}

	if( IsMaximized() )
	{
		CONFIG_Write("/FL", "STATE", (long)1);
	}
	else
	{
		CONFIG_Write("/FL", "STATE", (long)0);

		CONFIG_Write("/FL", "X"    , (long)GetPosition().x);
		CONFIG_Write("/FL", "Y"    , (long)GetPosition().y);
		CONFIG_Write("/FL", "DX"   , (long)GetSize    ().x);
		CONFIG_Write("/FL", "DY"   , (long)GetSize    ().y);
	}

	CONFIG_Write("/FL", "MANAGER", m_pLayout->SavePerspective());

	m_pLayout->UnInit();

	delete(m_pLayout);

	//-----------------------------------------------------
	if( GetMenuBar()->GetMenuCount() == 5 )
	{
		GetMenuBar()->Remove(2);
	}

	delete(m_pMN_Table      );
	delete(m_pMN_Diagram    );
	delete(m_pMN_Map        );
	delete(m_pMN_Map_3D     );
	delete(m_pMN_Histogram  );
	delete(m_pMN_ScatterPlot);
	delete(m_pMN_Layout     );

	//-----------------------------------------------------
	if( m_pTopWindows )
	{
		delete(m_pTopWindows);
	}

	SG_Set_UI_Callback(NULL);

	g_pSAGA_Frame	= NULL;
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::On_Close(wxCloseEvent &event)
{
	if( event.CanVeto() == false )
	{
		g_pTools->Finalise();

		g_pData->Close(true);

		event.Skip();

		return;
	}

	if( g_pTool )
	{
		if( g_pTool->is_Executing() )
		{
			g_pTool->Finish( true,  true);
		}
		else
		{
			g_pTool->Finish(false, false);
		}
	}

	if( !g_pTool && g_pData->Finalise() && g_pData->Close(true) )
	{
		g_pTools->Finalise();

		Destroy();
	}
	else
	{
		event.Veto();
	}
}

//---------------------------------------------------------
void CSAGA_Frame::On_Size(wxSizeEvent &event)
{
	if( wxDisplay::GetFromWindow(this) == wxNOT_FOUND )
	{
		wxRect	r	= wxGetClientDisplayRect();	r.Deflate((int)(0.1 * r.GetWidth()), (int)(0.1 * r.GetHeight()));

		SetSize(r);
	}

	event.Skip();
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::On_Quit(wxCommandEvent &WXUNUSED(event))
{
	Close(false);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Help(wxCommandEvent &WXUNUSED(event))
{
	if( !Open_WebBrowser("https://sourceforge.net/p/saga-gis/wiki/Online%20Help/") )
	{
	//	DLG_Message_Show(_TL("Online Help"), _TL("SAGA Help"));
	}
}

//---------------------------------------------------------
void CSAGA_Frame::On_About(wxCommandEvent &WXUNUSED(event))
{
	CDLG_About dlg; dlg.ShowModal();
}

//---------------------------------------------------------
void CSAGA_Frame::On_Tips(wxCommandEvent &WXUNUSED(event))
{
	Show_Tips(true);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_Unsplit(wxCommandEvent &WXUNUSED(event))
{
#ifdef MDI_TABBED
	int		i, n	= GetNotebook()->GetPageCount();

	if( n > 1 )
	{
		GetNotebook()->Freeze();

		class CPage { public: wxWindow *pPage; wxString Text; wxBitmap Bmp; };

		CPage	*pPages	= new CPage[n];

		for(i=n-1; i>=0; i--)
		{
			pPages[i].pPage	= GetNotebook()->GetPage      (i);
			pPages[i].Text	= GetNotebook()->GetPageText  (i);
			pPages[i].Bmp	= GetNotebook()->GetPageBitmap(i);

			GetNotebook()->RemovePage(i);
		}

		for(i=0; i<n; i++)
		{
			GetNotebook()->AddPage(pPages[i].pPage, pPages[i].Text, false, pPages[i].Bmp);
		}

		delete[](pPages);

		GetNotebook()->Thaw();
	}
#endif
}

void CSAGA_Frame::On_Frame_Unsplit_UI(wxUpdateUIEvent &event)
{
#ifdef MDI_TABBED
	event.Enable(GetNotebook()->GetPageCount() > 1);
#endif
}

//---------------------------------------------------------
void CSAGA_Frame::Split(int Arrange)
{
#ifdef MDI_TABBED
	int n = GetNotebook()->GetPageCount();

	if( n > 1 )
	{
		GetNotebook()->Freeze();

		int iActive = GetNotebook()->GetSelection(); if( iActive < 0 ) iActive = 0;
	//	int nSqrt   = (int)sqrt((double)n);

		switch( Arrange )
		{
		case ID_CMD_FRAME_SPLIT_LEFT    : GetNotebook()->Split(iActive, wxLEFT  ); break;
		case ID_CMD_FRAME_SPLIT_RIGHT   : GetNotebook()->Split(iActive, wxRIGHT ); break;
		case ID_CMD_FRAME_SPLIT_TOP     : GetNotebook()->Split(iActive, wxTOP   ); break;
		case ID_CMD_FRAME_SPLIT_BOTTOM  : GetNotebook()->Split(iActive, wxBOTTOM); break;

		case ID_CMD_FRAME_SPLIT_ALL_HORZ:
			{
				for(int i=n-1, iRow=0; i>0; i--, iRow++)
				{
					GetNotebook()->Split(i, wxLEFT);
				}
			}
			break;

		case ID_CMD_FRAME_SPLIT_ALL_VERT:
			{
				for(int i=n-1, iRow=0; i>0; i--, iRow++)
				{
					GetNotebook()->Split(i, wxBOTTOM);
				}
			}
			break;
		}

		GetNotebook()->Thaw();
	}
#endif
}

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_Split(wxCommandEvent &event)
{
	Split(event.GetId());
}

void CSAGA_Frame::On_Frame_Split_UI(wxUpdateUIEvent &event)
{
#ifdef MDI_TABBED
	event.Enable(GetNotebook()->GetPageCount() > 1);
#endif
}

//---------------------------------------------------------
void CSAGA_Frame::Tile(wxOrientation orient)
{
#ifndef MDI_TABBED
	if( m_Children.Get_Size() == 1 && GetActiveChild() )
	{
		GetActiveChild()->Maximize();
	}
	else
	{
		MDI_ParentFrame::Tile(orient);
	}
#endif
}

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_Cascade(wxCommandEvent &WXUNUSED(event))
{
#ifndef MDI_TABBED
	Cascade();
#endif
}

void CSAGA_Frame::On_Frame_Cascade_UI(wxUpdateUIEvent &event)
{
	event.Enable(GetActiveChild() != NULL);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_hTile(wxCommandEvent &WXUNUSED(event))
{
	Tile(wxHORIZONTAL);
}

void CSAGA_Frame::On_Frame_hTile_UI(wxUpdateUIEvent &event)
{
	event.Enable(GetActiveChild() != NULL);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_vTile(wxCommandEvent &WXUNUSED(event))
{
	Tile(wxVERTICAL);
}

void CSAGA_Frame::On_Frame_vTile_UI(wxUpdateUIEvent &event)
{
	event.Enable(GetActiveChild() != NULL);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_ArrangeIcons(wxCommandEvent &WXUNUSED(event))
{
	ArrangeIcons();
}

void CSAGA_Frame::On_Frame_ArrangeIcons_UI(wxUpdateUIEvent &event)
{
	event.Enable(GetActiveChild() != NULL);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_Find(wxCommandEvent &WXUNUSED(event))
{
	wxArrayString Children;

	for(sLong i=0; i<m_Children.Get_Size(); i++)
	{
		Children.Add(((CVIEW_Base *)m_Children[i])->GetTitle());
	}

	wxSingleChoiceDialog dlg(this, _TL("Goto Window"), _TL("Find"), Children);

	if( dlg.ShowModal() == wxID_OK )
	{
		((CVIEW_Base *)m_Children[dlg.GetSelection()])->Activate();
	}
}

void CSAGA_Frame::On_Frame_Find_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_Children.Get_Size() > 1);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_Next(wxCommandEvent &WXUNUSED(event))
{
	ActivateNext();
}

void CSAGA_Frame::On_Frame_Next_UI(wxUpdateUIEvent &event)
{
	event.Enable(GetActiveChild() != NULL);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_Previous(wxCommandEvent &WXUNUSED(event))
{
	ActivatePrevious();
}

void CSAGA_Frame::On_Frame_Previous_UI(wxUpdateUIEvent &event)
{
	event.Enable(GetActiveChild() != NULL);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_Close(wxCommandEvent &WXUNUSED(event))
{
	if( GetActiveChild() != NULL )
	{
		GetActiveChild()->Close();
	}
}

void CSAGA_Frame::On_Frame_Close_UI(wxUpdateUIEvent &event)
{
	event.Enable(GetActiveChild() != NULL);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Frame_Close_All(wxCommandEvent &WXUNUSED(event))
{
	Close_Children();
}

void CSAGA_Frame::On_Frame_Close_All_UI(wxUpdateUIEvent &event)
{
	event.Enable(GetActiveChild() != NULL);
}

//---------------------------------------------------------
bool CSAGA_Frame::Arrange_Children(int Arrange)
{
#ifndef MDI_TABBED
	if     ( (Arrange & SG_UI_WINDOW_ARRANGE_MDI_CASCADE     ) != 0 ) { Cascade();          }
	else if( (Arrange & SG_UI_WINDOW_ARRANGE_MDI_TILE_VER    ) != 0 ) { Tile(wxVERTICAL  ); }
	else if( (Arrange & SG_UI_WINDOW_ARRANGE_MDI_TILE_HOR    ) != 0 ) { Tile(wxHORIZONTAL); }
#else // #ifdef MDI_TABBED
	if     ( (Arrange & SG_UI_WINDOW_ARRANGE_TDI_TILE_VER    ) != 0 ) { Split(ID_CMD_FRAME_SPLIT_ALL_VERT); }
	else if( (Arrange & SG_UI_WINDOW_ARRANGE_TDI_TILE_HOR    ) != 0 ) { Split(ID_CMD_FRAME_SPLIT_ALL_HORZ); }
	else if( (Arrange & SG_UI_WINDOW_ARRANGE_TDI_SPLIT_LEFT  ) != 0 ) { Split(ID_CMD_FRAME_SPLIT_LEFT    ); }
	else if( (Arrange & SG_UI_WINDOW_ARRANGE_TDI_SPLIT_RIGHT ) != 0 ) { Split(ID_CMD_FRAME_SPLIT_RIGHT   ); }
	else if( (Arrange & SG_UI_WINDOW_ARRANGE_TDI_SPLIT_TOP   ) != 0 ) { Split(ID_CMD_FRAME_SPLIT_TOP     ); }
	else if( (Arrange & SG_UI_WINDOW_ARRANGE_TDI_SPLIT_BOTTOM) != 0 ) { Split(ID_CMD_FRAME_SPLIT_BOTTOM  ); }
#endif
	else
	{
		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::On_WKSP_Show(wxCommandEvent &WXUNUSED(event))
{
	_Bar_Toggle(m_pWKSP);
}

void CSAGA_Frame::On_WKSP_Show_UI(wxUpdateUIEvent &event)
{
	event.Check(m_pWKSP->IsShown() && m_pLayout->GetPane(m_pWKSP).IsShown());
}

//---------------------------------------------------------
void CSAGA_Frame::On_Active_Show(wxCommandEvent &WXUNUSED(event))
{
	_Bar_Toggle(m_pActive);
}

void CSAGA_Frame::On_Active_Show_UI(wxUpdateUIEvent &event)
{
	event.Check(m_pActive->IsShown() && m_pLayout->GetPane(m_pActive).IsShown());
}

//---------------------------------------------------------
void CSAGA_Frame::On_Data_Source_Show(wxCommandEvent &WXUNUSED(event))
{
	_Bar_Toggle(m_pData_Source);
}

void CSAGA_Frame::On_Data_Source_Show_UI(wxUpdateUIEvent &event)
{
	event.Check(m_pData_Source->IsShown() && m_pLayout->GetPane(m_pData_Source).IsShown());
}

//---------------------------------------------------------
void CSAGA_Frame::On_INFO_Show(wxCommandEvent &WXUNUSED(event))
{
	_Bar_Toggle(m_pINFO);
}

void CSAGA_Frame::On_INFO_Show_UI(wxUpdateUIEvent &event)
{
	event.Check(m_pINFO->IsShown() && m_pLayout->GetPane(m_pINFO).IsShown());
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::On_Command_Workspace(wxCommandEvent &event)
{
	m_pWKSP->On_Command(event);
}

void CSAGA_Frame::On_Command_Workspace_UI(wxUpdateUIEvent &event)
{
	m_pWKSP->On_Command_UI(event);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Command_Data(wxCommandEvent &event)
{
	m_pWKSP->Get_Data ()->On_Command(event);
}

void CSAGA_Frame::On_Command_Data_UI(wxUpdateUIEvent &event)
{
	m_pWKSP->Get_Data ()->On_Command_UI(event);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Command_Tool(wxCommandEvent &event)
{
	m_pWKSP->Get_Tools()->On_Command(event);
}

void CSAGA_Frame::On_Command_Tool_UI(wxUpdateUIEvent &event)
{
	m_pWKSP->Get_Tools()->On_Command_UI(event);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Command_Child(wxCommandEvent &event)
{
	MDI_ChildFrame *pChild = GetActiveChild();

	if( pChild )
	{
		pChild->GetEventHandler()->AddPendingEvent(event);
	}
}

void CSAGA_Frame::On_Command_Child_UI(wxUpdateUIEvent &event)
{
	CVIEW_Base *pChild = wxDynamicCast(GetActiveChild(), CVIEW_Base);

	if( pChild )
	{
		pChild->On_Command_UI(event);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::Show_Tips(bool bShow)
{
	wxFileName FileName(SG_UI_Get_Application_Path(true).c_str(), "saga_gui", "tip");

#ifdef SHARE_PATH
	if( !FileName.FileExists() )
	{
		FileName.Assign(SHARE_PATH, "saga_tip", "txt");
	}
#endif

	if( FileName.FileExists() )
	{
		bool bAtStart; bAtStart = CONFIG_Read("/TIPS", "ATSTART", bAtStart) ? bAtStart : true;

		if( bShow || bAtStart )
		{
			long Tip = CONFIG_Read("/TIPS", "CURRENT", Tip) ? Tip : 0;

			wxTipProvider *pTips = wxCreateFileTipProvider(FileName.GetFullPath(), Tip);

			CONFIG_Write("/TIPS", "ATSTART", wxShowTip(this, pTips, bAtStart));
			CONFIG_Write("/TIPS", "CURRENT", (long)pTips->GetCurrentTip());

			delete(pTips);
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSAGA_Frame::Process_Get_Okay(bool bBlink)
{
	if( bBlink )
	{
		#ifdef _SAGA_MSW
		if( !m_pProgressBar->IsShown() )
		{
			m_pProgressBar->Show();
		}
		#endif

		m_pProgressBar->Pulse();
	}

	return( g_pSAGA->Process_Get_Okay() );
}

//---------------------------------------------------------
bool CSAGA_Frame::Process_Set_Okay(bool bOkay)
{
	StatusBar_Set_Text(_TL("ready"));

	#ifdef _SAGA_MSW
	if( wxSystemSettings::GetAppearance().IsDark() )
	{
		m_pProgressBar->Hide();
	}
	#endif

	m_pProgressBar->SetValue(0);

	g_pSAGA->Process_Set_Okay(bOkay);

	return( bOkay );
}

//---------------------------------------------------------
bool CSAGA_Frame::ProgressBar_Set_Position(double Position, double Range)
{
	if( !m_pProgressBar->IsShown() )
	{
		m_pProgressBar->Show();
	}

	int Value = Range > 0. ? (int)(0.5 + 100. * Position / Range) : 0;

	if( Value < 0 ) { Value = 0; } else if( Value > 100 ) { Value = 100; }

	if( m_pProgressBar->GetValue() != Value )
	{
		m_pProgressBar->SetValue(Value);
	//	m_pProgressBar->SetLabel(wxString::Format("%d%%", Value));
	}

	return( g_pSAGA->Process_Get_Okay() );
}

//---------------------------------------------------------
void CSAGA_Frame::StatusBar_Set_Text(const wxString &Text, int iPane)
{
	if( iPane < 0 || iPane >= STATUSBAR_PROGRESS )
	{
		iPane = STATUSBAR_DEFAULT;
	}

	SetStatusText(Text, iPane);
}

//---------------------------------------------------------
void CSAGA_Frame::Set_Project_Name(wxString Project_Name)
{
	if( Project_Name.Length() > 0 )
	{
		SetTitle(wxString::Format("%s [%s]", SAGA_CAPTION, Project_Name.c_str()));
	}
	else
	{
		SetTitle(SAGA_CAPTION);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::Top_Window_Push(wxWindow *pWindow)
{
	if( pWindow )
	{
		for(int i=0; i<m_nTopWindows; i++)
		{
			if( m_pTopWindows[i] == pWindow )
			{
				return;
			}
		}

		m_pTopWindows = (wxWindow **)SG_Realloc(m_pTopWindows, (m_nTopWindows + 1) * sizeof(wxWindow *));
		m_pTopWindows[m_nTopWindows++] = pWindow;
	}
}

//---------------------------------------------------------
void CSAGA_Frame::Top_Window_Pop(wxWindow *pWindow)
{
	if( pWindow )
	{
		int i, j;

		for(i=j=0; j<m_nTopWindows; i++, j++)
		{
			if( m_pTopWindows[i] == pWindow )
			{
				j++;
			}

			if( i < j && j < m_nTopWindows )
			{
				m_pTopWindows[i] = m_pTopWindows[j];
			}
		}

		if( i < j )
		{
			m_nTopWindows--;
			m_pTopWindows = (wxWindow **)SG_Realloc(m_pTopWindows, m_nTopWindows * sizeof(wxWindow *));
		}
	}
}

//---------------------------------------------------------
wxWindow * CSAGA_Frame::Top_Window_Get(void)
{
	return( m_nTopWindows > 0 ? m_pTopWindows[m_nTopWindows - 1] : this );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::Close_Children(void)
{
	if( GetActiveChild() && GetActiveChild()->IsMaximized() )
	{
		GetActiveChild()->Restore();
	}

	while( GetActiveChild() != NULL )
	{
		delete(GetActiveChild());
	}
}
//---------------------------------------------------------
bool CSAGA_Frame::On_Child_Created(CVIEW_Base *pChild)
{
	return( m_Children.Add(pChild) );
}

//---------------------------------------------------------
bool CSAGA_Frame::On_Child_Deleted(CVIEW_Base *pChild)
{
	return( m_Children.Del(pChild) == 1 );
}

//---------------------------------------------------------
void CSAGA_Frame::On_Child_Activates(int View_ID)
{
	if( View_ID < 0 && m_Children.Get_Size() > 0 ) // child view closes, but it's not the last one
	{
		return; // nothing to do, another child will be activated next!
	}

	//-----------------------------------------------------
	wxMenu *pMenu = NULL; wxString Title;

	switch( View_ID )
	{
	case ID_VIEW_TABLE        : pMenu = m_pMN_Table      ; Title = _TL("Table"      ); break;
	case ID_VIEW_TABLE_DIAGRAM: pMenu = m_pMN_Diagram    ; Title = _TL("Diagram"    ); break;
	case ID_VIEW_MAP          : pMenu = m_pMN_Map        ; Title = _TL("Map"        ); break;
	case ID_VIEW_MAP_3D       : pMenu = m_pMN_Map_3D     ; Title = _TL("3D View"    ); break;
	case ID_VIEW_HISTOGRAM    : pMenu = m_pMN_Histogram  ; Title = _TL("Histogram"  ); break;
	case ID_VIEW_SCATTERPLOT  : pMenu = m_pMN_ScatterPlot; Title = _TL("Scatterplot"); break;
	case ID_VIEW_LAYOUT       : pMenu = m_pMN_Layout     ; Title = _TL("Layout"     ); break;
	}

	bool bChanged = false;

	if( pMenu )
	{
		if( GetMenuBar()->GetMenuCount() < 5 )
		{
			bChanged = true; GetMenuBar()->Insert (2, pMenu, Title);
		}
		else if( GetMenuBar()->GetMenu(2) != pMenu )
		{
			bChanged = true; GetMenuBar()->Replace(2, pMenu, Title);
		}
	}
	else if( GetMenuBar()->GetMenuCount() == 5 )
	{
		bChanged = true; GetMenuBar()->Remove(2);
	}

	//-----------------------------------------------------
	if( bChanged )
	{
		_Bar_Show(m_pTB_Main       , true                            );
		_Bar_Show(m_pTB_Table      , View_ID == ID_VIEW_TABLE        );
		_Bar_Show(m_pTB_Diagram    , View_ID == ID_VIEW_TABLE_DIAGRAM);
		_Bar_Show(m_pTB_Map        , View_ID == ID_VIEW_MAP          );
		_Bar_Show(m_pTB_Map_3D     , View_ID == ID_VIEW_MAP_3D       );
		_Bar_Show(m_pTB_Histogram  , View_ID == ID_VIEW_HISTOGRAM    );
		_Bar_Show(m_pTB_ScatterPlot, View_ID == ID_VIEW_SCATTERPLOT  );
		_Bar_Show(m_pTB_Layout     , View_ID == ID_VIEW_LAYOUT       );
	}
}

//---------------------------------------------------------
CVIEW_Base * CSAGA_Frame::Get_Active_Child(int View_ID)
{
	CVIEW_Base *pChild = (CVIEW_Base *)GetActiveChild();

	if( View_ID == -1 )
	{
		return( pChild );
	}

	return( pChild && pChild->Get_ID() == View_ID ? pChild : NULL );
}


///////////////////////////////////////////////////////////
//                                                       //
//						MenuBar							 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenuBar * CSAGA_Frame::_Create_MenuBar(void)
{
	m_pMN_Table       = CVIEW_Table        ::_Create_Menu();
	m_pMN_Diagram     = CVIEW_Table_Diagram::_Create_Menu();
	m_pMN_Map         = CVIEW_Map          ::_Create_Menu();
	m_pMN_Map_3D      = CVIEW_Map_3D       ::_Create_Menu();
	m_pMN_Histogram	  = CVIEW_Histogram    ::_Create_Menu();
	m_pMN_ScatterPlot = CVIEW_ScatterPlot  ::_Create_Menu();
	m_pMN_Layout      = CVIEW_Layout       ::_Create_Menu();

	//-----------------------------------------------------
	wxMenu *pMenu_Window = new wxMenu; // Window...

	CMD_Menu_Add_Item(pMenu_Window,  true, ID_CMD_FRAME_WKSP_SHOW);
	CMD_Menu_Add_Item(pMenu_Window,  true, ID_CMD_FRAME_ACTIVE_SHOW);
	CMD_Menu_Add_Item(pMenu_Window,  true, ID_CMD_FRAME_DATA_SOURCE_SHOW);
	CMD_Menu_Add_Item(pMenu_Window,  true, ID_CMD_FRAME_INFO_SHOW);

#if defined(MDI_TABBED)
	wxMenu *pMenu_Split = new wxMenu;
	CMD_Menu_Add_Item(pMenu_Split, false, ID_CMD_FRAME_SPLIT_LEFT);
	CMD_Menu_Add_Item(pMenu_Split, false, ID_CMD_FRAME_SPLIT_RIGHT);
	CMD_Menu_Add_Item(pMenu_Split, false, ID_CMD_FRAME_SPLIT_TOP);
	CMD_Menu_Add_Item(pMenu_Split, false, ID_CMD_FRAME_SPLIT_BOTTOM);
	pMenu_Split->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Split, false, ID_CMD_FRAME_SPLIT_ALL_HORZ);
	CMD_Menu_Add_Item(pMenu_Split, false, ID_CMD_FRAME_SPLIT_ALL_VERT);

	pMenu_Window->AppendSeparator();
	pMenu_Window->AppendSubMenu(pMenu_Split, _TL("Split"));
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_UNSPLIT);
#elif defined(__WXMSW__)
	pMenu_Window->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_CASCADE);
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_TILE_HORZ);
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_TILE_VERT);
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_ARRANGEICONS);
#endif	// #ifdef __WXMSW__

	pMenu_Window->AppendSeparator();
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_NEXT);
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_PREVIOUS);
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_FIND);
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_CLOSE);
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_CLOSE_ALL);

	//-----------------------------------------------------
	wxMenu *pMenu_Help = new wxMenu; // Help...

	CMD_Menu_Add_Item(pMenu_Help, false, ID_CMD_FRAME_HELP);
	CMD_Menu_Add_Item(pMenu_Help, false, ID_CMD_FRAME_TIPS);
	CMD_Menu_Add_Item(pMenu_Help, false, ID_CMD_FRAME_ABOUT);

	//-----------------------------------------------------
	wxMenuBar *pMenuBar = new wxMenuBar;

#if defined(__WXMAC__)
	wxMenu *OSX_pMenu_Window = pMenu_Window; pMenu_Window = new wxMenu;
#endif

	pMenuBar->Append(g_pData ->Get_Menu_Files()->Get_Menu(), _TL("File"         )); // 0
	pMenuBar->Append(g_pTools->Get_Menu_Tools()            , _TL("Geoprocessing")); // 1
	pMenuBar->Append(pMenu_Window                          , _TL("Window"       )); // 2
	pMenuBar->Append(pMenu_Help                            , _TL("?"            )); // 3

	SetMenuBar(pMenuBar);

#if defined(__WXMAC__)
	pMenuBar->Replace(2, OSX_pMenu_Window                  , _TL("Window"       )); // 2
#endif

	return( pMenuBar );
}


///////////////////////////////////////////////////////////
//                                                       //
//						ToolBar							 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxToolBarBase * CSAGA_Frame::_Create_ToolBar(void)
{
	wxToolBarBase *pToolBar = CMD_ToolBar_Create(ID_TB_MAIN);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DATA_OPEN);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DATA_PROJECT_NEW);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DATA_PROJECT_SAVE);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_FRAME_WKSP_SHOW);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_FRAME_ACTIVE_SHOW);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_FRAME_DATA_SOURCE_SHOW);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_FRAME_INFO_SHOW);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_TOOL_SEARCH);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_FRAME_HELP);

	CMD_ToolBar_Add(pToolBar, _TL("Main"));

	return( pToolBar );
}

//---------------------------------------------------------
void CSAGA_Frame::Show_Toolbar_Data(wxToolBarBase *pToolBar)
{
	if( m_pTB_Data != pToolBar )
	{
		if( m_pTB_Data ) // hide
		{
			_Bar_Show(m_pTB_Data, false);
		}

		m_pTB_Data = pToolBar;

		if( m_pTB_Data )
		{
		//	_Bar_Show(m_pTB_Data, true);

			wxAuiPaneInfo Pane(m_pLayout->GetPane(m_pTB_Data));

			if( Pane.IsOk() )
			{
				Pane.Show(true);

				if( Pane.IsToolbar() && Pane.IsDocked() )
				{
					Pane.Position(2);
				}

				m_pLayout->GetPane(m_pTB_Data) = Pane;

				m_pLayout->GetManager(m_pTB_Data)->Update();
			}
		}
	}
}

//---------------------------------------------------------
void CSAGA_Frame::Add_Toolbar(wxToolBarBase *pToolBar, const wxString &Name)
{
	pToolBar->Realize();

	m_pLayout->AddPane(pToolBar, wxAuiPaneInfo()
		.Name         (wxString::Format("TOOLBAR%d", pToolBar->GetId()))
		.Caption      (Name)
		.ToolbarPane  ()
		.Top          ()
		.LeftDockable (false)
		.RightDockable(false)
		.Hide         ()
		.BestSize     (pToolBar->GetBestSize())
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::Set_Pane_Caption(wxWindow *pWindow, const wxString &Caption)
{
	if( m_pLayout && pWindow )
	{
		m_pLayout->GetPane(pWindow).Caption(Caption);
		m_pLayout->Update();
	}
}

//---------------------------------------------------------
void CSAGA_Frame::_Bar_Add(wxWindow *pWindow, int Position, int Row)
{
	wxAuiPaneInfo Pane;

	Pane.Name        (wxString::Format("PANE_%d", pWindow->GetId()));
	Pane.Caption     (pWindow->GetName());
	Pane.MinSize     (100, 100);
	Pane.BestSize    (400, 400);
	Pane.FloatingSize(400, 400);
	Pane.Position    (0);
	Pane.Layer       (Row);
	Pane.Row         (Row);

	switch( Position )
	{
	default: Pane.Bottom(); break;
	case  1: Pane.Right (); break;
	case  2: Pane.Left  (); break;
	case  3: Pane.Top   (); break;
	case  4: Pane.Center(); break;
	}

	m_pLayout->AddPane(pWindow, Pane);
}

//---------------------------------------------------------
void CSAGA_Frame::_Bar_Toggle(wxWindow *pWindow)
{
	if( m_pLayout->GetPane(pWindow).IsOk() )
	{
		_Bar_Show(pWindow, !m_pLayout->GetPane(pWindow).IsShown());
	}
}

//---------------------------------------------------------
void CSAGA_Frame::_Bar_Show(wxWindow *pWindow, bool bShow)
{
	wxAuiPaneInfo Pane(m_pLayout->GetPane(pWindow));

	if( Pane.IsOk() && Pane.IsShown() != bShow )
	{
		Pane.Show(bShow);

		if( bShow && Pane.IsToolbar() && Pane.IsDocked() )
		{
			Pane.Position(pWindow == m_pTB_Main ? 0 : 1);
		}

		m_pLayout->GetPane(pWindow)	= Pane;

		m_pLayout->GetManager(pWindow)->Update();
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
