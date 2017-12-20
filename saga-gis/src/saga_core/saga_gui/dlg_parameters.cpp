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
//                  DLG_Parameters.cpp                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include "res_controls.h"

#include "wksp_base_item.h"

#include "active.h"
#include "active_parameters.h"
#include "active_description.h"

#include "parameters_control.h"

#include "dlg_parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	CDLG_Parameters::m_bInfo	= false;

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_Parameters, CDLG_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Parameters, CDLG_Base)
	EVT_BUTTON(wxID_OK           , CDLG_Parameters::On_Ok      )
	EVT_BUTTON(ID_BTN_LOAD       , CDLG_Parameters::On_Load    )
	EVT_BUTTON(ID_BTN_SAVE       , CDLG_Parameters::On_Save    )
	EVT_BUTTON(ID_BTN_DEFAULTS   , CDLG_Parameters::On_Defaults)
	EVT_BUTTON(ID_BTN_DESCRIPTION, CDLG_Parameters::On_Info    )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Parameters::CDLG_Parameters(CSG_Parameters *pParameters, const wxString &Caption, const wxString &Info)
	: CDLG_Base(-1, Caption)
{
	if( Caption.IsEmpty() )
	{
		if( pParameters && !pParameters->Get_Name().is_Empty() )
		{
			SetTitle(pParameters->Get_Name().c_str());
		}
		else
		{
			SetTitle(_TL("Parameters"));
		}
	}

	m_pControl		= new CParameters_Control(this, true);

	m_pParameters	= pParameters;

	g_pACTIVE->Get_Parameters()->Update_Parameters(m_pParameters, true);

	m_pControl->Set_Parameters(m_pParameters);

	Add_Button(ID_BTN_LOAD);
	Add_Button(ID_BTN_SAVE);
	Add_Button(ID_BTN_DEFAULTS);

	if( Info.IsEmpty() )
	{
		m_pInfo_Button	= NULL;
		m_pInfo			= NULL;
	}
	else
	{
		Add_Button(0);

		wxString	_Info(Info); _Info.Replace("\n", "<br>");

		m_pInfo_Button	= Add_Button(ID_BTN_DESCRIPTION);
		m_pInfo			= new CACTIVE_Description(this);
		m_pInfo->SetPage(_Info);

		Show_Info(m_bInfo);
	}

	Set_Positions();
}

//---------------------------------------------------------
CDLG_Parameters::~CDLG_Parameters(void)
{
	if( m_pInfo )
	{
		m_bInfo	= m_pInfo->IsShown();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Parameters::On_Ok(wxCommandEvent &event)
{
	if( m_pControl->Get_Parameters()->DataObjects_Check() )
	{
		CDLG_Base::On_Ok(event);
	}
}

//---------------------------------------------------------
void CDLG_Parameters::On_Load(wxCommandEvent &event)
{
	m_pControl->Load();
}

//---------------------------------------------------------
void CDLG_Parameters::On_Save(wxCommandEvent &event)
{
	m_pControl->Save();
}

//---------------------------------------------------------
void CDLG_Parameters::On_Defaults(wxCommandEvent &event)
{
	m_pControl->Restore_Defaults();
}

//---------------------------------------------------------
void CDLG_Parameters::On_Info(wxCommandEvent &event)
{
	if( m_pInfo )
	{
		Show_Info(!m_pInfo->IsShown());
	}
}

//---------------------------------------------------------
void CDLG_Parameters::Show_Info(bool bShow)
{
	if( m_pInfo )
	{
		m_pInfo_Button->SetLabel(wxString::Format("%s %s", _TL("Info"), bShow ? wxT("<<") : wxT(">>")));

		if( bShow != m_pInfo->IsShown() )
		{
			m_pInfo->Show(bShow);

			Set_Positions();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Parameters::Save_Changes(void)
{
	m_pControl->Save_Changes(true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Parameters::Set_Position(wxRect r)
{
	if( m_pInfo && m_pInfo->IsShown() )
	{
		r.SetWidth(r.GetWidth() / 2 - 4);
		m_pInfo->SetSize(r);

		r.SetLeft(r.GetRight() + 9);
		m_pControl->SetSize(r);
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
