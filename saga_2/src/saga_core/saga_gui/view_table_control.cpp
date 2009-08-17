
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

#include "wksp_shapes_manager.h"
#include "wksp_shapes.h"

#include "view_table_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define FIXED_COLS		((m_Constraint & TABLE_CTRL_FIXED_COLS)   != 0)
#define FIXED_ROWS		((m_Constraint & TABLE_CTRL_FIXED_ROWS)   != 0)
#define LABEL_COL		((m_Constraint & TABLE_CTRL_COL1ISLABEL)  != 0)

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

	EVT_SIZE					(CVIEW_Table_Control::On_Size)

	EVT_MENU					(ID_CMD_TABLE_FIELD_ADD			, CVIEW_Table_Control::On_Field_Add)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_ADD			, CVIEW_Table_Control::On_Field_Add_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_DEL			, CVIEW_Table_Control::On_Field_Del)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_DEL			, CVIEW_Table_Control::On_Field_Del_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_SORT		, CVIEW_Table_Control::On_Field_Sort)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_SORT		, CVIEW_Table_Control::On_Field_Sort_UI)
	EVT_MENU					(ID_CMD_TABLE_FIELD_RENAME		, CVIEW_Table_Control::On_Field_Rename)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_RENAME		, CVIEW_Table_Control::On_Field_Rename_UI)

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

	Set_Labeling(false);

	CreateGrid(m_pTable->Get_Record_Count(), m_pTable->Get_Field_Count(), wxGrid::wxGridSelectRows);

	_Set_Table();
}

//---------------------------------------------------------
CVIEW_Table_Control::~CVIEW_Table_Control(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::Set_Labeling(bool bOn)
{
	if( bOn )
	{
		m_Field_Offset	= 1;

		SetRowLabelAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
	}
	else
	{
		m_Field_Offset	= 0;

		SetRowLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
	}
}

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
		case 0:		m_pTable->Set_Index(iField, TABLE_INDEX_None      );	break;
		case 1:		m_pTable->Set_Index(iField, TABLE_INDEX_Ascending );	break;
		case 2:		m_pTable->Set_Index(iField, TABLE_INDEX_Descending);	break;
		}

		_Set_Records();
	}
}

