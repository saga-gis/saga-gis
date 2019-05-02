
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
//                 ACTIVE_Attributes.cpp                 //
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
#include <wx/event.h>
#include <wx/button.h>

#include <saga_api/saga_api.h>

#include "res_controls.h"
#include "res_dialogs.h"

#include "wksp_layer.h"

#include "active_attributes.h"
#include "active_attributes_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CActive_Attributes, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CActive_Attributes, wxPanel)
	EVT_CHOICE   (ID_COMBOBOX_SELECT, CActive_Attributes::On_Choice   )
	EVT_BUTTON   (ID_BTN_APPLY      , CActive_Attributes::On_Button   )
	EVT_BUTTON   (ID_BTN_RESTORE    , CActive_Attributes::On_Button   )
	EVT_UPDATE_UI(ID_BTN_APPLY      , CActive_Attributes::On_Button_UI)
	EVT_UPDATE_UI(ID_BTN_RESTORE    , CActive_Attributes::On_Button_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CActive_Attributes::CActive_Attributes(wxWindow *pParent)
	: wxPanel(pParent, ID_WND_INFO_ATTRIBUTES, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
	m_pItem			= NULL;

	m_pSelections	= new wxChoice(this, ID_COMBOBOX_SELECT, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);

	m_pControl		= new CActive_Attributes_Control(this);

	//-----------------------------------------------------
	wxBoxSizer	*pButtons	= new wxBoxSizer(wxHORIZONTAL);

	pButtons->Add(new wxButton(this, ID_BTN_APPLY  , CTRL_Get_Name(ID_BTN_APPLY  ), wxPoint(0, 0)), 1, wxEXPAND|wxLEFT|wxRIGHT);
	pButtons->Add(new wxButton(this, ID_BTN_RESTORE, CTRL_Get_Name(ID_BTN_RESTORE), wxPoint(0, 0)), 1, wxEXPAND|wxLEFT|wxRIGHT);

	//-----------------------------------------------------
	wxBoxSizer	*pSizer		= new wxBoxSizer(wxVERTICAL  );

	pSizer->Add(m_pSelections, 0, wxEXPAND|wxLEFT|wxRIGHT);
	pSizer->Add(m_pControl   , 1, wxEXPAND|wxALL);
	pSizer->Add(  pButtons   , 0, wxEXPAND|wxLEFT|wxRIGHT);

	SetSizer(pSizer);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes::On_Button(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	case ID_BTN_APPLY  :
		Save_Changes(false);
		break;

	case ID_BTN_RESTORE:
		Set_Attributes();
		break;
	}
}

void CActive_Attributes::On_Button_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pControl->Get_Table()->is_Modified());
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes::On_Choice(wxCommandEvent &event)
{
	if( _Get_Table() && m_pSelections->GetSelection() < _Get_Table()->Get_Selection_Count() )
	{
		Save_Changes(true);

		m_pItem->Edit_Set_Index(m_pSelections->GetSelection());
		m_pItem->Update_Views(false);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline CSG_Table * CActive_Attributes::_Get_Table(void)
{
	switch( m_pItem ? m_pItem->Get_Type() : WKSP_ITEM_Undefined )
	{
	case WKSP_ITEM_Shapes    :
	case WKSP_ITEM_PointCloud: return( (CSG_Table *)m_pItem->Get_Object() );

	default                  : return( NULL );
	}
}

//---------------------------------------------------------
void CActive_Attributes::Set_Item(CWKSP_Layer *pItem)
{
	if( m_pItem != pItem )
	{
		Save_Changes(true);

		m_pItem	= pItem;

		Set_Attributes();
	}
}

//---------------------------------------------------------
void CActive_Attributes::Set_Attributes(void)
{
	Freeze();

	m_pSelections->Clear();

	if( m_pItem && m_pItem->Edit_Get_Attributes()->is_Valid() )
	{
		m_pControl->Get_Table()->Assign(m_pItem->Edit_Get_Attributes());
		m_pControl->Get_Table()->Set_Modified(false);

		if( _Get_Table() && _Get_Table()->Get_Selection_Count() > 1 )
		{
			for(int i=0; i<_Get_Table()->Get_Selection_Count(); i++)
			{
				m_pSelections->Append(wxString::Format("%d", i + 1));
			}

			m_pSelections->Select(m_pItem->Edit_Get_Index());
		}
	}
	else
	{
		m_pControl->Get_Table()->Destroy();
	}

	m_pControl->Set_Row_Labeling(_Get_Table() != NULL);
	m_pControl->Update_Table();

	m_pSelections->Show(m_pSelections->GetCount() > 1);

	GetSizer()->Layout();

	Thaw();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Attributes::Save_Changes(bool bConfirm)
{
	if( m_pItem && m_pControl->Get_Table()->is_Modified() && (!bConfirm || DLG_Message_Confirm(_TL("Save changes?"), _TL("Attributes"))) )
	{
		m_pItem->Edit_Get_Attributes()->Assign_Values(m_pControl->Get_Table());
		m_pItem->Edit_Set_Attributes();

		Set_Attributes();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
