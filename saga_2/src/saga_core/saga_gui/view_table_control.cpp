
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
#define FIXED_COLS		((m_Constraint & TABLE_CTRL_FIXED_COLS)  != 0)
#define FIXED_ROWS		((m_Constraint & TABLE_CTRL_FIXED_ROWS)  != 0)
#define FIXED_TABLE		((m_Constraint & TABLE_CTRL_FIXED_TABLE) != 0)

//---------------------------------------------------------
#define SET_CELL_VALUE(REC, FLD, VAL)	SetCellValue(REC, FLD, VAL)
#define SET_CELL_COLOR(REC, FLD, VAL)	SetCellBackgroundColour(REC, FLD, wxColour(SG_GET_R(VAL), SG_GET_G(VAL), SG_GET_B(VAL)))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Table_Control, wxGrid)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Table_Control, wxGrid)
	EVT_GRID_CELL_CHANGE		(CVIEW_Table_Control::On_Change)
	EVT_GRID_CELL_LEFT_CLICK	(CVIEW_Table_Control::On_LClick)
	EVT_GRID_LABEL_LEFT_CLICK	(CVIEW_Table_Control::On_LClick_Label)
	EVT_GRID_LABEL_LEFT_DCLICK	(CVIEW_Table_Control::On_LDClick_Label)
	EVT_GRID_LABEL_RIGHT_CLICK	(CVIEW_Table_Control::On_RClick_Label)
	EVT_GRID_RANGE_SELECT		(CVIEW_Table_Control::On_Select)

	EVT_MENU					(ID_CMD_TABLE_FIELD_ADD			, CVIEW_Table_Control::On_Field_Add)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_ADD			, CVIEW_Table_Control::On_Field_Add_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_DEL			, CVIEW_Table_Control::On_Field_Del)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_DEL			, CVIEW_Table_Control::On_Field_Del_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_SORT		, CVIEW_Table_Control::On_Field_Sort)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_SORT		, CVIEW_Table_Control::On_Field_Sort_UI)

	EVT_MENU					(ID_CMD_TABLE_RECORD_ADD		, CVIEW_Table_Control::On_Record_Add)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_ADD		, CVIEW_Table_Control::On_Record_Add_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_INS		, CVIEW_Table_Control::On_Record_Ins)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_INS		, CVIEW_Table_Control::On_Record_Ins_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_DEL		, CVIEW_Table_Control::On_Record_Del)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_DEL		, CVIEW_Table_Control::On_Record_Del_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_DEL_ALL	, CVIEW_Table_Control::On_Record_Clr)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_DEL_ALL	, CVIEW_Table_Control::On_Record_Clr_UI)

	EVT_MENU					(ID_CMD_TABLE_AUTOSIZE_COLS		, CVIEW_Table_Control::On_Autosize_Cols)
	EVT_MENU					(ID_CMD_TABLE_AUTOSIZE_ROWS		, CVIEW_Table_Control::On_Autosize_Rows)
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
	m_Constraint	= Constraint;

	CreateGrid(m_pTable->Get_Record_Count(), m_pTable->Get_Field_Count(), wxGrid::wxGridSelectRows);

	_Set_Table();
}

