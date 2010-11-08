
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
//                       WKSP.cpp                        //
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
#include <wx/image.h>
#include <wx/imaglist.h>

#include <saga_api/saga_api.h>

#include "helper.h"

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"
#include "res_dialogs.h"

#include "active.h"

#include "wksp.h"

#include "wksp_module_control.h"
#include "wksp_module_manager.h"

#include "wksp_data_control.h"
#include "wksp_data_manager.h"
#include "wksp_data_layers.h"

#include "wksp_map_control.h"
#include "wksp_map_manager.h"
#include "wksp_map_buttons.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	IMG_MODULES	= 0,
	IMG_DATA,
	IMG_MAPS
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SUBNB_CAPTION_TREE		LNG("Tree")
#define SUBNB_CAPTION_BUTTONS	LNG("Thumbnails")

//---------------------------------------------------------
#define SUBNB_CREATE(ID, Name)	pNotebook	= new wxNotebook(this, ID, wxDefaultPosition, wxDefaultSize, NOTEBOOK_STYLE|wxNB_MULTILINE, Name);\
								pNotebook	->AssignImageList(new wxImageList(IMG_SIZE_NOTEBOOK, IMG_SIZE_NOTEBOOK, true, 0));\
								pNotebook	->IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_WKSP_TREEVIEW);\
								pNotebook	->IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_WKSP_THUMBNAILS);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP	*g_pWKSP	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP, wxNotebook)
	EVT_NOTEBOOK_PAGE_CHANGING	(ID_WND_WKSP, CWKSP::On_Page_Changing)
	EVT_NOTEBOOK_PAGE_CHANGED	(ID_WND_WKSP, CWKSP::On_Page_Changed)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP::CWKSP(wxWindow *pParent)
	: wxNotebook(pParent, ID_WND_WKSP, wxDefaultPosition, wxDefaultSize, NOTEBOOK_STYLE|wxNB_MULTILINE, LNG("[CAP] Workspace"))
{
	wxNotebook	*pNotebook;

	//-----------------------------------------------------
	g_pWKSP		= this;

	//-----------------------------------------------------
	AssignImageList(new wxImageList(IMG_SIZE_NOTEBOOK, IMG_SIZE_NOTEBOOK, true, 0));

	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_WKSP_MODULES);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_WKSP_DATA);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_WKSP_MAPS);

	//-----------------------------------------------------
	m_pModules		= new CWKSP_Module_Control	(this);

	SUBNB_CREATE(ID_WND_WKSP_DATA, LNG("[CAP] Data"));
	m_pData			= new CWKSP_Data_Control	(pNotebook);
	m_pData_Buttons	= new CWKSP_Data_Buttons	(pNotebook);

	SUBNB_CREATE(ID_WND_WKSP_MAPS, LNG("[CAP] Maps"));
	m_pMaps			= new CWKSP_Map_Control		(pNotebook);
	m_pMaps_Buttons	= new CWKSP_Map_Buttons		(pNotebook);
}

//---------------------------------------------------------
void CWKSP::Add_Pages(void)
{
	long	lValue;

	AddPage(m_pModules				, LNG("[CAP] Modules")	, false, IMG_MODULES);
	AddPage(m_pData->GetParent()	, LNG("[CAP] Data")		, false, IMG_DATA);
	AddPage(m_pMaps->GetParent()	, LNG("[CAP] Maps")		, false, IMG_MAPS);

	((wxNotebook *)m_pData->GetParent())->AddPage(m_pData			, SUBNB_CAPTION_TREE	, false, 0);
	((wxNotebook *)m_pData->GetParent())->AddPage(m_pData_Buttons	, SUBNB_CAPTION_BUTTONS	, false, 1);

	if( CONFIG_Read(wxT("/BUTTONS_DATA"), wxT("TAB"), lValue) )
		((wxNotebook *)m_pData->GetParent())->SetSelection((size_t)lValue);

	((wxNotebook *)m_pMaps->GetParent())->AddPage(m_pMaps			, SUBNB_CAPTION_TREE	, false, 0);
	((wxNotebook *)m_pMaps->GetParent())->AddPage(m_pMaps_Buttons	, SUBNB_CAPTION_BUTTONS	, false, 1);

	if( CONFIG_Read(wxT("/BUTTONS_MAPS"), wxT("TAB"), lValue) )
		((wxNotebook *)m_pMaps->GetParent())->SetSelection((size_t)lValue);
}

