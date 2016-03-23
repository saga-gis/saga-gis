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
//                    VIEW_Table.cpp                     //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include <wx/window.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"

#include "wksp_table.h"

#include "view_table.h"
#include "view_table_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Table, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Table, CVIEW_Base)
	EVT_SIZE			(CVIEW_Table::On_Size)

	EVT_MENU_RANGE		(ID_CMD_TABLE_FIRST, ID_CMD_TABLE_LAST, CVIEW_Table::On_Command)
	EVT_UPDATE_UI_RANGE	(ID_CMD_TABLE_FIRST, ID_CMD_TABLE_LAST, CVIEW_Table::On_Command_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Table::CVIEW_Table(CWKSP_Table *pTable)
	: CVIEW_Base(pTable, ID_VIEW_TABLE, pTable->Get_Name(), ID_IMG_WND_TABLE, false)
{
	m_pControl	= new CVIEW_Table_Control(this, pTable->Get_Table(), 0);
	m_pControl->SetSize(GetClientSize());

	Do_Show();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Table::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_FIELD_ADD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_FIELD_DEL);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_RECORD_ADD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_RECORD_INS);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_RECORD_DEL);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_RECORD_DEL_ALL);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Table::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_TABLE);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_TABLE_FIELD_ADD);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_TABLE_FIELD_DEL);
	CMD_ToolBar_Add_Separator(pToolBar);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_TABLE_RECORD_ADD);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_TABLE_RECORD_INS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_TABLE_RECORD_DEL);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_TABLE_RECORD_DEL_ALL);

	CMD_ToolBar_Add(pToolBar, _TL("Table"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table::On_Size(wxSizeEvent &event)
{
	m_pControl->SetSize(GetClientSize());
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table::Do_Update(void)
{
	m_pControl->Update_Table();
}

//---------------------------------------------------------
void CVIEW_Table::Update_Selection(void)
{
	m_pControl->Update_Selection();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table::On_Command(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_TABLE_FIELD_ADD     :
	case ID_CMD_TABLE_FIELD_DEL     :
	case ID_CMD_TABLE_RECORD_ADD    :
	case ID_CMD_TABLE_RECORD_INS    :
	case ID_CMD_TABLE_RECORD_DEL    :
	case ID_CMD_TABLE_RECORD_DEL_ALL:
		m_pControl->ProcessWindowEvent(event);
	}
}

//---------------------------------------------------------
void CVIEW_Table::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_TABLE_FIELD_ADD     :
	case ID_CMD_TABLE_FIELD_DEL     :
	case ID_CMD_TABLE_RECORD_ADD    :
	case ID_CMD_TABLE_RECORD_INS    :
	case ID_CMD_TABLE_RECORD_DEL    :
	case ID_CMD_TABLE_RECORD_DEL_ALL:
		m_pControl->ProcessWindowEvent(event);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
