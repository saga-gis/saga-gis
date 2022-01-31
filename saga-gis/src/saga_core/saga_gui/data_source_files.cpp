
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
//                 data_source_files.cpp                 //
//                                                       //
//           Copyright (C) 2010 by Sun Zhuo              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'MicroCity: Spatial Analysis and //
// Simulation Framework'. MicroCity is free software;you //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// MicroCity is distributed in the hope that it will be  //
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
//    contact:    Sun Zhuo                               //
//                Centre for Maritime Studies            //
//                National University of Singapore       //
//                12 Prince George's Park                //
//                Singapore                              //
//                118411                                 //
//                                                       //
//    e-mail:     mixwind@gmail.com                      //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wx/menu.h"

#include "res_controls.h"
#include "res_commands.h"
#include "res_dialogs.h"

#include "wksp.h"

#include "data_source_files.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CData_Source_Files, wxPanel)
//	EVT_DIRCTRL_FILEACTIVATED(ID_WND_DATA_SOURCE_FILES, CData_Source_Files::On_TreeEvent)

	EVT_CHOICE(ID_WND_DATA_SOURCE_FILES, CData_Source_Files::On_Change_Filter)

	EVT_MENU(ID_CMD_DATASOURCE_REFRESH, CData_Source_Files::On_Refresh)
	EVT_MENU(ID_CMD_DATASOURCE_OPEN   , CData_Source_Files::On_Open   )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CData_Source_Files::CData_Source_Files(wxWindow *pParent)
	: wxPanel(pParent)
{
	m_pControl	= new wxGenericDirCtrl(this, wxID_ANY, wxDirDialogDefaultFolderStr, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

	m_pControl->GetTreeCtrl()->SetId(ID_WND_DATA_SOURCE_FILES);

	m_pControl->Connect(m_pControl->GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_ITEM_EXPANDING , wxTreeEventHandler(wxGenericDirCtrl::OnExpandItem  ));
	m_pControl->Connect(m_pControl->GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_ITEM_COLLAPSING, wxTreeEventHandler(wxGenericDirCtrl::OnCollapseItem));

	Connect(m_pControl->GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(CData_Source_Files::On_TreeEvent));
	Connect(m_pControl->GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_ITEM_MENU     , wxTreeEventHandler(CData_Source_Files::On_TreeEvent));

	//-----------------------------------------------------
	wxString Filter(DLG_Get_FILE_Filter(ID_DLG_FILE_OPEN));

	wxArrayString Choices;

	while( Filter.Length() > 0 )
	{
		Choices.Add(Filter.BeforeFirst('|'));

		Filter	= Filter.AfterFirst('|').AfterFirst('|');
	}

	m_pFilter	= new wxChoice(this, ID_WND_DATA_SOURCE_FILES, wxDefaultPosition, wxDefaultSize, Choices);

	m_pFilter ->Select(0);
	m_pControl->SetFilter(DLG_Get_FILE_Filter(ID_DLG_FILE_OPEN));
	m_pControl->SetFilterIndex(m_pFilter->GetSelection());
	m_pControl->ReCreateTree();

	//-----------------------------------------------------
	wxBoxSizer *Sizer = new wxBoxSizer(wxVERTICAL);

	Sizer->Add(m_pControl, wxSizerFlags(1).Expand());
	Sizer->Add(m_pFilter , wxSizerFlags(0).Expand());

	SetSizerAndFit(Sizer);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_Files::On_TreeEvent(wxTreeEvent &event)
{
	if( event.GetEventType() == wxEVT_COMMAND_TREE_ITEM_ACTIVATED )
	{
		g_pWKSP->Open(m_pControl->GetFilePath());

		event.Skip();
	}

	//-----------------------------------------------------
	if( event.GetEventType() == wxEVT_COMMAND_TREE_ITEM_MENU )
	{
		m_pControl->SetPath(m_pControl->GetPath(event.GetItem()));

		wxMenu Menu;

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DATASOURCE_REFRESH);

		if( wxFileExists(m_pControl->GetPath()) )
		{
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_DATASOURCE_OPEN);
		}

		PopupMenu(&Menu);
	}
}

//---------------------------------------------------------
void CData_Source_Files::On_Refresh(wxCommandEvent &WXUNUSED(event))
{
	m_pControl->ReCreateTree();
}

//---------------------------------------------------------
void CData_Source_Files::On_Open(wxCommandEvent &WXUNUSED(event))
{
	g_pWKSP->Open(m_pControl->GetPath());
}

//---------------------------------------------------------
void CData_Source_Files::On_Change_Filter(wxCommandEvent &WXUNUSED(event))
{
	wxString Path(m_pControl->GetPath());

	m_pControl->SetFilterIndex(m_pFilter->GetSelection());
	m_pControl->ReCreateTree();
	m_pControl->SetPath(Path);
}

//---------------------------------------------------------
void CData_Source_Files::SetPath(const wxString &Path)
{
	m_pControl->SetPath(Path);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
