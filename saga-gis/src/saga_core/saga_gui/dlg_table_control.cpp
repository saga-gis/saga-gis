
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
//                 dlg_table_control.cpp                 //
//                                                       //
//          Copyright (C) 2015 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/window.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_data_manager.h"

#include "view_table_data.h"
#include "dlg_table_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_Table_Control, wxGrid)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Table_Control, wxGrid)
#if (wxMAJOR_VERSION == 3 && wxMINOR_VERSION <= 1 && wxRELEASE_NUMBER < 5)
	EVT_GRID_RANGE_SELECT     (CDLG_Table_Control::On_Selected     ) // RANGE_SELECT was split in RANGE_SELECTING and SELECTED in 3.2
#else
	EVT_GRID_RANGE_SELECTED   (CDLG_Table_Control::On_Selected     )
#endif
	EVT_GRID_CELL_CHANGED     (CDLG_Table_Control::On_Changed      )

	EVT_GRID_CELL_LEFT_CLICK  (CDLG_Table_Control::On_LClick       )
	EVT_GRID_LABEL_LEFT_CLICK (CDLG_Table_Control::On_LClick_Label )
	EVT_GRID_LABEL_LEFT_DCLICK(CDLG_Table_Control::On_LDClick_Label)

	EVT_GRID_CELL_RIGHT_CLICK (CDLG_Table_Control::On_RClick       )
	EVT_GRID_LABEL_RIGHT_CLICK(CDLG_Table_Control::On_RClick_Label )

	EVT_MENU     (ID_CMD_TABLE_AUTOSIZE_COLS  , CDLG_Table_Control::On_Autosize_Cols)
	EVT_MENU     (ID_CMD_TABLE_AUTOSIZE_ROWS  , CDLG_Table_Control::On_Autosize_Rows)

	EVT_MENU     (ID_CMD_TABLE_FIELD_OPEN_APP , CDLG_Table_Control::On_Cell_Open    )
	EVT_MENU     (ID_CMD_TABLE_FIELD_OPEN_DATA, CDLG_Table_Control::On_Cell_Open    )

	EVT_MENU     (ID_CMD_TABLE_RECORD_ADD     , CDLG_Table_Control::On_Record_Add   )
	EVT_UPDATE_UI(ID_CMD_TABLE_RECORD_ADD     , CDLG_Table_Control::On_Record_Add_UI)
	EVT_MENU     (ID_CMD_TABLE_RECORD_INS     , CDLG_Table_Control::On_Record_Ins   )
	EVT_UPDATE_UI(ID_CMD_TABLE_RECORD_INS     , CDLG_Table_Control::On_Record_Ins_UI)
	EVT_MENU     (ID_CMD_TABLE_RECORD_DEL     , CDLG_Table_Control::On_Record_Del   )
	EVT_UPDATE_UI(ID_CMD_TABLE_RECORD_DEL     , CDLG_Table_Control::On_Record_Del_UI)
	EVT_MENU     (ID_CMD_TABLE_RECORD_DEL_ALL , CDLG_Table_Control::On_Record_Clr   )
	EVT_UPDATE_UI(ID_CMD_TABLE_RECORD_DEL_ALL , CDLG_Table_Control::On_Record_Clr_UI)

	EVT_MENU     (ID_CMD_TABLE_FIELD_SORT     , CDLG_Table_Control::On_Field_Sort   )
	EVT_UPDATE_UI(ID_CMD_TABLE_FIELD_SORT     , CDLG_Table_Control::On_Field_Sort_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Table_Control::CDLG_Table_Control(wxWindow *pParent, CSG_Table *pTable)
	: wxGrid(pParent, -1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxSUNKEN_BORDER)
{
	m_Table.Create(*pTable);
	m_pData = new CVIEW_Table_Data(&m_Table);

	if( pTable->Get_MetaData()("SAGA_GUI_LUT_TYPE")
	&&  pTable->Get_Field_Count() == 5
	&&  pTable->Get_Field_Type(0) == SG_DATATYPE_Color
	&&  pTable->Get_Field_Type(1) == SG_DATATYPE_String
	&&  pTable->Get_Field_Type(2) == SG_DATATYPE_String )
	{
		m_LUT_Type	= pTable->Get_MetaData()["SAGA_GUI_LUT_TYPE"].Get_Content().asInt();
	}
	else
	{
		m_LUT_Type	= SG_DATAOBJECT_TYPE_Undefined;
	}

	EnableDragGridSize(false);
	DisableDragRowSize();
	SetRowLabelAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
	SetCellHighlightColour(SYS_Get_Color(wxSYS_COLOUR_HIGHLIGHT));

	wxGridCellRenderer *pRenderer;

	pRenderer = GetDefaultRendererForType(wxGRID_VALUE_FLOAT);
	pRenderer->DecRef();
	pRenderer->SetParameters("-1,-1,g"); // Use the shorter of e or f (g)

#if !(wxMAJOR_VERSION == 3 && wxMINOR_VERSION <= 1 && wxRELEASE_NUMBER < 5)
	pRenderer = GetDefaultRendererForType(wxGRID_VALUE_DATE );
	pRenderer->DecRef();
	pRenderer->SetParameters("%Y-%m-%d");
#endif

	SetTable(m_pData, true, wxGrid::wxGridSelectRows);

	Update_Table();
}

//---------------------------------------------------------
CDLG_Table_Control::~CDLG_Table_Control(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDLG_Table_Control::Update_Table(void)
{
	return( GetBatchCount() == 0 && _Update_Records() );
}

//---------------------------------------------------------
bool CDLG_Table_Control::Update_Selection(void)
{
	return( _Update_Selection() );
}

//---------------------------------------------------------
bool CDLG_Table_Control::_Update_Records(void)
{
	//-----------------------------------------------------
	int	dCols = m_pData->GetNumberCols() - GetNumberCols();

	if( dCols > 0 )
	{
		AppendCols(dCols);
	}
	else if( dCols < 0 )
	{
		DeleteCols(0, -dCols);
	}

	//-----------------------------------------------------
	int	dRows = m_pData->GetNumberRows() - GetNumberRows();

	if( dRows > 0 )
	{
		AppendRows(dRows);
	}
	else if( dRows < 0 )
	{
		DeleteRows(0, -dRows);
	}

	//-----------------------------------------------------
	for(int iField=0; iField<m_Table.Get_Field_Count(); iField++)
	{
		if( m_Table.Get_Field_Type(iField) == SG_DATATYPE_Color )
		{
			BeginBatch();

			for(int i=0; i<m_pData->GetNumberRows(); i++)
			{
				wxColour Colour(Get_Color_asWX(m_pData->Get_Record(i)->asInt(iField)));

				SetCellBackgroundColour(i, iField, Colour);
				SetCellTextColour      (i, iField, Colour);
			}

			EndBatch();
		}
	}

	//-----------------------------------------------------
	_Update_Selection();

	return( true );
}

//---------------------------------------------------------
bool CDLG_Table_Control::_Update_Selection(void)
{
	if( GetBatchCount() > 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	BeginBatch();

	if( m_pData->m_bSelection )
	{
		SelectAll();
	}
	else
	{
		ClearSelection();

		if( m_Table.Get_Selection_Count() > 0 )
		{
			for(int i=0; i<m_pData->GetRowsCount(); i++)
			{
				if( m_pData->Get_Record(i)->is_Selected() )
				{
					SelectRow(i, true);
				}
			}
		}
	}

	EndBatch();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_Selected(wxGridRangeSelectEvent &event)
{
	if( GetBatchCount() == 0 )
	{
		for(int iRow=event.GetTopRow(); iRow<=event.GetBottomRow(); iRow++)
		{
			CSG_Table_Record	*pRecord	= m_Table.Get_Record_byIndex(iRow);

			if( pRecord->is_Selected() != event.Selecting() )
			{
				m_Table.Select(pRecord, true);
			}
		}

		_Update_Selection();
	}

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_Changed(wxGridEvent &event)
{
	m_pData->On_Changed(event.GetRow(), event.GetCol());
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_LClick(wxGridEvent &event)
{
	SetGridCursor(event.GetRow(), event.GetCol());

	//-----------------------------------------------------
	if( event.AltDown    () && m_Table.Get_Field_Type(event.GetCol()) == SG_DATATYPE_String )
	{
		Open_Application(GetCellValue(event.GetRow(), event.GetCol()));
	}

	//-----------------------------------------------------
	if( event.ControlDown() && m_Table.Get_Field_Type(event.GetCol()) == SG_DATATYPE_String )
	{
		g_pData->Open   (GetCellValue(event.GetRow(), event.GetCol()));
	}
}

//---------------------------------------------------------
void CDLG_Table_Control::On_LClick_Label(wxGridEvent &event)
{
	if( event.GetRow() != -1 )	// select records
	{
		if( event.ControlDown() )
		{
			m_Table.Select(m_Table.Get_Record_byIndex(event.GetRow()), true);

			Update_Selection();
		}
		else if( event.ShiftDown() )
		{
			SelectBlock(event.GetRow(), 0, GetGridCursorRow(), GetNumberCols(), false);
		}
		else
		{
			SelectRow(event.GetRow(), false);
		}

		SetGridCursor(event.GetRow(), GetGridCursorCol());
	}
	else if( event.GetCol() < 0 )
	{
		ClearSelection();
	}
}

//---------------------------------------------------------
void CDLG_Table_Control::On_LDClick_Label(wxGridEvent &event)
{
	if( m_pData->Sort(event.GetCol(), -1) )
	{
		_Update_Records();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_RClick(wxGridEvent &event)
{
	SetGridCursor(event.GetRow(), event.GetCol());

	//-----------------------------------------------------
	switch( m_Table.Get_Field_Type(event.GetCol()) )
	{
	default:
		{
			// nop
		}
		break;

	case SG_DATATYPE_String:
		{
			wxMenu	Menu;

			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_OPEN_APP);

			wxString	Value	= GetCellValue(event.GetRow(), event.GetCol());

			if( m_pData->is_DataSource(Value) )
			{
				CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_OPEN_DATA);
			}

			PopupMenu(&Menu, event.GetPosition());
		}
		break;

	case SG_DATATYPE_Color:
		{
			CSG_Table_Record *pRecord = m_pData->Get_Record(event.GetRow()); long lValue;

			if( pRecord && DLG_Color(lValue = pRecord->asInt(event.GetCol())) )
			{
				pRecord->Set_Value(event.GetCol(), lValue);

				wxColour Colour(Get_Color_asWX(pRecord->asInt(event.GetCol())));

				SetCellBackgroundColour(event.GetRow(), event.GetCol(), Colour);
				SetCellTextColour      (event.GetRow(), event.GetCol(), Colour);

				ForceRefresh();
			}
		}
		break;
	}
}

//---------------------------------------------------------
void CDLG_Table_Control::On_RClick_Label(wxGridEvent &event)
{
	//-----------------------------------------------------
	if( event.GetCol() != -1 )
	{
		wxMenu	Menu(_TL("Columns"));

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_AUTOSIZE_COLS);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_SORT);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_RENAME);

		PopupMenu(&Menu, event.GetPosition().x, event.GetPosition().y - GetColLabelSize());
	}

	//-----------------------------------------------------
	else if( event.GetRow() != -1 )
	{
		wxMenu	Menu(_TL("Rows"));

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_ADD);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_INS);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_DEL);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_DEL_ALL);

		Menu.AppendSeparator();
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_AUTOSIZE_ROWS);

		PopupMenu(&Menu, event.GetPosition().x - GetRowLabelSize(), event.GetPosition().y);
	}

	//-----------------------------------------------------
	else
	{
		ClearSelection();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_Autosize_Cols(wxCommandEvent &event)
{
	AutoSizeColumns(false);
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Autosize_Rows(wxCommandEvent &event)
{
	AutoSizeRows(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_Cell_Open(wxCommandEvent &event)
{
	wxString	Value	= GetCellValue(GetGridCursorRow(), GetGridCursorCol());

	if( event.GetId() == ID_CMD_TABLE_FIELD_OPEN_APP )
	{
		if( !Open_Application(Value) )
		{
			DLG_Message_Show_Error(_TL("failed"), CMD_Get_Name(ID_CMD_TABLE_FIELD_OPEN_APP));
		}
	}

	if( event.GetId() == ID_CMD_TABLE_FIELD_OPEN_DATA )
	{
		if( !m_pData->is_DataSource(Value) || !g_pData->Open(Value) )
		{
			DLG_Message_Show_Error(_TL("failed"), CMD_Get_Name(ID_CMD_TABLE_FIELD_OPEN_DATA));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDLG_Table_Control::Add_Record(void)
{
	return( m_Table.Add_Record()
		&& _Update_Records()
	);
}

//---------------------------------------------------------
bool CDLG_Table_Control::Ins_Record(void)
{
	return( m_Table.Ins_Record(GetGridCursorRow())
		&& _Update_Records()
	);
}

//---------------------------------------------------------
bool CDLG_Table_Control::Del_Record(void)
{
	return( m_Table.Del_Record(GetGridCursorRow())
		&& _Update_Records()
	);
}

//---------------------------------------------------------
bool CDLG_Table_Control::Del_Records(void)
{
	return( m_Table.Del_Records()
		&& _Update_Records()
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_Record_Add(wxCommandEvent &event)
{
	if( m_Table.Add_Record() )
	{
		_Update_Records();
	}
}

void CDLG_Table_Control::On_Record_Add_UI(wxUpdateUIEvent &event)
{
	event.Enable(!m_Table.is_Indexed());
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Record_Ins(wxCommandEvent &event)
{
	if( m_Table.Ins_Record(GetGridCursorRow()) )
	{
		_Update_Records();
	}
}

void CDLG_Table_Control::On_Record_Ins_UI(wxUpdateUIEvent &event)
{
	event.Enable(!m_Table.is_Indexed());
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Record_Del(wxCommandEvent &event)
{
	if( m_Table.Del_Selection() )
	{
		_Update_Records();
	}
}

void CDLG_Table_Control::On_Record_Del_UI(wxUpdateUIEvent &event)
{
	event.Enable(!m_Table.is_Indexed() && m_Table.Get_Selection_Count() > 0);
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Record_Clr(wxCommandEvent &event)
{
	if( m_Table.Del_Records() )
	{
		_Update_Records();
	}
}

void CDLG_Table_Control::On_Record_Clr_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_Table.Get_Selection_Count() > 0);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_Field_Sort(wxCommandEvent &event)
{
	if( m_pData->On_Sort() )
	{
		_Update_Records();
	}
}

void CDLG_Table_Control::On_Field_Sort_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_Table.Get_Field_Count() > 0 && m_Table.Get_Record_Count() > 1);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDLG_Table_Control::Load(void)
{
	bool	bResult	= false;

	//-----------------------------------------------------
	wxString	File, Filter;

	Filter	+= wxString::Format("%s (*.txt, *.csv, *.dbf)|*.txt;*.csv;*.dbf|", _TL("Tables"));

	if( m_LUT_Type != SG_DATAOBJECT_TYPE_Undefined )
	{
		Filter	+= wxString::Format("%s (*.qml)|*.qml|", _TL("QGIS Layer Style File"));
	}

	Filter	+= wxString::Format("%s|*.*", _TL("All Files"));

	//-----------------------------------------------------
	if( DLG_Open(File, _TL("Load Table"), Filter) )
	{
		CSG_Table	Table;

		if( m_LUT_Type != SG_DATAOBJECT_TYPE_Undefined && SG_File_Cmp_Extension(&File, "qml") )
		{
			QGIS_Styles_Import(&File, Table);
		}
		else
		{
			Table.Create(&File);
		}

		bResult	= Table.Get_Count() > 0 && Table.Get_Field_Count() == m_Table.Get_Field_Count()
			&& m_Table.Assign_Values(&Table) && Update_Table();

		PROCESS_Set_Okay();
	}

	return( bResult );
}

//---------------------------------------------------------
bool CDLG_Table_Control::Save(void)
{
	bool	bResult	= false;

	//-----------------------------------------------------
	wxString	File, Filter;

	Filter	+= wxString::Format("%s (*.txt, *.csv, *.dbf)|*.txt;*.csv;*.dbf|", _TL("Tables"));

	if( m_LUT_Type == SG_DATAOBJECT_TYPE_Grid )
	{
		Filter	+= wxString::Format("%s (*.qml)|*.qml|", _TL("QGIS Layer Style File"));
	}

	Filter	+= wxString::Format("%s|*.*", _TL("All Files"));

	//-----------------------------------------------------
	if( DLG_Save(File, _TL("Save Table"), Filter) )
	{
		if( m_LUT_Type == SG_DATAOBJECT_TYPE_Grid && SG_File_Cmp_Extension(&File, "qml") )
		{
			bResult	= QGIS_Styles_Export(&File, m_Table);
		}
		else
		{
			bResult	= m_Table.Save(&File);
		}

		PROCESS_Set_Okay();
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
