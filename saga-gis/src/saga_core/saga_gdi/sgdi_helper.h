
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
//                    sgdi_helper.h                      //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#ifndef HEADER_INCLUDED__SAGA_GDI_sgdi_helper_H
#define HEADER_INCLUDED__SAGA_GDI_sgdi_helper_H


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
#define TEXTALIGN_LEFT				0x01
#define TEXTALIGN_XCENTER			0x02
#define TEXTALIGN_RIGHT				0x04
#define TEXTALIGN_TOP				0x08
#define TEXTALIGN_YCENTER			0x10
#define TEXTALIGN_BOTTOM			0x20

#define TEXTALIGN_TOPLEFT			(TEXTALIGN_TOP    |TEXTALIGN_LEFT)
#define TEXTALIGN_TOPCENTER			(TEXTALIGN_TOP    |TEXTALIGN_XCENTER)
#define TEXTALIGN_TOPRIGHT			(TEXTALIGN_TOP    |TEXTALIGN_RIGHT)
#define TEXTALIGN_CENTERLEFT		(TEXTALIGN_YCENTER|TEXTALIGN_LEFT)
#define TEXTALIGN_CENTER			(TEXTALIGN_YCENTER|TEXTALIGN_XCENTER)
#define TEXTALIGN_CENTERRIGHT		(TEXTALIGN_YCENTER|TEXTALIGN_RIGHT)
#define TEXTALIGN_BOTTOMLEFT		(TEXTALIGN_BOTTOM |TEXTALIGN_LEFT)
#define TEXTALIGN_BOTTOMCENTER		(TEXTALIGN_BOTTOM |TEXTALIGN_XCENTER)
#define TEXTALIGN_BOTTOMRIGHT		(TEXTALIGN_BOTTOM |TEXTALIGN_RIGHT)

//---------------------------------------------------------
SGDI_API_DLL_EXPORT void	Draw_Text	(wxDC &dc, int Align, int x, int y, const wxString &Text);
SGDI_API_DLL_EXPORT void	Draw_Text	(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text);

SGDI_API_DLL_EXPORT bool	Draw_Ruler	(wxDC &dc, const wxRect &r, bool bHorizontal, double zMin, double zMax, bool bAscendent = true, int FontSize = 7, const wxColour &Colour = wxColour(127, 127, 127));


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
#endif // #ifndef HEADER_INCLUDED__SAGA_GDI_sgdi_helper_H
