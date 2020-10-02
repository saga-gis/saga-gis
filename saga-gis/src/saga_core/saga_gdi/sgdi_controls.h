
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    sgdi_controls.h                    //
//                                                       //
//                 Copyright (C) 2020 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_GDI_sgdi_controls_H
#define HEADER_INCLUDED__SAGA_GDI_sgdi_controls_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgdi_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wxprec.h>

#include <wx/dc.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/image.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SGDI_CTRL_WIDTH				100
#define SGDI_CTRL_SPACE				10
#define SGDI_CTRL_SMALLSPACE		2

#define SGDI_BTN_HEIGHT				25
#define SGDI_BTN_WIDTH				SGDI_CTRL_WIDTH
#define SGDI_BTN_SIZE				wxSize(SGDI_BTN_WIDTH, SGDI_BTN_HEIGHT)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SGDI_API_DLL_EXPORT CSGDI_Slider : public wxSlider
{
public:
	CSGDI_Slider(wxWindow *pParent, int ID, double Value, double minValue, double maxValue, const wxPoint &Point = wxDefaultPosition, const wxSize &Size = wxDefaultSize, long Style = wxSL_HORIZONTAL);
	virtual ~CSGDI_Slider(void);

	bool				Set_Value			(double Value);
	double				Get_Value			(void);

	double				Get_Min				(void)	{	return( m_Min );	}
	double				Get_Max				(void)	{	return( m_Max );	}
	double				Get_Range			(void)	{	return( m_Max - m_Min );	}
	bool				Set_Range			(double minValue, double maxValue);


private:

	double				m_Min, m_Max;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SGDI_API_DLL_EXPORT CSGDI_SpinCtrl : public wxSpinCtrl
{
public:
	CSGDI_SpinCtrl(wxWindow *pParent, int ID, double Value, double minValue, double maxValue, bool bPercent = false, const wxPoint &Point = wxDefaultPosition, const wxSize &Size = wxDefaultSize, long Style = wxSP_ARROW_KEYS);
	virtual ~CSGDI_SpinCtrl(void);

	bool				Set_Value			(double Value);
	double				Get_Value			(void);

	double				Get_Min				(void)	{	return( m_Min );	}
	double				Get_Max				(void)	{	return( m_Max );	}
	double				Get_Range			(void)	{	return( m_Max - m_Min );	}
	bool				Set_Range			(double minValue, double maxValue);


private:

	bool				m_bPercent;

	double				m_Min, m_Max;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_GDI_sgdi_controls_H
