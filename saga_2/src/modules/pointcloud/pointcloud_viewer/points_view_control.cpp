
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
//                points_view_control.cpp                //
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
#include "points_view_control.h"
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
BEGIN_EVENT_TABLE(CPoints_View_Control, wxPanel)
	EVT_SIZE				(CPoints_View_Control::On_Size)
	EVT_ERASE_BACKGROUND	(CPoints_View_Control::On_EraseBackGround)
	EVT_PAINT				(CPoints_View_Control::On_Paint)
	EVT_KEY_DOWN			(CPoints_View_Control::On_Key_Down)
	EVT_LEFT_DOWN			(CPoints_View_Control::On_Mouse_LDown)
	EVT_LEFT_UP				(CPoints_View_Control::On_Mouse_LUp)
	EVT_RIGHT_DOWN			(CPoints_View_Control::On_Mouse_RDown)
	EVT_RIGHT_UP			(CPoints_View_Control::On_Mouse_RUp)
	EVT_MOTION				(CPoints_View_Control::On_Mouse_Motion)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_View_Control::CPoints_View_Control(wxWindow *pParent, CSG_PointCloud *pPoints, CSG_Parameters &Settings)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE)
{
	m_pPoints	= pPoints;

	m_pSettings	= &Settings;

	m_zField	= 2;
	m_cField	= 2;

	m_xRotate	= 0.0;
	m_yRotate	= 0.0;
	m_zRotate	= 0.0;

	m_xShift	= 0.0;
	m_yShift	= 0.0;
	m_zShift	= 1000.0;

	m_bCentral	= true;
	m_bStereo	= false;
	m_bScale	= false;

	m_dCentral	= 500.0;
	m_Detail	= 1.0;

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= m_pSettings->Add_Node(NULL, "NODE_CONTROL", _TL("3D View"), _TL(""));

	m_pSettings->Add_Colors(
		pNode	, "COLORS"			, _TL("Colors"),
		_TL("")
	);

	m_pSettings->Add_Value(
		pNode	, "BGCOLOR"			, _TL("Background Color"),
		_TL(""),
		PARAMETER_TYPE_Color, 0
	);

	m_pSettings->Add_Range(
		pNode	, "C_RANGE"			, _TL("Colors Value Range"),
		_TL("")
	);

	m_pSettings->Add_Value(
		pNode	, "SIZE_DEF"		, _TL("Point Size: Default"),
		_TL(""),
		PARAMETER_TYPE_Int, 0, 0, true
	);

	m_pSettings->Add_Value(
		pNode	, "SIZE_SCALE"		, _TL("Point Size: Scaling"),
		_TL(""),
		PARAMETER_TYPE_Double, 250.0, 1.0, true
	);

	m_pSettings->Add_Value(
		pNode	, "EXAGGERATION"	, _TL("Exaggeration"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);

//	m_pSettings->Add_Value(
//		pNode	, "STEREO"			, _TL("Stereo Anaglyph"),
//		_TL(""),
//		PARAMETER_TYPE_Bool, false
//	);

	m_pSettings->Add_Value(
		pNode	, "STEREO_DIST"		, _TL("Stereo Eye Distance [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	//-----------------------------------------------------
	m_pSelection	= (int *)SG_Malloc(m_pPoints->Get_Count() * sizeof(int));

	Update_Extent(m_pPoints->Get_Extent());
}

//---------------------------------------------------------
CPoints_View_Control::~CPoints_View_Control(void)
{
	if( m_pSelection )
	{
		SG_Free(m_pSelection);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPoints_View_Control::On_Size(wxSizeEvent &event)
{
	_Set_Size();

	event.Skip();
}

//---------------------------------------------------------
void CPoints_View_Control::On_EraseBackGround(wxEraseEvent &event)
{
//	event.Skip();
}

//---------------------------------------------------------
void CPoints_View_Control::On_Paint(wxPaintEvent &WXUNUSED(event))
{
	if( m_Image.IsOk() && m_Image.GetWidth() > 0 && m_Image.GetHeight() > 0 )
	{
		wxPaintDC	dc(this);

		dc.DrawBitmap(wxBitmap(m_Image), 0, 0, false);
	}
}

//---------------------------------------------------------
void CPoints_View_Control::Update_View(void)
{
	if( _Draw_Image() )
	{
		wxClientDC	dc(this);

		dc.DrawBitmap(wxBitmap(m_Image), 0, 0, false);
	}
}

//---------------------------------------------------------
void CPoints_View_Control::Update_Extent(CSG_Rect Extent)
{
	m_Extent.Assign(Extent);

	m_zStats.Invalidate();
	m_cStats.Invalidate();

	m_nSelection	= 0;

	for(int i=0; i<m_pPoints->Get_Count(); i++)
	{
		m_pPoints->Set_Cursor(i);

		TSG_Point_3D	p	= m_pPoints->Get_Point();

		if( m_Extent.Contains(p.x, p.y) )
		{
			m_pSelection[m_nSelection++]	= i;

			m_zStats.Add_Value(m_pPoints->Get_Value(m_zField));
			m_cStats.Add_Value(m_pPoints->Get_Value(m_cField));
		}
	}

	m_Settings("C_RANGE")->asRange()->Set_Range(
		m_cStats.Get_Mean() - 1.5 * m_cStats.Get_StdDev(),
		m_cStats.Get_Mean() + 1.5 * m_cStats.Get_StdDev()
	);

	Update_View();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPoints_View_Control::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:
		event.Skip();
		break;

	case WXK_F10:				break;
	case WXK_UP:		m_Extent.Deflate(10.0);	Update_View();		break;
	case WXK_DOWN:				break;
	}
}

//---------------------------------------------------------
#define GET_MOUSE_X_RELDIFF	((double)(m_Mouse_Down.x - event.GetX()) / (double)GetClientSize().x)
#define GET_MOUSE_Y_RELDIFF	((double)(m_Mouse_Down.y - event.GetY()) / (double)GetClientSize().y)

//---------------------------------------------------------
void CPoints_View_Control::On_Mouse_LDown(wxMouseEvent &event)
{
	m_Mouse_Down	= event.GetPosition();
	m_xDown			= m_zRotate;
	m_yDown			= m_xRotate;

	CaptureMouse();
}

//---------------------------------------------------------
void CPoints_View_Control::On_Mouse_LUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Mouse_Down.x != event.GetX() || m_Mouse_Down.y != event.GetY() )
	{
		m_zRotate	= m_xDown + GET_MOUSE_X_RELDIFF * M_PI_180;
		m_xRotate	= m_yDown + GET_MOUSE_Y_RELDIFF * M_PI_180;

		Update_View();

		((CPoints_View_Dialog *)GetParent())->Update_Rotation();
	}
}

//---------------------------------------------------------
void CPoints_View_Control::On_Mouse_RDown(wxMouseEvent &event)
{
	m_Mouse_Down	= event.GetPosition();
	m_yDown			= m_yShift;
	m_xDown			= m_zShift;

	CaptureMouse();
}

//---------------------------------------------------------
void CPoints_View_Control::On_Mouse_RUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Mouse_Down.x != event.GetX() || m_Mouse_Down.y != event.GetY() )
	{
		m_yShift	= m_yDown - GET_MOUSE_Y_RELDIFF * 1000.0;
		m_zShift	= m_xDown + GET_MOUSE_X_RELDIFF * 1000.0;

		Update_View();

		((CPoints_View_Dialog *)GetParent())->Update_Rotation();
	}
}

