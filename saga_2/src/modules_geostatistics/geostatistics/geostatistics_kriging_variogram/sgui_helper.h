
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
//                    sgui_helper.h                      //
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
#ifndef HEADER_INCLUDED__SGUI_Helper_H
#define HEADER_INCLUDED__SGUI_Helper_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"

#ifdef geostatistics_kriging_variogram_EXPORTS
	#define	SGUI_API_DLL_EXPORT		_SAGA_DLL_EXPORT
#else
	#define	SGUI_API_DLL_EXPORT		_SAGA_DLL_IMPORT
#endif

// #ifdef SGUI_API_EXPORTS
// 	#define	SGUI_API_DLL_EXPORT		__declspec( dllexport )
// #else
// 	#define	SGUI_API_DLL_EXPORT		__declspec( dllimport )
// #endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <wx/dc.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/dialog.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SGUI_CTRL_WIDTH				100
#define SGUI_CTRL_SPACE				10
#define SGUI_CTRL_SMALLSPACE		2

#define SGUI_BTN_HEIGHT				25
#define SGUI_BTN_WIDTH				SGUI_CTRL_WIDTH
#define SGUI_BTN_SIZE				wxSize(SGUI_BTN_WIDTH, SGUI_BTN_HEIGHT)


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
SGUI_API_DLL_EXPORT void	Draw_Text	(wxDC &dc, int Align, int x, int y, const wxString &Text);
SGUI_API_DLL_EXPORT void	Draw_Text	(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text);

SGUI_API_DLL_EXPORT bool	Draw_Ruler	(wxDC &dc, const wxRect &r, bool bHorizontal, double zMin, double zMax, bool bAscendent = true, int FontSize = 7, const wxColour &Colour = wxColour(127, 127, 127));


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SGUI_API_DLL_EXPORT CSGUI_Slider : public wxSlider
{
public:
	CSGUI_Slider(wxWindow *pParent, int ID, double Value, double minValue, double maxValue, const wxPoint &Point = wxDefaultPosition, const wxSize &Size = wxDefaultSize, long Style = wxSL_HORIZONTAL);
	virtual ~CSGUI_Slider(void);

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
class SGUI_API_DLL_EXPORT CSGUI_SpinCtrl : public wxSpinCtrl
{
public:
	CSGUI_SpinCtrl(wxWindow *pParent, int ID, double Value, double minValue, double maxValue, bool bPercent = false, const wxPoint &Point = wxDefaultPosition, const wxSize &Size = wxDefaultSize, long Style = wxSP_ARROW_KEYS);
	virtual ~CSGUI_SpinCtrl(void);

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
#endif // #ifndef HEADER_INCLUDED__SGUI_Helper_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
