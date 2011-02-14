
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__SAGA_Frame_H
#define _HEADER_INCLUDED__SAGA_GUI__SAGA_Frame_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/mdi.h>

//---------------------------------------------------------
class CSAGA_Frame_Layout;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSAGA_Frame : public wxMDIParentFrame
{
public:

	CSAGA_Frame(void);
	virtual ~CSAGA_Frame(void);

	//-----------------------------------------------------
	void						On_Close					(wxCloseEvent    &event);
	void						On_Size						(wxSizeEvent     &event);

	//-----------------------------------------------------
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
	void						On_INFO_Show				(wxCommandEvent  &event);
	void						On_INFO_Show_UI				(wxUpdateUIEvent &event);

	//-----------------------------------------------------
	void						On_Command_Workspace		(wxCommandEvent  &event);
	void						On_Command_Workspace_UI		(wxUpdateUIEvent &event);
	void						On_Command_Module			(wxCommandEvent  &event);
	void						On_Command_Module_UI		(wxUpdateUIEvent &event);

	void						On_Command_Child			(wxCommandEvent  &event);
	void						On_Command_Child_UI			(wxUpdateUIEvent &event);

	//-----------------------------------------------------
	void						Show_Tips					(bool bShow);

	//-----------------------------------------------------
	bool						Process_Get_Okay			(bool bBlink);
	bool						Process_Set_Okay			(bool bOkay = true);

	bool						ProgressBar_Set_Position	(int Position);
	bool						ProgressBar_Set_Position	(double Position, double Range);

	virtual wxStatusBar *		OnCreateStatusBar			(int number, long style, wxWindowID id, const wxString& name);

	void						StatusBar_Set_Text			(const wxChar *Text, int iPane = 0);

	void						Set_Project_Name			(wxString Project_Name = wxEmptyString);

	void						Top_Window_Push				(wxWindow *pWindow);
	void						Top_Window_Pop				(wxWindow *pWindow);
	wxWindow *					Top_Window_Get				(void);

	//-----------------------------------------------------
	void						On_Child_Activates			(class CVIEW_Base *pChild, bool bActivates);

	void						Set_Pane_Caption			(wxWindow *pWindow, wxString Caption);

	wxMenuBar *					MB_Create					(class CVIEW_Base *pChild);
	void						MB_Remove					(wxMenu *pMenu_File, wxMenu *pMenu_Modules);

	class wxToolBarBase *		TB_Create					(int ID);
	void						TB_Add						(class wxToolBarBase *pToolBar, const wxChar *Name);
	void						TB_Add_Item					(class wxToolBarBase *pToolBar, bool bCheck, int Cmd_ID);
	void						TB_Add_Separator			(class wxToolBarBase *pToolBar);


private:

	int							m_nTopWindows;

	wxWindow					**m_pTopWindows;

	class wxGauge				*m_pProgressBar;

	class wxToolBarBase			*m_pTB_Main, *m_pTB_Table, *m_pTB_Diagram, *m_pTB_Map, *m_pTB_Map_3D, *m_pTB_Histogram, *m_pTB_ScatterPlot, *m_pTB_Layout;

	class CINFO					*m_pINFO;

	class CWKSP					*m_pWKSP;

	class CACTIVE				*m_pActive;

	class wxAuiManager			*m_pLayout;


	void						_Bar_Add					(class wxWindow *pWindow, int Position);
	void						_Bar_Toggle					(wxWindow *pWindow);
	void						_Bar_Show					(wxWindow *pWindow, bool bShow);

	class wxToolBarBase *		_Create_ToolBar				(void);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
extern CSAGA_Frame				*g_pSAGA_Frame;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__SAGA_Frame_H
