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
IMPLEMENT_CLASS(CINFO_Messages, wxTextCtrl)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CINFO_Messages, wxTextCtrl)
    EVT_CONTEXT_MENU(CINFO_Messages::On_Context_Menu)

	EVT_MENU		(ID_CMD_INFO_COPY	, CINFO_Messages::On_Copy)
	EVT_MENU		(ID_CMD_INFO_CLEAR	, CINFO_Messages::On_Clear)

END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CINFO_Messages::CINFO_Messages(wxWindow *pParent)
	: wxTextCtrl(pParent, ID_WND_INFO_MESSAGES, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_RICH2|wxTE_NOHIDESEL|wxTE_MULTILINE|wxTE_READONLY|wxSUNKEN_BORDER)
{
	m_MaxLength	= 0x10000;

	SetMaxLength(m_MaxLength);
}

//---------------------------------------------------------
CINFO_Messages::~CINFO_Messages(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CINFO_Messages::On_Context_Menu(wxContextMenuEvent &event)
{
	wxMenu	Menu;

	Menu.Append(ID_CMD_INFO_COPY		, _TL("Copy"));
	Menu.Append(ID_CMD_INFO_CLEAR		, _TL("Clear"));

	PopupMenu(&Menu);
}

//---------------------------------------------------------
void CINFO_Messages::On_Copy(wxCommandEvent &WXUNUSED(event))
{
	if( GetStringSelection().IsEmpty() )
	{
		SetSelection(-1, -1);
		Copy();
		SetSelection(GetLastPosition(), GetLastPosition());
	}
	else
	{
		Copy();
	}
}

//---------------------------------------------------------
void CINFO_Messages::On_Clear(wxCommandEvent &WXUNUSED(event))
{
	Clear();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CINFO_Messages::_Add_Text(wxString Text)
{
	int		i, n;

	if( m_MaxLength <= (int)(GetLastPosition() + Text.Length()) )
	{
		for(i=0, n=0; i<GetNumberOfLines() && n<(int)Text.Length(); i++)
		{
			n	+= 1 + GetLineLength(i);
		}

		Remove(0, n + 1);
	}

	AppendText(Text);
}

//---------------------------------------------------------
void CINFO_Messages::_Set_Style(TSG_UI_MSG_STYLE Style)
{
	int			i	= 0;
	wxColour	c	= wxColour(  0,   0,   0);
	wxFont		f	= wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxTextAttr	t;

	switch( Style )
	{
	default:
	case SG_UI_MSG_STYLE_NORMAL:
		break;

	case SG_UI_MSG_STYLE_BOLD:
		f.SetWeight(wxFONTWEIGHT_BOLD);
		break;

	case SG_UI_MSG_STYLE_ITALIC:
		f.SetStyle(wxFONTSTYLE_ITALIC);
		break;

	case SG_UI_MSG_STYLE_SUCCESS:
		c	= wxColour(  0, 127,   0);
		break;

	case SG_UI_MSG_STYLE_FAILURE:
		c	= wxColour(127,   0,   0);
		break;

	case SG_UI_MSG_STYLE_BIG:
		f.SetWeight(wxFONTWEIGHT_BOLD);
		f.SetPointSize(10);
		break;

	case SG_UI_MSG_STYLE_SMALL:
		f.SetWeight(wxFONTWEIGHT_LIGHT);
		f.SetPointSize(6);
		break;

	case SG_UI_MSG_STYLE_01:
		i	= 50;
		c	= wxColour(  0,   0, 127);
		f.SetWeight(wxFONTWEIGHT_LIGHT);
		f.SetPointSize(8);
		break;

	case SG_UI_MSG_STYLE_02:
		break;

	case SG_UI_MSG_STYLE_03:
		c	= wxColour(  0,   0, 127);
		f.SetWeight(wxFONTWEIGHT_BOLD);
		break;
	}

	t.SetFlags(wxTEXT_ATTR_TEXT_COLOUR|wxTEXT_ATTR_FONT_WEIGHT|wxTEXT_ATTR_FONT_ITALIC|wxTEXT_ATTR_FONT_SIZE|wxTEXT_ATTR_LEFT_INDENT);

	t.SetLeftIndent(i);
	t.SetTextColour(c);
	t.SetFont(f);

	SetDefaultStyle(t);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CINFO_Messages::Add_Time(bool bNewLine)
{
	wxDateTime	Time;

	if( bNewLine )
	{
		_Add_Text(wxT("\n"));
	}

	Time.SetToCurrent();

	_Set_Style(SG_UI_MSG_STYLE_03);

	_Add_Text(wxString::Format(wxT("[%s/%s]"),
		Time.FormatISODate().c_str(),
		Time.FormatISOTime().c_str())
	);
}

//---------------------------------------------------------
void CINFO_Messages::Add_Line(void)
{
	CSG_Colors	c;

	SetDefaultStyle(wxTextAttr(wxNullColour, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));

	_Add_Text(wxT("\n\n"));

	c.Set_Ramp(
		Get_Color_asInt(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION)),
		Get_Color_asInt(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW))
	);

	c.Set_Count(100);

	for(int i=0; i<c.Get_Count(); i++)
	{
		SetDefaultStyle(wxTextAttr(
			Get_Color_asWX(c.Get_Color(c.Get_Count() - 1 - i)),
			Get_Color_asWX(c.Get_Color(i))
		));

		_Add_Text(wxT("  "));
	}

	SetDefaultStyle(wxTextAttr(wxNullColour, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
}

//---------------------------------------------------------
void CINFO_Messages::Add_String(wxString sMessage, bool bNewLine, bool bTime, TSG_UI_MSG_STYLE Style)
{
	if( bNewLine )
	{
		_Add_Text(wxT("\n"));
	}

	if( bTime )
	{
		Add_Time(false);

		_Add_Text(wxT(" "));
	}

	_Set_Style(Style);

	_Add_Text(sMessage);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
