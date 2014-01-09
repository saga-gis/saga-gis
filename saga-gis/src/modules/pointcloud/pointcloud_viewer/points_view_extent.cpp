/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                 points_view_extent.cpp                //
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

//---------------------------------------------------------
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <wx/settings.h>
#include <wx/dc.h>
#include <wx/dcclient.h>

#include <saga_api/saga_api.h>

//---------------------------------------------------------
#include "points_view_extent.h"
#include "points_view_dialog.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define m_Settings	(*m_pSettings)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CPoints_View_Extent, wxPanel)
	EVT_SIZE				(CPoints_View_Extent::On_Size)
	EVT_ERASE_BACKGROUND	(CPoints_View_Extent::On_EraseBackGround)
	EVT_PAINT				(CPoints_View_Extent::On_Paint)
	EVT_LEFT_DOWN			(CPoints_View_Extent::On_Mouse_LDown)
	EVT_LEFT_UP				(CPoints_View_Extent::On_Mouse_LUp)
	EVT_RIGHT_DOWN			(CPoints_View_Extent::On_Mouse_RDown)
	EVT_RIGHT_UP			(CPoints_View_Extent::On_Mouse_RUp)
	EVT_MOTION				(CPoints_View_Extent::On_Mouse_Motion)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_View_Extent::CPoints_View_Extent(wxWindow *pParent, CSG_PointCloud *pPoints, CSG_Parameters &Settings, wxSize Size)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, Size, wxTAB_TRAVERSAL|wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE)
{
	m_pPoints	= pPoints;

	m_pSettings	= &Settings;

	m_cField	= 2;

	m_Select	= wxRect(0, 0, GetClientSize().x - 1, GetClientSize().y - 1);

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= m_pSettings->Add_Node(NULL, "NODE_EXTENT", _TL("Zoom View"), _TL(""));

	m_pSettings->Add_Range(
		pNode	, "EXT_C_RANGE"		, _TL("Colors Value Range"),
		_TL("")
	);

	//-----------------------------------------------------
	_Draw_Image();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPoints_View_Extent::On_Size(wxSizeEvent &event)
{
	_Draw_Image();

	event.Skip();
}

//---------------------------------------------------------
void CPoints_View_Extent::On_EraseBackGround(wxEraseEvent &event)
{
//	event.Skip();
}

//---------------------------------------------------------
void CPoints_View_Extent::On_Paint(wxPaintEvent &WXUNUSED(event))
{
	if( m_Image.IsOk() && m_Image.GetWidth() > 0 && m_Image.GetHeight() > 0 )
	{
		wxPaintDC	dc(this);

		dc.DrawBitmap(wxBitmap(m_Image), 0, 0, false);

		dc.SetPen(wxPen(*wxRED));

		int		ax	= m_Select.GetX();
		int		ay	= m_Select.GetY();
		int		bx	= m_Select.GetX() + m_Select.GetWidth();
		int		by	= m_Select.GetY() + m_Select.GetHeight();

		dc.DrawLine(ax, ay, bx, ay);
		dc.DrawLine(bx, ay, bx, by);
		dc.DrawLine(bx, by, ax, by);
		dc.DrawLine(ax, by, ax, ay);
	}
}

//---------------------------------------------------------
void CPoints_View_Extent::Update_View(void)
{
	if( _Draw_Image() )
	{
		Refresh(false);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPoints_View_Extent::On_Mouse_LDown(wxMouseEvent &event)
{
	m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();

	CaptureMouse();
}

//---------------------------------------------------------
void CPoints_View_Extent::On_Mouse_LUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	_Draw_Inverse(m_Mouse_Down, m_Mouse_Move);

	wxPoint	p;

	p.x	= event.GetX() < 0 ? 0 : event.GetX() >= GetClientSize().x ? GetClientSize().x - 1 : event.GetX();
	p.y	= event.GetY() < 0 ? 0 : event.GetY() >= GetClientSize().y ? GetClientSize().y - 1 : event.GetY();

	if( m_Mouse_Down.x != p.x || m_Mouse_Down.y != p.y )
	{
		m_Select	= wxRect(wxPoint(m_Mouse_Down.x, m_Mouse_Down.y), p);
	}
	else
	{
		m_Select.SetX(p.x - m_Select.GetWidth () / 2);
		m_Select.SetY(p.y - m_Select.GetHeight() / 2);
	}

	Refresh(false);

	((CPoints_View_Dialog *)GetParent())->Update_Extent();
}

//---------------------------------------------------------
void CPoints_View_Extent::On_Mouse_RDown(wxMouseEvent &event)
{
	m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();

	CaptureMouse();
}

//---------------------------------------------------------
void CPoints_View_Extent::On_Mouse_RUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	m_Select	= wxRect(0, 0, GetClientSize().x - 1, GetClientSize().y - 1);

	Refresh(false);

	((CPoints_View_Dialog *)GetParent())->Update_Extent();
}

//---------------------------------------------------------
void CPoints_View_Extent::On_Mouse_Motion(wxMouseEvent &event)
{
	if( HasCapture() && event.Dragging() )
	{
		if( event.LeftIsDown() )
		{
			_Draw_Inverse(m_Mouse_Down, m_Mouse_Move);
			_Draw_Inverse(m_Mouse_Down, event.GetPosition());
		}
	}

	m_Mouse_Move	= event.GetPosition();
}

//---------------------------------------------------------
TSG_Rect CPoints_View_Extent::Get_Extent(void)
{
	TSG_Rect	r;

	double	d	= m_Extent.Get_XRange() / m_Image.GetWidth();

	r.xMin	= m_Extent.Get_XMin() + d *  m_Select.GetX();
	r.xMax	= m_Extent.Get_XMin() + d * (m_Select.GetX() + m_Select.GetWidth());

	r.yMax	= m_Extent.Get_YMax() - d *  m_Select.GetY();
	r.yMin	= m_Extent.Get_YMax() - d * (m_Select.GetY() + m_Select.GetHeight());

	return( r );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_View_Extent::_Draw_Image(void)
{
	if( m_pPoints->Get_Count() <= 0 || m_pPoints->Get_Extent().Get_XRange() <= 0.0 || m_pPoints->Get_Extent().Get_YRange() <= 0.0
	||	m_cField < 0 || m_cField >= m_pPoints->Get_Field_Count() )
	{
		return( false );
	}

	//-------------------------------------------------
	wxSize		Size;
	double		dx, dy;

	Size		= GetClientSize();

	if (Size.x <= 0 || Size.y <= 0)	// temporary hack for wxGTK to suppress 'assert "isOK()" failed in Get_Height(): invalid image'
		return( false );			// when dlg.ShowModal() is called; during construction everything is fine, but the first call of
									// the On_Size() event returns a client size of 0

	m_Image			.Create(Size.x, Size.y);
	m_Image_Value	.Create(Size.x, Size.y);
	m_Image_Count	.Create(Size.x, Size.y);

	//-------------------------------------------------
	m_Extent	= m_pPoints->Get_Extent();

	if( (dx = Size.y / (double)Size.x) < (m_Extent.Get_YRange() / m_Extent.Get_XRange()) )
	{
		dx	= 0.5 * (m_Extent.Get_XRange() - m_Extent.Get_YRange() / dx);
		m_Extent.m_rect.xMin	+= dx;
		m_Extent.m_rect.xMax	-= dx;
	}
	else
	{
		dy	= 0.5 * (m_Extent.Get_YRange() - m_Extent.Get_XRange() * dx);
		m_Extent.m_rect.yMin	+= dy;
		m_Extent.m_rect.yMax	-= dy;
	}

	dx	= Size.x / m_Extent.Get_XRange();
	dy	= Size.y / m_Extent.Get_YRange();

	//-------------------------------------------------
	bool	bColorAsRGB	= m_Settings("C_AS_RGB")->asBool();

	for(int i=0; i<m_pPoints->Get_Count(); i++)
	{
		TSG_Point_Z	p	= m_pPoints->Get_Point(i);	p.z	= m_pPoints->Get_Value(i, m_cField);

		int	ix	= (p.x - m_Extent.Get_XMin()) * dx;
		int	iy	= (p.y - m_Extent.Get_YMin()) * dy;

		if( ix >= 0 && ix <= m_Image.GetWidth() && iy >= 0 && iy < m_Image.GetHeight() )
		{
			if( !bColorAsRGB )
			{
				m_Image_Value[iy][ix]	+= p.z;
			}
			else
			{
				m_Image_Value[iy][ix]	 = p.z;
			}

			m_Image_Count[iy][ix]++;
		}
	}

	//-------------------------------------------------
	double	zMin	= m_pPoints->Get_Mean(m_cField) - 1.5 * m_pPoints->Get_StdDev(m_cField);
	double	zRange	= m_pPoints->Get_Mean(m_cField) + 1.5 * m_pPoints->Get_StdDev(m_cField) - zMin;

	CSG_Colors	*pColors	= m_Settings("COLORS")->asColors();

	for(int iy=0; iy<m_Image.GetHeight(); iy++)
	{
		for(int ix=0; ix<m_Image.GetWidth(); ix++)
		{
			if( m_Image_Count[iy][ix] > 0 )
			{
				if( !bColorAsRGB )
				{
					int	ic	= (int)(pColors->Get_Count() * (m_Image_Value[iy][ix] / m_Image_Count[iy][ix] - zMin) / zRange);
					int	c	= pColors->Get_Color(ic < 0 ? 0 : ic >= pColors->Get_Count() ? pColors->Get_Count() - 1 : ic);

					_Draw_Pixel(ix, iy, c);
				}
				else
				{
					_Draw_Pixel(ix, iy, m_Image_Value[iy][ix]);
				}
			}
			else
			{
				_Draw_Pixel(ix, iy, 0);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CPoints_View_Extent::_Draw_Pixel(int x, int y, int color)
{
	if( x >= 0 && x < m_Image.GetWidth() && y >= 0 && y < m_Image.GetHeight() )
	{
		BYTE	*pRGB	= m_Image.GetData() + 3 * ((m_Image.GetHeight() - y - 1) * m_Image.GetWidth() + x);
		*pRGB	= SG_GET_R(color);	pRGB++;
		*pRGB	= SG_GET_G(color);	pRGB++;
		*pRGB	= SG_GET_B(color);
	}
}


//---------------------------------------------------------
void CPoints_View_Extent::_Draw_Inverse(wxPoint pa, wxPoint pb)
{
	wxClientDC	dc(this);

	dc.SetLogicalFunction(wxINVERT);

	dc.DrawRectangle(pa.x, pa.y, pb.x - pa.x, pb.y - pa.y);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