//---------------------------------------------------------
void CPoints_View_Control::On_Mouse_Motion(wxMouseEvent &event)
{
	if( HasCapture() && event.Dragging() )
	{
		if( event.LeftIsDown() )
		{
			m_zRotate	= m_xDown + GET_MOUSE_X_RELDIFF * M_PI_180;
			m_xRotate	= m_yDown + GET_MOUSE_Y_RELDIFF * M_PI_180;
		}
		else if( event.RightIsDown() )
		{
			m_yShift	= m_yDown - GET_MOUSE_Y_RELDIFF * 1000.0;
			m_zShift	= m_xDown + GET_MOUSE_X_RELDIFF * 1000.0;
		}
		else
		{
			return;
		}

		Update_View();

		((CPoints_View_Dialog *)GetParent())->Update_Rotation();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPoints_View_Control::_Set_Size(void)
{
	Update_View();
}

//---------------------------------------------------------
bool CPoints_View_Control::_Draw_Image(void)
{
	wxSize	dcSize	= GetClientSize();

	if( m_pPoints->Get_Count() <= 0
	||	dcSize.x <= 0 || dcSize.y <= 0
	||	m_Extent.Get_XRange() <= 0.0 || m_Extent.Get_YRange() <= 0.0
	||	m_zField < 0 || m_zField >= m_pPoints->Get_Field_Count()
	||	m_cField < 0 || m_cField >= m_pPoints->Get_Field_Count()
	||	m_zStats.Get_Range() <= 0.0 )
	{
		return( false );
	}

	//-------------------------------------------------
	if( !m_Image.IsOk() || dcSize.x != m_Image.GetWidth() || dcSize.y != m_Image.GetHeight() )
	{
		m_Image		.Create(dcSize.x, dcSize.y);
		m_Image_zMax.Create(dcSize.x, dcSize.y);
	}

	//-------------------------------------------------
	if( m_Settings("C_RANGE")->asRange()->Get_LoVal() >= m_Settings("C_RANGE")->asRange()->Get_HiVal() )
	{
		m_Settings("C_RANGE")->asRange()->Set_Range(
			m_cStats.Get_Mean() - 1.5 * m_pPoints->Get_StdDev(m_cField),
			m_cStats.Get_Mean() + 1.5 * m_pPoints->Get_StdDev(m_cField)
		);
	}

	m_pColors	= m_Settings("COLORS")->asColors();
	m_cMin		= m_Settings("C_RANGE")->asRange()->Get_LoVal();
	m_cScale	= m_pColors->Get_Count() / (m_Settings("C_RANGE")->asRange()->Get_HiVal() - m_cMin);

	//-------------------------------------------------
	m_Image_zMax.Assign(999999.0);

	//-------------------------------------------------
	if( (dcSize.x / (double)dcSize.y) > (m_Extent.Get_XRange() / m_Extent.Get_YRange()) )
	{
		r_Scale	= m_Image.GetWidth() / m_Extent.Get_XRange();
	}
	else
	{
		r_Scale	= m_Image.GetHeight() / m_Extent.Get_YRange();
	}

	r_sin_x	= sin(m_xRotate - M_PI_180);
	r_cos_x	= cos(m_xRotate - M_PI_180);
	r_sin_y	= sin(m_yRotate);
	r_cos_y	= cos(m_yRotate);
	r_sin_z	= sin(m_zRotate);
	r_cos_z	= cos(m_zRotate);

	r_xc	= m_Extent.Get_XCenter();
	r_yc	= m_Extent.Get_YCenter();
	r_zc	= m_zStats.Get_Minimum() + 0.5 * m_zStats.Get_Range();

	r_Scale_z	= r_Scale * m_Settings("EXAGGERATION")->asDouble();

	//-------------------------------------------------
	int		ix, iy, ic, nSkip, iSelection;
	double	iz;

	//-------------------------------------------------
	m_Size_Def		= m_Settings("SIZE_DEF")->asInt();
	m_Size_Scale	= 1.0 / m_Settings("SIZE_SCALE")->asDouble();

	//-------------------------------------------------
	nSkip	= 1 + (int)(0.001 * m_pPoints->Get_Count() * SG_Get_Square(1.0 - m_Detail));

	if( m_bStereo == false )
	{
		_Draw_Background(m_Settings("BGCOLOR")->asColor());

		m_Color_Mode	= COLOR_MODE_RGB;

		for(iSelection=0; iSelection<m_nSelection; iSelection+=nSkip)
		{
			_Get_Point(m_pSelection[iSelection], ix, iy, iz, ic);
			_Draw_Point(ix, iy, iz, ic, _Get_Size(iz));
		}
	}
	else
	{
		ic	= m_Settings("BGCOLOR")->asColor();
		ic	= (int)((SG_GET_R(ic) + SG_GET_G(ic) + SG_GET_B(ic)) / 3.0);
		_Draw_Background(SG_GET_RGB(ic, ic, ic));

		double	d	= m_Settings("STEREO_DIST")->asDouble() / 2.0;

		r_sin_y	= sin(m_yRotate - d * M_DEG_TO_RAD);
		r_cos_y	= cos(m_yRotate - d * M_DEG_TO_RAD);

		m_Color_Mode	= COLOR_MODE_RED;

		for(iSelection=0; iSelection<m_nSelection; iSelection+=nSkip)
		{
			_Get_Point(m_pSelection[iSelection], ix, iy, iz, ic);
			_Draw_Point(ix, iy, iz, ic, _Get_Size(iz));
		}

		m_Image_zMax.Assign(999999.0);

		r_sin_y	= sin(m_yRotate + d * M_DEG_TO_RAD);
		r_cos_y	= cos(m_yRotate + d * M_DEG_TO_RAD);

		m_Color_Mode	= COLOR_MODE_BLUE;

		for(iSelection=0; iSelection<m_nSelection; iSelection+=nSkip)
		{
			_Get_Point(m_pSelection[iSelection], ix, iy, iz, ic);
			_Draw_Point(ix, iy, iz, ic, _Get_Size(iz));
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
inline int CPoints_View_Control::_Get_Size(double zDistance)
{
	return( m_Size_Def + (!m_bScale ? 0 : (int)(20.0 * exp(-m_Size_Scale * zDistance))) );
}

//---------------------------------------------------------
inline void CPoints_View_Control::_Get_Point(int iPoint, int &ix, int &iy, double &iz, int &iColor)
{
	TSG_Point_3D	p;

	p		= m_pPoints->Get_Point(iPoint);
	p.z		= m_pPoints->Get_Value(iPoint, m_zField);

	p		= _Get_Projection(p);

	ix		= (int)p.x + 0.5 * m_Image.GetWidth();
	iy		= (int)p.y + 0.5 * m_Image.GetHeight();
	iz		= p.z;

	iColor	= (int)(m_cScale * (m_pPoints->Get_Value(iPoint, m_cField) - m_cMin));
	iColor	= m_pColors->Get_Color(iColor < 0 ? 0 : (iColor >= m_pColors->Get_Count() ? m_pColors->Get_Count() - 1 : iColor));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPoints_View_Control::_Draw_Background(int color)
{
	BYTE	r, g, b, *pRGB;
	int		i, n;

	r	= SG_GET_R(color);
	g	= SG_GET_G(color);
	b	= SG_GET_B(color);

	n	= m_Image.GetWidth() * m_Image.GetHeight();

	for(i=0, pRGB=m_Image.GetData(); i<n; i++)
	{
		*pRGB	= r;	pRGB++;
		*pRGB	= g;	pRGB++;
		*pRGB	= b;	pRGB++;
	}
}

//---------------------------------------------------------
inline void CPoints_View_Control::_Draw_Point(int x, int y, double z, int color, int Size)
{
	if( z > 0.0 )
	{
		_Draw_Pixel(x, y, z, color);

		if( Size > 0 && Size < 50 )
		{
			for(int iy=1; iy<=Size; iy++)
			{
				for(int ix=0; ix<=Size; ix++)
				{
					if( ix*ix + iy*iy <= Size*Size )
					{
						_Draw_Pixel(x + ix, y + iy, z, color);
						_Draw_Pixel(x + iy, y - ix, z, color);
						_Draw_Pixel(x - ix, y - iy, z, color);
						_Draw_Pixel(x - iy, y + ix, z, color);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
inline void CPoints_View_Control::_Draw_Pixel(int x, int y, double z, int color)
{
	if( x >= 0 && x < m_Image.GetWidth() && y >= 0 && y < m_Image.GetHeight() && z < m_Image_zMax[y][x] )
	{
		BYTE	*pRGB	= m_Image.GetData() + 3 * (y * m_Image.GetWidth() + x);

		switch( m_Color_Mode )
		{
		case COLOR_MODE_RGB:
			*pRGB		= SG_GET_R(color);	pRGB++;
			*pRGB		= SG_GET_G(color);	pRGB++;
			*pRGB		= SG_GET_B(color);
			break;

		case COLOR_MODE_RED:
			*pRGB		= ((SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3);
			break;

		case COLOR_MODE_BLUE:
			*(pRGB + 1)	= ((SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3);
			*(pRGB + 2)	= ((SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3);
			break;
		}

		m_Image_zMax[y][x]	= z;
	}
}

//---------------------------------------------------------
inline TSG_Point_3D CPoints_View_Control::_Get_Projection(TSG_Point_3D &p)
{
	TSG_Point_3D	q;

	p.x	= (p.x - r_xc) * r_Scale;
	p.y	= (p.y - r_yc) * r_Scale;
	p.z	= (p.z - r_zc) * r_Scale_z;

	double	a	= (r_cos_y * p.z + r_sin_y * (r_sin_z * p.y + r_cos_z * p.x));
	double	b	= (r_cos_z * p.y - r_sin_z * p.x);

	q.x	= r_cos_y * (r_sin_z * p.y + r_cos_z * p.x) - r_sin_y * p.z;
	q.y	= r_sin_x * a + r_cos_x * b;
	q.z	= r_cos_x * a - r_sin_x * b;

	q.x	+= m_xShift;
	q.y	+= m_yShift;
	q.z	+= m_zShift;

	if( m_bCentral )
	{
		q.x	*= m_dCentral / q.z;
		q.y	*= m_dCentral / q.z;
	}
	else
	{
		double	z	= m_dCentral / m_zShift;
		q.x	*= z;
		q.y	*= z;
	//	q.z	 = -q.z;
	}

	return( q );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