//---------------------------------------------------------
CWKSP::~CWKSP(void)
{
	CONFIG_Write(wxT("/BUTTONS_DATA"), wxT("TAB"), (long)((wxNotebook *)m_pData->GetParent())->GetSelection());
	CONFIG_Write(wxT("/BUTTONS_MAPS"), wxT("TAB"), (long)((wxNotebook *)m_pMaps->GetParent())->GetSelection());

	g_pWKSP		= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP::On_Page_Changing(wxNotebookEvent &event)
{
	event.Skip();
}

//---------------------------------------------------------
void CWKSP::On_Page_Changed(wxNotebookEvent &event)
{
	event.Skip();

	if( event.GetSelection() >= 0 && g_pACTIVE )
	{
		CWKSP_Base_Item	*pItem	= NULL;

		if( GetPage(event.GetSelection()) == m_pModules )
		{
			pItem	= m_pModules->Get_Item_Selected();
		}
		else if( GetPage(event.GetSelection()) == m_pData->GetParent() )
		{
			pItem	= m_pData->Get_Item_Selected();
		}
		else if( GetPage(event.GetSelection()) == m_pMaps->GetParent() )
		{
			pItem	= m_pMaps->Get_Item_Selected();
		}

		if( pItem )
		{
			g_pACTIVE->Set_Active(pItem);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP::On_Command(wxCommandEvent &event)
{
	if(	(event.GetId() >= ID_CMD_TABLES_RECENT_FIRST       && event.GetId() <= ID_CMD_TABLES_RECENT_LAST)
	||	(event.GetId() >= ID_CMD_SHAPES_RECENT_FIRST       && event.GetId() <= ID_CMD_SHAPES_RECENT_LAST)
	||	(event.GetId() >= ID_CMD_POINTCLOUD_RECENT_FIRST   && event.GetId() <= ID_CMD_POINTCLOUD_RECENT_LAST)
	||	(event.GetId() >= ID_CMD_TIN_RECENT_FIRST          && event.GetId() <= ID_CMD_TIN_RECENT_LAST)
	||	(event.GetId() >= ID_CMD_GRIDS_RECENT_FIRST        && event.GetId() <= ID_CMD_GRIDS_RECENT_LAST)
	||	(event.GetId() >= ID_CMD_DATA_PROJECT_RECENT_FIRST && event.GetId() <= ID_CMD_DATA_PROJECT_RECENT_LAST) )
	{
		m_pData   ->On_Command(event);
	}
	else switch( event.GetId() )
	{
	default:
		if( !g_pACTIVE->Get_Item() || !g_pACTIVE->Get_Item()->On_Command(event.GetId()) )
		{
			if( GetCurrentPage() )
			{
				switch( GetCurrentPage()->GetId() )
				{
				case ID_WND_WKSP_MODULES:	m_pModules->On_Command(event);	break;
				case ID_WND_WKSP_DATA:		m_pData   ->On_Command(event);	break;
				case ID_WND_WKSP_MAPS:		m_pMaps   ->On_Command(event);	break;
				}
			}
		}
		break;

	case ID_CMD_WKSP_ITEM_CLOSE:
		if( GetCurrentPage() )
		{
			switch( GetCurrentPage()->GetId() )
			{
			case ID_WND_WKSP_MODULES:	m_pModules->On_Command(event);	break;
			case ID_WND_WKSP_DATA:		m_pData   ->On_Command(event);	break;
			case ID_WND_WKSP_MAPS:		m_pMaps   ->On_Command(event);	break;
			}
		}
		break;

	case ID_CMD_MODULES_OPEN:
		m_pModules->On_Command(event);
		break;

	case ID_CMD_DATA_PROJECT_NEW:
	case ID_CMD_DATA_PROJECT_OPEN:
	case ID_CMD_DATA_PROJECT_OPEN_ADD:
	case ID_CMD_DATA_PROJECT_SAVE:
	case ID_CMD_DATA_PROJECT_SAVE_AS:
	case ID_CMD_TABLES_OPEN:
	case ID_CMD_SHAPES_OPEN:
	case ID_CMD_TIN_OPEN:
	case ID_CMD_POINTCLOUD_OPEN:
	case ID_CMD_GRIDS_OPEN:
		m_pData   ->On_Command(event);
		break;

	case ID_CMD_WKSP_OPEN:
		Open();
		break;
	}
}

void CWKSP::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		if( !g_pACTIVE->Get_Item() || !g_pACTIVE->Get_Item()->On_Command_UI(event) )
		{
			m_pModules->On_Command_UI(event);
			m_pData   ->On_Command_UI(event);
			m_pMaps   ->On_Command_UI(event);
		}
		break;

	case ID_CMD_WKSP_ITEM_CLOSE:
		if( GetCurrentPage() )
		{
			switch( GetCurrentPage()->GetId() )
			{
			case ID_WND_WKSP_MODULES:	m_pModules->On_Command_UI(event);	break;
			case ID_WND_WKSP_DATA:		m_pData   ->On_Command_UI(event);	break;
			case ID_WND_WKSP_MAPS:		m_pMaps   ->On_Command_UI(event);	break;
			}
		}
		break;

	case ID_CMD_WKSP_OPEN:
		break;
	}
}

//---------------------------------------------------------
void CWKSP::On_Command_Module(wxCommandEvent &event)
{
	m_pModules->On_Execute(event);
}

void CWKSP::On_Command_UI_Module(wxUpdateUIEvent &event)
{
	m_pModules->On_Execute_UI(event);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP::Open(void)
{
	wxArrayString	File_Paths;

	if( DLG_Open(File_Paths, ID_DLG_WKSP_OPEN) )
	{
		MSG_General_Add_Line();

		for(size_t i=0; i<File_Paths.GetCount(); i++)
		{
			Open(File_Paths[i]);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP::Open(const wxChar *File_Name)
{
	return(	m_pModules	->Get_Manager()->Open(File_Name)
		||	m_pData		->Get_Manager()->Open(File_Name)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
