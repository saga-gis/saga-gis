/**********************************************************
 * Version $Id: Data_Source.cpp 911 2011-02-14 16:38:15Z reklov_w $
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
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// MicroCity is distributed in the hope that it will be  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wx/menu.h"

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
BEGIN_EVENT_TABLE(CData_Source_Files, wxGenericDirCtrl)
	EVT_MENU			(ID_CMD_DATASOURCE_REFRESH	, CData_Source_Files::On_Refresh)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CData_Source_Files::CData_Source_Files(wxWindow *pParent)
	: wxGenericDirCtrl(pParent) //, wxID_ANY, wxDirDialogDefaultFolderStr, wxDefaultPosition, wxDefaultSize, wxDIRCTRL_SHOW_FILTERS|wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER, DLG_Get_FILE_Filter(ID_DLG_ALLFILES_OPEN))
{
	SetFilter(DLG_Get_FILE_Filter(ID_DLG_ALLFILES_OPEN));
	SetFilterIndex(0);
	GetTreeCtrl      ()->SetId(wxID_ANY);

	Connect(GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_ITEM_EXPANDING , wxTreeEventHandler (wxGenericDirCtrl::OnExpandItem));
	Connect(GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_ITEM_COLLAPSING, wxTreeEventHandler (wxGenericDirCtrl::OnCollapseItem));

	Connect(GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_ITEM_ACTIVATED , wxTreeEventHandler (CData_Source_Files::On_TreeEvent));
	Connect(GetTreeCtrl()->GetId(), wxEVT_COMMAND_TREE_ITEM_MENU      , wxTreeEventHandler (CData_Source_Files::On_TreeEvent));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_Files::On_TreeEvent(wxTreeEvent &event)
{
	//-----------------------------------------------------
	if     ( event.GetEventType() == wxEVT_COMMAND_TREE_ITEM_ACTIVATED )
	{
		g_pWKSP->Open(GetFilePath());
	}

	//-----------------------------------------------------
	else if( event.GetEventType() == wxEVT_COMMAND_TREE_ITEM_MENU )
	{
		wxMenu	Menu;

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_DATASOURCE_REFRESH);

		PopupMenu(&Menu);

		return;
	}

	//-----------------------------------------------------
	event.Skip();
}

//---------------------------------------------------------
void CData_Source_Files::On_Refresh(wxCommandEvent &WXUNUSED(event))
{
	ReCreateTree();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
