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
#include "res_dialogs.h"

#include "wksp.h"

#include "data_source_files.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CData_Source_Files, wxGenericDirCtrl)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CData_Source_Files, wxGenericDirCtrl)
	//EVT_LEFT_DCLICK		(CData_Source_Files::On_DblClick)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CData_Source_Files::CData_Source_Files(wxWindow *pParent)
	: wxGenericDirCtrl(pParent, -1, wxDirDialogDefaultFolderStr, wxDefaultPosition, wxDefaultSize, wxDIRCTRL_SHOW_FILTERS|wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER, DLG_Get_FILE_Filter(ID_DLG_ALLFILES_OPEN))
{
	Connect(
		GetTreeCtrl()->GetId(),
		wxEVT_COMMAND_TREE_ITEM_ACTIVATED, 
		wxTreeEventHandler(CData_Source_Files::On_DblClick)
	);
}

//---------------------------------------------------------
CData_Source_Files::~CData_Source_Files(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source_Files::On_DblClick(wxTreeEvent &event)
{
	g_pWKSP->Open(GetFilePath());

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