//---------------------------------------------------------
bool CVIEW_Table_Control::_Set_Table(void)
{
	int		Difference;

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
		Difference	= -Difference < GetNumberRows() ? -Difference : GetNumberRows();

		if( Difference > 0 )
		{
			DeleteRows(0, Difference);
		}
	}

	//-----------------------------------------------------
	Difference	= (m_pTable->Get_Field_Count() - m_Field_Offset) - GetNumberCols();

	if( Difference > 0 )
	{
		AppendCols(Difference);
	}
	else
	{
		Difference	= -Difference < GetNumberCols() ? -Difference : GetNumberCols();

		if( Difference > 0 )
		{
			DeleteCols(0, Difference);
		}	// here is (or was!?) a memory leak - solution: use own wxGridTableBase derived grid table class
	}

	//-----------------------------------------------------
	for(int iCol=0, iField=m_Field_Offset; iField<m_pTable->Get_Field_Count(); iCol++, iField++)
	{
		SetColLabelValue(iCol, m_pTable->Get_Field_Name(iField));

		switch( m_pTable->Get_Field_Type(iField) )
		{
		case TABLE_FIELDTYPE_Char:
		case TABLE_FIELDTYPE_String: default:
			break;

		case TABLE_FIELDTYPE_Short:
		case TABLE_FIELDTYPE_Int:
		case TABLE_FIELDTYPE_Long:
			SetColFormatNumber(iCol);
			break;

		case TABLE_FIELDTYPE_Float:
		case TABLE_FIELDTYPE_Double:
			SetColFormatFloat(iCol);
			break;

		case TABLE_FIELDTYPE_Color:
			SetColFormatNumber(iCol);
			break;
		}
	}

	//-----------------------------------------------------
	_Set_Records();

	//-----------------------------------------------------
	EndBatch();

	_Update_Views();

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
		if( m_Field_Offset )
		{
			SetRowLabelValue(iRecord, pRecord->asString(0));
		}

		for(int iCol=0, iField=m_Field_Offset; iField<m_pTable->Get_Field_Count(); iCol++, iField++)
		{
			switch( m_pTable->Get_Field_Type(iField) )
			{
			default:
				SET_CELL_VALUE(iRecord, iCol, pRecord->asString	(iField));
				break;

			case TABLE_FIELDTYPE_Color:
				SET_CELL_COLOR(iRecord, iCol, pRecord->asInt	(iField));
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
	if( !FIXED_ROWS && !m_pTable->is_Private() )
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
	if( !FIXED_ROWS && !m_pTable->is_Private() )
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
	if( !FIXED_ROWS && (!m_pTable->is_Private() || (m_pTable->Get_Owner() && m_pTable->Get_Owner()->Get_ObjectType() == DATAOBJECT_TYPE_Shapes)) )
	{
		wxArrayInt	Records	= GetSelectedRows();

		if( Records.GetCount() > 0 )
		{
			do
			{
				DeleteRows(Records[0]);
				Records	= GetSelectedRows();
			}
			while( Records.GetCount() > 0 );

			if( !m_pTable->is_Private() )
			{
				m_pTable->Del_Selection();

				g_pData->Update_Views(m_pTable);
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::Del_Records(void)
{
	if( !FIXED_ROWS && !m_pTable->is_Private() && m_pTable->Del_Records() )
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
void CVIEW_Table_Control::On_Size(wxSizeEvent &event)//&WXUNUSED(event))
{
	if( m_Field_Offset && GetNumberCols() )
	{
		SetColSize(0, GetClientSize().x - GetRowLabelSize());
	}

	event.Skip();
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Change(wxGridEvent &event)
{
	CSG_Table_Record	*pRecord;

	if( (pRecord = m_pTable->Get_Record_byIndex(event.GetRow())) != NULL )
	{
		int	iField	= m_Field_Offset + event.GetCol();

		if( iField >= m_Field_Offset && iField < m_pTable->Get_Field_Count() )
		{
			pRecord->Set_Value(iField, GetCellValue(event.GetRow(), event.GetCol()).c_str());

			SET_CELL_VALUE(event.GetRow(), event.GetCol(), pRecord->asString(iField));
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
	CSG_String		sFields, sOrder;
	CSG_Parameter	*pNode;
	CSG_Parameters	P;

	//-----------------------------------------------------
	for(int i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		sFields.Append(m_pTable->Get_Field_Name(i));	sFields.Append('|');
	}

	sOrder.Printf(wxT("%s|%s|%s|"),
		LNG("unsorted"),
		LNG("ascending"),
		LNG("descending")
	);

	//-----------------------------------------------------
	P.Set_Name(LNG("Sort Table"));

	pNode	= P.Add_Choice(NULL , "FIELD_1"	, LNG("Sort first by")	,	LNG(""),	sFields	, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(0));
	pNode	= P.Add_Choice(pNode, "ORDER_1"	, LNG("Direction")		,	LNG(""),	sOrder	, !m_pTable->is_Indexed() ? 1 : m_pTable->Get_Index_Order(0));

	pNode	= P.Add_Choice(NULL , "FIELD_2"	, LNG("Sort second by")	,	LNG(""),	sFields	, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(1));
	pNode	= P.Add_Choice(pNode, "ORDER_2"	, LNG("Direction")		,	LNG(""),	sOrder	, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Order(1));

	pNode	= P.Add_Choice(NULL , "FIELD_3"	, LNG("Sort third by")	,	LNG(""),	sFields	, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(2));
	pNode	= P.Add_Choice(pNode, "ORDER_3"	, LNG("Direction")		,	LNG(""),	sOrder	, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Order(2));

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

void CVIEW_Table_Control::On_Field_Sort_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Field_Count() > 0 && m_pTable->Get_Record_Count() > 1);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Field_Rename(wxCommandEvent &event)
{
	int				i;
	CSG_Parameters	P;

	P.Set_Name(LNG("Rename Fields"));

	for(i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		P.Add_String(NULL, "", m_pTable->Get_Field_Name(i), LNG(""), m_pTable->Get_Field_Name(i));
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
	}
}

void CVIEW_Table_Control::On_Field_Rename_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Field_Count() > 0);
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
	event.Enable(!FIXED_ROWS && !m_pTable->is_Private());
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Ins(wxCommandEvent &event)
{
	Ins_Record();
}

void CVIEW_Table_Control::On_Record_Ins_UI(wxUpdateUIEvent &event)
{
	event.Enable(!FIXED_ROWS && !m_pTable->is_Private());
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
	event.Enable(!FIXED_ROWS && !m_pTable->is_Private());
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
	CSG_Table_Record	*pRecord;

	if( (pRecord = m_pTable->Get_Record_byIndex(event.GetRow())) != NULL )
	{
		int	iField	= m_Field_Offset + event.GetCol();

		if( iField >= m_Field_Offset && iField < m_pTable->Get_Field_Count() )
		{
			long	lValue;

			switch( m_pTable->Get_Field_Type(iField) )
			{
			default:
				break;

			case TABLE_FIELDTYPE_Color:
				if( DLG_Color(lValue = pRecord->asInt(iField)) )
				{
					pRecord->Set_Value(iField, lValue);

					SET_CELL_COLOR(event.GetRow(), event.GetCol(), lValue);

					ForceRefresh();
				}
				return;
			}
		}
	}

	SelectRow(event.GetRow(), wxGetKeyState(WXK_CONTROL));

	SetGridCursor(event.GetRow(), event.GetCol());

//	event.Skip();
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
		SetGridCursor	(GetGridCursorRow(), event.GetCol());
	}
	else if( event.GetRow() >= 0 )
	{
		SetGridCursor	(event.GetRow(), GetGridCursorCol());
	}
	else
	{
	//	SelectAll();
	}
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_RClick_Label(wxGridEvent &event)
{
	wxMenu	*pMenu;

	//-----------------------------------------------------
	if( event.GetCol() != -1 )
	{
		pMenu	= new wxMenu(wxString::Format(wxT("%s"), LNG("Columns")), 0);

		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_FIELD_ADD);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_FIELD_DEL);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_AUTOSIZE_COLS);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_FIELD_SORT);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_FIELD_RENAME);

		PopupMenu(pMenu, event.GetPosition().x, event.GetPosition().y - GetColLabelSize());
		delete(pMenu);
	}

	//-----------------------------------------------------
	else if( event.GetRow() != -1 )
	{
		pMenu	= new wxMenu(wxString::Format(wxT("%s"), LNG("Rows")), 0);

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
	BeginBatch();

	for(int iRow=event.GetTopRow(); iRow<=event.GetBottomRow(); iRow++)
	{
		_Select(iRow, event.Selecting());
	}

	EndBatch();

	_Update_Views();

	event.Skip();
}

//---------------------------------------------------------
inline void CVIEW_Table_Control::_Select(int iRow, bool bSelect)
{
	CSG_Table_Record	*pRecord;

	if( (pRecord = m_pTable->Get_Record_byIndex(iRow)) != NULL && bSelect != pRecord->is_Selected() )
	{
		m_pTable->Select(pRecord, true);

		_Update_Views();
	}
}

//---------------------------------------------------------
void CVIEW_Table_Control::Update_Selection(void)
{
	BeginBatch();

	if( m_pTable->Get_Selection_Count() > 0 )
	{
		int		iRecord;
		bool	*Select	= (bool *)SG_Malloc(m_pTable->Get_Record_Count() * sizeof(bool));

		for(iRecord=0; iRecord<m_pTable->Get_Record_Count(); iRecord++)
		{
			Select[iRecord]	= m_pTable->Get_Record_byIndex(iRecord)->is_Selected();
		}

		ClearSelection();

		for(iRecord=0; iRecord<m_pTable->Get_Record_Count(); iRecord++)
		{
			if( Select[iRecord] )
			{
				SelectRow(iRecord, true);
			}
		}

		SG_Free(Select);
	}
	else
	{
		ClearSelection();
	}

	EndBatch();

	_Update_Views();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::_Update_Views(void)
{
	if( GetBatchCount() == 0 )
	{
		if( m_pTable->is_Private() )
		{
			g_pData->Update_Views(m_pTable->Get_Owner());
		}
		else if( m_pTable->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
		{
			g_pData->Get_Shapes()->Get_Shapes((CSG_Shapes *)m_pTable)->Update_Views(true);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
