
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
//                 ACTIVE_Parameters.cpp                 //
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

#include "res_controls.h"

#include "wksp_layer.h"
#include "wksp_tool.h"
#include "wksp_map_manager.h"

#include "parameters_control.h"

#include "active_parameters.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CActive_Parameters, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CActive_Parameters, wxPanel)
	EVT_SIZE			(CActive_Parameters::On_Size)
	EVT_KEY_DOWN		(CActive_Parameters::On_Key)

	EVT_BUTTON			(ID_BTN_APPLY  , CActive_Parameters::On_Apply)
	EVT_UPDATE_UI		(ID_BTN_APPLY  , CActive_Parameters::On_Apply_UI)
	EVT_BUTTON			(ID_BTN_RESTORE, CActive_Parameters::On_Restore)
	EVT_UPDATE_UI		(ID_BTN_RESTORE, CActive_Parameters::On_Restore_UI)

	EVT_BUTTON			(ID_BTN_EXECUTE, CActive_Parameters::On_Execute)
	EVT_BUTTON			(ID_BTN_LOAD   , CActive_Parameters::On_Load)
	EVT_BUTTON			(ID_BTN_SAVE   , CActive_Parameters::On_Save)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CActive_Parameters::CActive_Parameters(wxWindow *pParent)
	: wxPanel(pParent, ID_WND_ACTIVE_PARAMETERS, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxTAB_TRAVERSAL)
{
	m_pItem			= NULL;
	m_pControl		= new CParameters_Control(this);

	//-----------------------------------------------------
	m_Btn_Apply		= new wxButton(this, ID_BTN_APPLY  , CTRL_Get_Name(ID_BTN_APPLY  ), wxPoint(0, 0));
	m_Btn_Restore	= new wxButton(this, ID_BTN_RESTORE, CTRL_Get_Name(ID_BTN_RESTORE), wxPoint(0, 0));
	m_Btn_Execute	= new wxButton(this, ID_BTN_EXECUTE, CTRL_Get_Name(ID_BTN_EXECUTE), wxPoint(0, 0));
	m_Btn_Load		= new wxButton(this, ID_BTN_LOAD   , CTRL_Get_Name(ID_BTN_LOAD   ), wxPoint(0, 0));
	m_Btn_Save		= new wxButton(this, ID_BTN_SAVE   , CTRL_Get_Name(ID_BTN_SAVE   ), wxPoint(0, 0));

	m_Btn_Apply		->Show(false);
	m_Btn_Restore	->Show(false);
	m_Btn_Execute	->Show(false);
	m_Btn_Load		->Show(false);
	m_Btn_Save		->Show(false);

	m_btn_height	= m_Btn_Apply->GetDefaultSize().y;

	m_pControl->Set_Parameters(NULL);

	Set_Parameters(NULL);
}

//---------------------------------------------------------
CActive_Parameters::~CActive_Parameters(void)
{}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Parameters::On_Size(wxSizeEvent &event)
{
	_Set_Positions();

	event.Skip();
}

//---------------------------------------------------------
void CActive_Parameters::_Set_Positions(void)
{
	int	nButtons	= (m_Btn_Apply  ->IsShown() ? 1 : 0)
					+ (m_Btn_Restore->IsShown() ? 1 : 0)
					+ (m_Btn_Execute->IsShown() ? 1 : 0)
					+ (m_Btn_Load   ->IsShown() ? 1 : 0)
					+ (m_Btn_Save   ->IsShown() ? 1 : 0);

	wxRect	r(GetClientRect());

	if( nButtons > 0 )
	{
		#define BUTTON_DIST	1

		m_pControl->SetSize(r.GetTop(), r.GetLeft(), r.GetWidth(), r.GetHeight() - (m_btn_height + 4 * BUTTON_DIST));

		r	= wxRect(
			r.GetLeft  () + BUTTON_DIST,
			r.GetBottom() - m_btn_height,
			r.GetWidth () / nButtons - 2 * BUTTON_DIST,
			m_btn_height
		);

		#define SET_BTN_POS(BTN)	if( BTN->IsShown() ) { BTN->SetSize(r); r.SetLeft(r.GetLeft() + r.GetWidth() + 2 * BUTTON_DIST); }

		SET_BTN_POS(m_Btn_Apply  );
		SET_BTN_POS(m_Btn_Restore);
		SET_BTN_POS(m_Btn_Execute);
		SET_BTN_POS(m_Btn_Load   );
		SET_BTN_POS(m_Btn_Save   );
	}
	else
	{
		m_pControl->SetSize(r);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Parameters::On_Key(wxKeyEvent &event)
{
	if( event.GetKeyCode() == WXK_RETURN && m_pItem )
	{
		if( m_pControl->is_Modified() )
		{
			Update_Parameters(m_pItem->Get_Parameters(), true);
		}
		else if( m_pItem->Get_Type() == WKSP_ITEM_Tool )
		{
			m_pControl->Save_Changes(true);

			((CWKSP_Tool *)m_pItem)->Execute(false);
		}
	}

	event.Skip();
}

//---------------------------------------------------------
void CActive_Parameters::On_Apply(wxCommandEvent &event)
{
	if( m_pItem )
	{
		Update_Parameters(m_pItem->Get_Parameters(), true);
	}
}

void CActive_Parameters::On_Apply_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pControl->is_Modified());
}

