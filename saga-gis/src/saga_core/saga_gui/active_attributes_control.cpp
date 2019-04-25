
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
	EVT_SIZE					(CActive_Attributes_Control::On_Size)
	EVT_KEY_DOWN				(CActive_Attributes_Control::On_Key)

	EVT_GRID_EDITOR_SHOWN		(CActive_Attributes_Control::On_Edit_Start)
	EVT_GRID_EDITOR_HIDDEN		(CActive_Attributes_Control::On_Edit_Stop)
	EVT_GRID_CELL_CHANGED		(CActive_Attributes_Control::On_Changed)
	EVT_GRID_CELL_LEFT_CLICK	(CActive_Attributes_Control::On_LClick)
	EVT_GRID_CELL_RIGHT_CLICK	(CActive_Attributes_Control::On_RClick)
	EVT_GRID_LABEL_LEFT_CLICK	(CActive_Attributes_Control::On_LClick_Label)
	EVT_GRID_LABEL_RIGHT_CLICK	(CActive_Attributes_Control::On_RClick_Label)

	EVT_MENU					(ID_CMD_TABLE_FIELD_ADD			, CActive_Attributes_Control::On_Field_Add)
	EVT_MENU					(ID_CMD_TABLE_FIELD_DEL			, CActive_Attributes_Control::On_Field_Del)
	EVT_MENU					(ID_CMD_TABLE_FIELD_RENAME		, CActive_Attributes_Control::On_Field_Rename)
	EVT_MENU					(ID_CMD_TABLE_FIELD_TYPE		, CActive_Attributes_Control::On_Field_Type)

	EVT_MENU					(ID_CMD_TABLE_FIELD_OPEN_APP	, CActive_Attributes_Control::On_Field_Open)
	EVT_MENU					(ID_CMD_TABLE_FIELD_OPEN_DATA	, CActive_Attributes_Control::On_Field_Open)

	EVT_MENU					(ID_CMD_TABLE_AUTOSIZE_COLS		, CActive_Attributes_Control::On_Autosize_Cols)
	EVT_MENU					(ID_CMD_TABLE_AUTOSIZE_ROWS		, CActive_Attributes_Control::On_Autosize_Rows)

	EVT_MENU					(ID_CMD_TABLE_TO_CLIPBOARD		, CActive_Attributes_Control::On_ToClipboard)
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
	m_pTable	= new CSG_Table;
	m_bEditing	= false;

	SetRowLabelAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);

	Set_Row_Labeling(false);

	CreateGrid(0, m_pTable->Get_Field_Count());

	Update_Table();
}

