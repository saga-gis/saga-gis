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
//                   dlg_choices.cpp                     //
//                                                       //
//          Copyright (C) 2017 by Olaf Conrad            //
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
#include <saga_api/saga_api.h>

#include "res_controls.h"

#include "dlg_choices.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define dlg_choices_BTN_WIDTH	40
#define dlg_choices_BTN_DIST	4


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_Choices, CDLG_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Choices, CDLG_Base)
	EVT_BUTTON			(ID_BTN_ADD			, CDLG_Choices::On_Add)
	EVT_BUTTON			(ID_BTN_ADD_ALL		, CDLG_Choices::On_Add_All)
	EVT_BUTTON			(ID_BTN_DELETE		, CDLG_Choices::On_Del)
	EVT_BUTTON			(ID_BTN_DELETE_ALL	, CDLG_Choices::On_Del_All)
	EVT_BUTTON			(ID_BTN_UP			, CDLG_Choices::On_Up)
	EVT_BUTTON			(ID_BTN_DOWN		, CDLG_Choices::On_Down)

	EVT_LISTBOX_DCLICK	(ID_LISTBOX_SELECT	, CDLG_Choices::On_DClick_Add)
	EVT_LISTBOX_DCLICK	(ID_LISTBOX_ADD		, CDLG_Choices::On_DClick_Del)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Choices::CDLG_Choices(CSG_Parameter_Choices *Parameter, wxString Caption)
	: CDLG_Base(-1, Caption)
{
	m_pParameter	= Parameter;

	m_pChoices		= new wxListBox(this, ID_LISTBOX_SELECT, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB);
	m_pSelection	= new wxListBox(this, ID_LISTBOX_ADD   , wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_NEEDED_SB);

	m_pBtn_Add_All	= new wxButton (this, ID_BTN_ADD_ALL   , ">>");
	m_pBtn_Add		= new wxButton (this, ID_BTN_ADD       , ">" );
	m_pBtn_Del		= new wxButton (this, ID_BTN_DELETE    , "<" );
	m_pBtn_Del_All	= new wxButton (this, ID_BTN_DELETE_ALL, "<<");
	m_pBtn_Up		= new wxButton (this, ID_BTN_UP        , CTRL_Get_Name(ID_BTN_UP));
	m_pBtn_Down		= new wxButton (this, ID_BTN_DOWN      , CTRL_Get_Name(ID_BTN_DOWN));

	m_Btn_Height	= m_pBtn_Add->GetDefaultSize().y;

	//-----------------------------------------------------
	int		*Index	= (int *)m_Index.Get_Array(m_pParameter->Get_Item_Count());

	for(int i=0; i<m_pParameter->Get_Item_Count(); i++)
	{
		Index[i]	= i;

		if( m_pParameter->is_Selected(i) )
		{
			m_pSelection->Append(m_pParameter->Get_Item(i).c_str(), (void *)&Index[i]);
		}
		else
		{
			m_pChoices  ->Append(m_pParameter->Get_Item(i).c_str(), (void *)&Index[i]);
		}
	}

	//-----------------------------------------------------
	Set_Positions();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Choices::Set_Position(wxRect r)
{
	r.Deflate(5);

	int	Center	= r.GetLeft() + r.GetWidth() / 2;

	r.SetWidth(r.GetWidth() / 2 - (dlg_choices_BTN_WIDTH / 2 + dlg_choices_BTN_DIST));
	m_pChoices->SetSize(r);

	r.SetLeft(Center            + (dlg_choices_BTN_WIDTH / 2 + dlg_choices_BTN_DIST));
	m_pSelection->SetSize(r);

	r		= wxRect(Center - dlg_choices_BTN_WIDTH / 2, r.GetTop(), dlg_choices_BTN_WIDTH, m_Btn_Height);
	m_pBtn_Add_All->SetSize(r);

	r.SetTop(r.GetBottom() + dlg_choices_BTN_DIST);
	m_pBtn_Add->SetSize(r);

	r.SetTop(r.GetBottom() + dlg_choices_BTN_DIST * 2);
	m_pBtn_Del->SetSize(r);

	r.SetTop(r.GetBottom() + dlg_choices_BTN_DIST);
	m_pBtn_Del_All->SetSize(r);

	r.SetTop(r.GetBottom() + dlg_choices_BTN_DIST * 2);
	m_pBtn_Up->SetSize(r);

	r.SetTop(r.GetBottom() + dlg_choices_BTN_DIST);
	m_pBtn_Down->SetSize(r);
}

//---------------------------------------------------------
void CDLG_Choices::Save_Changes(void)
{
	m_pParameter->Clr_Selection();

	for(unsigned int i=0; i<m_pSelection->GetCount(); i++)
	{
		m_pParameter->Select(*(int *)m_pSelection->GetClientData(i));
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDLG_Choices::_Compare_Up(int *first, int *second)
{
	return( *first - *second );
}

//---------------------------------------------------------
int CDLG_Choices::_Compare_Down(int *first, int *second)
{
	return( *second - *first );
}

//---------------------------------------------------------
void CDLG_Choices::On_DClick_Del(wxCommandEvent &event)
{
	_Del();
}

void CDLG_Choices::On_Del(wxCommandEvent &event)
{
	_Del();
}

void CDLG_Choices::_Del(void)
{
	wxArrayInt	Selections;

	if( m_pSelection->GetSelections(Selections) > 0 )
	{
		Selections.Sort(_Compare_Down);

		for(size_t i=0; i<Selections.GetCount(); i++)
		{
			size_t	j	= Selections.Item(i);

			m_pChoices->Append(m_pSelection->GetString(j), m_pSelection->GetClientData(j));
			m_pSelection->Delete(j);
		}
	}
}

void CDLG_Choices::On_Del_All(wxCommandEvent &event)
{
	for(unsigned int i=0; i<m_pSelection->GetCount(); i++)
	{
		m_pChoices->Append(m_pSelection->GetString(i), m_pSelection->GetClientData(i));
	}

	m_pSelection->Clear();
}

//---------------------------------------------------------
void CDLG_Choices::On_DClick_Add(wxCommandEvent &event)
{
	_Add();
}

void CDLG_Choices::On_Add(wxCommandEvent &event)
{
	_Add();
}

void CDLG_Choices::_Add(void)
{
	wxArrayInt	Selections;

	if( m_pChoices->GetSelections(Selections) > 0 )
	{
		Selections.Sort(_Compare_Down);

		for(size_t i=0, Position=m_pSelection->GetCount(); i<Selections.GetCount(); i++)
		{
			size_t	j	= Selections.Item(i);

			m_pSelection->Insert(m_pChoices->GetString(j), Position, m_pChoices->GetClientData(j));
			m_pChoices->Delete(j);
		}
	}
}

void CDLG_Choices::On_Add_All(wxCommandEvent &event)
{
	for(unsigned int i=0; i<m_pChoices->GetCount(); i++)
	{
		m_pSelection->Append(m_pChoices->GetString(i), m_pChoices->GetClientData(i));
	}

	m_pChoices->Clear();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Choices::On_Up(wxCommandEvent &event)
{
	wxArrayInt	Selections;

	if( m_pSelection->GetSelections(Selections) > 0 && Selections.GetCount() < m_pSelection->GetCount() )
	{
		Selections.Sort(_Compare_Up);

		if( Selections.Item(0) > 0 )
		{
			for(size_t i=0; i<Selections.GetCount(); i++)
			{
				size_t	j	= Selections.Item(i);

				wxString	String	= m_pSelection->GetString    (j);
				void		*pData	= m_pSelection->GetClientData(j);

				m_pSelection->SetString    (j    , m_pSelection->GetString    (j - 1));
				m_pSelection->SetClientData(j    , m_pSelection->GetClientData(j - 1));
				m_pSelection->SetSelection (j    , m_pSelection->IsSelected   (j - 1));

				m_pSelection->SetString    (j - 1, String);
				m_pSelection->SetClientData(j - 1, pData);
				m_pSelection->SetSelection (j - 1, true);
			}
		}
	}
}

//---------------------------------------------------------
void CDLG_Choices::On_Down(wxCommandEvent &event)
{
	wxArrayInt	Selections;

	if( m_pSelection->GetSelections(Selections) > 0 && Selections.GetCount() < m_pSelection->GetCount() )
	{
		Selections.Sort(_Compare_Down);

		if( (int)Selections.Item(0) < m_pSelection->GetCount() - 1 )
		{
			for(size_t i=0; i<Selections.GetCount(); i++)
			{
				size_t	j	= Selections.Item(i);

				wxString	String	= m_pSelection->GetString(j);
				void		*pData	= m_pSelection->GetClientData(j);

				m_pSelection->SetString    (j    , m_pSelection->GetString    (j + 1));
				m_pSelection->SetClientData(j    , m_pSelection->GetClientData(j + 1));
				m_pSelection->SetSelection (j    , m_pSelection->IsSelected   (j + 1));

				m_pSelection->SetString    (j + 1, String);
				m_pSelection->SetClientData(j + 1, pData);
				m_pSelection->SetSelection (j + 1, true);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
