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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include <wx/wx.h>
#include <wx/textdlg.h>

#include <saga_api/saga_api.h>

#include "helper.h"
#include "dc_helper.h"

#include "dlg_colors_control.h"

//---------------------------------------------------------
#define BOX_DISTANCE	10


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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Colors_Control::CDLG_Colors_Control(wxWindow *pParent, CSG_Colors *pColors)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize)
{
	m_pColors	= pColors;
	m_selBox	= -1;
}

//---------------------------------------------------------
CDLG_Colors_Control::~CDLG_Colors_Control(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Colors_Control::Set_Positions(void)
{
	int		Height, Position;
	wxRect	r(wxPoint(0, 0), GetClientSize());

	r.Deflate(BOX_DISTANCE);

	Height	= (int)(r.GetHeight() / (4 + 0.25));

	Position	= r.GetTop();
	m_red		= wxRect(r.GetX(), Position, r.GetWidth(), Height - BOX_DISTANCE);

	Position	+= Height;
	m_green		= wxRect(r.GetX(), Position, r.GetWidth(), Height - BOX_DISTANCE);

	Position	+= Height;
	m_blue		= wxRect(r.GetX(), Position, r.GetWidth(), Height - BOX_DISTANCE);

	Position	+= Height;
	m_sum		= wxRect(r.GetX(), Position, r.GetWidth(), Height - BOX_DISTANCE);

	Position	+= Height;
	m_rgb		= wxRect(r.GetX(), Position, r.GetWidth(), (int)(Height * 0.25));

	Refresh();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
	wxPaintDC	dc(this);

	Draw_Edge(dc, EDGE_STYLE_STATIC, wxRect(wxPoint(0, 0), GetClientSize()));

	Draw_Box(dc, 0);
	Draw_Box(dc, 1);
	Draw_Box(dc, 2);
	Draw_Box(dc, 3);
	Draw_Box(dc, 4);
}

//---------------------------------------------------------
void CDLG_Colors_Control::Draw_Box(wxDC &dc, int BoxID)
{
	int		i, r, g, b, s, ax, ay, bx, by;
	double	xStep, yStep;
	wxRect	rBox;

	//-----------------------------------------------------
	Get_BoxRect(rBox, BoxID);

	rBox.Inflate(1);
	Draw_FillRect(dc, SYS_Get_Color(wxSYS_COLOUR_WINDOW), rBox);
	Draw_Edge(dc, EDGE_STYLE_SUNKEN, rBox);
	rBox.Deflate(1);

	//-----------------------------------------------------
	xStep	= (double)rBox.GetWidth() / (double)m_pColors->Get_Count();
	yStep	= (double)rBox.GetHeight() / 255.0;

	ax		= rBox.GetLeft();
	ay		= rBox.GetBottom() + 1;

	r	= g	= b	= 0;
	s	= 255;

	//-----------------------------------------------------
	for(i=0; i<m_pColors->Get_Count(); i++)
	{
		switch( BoxID )
		{
		case 0:
			r	= s	= m_pColors->Get_Red  (i);
			break;

		case 1:
			g	= s	= m_pColors->Get_Green(i);
			break;

		case 2:
			b	= s	= m_pColors->Get_Blue (i);
			break;

		case 3:
			r	= m_pColors->Get_Red  (i);
			g	= m_pColors->Get_Green(i);
			b	= m_pColors->Get_Blue (i);
			s	= (r + g + b) / 3;
			break;

		case 4:
			r	= m_pColors->Get_Red  (i);
			g	= m_pColors->Get_Green(i);
			b	= m_pColors->Get_Blue (i);
			break;
		}

		bx	= ax;
		ax	= rBox.GetLeft() + (int)(xStep * (i + 1.0));
		by	= ay - (int)(yStep * (double)s);

		Draw_FillRect(dc, wxColour(r, g, b), ax, ay, bx, by);
	}
}

//---------------------------------------------------------
void CDLG_Colors_Control::Draw_Line(wxPoint pA, wxPoint pB)
{
	wxClientDC	dc(this);

	dc.SetLogicalFunction(wxINVERT);
	dc.DrawLine(pA.x, pA.y, pB.x, pB.y);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Colors_Control::On_Mouse_LDown(wxMouseEvent &event)
{
	m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();

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
		Draw_Line(m_Mouse_Down, m_Mouse_Move);
		m_Mouse_Move	= event.GetPosition();
		KeepInBoxRect(m_Mouse_Move, m_selBox);
		Draw_Line(m_Mouse_Down, m_Mouse_Move);
	}
}

//---------------------------------------------------------
void CDLG_Colors_Control::On_Mouse_LUp(wxMouseEvent &event)
{
	if( m_selBox >= 0 )
	{
		Draw_Line(m_Mouse_Down, m_Mouse_Move);

		m_Mouse_Move	= event.GetPosition();

		Set_Colors(m_Mouse_Down, m_Mouse_Move, m_selBox);

		m_selBox		= -1;

		ReleaseMouse();

		Refresh(false);
	}
}

//---------------------------------------------------------
#include "res_dialogs.h"

void CDLG_Colors_Control::On_Mouse_RUp(wxMouseEvent &event)
{
	wxPoint	P	= event.GetPosition();
	int	selBox	= Get_SelBox(P);

	if( selBox >= 0 )
	{
		P	= Get_ColorPosition(P, selBox);

		long	Color;

		if( DLG_Color(Color = m_pColors->Get_Color(P.x)) )
		{
			m_pColors->Set_Color(P.x, Color);

			Refresh(false);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxPoint CDLG_Colors_Control::Get_ColorPosition(wxPoint p, int BoxID)
{
	wxRect	r;

	if( Get_BoxRect(r, BoxID) )
	{
		KeepInBoxRect(p, BoxID);

		p.x	= (int)((p.x   - r.GetLeft()) * m_pColors->Get_Count() / (double)r.GetWidth ());
		p.y	= (int)((r.GetBottom() - p.y) * 256.0                  / (double)r.GetHeight());
	}

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

	double	dy	= A.x == B.x ? 0.0 : (B.y - A.y) / (double)(B.x - A.x);

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDLG_Colors_Control::Get_SelBox(wxPoint p)
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
bool CDLG_Colors_Control::IsInBoxRect(wxPoint p, int BoxID)
{
	wxRect	r;

	if( Get_BoxRect(r, BoxID) )
	{
		if(	p.x >= r.GetLeft()	&& p.x <= r.GetRight()
		&&	p.y >= r.GetTop()	&& p.y <= r.GetBottom()	)
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
void CDLG_Colors_Control::KeepInBoxRect(wxPoint &p, int BoxID)
{
	wxRect	r;

	if( Get_BoxRect(r, BoxID) )
	{
		if( p.x < r.GetLeft() )
		{
			p.x	= r.GetLeft();
		}
		else if( p.x > r.GetRight() )
		{
			p.x	= r.GetRight();
		}
		
		if( p.y < r.GetTop() )
		{
			p.y	= r.GetTop();
		}
		else if( p.y > r.GetBottom() )
		{
			p.y	= r.GetBottom();
		}
	}
}

//---------------------------------------------------------
bool CDLG_Colors_Control::Get_BoxRect(wxRect &r, int BoxID)
{
	switch( BoxID )
	{
	case 0:	r	= m_red;	break;
	case 1:	r	= m_green;	break;
	case 2:	r	= m_blue;	break;
	case 3:	r	= m_sum;	break;
	case 4:	r	= m_rgb;	break;
	default:	return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
