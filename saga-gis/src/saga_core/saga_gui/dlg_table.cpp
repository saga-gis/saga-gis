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
#include <saga_api/saga_api.h>

#include "res_controls.h"
#include "res_dialogs.h"

#include "helper.h"

#include "dlg_table.h"
#include "dlg_table_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_Table, CDLG_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Table, CDLG_Base)
	EVT_KEY_DOWN		(CDLG_Table::On_Key)

	EVT_BUTTON			(ID_BTN_LOAD			, CDLG_Table::On_Load)
	EVT_BUTTON			(ID_BTN_SAVE			, CDLG_Table::On_Save)
	EVT_BUTTON			(ID_BTN_ADD				, CDLG_Table::On_Add)
	EVT_BUTTON			(ID_BTN_TABLE_FROM_WKSP	, CDLG_Table::On_WKSP_Load)
	EVT_BUTTON			(ID_BTN_TABLE_TO_WKSP	, CDLG_Table::On_WKSP_Save)
	EVT_BUTTON			(ID_BTN_INSERT			, CDLG_Table::On_Insert)
	EVT_BUTTON			(ID_BTN_DELETE			, CDLG_Table::On_Delete)
	EVT_BUTTON			(ID_BTN_DELETE_ALL		, CDLG_Table::On_Delete_All)
	EVT_BUTTON			(ID_BTN_COLORS			, CDLG_Table::On_Colors)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Table::CDLG_Table(CSG_Table *pTable, wxString Caption)
	: CDLG_Base(-1, Caption)
{
	m_pTable	= pTable;
	m_pControl	= new CDLG_Table_Control(this, m_pTable);

	Add_Button(ID_BTN_LOAD);
	Add_Button(ID_BTN_TABLE_FROM_WKSP);
	Add_Button(-1);
	Add_Button(ID_BTN_SAVE);
	Add_Button(ID_BTN_TABLE_TO_WKSP);
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


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table::On_Key(wxKeyEvent &event)
{
	if( event.GetKeyCode() == WXK_RETURN )
	{
		if( m_pControl->IsCellEditControlShown() )
		{
			m_pControl->SaveEditControlValue();

			return;
		}
	}

	event.Skip();
}

//---------------------------------------------------------
void CDLG_Table::Set_Position(wxRect r)
{
	m_pControl->SetSize(r);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table::Save_Changes(void)
{
	CSG_Table	*pTable	= m_pControl->Get_Table();

	m_pTable->Del_Records();

	for(int i=0; i<pTable->Get_Count(); i++)
	{
		m_pTable->Add_Record(pTable->Get_Record_byIndex(i));	// apply any sorting
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table::On_Load(wxCommandEvent &event)
{
	if( m_pControl->Load() )
	{
		Refresh();
	}
}

//---------------------------------------------------------
void CDLG_Table::On_Save(wxCommandEvent &event)
{
	if( m_pControl->Save() )
	{
		Refresh();
	}
}

//---------------------------------------------------------
void CDLG_Table::On_WKSP_Load(wxCommandEvent &event)
{
	CSG_Data_Collection	*pTables	= SG_Get_Data_Manager().Get_Table();

	if( pTables )
	{
		wxArrayString	Names;
		CSG_Table		Index;

		Index.Add_Field("INDEX", SG_DATATYPE_Int);

		for(size_t i=0; i<pTables->Count(); i++)
		{
			CSG_Table	*pTable	= (CSG_Table *)pTables->Get(i);

			if( pTable->is_Compatible(m_pTable) && pTable->Get_Count() > 0 )
			{
				Names.Add(pTable->Get_Name());

				Index.Add_Record()->Set_Value(0, i);
			}
		}

		if( Names.Count() > 0 )
		{
			wxSingleChoiceDialog	dlg(MDI_Get_Top_Window(), _TL("Tables"), _TL("Load from Workspace"), Names);

			if( dlg.ShowModal() == wxID_OK )
			{
				m_pControl->Get_Table()->Assign_Values((CSG_Table *)pTables->Get(Index[dlg.GetSelection()].asInt(0)));
				m_pControl->Update_Table();

				Refresh();
			}

			return;
		}
	}

	DLG_Message_Show(_TL("No compatible table has been found."), CTRL_Get_Name(ID_BTN_TABLE_FROM_WKSP));
}

//---------------------------------------------------------
void CDLG_Table::On_WKSP_Save(wxCommandEvent &event)
{
	wxTextEntryDialog	dlg(MDI_Get_Top_Window(), _TL("Table Name"), _TL("Save to Workspace"), _TL("Table"));

	if( dlg.ShowModal() == wxID_OK && !dlg.GetValue().IsEmpty() )
	{
		CSG_Table	*pTable	= SG_Create_Table(*m_pControl->Get_Table());

		wxString	Name(dlg.GetValue());

		pTable->Set_Name(&Name);

		SG_Get_Data_Manager().Add(pTable);
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
	if( m_pControl->Get_Table()->Get_Count() > 0 )
	{
		int		i;

		CSG_Table	*pTable	= m_pControl->Get_Table();

		CSG_Colors	Colors(pTable->Get_Count());

		for(i=0; i<pTable->Get_Count(); i++)
		{
			Colors[i]	= pTable->Get_Record(i)->asInt(0);
		}

		if( DLG_Colors(&Colors) )
		{
			Colors.Set_Count(pTable->Get_Count());

			for(i=0; i<pTable->Get_Count(); i++)
			{
				pTable->Get_Record(i)->Set_Value(0, Colors[i]);
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