//---------------------------------------------------------
CVIEW_Table_Control::~CVIEW_Table_Control(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::Update_Table(void)
{
	_Set_Table();
}

//---------------------------------------------------------
void CVIEW_Table_Control::Sort_Table(int iField, int Direction)
{
	if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
	{
		switch( Direction )
		{
		default:	m_pTable->Toggle_Index(iField);	break;
		case 0:		m_pTable->Set_Index(iField, TABLE_INDEX_None);	break;
		case 1:		m_pTable->Set_Index(iField, TABLE_INDEX_Up  );	break;
		case 2:		m_pTable->Set_Index(iField, TABLE_INDEX_Down);	break;
		}

		_Set_Records();
	}
}

//---------------------------------------------------------
bool CVIEW_Table_Control::_Set_Table(void)
{
	int		iField, Difference;

	//-----------------------------------------------------
	BeginBatch();

	//-----------------------------------------------------
	Difference	= m_pTable->Get_Record_Count() - GetNumberRows();

	if( Difference > 0 )
	{
		AppendRows(Difference);
	}
	else if( Difference < 0 )
	{
		DeleteRows(0, -Difference);
	}

	//-----------------------------------------------------
	Difference	= m_pTable->Get_Field_Count() - GetNumberCols();

	if( Difference > 0 )
	{
		AppendCols(Difference);
	}
	else if( Difference < 0 )
	{
		DeleteCols(0, -Difference);
		// here is a memory leak - solution: use own wxGridTableBase derived grid table class
	}

	//-----------------------------------------------------
	for(iField=0; iField<m_pTable->Get_Field_Count(); iField++)
	{
		SetColLabelValue(iField, m_pTable->Get_Field_Name(iField));

		switch( m_pTable->Get_Field_Type(iField) )
		{
		case TABLE_FIELDTYPE_Char:
		case TABLE_FIELDTYPE_String: default:
			break;

		case TABLE_FIELDTYPE_Short:
		case TABLE_FIELDTYPE_Int:
		case TABLE_FIELDTYPE_Long:
			SetColFormatNumber(iField);
			break;

		case TABLE_FIELDTYPE_Float:
		case TABLE_FIELDTYPE_Double:
			SetColFormatFloat(iField);
			break;

		case TABLE_FIELDTYPE_Color:
			SetColFormatNumber(iField);
			break;
		}
	}

	//-----------------------------------------------------
	_Set_Records();

	//-----------------------------------------------------
	EndBatch();

	return( true );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::_Set_Records(void)
{
	BeginBatch();

	for(int iRecord=0; iRecord<m_pTable->Get_Record_Count() && PROGRESSBAR_Set_Position(iRecord, m_pTable->Get_Record_Count()); iRecord++)
	{
		_Set_Record(iRecord, m_pTable->Get_Record_byIndex(iRecord));
	}

	PROCESS_Set_Okay();

	Update_Selection();

	EndBatch();

	return( true );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::_Set_Record(int iRecord, CSG_Table_Record *pRecord)
{
	if( pRecord && iRecord >= 0 && iRecord < GetNumberRows() )
	{
		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			switch( m_pTable->Get_Field_Type(iField) )
			{
			default:
				SET_CELL_VALUE(iRecord, iField, pRecord->asString	(iField));
				break;

			case TABLE_FIELDTYPE_Color:
				SET_CELL_COLOR(iRecord, iField, pRecord->asInt		(iField));
				break;
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Control::Add_Record(void)
{
	if( !FIXED_ROWS )
	{
		AppendRows();

		_Set_Record(m_pTable->Get_Record_Count(), m_pTable->Add_Record());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::Ins_Record(void)
{
	if( !FIXED_ROWS )
	{
		int		iRecord	= GetGridCursorRow();

		if( iRecord >= 0 && iRecord < GetNumberRows() )
		{
			InsertRows(iRecord);

			_Set_Record(iRecord, m_pTable->Ins_Record(iRecord));

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::Del_Record(void)
{
	if( !FIXED_ROWS )
	{
		int		iRecord	= GetGridCursorRow();

		if( iRecord >= 0 && iRecord < GetNumberRows() )
		{
			DeleteRows(iRecord);

			m_pTable->Del_Record(iRecord);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::Del_Records(void)
{
	if( !FIXED_ROWS && m_pTable->Del_Records() )
	{
		DeleteRows(0, GetNumberRows());

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Control::Load(const wxChar *File_Name)
{
	bool	bResult	= false;
	CSG_Table	Table;

	if(	Table.Create(File_Name)
	&&	Table.Get_Record_Count() > 0
	&&	Table.Get_Field_Count() == m_pTable->Get_Field_Count() )
	{
		m_pTable->Assign_Values(&Table);

		_Set_Table();

		bResult	= true;
	}

	PROCESS_Set_Okay();

	return( bResult );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::Save(const wxChar *File_Name, int Format)
{
	bool	bResult;

	bResult	= m_pTable->Save(File_Name);

	PROCESS_Set_Okay();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_Change(wxGridEvent &event)
{
	int				iRecord, iField;
	CSG_Table_Record	*pRecord;

	iRecord	= event.GetRow();

	if( (pRecord = m_pTable->Get_Record_byIndex(iRecord)) != NULL )
	{
		iField	= event.GetCol();

		if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
		{
			pRecord->Set_Value(iField, GetCellValue(iRecord, iField).c_str());

			SET_CELL_VALUE(iRecord, iField, pRecord->asString(iField));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_Field_Add(wxCommandEvent &event)
{
	int				i;
	CSG_String		sFields;
	CSG_Parameters	P;

	//-----------------------------------------------------
	for(i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		sFields.Append(m_pTable->Get_Field_Name(i));	sFields.Append(wxT('|'));
	}

	//-----------------------------------------------------
	P.Set_Name(LNG("Add Field"));

	P.Add_String(
		NULL	, "NAME"	, LNG("Name"),
		LNG(""),
		LNG("Field")
	);

	P.Add_Choice(
		NULL	, "TYPE"	, LNG("Field Type"),
		LNG(""),
		CSG_String::Format(wxT("%s|%s|%s|%s|%s|%s|%s|%s|"),
			LNG("character string"),
			LNG("1 byte integer"),
			LNG("2 byte integer"),
			LNG("4 byte integer"),
			LNG("4 byte floating point"),
			LNG("8 byte floating point"),
			LNG("date (dd.mm.yyyy)"),
			LNG("color (rgb)")
		)
	);

	P.Add_Choice(
		NULL	, "FIELD"	, LNG("Insert Position"),
		LNG(""),
		sFields, m_pTable->Get_Field_Count() - 1
	);

	P.Add_Choice(
		NULL	, "INSERT"	, LNG("Insert Method"),
		LNG(""),
		CSG_String::Format(wxT("%s|%s|"),
			LNG("before"),
			LNG("after")
		), 1
	);

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		int					Position;
		TSG_Table_Field_Type	Type;

		switch( P("TYPE")->asInt() )
		{
		default:
		case 0:	Type	= TABLE_FIELDTYPE_String;	break;
		case 1:	Type	= TABLE_FIELDTYPE_Char;		break;
		case 2:	Type	= TABLE_FIELDTYPE_Short;	break;
		case 3:	Type	= TABLE_FIELDTYPE_Int;		break;
		case 4:	Type	= TABLE_FIELDTYPE_Float;	break;
		case 5:	Type	= TABLE_FIELDTYPE_Double;	break;
		case 6:	Type	= TABLE_FIELDTYPE_Date;		break;
		case 7:	Type	= TABLE_FIELDTYPE_Color;	break;
		}

		Position	= P("FIELD")->asInt() + P("INSERT")->asInt();

		m_pTable->Add_Field(P("NAME")->asString(), Type, Position);

		Update_Table();
	}
}

void CVIEW_Table_Control::On_Field_Add_UI(wxUpdateUIEvent &event)
{
	event.Enable(FIXED_COLS == false);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Field_Del(wxCommandEvent &event)
{
	int			i;
	CSG_Parameters	P;

	//-----------------------------------------------------
	P.Set_Name(LNG("Delete Fields"));

	for(i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		P.Add_Value(
			NULL	, CSG_String::Format(wxT("_%d_"), i), m_pTable->Get_Field_Name(i),
			LNG(""),
			PARAMETER_TYPE_Bool, false
		);
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		for(i=m_pTable->Get_Field_Count()-1; i>=0; i--)
		{
			if( P(CSG_String::Format(wxT("_%d_"), i))->asBool() )
			{
				m_pTable->Del_Field(i);
			}
		}

		Update_Table();
	}
}

void CVIEW_Table_Control::On_Field_Del_UI(wxUpdateUIEvent &event)
{
	event.Enable(FIXED_COLS == false);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Field_Sort(wxCommandEvent &event)
{
	CSG_String	sFields;
	CSG_Parameters	P;

	//-----------------------------------------------------
	for(int i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		sFields.Append(m_pTable->Get_Field_Name(i));	sFields.Append('|');
	}

	//-----------------------------------------------------
	P.Set_Name(LNG("Sort Table"));

	P.Add_Choice(
		NULL	, "FIELD"	, LNG("Sort by"),
		LNG(""),
		sFields
	);

	P.Add_Choice(
		NULL	, "METHOD"	, LNG("Direction"),
		LNG(""),
		CSG_String::Format(wxT("%s|%s|%s|"),
			LNG("unsorted"),
			LNG("ascending"),
			LNG("descending")
		), 1
	);

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		Sort_Table(P("FIELD")->asInt(), P("METHOD")->asInt());
	}
}

void CVIEW_Table_Control::On_Field_Sort_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Field_Count() > 0 && m_pTable->Get_Record_Count() > 1);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Add(wxCommandEvent &event)
{
	Add_Record();
}

void CVIEW_Table_Control::On_Record_Add_UI(wxUpdateUIEvent &event)
{
	event.Enable(!FIXED_ROWS);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Ins(wxCommandEvent &event)
{
	Ins_Record();
}

void CVIEW_Table_Control::On_Record_Ins_UI(wxUpdateUIEvent &event)
{
	event.Enable(!FIXED_ROWS);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Del(wxCommandEvent &event)
{
	Del_Record();
}

void CVIEW_Table_Control::On_Record_Del_UI(wxUpdateUIEvent &event)
{
	event.Enable(!FIXED_ROWS);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Clr(wxCommandEvent &event)
{
	Del_Records();
}

void CVIEW_Table_Control::On_Record_Clr_UI(wxUpdateUIEvent &event)
{
	event.Enable(!FIXED_ROWS);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_LClick(wxGridEvent &event)
{
	int					iRecord, iField;
	long				lValue;
	CSG_Table_Record	*pRecord;

	iRecord	= event.GetRow();

	if( (pRecord = m_pTable->Get_Record_byIndex(iRecord)) != NULL )
	{
		iField	= event.GetCol();

		if( iField >= 0 && iField < m_pTable->Get_Field_Count() )
		{
			switch( m_pTable->Get_Field_Type(iField) )
			{
			default:
				break;

			case TABLE_FIELDTYPE_Color:
				if( DLG_Color(lValue = pRecord->asInt(iField)) )
				{
					pRecord->Set_Value(iField, lValue);

					SET_CELL_COLOR(iRecord, iField, lValue);

					ForceRefresh();
				}

				return;
			}
		}
	}

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_LClick_Label(wxGridEvent &event)
{
	if( event.GetCol() >= 0 )
	{
		SelectRow		(GetGridCursorRow(), false);
		SetGridCursor	(GetGridCursorRow(), event.GetCol());
	}
	else if( event.GetRow() >= 0 )
	{
		SelectRow		(event.GetRow(), false);
		SetGridCursor	(event.GetRow(), GetGridCursorCol());
	}
	else
	{
		SelectAll();
	}
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_RClick_Label(wxGridEvent &event)
{
	wxMenu	*pMenu;

	//-----------------------------------------------------
	if( event.GetCol() != -1 )
	{
		SetGridCursor	(GetGridCursorRow(), event.GetCol());

		pMenu	= new wxMenu(GetColLabelValue(event.GetCol()), 0);

		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_FIELD_ADD);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_FIELD_DEL);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_AUTOSIZE_COLS);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_FIELD_SORT);

		PopupMenu(pMenu, event.GetPosition().x, event.GetPosition().y - GetColLabelSize());
		delete(pMenu);
	}

	//-----------------------------------------------------
	else if( event.GetRow() != -1 )
	{
		SelectRow		(event.GetRow(), false);
		SetGridCursor	(event.GetRow(), GetGridCursorCol());

		pMenu	= new wxMenu(wxString::Format(wxT("%s %d"), LNG("Row"), 1 + event.GetRow()), 0);

		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_RECORD_ADD);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_RECORD_INS);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_RECORD_DEL);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_RECORD_DEL_ALL);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_AUTOSIZE_ROWS);

		PopupMenu(pMenu, event.GetPosition().x - GetRowLabelSize(), event.GetPosition().y);
		delete(pMenu);
	}

	//-----------------------------------------------------
	else
	{
		ClearSelection();
	}
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_LDClick_Label(wxGridEvent &event)
{
	Sort_Table(event.GetCol(), -1);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_Select(wxGridRangeSelectEvent &event)
{
	for(int iRow=event.GetTopRow(); iRow<=event.GetBottomRow(); iRow++)
	{
		_Select(iRow, event.Selecting());
	}

	event.Skip();
}

//---------------------------------------------------------
inline void CVIEW_Table_Control::_Select(int iRow, bool bSelect)
{
	CSG_Table_Record	*pRecord;

	if( (pRecord = m_pTable->Get_Record_byIndex(iRow)) != NULL && bSelect != pRecord->is_Selected() )
	{
		m_pTable->Select(pRecord, true);

		if( m_pTable->is_Private() )
		{
			g_pData->Update_Views(m_pTable->Get_Owner());
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Control::Update_Selection(void)
{
	int				i, j, n;
	CSG_Table_Record	*pRecord, **Selection;

	if( (n = m_pTable->Get_Selection_Count()) > 0 )
	{
		Selection	= (CSG_Table_Record **)SG_Malloc(n * sizeof(CSG_Table_Record *));

		for(i=0; i<n; i++)
		{
			Selection[i]	= m_pTable->Get_Selection(i);
		}

		ClearSelection();

		for(j=0; j<m_pTable->Get_Record_Count(); j++)
		{
			pRecord	= m_pTable->Get_Record_byIndex(j);

			for(i=0; i<n; i++)
			{
				if( pRecord == Selection[i] )
				{
					SelectRow(j, true);
					break;
				}
			}
		}

		SG_Free(Selection);
	}
	else
	{
		ClearSelection();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