//---------------------------------------------------------
CActive_Attributes_Control::~CActive_Attributes_Control(void)
{
	delete(m_pTable);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes_Control::Set_Row_Labeling(bool bOn)
{
	if( bOn && m_Field_Offset == 0 )
	{
		m_Field_Offset	= 1;	// feature attributes
	}
	else if( !bOn && m_Field_Offset != 0 )
	{
		m_Field_Offset	= 0;	// grid cell values
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CActive_Attributes_Control::Update_Table(void)
{
	if( GetBatchCount() > 0 )
	{
		return( false );
	}

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
			SetColFormatCustom(iCol, wxGRID_VALUE_STRING);
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
	if( (Difference = m_pTable->Get_Count() - GetNumberRows()) > 0 )
	{
		AppendRows(Difference);
	}
	else if( Difference < 0 && (Difference = -Difference < GetNumberRows() ? -Difference : GetNumberRows()) > 0 )
	{
		DeleteRows(0, Difference);
	}

	//-------------------------------------------------
	for(int iRecord=0; iRecord<m_pTable->Get_Count(); iRecord++)
	{
		_Set_Record(iRecord);
	}

	//-----------------------------------------------------
	if( GetNumberCols() > 0 && GetNumberRows() > 0 )
	{
		SetRowLabelSize(wxGRID_AUTOSIZE);

		if( m_Field_Offset )	// feature attributes
		{
			if( GetClientSize().x > GetRowLabelSize() )
			{
				SetColSize(0, GetClientSize().x - GetRowLabelSize());
			}
		}
		else					// grid cell values
		{
			AutoSizeColumns();
		}
	}

	Show(GetNumberRows() > 0);

	m_pTable->Set_Modified(false);

	//-----------------------------------------------------
	EndBatch();

	return( true );
}

//---------------------------------------------------------
bool CActive_Attributes_Control::_Set_Record(int iRecord)
{
	CSG_Table_Record *pRecord	= m_pTable->Get_Record(iRecord);

	if( m_Field_Offset )	// feature attributes
	{
		SetRowLabelValue(iRecord, pRecord->asString(0));
	}
	else					// grid cell values
	{
		SetRowLabelValue(iRecord, wxString::Format("%d", 1 + iRecord));
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes_Control::On_Size(wxSizeEvent &event)//&WXUNUSED(event))
{
	if( m_Field_Offset )	// feature attributes
	{
		if( GetNumberCols() > 0 && GetClientSize().x > GetRowLabelSize() )
		{
			SetColSize(0, GetClientSize().x - GetRowLabelSize());
		}
	}

	event.Skip();
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_Key(wxKeyEvent &event)
{
	event.Skip(true);

	if( event.GetKeyCode() == WXK_RETURN && !m_bEditing )
	{
		event.Skip(false);

		((CActive_Attributes *)GetParent())->Save_Changes(false);
	}
}

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
{
	CSG_Table_Record	*pRecord	= m_pTable->Get_Record(event.GetRow());

	if( pRecord )
	{
		int	iField	= m_Field_Offset + event.GetCol();

		if( iField >= m_Field_Offset && iField < m_pTable->Get_Field_Count() )
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
void CActive_Attributes_Control::On_Field_Add(wxCommandEvent &event)
{
	if( !g_pActive->Get_Active_Data_Item() || g_pActive->Get_Active_Data_Item()->Get_Type() != WKSP_ITEM_Grids )	{	return;	}

	CSG_Grids	*pGrids	= ((CWKSP_Grids *)g_pActive->Get_Active_Data_Item())->Get_Grids();

	//-----------------------------------------------------
	CSG_String	Fields;

	for(int i=0; i<m_pTable->Get_Field_Count(); i++)
	{
		Fields	+= m_pTable->Get_Field_Name(i) + CSG_String('|');
	}

	//-----------------------------------------------------
	CSG_Parameters	P(NULL, _TL("Add Field"), SG_T(""));

	P.Add_String(
		NULL	, "NAME"	, _TL("Name"),
		_TL(""),
		_TL("Field")
	);

	P.Add_Choice(
		NULL	, "TYPE"	, _TL("Field Type"),
		_TL(""),
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
		), 0
	);

	P.Add_Choice(
		NULL	, "FIELD"	, _TL("Insert Position"),
		_TL(""),
		Fields, m_pTable->Get_Field_Count() - 1
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
		case  0:	Type	= SG_DATATYPE_String;	break;
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

		pGrids->Add_Attribute(P("NAME")->asString(), Type, Position);

		g_pData->Update(pGrids, NULL);
	}
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_Field_Del(wxCommandEvent &event)
{
	if( !g_pActive->Get_Active_Data_Item() || g_pActive->Get_Active_Data_Item()->Get_Type() != WKSP_ITEM_Grids )	{	return;	}

	CSG_Grids	*pGrids	= ((CWKSP_Grids *)g_pActive->Get_Active_Data_Item())->Get_Grids();

	//-----------------------------------------------------
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
				pGrids->Get_Attributes_Ptr()->Del_Field(i);

				DeleteCols(i);
			}
		}

		g_pData->Update(pGrids, NULL);
	}
}

//---------------------------------------------------------
void CActive_Attributes_Control::On_Field_Rename(wxCommandEvent &event)
{
	if( !g_pActive->Get_Active_Data_Item() || g_pActive->Get_Active_Data_Item()->Get_Type() != WKSP_ITEM_Grids )	{	return;	}

	CSG_Grids	*pGrids	= ((CWKSP_Grids *)g_pActive->Get_Active_Data_Item())->Get_Grids();

	//-----------------------------------------------------
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
	if( !g_pActive->Get_Active_Data_Item() || g_pActive->Get_Active_Data_Item()->Get_Type() != WKSP_ITEM_Grids )	{	return;	}

	CSG_Grids	*pGrids	= ((CWKSP_Grids *)g_pActive->Get_Active_Data_Item())->Get_Grids();

	//-----------------------------------------------------
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
				pGrids->Get_Attributes_Ptr()->Set_Field_Type(i, Type);

				bChanged	= true;
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes_Control::On_ToClipboard(wxCommandEvent &event)
{
	_ToClipboard();
}

//---------------------------------------------------------
bool CActive_Attributes_Control::_ToClipboard(void)
{
	if( wxTheClipboard->Open() )
	{
		wxString	Data;

		for(int i=0; i<m_pTable->Get_Count(); i++)
		{
			CSG_Table_Record	*pRecord	= m_pTable->Get_Record(i);

			for(int j=0; j<m_pTable->Get_Field_Count(); j++)
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
void CActive_Attributes_Control::On_LClick(wxGridEvent &event)
{
	int					iField		= m_Field_Offset + event.GetCol();
	CSG_Table_Record	*pRecord	= m_pTable->Get_Record(event.GetRow());

	//-----------------------------------------------------
	if( event.AltDown() )
	{
		if( m_pTable->Get_Field_Type(iField) == SG_DATATYPE_String )
		{
			Open_Application(pRecord->asString(iField));
		}
	}

	//-----------------------------------------------------
	if( event.ControlDown() )
	{
		if( m_pTable->Get_Field_Type(iField) == SG_DATATYPE_String )
		{
			g_pData->Open   (pRecord->asString(iField));
		}
	}

	//-----------------------------------------------------
	else
	{
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

//---------------------------------------------------------
void CActive_Attributes_Control::On_RClick(wxGridEvent &event)
{
	SetGridCursor(event.GetRow(), event.GetCol());

	int					iField		= m_Field_Offset + event.GetCol();
	CSG_Table_Record	*pRecord	= m_pTable->Get_Record(event.GetRow());

	//-----------------------------------------------------
	if( m_pTable->Get_Field_Type(iField) == SG_DATATYPE_String )
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
void CActive_Attributes_Control::On_Field_Open(wxCommandEvent &event)
{
	int	iField	= m_Field_Offset + GetGridCursorCol();
	int	iRecord	=                  GetGridCursorRow();

	if( iField >= 0 && iField<m_pTable->Get_Field_Count() && iRecord >= 0 && iRecord < m_pTable->Get_Count() )
	{
		CSG_Table_Record	*pRecord	= m_pTable->Get_Record(iRecord);

		if( event.GetId() == ID_CMD_TABLE_FIELD_OPEN_APP )
		{
			Open_Application(pRecord->asString(iField));
		}

		if( event.GetId() == ID_CMD_TABLE_FIELD_OPEN_DATA )
		{
			wxString	Value	= pRecord->asString(iField);

			if( _Get_DataSource(Value) )
			{
				g_pData->Open   (Value);
			}
		}
	}
}

//---------------------------------------------------------
bool CActive_Attributes_Control::_Get_DataSource(wxString &Source)
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

	if( g_pActive->Get_Active_Data_Item() && g_pActive->Get_Active_Data_Item()->Get_Object()->Get_File_Name(false) )
	{
		wxFileName	fn(Source), dir(g_pActive->Get_Active_Data_Item()->Get_Object()->Get_File_Name(false));

		if( fn.MakeAbsolute(dir.GetPath()) && fn.Exists() )
		{
			Source	= fn.GetFullPath();

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
