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
	EVT_MENU					(ID_CMD_TABLE_FIELD_ADD			, CVIEW_Table::On_Field_Add)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_ADD			, CVIEW_Table::On_Field_Add_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_DEL			, CVIEW_Table::On_Field_Del)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_DEL			, CVIEW_Table::On_Field_Del_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_SORT		, CVIEW_Table::On_Field_Sort)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_SORT		, CVIEW_Table::On_Field_Sort_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_RENAME		, CVIEW_Table::On_Field_Rename)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_RENAME		, CVIEW_Table::On_Field_Rename_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_TYPE		, CVIEW_Table::On_Field_Type)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_TYPE		, CVIEW_Table::On_Field_Type_UI)

	EVT_MENU					(ID_CMD_TABLE_RECORD_ADD		, CVIEW_Table::On_Record_Add)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_ADD		, CVIEW_Table::On_Record_Add_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_INS		, CVIEW_Table::On_Record_Ins)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_INS		, CVIEW_Table::On_Record_Ins_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_DEL		, CVIEW_Table::On_Record_Del)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_DEL		, CVIEW_Table::On_Record_Del_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_DEL_ALL	, CVIEW_Table::On_Record_Clr)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_DEL_ALL	, CVIEW_Table::On_Record_Clr_UI)
	EVT_MENU					(ID_CMD_TABLE_SELECTION_TO_TOP	, CVIEW_Table::On_Sel_To_Top)
	EVT_UPDATE_UI				(ID_CMD_TABLE_SELECTION_TO_TOP	, CVIEW_Table::On_Sel_To_Top_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Table::CVIEW_Table(CWKSP_Table *pTable)
	: CVIEW_Base(ID_VIEW_TABLE, pTable->Get_Name(), ID_IMG_WND_TABLE)
{
	m_pTable	= pTable;

	m_pControl	= new CVIEW_Table_Control(this, m_pTable->Get_Table(), 0);
}

//---------------------------------------------------------
CVIEW_Table::~CVIEW_Table(void)
{
	m_pTable->View_Closes(this);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table::On_Field_Add(wxCommandEvent &event)
{
	m_pControl->On_Field_Add(event);
}

void CVIEW_Table::On_Field_Add_UI(wxUpdateUIEvent &event)
{
	m_pControl->On_Field_Add_UI(event);
}

//---------------------------------------------------------
void CVIEW_Table::On_Field_Del(wxCommandEvent &event)
{
	m_pControl->On_Field_Del(event);
}

void CVIEW_Table::On_Field_Del_UI(wxUpdateUIEvent &event)
{
	m_pControl->On_Field_Del_UI(event);
}

//---------------------------------------------------------
void CVIEW_Table::On_Field_Sort(wxCommandEvent &event)
{
	m_pControl->On_Field_Sort(event);
}

void CVIEW_Table::On_Field_Sort_UI(wxUpdateUIEvent &event)
{
	m_pControl->On_Field_Sort_UI(event);
}

//---------------------------------------------------------
void CVIEW_Table::On_Field_Rename(wxCommandEvent &event)
{
	m_pControl->On_Field_Rename(event);
}

void CVIEW_Table::On_Field_Rename_UI(wxUpdateUIEvent &event)
{
	m_pControl->On_Field_Rename_UI(event);
}

//---------------------------------------------------------
void CVIEW_Table::On_Field_Type(wxCommandEvent &event)
{
	m_pControl->On_Field_Type(event);
}

void CVIEW_Table::On_Field_Type_UI(wxUpdateUIEvent &event)
{
	m_pControl->On_Field_Type_UI(event);
}

//---------------------------------------------------------
void CVIEW_Table::On_Record_Add(wxCommandEvent &event)
{
	m_pControl->On_Record_Add(event);
}

void CVIEW_Table::On_Record_Add_UI(wxUpdateUIEvent &event)
{
	m_pControl->On_Record_Add_UI(event);
}

//---------------------------------------------------------
void CVIEW_Table::On_Record_Ins(wxCommandEvent &event)
{
	m_pControl->On_Record_Ins(event);
}

void CVIEW_Table::On_Record_Ins_UI(wxUpdateUIEvent &event)
{
	m_pControl->On_Record_Ins_UI(event);
}

//---------------------------------------------------------
void CVIEW_Table::On_Record_Del(wxCommandEvent &event)
{
	m_pControl->On_Record_Del(event);
}

void CVIEW_Table::On_Record_Del_UI(wxUpdateUIEvent &event)
{
	m_pControl->On_Record_Del_UI(event);
}

//---------------------------------------------------------
void CVIEW_Table::On_Record_Clr(wxCommandEvent &event)
{
	m_pControl->On_Record_Clr(event);
}

void CVIEW_Table::On_Record_Clr_UI(wxUpdateUIEvent &event)
{
	m_pControl->On_Record_Clr_UI(event);
}

//---------------------------------------------------------
void CVIEW_Table::On_Sel_To_Top(wxCommandEvent &event)
{
	m_pControl->On_Sel_To_Top(event);
}

void CVIEW_Table::On_Sel_To_Top_UI(wxUpdateUIEvent &event)
{
	m_pControl->On_Sel_To_Top_UI(event);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table::Update_Table(void)
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