//---------------------------------------------------------
void CActive_Parameters::On_Restore(wxCommandEvent &event)
{
	Restore_Parameters();
}

void CActive_Parameters::On_Restore_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pControl->is_Modified());
}

//---------------------------------------------------------
void CActive_Parameters::On_Execute(wxCommandEvent &event)
{
	if( m_pItem && m_pItem->Get_Type() == WKSP_ITEM_Tool )
	{
		m_pControl->Save_Changes(true);

		((CWKSP_Tool *)m_pItem)->Execute(false);
	}
}

//---------------------------------------------------------
void CActive_Parameters::On_Load(wxCommandEvent &event)
{
	m_pControl->Load();
}

//---------------------------------------------------------
void CActive_Parameters::On_Save(wxCommandEvent &event)
{
	m_pControl->Save();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CActive_Parameters::Set_Parameters(CWKSP_Base_Item *pItem)
{
	if( m_pItem && m_pControl->Save_Changes(false) )
	{
		m_pItem->Parameters_Changed();

		switch( m_pItem->Get_Type() )
		{
		case WKSP_ITEM_Shapes    :
		case WKSP_ITEM_TIN       :
		case WKSP_ITEM_PointCloud:
		case WKSP_ITEM_Grid      :
		case WKSP_ITEM_Grids     :
			g_pMaps->Update((CWKSP_Layer *)m_pItem, false);
			break;

		default:
			break;
		}
	}

	//-----------------------------------------------------
	if( m_pItem != pItem )
	{
		m_pItem	= pItem;

		Freeze();

		if( m_pItem && m_pItem->Get_Parameters() )
		{
			m_pControl->Set_Parameters(m_pItem->Get_Parameters());

			m_Btn_Apply  ->Show(true);
			m_Btn_Restore->Show(true);
			m_Btn_Execute->Show(m_pItem->Get_Type() == WKSP_ITEM_Tool);
			m_Btn_Load   ->Show(true);
			m_Btn_Save   ->Show(true);
		}
		else
		{
			m_pControl->Set_Parameters(NULL);

			m_Btn_Apply  ->Show(false);
			m_Btn_Restore->Show(false);
			m_Btn_Execute->Show(false);
			m_Btn_Load   ->Show(false);
			m_Btn_Save   ->Show(false);
		}

		if( m_pItem )
		{
			_Set_Positions();
		}

		Update();

		Thaw();
	}

	return( true );
}

//---------------------------------------------------------
bool CActive_Parameters::Update_Parameters(CSG_Parameters *pParameters, bool bSave)
{
	if( pParameters && m_pItem && (m_pItem->Get_Parameters() == pParameters || m_pControl->Get_Parameters() == pParameters) )
	{
		CWKSP_Base_Item	*pItem	= m_pItem;	m_pItem	= NULL;

		if( bSave )
		{
			m_pControl->Save_Changes(true);

			pItem->Parameters_Changed();
		}
		else
		{
			m_pControl->Set_Parameters(pParameters);
		}

		m_pItem	= pItem;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CActive_Parameters::Update_DataObjects(void)
{
	return( m_pControl->Update_DataObjects() );
}

//---------------------------------------------------------
bool CActive_Parameters::Restore_Parameters(void)
{
	return( m_pControl->Restore() );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
