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
class CDLG_Info : public wxDialog
{
public:
	CDLG_Info(wxWindow *pParent, const wxString &Info)
		: wxDialog(pParent, wxID_ANY, _TL("Info"), m_Position.GetTopLeft(), m_Position.GetSize(), wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER) // |wxSTAY_ON_TOP
	{
		CACTIVE_Description	*pDescription	= new CACTIVE_Description(this);

		pDescription->SetPage(Info);
	}

	virtual ~CDLG_Info(void)
	{
		m_bShow		= IsShown();
		m_Position	= GetRect();
	}


	static bool		m_bShow;

	static wxRect	m_Position;


	void			On_Close	(wxCloseEvent &event)
	{
		((CDLG_Parameters *)GetParent())->Show_Info(false);
	}

	DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Info, wxDialog)
	EVT_CLOSE(CDLG_Info::On_Close)
END_EVENT_TABLE()

//---------------------------------------------------------
bool	CDLG_Info::m_bShow	= false;
wxRect	CDLG_Info::m_Position(wxRect(wxDefaultPosition, wxDefaultSize));


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

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

		m_pInfo_Button	= Add_Button(ID_BTN_DESCRIPTION);
		m_pInfo			= new CDLG_Info(this, Info);
	}

	Set_Positions();

//	Show_Info(CDLG_Info::m_bShow);
}

//---------------------------------------------------------
CDLG_Parameters::~CDLG_Parameters(void)
{}


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
	if( m_pInfo && bShow != m_pInfo->IsShown() )
	{
#ifdef _SAGA_MSW
		m_pInfo->Show(bShow);	// unluckily this does not work with linux (broken event handler chain, non-modal dialog as subprocess of a modal one!!)
		m_pInfo_Button->SetLabel(wxString::Format("%s %s", _TL("Info"), bShow ? wxT("<<") : wxT(">>")));
#else
		if( bShow )
		{
			m_pInfo->ShowModal();
		}
#endif
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
	m_pControl->SetSize(r);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
