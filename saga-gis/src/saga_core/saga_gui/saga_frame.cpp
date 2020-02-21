
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
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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

#include "wksp_tool_manager.h"
#include "wksp_tool.h"

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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _DEBUG
	#define SAGA_CAPTION	wxT("SAGA [Debug]")
#else
	#define SAGA_CAPTION	wxT("SAGA")
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSAGA_Frame_StatusBar : public wxStatusBar
{
public:
	CSAGA_Frame_StatusBar(wxWindow *parent, wxWindowID id, long style = wxST_SIZEGRIP, const wxString& name = wxT("statusBar"))
		: wxStatusBar(parent, id, style, name)
	{
		m_pProgressBar	= new wxGauge(this, ID_WND_PROGRESSBAR, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH);
	}

	void		On_Size		(wxSizeEvent &event)
	{
		wxRect	r;

		if( m_pProgressBar && GetFieldRect(STATUSBAR_PROGRESS, r) )
		{
			m_pProgressBar->SetSize(r);
		}
	}

	wxGauge		*m_pProgressBar;

	DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSAGA_Frame_StatusBar, wxStatusBar)
	EVT_SIZE			(CSAGA_Frame_StatusBar::On_Size)
END_EVENT_TABLE()

//---------------------------------------------------------
wxStatusBar * CSAGA_Frame::OnCreateStatusBar(int number, long style, wxWindowID id, const wxString& name)
{
	CSAGA_Frame_StatusBar	*sb	= new CSAGA_Frame_StatusBar(this, id, style, name);

	sb->SetFieldsCount(number);

	return( sb );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSAGA_Frame	*g_pSAGA_Frame	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSAGA_Frame, MDI_ParentFrame)
	EVT_CLOSE			(CSAGA_Frame::On_Close)
	EVT_SIZE			(CSAGA_Frame::On_Size)

	EVT_MENU			(ID_CMD_FRAME_QUIT				, CSAGA_Frame::On_Quit)
	EVT_MENU			(ID_CMD_FRAME_HELP				, CSAGA_Frame::On_Help)
	EVT_MENU			(ID_CMD_FRAME_ABOUT				, CSAGA_Frame::On_About)
	EVT_MENU			(ID_CMD_FRAME_TIPS				, CSAGA_Frame::On_Tips)

	EVT_MENU			(ID_CMD_FRAME_CASCADE			, CSAGA_Frame::On_Frame_Cascade)
	EVT_UPDATE_UI		(ID_CMD_FRAME_CASCADE			, CSAGA_Frame::On_Frame_Cascade_UI)
	EVT_MENU			(ID_CMD_FRAME_TILE_HORZ			, CSAGA_Frame::On_Frame_hTile)
	EVT_UPDATE_UI		(ID_CMD_FRAME_TILE_HORZ			, CSAGA_Frame::On_Frame_hTile_UI)
	EVT_MENU			(ID_CMD_FRAME_TILE_VERT			, CSAGA_Frame::On_Frame_vTile)
	EVT_UPDATE_UI		(ID_CMD_FRAME_TILE_VERT			, CSAGA_Frame::On_Frame_vTile_UI)
	EVT_MENU			(ID_CMD_FRAME_ARRANGEICONS		, CSAGA_Frame::On_Frame_ArrangeIcons)
	EVT_UPDATE_UI		(ID_CMD_FRAME_ARRANGEICONS		, CSAGA_Frame::On_Frame_ArrangeIcons_UI)
	EVT_MENU			(ID_CMD_FRAME_UNSPLIT			, CSAGA_Frame::On_Frame_Unsplit)
	EVT_UPDATE_UI		(ID_CMD_FRAME_UNSPLIT			, CSAGA_Frame::On_Frame_Unsplit_UI)
	EVT_MENU			(ID_CMD_FRAME_SPLIT_LEFT		, CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI		(ID_CMD_FRAME_SPLIT_LEFT		, CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU			(ID_CMD_FRAME_SPLIT_RIGHT		, CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI		(ID_CMD_FRAME_SPLIT_RIGHT		, CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU			(ID_CMD_FRAME_SPLIT_TOP			, CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI		(ID_CMD_FRAME_SPLIT_TOP			, CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU			(ID_CMD_FRAME_SPLIT_BOTTOM		, CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI		(ID_CMD_FRAME_SPLIT_BOTTOM		, CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU			(ID_CMD_FRAME_SPLIT_ALL_HORZ	, CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI		(ID_CMD_FRAME_SPLIT_ALL_HORZ	, CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU			(ID_CMD_FRAME_SPLIT_ALL_VERT	, CSAGA_Frame::On_Frame_Split)
	EVT_UPDATE_UI		(ID_CMD_FRAME_SPLIT_ALL_VERT	, CSAGA_Frame::On_Frame_Split_UI)
	EVT_MENU			(ID_CMD_FRAME_NEXT				, CSAGA_Frame::On_Frame_Next)
	EVT_UPDATE_UI		(ID_CMD_FRAME_NEXT				, CSAGA_Frame::On_Frame_Next_UI)
	EVT_MENU			(ID_CMD_FRAME_PREVIOUS			, CSAGA_Frame::On_Frame_Previous)
	EVT_UPDATE_UI		(ID_CMD_FRAME_PREVIOUS			, CSAGA_Frame::On_Frame_Previous_UI)
	EVT_MENU			(ID_CMD_FRAME_CLOSE				, CSAGA_Frame::On_Frame_Close)
	EVT_UPDATE_UI		(ID_CMD_FRAME_CLOSE				, CSAGA_Frame::On_Frame_Close_UI)
	EVT_MENU			(ID_CMD_FRAME_CLOSE_ALL			, CSAGA_Frame::On_Frame_Close_All)
	EVT_UPDATE_UI		(ID_CMD_FRAME_CLOSE_ALL			, CSAGA_Frame::On_Frame_Close_All_UI)

	EVT_MENU			(ID_CMD_FRAME_WKSP_SHOW			, CSAGA_Frame::On_WKSP_Show)
	EVT_UPDATE_UI		(ID_CMD_FRAME_WKSP_SHOW			, CSAGA_Frame::On_WKSP_Show_UI)
	EVT_MENU			(ID_CMD_FRAME_ACTIVE_SHOW		, CSAGA_Frame::On_Active_Show)
	EVT_UPDATE_UI		(ID_CMD_FRAME_ACTIVE_SHOW		, CSAGA_Frame::On_Active_Show_UI)
	EVT_MENU			(ID_CMD_FRAME_DATA_SOURCE_SHOW	, CSAGA_Frame::On_Data_Source_Show)
	EVT_UPDATE_UI		(ID_CMD_FRAME_DATA_SOURCE_SHOW	, CSAGA_Frame::On_Data_Source_Show_UI)
	EVT_MENU			(ID_CMD_FRAME_INFO_SHOW			, CSAGA_Frame::On_INFO_Show)
	EVT_UPDATE_UI		(ID_CMD_FRAME_INFO_SHOW			, CSAGA_Frame::On_INFO_Show_UI)

	EVT_MENU_RANGE		(ID_CMD_WKSP_FIRST				, ID_CMD_WKSP_LAST		, CSAGA_Frame::On_Command_Workspace)
	EVT_UPDATE_UI_RANGE	(ID_CMD_WKSP_FIRST				, ID_CMD_WKSP_LAST		, CSAGA_Frame::On_Command_Workspace_UI)
	EVT_MENU_RANGE		(ID_CMD_TOOL_MENU_FIRST			, ID_CMD_TOOL_MENU_LAST	, CSAGA_Frame::On_Command_Tool)
	EVT_UPDATE_UI_RANGE	(ID_CMD_TOOL_MENU_FIRST			, ID_CMD_TOOL_MENU_LAST	, CSAGA_Frame::On_Command_Tool_UI)

	EVT_MENU_RANGE		(ID_CMD_CHILD_FIRST				, ID_CMD_CHILD_LAST		, CSAGA_Frame::On_Command_Child)
	EVT_UPDATE_UI_RANGE	(ID_CMD_MAP_FIRST				, ID_CMD_MAP_LAST		, CSAGA_Frame::On_Command_Child_UI)
	EVT_UPDATE_UI_RANGE	(ID_CMD_HISTOGRAM_FIRST			, ID_CMD_HISTOGRAM_LAST	, CSAGA_Frame::On_Command_Child_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	//-----------------------------------------------------
	g_pSAGA_Frame		= this;

	m_nTopWindows		= 0;
	m_pTopWindows		= NULL;

	m_pINFO				= NULL;
	m_pData_Source		= NULL;
	m_pActive			= NULL;
	m_pWKSP				= NULL;

	SG_Set_UI_Callback	(Get_Callback());

	SetIcon				(IMG_Get_Icon(ID_IMG_SAGA_ICON_32));

	SetDropTarget		(new CSAGA_Frame_DropTarget);

	//-----------------------------------------------------
	int		STATUSBAR_Sizes[STATUSBAR_COUNT]	= {	-1, 125, 125, 125, -1	};

	CreateStatusBar		(STATUSBAR_COUNT);
	SetStatusWidths		(STATUSBAR_COUNT, STATUSBAR_Sizes);
	SetStatusBarPane	(STATUSBAR_DEFAULT);
	StatusBar_Set_Text	(_TL("ready"));

	m_pProgressBar		= ((CSAGA_Frame_StatusBar *)GetStatusBar())->m_pProgressBar;

	//-----------------------------------------------------
	m_pLayout			= new wxAuiManager(this);

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
	wxString	s;

	if( CONFIG_Read("/FL", "MANAGER", s) )
	{
		m_pLayout->LoadPerspective(s, false);
	}

	Set_Pane_Caption(m_pINFO       , m_pINFO       ->GetName());	// captions might have been modified by perspective, so update again...
	Set_Pane_Caption(m_pWKSP       , m_pWKSP       ->GetName());
	Set_Pane_Caption(m_pData_Source, m_pData_Source->GetName());
	Set_Pane_Caption(m_pActive     , m_pActive     ->GetName());

	//-----------------------------------------------------
	m_pTB_Main			=                      _Create_ToolBar();
	m_pTB_Map			= CVIEW_Map          ::_Create_ToolBar();
	m_pTB_Map_3D		= CVIEW_Map_3D       ::_Create_ToolBar();
	m_pTB_Layout		= CVIEW_Layout       ::_Create_ToolBar();
	m_pTB_Table			= CVIEW_Table        ::_Create_ToolBar();
	m_pTB_Diagram		= CVIEW_Table_Diagram::_Create_ToolBar();
	m_pTB_Histogram		= CVIEW_Histogram    ::_Create_ToolBar();
	m_pTB_ScatterPlot	= CVIEW_ScatterPlot  ::_Create_ToolBar();

	_Bar_Show(m_pTB_Main, true);

	m_pLayout->Update();

	//-----------------------------------------------------
	long	l;
	wxRect	r, rDefault	= wxGetClientDisplayRect();

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
		r	= rDefault;
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
	//-----------------------------------------------------
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

	//-----------------------------------------------------
	CONFIG_Write("/FL", "MANAGER", m_pLayout->SavePerspective());

	m_pLayout->UnInit();

	delete(m_pLayout);

	//-----------------------------------------------------
	On_Child_Activates(-1);

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
//														 //
//														 //
//														 //
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
			DLG_Message_Show(wxString::Format("[%s]\n%s", g_pTool->Get_Name().c_str(),
				_TL("Please wait until the tool has terminated its execution.")), _TL("Exit SAGA")
			);

			event.Veto();

			return;
		}

		g_pTool->Execute(false);
	}

	if( !g_pTool && DLG_Message_Confirm(ID_DLG_CLOSE) && g_pData->Finalise() && g_pData->Close(true) )
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::On_Quit(wxCommandEvent &WXUNUSED(event))
{
	Close(false);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Help(wxCommandEvent &WXUNUSED(event))
{
	if( !Open_WebBrowser("https://sourceforge.net/p/saga-gis/wiki/Home/") )
	{
	//	DLG_Message_Show(_TL("Online Help"), _TL("SAGA Help"));
	}
}

//---------------------------------------------------------
void CSAGA_Frame::On_About(wxCommandEvent &WXUNUSED(event))
{
	CDLG_About	dlg;

	dlg.ShowModal();
}

//---------------------------------------------------------
void CSAGA_Frame::On_Tips(wxCommandEvent &WXUNUSED(event))
{
	Show_Tips(true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
void CSAGA_Frame::On_Frame_Split(wxCommandEvent &event)
{
#ifdef MDI_TABBED
	int		n	= GetNotebook()->GetPageCount();

	if( n > 1 )
	{
		GetNotebook()->Freeze();

		int	iActive	= GetNotebook()->GetSelection(); if( iActive < 0 ) iActive = 0;
		int	nSqrt	= (int)sqrt((double)n);

		switch( event.GetId() )
		{
		case ID_CMD_FRAME_SPLIT_LEFT  : GetNotebook()->Split(iActive, wxLEFT  ); break;
		case ID_CMD_FRAME_SPLIT_RIGHT : GetNotebook()->Split(iActive, wxRIGHT ); break;
		case ID_CMD_FRAME_SPLIT_TOP   : GetNotebook()->Split(iActive, wxTOP   ); break;
		case ID_CMD_FRAME_SPLIT_BOTTOM: GetNotebook()->Split(iActive, wxBOTTOM); break;

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
	int		n	= _Get_MDI_Children_Count();

	if( n == 1 && GetActiveChild() )
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
//														 //
//														 //
//														 //
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
void CSAGA_Frame::On_Command_Tool(wxCommandEvent &event)
{
	m_pWKSP->On_Command_Tool(event);
}

void CSAGA_Frame::On_Command_Tool_UI(wxUpdateUIEvent &event)
{
	m_pWKSP->On_Command_UI_Tool(event);
}

//---------------------------------------------------------
void CSAGA_Frame::On_Command_Child(wxCommandEvent &event)
{
	MDI_ChildFrame	*pChild;

	if( (pChild = GetActiveChild()) != NULL )
	{
		pChild->GetEventHandler()->AddPendingEvent(event);
	}
}

void CSAGA_Frame::On_Command_Child_UI(wxUpdateUIEvent &event)
{
	CVIEW_Base	*pChild;

	if( (pChild = wxDynamicCast(GetActiveChild(), CVIEW_Base)) != NULL )
	{
		pChild->On_Command_UI(event);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::Show_Tips(bool bShow)
{
	bool			bTip;
	long			iTip;

#ifdef SHARE_PATH
        wxFileName      fTip(wxT(SHARE_PATH), wxT("saga_tip.txt"));
#else
        wxFileName      fTip(g_pSAGA->Get_App_Path(), wxT("saga_gui"), wxT("tip"));
#endif

	wxTipProvider	*pTip;

	bTip	= CONFIG_Read(wxT("/TIPS"), wxT("ATSTART"), bTip) ? bTip : true;

	if( bShow || (bTip && fTip.FileExists()) )
	{
		iTip	= CONFIG_Read(wxT("/TIPS"), wxT("CURRENT"), iTip) ? iTip : 0;
		pTip	= wxCreateFileTipProvider(fTip.GetFullPath(), iTip);

		bTip	= wxShowTip(this, pTip, bTip);
		iTip	= pTip->GetCurrentTip();

		CONFIG_Write(wxT("/TIPS"), wxT("ATSTART"), bTip);
		CONFIG_Write(wxT("/TIPS"), wxT("CURRENT"), iTip);

		delete(pTip);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSAGA_Frame::Process_Get_Okay(bool bBlink)
{
	if( bBlink )
	{
		m_pProgressBar->Pulse();
	}

	return( g_pSAGA->Process_Get_Okay() );
}

//---------------------------------------------------------
bool CSAGA_Frame::Process_Set_Okay(bool bOkay)
{
	StatusBar_Set_Text(_TL("ready"));

	m_pProgressBar->SetValue(0);

	g_pSAGA->Process_Set_Okay(bOkay);

	return( bOkay );
}

//---------------------------------------------------------
bool CSAGA_Frame::ProgressBar_Set_Position(int Position)
{
	if( Position < 0 )
	{
		Position	= 0;
	}
	else if( Position > 100 )
	{
		Position	= 100;
	}

	if( m_pProgressBar->GetValue() != Position )
	{
		m_pProgressBar->SetValue(Position);
	//	m_pProgressBar->SetLabel(wxString::Format("%d%%", Position));
	}

	return( g_pSAGA->Process_Get_Okay() );
}

//---------------------------------------------------------
bool CSAGA_Frame::ProgressBar_Set_Position(double Position, double Range)
{
	return( ProgressBar_Set_Position(Range > 0.0 ? (int)(0.5 + 100.0 * Position / Range) : 0) );
}

//---------------------------------------------------------
void CSAGA_Frame::StatusBar_Set_Text(const wxString &Text, int iPane)
{
	if( iPane < 0 || iPane >= STATUSBAR_PROGRESS )
	{
		iPane	= STATUSBAR_DEFAULT;
	}

	SetStatusText(Text, iPane);
}

//---------------------------------------------------------
void CSAGA_Frame::Set_Project_Name(wxString Project_Name)
{
	if( Project_Name.Length() > 0 )
	{
		SetTitle(wxString::Format(wxT("%s [%s]"), SAGA_CAPTION, Project_Name.c_str()));
	}
	else
	{
		SetTitle(SAGA_CAPTION);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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

		m_pTopWindows	= (wxWindow **)SG_Realloc(m_pTopWindows, (m_nTopWindows + 1) * sizeof(wxWindow *));
		m_pTopWindows[m_nTopWindows++]	= pWindow;
	}
}

//---------------------------------------------------------
void CSAGA_Frame::Top_Window_Pop(wxWindow *pWindow)
{
	if( pWindow )
	{
		int		i, j;

		for(i=j=0; j<m_nTopWindows; i++, j++)
		{
			if( m_pTopWindows[i] == pWindow )
				j++;

			if( i < j && j < m_nTopWindows )
				m_pTopWindows[i]	= m_pTopWindows[j];
		}

		if( i < j )
		{
			m_nTopWindows--;
			m_pTopWindows	= (wxWindow **)SG_Realloc(m_pTopWindows, m_nTopWindows * sizeof(wxWindow *));
		}
	}
}

//---------------------------------------------------------
wxWindow * CSAGA_Frame::Top_Window_Get(void)
{
	return( m_nTopWindows > 0 ? m_pTopWindows[m_nTopWindows - 1] : this );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSAGA_Frame::Close_Children(void)
{
	while( GetActiveChild() != NULL )
	{
		delete(GetActiveChild());
	}
}

//---------------------------------------------------------
int CSAGA_Frame::_Get_MDI_Children_Count(void)
{
#ifdef MDI_TABBED
	return( GetNotebook()->GetPageCount() );
#else
	int		n	= 0;

	for(wxWindowList::const_iterator Child=GetChildren().begin(); Child!=GetChildren().end(); Child++)
	{
		if( wxDynamicCast(*Child, wxMDIChildFrame) )
		{
			n++;
		}
	}

	return( n );
#endif
}

//---------------------------------------------------------
void CSAGA_Frame::On_Child_Activates(int View_ID)
{
#ifdef MDI_TABBED
	if( View_ID < 0 && GetNotebook()->GetPageCount() > 0 )	// child view closes, but it's not the last one
#else
	if( View_ID < 0 && _Get_MDI_Children_Count    () > 1 )	// child view closes, but it's not the last one
#endif
	{
		return;	// nothing to do, another child will be activated next!
	}

	//-----------------------------------------------------
	wxString		Title;
	wxMenu			*pMenu		= NULL;
	wxToolBarBase	*pToolBar	= NULL;

	switch( View_ID )
	{
	case ID_VIEW_TABLE        : pToolBar = m_pTB_Table      ; pMenu	= m_pMN_Table      ; Title = _TL("Table"      ); break;
	case ID_VIEW_TABLE_DIAGRAM: pToolBar = m_pTB_Diagram    ; pMenu	= m_pMN_Diagram    ; Title = _TL("Diagram"    ); break;
	case ID_VIEW_MAP          : pToolBar = m_pTB_Map        ; pMenu	= m_pMN_Map        ; Title = _TL("Map"        ); break;
	case ID_VIEW_MAP_3D       : pToolBar = m_pTB_Map_3D     ; pMenu	= m_pMN_Map_3D     ; Title = _TL("3D View"    ); break;
	case ID_VIEW_HISTOGRAM    : pToolBar = m_pTB_Histogram  ; pMenu	= m_pMN_Histogram  ; Title = _TL("Histogram"  ); break;
	case ID_VIEW_SCATTERPLOT  : pToolBar = m_pTB_ScatterPlot; pMenu	= m_pMN_ScatterPlot; Title = _TL("Scatterplot"); break;
	case ID_VIEW_LAYOUT       : pToolBar = m_pTB_Layout     ; pMenu	= m_pMN_Layout     ; Title = _TL("Layout"     ); break;
	}

	//-----------------------------------------------------
	_Bar_Show(m_pTB_Main       , true                         );
	_Bar_Show(m_pTB_Table      , pToolBar == m_pTB_Table      );
	_Bar_Show(m_pTB_Diagram    , pToolBar == m_pTB_Diagram    );
	_Bar_Show(m_pTB_Map        , pToolBar == m_pTB_Map        );
	_Bar_Show(m_pTB_Map_3D     , pToolBar == m_pTB_Map_3D     );
	_Bar_Show(m_pTB_Histogram  , pToolBar == m_pTB_Histogram  );
	_Bar_Show(m_pTB_ScatterPlot, pToolBar == m_pTB_ScatterPlot);
	_Bar_Show(m_pTB_Layout     , pToolBar == m_pTB_Layout     );

	//-----------------------------------------------------
	wxMenuBar	*pMenuBar	= GetMenuBar();

	if( pMenu )
	{
		if( pMenuBar->GetMenuCount() < 5 )
		{
			pMenuBar->Insert (2, pMenu, Title);
		}
		else if( pMenuBar->GetMenu(2) != pMenu )
		{
			pMenuBar->Replace(2, pMenu, Title);
		}
	}
	else if( pMenuBar->GetMenuCount() == 5 )
	{
		pMenuBar->Remove(2);
	}
}


///////////////////////////////////////////////////////////
//														 //
//						MenuBar							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenuBar * CSAGA_Frame::_Create_MenuBar(void)
{
	//-----------------------------------------------------
	m_pMN_Table			= CVIEW_Table        ::_Create_Menu();
	m_pMN_Diagram		= CVIEW_Table_Diagram::_Create_Menu();
	m_pMN_Map			= CVIEW_Map          ::_Create_Menu();
	m_pMN_Map_3D		= CVIEW_Map_3D       ::_Create_Menu();
	m_pMN_Histogram		= CVIEW_Histogram    ::_Create_Menu();
	m_pMN_ScatterPlot	= CVIEW_ScatterPlot  ::_Create_Menu();
	m_pMN_Layout		= CVIEW_Layout       ::_Create_Menu();

	//-----------------------------------------------------
	wxMenu	*pMenu_Window	= new wxMenu;	// Window...

	CMD_Menu_Add_Item(pMenu_Window,  true, ID_CMD_FRAME_WKSP_SHOW);
	CMD_Menu_Add_Item(pMenu_Window,  true, ID_CMD_FRAME_ACTIVE_SHOW);
	CMD_Menu_Add_Item(pMenu_Window,  true, ID_CMD_FRAME_DATA_SOURCE_SHOW);
	CMD_Menu_Add_Item(pMenu_Window,  true, ID_CMD_FRAME_INFO_SHOW);

#if defined(MDI_TABBED)
	wxMenu	*pMenu_Split	= new wxMenu;
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
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_CLOSE);
	CMD_Menu_Add_Item(pMenu_Window, false, ID_CMD_FRAME_CLOSE_ALL);

	//-----------------------------------------------------
	wxMenu	*pMenu_Help		= new wxMenu;	// Help...

	CMD_Menu_Add_Item(pMenu_Help, false, ID_CMD_FRAME_HELP);
	CMD_Menu_Add_Item(pMenu_Help, false, ID_CMD_FRAME_TIPS);
	CMD_Menu_Add_Item(pMenu_Help, false, ID_CMD_FRAME_ABOUT);

	//-----------------------------------------------------
	wxMenuBar	*pMenuBar	= new wxMenuBar;

	pMenuBar->Append(g_pData ->Get_Menu_Files()->Get_Menu(), _TL("File"         ));	// 0
	pMenuBar->Append(g_pTools->Get_Menu_Tools()            , _TL("Geoprocessing"));	// 1
	pMenuBar->Append(pMenu_Window                          , _TL("Window"       ));	// 2
	pMenuBar->Append(pMenu_Help                            , _TL("?"            ));	// 3

	SetMenuBar(pMenuBar);

	return( pMenuBar );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	wxAuiPaneInfo	Pane;

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
	default:	Pane.Bottom();	break;
	case  1:	Pane.Right ();	break;
	case  2:	Pane.Left  ();	break;
	case  3:	Pane.Top   ();	break;
	case  4:	Pane.Center();	break;
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
	wxAuiPaneInfo	Pane(m_pLayout->GetPane(pWindow));

	if( Pane.IsOk() && Pane.IsShown() != bShow )
	{
		Pane.Show(bShow);

		if( bShow && Pane.IsToolbar() && Pane.IsDocked() )
		{
			Pane.Position(pWindow == m_pTB_Main ? 0 : 1);
		}

		m_pLayout->GetPane(pWindow)	= Pane;

		m_pLayout->Update();
	}
}


///////////////////////////////////////////////////////////
//														 //
//						ToolBar							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxToolBarBase * CSAGA_Frame::TB_Create(int ID)
{
	int	Size	= g_pTools->Get_Parameter("LOOK_TB_SIZE")->asInt();

	wxToolBar	*pToolBar	= new wxToolBar(this, ID, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_FLAT|wxTB_NODIVIDER);

	pToolBar->SetToolBitmapSize(wxSize(Size, Size));

	return( pToolBar );
}

//---------------------------------------------------------
void CSAGA_Frame::TB_Add(wxToolBarBase *pToolBar, const wxString &Name)
{
	pToolBar->Realize();

	m_pLayout->AddPane(pToolBar, wxAuiPaneInfo()
		.Name         (wxString::Format("PANE_%d", pToolBar->GetId()))
		.Caption      (Name)
		.ToolbarPane  ()
		.Top          ()
		.LeftDockable (false)
		.RightDockable(false)
		.Hide         ()
		.BestSize     (pToolBar->GetBestSize())
	);
}

//---------------------------------------------------------
void CSAGA_Frame::TB_Add_Item(wxToolBarBase *pToolBar, bool bCheck, int Cmd_ID)
{
	if( bCheck )
		((wxToolBar *)pToolBar)->AddTool(Cmd_ID, CMD_Get_Name(Cmd_ID), IMG_Get_Bitmap(CMD_Get_ImageID(Cmd_ID), pToolBar->GetToolBitmapSize()), CMD_Get_Help(Cmd_ID), wxITEM_CHECK);
	else
		((wxToolBar *)pToolBar)->AddTool(Cmd_ID, CMD_Get_Name(Cmd_ID), IMG_Get_Bitmap(CMD_Get_ImageID(Cmd_ID), pToolBar->GetToolBitmapSize()), CMD_Get_Help(Cmd_ID));
}

//---------------------------------------------------------
void CSAGA_Frame::TB_Add_Separator(wxToolBarBase *pToolBar)
{
	((wxToolBar *)pToolBar)->AddSeparator();
}

//---------------------------------------------------------
wxToolBarBase * CSAGA_Frame::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= TB_Create(ID_TB_MAIN);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DATA_OPEN);
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

	TB_Add(pToolBar, _TL("Main"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
