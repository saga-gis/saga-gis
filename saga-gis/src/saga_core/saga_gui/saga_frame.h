
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
//                     SAGA_Frame.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__SAGA_Frame_H
#define _HEADER_INCLUDED__SAGA_GUI__SAGA_Frame_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if !defined(_SAGA_MSW)
#define MDI_TABBED
#endif

#if defined(MDI_TABBED)
#include <wx/aui/tabmdi.h>
#define MDI_ParentFrame	wxAuiMDIParentFrame
#else
#include <wx/mdi.h>
#define MDI_ParentFrame	wxMDIParentFrame
#endif

//---------------------------------------------------------
class CSAGA_Frame_Layout;


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSAGA_Frame : public MDI_ParentFrame
{
public:
	CSAGA_Frame(void);
	virtual ~CSAGA_Frame(void);

	void						Show_Tips					(bool bShow);

	bool						Process_Get_Okay			(bool bBlink);
	bool						Process_Set_Okay			(bool bOkay = true);

	bool						ProgressBar_Set_Position	(double Position, double Range);

	void						Set_Project_Name			(wxString Project_Name = wxEmptyString);

	void						Top_Window_Push				(wxWindow *pWindow);
	void						Top_Window_Pop				(wxWindow *pWindow);
	wxWindow *					Top_Window_Get				(void);

	void						StatusBar_Set_Text			(const wxString &Text, int iPane = 0);

	virtual wxStatusBar *		OnCreateStatusBar			(int number, long style, wxWindowID id, const wxString& name);

	virtual void				Tile						(wxOrientation orient = wxHORIZONTAL);
	void						Split						(int Arrange);

	bool						Arrange_Children			(int Arrange);
	void						Close_Children				(void);

	bool						On_Child_Created			(class CVIEW_Base *pChild);
	bool						On_Child_Deleted			(class CVIEW_Base *pChild);
	void						On_Child_Activates			(int View_ID);

	class CVIEW_Base *			Get_Active_Child			(int View_ID = -1);

	void						Set_Pane_Caption			(wxWindow *pWindow, const wxString &Caption);

	void						Add_Toolbar					(class wxToolBarBase *pToolBar, const wxString &Name);

	void						Show_Toolbar_Data			(class wxToolBarBase *pToolBar);


private: //------------------------------------------------

	int							m_nTopWindows;

	CSG_Array_Pointer			m_Children;

	wxWindow					**m_pTopWindows;

	class wxGauge				*m_pProgressBar;

	class wxMenu				*m_pMN_Table, *m_pMN_Diagram, *m_pMN_Map, *m_pMN_Map_3D, *m_pMN_Histogram, *m_pMN_ScatterPlot, *m_pMN_Layout;

	class wxToolBarBase			*m_pTB_Data, *m_pTB_Child, *m_pTB_Table, *m_pTB_Diagram, *m_pTB_Map, *m_pTB_Map_3D, *m_pTB_Histogram, *m_pTB_ScatterPlot, *m_pTB_Layout, *m_pTB_Main;

	class CINFO					*m_pINFO;

	class CData_Source			*m_pData_Source;

	class CActive				*m_pActive;

	class CWKSP					*m_pWKSP;

	class wxAuiManager			*m_pLayout;


	//-----------------------------------------------------
	class wxMenuBar *			_Create_MenuBar				(void);
	class wxToolBarBase *		_Create_ToolBar				(void);

	void						_ToolBars_Arrange			(void);

	void						_Bar_Add					(wxWindow *pWindow, int Position, int Row);
	void						_Bar_Toggle					(wxWindow *pWindow);
	void						_Bar_Show					(wxWindow *pWindow, bool bShow, bool bUpdate);

	//-----------------------------------------------------
	void						On_Close					(wxCloseEvent    &event);
	void						On_Size						(wxSizeEvent     &event);

	void						On_Quit						(wxCommandEvent  &event);
	void						On_Help						(wxCommandEvent  &event);
	void						On_About					(wxCommandEvent  &event);
	void						On_Tips						(wxCommandEvent  &event);

	void						On_Frame_Cascade			(wxCommandEvent  &event);
	void						On_Frame_Cascade_UI			(wxUpdateUIEvent &event);
	void						On_Frame_hTile				(wxCommandEvent  &event);
	void						On_Frame_hTile_UI			(wxUpdateUIEvent &event);
	void						On_Frame_vTile				(wxCommandEvent  &event);
	void						On_Frame_vTile_UI			(wxUpdateUIEvent &event);
	void						On_Frame_ArrangeIcons		(wxCommandEvent  &event);
	void						On_Frame_ArrangeIcons_UI	(wxUpdateUIEvent &event);
	void						On_Frame_Unsplit			(wxCommandEvent  &event);
	void						On_Frame_Unsplit_UI			(wxUpdateUIEvent &event);
	void						On_Frame_Split				(wxCommandEvent  &event);
	void						On_Frame_Split_UI			(wxUpdateUIEvent &event);
	void						On_Frame_Find				(wxCommandEvent  &event);
	void						On_Frame_Find_UI			(wxUpdateUIEvent &event);
	void						On_Frame_Next				(wxCommandEvent  &event);
	void						On_Frame_Next_UI			(wxUpdateUIEvent &event);
	void						On_Frame_Previous			(wxCommandEvent  &event);
	void						On_Frame_Previous_UI		(wxUpdateUIEvent &event);
	void						On_Frame_Close				(wxCommandEvent  &event);
	void						On_Frame_Close_UI			(wxUpdateUIEvent &event);
	void						On_Frame_Close_All			(wxCommandEvent  &event);
	void						On_Frame_Close_All_UI		(wxUpdateUIEvent &event);

	void						On_WKSP_Show				(wxCommandEvent  &event);
	void						On_WKSP_Show_UI				(wxUpdateUIEvent &event);
	void						On_Active_Show				(wxCommandEvent  &event);
	void						On_Active_Show_UI			(wxUpdateUIEvent &event);
	void						On_Data_Source_Show			(wxCommandEvent  &event);
	void						On_Data_Source_Show_UI		(wxUpdateUIEvent &event);
	void						On_INFO_Show				(wxCommandEvent  &event);
	void						On_INFO_Show_UI				(wxUpdateUIEvent &event);

	void						On_Command_Workspace		(wxCommandEvent  &event);
	void						On_Command_Workspace_UI		(wxUpdateUIEvent &event);
	void						On_Command_Data				(wxCommandEvent  &event);
	void						On_Command_Data_UI			(wxUpdateUIEvent &event);
	void						On_Command_Tool				(wxCommandEvent  &event);
	void						On_Command_Tool_UI			(wxUpdateUIEvent &event);

	void						On_Command_Child			(wxCommandEvent  &event);
	void						On_Command_Child_UI			(wxUpdateUIEvent &event);

	void						On_Menu_Highlight_All		(wxMenuEvent     &event);


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
extern CSAGA_Frame				*g_pSAGA_Frame;


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__SAGA_Frame_H
