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
	EVT_MENU					(ID_CMD_TABLE_FIELD_TYPE		, CVIEW_Table_Control::On_Field_Type)
	EVT_UPDATE_UI				(ID_CMD_TABLE_FIELD_TYPE		, CVIEW_Table_Control::On_Field_Type_UI)

	EVT_MENU					(ID_CMD_TABLE_RECORD_ADD		, CVIEW_Table_Control::On_Record_Add)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_ADD		, CVIEW_Table_Control::On_Record_Add_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_INS		, CVIEW_Table_Control::On_Record_Ins)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_INS		, CVIEW_Table_Control::On_Record_Ins_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_DEL		, CVIEW_Table_Control::On_Record_Del)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_DEL		, CVIEW_Table_Control::On_Record_Del_UI)
	EVT_MENU					(ID_CMD_TABLE_RECORD_DEL_ALL	, CVIEW_Table_Control::On_Record_Clr)
	EVT_UPDATE_UI				(ID_CMD_TABLE_RECORD_DEL_ALL	, CVIEW_Table_Control::On_Record_Clr_UI)

	EVT_MENU					(ID_CMD_TABLE_SELECTION_ONLY	, CVIEW_Table_Control::On_Sel_Only)
	EVT_UPDATE_UI				(ID_CMD_TABLE_SELECTION_ONLY	, CVIEW_Table_Control::On_Sel_Only_UI)

	EVT_MENU					(ID_CMD_TABLE_SELECTION_TO_TOP	, CVIEW_Table_Control::On_Sel_To_Top)
	EVT_UPDATE_UI				(ID_CMD_TABLE_SELECTION_TO_TOP	, CVIEW_Table_Control::On_Sel_To_Top_UI)

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
	m_pRecords		= NULL;
	m_Constraint	= Constraint;
	m_bUpdating		= false;
	m_bSelOnly		= false;

	Set_Labeling(false);

	CreateGrid(m_pTable->Get_Record_Count(), m_pTable->Get_Field_Count(), wxGrid::wxGridSelectRows);

	_Set_Table();
}

