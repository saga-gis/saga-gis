
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
//                  VIEW_Table_Data.cpp                  //
//                                                       //
//          Copyright (C) 2021 by Olaf Conrad            //
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

#include "helper.h"

#include "wksp_data_manager.h"

#include "view_table_data.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Table_Data::CVIEW_Table_Data(CSG_Table *pTable)
{
	m_pTable     = pTable;

	m_bSelection = false;
	m_bRowLabels = false;
}

//---------------------------------------------------------
CVIEW_Table_Data::~CVIEW_Table_Data(void)
{
	// nop
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Data::InsertRows			(size_t Position, size_t Number)
{
	wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_INSERTED, Position, Number);

	return( GetView()->ProcessTableMessage(msg) );
}

//---------------------------------------------------------
bool CVIEW_Table_Data::AppendRows			(                 size_t Number)
{
	wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_APPENDED,           Number);

	return( GetView()->ProcessTableMessage(msg) );
}

//---------------------------------------------------------
bool CVIEW_Table_Data::DeleteRows			(size_t Position, size_t Number)
{
	wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_DELETED , Position, Number);

	return( GetView()->ProcessTableMessage(msg) );
}

//---------------------------------------------------------
bool CVIEW_Table_Data::InsertCols			(size_t Position, size_t Number)
{
	wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_COLS_INSERTED, Position, Number);

	return( GetView()->ProcessTableMessage(msg) );
}

//---------------------------------------------------------
bool CVIEW_Table_Data::AppendCols			(                 size_t Number)
{
	wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_COLS_APPENDED,           Number);

	return( GetView()->ProcessTableMessage(msg) );
}

//---------------------------------------------------------
bool CVIEW_Table_Data::DeleteCols			(size_t Position, size_t Number)
{
	wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_COLS_DELETED , Position, Number);

	return( GetView()->ProcessTableMessage(msg) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Data::SetValue(int iRecord, int iField, const wxString &Value)
{
	CSG_Table_Record *pRecord = Get_Record(iRecord, iField); if( m_bRowLabels ) iField++;

	if( pRecord )
	{
		if( Value.IsEmpty() )
		{
			pRecord->Set_NoData(iField);
		}
		else switch( m_pTable->Get_Field_Type(iField) )
		{
		default:
			{
				pRecord->Set_Value(iField, &Value);
			}
			break;

		case SG_DATATYPE_Color:
			{
				if( Value[0] == '#' )
				{
					wxColour Color; Color.Set(Value);

					pRecord->Set_Value(iField, Get_Color_asInt(Color));
				}
				else
				{
					pRecord->Set_Value(iField, &Value);
				}
			}
			break;
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Data::SetValueAsLong(int iRecord, int iField, long Value)
{
	CSG_Table_Record *pRecord = Get_Record(iRecord, iField); if( m_bRowLabels ) iField++;

	if( pRecord ) { pRecord->Set_Value(iField, Value); }
}

//---------------------------------------------------------
void CVIEW_Table_Data::SetValueAsDouble(int iRecord, int iField, double Value)
{
	CSG_Table_Record *pRecord = Get_Record(iRecord, iField); if( m_bRowLabels ) iField++;

	if( pRecord ) { pRecord->Set_Value(iField, Value); }
}

//---------------------------------------------------------
void CVIEW_Table_Data::SetValueAsBool(int iRecord, int iField, bool Value)
{
	CSG_Table_Record *pRecord = Get_Record(iRecord, iField); if( m_bRowLabels ) iField++;

	if( pRecord ) { pRecord->Set_Value(iField, Value); }
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Data::On_Changed(int iRecord, int iField)
{
	CSG_Table_Record *pRecord = Get_Record(iRecord);

	if( pRecord && GetView() )
	{
		switch( m_pTable->Get_Field_Type(iField) )
		{
		default:
			{
				// nop
			}
			break;

		case SG_DATATYPE_Color:
			{
				wxColour Colour(Get_Color_asWX(pRecord->asInt(iField)));

				GetView()->SetCellBackgroundColour(iRecord, iField, Colour);
				GetView()->SetCellTextColour      (iRecord, iField, Colour);
			}
			break;
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CVIEW_Table_Data::On_Sort(void)
{
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

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Data::Sort(int iField, int Direction)
{
	if( iField >= 0 && iField < GetNumberCols() )
	{
		switch( Direction )
		{
		default: m_pTable->Toggle_Index(iField                        ); break;
		case  0: m_pTable->   Set_Index(iField, TABLE_INDEX_None      ); break;
		case  1: m_pTable->   Set_Index(iField, TABLE_INDEX_Ascending ); break;
		case  2: m_pTable->   Set_Index(iField, TABLE_INDEX_Descending); break;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Data::To_Clipboard(void)
{
	if( wxTheClipboard->Open() )
	{
		wxString	Data;

		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			Data	+= m_pTable->Get_Field_Name(iField);
			Data	+= 1 + iField < m_pTable->Get_Field_Count() ? '\t' : '\n';
		}

		int nRecords = m_pTable->Get_Selection_Count()
			? (int)m_pTable->Get_Selection_Count() : m_pTable->Get_Count();

		for(int iRecord=0; iRecord<nRecords; iRecord++)
		{
			CSG_Table_Record	*pRecord	= m_pTable->Get_Selection_Count()
				? m_pTable->Get_Selection(iRecord) : m_pTable->Get_Record_byIndex(iRecord);

			for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
			{
				Data	+= pRecord->asString(iField);
				Data	+= 1 + iField < m_pTable->Get_Field_Count() ? '\t' : '\n';
			}
		}

		wxTheClipboard->SetData(new wxTextDataObject(Data));
		wxTheClipboard->Close();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Data::is_DataSource(wxString &Source)
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
