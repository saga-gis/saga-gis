
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
//                DLG_Colors_Control.cpp                 //
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
#include <wx/wx.h>
#include <wx/textdlg.h>

#include <saga_api/saga_api.h>
#include <saga_gdi/sgdi_helper.h>

#include "helper.h"
#include "res_dialogs.h"

#include "dlg_colors_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_Colors_Control, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Colors_Control, wxPanel)
	EVT_SIZE			(CDLG_Colors_Control::On_Size)
	EVT_PAINT			(CDLG_Colors_Control::On_Paint)

	EVT_LEFT_DOWN		(CDLG_Colors_Control::On_Mouse_LDown)
	EVT_MOTION			(CDLG_Colors_Control::On_Mouse_Motion)
	EVT_LEFT_UP			(CDLG_Colors_Control::On_Mouse_LUp)
	EVT_RIGHT_DOWN		(CDLG_Colors_Control::On_Mouse_RUp)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Colors_Control::CDLG_Colors_Control(wxWindow *pParent, CSG_Colors *pColors)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize)
{
	m_pColors = pColors;
	m_selBox  = -1;
}

//---------------------------------------------------------
CDLG_Colors_Control::~CDLG_Colors_Control(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Colors_Control::Set_Positions(void)
{
	#define BOX_DISTANCE	10

	wxRect	r(wxPoint(0, 0), GetClientSize());

	r.Deflate(BOX_DISTANCE);

	int	h	= (int)(r.GetHeight() / (4 + 0.25));
	int	p	= r.GetTop();

	m_red	= wxRect(r.GetX(), p, r.GetWidth(), h - BOX_DISTANCE); p += h;
	m_green	= wxRect(r.GetX(), p, r.GetWidth(), h - BOX_DISTANCE); p += h;
	m_blue	= wxRect(r.GetX(), p, r.GetWidth(), h - BOX_DISTANCE); p += h;
	m_sum	= wxRect(r.GetX(), p, r.GetWidth(), h - BOX_DISTANCE); p += h;
	m_rgb	= wxRect(r.GetX(), p, r.GetWidth(), (int)(h * 0.25));

	Refresh();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Colors_Control::On_Size(wxSizeEvent &event)
{
	Set_Positions();
}

//---------------------------------------------------------
void CDLG_Colors_Control::On_Paint(wxPaintEvent &event)
{
	wxPaintDC dc(this);

	Draw_Edge(dc, EDGE_STYLE_STATIC, wxRect(wxPoint(0, 0), GetClientSize()));

	Draw_Box(dc, 0);
	Draw_Box(dc, 1);
	Draw_Box(dc, 2);
	Draw_Box(dc, 3);
	Draw_Box(dc, 4);

	if( m_selBox >= 0 && m_Mouse_Down != m_Mouse_Move )
	{
		wxRect r(Get_BoxRect(m_selBox).Deflate(2)); r.Offset(0, -1);
		wxPoint p(m_Mouse_Move); KeepInBoxRect(p, r);

		#ifdef _SAGA_MSW
	//	dc.SetLogicalFunction(wxINVERT);
		#endif

		dc.SetPen(wxPen(*wxWHITE, 3)); dc.DrawLine(m_Mouse_Down, p);
		dc.SetPen(wxPen(*wxBLACK, 1)); dc.DrawLine(m_Mouse_Down, p);
	}
}

//---------------------------------------------------------
void CDLG_Colors_Control::Draw_Box(wxDC &dc, int BoxID)
{
	wxRect rBox(Get_BoxRect(BoxID));

	rBox.Inflate(1); Draw_Edge(dc, EDGE_STYLE_SUNKEN, rBox); rBox.Deflate(1);

	//-----------------------------------------------------
	double xStep = (double)rBox.GetWidth () / (double)m_pColors->Get_Count();
	double yStep = (double)rBox.GetHeight() / 255.;

	int r = 0, g = 0, b = 0, s = 255;

	//-----------------------------------------------------
	for(int i=0, ax=rBox.GetLeft(), ay=rBox.GetBottom(); i<m_pColors->Get_Count(); i++)
	{
		switch( BoxID )
		{
		case 0:
			r = s = m_pColors->Get_Red  (i);
			break;

		case 1:
			g = s = m_pColors->Get_Green(i);
			break;

		case 2:
			b = s = m_pColors->Get_Blue (i);
			break;

		case 3:
			r     = m_pColors->Get_Red  (i);
			g     = m_pColors->Get_Green(i);
			b     = m_pColors->Get_Blue (i);
			s     = (r + g + b) / 3;
			break;

		case 4:
			r     = m_pColors->Get_Red  (i);
			g     = m_pColors->Get_Green(i);
			b     = m_pColors->Get_Blue (i);
			break;
		}

		int bx = ax; ax	= rBox.GetLeft() + (int)(xStep * (i + 1.));
		int by = ay - (int)(yStep * (double)s);

		if( BoxID != 4 )
		{
			Draw_FillRect(dc, SYS_Get_Color(wxSYS_COLOUR_WINDOW), ax, by, bx, rBox.GetTop()-1);
		}

		Draw_FillRect(dc, wxColour(r, g, b), ax, ay, bx, by);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Colors_Control::On_Mouse_LDown(wxMouseEvent &event)
{
	m_Mouse_Down = m_Mouse_Move = event.GetPosition();

	if( (m_selBox = Get_SelBox(m_Mouse_Down)) >= 0 )
	{
		CaptureMouse();
	}
}

//---------------------------------------------------------
void CDLG_Colors_Control::On_Mouse_Motion(wxMouseEvent &event)
{
	if( m_selBox >= 0 )
	{
		KeepInBoxRect(m_Mouse_Move = event.GetPosition(), m_selBox);

		wxRect r(Get_BoxRect(m_selBox));

		RefreshRect(r, false);
	}
}

//---------------------------------------------------------
void CDLG_Colors_Control::On_Mouse_LUp(wxMouseEvent &event)
{
	if( m_selBox >= 0 )
	{
		Set_Colors(m_Mouse_Down, event.GetPosition(), m_selBox);

		m_selBox = -1;

		ReleaseMouse();

		Refresh(false);
	}
}

//---------------------------------------------------------
void CDLG_Colors_Control::On_Mouse_RUp(wxMouseEvent &event)
{
	int selBox = Get_SelBox(event.GetPosition());

	if( selBox >= 0 )
	{
		wxPoint P = Get_ColorPosition(event.GetPosition(), selBox);
		long    C = m_pColors->Get_Color(P.x);

		if( DLG_Color(C) )
		{
			m_pColors->Set_Color(P.x, C);
			
			Refresh(false);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxPoint CDLG_Colors_Control::Get_ColorPosition(wxPoint p, int BoxID)
{
	wxRect r(Get_BoxRect(BoxID));

	KeepInBoxRect(p, BoxID);

	int dy = r.GetBottom() - p.y;

	p.x	= (int)((p.x   - r.GetLeft()) * m_pColors->Get_Count() / (r.GetWidth () - 1.));
	p.y	= (int)((r.GetBottom() - p.y) * 255.                   / (r.GetHeight() - 1.));

	return( p );
}

//---------------------------------------------------------
void CDLG_Colors_Control::Set_Colors(wxPoint A, wxPoint B, int BoxID) 
{
	A	= Get_ColorPosition(A, BoxID);
	B	= Get_ColorPosition(B, BoxID);

	if( A.x > B.x )
	{
		wxPoint	P = A; A = B; B = P;
	}

	if( A.x < 0 )
	{
		A.x	= 0;
	}

	if( B.x >= m_pColors->Get_Count() )
	{
		B.x	= m_pColors->Get_Count() - 1;
	}

	double dy = A.x == B.x ? 0. : (B.y - A.y) / (double)(B.x - A.x);

	//-----------------------------------------------------
	for(int iColor=A.x, x=0; iColor<=B.x; iColor++, x++)
	{
		int	Color	= (int)(0.5 + A.y + dy * x);

		switch( BoxID )
		{
		case 0: m_pColors->Set_Red       (iColor, Color); break;
		case 1: m_pColors->Set_Green     (iColor, Color); break;
		case 2: m_pColors->Set_Blue      (iColor, Color); break;
		case 3: m_pColors->Set_Brightness(iColor, Color); break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDLG_Colors_Control::Get_SelBox(const wxPoint &p)
{
	for(int i=0; i<4; i++)
	{
		if( IsInBoxRect(p, i) )
		{
			return( i );
		}
	}

	return( -1 );
}

//---------------------------------------------------------
bool CDLG_Colors_Control::IsInBoxRect(const wxPoint &p, int BoxID)
{
	wxRect r(Get_BoxRect(BoxID));

	return( p.x >= r.GetLeft() && p.x <= r.GetRight ()
		&&  p.y >= r.GetTop () && p.y <= r.GetBottom() );
}

//---------------------------------------------------------
void CDLG_Colors_Control::KeepInBoxRect(wxPoint &p, const wxRect &r)
{
	if( p.x < r.GetLeft() ) { p.x = r.GetLeft(); } else if( p.x > r.GetRight () ) { p.x = r.GetRight (); }		
	if( p.y < r.GetTop () ) { p.y = r.GetTop (); } else if( p.y > r.GetBottom() ) { p.y = r.GetBottom(); }
}

//---------------------------------------------------------
void CDLG_Colors_Control::KeepInBoxRect(wxPoint &p, int BoxID)
{
	KeepInBoxRect(p, Get_BoxRect(BoxID));
}

//---------------------------------------------------------
wxRect CDLG_Colors_Control::Get_BoxRect(int BoxID)
{
	switch( BoxID )
	{
	case 0: return( m_red   ); break;
	case 1: return( m_green ); break;
	case 2: return( m_blue  ); break;
	case 3: return( m_sum   ); break;
	case 4: return( m_rgb   ); break;
	}

	return( wxRect() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
