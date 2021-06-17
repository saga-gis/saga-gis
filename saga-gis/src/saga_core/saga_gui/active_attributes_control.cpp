
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
//            active_attributes_control.cpp              //
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
#include <wx/filename.h>
#include <wx/clipbrd.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "view_table_data.h"

#include "active.h"
#include "active_attributes.h"
#include "active_attributes_control.h"

#include "helper.h"

#include "wksp_data_manager.h"
#include "wksp_data_item.h"

#include "wksp_grids.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CActive_Attributes_Control, wxGrid)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CActive_Attributes_Control, wxGrid)
	EVT_KEY_DOWN              (CActive_Attributes_Control::On_Key         )

	EVT_GRID_EDITOR_SHOWN     (CActive_Attributes_Control::On_Edit_Start  )
	EVT_GRID_EDITOR_HIDDEN    (CActive_Attributes_Control::On_Edit_Stop   )
	EVT_GRID_CELL_CHANGED     (CActive_Attributes_Control::On_Changed     )

	EVT_GRID_CELL_LEFT_CLICK  (CActive_Attributes_Control::On_LClick      )
	EVT_GRID_LABEL_LEFT_CLICK (CActive_Attributes_Control::On_LClick_Label)
	EVT_GRID_CELL_RIGHT_CLICK (CActive_Attributes_Control::On_RClick      )
	EVT_GRID_LABEL_RIGHT_CLICK(CActive_Attributes_Control::On_RClick_Label)

	EVT_MENU(ID_CMD_TABLE_AUTOSIZE_COLS  , CActive_Attributes_Control::On_Autosize_Cols)
	EVT_MENU(ID_CMD_TABLE_AUTOSIZE_ROWS  , CActive_Attributes_Control::On_Autosize_Rows)

	EVT_MENU(ID_CMD_TABLE_FIELD_OPEN_APP , CActive_Attributes_Control::On_Cell_Open    )
	EVT_MENU(ID_CMD_TABLE_FIELD_OPEN_DATA, CActive_Attributes_Control::On_Cell_Open    )

	EVT_MENU(ID_CMD_TABLE_TO_CLIPBOARD   , CActive_Attributes_Control::On_ToClipboard  )

	EVT_MENU(ID_CMD_TABLE_FIELD_ADD      , CActive_Attributes_Control::On_Field_Add    )
	EVT_MENU(ID_CMD_TABLE_FIELD_DEL      , CActive_Attributes_Control::On_Field_Del    )
	EVT_MENU(ID_CMD_TABLE_FIELD_RENAME   , CActive_Attributes_Control::On_Field_Rename )
	EVT_MENU(ID_CMD_TABLE_FIELD_TYPE     , CActive_Attributes_Control::On_Field_Type   )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CActive_Attributes_Control::CActive_Attributes_Control(wxWindow *pParent)
	: wxGrid(pParent, -1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS)
{
	m_pData = new CVIEW_Table_Data(&m_Table);

	m_bEditing	= false;

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

	SetTable(m_pData, true, wxGrid::wxGridSelectNone);
#else
	SetTable(m_pData, true, wxGrid::wxGridSelectCells);
#endif

	Set_Row_Labeling(false);

	Update_Table();
}

