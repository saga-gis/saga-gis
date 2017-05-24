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
//                 wksp_tool_control.cpp                 //
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
#include <wx/image.h>
#include <wx/imaglist.h>

#include "res_controls.h"
#include "res_images.h"

#include "helper.h"

#include "wksp_tool_control.h"
#include "wksp_tool_manager.h"
#include "wksp_tool_library.h"
#include "wksp_tool_menu.h"
#include "wksp_tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	IMG_MANAGER		= 1,
	IMG_GROUP,
	IMG_LIBRARY,
	IMG_CHAIN,
	IMG_TOOL
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CWKSP_Tool_Control, CWKSP_Base_Control)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP_Tool_Control, CWKSP_Base_Control)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Tool_Control	*g_pTool_Ctrl	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Tool_Control::CWKSP_Tool_Control(wxWindow *pParent)
	: CWKSP_Base_Control(pParent, ID_WND_WKSP_TOOLS)
{
	g_pTool_Ctrl	= this;

	//-----------------------------------------------------
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TOOL_MANAGER)
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TOOL_GROUP);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TOOL_LIBRARY);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TOOL_CHAIN);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TOOL);

	//-----------------------------------------------------
	_Set_Manager(new CWKSP_Tool_Manager);

	Get_Manager()->Initialise();
}

//---------------------------------------------------------
CWKSP_Tool_Control::~CWKSP_Tool_Control(void)
{
//	Get_Manager()->Finalise();

	g_pTool_Ctrl	= NULL;

	_Del_Item(m_pManager, true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool_Control::On_Execute(wxCommandEvent &event)
{
	Get_Manager()->On_Execute(event);
}

//---------------------------------------------------------
void CWKSP_Tool_Control::On_Execute_UI(wxUpdateUIEvent &event)
{
	Get_Manager()->On_Execute_UI(event);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool_Control::Add_Group(CWKSP_Tool_Group *pGroup)
{
	_Add_Item(pGroup, IMG_GROUP, IMG_GROUP);
}

//---------------------------------------------------------
void CWKSP_Tool_Control::Add_Library(const wxTreeItemId &Group, CWKSP_Tool_Library *pLibrary)
{
	if( pLibrary != NULL )
	{
		wxString	Name	= pLibrary->Get_Name();

		if( Name.IsEmpty() )
		{
			Name	= pLibrary->Get_Name();
		}
		else
		{
			Name.Trim(false);
		}

		AppendItem(Group, Name, IMG_LIBRARY, IMG_LIBRARY, pLibrary);

		for(int i=0; i<pLibrary->Get_Count(); i++)
		{
			Add_Tool(pLibrary->GetId(), pLibrary->Get_Tool(i));
		}

		SortChildren(pLibrary->GetId());
	}

	SortChildren(Group);
}

//---------------------------------------------------------
void CWKSP_Tool_Control::Add_Tool(const wxTreeItemId &Library, CWKSP_Tool *pTool)
{
	if( pTool != NULL )
	{
		AppendItem(Library, pTool->Get_Name(), IMG_TOOL, IMG_TOOL, pTool);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
