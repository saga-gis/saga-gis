
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     SAGA_GUI_API                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     sgui_dialog.h                     //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    SAGA User Group Association            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SGUI_Dialog_H
#define HEADER_INCLUDED__SGUI_Dialog_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgui_helper.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SGUI_API_DLL_EXPORT CSGUI_Dialog : public wxDialog
{
public:
	CSGUI_Dialog(const wxString &Name);
	virtual ~CSGUI_Dialog(void);


protected:

	void					Add_Spacer			(int Space = SGUI_CTRL_SPACE);
	wxButton *				Add_Button			(const wxString &Name, int ID, const wxSize &Size = SGUI_BTN_SIZE);
	wxChoice *				Add_Choice			(const wxString &Name, const wxArrayString &Choices, int iSelect = 0, int ID = wxID_ANY);
	wxCheckBox *			Add_CheckBox		(const wxString &Name, bool bCheck, int ID = wxID_ANY);
	wxTextCtrl *			Add_TextCtrl		(const wxString &Name, int Style = 0, const wxString &Text = wxT(""), int ID = wxID_ANY);
	CSGUI_Slider *			Add_Slider			(const wxString &Name, double Value, double minValue, double maxValue, bool bValueAsPercent = false, int ID = wxID_ANY, int Width = SGUI_CTRL_WIDTH);
	CSGUI_SpinCtrl *		Add_SpinCtrl		(const wxString &Name, double Value, double minValue, double maxValue, bool bValueAsPercent = false, int ID = wxID_ANY, int Width = SGUI_CTRL_WIDTH);

	bool					Add_Output			(wxWindow *pOutput);
	bool					Add_Output			(wxWindow *pOutput_A, wxWindow *pOutput_B, int Proportion_A = 1, int Proportion_B = 0);


private:

	wxColour				m_Ctrl_Color;

	wxSizer					*m_pSizer_Ctrl, *m_pSizer_Output;


	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SGUI_Dialog_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
