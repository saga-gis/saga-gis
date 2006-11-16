
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
#include <saga_api/saga_api.h>

#include "res_controls.h"

#include "wksp_base_item.h"

#include "active.h"
#include "active_parameters.h"

#include "parameters_control.h"

#include "dlg_parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_Parameters, CDLG_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Parameters, CDLG_Base)
	EVT_BUTTON			(wxID_OK		, CDLG_Parameters::On_Ok)
	EVT_BUTTON			(ID_BTN_LOAD	, CDLG_Parameters::On_Load)
	EVT_BUTTON			(ID_BTN_SAVE	, CDLG_Parameters::On_Save)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Parameters::CDLG_Parameters(CSG_Parameters *pParameters)
	: CDLG_Base(-1, pParameters ? pParameters->Get_Name() : LNG("[CAP] Parameters"))
{
	m_pControl		= new CParameters_Control(this, true);

	m_pParameters	= pParameters;

	g_pACTIVE->Get_Parameters()->Update_Parameters(m_pParameters, true);

	m_pControl->Set_Parameters(m_pParameters);

	Add_Button(ID_BTN_LOAD);
	Add_Button(ID_BTN_SAVE);

	Set_Positions();
}

//---------------------------------------------------------
CDLG_Parameters::~CDLG_Parameters(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Parameters::Save_Changes(void)
{
	m_pControl->Save_Changes(true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
