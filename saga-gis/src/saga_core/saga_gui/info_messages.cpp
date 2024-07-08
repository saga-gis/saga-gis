
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
//                  INFO_Messages.cpp                    //
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
#include <wx/settings.h>
#include <wx/datetime.h>

#include "helper.h"

#include "res_controls.h"
#include "res_commands.h"

#include "info_messages.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxBEGIN_EVENT_TABLE(CINFO_Messages, wxTextCtrl)
	EVT_SYS_COLOUR_CHANGED(CINFO_Messages::On_SysColourChanged)

	EVT_RIGHT_UP(CINFO_Messages::On_Context_Menu)

	EVT_MENU(ID_CMD_INFO_SELECTALL, CINFO_Messages::On_SelectAll)
	EVT_MENU(ID_CMD_INFO_COPY     , CINFO_Messages::On_Copy)
	EVT_MENU(ID_CMD_INFO_CLEAR    , CINFO_Messages::On_Clear)
wxEND_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CINFO_Messages::CINFO_Messages(wxWindow *pParent)
	: wxTextCtrl(pParent, ID_WND_INFO_MESSAGES, "", wxDefaultPosition, wxDefaultSize, wxTE_RICH2|wxTE_NOHIDESEL|wxTE_MULTILINE|wxTE_READONLY|wxSUNKEN_BORDER)
{
	SetBackgroundColour(SYS_Get_Color(wxSYS_COLOUR_WINDOW));

	m_MaxLength  = 0x10000;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CINFO_Messages::On_SysColourChanged(wxSysColourChangedEvent &event)
{
	SetBackgroundColour(SYS_Get_Color(wxSYS_COLOUR_WINDOW));

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CINFO_Messages::On_Context_Menu(wxMouseEvent &event)
{
	wxMenu Menu;

	CMD_Menu_Add_Item(&Menu, false, ID_CMD_INFO_CLEAR);
	CMD_Menu_Add_Item(&Menu, false, ID_CMD_INFO_SELECTALL);

	if( CanCopy() )
	{
		CMD_Menu_Add_Item(&Menu, false, ID_CMD_INFO_COPY);
	}

	PopupMenu(&Menu);
}

//---------------------------------------------------------
void CINFO_Messages::On_Clear(wxCommandEvent &WXUNUSED(event))
{
	Clear();
}

//---------------------------------------------------------
void CINFO_Messages::On_SelectAll(wxCommandEvent &WXUNUSED(event))
{
	SelectAll();
}

//---------------------------------------------------------
void CINFO_Messages::On_Copy(wxCommandEvent &WXUNUSED(event))
{
	if( GetStringSelection().IsEmpty() )
	{
		SelectAll(); Copy(); SetSelection(GetLastPosition(), GetLastPosition());
	}
	else
	{
		Copy();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CINFO_Messages::_Add_Text(const wxString &Text)
{
	if( m_MaxLength <= (int)(GetLastPosition() + Text.Length()) )
	{
		int n = 0;

		for(int i=0; i<GetNumberOfLines() && n<(int)Text.Length(); i++)
		{
			n += 1 + GetLineLength(i);
		}

		Remove(0, n + 1);
	}

	AppendText(Text);
}

//---------------------------------------------------------
void CINFO_Messages::_Set_Style(TSG_UI_MSG_STYLE Style_Code)
{
	wxTextAttr Style(GetDefaultStyle());

	Style.SetTextColour(wxSYS_COLOUR_WINDOWTEXT);

	bool bDark = wxSystemSettings::GetAppearance().IsUsingDarkBackground();

	wxFont Font(Style.GetFont());

	if( !Font.IsOk() )
	{
	//	Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		Font = GetFont();
		Font.SetPointSize((int)(0.5 + 0.9 * Font.GetPointSize())); // generally scale message window font to 90 percent
	}

	Font.SetStyle (wxFONTSTYLE_NORMAL );
	Font.SetWeight(wxFONTWEIGHT_NORMAL);

	switch( Style_Code )
	{
	case SG_UI_MSG_STYLE_NORMAL: default:
		break;

	case SG_UI_MSG_STYLE_BOLD:
		Font.SetWeight(wxFONTWEIGHT_BOLD);
		break;

	case SG_UI_MSG_STYLE_ITALIC:
		Font.SetStyle(wxFONTSTYLE_ITALIC);
		break;

	case SG_UI_MSG_STYLE_SUCCESS:
		Style.SetTextColour(bDark ? wxColour(0, 255, 0) : wxColour(0, 127, 0));
		break;

	case SG_UI_MSG_STYLE_FAILURE:
		Style.SetTextColour(bDark ? wxColour(255, 0, 0) : wxColour(127, 0, 0));
		break;

	case SG_UI_MSG_STYLE_BIG:
		Font.SetWeight(wxFONTWEIGHT_BOLD);
	//	Font.SetNumericWeight(wxFONTSIZE_SMALL);
		break;

	case SG_UI_MSG_STYLE_SMALL:
		Font.SetWeight(wxFONTWEIGHT_LIGHT);
	//	Font.SetNumericWeight(wxFONTSIZE_LARGE);
		break;

	case SG_UI_MSG_STYLE_01:
		Style.SetLeftIndent(50);
		Style.SetTextColour(bDark ? wxColour(0, 127, 255) : wxColour(0, 0, 127));
		Font.SetWeight(wxFONTWEIGHT_LIGHT);
		break;

	case SG_UI_MSG_STYLE_02:
		break;

	case SG_UI_MSG_STYLE_03:
		Style.SetTextColour(bDark ? wxColour(0, 127, 255) : wxColour(0, 0, 127));
		Font.SetWeight(wxFONTWEIGHT_BOLD);
		break;
	}

	Style.SetFont(Font);

	SetDefaultStyle(Style);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CINFO_Messages::Add_Time(bool bNewLine)
{
	if( bNewLine )
	{
		_Add_Text("\n");
	}

	wxDateTime Time; Time.SetToCurrent();

	_Set_Style(SG_UI_MSG_STYLE_03);

	_Add_Text(wxString::Format("[%s/%s]",
		Time.FormatISODate().c_str(),
		Time.FormatISOTime().c_str())
	);
}

//---------------------------------------------------------
void CINFO_Messages::Add_Line(void)
{
	SetDefaultStyle(wxTextAttr(wxNullColour, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));

	_Add_Text("\n\n");

	CSG_Colors Colors; Colors.Set_Ramp(
		Get_Color_asInt(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION)),
		Get_Color_asInt(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW))
	);

	Colors.Set_Count(100);

	for(int i=0; i<Colors.Get_Count(); i++)
	{
		SetDefaultStyle(wxTextAttr(
			Get_Color_asWX(Colors.Get_Color(Colors.Get_Count() - 1 - i)),
			Get_Color_asWX(Colors.Get_Color(i))
		));

		_Add_Text("  ");
	}

	SetDefaultStyle(wxTextAttr(wxNullColour, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
}

//---------------------------------------------------------
void CINFO_Messages::Add_String(wxString sMessage, bool bNewLine, bool bTime, TSG_UI_MSG_STYLE Style)
{
	if( !sMessage.IsEmpty() )
	{
		if( bNewLine )
		{
			_Add_Text("\n");
		}

		if( bTime )
		{
			Add_Time(false);

			_Add_Text(" ");
		}

		_Set_Style(Style);

		_Add_Text(sMessage);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
