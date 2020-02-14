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
//                VIEW_Table_Control.cpp                 //
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
#include <wx/window.h>
#include <wx/filename.h>
#include <wx/clipbrd.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_data_manager.h"

#include "view_table_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Table_Control, wxGrid)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Table_Control, wxGrid)
	EVT_KEY_DOWN				(CVIEW_Table_Control::On_Key)
	EVT_SCROLLWIN				(CVIEW_Table_Control::On_Scroll)
	EVT_SIZE					(CVIEW_Table_Control::On_Size)

	EVT_GRID_EDITOR_SHOWN		(CVIEW_Table_Control::On_Edit_Start)
	EVT_GRID_EDITOR_HIDDEN		(CVIEW_Table_Control::On_Edit_Stop)
	EVT_GRID_CELL_CHANGED		(CVIEW_Table_Control::On_Changed)

	EVT_GRID_CELL_LEFT_CLICK	(CVIEW_Table_Control::On_LClick)
	EVT_GRID_CELL_RIGHT_CLICK	(CVIEW_Table_Control::On_RClick)
	EVT_GRID_LABEL_LEFT_CLICK	(CVIEW_Table_Control::On_LClick_Label)
	EVT_GRID_LABEL_LEFT_DCLICK	(CVIEW_Table_Control::On_LDClick_Label)
	EVT_GRID_LABEL_RIGHT_CLICK	(CVIEW_Table_Control::On_RClick_Label)

	EVT_GRID_RANGE_SELECT		(CVIEW_Table_Control::On_Select)

	EVT_MENU					(ID_CMD_TABLE_SELECTION_ONLY	, CVIEW_Table_Control::On_Sel_Only)
	EVT_UPDATE_UI				(ID_CMD_TABLE_SELECTION_ONLY	, CVIEW_Table_Control::On_Sel_Only_UI)
	EVT_MENU					(ID_CMD_TABLE_SELECTION_CLEAR	, CVIEW_Table_Control::On_Sel_Clear)
	EVT_UPDATE_UI				(ID_CMD_TABLE_SELECTION_CLEAR	, CVIEW_Table_Control::On_Sel_Clear_UI)

	EVT_MENU					(ID_CMD_TABLE_AUTOSIZE_COLS		, CVIEW_Table_Control::On_Autosize_Cols)
	EVT_MENU					(ID_CMD_TABLE_AUTOSIZE_ROWS		, CVIEW_Table_Control::On_Autosize_Rows)

	EVT_MENU					(ID_CMD_TABLE_FIELD_OPEN_APP	, CVIEW_Table_Control::On_Cell_Open)
	EVT_MENU					(ID_CMD_TABLE_FIELD_OPEN_DATA	, CVIEW_Table_Control::On_Cell_Open)

	EVT_MENU					(ID_CMD_TABLE_TO_CLIPBOARD		, CVIEW_Table_Control::On_ToClipboard)

	EVT_MENU					(ID_CMD_TABLE_FIELD_ADD			, CVIEW_Table_Control::On_Field_Add)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_ADD			, CVIEW_Table_Control::On_Field_Add_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_DEL			, CVIEW_Table_Control::On_Field_Del)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_DEL			, CVIEW_Table_Control::On_Field_Del_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_RENAME		, CVIEW_Table_Control::On_Field_Rename)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_RENAME		, CVIEW_Table_Control::On_Field_Rename_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_TYPE		, CVIEW_Table_Control::On_Field_Type)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_TYPE		, CVIEW_Table_Control::On_Field_Type_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_SORT		, CVIEW_Table_Control::On_Field_Sort)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_SORT		, CVIEW_Table_Control::On_Field_Sort_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_CALC		, CVIEW_Table_Control::On_Field_Calc)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_CALC		, CVIEW_Table_Control::On_Field_Calc_UI)

	EVT_MENU					(ID_CMD_TABLE_RECORD_ADD		, CVIEW_Table_Control::On_Record_Add)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_ADD		, CVIEW_Table_Control::On_Record_Add_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_INS		, CVIEW_Table_Control::On_Record_Ins)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_INS		, CVIEW_Table_Control::On_Record_Ins_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_DEL		, CVIEW_Table_Control::On_Record_Del)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_DEL		, CVIEW_Table_Control::On_Record_Del_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_DEL_ALL	, CVIEW_Table_Control::On_Record_Clr)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_DEL_ALL	, CVIEW_Table_Control::On_Record_Clr_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Table_Control::CVIEW_Table_Control(wxWindow *pParent, CSG_Table *pTable, int Constraint)
	: wxGrid(pParent, -1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxSUNKEN_BORDER)
{
	m_pTable		= pTable;

	m_bSelOnly		= false;
	m_bEditing		= false;

	m_Cursor		= -1;

	m_Scroll_Start	= 0;
	m_Scroll_Range	= 0;

	m_Decimals		= -1;

	SetRowLabelAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);

	DisableDragRowSize();

	CreateGrid(0, 0, wxGrid::wxGridSelectRows);

	Update_Table();

	wxGrid::AdjustScrollbars();
}

