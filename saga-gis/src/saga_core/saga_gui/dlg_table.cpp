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
//                    DLG_Table.cpp                      //
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
#include <saga_api/saga_api.h>

#include "res_controls.h"
#include "res_dialogs.h"

#include "helper.h"

#include "dlg_table.h"
#include "view_table_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_Table, CDLG_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Table, CDLG_Base)
	EVT_BUTTON			(ID_BTN_LOAD			, CDLG_Table::On_Load)
	EVT_BUTTON			(ID_BTN_SAVE			, CDLG_Table::On_Save)
	EVT_BUTTON			(ID_BTN_ADD				, CDLG_Table::On_Add)
	EVT_BUTTON			(ID_BTN_INSERT			, CDLG_Table::On_Insert)
	EVT_BUTTON			(ID_BTN_DELETE			, CDLG_Table::On_Delete)
	EVT_BUTTON			(ID_BTN_DELETE_ALL		, CDLG_Table::On_Delete_All)
	EVT_BUTTON			(ID_BTN_COLORS			, CDLG_Table::On_Colors)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Table::CDLG_Table(CSG_Table *pTable, wxString Caption)
	: CDLG_Base(-1, Caption)
{
	m_pTable_Orig	= pTable;
	m_pTable		= new CSG_Table(*m_pTable_Orig);
	m_pControl		= new CVIEW_Table_Control(this, m_pTable, TABLE_CTRL_FIXED_COLS);

	Add_Button(ID_BTN_LOAD);
	Add_Button(ID_BTN_SAVE);
	Add_Button(-1);
	Add_Button(ID_BTN_ADD);
	Add_Button(ID_BTN_INSERT);
	Add_Button(ID_BTN_DELETE);
	Add_Button(ID_BTN_DELETE_ALL);

	if( pTable->Get_Field_Count() > 0 && pTable->Get_Field_Type(0) == SG_DATATYPE_Color )
	{
		Add_Button(-1);
		Add_Button(ID_BTN_COLORS);
	}

	Set_Positions();
}

//---------------------------------------------------------
CDLG_Table::~CDLG_Table(void)
{
	delete(m_pTable);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table::Set_Position(wxRect r)
{
	m_pControl->SetSize(r);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table::Save_Changes(void)
{
	m_pTable_Orig->Assign(m_pTable);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table::On_Load(wxCommandEvent &event)
{
	wxString	File_Path;

	if( DLG_Open(File_Path, ID_DLG_TABLES_OPEN) )
	{
		m_pControl->Load(File_Path);

		Refresh();
	}
}

//---------------------------------------------------------
void CDLG_Table::On_Save(wxCommandEvent &event)
{
	wxString	File_Path;

	if( DLG_Save(File_Path, ID_DLG_TABLES_SAVE) )
	{
		m_pControl->Save(File_Path);

		Refresh();
	}
}

//---------------------------------------------------------
void CDLG_Table::On_Add(wxCommandEvent &event)
{
	m_pControl->Add_Record();
}

//---------------------------------------------------------
void CDLG_Table::On_Insert(wxCommandEvent &event)
{
	m_pControl->Ins_Record();
}

//---------------------------------------------------------
void CDLG_Table::On_Delete(wxCommandEvent &event)
{
	m_pControl->Del_Record();
}

//---------------------------------------------------------
void CDLG_Table::On_Delete_All(wxCommandEvent &event)
{
	m_pControl->Del_Records();
}

//---------------------------------------------------------
void CDLG_Table::On_Colors(wxCommandEvent &event)
{
	if( m_pTable->Get_Count() > 0 )
	{
		int		i;

		CSG_Colors	Colors(m_pTable->Get_Count());

		for(i=0; i<m_pTable->Get_Count(); i++)
		{
			Colors[i]	= m_pTable->Get_Record(i)->asInt(0);
		}

		if( DLG_Colors(&Colors) )
		{
			Colors.Set_Count(m_pTable->Get_Count());

			for(i=0; i<m_pTable->Get_Count(); i++)
			{
				m_pTable->Get_Record(i)->Set_Value(0, Colors[i]);
			}

			m_pControl->Update_Table();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