//---------------------------------------------------------
CActive_Attributes_Control::~CActive_Attributes_Control(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes_Control::Set_Row_Labeling(bool bOn)
{
	if(  bOn && m_pData->m_bRowLabels == false )
	{
		m_pData->m_bRowLabels	=  true; // feature attributes
	}

	if( !bOn && m_pData->m_bRowLabels == true  )
	{
		m_pData->m_bRowLabels	= false; // grid cell values
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CActive_Attributes_Control::Update_Table(void)
{
	return( GetBatchCount() == 0 && _Update_Records() );
}

//---------------------------------------------------------
bool CActive_Attributes_Control::_Update_Records(void)
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
	if( GetNumberCols() > 0 )
	{
		AutoSizeColumns();
	}

	if( GetNumberRows() > 0 )
	{
		SetRowLabelSize(wxGRID_AUTOSIZE);
	}

	Enable(GetNumberRows() > 0);

	m_Table.Set_Modified(false);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes_Control::On_Key(wxKeyEvent &event)
{
	if( event.GetKeyCode() == WXK_RETURN && !m_bEditing )
	{
		event.Skip(false);

		((CActive_Attributes *)GetParent())->Save_Changes(false);
	}
	else
	{
		event.Skip(true);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes_Control::On_Edit_Start(wxGridEvent &event)
{
	m_bEditing	= true;

	event.Skip();
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_Edit_Stop(wxGridEvent &event)
{
	m_bEditing	= false;

	event.Skip();
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_Changed(wxGridEvent &event)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes_Control::On_LClick(wxGridEvent &event)
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
void CActive_Attributes_Control::On_LClick_Label(wxGridEvent &event)
{
	if( event.GetCol() >= 0 )
	{
		SetGridCursor(GetGridCursorRow(), event.GetCol());
	}
	else if( event.GetRow() >= 0 )
	{
		SetGridCursor(event.GetRow(), GetGridCursorCol());
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes_Control::On_RClick(wxGridEvent &event)
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
	}
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_RClick_Label(wxGridEvent &event)
{
	wxMenu	Menu;

	//-----------------------------------------------------
	if( event.GetCol() != -1 )
	{
		Menu.SetTitle(_TL("Columns"));

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_TO_CLIPBOARD);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_AUTOSIZE_COLS);

		if( g_pActive->Get_Active_Data_Item() && g_pActive->Get_Active_Data_Item()->Get_Type() == WKSP_ITEM_Grids )
		{
			Menu.AppendSeparator();

			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_ADD);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_DEL);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_RENAME);
			CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_FIELD_TYPE);
		}
	}
	else if( event.GetRow() != -1 )
	{
		Menu.SetTitle(_TL("Rows"));

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_TO_CLIPBOARD);
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_AUTOSIZE_ROWS);
	}
	else
	{
		Menu.SetTitle(_TL("Attributes"));

		CMD_Menu_Add_Item(&Menu, false, ID_CMD_TABLE_TO_CLIPBOARD);
	}

	//-----------------------------------------------------
	PopupMenu(&Menu, event.GetPosition().x - GetRowLabelSize(), event.GetPosition().y);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes_Control::On_Autosize_Cols(wxCommandEvent &event)
{
	AutoSizeColumns(false);
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_Autosize_Rows(wxCommandEvent &event)
{
	AutoSizeRows(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes_Control::On_Cell_Open(wxCommandEvent &event)
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
void CActive_Attributes_Control::On_ToClipboard(wxCommandEvent &event)
{
	m_pData->To_Clipboard();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grids * CActive_Attributes_Control::_Get_Grids(void)
{
	if( g_pActive->Get_Active_Data_Item() && g_pActive->Get_Active_Data_Item()->Get_Type() == WKSP_ITEM_Grids )
	{
		return( ((CWKSP_Grids *)g_pActive->Get_Active_Data_Item())->Get_Grids() );
	}

	return( NULL );
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_Field_Add(wxCommandEvent &event)
{
	CSG_Grids *pGrids = _Get_Grids(); if( !pGrids ) { return; }

	//-----------------------------------------------------
	CSG_String	Fields;

	for(int i=0; i<m_Table.Get_Field_Count(); i++)
	{
		Fields	+= m_Table.Get_Field_Name(i) + CSG_String('|');
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
		Fields, m_Table.Get_Field_Count() - 1
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
		TSG_Data_Type Type;

		switch( P("TYPE")->asInt() )
		{
		default: Type = SG_DATATYPE_String; break;
		case  1: Type = SG_DATATYPE_Date  ; break;
		case  2: Type = SG_DATATYPE_Color ; break;
		case  3: Type = SG_DATATYPE_Byte  ; break;
		case  4: Type = SG_DATATYPE_Char  ; break;
		case  5: Type = SG_DATATYPE_Word  ; break;
		case  6: Type = SG_DATATYPE_Short ; break;
		case  7: Type = SG_DATATYPE_DWord ; break;
		case  8: Type = SG_DATATYPE_Int   ; break;
		case  9: Type = SG_DATATYPE_ULong ; break;
		case 10: Type = SG_DATATYPE_Long  ; break;
		case 11: Type = SG_DATATYPE_Float ; break;
		case 12: Type = SG_DATATYPE_Double; break;
		case 13: Type = SG_DATATYPE_Binary; break;
		}

		int Position	= P("FIELD")->asInt() + P("INSERT")->asInt();

		pGrids->Add_Attribute(P("NAME")->asString(), Type, Position);

		g_pData->Update(pGrids, NULL);
	}
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_Field_Del(wxCommandEvent &event)
{
	CSG_Grids *pGrids = _Get_Grids(); if( !pGrids ) { return; }

	//-----------------------------------------------------
	CSG_Parameters	P(_TL("Delete Fields"));

	for(int i=0; i<m_Table.Get_Field_Count(); i++)
	{
		P.Add_Bool("", SG_Get_String(i), m_Table.Get_Field_Name(i), _TL(""), false);
	}

	if( DLG_Parameters(&P) )
	{
		bool bChanged = false;

		for(int i=m_Table.Get_Field_Count()-1; i>=0; i--)
		{
			if( P(SG_Get_String(i))->asBool() )
			{
				pGrids->Get_Attributes_Ptr()->Del_Field(i);

				bChanged = true;
			}
		}

		if( bChanged )
		{
			g_pData->Update(pGrids, NULL);
		}
	}
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_Field_Rename(wxCommandEvent &event)
{
	CSG_Grids *pGrids = _Get_Grids(); if( !pGrids ) { return; }

	//-----------------------------------------------------
	CSG_Parameters P(_TL("Rename Fields"));

	for(int i=0; i<m_Table.Get_Field_Count(); i++)
	{
		P.Add_String("", SG_Get_String(i), m_Table.Get_Field_Name(i), _TL(""), m_Table.Get_Field_Name(i));
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		for(int i=0; i<m_Table.Get_Field_Count(); i++)
		{
			CSG_String	s(P(i)->asString());

			if( s.Length() > 0 && s.Cmp(m_Table.Get_Field_Name(i)) )
			{
				pGrids->Get_Attributes_Ptr()->Set_Field_Name(i, s);

				SetColLabelValue(i, s.c_str());
			}
		}

		g_pData->Update(pGrids, NULL);
	}
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_Field_Type(wxCommandEvent &event)
{
	CSG_Grids *pGrids = _Get_Grids(); if( !pGrids ) { return; }

	//-----------------------------------------------------
	CSG_Parameters P(_TL("Change Field Type"));

	CSG_Array_Int Types(m_Table.Get_Field_Count());

	for(int i=0; i<m_Table.Get_Field_Count(); i++)
	{
		switch( m_Table.Get_Field_Type(i) )
		{
		default:
		case SG_DATATYPE_String: Types[i] =  0; break;
		case SG_DATATYPE_Date  : Types[i] =  1; break;
		case SG_DATATYPE_Color : Types[i] =  2; break;
		case SG_DATATYPE_Byte  : Types[i] =  3; break;
		case SG_DATATYPE_Char  : Types[i] =  4; break;
		case SG_DATATYPE_Word  : Types[i] =  5; break;
		case SG_DATATYPE_Short : Types[i] =  6; break;
		case SG_DATATYPE_DWord : Types[i] =  7; break;
		case SG_DATATYPE_Int   : Types[i] =  8; break;
		case SG_DATATYPE_ULong : Types[i] =  9; break;
		case SG_DATATYPE_Long  : Types[i] = 10; break;
		case SG_DATATYPE_Float : Types[i] = 11; break;
		case SG_DATATYPE_Double: Types[i] = 12; break;
		case SG_DATATYPE_Binary: Types[i] = 13; break;
		}

		P.Add_Choice("", SG_Get_String(i), m_Table.Get_Field_Name(i), _TL(""),
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
			), Types[i]
		);
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&P) )
	{
		bool bChanged = false;

		for(int i=0; i<m_Table.Get_Field_Count(); i++)
		{
			TSG_Data_Type	Type;

			switch( P(i)->asInt() )
			{
			default: Type = SG_DATATYPE_String; break;
			case  1: Type = SG_DATATYPE_Date  ; break;
			case  2: Type = SG_DATATYPE_Color ; break;
			case  3: Type = SG_DATATYPE_Byte  ; break;
			case  4: Type = SG_DATATYPE_Char  ; break;
			case  5: Type = SG_DATATYPE_Word  ; break;
			case  6: Type = SG_DATATYPE_Short ; break;
			case  7: Type = SG_DATATYPE_DWord ; break;
			case  8: Type = SG_DATATYPE_Int   ; break;
			case  9: Type = SG_DATATYPE_ULong ; break;
			case 10: Type = SG_DATATYPE_Long  ; break;
			case 11: Type = SG_DATATYPE_Float ; break;
			case 12: Type = SG_DATATYPE_Double; break;
			case 13: Type = SG_DATATYPE_Binary; break;
			}

			if( Type != m_Table.Get_Field_Type(i) )
			{
				pGrids->Get_Attributes_Ptr()->Set_Field_Type(i, Type);

				bChanged = true;
			}
		}

		if( bChanged )
		{
			Update_Table();

			g_pData->Update(pGrids, NULL);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
