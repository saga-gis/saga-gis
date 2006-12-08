
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

#include "res_controls.h"

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
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CINFO_Messages::CINFO_Messages(wxWindow *pParent)
	: wxTextCtrl(pParent, ID_WND_INFO_MESSAGES, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxSUNKEN_BORDER)
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

	_Add_Text(wxString::Format(wxT("[%s/%s]"),
		Time.FormatISODate().c_str(),
		Time.FormatISOTime().c_str())
	);
}

//---------------------------------------------------------
void CINFO_Messages::Add_Line(void)
{
	_Add_Text(wxT("\n_______________________"));
}

//---------------------------------------------------------
void CINFO_Messages::Add_String(wxString sMessage, bool bNewLine, bool bTime)
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

	_Add_Text(sMessage);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
