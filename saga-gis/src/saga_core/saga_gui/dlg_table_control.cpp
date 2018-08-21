/**********************************************************
 * Version $Id: dlg_table_control.cpp 2665 2015-10-28 12:55:25Z oconrad $
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/window.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_data_manager.h"

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
	EVT_SIZE					(CDLG_Table_Control::On_Size)
	EVT_KEY_DOWN				(CDLG_Table_Control::On_Key)

	EVT_GRID_EDITOR_SHOWN		(CDLG_Table_Control::On_Edit_Start)
	EVT_GRID_EDITOR_HIDDEN		(CDLG_Table_Control::On_Edit_Stop)
	EVT_GRID_CELL_CHANGED		(CDLG_Table_Control::On_Changed)
	EVT_GRID_CELL_LEFT_CLICK	(CDLG_Table_Control::On_LClick)
	EVT_GRID_CELL_RIGHT_CLICK	(CDLG_Table_Control::On_RClick)
	EVT_GRID_LABEL_LEFT_CLICK	(CDLG_Table_Control::On_LClick_Label)
	EVT_GRID_LABEL_LEFT_DCLICK	(CDLG_Table_Control::On_LDClick_Label)
	EVT_GRID_LABEL_RIGHT_CLICK	(CDLG_Table_Control::On_RClick_Label)
	EVT_GRID_RANGE_SELECT		(CDLG_Table_Control::On_Select)

	EVT_MENU					(ID_CMD_TABLE_FIELD_SORT		, CDLG_Table_Control::On_Field_Sort)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_SORT		, CDLG_Table_Control::On_Field_Sort_UI)

	EVT_MENU					(ID_CMD_TABLE_FIELD_OPEN_APP	, CDLG_Table_Control::On_Field_Open)
	EVT_MENU					(ID_CMD_TABLE_FIELD_OPEN_DATA	, CDLG_Table_Control::On_Field_Open)

	EVT_MENU					(ID_CMD_TABLE_RECORD_ADD		, CDLG_Table_Control::On_Record_Add)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_ADD		, CDLG_Table_Control::On_Record_Add_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_INS		, CDLG_Table_Control::On_Record_Ins)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_INS		, CDLG_Table_Control::On_Record_Ins_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_DEL		, CDLG_Table_Control::On_Record_Del)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_DEL		, CDLG_Table_Control::On_Record_Del_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_DEL_ALL	, CDLG_Table_Control::On_Record_Clr)

	EVT_MENU					(ID_CMD_TABLE_AUTOSIZE_COLS		, CDLG_Table_Control::On_Autosize_Cols)
	EVT_MENU					(ID_CMD_TABLE_AUTOSIZE_ROWS		, CDLG_Table_Control::On_Autosize_Rows)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Table_Control::CDLG_Table_Control(wxWindow *pParent, CSG_Table *pTable)
	: wxGrid(pParent, -1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxSUNKEN_BORDER)
{
	m_pTable	= new CSG_Table(*pTable);
	m_bEditing	= false;

	m_bLUT		= pTable->Get_Field_Count() == 5
		&& pTable->Get_Field_Type(0) == SG_DATATYPE_Color
		&& pTable->Get_Field_Type(1) == SG_DATATYPE_String
		&& pTable->Get_Field_Type(2) == SG_DATATYPE_String;

	CreateGrid(0, m_pTable->Get_Field_Count(), wxGrid::wxGridSelectRows);

	Update_Table();
}

//---------------------------------------------------------
CDLG_Table_Control::~CDLG_Table_Control(void)
{
	delete(m_pTable);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDLG_Table_Control::Update_Table(void)
{
	if( GetBatchCount() > 0 )
	{
		return( false );
	}

	BeginBatch();

	//-----------------------------------------------------
	int	Difference	= (m_pTable->Get_Field_Count()) - GetNumberCols();

	if( Difference > 0 )
	{
		AppendCols(Difference);
	}
	else if( (Difference = -Difference < GetNumberCols() ? -Difference : GetNumberCols()) > 0 )
	{	// here is (or was!?) a memory leak - solution: use own wxGridTableBase derived grid table class
		DeleteCols(0, Difference);
	}

	//-----------------------------------------------------
	for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
	{
		SetColLabelValue(iField, m_pTable->Get_Field_Name(iField));

		switch( m_pTable->Get_Field_Type(iField) )
		{
		default:
		case SG_DATATYPE_Byte:
		case SG_DATATYPE_Char:
		case SG_DATATYPE_String:
		case SG_DATATYPE_Date:
		case SG_DATATYPE_Binary:
			SetColFormatCustom(iField, wxGRID_VALUE_STRING);
			break;

		case SG_DATATYPE_Bit:
		case SG_DATATYPE_Word:
		case SG_DATATYPE_Short:
		case SG_DATATYPE_DWord:
		case SG_DATATYPE_Int:
		case SG_DATATYPE_ULong:
		case SG_DATATYPE_Long:
		case SG_DATATYPE_Color:
			SetColFormatNumber(iField);
			break;

		case SG_DATATYPE_Float:
		case SG_DATATYPE_Double:
			SetColFormatFloat(iField);
			break;
		}
	}

	//-----------------------------------------------------
	EndBatch();

	return( _Set_Records() );
}

//---------------------------------------------------------
bool CDLG_Table_Control::_Set_Records(void)
{
	BeginBatch();

	ClearSelection();

	//-----------------------------------------------------
	int	Difference, nRecords	= m_pTable->Get_Count();

	if( (Difference = nRecords - GetNumberRows()) > 0 )
	{
		AppendRows(Difference);
	}
	else if( Difference < 0 && (Difference = -Difference < GetNumberRows() ? -Difference : GetNumberRows()) > 0 )
	{
		DeleteRows(0, Difference);
	}

	//-----------------------------------------------------
//	#pragma omp parallel for
	for(int iRecord=0; iRecord<nRecords; iRecord++)
	{
		_Set_Record(iRecord, m_pTable->Get_Record_byIndex(iRecord));
	}

	//-----------------------------------------------------
	EndBatch();

	return( true );
}

//---------------------------------------------------------
bool CDLG_Table_Control::_Set_Record(int iRecord, CSG_Table_Record *pRecord)
{
	SetRowLabelValue(iRecord, wxString::Format("%d", 1 + iRecord));

	for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
	{
		switch( m_pTable->Get_Field_Type(iField) )
		{
		default:
			SetCellValue(iRecord, iField, pRecord->is_NoData(iField) ? SG_T("") : pRecord->asString(iField));
			break;

		case SG_DATATYPE_Color:
			SetCellBackgroundColour(iRecord, iField, Get_Color_asWX(pRecord->asInt(iField)));
			break;
		}
	}

	if( pRecord->is_Selected() )
	{
		SelectRow(iRecord, true);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDLG_Table_Control::Update_Sorting(int iField, int Direction)
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		switch( Direction )
		{
		default:	m_pTable->Toggle_Index(iField);	break;
		case  0:	m_pTable->Set_Index(iField, TABLE_INDEX_None      );	break;
		case  1:	m_pTable->Set_Index(iField, TABLE_INDEX_Ascending );	break;
		case  2:	m_pTable->Set_Index(iField, TABLE_INDEX_Descending);	break;
		}

		return( _Set_Records() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDLG_Table_Control::Add_Record(void)
{
	CSG_Table_Record	*pRecord	= m_pTable->Add_Record();

	if( pRecord && AppendRows(1) )
	{
		_Set_Record(GetNumberRows() - 1, pRecord);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CDLG_Table_Control::Ins_Record(void)
{
	int		iRecord	= GetGridCursorRow();

	if( iRecord >= 0 && iRecord < GetNumberRows() )
	{
		InsertRows(iRecord);

		_Set_Record(iRecord, m_pTable->Ins_Record(iRecord));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CDLG_Table_Control::Del_Record(void)
{
	return( m_pTable->Del_Selection() && Update_Table() );
}

//---------------------------------------------------------
bool CDLG_Table_Control::Del_Records(void)
{
	return( m_pTable->Del_Records() && Update_Table() );
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

	if( m_bLUT )
	{
		Filter	+= wxString::Format("%s (*.qml)|*.qml|", _TL("QGIS Layer Style File"));
	}

	Filter	+= wxString::Format("%s|*.*|", _TL("All Files"));

	//-----------------------------------------------------
	if( DLG_Open(File, _TL("Load Table"), Filter) )
	{
		CSG_Table	Table;

		if( m_bLUT && SG_File_Cmp_Extension(&File, "qml") )
		{
			QGIS_Styles_Import(&File, Table);
		}
		else
		{
			Table.Create(&File);
		}

		bResult	= Table.Get_Count() > 0 && Table.Get_Field_Count() == m_pTable->Get_Field_Count()
			&& m_pTable->Assign_Values(&Table) && Update_Table();

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

	if(0&& m_bLUT )
	{
		Filter	+= wxString::Format("%s (*.qml)|*.qml|", _TL("QGIS Layer Style File"));
	}

	Filter	+= wxString::Format("%s|*.*|", _TL("All Files"));

	//-----------------------------------------------------
	if( DLG_Save(File, _TL("Save Table"), Filter) )
	{
		if( m_bLUT && SG_File_Cmp_Extension(&File, "qml") )
		{
			bResult	= QGIS_Styles_Export(&File, *m_pTable);
		}
		else
		{
			bResult	= m_pTable->Save(&File);
		}

		PROCESS_Set_Okay();
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_Size(wxSizeEvent &event)//&WXUNUSED(event))
{
	event.Skip();
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Key(wxKeyEvent &event)
{
	event.Skip(true);

	if( event.GetKeyCode() == WXK_RETURN )
	{
		// nop
	}
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Edit_Start(wxGridEvent &event)
{
	m_bEditing	= true;

	event.Skip();
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Edit_Stop(wxGridEvent &event)
{
	m_bEditing	= false;

	event.Skip();
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Changed(wxGridEvent &event)
{
	CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(event.GetRow());

	if( pRecord )
	{
		int	iField	= event.GetCol();

		if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
		{
			pRecord->Set_Value(iField, GetCellValue(event.GetRow(), event.GetCol()).wx_str());

			SetCellValue(event.GetRow(), event.GetCol(), pRecord->asString(iField));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_Field_Sort(wxCommandEvent &event)
{
	//-----------------------------------------------------
	CSG_String	sFields, sOrder;

	for(int i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		sFields	+= m_pTable->Get_Field_Name(i);	sFields	+= '|';
	}

	sOrder.Printf("%s|%s|%s", _TL("unsorted"), _TL("ascending"), _TL("descending") );

	//-----------------------------------------------------
	CSG_Parameters	P(NULL, _TL("Sort Table"), _TL(""));

	P.Add_Choice(""       , "FIELD_1", _TL("Sort first by" ), _TL(""), sFields, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(0));
	P.Add_Choice("FIELD_1", "ORDER_1", _TL("Direction"     ), _TL(""), sOrder , !m_pTable->is_Indexed() ? 1 : m_pTable->Get_Index_Order(0));
	P.Add_Choice(""       , "FIELD_2", _TL("Sort second by"), _TL(""), sFields, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(1));
	P.Add_Choice("FIELD_2", "ORDER_2", _TL("Direction"     ), _TL(""), sOrder , !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Order(1));
	P.Add_Choice(""       , "FIELD_3", _TL("Sort third by" ), _TL(""), sFields, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(2));
	P.Add_Choice("FIELD_3", "ORDER_3", _TL("Direction"     ), _TL(""), sOrder , !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Order(2));

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		m_pTable->Set_Index(
			P("FIELD_1")->asInt(), P("ORDER_1")->asInt() == 1 ? TABLE_INDEX_Ascending : P("ORDER_1")->asInt() == 2 ? TABLE_INDEX_Descending : TABLE_INDEX_None,
			P("FIELD_2")->asInt(), P("ORDER_2")->asInt() == 1 ? TABLE_INDEX_Ascending : P("ORDER_2")->asInt() == 2 ? TABLE_INDEX_Descending : TABLE_INDEX_None,
			P("FIELD_3")->asInt(), P("ORDER_3")->asInt() == 1 ? TABLE_INDEX_Ascending : P("ORDER_3")->asInt() == 2 ? TABLE_INDEX_Descending : TABLE_INDEX_None
		);

		_Set_Records();
	}
}

void CDLG_Table_Control::On_Field_Sort_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Field_Count() > 0 && m_pTable->Get_Record_Count() > 1);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_Record_Add(wxCommandEvent &event)
{
	Add_Record();
}

void CDLG_Table_Control::On_Record_Add_UI(wxUpdateUIEvent &event)
{
	event.Enable(!m_pTable->is_Indexed() && m_pTable->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table);
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Record_Ins(wxCommandEvent &event)
{
	Ins_Record();
}

void CDLG_Table_Control::On_Record_Ins_UI(wxUpdateUIEvent &event)
{
	event.Enable(!m_pTable->is_Indexed());
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Record_Del(wxCommandEvent &event)
{
	Del_Record();
}

void CDLG_Table_Control::On_Record_Del_UI(wxUpdateUIEvent &event)
{
	event.Enable(!m_pTable->is_Indexed() && m_pTable->Get_Selection_Count() > 0);
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Record_Clr(wxCommandEvent &event)
{
	Del_Records();
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
void CDLG_Table_Control::On_LClick(wxGridEvent &event)
{
	SetGridCursor(event.GetRow(), event.GetCol());

	//-----------------------------------------------------
	if( event.AltDown() )
	{
		if( m_pTable->Get_Field_Type(event.GetCol()) == SG_DATATYPE_String )
		{
			Open_Application(GetCellValue(event.GetRow(), event.GetCol()));
		}
	}

	//-----------------------------------------------------
	else if( event.ControlDown() )
	{
		if( m_pTable->Get_Field_Type(event.GetCol()) == SG_DATATYPE_String )
		{
			g_pData->Open   (GetCellValue(event.GetRow(), event.GetCol()));
		}
	}

	//-----------------------------------------------------
	else if( m_pTable->Get_Field_Type(event.GetCol()) == SG_DATATYPE_Color )
	{
		CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(event.GetRow());

		long	lValue;

		if( DLG_Color(lValue = pRecord->asInt(event.GetCol())) )
		{
			pRecord->Set_Value(event.GetCol(), lValue);

			SetCellBackgroundColour(event.GetRow(), event.GetCol(), Get_Color_asWX(pRecord->asInt(event.GetCol())));

			ForceRefresh();
		}
	}
}

//---------------------------------------------------------
void CDLG_Table_Control::On_RClick(wxGridEvent &event)
{
	SetGridCursor(event.GetRow(), event.GetCol());

	//-----------------------------------------------------
	if( m_pTable->Get_Field_Type(event.GetCol()) == SG_DATATYPE_String )
	{
		wxMenu	Menu;

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_OPEN_APP);

		if( GetCellValue(event.GetRow(), event.GetCol()).Find("PGSQL:") == 0 || wxFileExists(GetCellValue(event.GetRow(), event.GetCol())) )
		{
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_OPEN_DATA);
		}

		PopupMenu(&Menu, event.GetPosition());
	}
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Field_Open(wxCommandEvent &event)
{
	if( event.GetId() == ID_CMD_TABLE_FIELD_OPEN_APP )
	{
		Open_Application(GetCellValue(GetGridCursorRow(), GetGridCursorCol()));
	}

	if( event.GetId() == ID_CMD_TABLE_FIELD_OPEN_DATA )
	{
		g_pData->Open   (GetCellValue(GetGridCursorRow(), GetGridCursorCol()));
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Table_Control::On_LClick_Label(wxGridEvent &event)
{
	if( event.GetRow() != -1 )	// select records
	{
		if( event.ControlDown() )
		{
			m_pTable->Select(m_pTable->Get_Record_byIndex(event.GetRow()), true);

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

//---------------------------------------------------------
void CDLG_Table_Control::On_LDClick_Label(wxGridEvent &event)
{
	Update_Sorting(event.GetCol(), -1);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDLG_Table_Control::Update_Selection(void)
{
	if( GetBatchCount() == 0 )
	{
		BeginBatch();

		if( m_pTable->Get_Selection_Count() >= m_pTable->Get_Count() )
		{
			SelectAll();
		}
		else
		{
			ClearSelection();

			if( m_pTable->Get_Selection_Count() > 0 )
			{
				#pragma omp parallel for
				for(int iRecord=0; iRecord<GetNumberRows(); iRecord++)
				{
					if( m_pTable->Get_Record_byIndex(iRecord)->is_Selected() )
					{
						SelectRow(iRecord, true);
					}
				}
			}
		}

		EndBatch();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CDLG_Table_Control::On_Select(wxGridRangeSelectEvent &event)
{
	if( GetBatchCount() == 0 )
	{
		BeginBatch();

		for(int iRow=event.GetTopRow(); iRow<=event.GetBottomRow(); iRow++)
		{
			CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(iRow);

			if( pRecord->is_Selected() != event.Selecting() )
			{
				m_pTable->Select(pRecord, true);
			}
		}

		EndBatch();
	}

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