//---------------------------------------------------------
CVIEW_Table_Control::~CVIEW_Table_Control(void)
{
	if( m_pRecords )
	{
		SG_Free(m_pRecords);
	}
}


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
	BeginBatch();

	//-----------------------------------------------------
	int	Difference	= (m_pTable->Get_Field_Count() - m_Field_Offset) - GetNumberCols();

	if( Difference > 0 )
	{
		AppendCols(Difference);
	}
	else if( (Difference = -Difference < GetNumberCols() ? -Difference : GetNumberCols()) > 0 )
	{	// here is (or was!?) a memory leak - solution: use own wxGridTableBase derived grid table class
		DeleteCols(0, Difference);
	}

	//-----------------------------------------------------
	for(int iCol=0, iField=m_Field_Offset; iField<m_pTable->Get_Field_Count(); iCol++, iField++)
	{
		SetColLabelValue(iCol, m_pTable->Get_Field_Name(iField));

		switch( m_pTable->Get_Field_Type(iField) )
		{
		default:
		case SG_DATATYPE_Byte:
		case SG_DATATYPE_Char:
		case SG_DATATYPE_String:
		case SG_DATATYPE_Date:
		case SG_DATATYPE_Binary:
			break;

		case SG_DATATYPE_Bit:
		case SG_DATATYPE_Word:
		case SG_DATATYPE_Short:
		case SG_DATATYPE_DWord:
		case SG_DATATYPE_Int:
		case SG_DATATYPE_ULong:
		case SG_DATATYPE_Long:
		case SG_DATATYPE_Color:
			SetColFormatNumber(iCol);
			break;

		case SG_DATATYPE_Float:
		case SG_DATATYPE_Double:
			SetColFormatFloat(iCol);
			break;
		}
	}

	//-----------------------------------------------------
	_Set_Records();

	EndBatch();

	_Update_Views();

	return( true );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::_Set_Records(bool bSelection_To_Top)
{
	BeginBatch();

	m_bUpdating	= true;

	//-----------------------------------------------------
	if( m_bSelOnly && m_pTable->Get_Selection_Count() <= 0 )
	{
		m_bSelOnly	= false;
	}

	int	Difference, nRecords	= m_bSelOnly ? m_pTable->Get_Selection_Count() : m_pTable->Get_Count();

	if( (Difference = nRecords - GetNumberRows()) > 0 )
	{
		AppendRows(Difference);
	}
	else if( Difference < 0 && (Difference = -Difference < GetNumberRows() ? -Difference : GetNumberRows()) > 0 )
	{
		DeleteRows(0, Difference);
	}

	m_pRecords	= (CSG_Table_Record **)SG_Realloc(m_pRecords, nRecords * sizeof(CSG_Table_Record *));

	ClearSelection();

	//-----------------------------------------------------
	if( m_bSelOnly )
	{
	//	#pragma omp parallel for
		for(int iRecord=0; iRecord<nRecords; iRecord++)
		{
			_Set_Record(iRecord, m_pTable->Get_Selection(iRecord));
		}
	}
	else if( !bSelection_To_Top )
	{
	//	#pragma omp parallel for
		for(int iRecord=0; iRecord<nRecords; iRecord++)
		{
			_Set_Record(iRecord, m_pTable->Get_Record_byIndex(iRecord));
		}
	}
	else // if( bSelection_To_Top && m_pTable->Get_Selection_Count() > 0 )
	{
		for(int iRecord=0, iSel=0, iNoSel=m_pTable->Get_Selection_Count(); iRecord<nRecords && PROGRESSBAR_Set_Position(iRecord, nRecords); iRecord++)
		{
			CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(iRecord);

			if( pRecord->is_Selected() )
			{
				_Set_Record(iSel  ++, pRecord);
			}
			else
			{
				_Set_Record(iNoSel++, pRecord);
			}
		}

		PROCESS_Set_Okay();
	}

	//-----------------------------------------------------
	m_bUpdating	= false;

	EndBatch();

	return( true );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::_Set_Record(int iRecord, CSG_Table_Record *pRecord)
{
	m_pRecords[iRecord]	= pRecord;

	if( m_Field_Offset )
	{
		SetRowLabelValue(iRecord, pRecord->asString(0));
	}

	for(int iCol=0, iField=m_Field_Offset; iField<m_pTable->Get_Field_Count(); iCol++, iField++)
	{
		switch( m_pTable->Get_Field_Type(iField) )
		{
		default:
			SetCellValue(iRecord, iCol, pRecord->is_NoData(iField) ? SG_T("") : pRecord->asString(iField));
			break;

		case SG_DATATYPE_Color:
			SetCellBackgroundColour(iRecord, iCol, Get_Color_asWX(pRecord->asInt(iField)));
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Control::Add_Record(void)
{
	if( !FIXED_ROWS && !m_pTable->is_Private() && m_pTable->Get_ObjectType() == DATAOBJECT_TYPE_Table )
	{
		AppendRows();

		m_pRecords	= (CSG_Table_Record **)SG_Realloc(m_pRecords, GetNumberRows() * sizeof(CSG_Table_Record *));

		_Set_Record(GetNumberRows() - 1, m_pTable->Add_Record());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::Ins_Record(void)
{
	if( !FIXED_ROWS && !m_pTable->is_Private() && m_pTable->Get_ObjectType() == DATAOBJECT_TYPE_Table )
	{
		int		iRecord	= GetGridCursorRow();

		if( iRecord >= 0 && iRecord < GetNumberRows() )
		{
			InsertRows(iRecord);

			m_pRecords	= (CSG_Table_Record **)SG_Realloc(m_pRecords, GetNumberRows() * sizeof(CSG_Table_Record *));

			for(int i=GetNumberRows()-1; i>iRecord; i--)
			{
				m_pRecords[i]	= m_pRecords[i - 1];
			}

			_Set_Record(iRecord, m_pTable->Ins_Record(iRecord));

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::Del_Record(void)
{
	if( !FIXED_ROWS && !m_pTable->is_Private() )
	{
		m_pTable->Del_Selection();

		g_pData->Update_Views(m_pTable);

		Update_Table();

		return( true );
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
bool CVIEW_Table_Control::Load(const wxString &File_Name)
{
	CSG_Table	Table;

	if(	Table.Create(CSG_String(&File_Name)) && Table.Get_Count() > 0 && Table.Get_Field_Count() == m_pTable->Get_Field_Count() )
	{
		m_pTable->Assign_Values(&Table);

		_Set_Table();

		return( true );
	}

	PROCESS_Set_Okay();

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Control::Save(const wxString &File_Name, int Format)
{
	bool	bResult;

	bResult	= m_pTable->Save(CSG_String(&File_Name));

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
	int					iField		= m_Field_Offset + event.GetCol();
	CSG_Table_Record	*pRecord	= m_pRecords[event.GetRow()];

	if( pRecord && iField >= m_Field_Offset && iField < m_pTable->Get_Field_Count() )
	{
		pRecord->Set_Value(iField, GetCellValue(event.GetRow(), event.GetCol()).wx_str());

		SetCellValue(event.GetRow(), event.GetCol(), pRecord->asString(iField));
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
	P.Set_Name(_TL("Add Field"));

	P.Add_String(
		NULL	, "NAME"	, _TL("Name"),
		_TL(""),
		_TL("Field")
	);

	P.Add_Choice(
		NULL	, "TYPE"	, _TL("Field Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("character string"),
			_TL("1 byte integer"),
			_TL("2 byte integer"),
			_TL("4 byte integer"),
			_TL("4 byte floating point"),
			_TL("8 byte floating point"),
			_TL("date (dd.mm.yyyy)"),
			_TL("color (rgb)")
		)
	);

	P.Add_Choice(
		NULL	, "FIELD"	, _TL("Insert Position"),
		_TL(""),
		sFields, m_pTable->Get_Field_Count() - 1
	);

	P.Add_Choice(
		NULL	, "INSERT"	, _TL("Insert Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
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
		default:
		case 0:	Type	= SG_DATATYPE_String;	break;
		case 1:	Type	= SG_DATATYPE_Char;		break;
		case 2:	Type	= SG_DATATYPE_Short;	break;
		case 3:	Type	= SG_DATATYPE_Int;		break;
		case 4:	Type	= SG_DATATYPE_Float;	break;
		case 5:	Type	= SG_DATATYPE_Double;	break;
		case 6:	Type	= SG_DATATYPE_Date;		break;
		case 7:	Type	= SG_DATATYPE_Color;	break;
		}

		Position	= P("FIELD")->asInt() + P("INSERT")->asInt();

		m_pTable->Add_Field(P("NAME")->asString(), Type, Position);

		Update_Table();

		g_pData->Update(m_pTable, NULL);
	}
}

void CVIEW_Table_Control::On_Field_Add_UI(wxUpdateUIEvent &event)
{
	event.Enable(FIXED_COLS == false);
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
		P.Add_Value(NULL, SG_Get_String(i, 0), m_pTable->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, false);
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		for(i=m_pTable->Get_Field_Count()-1; i>=0; i--)
		{
			if( P(SG_Get_String(i, 0))->asBool() )
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

	sOrder.Printf(SG_T("%s|%s|%s|"),
		_TL("unsorted"),
		_TL("ascending"),
		_TL("descending")
	);

	//-----------------------------------------------------
	P.Set_Name(_TL("Sort Table"));

	pNode	= P.Add_Choice(NULL , "FIELD_1"	, _TL("Sort first by")	,	_TL(""),	sFields	, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(0));
	pNode	= P.Add_Choice(pNode, "ORDER_1"	, _TL("Direction")		,	_TL(""),	sOrder	, !m_pTable->is_Indexed() ? 1 : m_pTable->Get_Index_Order(0));

	pNode	= P.Add_Choice(NULL , "FIELD_2"	, _TL("Sort second by")	,	_TL(""),	sFields	, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(1));
	pNode	= P.Add_Choice(pNode, "ORDER_2"	, _TL("Direction")		,	_TL(""),	sOrder	, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Order(1));

	pNode	= P.Add_Choice(NULL , "FIELD_3"	, _TL("Sort third by")	,	_TL(""),	sFields	, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Field(2));
	pNode	= P.Add_Choice(pNode, "ORDER_3"	, _TL("Direction")		,	_TL(""),	sOrder	, !m_pTable->is_Indexed() ? 0 : m_pTable->Get_Index_Order(2));

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

	P.Set_Name(_TL("Rename Fields"));

	for(i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		P.Add_String(NULL, "", m_pTable->Get_Field_Name(i), _TL(""), m_pTable->Get_Field_Name(i));
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
		case SG_DATATYPE_String:	Types[i]	= 0;	break;
		case SG_DATATYPE_Char:		Types[i]	= 1;	break;
		case SG_DATATYPE_Short:		Types[i]	= 2;	break;
		case SG_DATATYPE_Int:		Types[i]	= 3;	break;
		case SG_DATATYPE_Float:	    Types[i]	= 4;	break;
		case SG_DATATYPE_Double:	Types[i]	= 5;	break;
		case SG_DATATYPE_Date:	    Types[i]	= 6;	break;
		case SG_DATATYPE_Binary:	Types[i]	= 7;	break;
		}

        P.Add_Choice(NULL, "", m_pTable->Get_Field_Name(i), _TL(""),
            CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|"),
                _TL("character string"),
                _TL("1 byte integer"),
                _TL("2 byte integer"),
                _TL("4 byte integer"),
                _TL("4 byte floating point"),
                _TL("8 byte floating point"),
                _TL("date (dd.mm.yyyy)"),
                _TL("color (rgb)")
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
			default:
			case 0:	Type	= SG_DATATYPE_String;	break;
            case 1:	Type	= SG_DATATYPE_Char;		break;
            case 2:	Type	= SG_DATATYPE_Short;	break;
            case 3:	Type	= SG_DATATYPE_Int;		break;
            case 4:	Type	= SG_DATATYPE_Float;	break;
            case 5:	Type	= SG_DATATYPE_Double;	break;
            case 6:	Type	= SG_DATATYPE_Date;		break;
            case 7:	Type	= SG_DATATYPE_Color;	break;
			}

			if( Type != Types[i] )
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
	event.Enable(!FIXED_ROWS && !m_pTable->is_Private() && m_pTable->Get_ObjectType() == DATAOBJECT_TYPE_Table);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Ins(wxCommandEvent &event)
{
	Ins_Record();
}

void CVIEW_Table_Control::On_Record_Ins_UI(wxUpdateUIEvent &event)
{
	event.Enable(!FIXED_ROWS && !m_pTable->is_Private() && m_pTable->Get_ObjectType() == DATAOBJECT_TYPE_Table);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Del(wxCommandEvent &event)
{
	Del_Record();
}

void CVIEW_Table_Control::On_Record_Del_UI(wxUpdateUIEvent &event)
{
	event.Enable(!FIXED_ROWS && !m_pTable->is_Private() && m_pTable->Get_Selection_Count() > 0);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Record_Clr(wxCommandEvent &event)
{
	Del_Records();
}

void CVIEW_Table_Control::On_Record_Clr_UI(wxUpdateUIEvent &event)
{
	event.Enable(!FIXED_ROWS && !m_pTable->is_Private() && !m_pTable->is_Private());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Control::On_Sel_To_Top(wxCommandEvent  &event)
{
	if( m_pTable->Get_Selection_Count() > 0 )
	{
		_Set_Records(true);
	}
}

void CVIEW_Table_Control::On_Sel_To_Top_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pTable->Get_Selection_Count() > 0);
}

//---------------------------------------------------------
void CVIEW_Table_Control::On_Sel_Only(wxCommandEvent  &event)
{
	if( m_pTable->Get_Selection_Count() > 0 && m_pTable->Get_Selection_Count() < GetNumberRows() )
	{
		m_bSelOnly	= true;

		_Set_Records();
	}
	else if( m_bSelOnly )
	{
		m_bSelOnly	= false;

		_Set_Records();
	}
}

void CVIEW_Table_Control::On_Sel_Only_UI(wxUpdateUIEvent &event)
{
	event.Check(m_bSelOnly);
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
	int					iField		= m_Field_Offset + event.GetCol();
	CSG_Table_Record	*pRecord	= m_pRecords[event.GetRow()];

	//-----------------------------------------------------
	if( event.ControlDown() )
	{
		m_pTable->Select(pRecord, true);

		Update_Selection();
	}

	else if( event.ShiftDown() )
	{
		SelectBlock(event.GetRow(), 0, GetGridCursorRow(), GetNumberCols(), false);
	}

	else if( event.AltDown() )
	{
		if( m_pTable->Get_Field_Type(iField) == SG_DATATYPE_String )
		{
			Open_Application(pRecord->asString(iField));
		}
	}

	else
	{
		SelectRow(event.GetRow(), false);

		if( pRecord && iField >= m_Field_Offset && iField < m_pTable->Get_Field_Count() && m_pTable->Get_Field_Type(iField) == SG_DATATYPE_Color )
		{
			long	lValue;

			if( DLG_Color(lValue = pRecord->asInt(iField)) )
			{
				pRecord->Set_Value(iField, lValue);

				SetCellBackgroundColour(event.GetRow(), event.GetCol(), Get_Color_asWX(pRecord->asInt(iField)));

				ForceRefresh();
			}
		}
	}

	//-----------------------------------------------------
	SetGridCursor(event.GetRow(), event.GetCol());
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
	wxMenu	Menu;

	//-----------------------------------------------------
	if( event.GetCol() != -1 )
	{
		Menu.SetTitle(wxString::Format(wxT("%s"), _TL("Columns")));

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_ADD);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_DEL);

		Menu.AppendSeparator();
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_AUTOSIZE_COLS);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_SORT);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_RENAME);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_TYPE);

		PopupMenu(&Menu, event.GetPosition().x, event.GetPosition().y - GetColLabelSize());
	}

	//-----------------------------------------------------
	else if( event.GetRow() != -1 )
	{
		Menu.SetTitle(wxString::Format(wxT("%s"), _TL("Rows")));

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_ADD);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_INS);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_DEL);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_RECORD_DEL_ALL);

		Menu.AppendSeparator();
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_AUTOSIZE_ROWS);

		if( !FIXED_ROWS )
		{
			Menu.AppendSeparator();
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_SELECTION_TO_TOP);
			CMD_Menu_Add_Item(&Menu,  true, ID_CMD_TABLE_SELECTION_ONLY);
		}

		PopupMenu(&Menu, event.GetPosition().x - GetRowLabelSize(), event.GetPosition().y);
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
	if( !m_bUpdating )
	{
		BeginBatch();

		for(int iRow=event.GetTopRow(); iRow<=event.GetBottomRow(); iRow++)
		{
			if( m_pRecords[iRow]->is_Selected() != event.Selecting() )
			{
				m_pTable->Select(m_pRecords[iRow], true);
			}
		}

		EndBatch();

		_Update_Views();
	}

	event.Skip();
}

//---------------------------------------------------------
void CVIEW_Table_Control::Update_Selection(void)
{
	if( !m_bUpdating )
	{
		m_bUpdating	= true;

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
					if( m_pRecords[iRecord]->is_Selected() )
					{
						SelectRow(iRecord, true);
					}
				}
			}
		}

		EndBatch();

		m_bUpdating	= false;

		_Update_Views();
	}
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
