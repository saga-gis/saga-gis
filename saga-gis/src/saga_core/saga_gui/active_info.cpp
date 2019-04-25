
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
//                   active_info.cpp                     //
//                                                       //
//          Copyright (C) 2019 by Olaf Conrad            //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dataobj.h>
#include <wx/html/htmlwin.h>

#include "res_controls.h"

#include "helper.h"

#include "active_info.h"

#include "wksp_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CActive_Info_Control : public wxHtmlWindow
{
	DECLARE_CLASS(CActive_Info_Control)

public:
	CActive_Info_Control(wxWindow *pParent)
		: wxHtmlWindow(pParent, -1, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO|wxSUNKEN_BORDER)
	{
		SetPage("<html><body>Hello, world!</body></html>");
	}

	virtual void	OnLinkClicked	(const wxHtmlLinkInfo &Link)
	{
		Open_WebBrowser(Link.GetHref());
	}

};

//---------------------------------------------------------
IMPLEMENT_CLASS(CActive_Info_Control, wxHtmlWindow)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CActive_Info, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CActive_Info, wxPanel)
	EVT_SIZE		(CActive_Info::On_Size)

	EVT_CHOICE		(ID_COMBOBOX_SELECT, CActive_Info::On_Choice)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CActive_Info::CActive_Info(wxWindow *pParent)
	: wxPanel(pParent, ID_WND_ACTIVE_INFO , wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
	m_pItem			= NULL;

	m_pControl		= new CActive_Info_Control(this);

	m_pSelections	= new wxChoice(this, ID_COMBOBOX_SELECT, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
}

//---------------------------------------------------------
CActive_Info::~CActive_Info(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Info::Set_Item(CWKSP_Layer *pItem)
{
	if( m_pItem != pItem )
	{
		m_pItem	= pItem;
	}

	Set_Info();
}

//---------------------------------------------------------
void CActive_Info::_Set_Positions(void)
{
	wxRect	r(GetClientSize());

	if( m_pSelections->GetCount() > 0 )
	{
		m_pSelections->SetSize(0, 0, r.GetWidth(), m_pSelections->GetSize().GetHeight());

		r.SetTop(m_pSelections->GetSize().GetHeight());
		r.SetHeight(r.GetHeight() - m_pSelections->GetSize().GetHeight());
	}

	m_pControl->SetSize(r);
}

//---------------------------------------------------------
void CActive_Info::On_Size(wxSizeEvent &WXUNUSED(event))
{
	_Set_Positions();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Info::On_Choice(wxCommandEvent &event)
{
	CSG_Shapes	*pShapes	= m_pItem && m_pItem->Get_Type() == WKSP_ITEM_Shapes ? ((CWKSP_Shapes *)m_pItem)->Get_Shapes() : NULL;

	if( pShapes && m_pSelections->GetSelection() < pShapes->Get_Selection_Count() )
	{
		m_pItem->Edit_Set_Index(m_pSelections->GetSelection());
		m_pItem->Update_Views(false);

		_Set_Info();
	}
}

//---------------------------------------------------------
void CActive_Info::Set_Info(void)
{
	Freeze();

	m_pSelections->Clear();

	CSG_Shapes	*pShapes	= m_pItem && m_pItem->Get_Type() == WKSP_ITEM_Shapes ? ((CWKSP_Shapes *)m_pItem)->Get_Shapes() : NULL;

	if( pShapes && pShapes->Get_Selection_Count() > 0 )
	{
		if( pShapes->Get_Selection_Count() > 1 )
		{
			for(int i=0; i<pShapes->Get_Selection_Count(); i++)
			{
				m_pSelections->Append(wxString::Format("%d", i + 1));
			}

			m_pSelections->Select(m_pItem->Edit_Get_Index());
		}
	}

	m_pSelections->Show(m_pSelections->GetCount() > 1);

	_Set_Positions();

	Thaw();
}

//---------------------------------------------------------
void CActive_Info::_Set_Info(void)
{
	CSG_Shapes	*pShapes	= m_pItem && m_pItem->Get_Type() == WKSP_ITEM_Shapes ? ((CWKSP_Shapes *)m_pItem)->Get_Shapes() : NULL;

	if( pShapes && pShapes->Get_Selection_Count() > 0  && pShapes->Get_Selection_Count() > m_pItem->Edit_Get_Index() )
	{
		CSG_Shape	*pShape	= pShapes->Get_Selection(m_pItem->Edit_Get_Index());

		if( pShape )
		{
			wxFileName	fn(pShape->asString(((CWKSP_Shapes *)m_pItem)->Get_Field_Info()));

			if( fn.IsRelative() )
			{
				fn.MakeAbsolute(SG_File_Get_Path(pShapes->Get_File_Name()).c_str());
			}

			if( fn.Exists() && m_pControl->LoadFile(fn.GetFullPath()) )
			{
				return;
			}
		}
	}

	m_pControl->SetPage("<html><body>Hello, world!</body></html>");
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
