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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include <wx/event.h>
#include <wx/button.h>

#include <saga_api/saga_api.h>

#include "res_controls.h"
#include "res_dialogs.h"

#include "wksp_layer.h"
#include "view_table_control.h"

#include "active_attributes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CACTIVE_Attributes, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CACTIVE_Attributes, wxPanel)
	EVT_SIZE		(CACTIVE_Attributes::On_Size)

	EVT_CHOICE		(ID_COMBOBOX_SELECT, CACTIVE_Attributes::On_Choice)

	EVT_BUTTON		(ID_BTN_APPLY      , CACTIVE_Attributes::On_Apply)
	EVT_UPDATE_UI	(ID_BTN_APPLY      , CACTIVE_Attributes::On_Apply_UI)
	EVT_BUTTON		(ID_BTN_RESTORE    , CACTIVE_Attributes::On_Restore)
	EVT_UPDATE_UI	(ID_BTN_RESTORE    , CACTIVE_Attributes::On_Restore_UI)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CACTIVE_Attributes::CACTIVE_Attributes(wxWindow *pParent)
	: wxPanel(pParent, ID_WND_INFO_ATTRIBUTES)//, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
	m_pAttributes	= new CSG_Table;
	m_pControl		= new CVIEW_Table_Control(this, m_pAttributes, TABLE_CTRL_FIXED_TABLE|TABLE_CTRL_COL1ISLABEL);

	m_pItem			= NULL;

	//-----------------------------------------------------
	m_Btn_Apply		= new wxButton(this, ID_BTN_APPLY	, CTRL_Get_Name(ID_BTN_APPLY)	, wxPoint(0, 0));
	m_Btn_Restore	= new wxButton(this, ID_BTN_RESTORE	, CTRL_Get_Name(ID_BTN_RESTORE)	, wxPoint(0, 0));

	m_btn_height	= m_Btn_Apply->GetDefaultSize().y;

	m_pSelections	= new wxChoice(this, ID_COMBOBOX_SELECT, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);

	//-----------------------------------------------------
	m_pAttributes->Set_Modified(false);
}

//---------------------------------------------------------
CACTIVE_Attributes::~CACTIVE_Attributes(void)
{
	delete(m_pAttributes);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Attributes::On_Size(wxSizeEvent &WXUNUSED(event))
{
	_Set_Positions();
}

//---------------------------------------------------------
#define BUTTON_DIST			1
#define BUTTON_DIST2		2 * BUTTON_DIST

#define SET_BTN_POS(BTN)	if( BTN->IsShown() ) { BTN->SetSize(r); r.SetLeft(r.GetLeft() + r.GetWidth() + BUTTON_DIST2); }

//---------------------------------------------------------
void CACTIVE_Attributes::_Set_Positions(void)
{
	int		nButtons	= 2;
	wxRect	r(wxPoint(0, 0), GetSize());

	if( m_pSelections->GetCount() > 0 )
	{
		m_pSelections->SetSize(0, 0, r.GetWidth(), m_btn_height - BUTTON_DIST);

		r.SetTop(m_btn_height);
		r.SetHeight(r.GetHeight() - m_btn_height * 2);
	}
	else
	{
		r.SetHeight(r.GetHeight() - m_btn_height);
	}

	m_pControl->SetSize(r);

	r.SetTop(r.GetBottom() + BUTTON_DIST2);
	r.SetHeight(m_btn_height - BUTTON_DIST);
	r.SetWidth(r.GetWidth() / nButtons - BUTTON_DIST2);
	r.SetLeft(BUTTON_DIST);

	SET_BTN_POS(m_Btn_Apply);
	SET_BTN_POS(m_Btn_Restore);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Attributes::On_Apply(wxCommandEvent &event)
{
	Save_Changes(false);
}

void CACTIVE_Attributes::On_Apply_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pAttributes->is_Modified());
}

//---------------------------------------------------------
void CACTIVE_Attributes::On_Restore(wxCommandEvent &event)
{
	Set_Attributes();
}

void CACTIVE_Attributes::On_Restore_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pAttributes->is_Modified());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Attributes::On_Choice(wxCommandEvent &event)
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline CSG_Table * CACTIVE_Attributes::_Get_Table(void)
{
	switch( m_pItem ? m_pItem->Get_Type() : WKSP_ITEM_Undefined )
	{
	default:
		return( NULL );

	case WKSP_ITEM_Shapes:
	case WKSP_ITEM_PointCloud:
		return( (CSG_Table *)m_pItem->Get_Object() );
	}
}

//---------------------------------------------------------
void CACTIVE_Attributes::Set_Item(CWKSP_Layer *pItem)
{
	if( m_pItem != pItem )
	{
		Save_Changes(true);

		m_pItem	= pItem;

		Set_Attributes();
	}
}

//---------------------------------------------------------
void CACTIVE_Attributes::Set_Attributes(void)
{
	Freeze();

	m_pSelections->Clear();

	if( m_pItem && m_pItem->Edit_Get_Attributes()->is_Valid() )
	{
		Save_Changes(true);

		m_pAttributes->Assign(m_pItem->Edit_Get_Attributes());

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
		m_pAttributes->Destroy();
	}

	m_pAttributes->Set_Modified(false);

	m_pControl->Set_Labeling(_Get_Table() != NULL);

	m_pControl->Update_Table();

	m_pSelections->Show(m_pSelections->GetCount() > 1);

	_Set_Positions();

	Thaw();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Attributes::Save_Changes(bool bConfirm)
{
	if( m_pItem && m_pAttributes->is_Modified() && (!bConfirm || DLG_Message_Confirm(_TL("Save changes?"), _TL("Attributes"))) )
	{
		m_pItem->Edit_Get_Attributes()->Assign_Values(m_pAttributes);
		m_pItem->Edit_Set_Attributes();

		m_pAttributes->Set_Modified(false);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
