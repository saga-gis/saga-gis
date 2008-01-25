
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
//                    sgui_diagram.cpp                   //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <wx/settings.h>
#include <wx/dc.h>

#include <saga_api/saga_api.h>

//---------------------------------------------------------
#include "sgui_diagram.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSGUI_Diagram, wxPanel)
	EVT_PAINT				(CSGUI_Diagram::_On_Paint)
	EVT_LEFT_DOWN			(CSGUI_Diagram::_On_Mouse_Click_Left)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSGUI_Diagram::CSGUI_Diagram(wxWindow *pParent)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxSUNKEN_BORDER)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	m_xName		= LNG("X");
	m_yName		= LNG("Y");
}

//---------------------------------------------------------
CSGUI_Diagram::~CSGUI_Diagram(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSGUI_Diagram::_On_Mouse_Click_Left(wxMouseEvent &WXUNUSED(event))
{
//	wxMessageBox("Sonk", "Sonk");
}

//---------------------------------------------------------
void CSGUI_Diagram::_On_Paint(wxPaintEvent &WXUNUSED(event))
{
	wxPaintDC	dc(this);

	_Draw(dc);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RULER_LABEL_HEIGHT	20

//---------------------------------------------------------
bool CSGUI_Diagram::_Draw(wxDC &dc)
{
	if( m_xMin < m_xMax && m_yMin < m_yMax )
	{
		double	dx, dy;

		m_rDiagram	= wxRect(RULER_LABEL_HEIGHT, 0, GetClientSize().x - RULER_LABEL_HEIGHT, GetClientSize().y - RULER_LABEL_HEIGHT);

		dc.SetTextForeground(wxColour(0, 0, 0));
		dc.SetFont(wxFont((int)(0.5 * RULER_LABEL_HEIGHT), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

		Draw_Text(dc, TEXTALIGN_TOPCENTER   , 0, m_rDiagram.GetY() + m_rDiagram.GetHeight() / 2, 90, m_yName);
		Draw_Text(dc, TEXTALIGN_BOTTOMCENTER, m_rDiagram.GetX() + m_rDiagram.GetWidth() / 2, GetClientSize().y, m_xName);

		Draw_Ruler(dc, m_rDiagram, true , m_xMin, m_xMax);
		Draw_Ruler(dc, m_rDiagram, false, m_yMin, m_yMax);

		//---------------------------------------------------------------------
		dx	= m_rDiagram.GetWidth()  / (m_xMax - m_xMin);
		dy	= m_rDiagram.GetHeight() / (m_yMax - m_yMin);

		On_Draw(dc, m_rDiagram);

		return( true );
	}

	dc.DrawLine(0, 0, GetClientSize().x, GetClientSize().y);
	dc.DrawLine(0, GetClientSize().y, GetClientSize().x, 0);

	return( false );
}

//---------------------------------------------------------
#define DIAGRAM_BUFFER	100

//---------------------------------------------------------
int CSGUI_Diagram::Get_xToScreen(double x, bool bKeepInRange)
{
	int		i	= m_rDiagram.GetX() + (int)(m_rDiagram.GetWidth () * (x - m_xMin) / (m_xMax - m_xMin));

	if( bKeepInRange )
	{
		if( i		< m_rDiagram.GetLeft  () - DIAGRAM_BUFFER )
			i		= m_rDiagram.GetLeft  () - DIAGRAM_BUFFER;
		else if( i	> m_rDiagram.GetRight () + DIAGRAM_BUFFER )
			i		= m_rDiagram.GetRight () + DIAGRAM_BUFFER;
	}

	return( i );
}

//---------------------------------------------------------
int CSGUI_Diagram::Get_yToScreen(double y, bool bKeepInRange)
{
	int		i	= m_rDiagram.GetY() - (int)(m_rDiagram.GetHeight() * (y - m_yMin) / (m_yMax - m_yMin)) + m_rDiagram.GetHeight();

	if( bKeepInRange )
	{
		if( i		< m_rDiagram.GetTop   () - DIAGRAM_BUFFER )
			i		= m_rDiagram.GetTop   () - DIAGRAM_BUFFER;
		else if( i	> m_rDiagram.GetBottom() + DIAGRAM_BUFFER )
			i		= m_rDiagram.GetBottom() + DIAGRAM_BUFFER;
	}

	return( i );
}

//---------------------------------------------------------
bool CSGUI_Diagram::Get_ToScreen(wxPoint &Point, double x, double y)
{
	bool	bResult	= true;

	Point.x	= Get_xToScreen(x, false);

	if( Point.x			< m_rDiagram.GetLeft  () - DIAGRAM_BUFFER )
	{
		Point.x			= m_rDiagram.GetLeft  () - DIAGRAM_BUFFER;
		bResult			= false;
	}
	else if( Point.x	> m_rDiagram.GetRight () + DIAGRAM_BUFFER )
	{
		Point.x			= m_rDiagram.GetRight () + DIAGRAM_BUFFER;
		bResult			= false;
	}

	Point.y	= Get_yToScreen(y, false);

	if( Point.y			< m_rDiagram.GetTop   () - DIAGRAM_BUFFER )
	{
		Point.y			= m_rDiagram.GetTop   () - DIAGRAM_BUFFER;
		bResult			= false;
	}
	else if( Point.y	> m_rDiagram.GetBottom() + DIAGRAM_BUFFER )
	{
		Point.y			= m_rDiagram.GetBottom() + DIAGRAM_BUFFER;
		bResult			= false;
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
