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
#include <wx/event.h>
#include <wx/button.h>

#include "res_controls.h"

#include "wksp_layer.h"
#include "wksp_module.h"
#include "wksp_map_manager.h"

#include "parameters_control.h"

#include "active_parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CACTIVE_Parameters, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CACTIVE_Parameters, wxPanel)
	EVT_SIZE			(CACTIVE_Parameters::On_Size)

	EVT_BUTTON			(ID_BTN_APPLY	, CACTIVE_Parameters::On_Apply)
	EVT_UPDATE_UI		(ID_BTN_APPLY	, CACTIVE_Parameters::On_Apply_UI)
	EVT_BUTTON			(ID_BTN_RESTORE	, CACTIVE_Parameters::On_Restore)
	EVT_UPDATE_UI		(ID_BTN_RESTORE	, CACTIVE_Parameters::On_Restore_UI)

	EVT_BUTTON			(ID_BTN_EXECUTE	, CACTIVE_Parameters::On_Execute)
	EVT_BUTTON			(ID_BTN_LOAD	, CACTIVE_Parameters::On_Load)
	EVT_BUTTON			(ID_BTN_SAVE	, CACTIVE_Parameters::On_Save)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CACTIVE_Parameters::CACTIVE_Parameters(wxWindow *pParent)
	: wxPanel(pParent, ID_WND_ACTIVE_PARAMETERS, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
	m_pItem			= NULL;
	m_pControl		= new CParameters_Control(this);

	//-----------------------------------------------------
	m_Btn_Apply		= new wxButton(this, ID_BTN_APPLY	, CTRL_Get_Name(ID_BTN_APPLY)	, wxPoint(0, 0));
	m_Btn_Restore	= new wxButton(this, ID_BTN_RESTORE	, CTRL_Get_Name(ID_BTN_RESTORE)	, wxPoint(0, 0));
	m_Btn_Execute	= new wxButton(this, ID_BTN_EXECUTE	, CTRL_Get_Name(ID_BTN_EXECUTE)	, wxPoint(0, 0));
	m_Btn_Load		= new wxButton(this, ID_BTN_LOAD	, CTRL_Get_Name(ID_BTN_LOAD)	, wxPoint(0, 0));
	m_Btn_Save		= new wxButton(this, ID_BTN_SAVE	, CTRL_Get_Name(ID_BTN_SAVE)	, wxPoint(0, 0));

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
CACTIVE_Parameters::~CACTIVE_Parameters(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Parameters::On_Size(wxSizeEvent &WXUNUSED(event))
{
	_Set_Positions();
}

//---------------------------------------------------------
#define BUTTON_DIST			1

#define SET_BTN_POS(BTN)	if( BTN->IsShown() ) { BTN->SetSize(r); r.SetLeft(r.GetLeft() + r.GetWidth() + 2 * BUTTON_DIST); }

//---------------------------------------------------------
void CACTIVE_Parameters::_Set_Positions(void)
{
	int		nButtons;

	nButtons	= (m_Btn_Apply	->IsShown() ? 1 : 0)
				+ (m_Btn_Restore->IsShown() ? 1 : 0)
				+ (m_Btn_Execute->IsShown() ? 1 : 0)
				+ (m_Btn_Load	->IsShown() ? 1 : 0)
				+ (m_Btn_Save	->IsShown() ? 1 : 0);

	wxRect	r(GetClientRect());

	if( nButtons > 0 )
	{
		m_pControl->SetSize(r.GetTop(), r.GetLeft(), r.GetWidth(), r.GetHeight() - (m_btn_height + 4 * BUTTON_DIST));

		r	= wxRect(
			r.GetLeft  () + BUTTON_DIST,
			r.GetBottom() - m_btn_height,
			r.GetWidth () / nButtons - 2 * BUTTON_DIST,
			m_btn_height
		);

		SET_BTN_POS(m_Btn_Apply);
		SET_BTN_POS(m_Btn_Restore);
		SET_BTN_POS(m_Btn_Execute);
		SET_BTN_POS(m_Btn_Load);
		SET_BTN_POS(m_Btn_Save);
	}
	else
	{
		m_pControl->SetSize(r);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CACTIVE_Parameters::On_Apply(wxCommandEvent &event)
{
	if( m_pItem )
	{
		Update_Parameters(m_pItem->Get_Parameters(), true);
	}
}

void CACTIVE_Parameters::On_Apply_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pControl->is_Modified());
}

//---------------------------------------------------------
void CACTIVE_Parameters::On_Restore(wxCommandEvent &event)
{
	Restore_Parameters();
}

void CACTIVE_Parameters::On_Restore_UI(wxUpdateUIEvent &event)
{
	event.Enable(m_pControl->is_Modified());
}

//---------------------------------------------------------
void CACTIVE_Parameters::On_Execute(wxCommandEvent &event)
{
	if( m_pItem && m_pItem->Get_Type() == WKSP_ITEM_Module )
	{
		m_pControl->Save_Changes(true);

		((CWKSP_Module *)m_pItem)->Execute(false);
	}
}

//---------------------------------------------------------
void CACTIVE_Parameters::On_Load(wxCommandEvent &event)
{
	m_pControl->Load();
}

//---------------------------------------------------------
void CACTIVE_Parameters::On_Save(wxCommandEvent &event)
{
	m_pControl->Save();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CACTIVE_Parameters::Set_Parameters(CWKSP_Base_Item *pItem)
{
	if( m_pItem && m_pControl->Save_Changes(false) )
	{
		m_pItem->Parameters_Changed();

		switch( m_pItem->Get_Type() )
		{
		default:
			break;

		case WKSP_ITEM_Shapes:
		case WKSP_ITEM_TIN:
		case WKSP_ITEM_PointCloud:
		case WKSP_ITEM_Grid:
			g_pMaps->Update((CWKSP_Layer *)m_pItem, false);
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

			m_Btn_Apply		->Show(true);
			m_Btn_Restore	->Show(true);
			m_Btn_Execute	->Show(m_pItem->Get_Type() == WKSP_ITEM_Module);
			m_Btn_Load		->Show(true);
			m_Btn_Save		->Show(true);
		}
		else
		{
			m_pControl->Set_Parameters(NULL);

			m_Btn_Apply		->Show(false);
			m_Btn_Restore	->Show(false);
			m_Btn_Execute	->Show(false);
			m_Btn_Load		->Show(false);
			m_Btn_Save		->Show(false);
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
bool CACTIVE_Parameters::Update_Parameters(CSG_Parameters *pParameters, bool bSave)
{
	if( pParameters && m_pItem && (m_pItem->Get_Parameters() == pParameters || m_pControl->Get_Parameters() == pParameters) )
	{
		if( bSave )
		{
			m_pControl->Save_Changes(true);

			m_pItem->Parameters_Changed();
		}
		else
		{
			m_pControl->Set_Parameters(pParameters);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CACTIVE_Parameters::Update_DataObjects(void)
{
	return( m_pControl->Update_DataObjects() );
}

//---------------------------------------------------------
bool CACTIVE_Parameters::Restore_Parameters(void)
{
	return( m_pControl->Restore() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