//---------------------------------------------------------
CVIEW_Table_Control::~CVIEW_Table_Control(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Control::Load(const wxString &File_Name)
{
	CSG_Table	Table(&File_Name);

	bool	bResult	= Table.Get_Count() > 0
		&& Table.Get_Field_Count() == m_pTable->Get_Field_Count()
		&& m_pTable->Assign_Values(&Table)
		&& Update_Table();

	PROCESS_Set_Okay();

	return( bResult );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::Save(const wxString &File_Name, int Format)
{
	bool	bResult	= m_pTable->Save(&File_Name);

	PROCESS_Set_Okay();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Control::Update_Table(void)
{
	if( GetBatchCount() > 0 )
	{
		return( false );
	}

	BeginBatch();

	//-----------------------------------------------------
	int	Difference;

	if( (Difference = (m_pTable->Get_Field_Count()) - GetNumberCols()) > 0 )
	{
		AppendCols(Difference);
	}
	else if( Difference < 0 && (Difference = -Difference < GetNumberCols() ? -Difference : GetNumberCols()) > 0 )
	{	// here is (or was!?) a memory leak - solution: use own wxGridTableBase derived grid table class
		DeleteCols(0, Difference);
	}

	//-----------------------------------------------------
	CWKSP_Base_Item	*pItem	= (CWKSP_Base_Item *)g_pData->Get(m_pTable);

	if( pItem && pItem->Get_Parameter("TABLE_FLT_STYLE") && pItem->Get_Parameter("TABLE_FLT_DECIMALS") )
	{
		switch( pItem->Get_Parameter("TABLE_FLT_STYLE")->asInt() )
		{
		default:	// system default
			m_Decimals	= 0;
			break;

		case  1:	// maximum number of significant decimals
			m_Decimals	= -pItem->Get_Parameter("TABLE_FLT_DECIMALS")->asInt() - 1;
			break;

		case  2:	// fix number of decimals
			m_Decimals	=  pItem->Get_Parameter("TABLE_FLT_DECIMALS")->asInt() + 1;
			break;
		}
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
			if( m_Decimals == 0 )
			{
				SetColFormatFloat(iField);
			}
			else
			{
				SetColFormatCustom(iField, wxGRID_VALUE_STRING);

				wxGridCellAttr	*pAttr	= new wxGridCellAttr;	pAttr->SetAlignment(wxALIGN_RIGHT, wxALIGN_TOP);

				SetColAttr(iField, pAttr);
			}
			break;
		}
	}

	//-----------------------------------------------------
	if( _Update_Records() )
	{
		EndBatch();

		return( true );
	}

	EndBatch();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Control::_Update_Records(void)
{
	BeginBatch();

	//-----------------------------------------------------
	int	nRows	= _Get_Record_Count();

	if( nRows > m_Scroll_Range )
	{
		nRows	= m_Scroll_Range;
	}

	//-----------------------------------------------------
	int	Difference;

	if( (Difference = nRows - GetNumberRows()) > 0 )
	{
		AppendRows(Difference);
	}
	else if( Difference < 0 && (Difference = -Difference < GetNumberRows() ? -Difference : GetNumberRows()) > 0 )
	{
		DeleteRows(0, Difference);
	}

	//-----------------------------------------------------
	m_Scroll_Start	= _Get_Scroll_Start(m_Scroll_Start);

	ClearSelection();

	for(int iRow=0; iRow<nRows; iRow++)
	{
		_Set_Record(iRow);
	}

	//-----------------------------------------------------
	_Update_Views();

	EndBatch();

	return( true );
}

//---------------------------------------------------------
inline int CVIEW_Table_Control::_Get_Record_Count(void)
{
	return( m_bSelOnly ? m_pTable->Get_Selection_Count() : m_pTable->Get_Count() );
}

//---------------------------------------------------------
inline CSG_Table_Record * CVIEW_Table_Control::_Get_Record(int iRow)
{
	return( m_bSelOnly ? m_pTable->Get_Selection(m_Scroll_Start + iRow) : m_pTable->Get_Record_byIndex(m_Scroll_Start + iRow) );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::_Set_Record(int iRow)
{
	CSG_Table_Record	*pRecord	= _Get_Record(iRow);

	if( pRecord )
	{
		SetRowLabelValue(iRow, wxString::Format("%d", 1 + m_Scroll_Start + iRow));

		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			switch( m_pTable->Get_Field_Type(iField) )
			{
			default:
				SetCellValue(iRow, iField, _Get_Value(pRecord, iField));
				break;

			case SG_DATATYPE_Color:
				SetCellBackgroundColour(iRow, iField, Get_Color_asWX(pRecord->asInt(iField)));
				break;
			}
		}

		if( m_bSelOnly || pRecord->is_Selected() )
		{
			SelectRow(iRow, true);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
wxString CVIEW_Table_Control::_Get_Value(CSG_Table_Record *pRecord, int iField)
{
	if( pRecord->is_NoData(iField) )
	{
		return( "" );
	}

	switch( m_pTable->Get_Field_Type(iField) )
	{
	default:
		return( pRecord->asString(iField) );

	case SG_DATATYPE_Float:
	case SG_DATATYPE_Double:
		{
			double	Value	= pRecord->asDouble(iField);

			if( m_Decimals == 0 )		// system default
			{
				return( wxString::Format("%f", Value) );
			}
			else if( abs(m_Decimals) == 1 )	// no decimals
			{
				return( wxString::Format("%.0f", Value) );
			}
			else if( m_Decimals > 0 )	// fix number of decimals
			{
				return( wxString::Format("%.*f", m_Decimals - 1, Value) );
			}
			else //( m_Decimals < 0 )	// maximum number of significant decimals
			{
				return( SG_Get_String(Value, m_Decimals + 1).c_str() );
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Control::Update_Selection(void)
{
	if( GetBatchCount() > 0 )
	{
		return( false );
	}

	BeginBatch();

	//-----------------------------------------------------
	if( m_bSelOnly )
	{
		_Update_Records();
	}
	else if( m_pTable->Get_Selection_Count() >= (size_t)m_pTable->Get_Count() )
	{
		SelectAll();
	}
	else
	{
		ClearSelection();

		if( m_pTable->Get_Selection_Count() > 0 )
		{
			for(int iRow=0; iRow<GetNumberRows(); iRow++)
			{
				CSG_Table_Record	*pRecord	= _Get_Record(iRow);

				if( pRecord && pRecord->is_Selected() )
				{
					SelectRow(iRow, true);
				}
			}
		}
	}

	//-----------------------------------------------------
	EndBatch();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::_Update_Views(void)
{
	if( GetBatchCount() == 0 )
	{
		BeginBatch();

		g_pData->Update_Views(m_pTable);

		EndBatch();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Control::_Update_Sorting(int iField, int Direction)
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		m_Cursor	= -1;

		switch( Direction )
		{
		default:	m_pTable->Toggle_Index(iField);	break;

		case  0:	m_pTable->Set_Index(iField, TABLE_INDEX_None      );	break;
		case  1:	m_pTable->Set_Index(iField, TABLE_INDEX_Ascending );	break;
		case  2:	m_pTable->Set_Index(iField, TABLE_INDEX_Descending);	break;
		}

		return( _Update_Records() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CVIEW_Table_Control::_Get_Scroll_Start(int Position)
{
	if( Position   >= _Get_Record_Count() - m_Scroll_Range )
	{
		Position	= _Get_Record_Count() - m_Scroll_Range;
	}

	if( Position < 0 )
	{
		Position	= 0;
	}

	return( Position );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::_Set_Scroll_Start(int Position, bool bEnforceUpdate)
{
	Position	= _Get_Scroll_Start(Position);

	if( m_Scroll_Start != Position || bEnforceUpdate )
	{
		m_Scroll_Start	= Position;

		return( _Update_Records() );
	}

	return( true );
}

//---------------------------------------------------------
void CVIEW_Table_Control::AdjustScrollbars(void)
{
	//-----------------------------------------------------
	m_Scroll_Range	= (GetClientSize().y - GetColLabelSize()) / GetDefaultRowSize();
//	m_Scroll_Range	= GetGridWindow()->GetClientSize().y / GetDefaultRowSize();

	SetScrollbar(wxVERTICAL, m_Scroll_Start, m_Scroll_Range, _Get_Record_Count());

	//-----------------------------------------------------
	int	Range	= GetGridWindow()->GetVirtualSize().x / GetScrollLineX();
	int	Thumb	= GetGridWindow()->GetClientSize ().x / GetScrollLineX();

	SetScrollbar(wxHORIZONTAL, GetScrollPos(wxHORIZONTAL), Thumb, Range);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Scroll(wxScrollWinEvent &event)
{
	if( event.GetOrientation() == wxHORIZONTAL )
	{
		event.Skip();
	}
	else
	{
		if     ( event.GetEventType() == wxEVT_SCROLLWIN_LINEUP       )	_Set_Scroll_Start(m_Scroll_Start - 1             , false);
		else if( event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN     )	_Set_Scroll_Start(m_Scroll_Start + 1             , false);
		else if( event.GetEventType() == wxEVT_SCROLLWIN_PAGEUP       )	_Set_Scroll_Start(m_Scroll_Start - m_Scroll_Range, false);
		else if( event.GetEventType() == wxEVT_SCROLLWIN_PAGEDOWN     )	_Set_Scroll_Start(m_Scroll_Start + m_Scroll_Range, false);
		else if( event.GetEventType() == wxEVT_SCROLLWIN_TOP          )	_Set_Scroll_Start(0                              , false);
		else if( event.GetEventType() == wxEVT_SCROLLWIN_BOTTOM       )	_Set_Scroll_Start(m_pTable->Get_Count()          , false);
		else if( event.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK   )	_Set_Scroll_Start(event.GetPosition()            , false);
		else if( event.GetEventType() == wxEVT_SCROLLWIN_THUMBRELEASE )	_Set_Scroll_Start(event.GetPosition()            , false);
	}
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Key(wxKeyEvent &event)
{
	if     ( event.GetKeyCode() == WXK_UP       )
	{
		if( GetGridCursorRow() > 0 )
		{
			SetGridCursor(GetGridCursorRow() - 1, GetGridCursorCol());
		}
		else
		{
			_Set_Scroll_Start(m_Scroll_Start - 1             , false);
		}
	}
	else if( event.GetKeyCode() == WXK_DOWN     )
	{
		if( GetGridCursorRow() < m_Scroll_Range - 1 )
		{
			SetGridCursor(GetGridCursorRow() + 1, GetGridCursorCol());
		}
		else
		{
			_Set_Scroll_Start(m_Scroll_Start + 1             , false);
		}
	}
	else if( event.GetKeyCode() == WXK_PAGEUP   )	_Set_Scroll_Start(m_Scroll_Start - m_Scroll_Range, false);
	else if( event.GetKeyCode() == WXK_PAGEDOWN )	_Set_Scroll_Start(m_Scroll_Start + m_Scroll_Range, false);
	else if( event.GetKeyCode() == WXK_HOME     )	_Set_Scroll_Start(0                              , false);
	else if( event.GetKeyCode() == WXK_END      )	_Set_Scroll_Start(m_pTable->Get_Count()          , false);
	else
	{
		event.Skip(true);
	}
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Size(wxSizeEvent &event)//&WXUNUSED(event))
{
	event.Skip();

	if( GetNumberCols() > 0 )
	{
		int	Scroll_Range	= m_Scroll_Range;

		AdjustScrollbars();

		if( Scroll_Range != m_Scroll_Range )
		{
			_Update_Records();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_Edit_Start(wxGridEvent &event)
{
	m_bEditing	= true;

	event.Skip();
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Edit_Stop(wxGridEvent &event)
{
	m_bEditing	= false;

	event.Skip();
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Changed(wxGridEvent &event)
{
	CSG_Table_Record	*pRecord	= _Get_Record(event.GetRow());

	if( pRecord )
	{
		wxString	Value(GetCellValue(event.GetRow(), event.GetCol()));

		if( Value.IsEmpty() )
		{
			pRecord->Set_NoData(event.GetCol());
		}
		else
		{
			pRecord->Set_Value(event.GetCol(), &Value);

			SetCellValue(event.GetRow(), event.GetCol(), _Get_Value(pRecord, event.GetCol()));
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Select(wxGridRangeSelectEvent &event)
{
	Update_Selection();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_LClick(wxGridEvent &event)
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
		CSG_Table_Record	*pRecord	= _Get_Record(event.GetRow());

		long	lValue;

		if( pRecord && DLG_Color(lValue = pRecord->asInt(event.GetCol())) )
		{
			pRecord->Set_Value(event.GetCol(), lValue);

			SetCellBackgroundColour(event.GetRow(), event.GetCol(), Get_Color_asWX(pRecord->asInt(event.GetCol())));

			ForceRefresh();
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_LClick_Label(wxGridEvent &event)
{
	if( !m_bSelOnly )
	{
		if( event.GetRow() >= 0 )	// select records
		{
			if( event.ControlDown() )
			{
				m_pTable->Select(_Get_Record(event.GetRow()), true);
			}
			else if( !event.ShiftDown() )
			{
				m_pTable->Select();	// deselect everything

				m_pTable->Select(_Get_Record(event.GetRow()), false);
			}
			else if( m_Cursor >= 0 && m_Cursor < m_pTable->Get_Count() )
			{
				int	iFirst	= m_Scroll_Start + event.GetRow() <= m_Cursor ? m_Scroll_Start + event.GetRow() : m_Cursor + 1;
				int	iLast	= m_Scroll_Start + event.GetRow() >  m_Cursor ? m_Scroll_Start + event.GetRow() : m_Cursor - 1;

				for(int iRecord=iFirst; iRecord<=iLast; iRecord++)
				{
					m_pTable->Select(m_pTable->Get_Record_byIndex(iRecord)->Get_Index(), true);
				}
			}

			Update_Selection();	_Update_Views();
		}
		else if( event.GetCol() < 0 )
		{
			m_pTable->Select();	// deselect everything

			Update_Selection();	_Update_Views();
		}
	}

	m_Cursor	= m_Scroll_Start + event.GetRow();

	SetGridCursor(event.GetRow(), GetGridCursorCol());

	GetGridWindow()->SetFocus();
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_LDClick_Label(wxGridEvent &event)
{
	_Update_Sorting(event.GetCol(), -1);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Control::_Get_DataSource(wxString &Source)
{
	if( Source.Find("PGSQL:"  ) == 0
	||	Source.Find("ftp://"  ) == 0
	||	Source.Find("http://" ) == 0
	||	Source.Find("https://") == 0
	||	Source.Find("file://" ) == 0
	||  wxFileExists(Source)  )
	{
		return( true );
	}

	if( m_pTable->Get_File_Name(false) )
	{
		wxFileName	fn(Source), dir(m_pTable->Get_File_Name(false));

		if( fn.MakeAbsolute(dir.GetPath()) && fn.Exists() )
		{
			Source	= fn.GetFullPath();

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_RClick(wxGridEvent &event)
{
	SetGridCursor(event.GetRow(), event.GetCol());

	//-----------------------------------------------------
	if( m_pTable->Get_Field_Type(event.GetCol()) == SG_DATATYPE_String )
	{
		wxMenu	Menu;

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_OPEN_APP);

		wxString	Value	= GetCellValue(event.GetRow(), event.GetCol());

		if( _Get_DataSource(Value) )
		{
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_OPEN_DATA);
		}

		PopupMenu(&Menu, event.GetPosition());
	}
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_RClick_Label(wxGridEvent &event)
{
	//-----------------------------------------------------
	if( event.GetCol() != -1 )
	{
		wxMenu	Menu(_TL("Columns"));

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_ADD);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_DEL);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_RENAME);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_TYPE);
		Menu.AppendSeparator();
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_SORT);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_CALC);
		Menu.AppendSeparator();
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_TO_CLIPBOARD);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_AUTOSIZE_COLS);

		PopupMenu(&Menu, event.GetPosition().x, event.GetPosition().y - GetColLabelSize());
	}

	//-----------------------------------------------------
	else if( event.GetRow() != -1 )
	{
		wxMenu	Menu(_TL("Rows"));

		if( m_bSelOnly )
		{
			CMD_Menu_Add_Item(&Menu,  true, ID_CMD_TABLE_SELECTION_ONLY);
			Menu.AppendSeparator();
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_TO_CLIPBOARD);
		}
		else
		{
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_ADD);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_INS);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_DEL);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_DEL_ALL);
		//	Menu.AppendSeparator();
		//	CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_AUTOSIZE_ROWS);
			Menu.AppendSeparator();
			CMD_Menu_Add_Item(&Menu,  true, ID_CMD_TABLE_SELECTION_ONLY);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_SELECTION_CLEAR);
			Menu.AppendSeparator();
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_TO_CLIPBOARD);
		}

		PopupMenu(&Menu, event.GetPosition().x - GetRowLabelSize(), event.GetPosition().y);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_Sel_Only(wxCommandEvent  &event)
{
	if( m_bSelOnly == true )
	{
		m_bSelOnly	= false;

		_Update_Records();
	}
	else if( m_pTable->Get_Selection_Count() > 0 )
	{
		m_bSelOnly	= true;

		_Update_Records();
	}
}

void CVIEW_Table_Control::On_Sel_Only_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Selection_Count() > 0);

	event.Check (m_bSelOnly);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Sel_Clear(wxCommandEvent  &event)
{
	if( m_pTable->Get_Selection_Count() > 0 )
	{
		m_pTable->Select();

		Update_Selection();	_Update_Views();
	}
}

void CVIEW_Table_Control::On_Sel_Clear_UI(wxUpdateUIEvent &event)
{
	event.Enable(!m_bSelOnly && m_pTable->Get_Selection_Count() > 0);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Autosize_Cols(wxCommandEvent &event)
{
	AutoSizeColumns(false);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Autosize_Rows(wxCommandEvent &event)
{
	AutoSizeRows(false);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Cell_Open(wxCommandEvent &event)
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
		if( !_Get_DataSource(Value) || !g_pData->Open(Value) )
		{
			DLG_Message_Show_Error(_TL("failed"), CMD_Get_Name(ID_CMD_TABLE_FIELD_OPEN_DATA));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_ToClipboard(wxCommandEvent &event)
{
	_ToClipboard();
}

//---------------------------------------------------------
bool CVIEW_Table_Control::_ToClipboard(void)
{
	if( wxTheClipboard->Open() )
	{
		wxString	Data;

		int	i, j, n	= m_pTable->Get_Selection_Count()
			? m_pTable->Get_Selection_Count() : m_pTable->Get_Count();

		for(j=0; j<m_pTable->Get_Field_Count(); j++)
		{
			Data	+= m_pTable->Get_Field_Name(j);
			Data	+= j + 1 < m_pTable->Get_Field_Count() ? '\t' : '\n';
		}

		for(i=0; i<n; i++)
		{
			CSG_Table_Record	*pRecord	= m_pTable->Get_Selection_Count()
				? m_pTable->Get_Selection(i) : m_pTable->Get_Record_byIndex(i);

			for(j=0; j<m_pTable->Get_Field_Count(); j++)
			{
				Data	+= pRecord->asString(j);
				Data	+= j + 1 < m_pTable->Get_Field_Count() ? '\t' : '\n';
			}
		}

		wxTheClipboard->SetData(new wxTextDataObject(Data));
		wxTheClipboard->Close();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_Field_Add(wxCommandEvent &event)
{
	//-----------------------------------------------------
	CSG_String	Fields;

	for(int i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		Fields	+= m_pTable->Get_Field_Name(i) + CSG_String('|');
	}

	//-----------------------------------------------------
	CSG_Parameters	P(_TL("Add Field"));

	P.Add_String(
		"", "NAME"	, _TL("Name"),
		_TL(""),
		_TL("Field")
	);

	P.Add_Choice(
		"", "TYPE"	, _TL("Field Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			SG_Data_Type_Get_Name(SG_DATATYPE_String).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Date  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Color ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Byte  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Char  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Word  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Short ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_DWord ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Int   ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_ULong ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Long  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Float ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Double).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Binary).c_str()
		), 0
	);

	P.Add_Choice(
		"", "FIELD"	, _TL("Insert Position"),
		_TL(""),
		Fields, m_pTable->Get_Field_Count() - 1
	);

	P.Add_Choice(
		"", "INSERT"	, _TL("Insert Method"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("before"),
			_TL("after")
		), 1
	);

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		int				Position;
		TSG_Data_Type	Type;

		switch( P("TYPE")->asInt() )
		{
		default:	Type	= SG_DATATYPE_String;	break;
		case  1:	Type	= SG_DATATYPE_Date  ;	break;
		case  2:	Type	= SG_DATATYPE_Color ;	break;
		case  3:	Type	= SG_DATATYPE_Byte  ;	break;
		case  4:	Type	= SG_DATATYPE_Char  ;	break;
		case  5:	Type	= SG_DATATYPE_Word  ;	break;
		case  6:	Type	= SG_DATATYPE_Short ;	break;
		case  7:	Type	= SG_DATATYPE_DWord ;	break;
		case  8:	Type	= SG_DATATYPE_Int   ;	break;
		case  9:	Type	= SG_DATATYPE_ULong ;	break;
		case 10:	Type	= SG_DATATYPE_Long  ;	break;
		case 11:	Type	= SG_DATATYPE_Float ;	break;
		case 12:	Type	= SG_DATATYPE_Double;	break;
		case 13:	Type	= SG_DATATYPE_Binary;	break;
		}

		Position	= P("FIELD")->asInt() + P("INSERT")->asInt();

		m_pTable->Add_Field(P("NAME")->asString(), Type, Position);

		g_pData->Update(m_pTable, NULL);
	}
}

void CVIEW_Table_Control::On_Field_Add_UI(wxUpdateUIEvent &event)
{
	event.Enable(true);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Field_Del(wxCommandEvent &event)
{
	int				i;
	CSG_Parameters	P;

	//-----------------------------------------------------
	P.Set_Name(_TL("Delete Fields"));

	for(i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		P.Add_Value(NULL, SG_Get_String(i), m_pTable->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, false);
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		for(i=m_pTable->Get_Field_Count()-1; i>=0; i--)
		{
			if( P(SG_Get_String(i))->asBool() )
			{
				m_pTable->Del_Field(i);

				DeleteCols(i);
			}
		}

		g_pData->Update(m_pTable, NULL);
	}
}

void CVIEW_Table_Control::On_Field_Del_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Field_Count() > 0);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Field_Rename(wxCommandEvent &event)
{
	int				i;
	CSG_Parameters	P;

	P.Set_Name(_TL("Rename Fields"));

	for(i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		P.Add_String(NULL, SG_Get_String(i), m_pTable->Get_Field_Name(i), _TL(""), m_pTable->Get_Field_Name(i));
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		for(i=0; i<m_pTable->Get_Field_Count(); i++)
		{
			CSG_String	s(P(i)->asString());

			if( s.Length() > 0 && s.Cmp(m_pTable->Get_Field_Name(i)) )
			{
				m_pTable->Set_Field_Name(i, s);

				SetColLabelValue(i, s.c_str());
			}
		}

		g_pData->Update(m_pTable, NULL);
	}
}

void CVIEW_Table_Control::On_Field_Rename_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Field_Count() > 0);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Field_Type(wxCommandEvent &event)
{
	int				i, *Types	= new int[m_pTable->Get_Field_Count()];
	CSG_Parameters	P;

	P.Set_Name(_TL("Change Field Type"));

	for(i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		switch( m_pTable->Get_Field_Type(i) )
		{
		default:
		case SG_DATATYPE_String:	Types[i]	=  0;	break;
		case SG_DATATYPE_Date  :	Types[i]	=  1;	break;
		case SG_DATATYPE_Color :	Types[i]	=  2;	break;
		case SG_DATATYPE_Byte  :	Types[i]	=  3;	break;
		case SG_DATATYPE_Char  :	Types[i]	=  4;	break;
		case SG_DATATYPE_Word  :	Types[i]	=  5;	break;
		case SG_DATATYPE_Short :	Types[i]	=  6;	break;
		case SG_DATATYPE_DWord :	Types[i]	=  7;	break;
		case SG_DATATYPE_Int   :	Types[i]	=  8;	break;
		case SG_DATATYPE_ULong :	Types[i]	=  9;	break;
		case SG_DATATYPE_Long  :	Types[i]	= 10;	break;
		case SG_DATATYPE_Float :	Types[i]	= 11;	break;
		case SG_DATATYPE_Double:	Types[i]	= 12;	break;
		case SG_DATATYPE_Binary:	Types[i]	= 13;	break;
		}

		P.Add_Choice("", SG_Get_String(i), m_pTable->Get_Field_Name(i), _TL(""),
			CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			SG_Data_Type_Get_Name(SG_DATATYPE_String).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Date  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Color ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Byte  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Char  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Word  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Short ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_DWord ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Int   ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_ULong ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Long  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Float ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Double).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Binary).c_str()
			), Types[i]
		);
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		bool	bChanged	= false;

		for(i=0; i<m_pTable->Get_Field_Count(); i++)
		{
			TSG_Data_Type	Type;

			switch( P(i)->asInt() )
			{
			default:	Type	= SG_DATATYPE_String;	break;
			case  1:	Type	= SG_DATATYPE_Date  ;	break;
			case  2:	Type	= SG_DATATYPE_Color ;	break;
			case  3:	Type	= SG_DATATYPE_Byte  ;	break;
			case  4:	Type	= SG_DATATYPE_Char  ;	break;
			case  5:	Type	= SG_DATATYPE_Word  ;	break;
			case  6:	Type	= SG_DATATYPE_Short ;	break;
			case  7:	Type	= SG_DATATYPE_DWord ;	break;
			case  8:	Type	= SG_DATATYPE_Int   ;	break;
			case  9:	Type	= SG_DATATYPE_ULong ;	break;
			case 10:	Type	= SG_DATATYPE_Long  ;	break;
			case 11:	Type	= SG_DATATYPE_Float ;	break;
			case 12:	Type	= SG_DATATYPE_Double;	break;
			case 13:	Type	= SG_DATATYPE_Binary;	break;
			}

			if( Type != m_pTable->Get_Field_Type(i) )
			{
				m_pTable->Set_Field_Type(i, Type);

				bChanged	= true;
			}
		}

		if( bChanged )
		{
			Update_Table();

			g_pData->Update(m_pTable, NULL);
		}
	}
}

void CVIEW_Table_Control::On_Field_Type_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Field_Count() > 0);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Field_Sort(wxCommandEvent &event)
{
	//-----------------------------------------------------
	CSG_String	Fields, Order;

	for(int i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		Fields	+= m_pTable->Get_Field_Name(i) + CSG_String("|");
	}

	Order.Printf("%s|%s|%s|",
		_TL("do not sort"),
		_TL("ascending"),
		_TL("descending")
	);

	//-----------------------------------------------------
	CSG_Parameters	P(_TL("Sort Table"));

	P.Add_Choice(""       , "FIELD_1", _TL("Sort first by" ), _TL(""), Fields, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(0));
	P.Add_Choice("FIELD_1", "ORDER_1", _TL("Direction"     ), _TL(""), Order , !m_pTable->is_Indexed() ? 1 : m_pTable->Get_Index_Order(0));

	P.Add_Choice(""       , "FIELD_2", _TL("Sort second by"), _TL(""), Fields, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(1));
	P.Add_Choice("FIELD_2", "ORDER_2", _TL("Direction"     ), _TL(""), Order , !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Order(1));

	P.Add_Choice(""       , "FIELD_3", _TL("Sort third by" ), _TL(""), Fields, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(2));
	P.Add_Choice("FIELD_3", "ORDER_3", _TL("Direction"     ), _TL(""), Order , !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Order(2));

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		m_pTable->Set_Index(
			P("FIELD_1")->asInt(), P("ORDER_1")->asInt() == 1 ? TABLE_INDEX_Ascending : P("ORDER_1")->asInt() == 2 ? TABLE_INDEX_Descending : TABLE_INDEX_None,
			P("FIELD_2")->asInt(), P("ORDER_2")->asInt() == 1 ? TABLE_INDEX_Ascending : P("ORDER_2")->asInt() == 2 ? TABLE_INDEX_Descending : TABLE_INDEX_None,
			P("FIELD_3")->asInt(), P("ORDER_3")->asInt() == 1 ? TABLE_INDEX_Ascending : P("ORDER_3")->asInt() == 2 ? TABLE_INDEX_Descending : TABLE_INDEX_None
		);

		_Update_Records();
	}
}

void CVIEW_Table_Control::On_Field_Sort_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Field_Count() > 0 && m_pTable->Get_Record_Count() > 1);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Field_Calc(wxCommandEvent &event)
{
	//-----------------------------------------------------
	CSG_String	Fields;

	for(int i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		Fields	+= m_pTable->Get_Field_Name(i) + CSG_String("|");
	}

	Fields	+= _TL("<new>") + CSG_String("|");

	//-----------------------------------------------------
	if( m_Field_Calc.Get_Count() == 0 )
	{
		m_Field_Calc.Create(this, _TL("Table Field Calculator"), _TL(""), SG_T("FIELD_CALCULATOR"));

		m_Field_Calc.Set_Callback_On_Parameter_Changed(_Parameter_Callback);

		m_Field_Calc.Add_Choice(""     , "FIELD"    , _TL("Target Field"), _TL(""), Fields, m_pTable->Get_Field_Count());
		m_Field_Calc.Add_String("FIELD", "NAME"     , _TL("Field Name"  ), _TL(""), _TL("Result"));
		m_Field_Calc.Add_Bool  (""     , "SELECTION", _TL("Selection"   ), _TL(""), true);
		m_Field_Calc.Add_String(""     , "FORMULA"  , _TL("Formula"     ), _TL(""), "f1 + f2");
	}

	m_Field_Calc("FIELD")->asChoice()->Set_Items(Fields);

	m_Field_Calc.Set_Enabled("SELECTION", m_pTable->Get_Selection_Count() > 0);

	//-----------------------------------------------------
	if( DLG_Parameters(&m_Field_Calc) )
	{
		bool	bResult;

		SG_RUN_TOOL(bResult, "table_calculus", 1,	// table field calculator
				SG_TOOL_PARAMETER_SET("TABLE"    , m_pTable)
			&&	SG_TOOL_PARAMETER_SET("FIELD"    , m_Field_Calc("FIELD"    )->asInt   ())
			&&	SG_TOOL_PARAMETER_SET("NAME"     , m_Field_Calc("NAME"     )->asString())
			&&	SG_TOOL_PARAMETER_SET("SELECTION", m_Field_Calc("SELECTION")->asBool  ())
			&&	SG_TOOL_PARAMETER_SET("FORMULA"  , m_Field_Calc("FORMULA"  )->asString())
		);
	}
}

void CVIEW_Table_Control::On_Field_Calc_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Field_Count() > 0);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CVIEW_Table_Control::_Parameter_Callback(CSG_Parameter *pParameter, int Flags)
{
	CSG_Parameters	*pParameters	= pParameter ? pParameter->Get_Parameters() : NULL;

	if( pParameters && pParameters->Cmp_Identifier("FIELD_CALCULATOR") )
	{
		if( Flags & PARAMETER_CHECK_ENABLE )
		{
			if( pParameter->Cmp_Identifier("FIELD") )
			{
				pParameters->Set_Enabled("NAME", pParameter->asInt() >= pParameter->asChoice()->Get_Count() - 1);
			}
		}

		return( 1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Add(wxCommandEvent &event)
{
	if( m_pTable->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table && m_pTable->Add_Record() )
	{
		_Set_Scroll_Start(m_pTable->Get_Count() - 1);
	}
}

void CVIEW_Table_Control::On_Record_Add_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table && !m_pTable->is_Indexed());
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Ins(wxCommandEvent &event)
{
	if( m_pTable->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table && m_pTable->Ins_Record(m_Scroll_Start + GetGridCursorRow()) )
	{
		_Update_Records();
	}
}

void CVIEW_Table_Control::On_Record_Ins_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table && !m_pTable->is_Indexed());
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Del(wxCommandEvent &event)
{
	if( m_pTable->Del_Selection() )
	{
		_Update_Records();
	}
}

void CVIEW_Table_Control::On_Record_Del_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Selection_Count() > 0);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Clr(wxCommandEvent &event)
{
	if( DLG_Message_Confirm(CMD_Get_Name(ID_CMD_TABLE_RECORD_DEL_ALL), _TL("Table")) && m_pTable->Del_Records() )
	{
		_Update_Records();
	}
}

void CVIEW_Table_Control::On_Record_Clr_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Count() > 0);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
