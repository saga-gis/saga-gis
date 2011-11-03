/**********************************************************
 * Version $Id: 3dshapes_view_control.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    3dshapes_viewer                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               3dshapes_view_control.cpp               //
//                                                       //
//                 Copyright (C) 2011 by                 //
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

#include <wx/settings.h>
#include <wx/dc.h>
#include <wx/dcclient.h>

#include <saga_api/saga_api.h>

//---------------------------------------------------------
#include "3dshapes_view_control.h"
#include "3dshapes_view_dialog.h"


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
BEGIN_EVENT_TABLE(C3DShapes_View_Control, wxPanel)
	EVT_SIZE				(C3DShapes_View_Control::On_Size)
	EVT_PAINT				(C3DShapes_View_Control::On_Paint)
	EVT_KEY_DOWN			(C3DShapes_View_Control::On_Key_Down)
	EVT_LEFT_DOWN			(C3DShapes_View_Control::On_Mouse_LDown)
	EVT_LEFT_UP				(C3DShapes_View_Control::On_Mouse_LUp)
	EVT_RIGHT_DOWN			(C3DShapes_View_Control::On_Mouse_RDown)
	EVT_RIGHT_UP			(C3DShapes_View_Control::On_Mouse_RUp)
	EVT_MIDDLE_DOWN			(C3DShapes_View_Control::On_Mouse_MDown)
	EVT_MIDDLE_UP			(C3DShapes_View_Control::On_Mouse_MUp)
	EVT_MOTION				(C3DShapes_View_Control::On_Mouse_Motion)
	EVT_MOUSEWHEEL			(C3DShapes_View_Control::On_Mouse_Wheel)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3DShapes_View_Control::C3DShapes_View_Control(wxWindow *pParent, CSG_Shapes *pShapes, int Field_Color, CSG_Parameters &Settings)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE)
{
	m_pShapes		= pShapes;

	m_pSettings	= &Settings;

	m_cField	= Field_Color;

	m_Shading	= 1;

	m_Style		= 1;
	m_cWire		= SG_GET_RGB(150, 150, 150);

	m_xRotate	= 0.0;
	m_yRotate	= 0.0;
	m_zRotate	= 0.0;

	m_xShift	= 0.0;
	m_yShift	= 0.0;
	m_zShift	= 1000.0;

	m_bCentral	= true;
	m_bStereo	= false;
	m_bFrame	= true;

	m_Light_Hgt	=  45.0 * M_DEG_TO_RAD;
	m_Light_Dir	=  90.0 * M_DEG_TO_RAD;

	m_dCentral	= 500.0;

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
		pNode	, "COLOR_WIRE"		, _TL("Wire Frame Color"),
		_TL(""),
		PARAMETER_TYPE_Color, m_cWire
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

	m_pSettings->Add_Value(
		pNode	, "STEREO_DIST"		, _TL("Stereo Eye Distance [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	//-----------------------------------------------------
	Update_Extent();
}

//---------------------------------------------------------
C3DShapes_View_Control::~C3DShapes_View_Control(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3DShapes_View_Control::On_Size(wxSizeEvent &event)
{
	_Set_Size();

	event.Skip();
}

//---------------------------------------------------------
void C3DShapes_View_Control::On_Paint(wxPaintEvent &WXUNUSED(event))
{
	if( m_Image.IsOk() && m_Image.GetWidth() > 0 && m_Image.GetHeight() > 0 )
	{
		wxPaintDC	dc(this);

		dc.DrawBitmap(wxBitmap(m_Image), 0, 0, false);
	}
}

//---------------------------------------------------------
void C3DShapes_View_Control::Update_View(void)
{
	if( _Draw_Image() )
	{
		wxClientDC	dc(this);

		dc.DrawBitmap(wxBitmap(m_Image), 0, 0, false);
	}
}

//---------------------------------------------------------
void C3DShapes_View_Control::Update_Extent(void)
{
	m_Extent.Assign(m_pShapes->Get_Extent());

	m_zStats.Invalidate();
	m_cStats.Invalidate();

	for(int iShape=0; iShape<m_pShapes->Get_Count(); iShape++)
	{
		CSG_Shape	*pShape	= m_pShapes->Get_Shape(iShape);

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				m_zStats.Add_Value(pShape->Get_Z(iPoint, iPart));
			}
		}

		m_cStats.Add_Value(pShape->asDouble(m_cField));
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
void C3DShapes_View_Control::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:
		event.Skip();
		return;

	case WXK_NUMPAD_ADD:
	case WXK_ADD:		m_xRotate	-= 4.0 * M_DEG_TO_RAD;	break;
	case WXK_NUMPAD_SUBTRACT:
	case WXK_SUBTRACT:	m_xRotate	+= 4.0 * M_DEG_TO_RAD;	break;

	case WXK_F3:		m_yRotate	-= 4.0 * M_DEG_TO_RAD;	break;
	case WXK_F4:		m_yRotate	+= 4.0 * M_DEG_TO_RAD;	break;

	case WXK_NUMPAD_MULTIPLY:
	case WXK_MULTIPLY:	m_zRotate	-= 4.0 * M_DEG_TO_RAD;	break;
	case WXK_NUMPAD_DIVIDE:
	case WXK_DIVIDE:	m_zRotate	+= 4.0 * M_DEG_TO_RAD;	break;

	case WXK_INSERT:	m_xShift	-= 10.0;				break;
	case WXK_DELETE:	m_xShift	+= 10.0;				break;

	case WXK_HOME:		m_yShift	-= 10.0;				break;
	case WXK_END:		m_yShift	+= 10.0;				break;

	case WXK_PRIOR:		m_zShift	-= 10.0;				break;
	case WXK_NEXT:		m_zShift	+= 10.0;				break;

	case 'A':			m_bStereo	= !m_bStereo;			break;

	case WXK_F1:		m_Settings("EXAGGERATION")->Set_Value(m_Settings("EXAGGERATION")->asDouble() + 0.5);	break;
	case WXK_F2:		m_Settings("EXAGGERATION")->Set_Value(m_Settings("EXAGGERATION")->asDouble() - 0.5);	break;

	case WXK_F5:		m_Settings("SIZE_DEF")    ->Set_Value(m_Settings("SIZE_DEF")    ->asDouble() - 1.0);	break;
	case WXK_F6:		m_Settings("SIZE_DEF")    ->Set_Value(m_Settings("SIZE_DEF")    ->asDouble() + 1.0);	break;

	case WXK_F7:		m_Settings("SIZE_SCALE")  ->Set_Value(m_Settings("SIZE_SCALE")  ->asDouble() - 10.0);	break;
	case WXK_F8:		m_Settings("SIZE_SCALE")  ->Set_Value(m_Settings("SIZE_SCALE")  ->asDouble() + 10.0);	break;
	}

	Update_View();

	((C3DShapes_View_Dialog *)GetParent())->Update_Rotation();
}

//---------------------------------------------------------
#define GET_MOUSE_X_RELDIFF	((double)(m_Mouse_Down.x - event.GetX()) / (double)GetClientSize().x)
#define GET_MOUSE_Y_RELDIFF	((double)(m_Mouse_Down.y - event.GetY()) / (double)GetClientSize().y)

//---------------------------------------------------------
void C3DShapes_View_Control::On_Mouse_LDown(wxMouseEvent &event)
{
	SetFocus();

	m_Mouse_Down	= event.GetPosition();
	m_xDown			= m_zRotate;
	m_yDown			= m_xRotate;

	CaptureMouse();
}

void C3DShapes_View_Control::On_Mouse_LUp(wxMouseEvent &event)
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

		((C3DShapes_View_Dialog *)GetParent())->Update_Rotation();
	}
}

//---------------------------------------------------------
void C3DShapes_View_Control::On_Mouse_RDown(wxMouseEvent &event)
{
	SetFocus();

	m_Mouse_Down	= event.GetPosition();
	m_xDown			= m_xShift;
	m_yDown			= m_yShift;

	CaptureMouse();
}

void C3DShapes_View_Control::On_Mouse_RUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Mouse_Down.x != event.GetX() || m_Mouse_Down.y != event.GetY() )
	{
		m_xShift	= m_xDown - GET_MOUSE_X_RELDIFF * 1000.0;
		m_yShift	= m_yDown - GET_MOUSE_Y_RELDIFF * 1000.0;

		Update_View();

		((C3DShapes_View_Dialog *)GetParent())->Update_Rotation();
	}
}

//---------------------------------------------------------
void C3DShapes_View_Control::On_Mouse_MDown(wxMouseEvent &event)
{
	SetFocus();

	m_Mouse_Down	= event.GetPosition();
	m_xDown			= m_yRotate;
	m_yDown			= m_zShift;

	CaptureMouse();
}

void C3DShapes_View_Control::On_Mouse_MUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Mouse_Down.x != event.GetX() || m_Mouse_Down.y != event.GetY() )
	{
		m_yRotate	= m_xDown + GET_MOUSE_X_RELDIFF * M_PI_180;
		m_zShift	= m_yDown + GET_MOUSE_Y_RELDIFF * 1000.0;

		Update_View();

		((C3DShapes_View_Dialog *)GetParent())->Update_Rotation();
	}
}

//---------------------------------------------------------
void C3DShapes_View_Control::On_Mouse_Motion(wxMouseEvent &event)
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
			m_xShift	= m_xDown - GET_MOUSE_X_RELDIFF * 1000.0;
			m_yShift	= m_yDown - GET_MOUSE_Y_RELDIFF * 1000.0;
		}
		else if( event.MiddleIsDown() )
		{
			m_yRotate	= m_xDown + GET_MOUSE_X_RELDIFF * M_PI_180;
			m_zShift	= m_yDown + GET_MOUSE_Y_RELDIFF * 1000.0;
		}
		else
		{
			return;
		}

		Update_View();

		((C3DShapes_View_Dialog *)GetParent())->Update_Rotation();
	}
}

//---------------------------------------------------------
void C3DShapes_View_Control::On_Mouse_Wheel(wxMouseEvent &event)
{
	if( event.GetWheelRotation() )
	{
		m_zShift	+= 0.5 * event.GetWheelRotation();

		Update_View();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3DShapes_View_Control::_Set_Size(void)
{
	Update_View();
}

//---------------------------------------------------------
bool C3DShapes_View_Control::_Draw_Image(void)
{
	wxSize	dcSize	= GetClientSize();

	if( m_pShapes->Get_Count() <= 0
	||	dcSize.x <= 0 || dcSize.y <= 0
	||	m_Extent.Get_XRange() <= 0.0 || m_Extent.Get_YRange() <= 0.0
	||	m_cField < 0 || m_cField >= m_pShapes->Get_Field_Count()
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
			m_cStats.Get_Mean() - 1.5 * m_pShapes->Get_StdDev(m_cField),
			m_cStats.Get_Mean() + 1.5 * m_pShapes->Get_StdDev(m_cField)
		);
	}

	m_pColors	= m_Settings("COLORS")->asColors();
	m_cMin		= m_Settings("C_RANGE")->asRange()->Get_LoVal();
	m_cScale	= m_pColors->Get_Count() / (m_Settings("C_RANGE")->asRange()->Get_HiVal() - m_cMin);
	m_cWire		= m_Settings("COLOR_WIRE")->asColor();

	//-------------------------------------------------
	r_Scale		= (m_Image.GetWidth() / (double)m_Image.GetHeight()) > (m_Extent.Get_XRange() / m_Extent.Get_YRange())
				? m_Image.GetWidth () / m_Extent.Get_XRange()
				: m_Image.GetHeight() / m_Extent.Get_YRange();

	r_sin_x		= sin(m_xRotate - M_PI_180);
	r_cos_x		= cos(m_xRotate - M_PI_180);
	r_sin_y		= sin(m_yRotate);
	r_cos_y		= cos(m_yRotate);
	r_sin_z		= sin(m_zRotate);
	r_cos_z		= cos(m_zRotate);

	r_xc		= m_Extent.Get_XCenter();
	r_yc		= m_Extent.Get_YCenter();
	r_zc		= m_zStats.Get_Minimum() + 0.5 * m_zStats.Get_Range();

	r_Scale_z	= r_Scale * m_Settings("EXAGGERATION")->asDouble();

	//-------------------------------------------------
	m_Size_Def		= m_Settings("SIZE_DEF")->asInt();
	m_Size_Scale	= 1.0 / m_Settings("SIZE_SCALE")->asDouble();

	_Draw_Background();

	//-------------------------------------------------
	if( m_bStereo == false )
	{
		m_Image_zMax.Assign(999999.0);

		m_Color_Mode	= COLOR_MODE_RGB;

		for(int i=0; i<m_pShapes->Get_Count(); i++)
		{
			_Draw_Shape(m_pShapes->Get_Shape(i));
		}

		_Draw_Frame();
	}

	//-------------------------------------------------
	else
	{
		int		i;
		double	d	= m_Settings("STEREO_DIST")->asDouble() / 2.0;

		m_Image_zMax.Assign(999999.0);

		r_sin_y	= sin(m_yRotate - d * M_DEG_TO_RAD);
		r_cos_y	= cos(m_yRotate - d * M_DEG_TO_RAD);

		m_Color_Mode	= COLOR_MODE_RED;

		for(i=0; i<m_pShapes->Get_Count(); i++)
		{
			_Draw_Shape(m_pShapes->Get_Shape(i));
		}

		_Draw_Frame();

		m_Image_zMax.Assign(999999.0);

		r_sin_y	= sin(m_yRotate + d * M_DEG_TO_RAD);
		r_cos_y	= cos(m_yRotate + d * M_DEG_TO_RAD);

		m_Color_Mode	= COLOR_MODE_BLUE;

		for(i=0; i<m_pShapes->Get_Count(); i++)
		{
			_Draw_Shape(m_pShapes->Get_Shape(i));
		}

		_Draw_Frame();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3DShapes_View_Control::_Draw_Shape(CSG_Shape *pShape)
{
	int			c;
	TSG_Point	p;
	TNode		a, b;

	c	= _Get_Color(pShape->asDouble(m_cField));

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		switch( pShape->Get_Type() )
		{
		case SHAPE_TYPE_Point:
			{
				p	= pShape->Get_Point(0);
				a.x	= p.x;
				a.y	= p.y;
				a.z	= pShape->Get_Z(0);

				_Get_Projection(a);

				_Draw_Point(a.x, a.y, a.z, c, 1);
			}
			break;

		case SHAPE_TYPE_Line:
			{
				p	= pShape->Get_Point(0);
				a.x	= p.x;
				a.y	= p.y;
				a.z	= pShape->Get_Z(0);

				_Get_Projection(a);

				for(int iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					b	= a;
					p	= pShape->Get_Point(iPoint, iPart);
					a.x	= p.x;
					a.y	= p.y;
					a.z	= pShape->Get_Z(iPoint, iPart);

					_Get_Projection(a);

					_Draw_Line(a, b, c);
				}
			}
			break;

		case SHAPE_TYPE_Polygon:
			{
				p	= pShape->Get_Point(pShape->Get_Point_Count(iPart) - 1, iPart);
				a.x	= p.x;
				a.y	= p.y;
				a.z	= pShape->Get_Z(pShape->Get_Point_Count(iPart) - 1);

				_Get_Projection(a);

				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					b	= a;
					p	= pShape->Get_Point(iPoint, iPart);
					a.x	= p.x;
					a.y	= p.y;
					a.z	= pShape->Get_Z(iPoint, iPart);

					_Get_Projection(a);

					_Draw_Line(a, b, c);
				}
			}
			break;
		}
	}
}

//---------------------------------------------------------
inline void C3DShapes_View_Control::_Draw_Point(int x, int y, double z, int color, int size)
{
	if( z > 0.0 )
	{
		_Draw_Pixel(x, y, z, color);

		if( size > 0 && size < 50 )
		{
			for(int iy=1; iy<=size; iy++)
			{
				for(int ix=0; ix<=size; ix++)
				{
					if( ix*ix + iy*iy <= size*size )
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
inline void C3DShapes_View_Control::_Draw_Line(TNode a, TNode b, int Color)
{
	if(	(a.x < 0 && b.x < 0) || (a.x >= m_Image.GetWidth () && b.x >= m_Image.GetWidth ())
	||	(a.y < 0 && b.y < 0) || (a.y >= m_Image.GetHeight() && b.y >= m_Image.GetHeight()) )
	{
		return;
	}

	double	i, n, dx, dy, dz;

	dx		= b.x - a.x;
	dy		= b.y - a.y;
	dz		= b.z - a.z;

	if( b.z < 0.0 || a.z < 0.0 )
		return;

	if( fabs(dx) > fabs(dy) && fabs(dx) > 0.0 )
	{
		n	 = fabs(dx);
		dx	 = dx < 0 ? -1 : 1;
		dy	/= n;
		dz	/= n;
	}
	else if( fabs(dy) > 0.0 )
	{
		n	 = fabs(dy);
		dx	/= n;
		dy	 = dy < 0 ? -1 : 1;
		dz	/= n;
	}
	else
	{
		_Draw_Pixel((int)a.x, (int)a.y, a.z, Color);

		return;
	}

	//-----------------------------------------------------
	for(i=0; i<=n; i++, a.x+=dx, a.y+=dy, a.z+=dz)
	{
		_Draw_Pixel((int)a.x, (int)a.y, a.z, Color);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void C3DShapes_View_Control::_Draw_Triangle(TNode p[3], double dim)
{
	if( p[0].z < 0.0 || p[1].z < 0.0 || p[2].z < 0.0 )
		return;

	//-----------------------------------------------------
	if( p[1].y < p[0].y ) {	TNode pp = p[1]; p[1] = p[0]; p[0] = pp;	}
	if( p[2].y < p[0].y ) {	TNode pp = p[2]; p[2] = p[0]; p[0] = pp;	}
	if( p[2].y < p[1].y ) {	TNode pp = p[2]; p[2] = p[1]; p[1] = pp;	}

	//-----------------------------------------------------
	TSG_Rect	r;

	r.yMin	= p[0].y;
	r.yMax	= p[2].y;
	r.xMin	= p[0].x < p[1].x ? (p[0].x < p[2].x ? p[0].x : p[2].x) : (p[1].x < p[2].x ? p[1].x : p[2].x);
	r.xMax	= p[0].x > p[1].x ? (p[0].x > p[2].x ? p[0].x : p[2].x) : (p[1].x > p[2].x ? p[1].x : p[2].x);

	if( r.yMin >= r.yMax || r.xMin >= r.xMax )
	{
		return;	// no area
	}

	if( (r.yMin < 0.0 && r.yMax < 0.0) || (r.yMin >= m_Image.GetHeight() && r.yMax >= m_Image.GetHeight())
	||	(r.xMin < 0.0 && r.xMax < 0.0) || (r.xMin >= m_Image.GetWidth () && r.xMax >= m_Image.GetWidth ()) )
	{
		return;	// completely outside grid
	}

	//-----------------------------------------------------
	TNode	d[3];

	if( (d[0].y	= p[2].y - p[0].y) > 0.0 )
	{
		d[0].x	= (p[2].x - p[0].x) / d[0].y;
		d[0].z	= (p[2].z - p[0].z) / d[0].y;
		d[0].c	= (p[2].c - p[0].c) / d[0].y;
	}

	if( (d[1].y	= p[1].y - p[0].y) > 0.0 )
	{
		d[1].x	= (p[1].x - p[0].x) / d[1].y;
		d[1].z	= (p[1].z - p[0].z) / d[1].y;
		d[1].c	= (p[1].c - p[0].c) / d[1].y;
	}

	if( (d[2].y	= p[2].y - p[1].y) > 0.0 )
	{
		d[2].x	= (p[2].x - p[1].x) / d[2].y;
		d[2].z	= (p[2].z - p[1].z) / d[2].y;
		d[2].c	= (p[2].c - p[1].c) / d[2].y;
	}

	//-----------------------------------------------------
	int	ay	= (int)r.yMin;	if( ay < 0 )	ay	= 0;	if( ay < r.yMin )	ay++;
	int	by	= (int)r.yMax;	if( by >= m_Image.GetHeight() )	by	= m_Image.GetHeight() - 1;

	for(int y=ay; y<=by; y++)
	{
		if( y <= p[1].y && d[1].y > 0.0 )
		{
			_Draw_Triangle_Line(y,
				p[0].x + (y - p[0].y) * d[0].x,
				p[0].x + (y - p[0].y) * d[1].x,
				p[0].z + (y - p[0].y) * d[0].z,
				p[0].z + (y - p[0].y) * d[1].z,
				p[0].c + (y - p[0].y) * d[0].c,
				p[0].c + (y - p[0].y) * d[1].c,
				dim
			);
		}
		else if( d[2].y > 0.0 )
		{
			_Draw_Triangle_Line(y,
				p[0].x + (y - p[0].y) * d[0].x,
				p[1].x + (y - p[1].y) * d[2].x,
				p[0].z + (y - p[0].y) * d[0].z,
				p[1].z + (y - p[1].y) * d[2].z,
				p[0].c + (y - p[0].y) * d[0].c,
				p[1].c + (y - p[1].y) * d[2].c,
				dim
			);
		}
	}
}

//---------------------------------------------------------
inline void C3DShapes_View_Control::_Draw_Triangle_Line(int y, double xa, double xb, double za, double zb, double ca, double cb, double dim)
{
	if( xb < xa )
	{
		double	d;

		d	= xa;	xa	= xb;	xb	= d;
		d	= za;	za	= zb;	zb	= d;
		d	= ca;	ca	= cb;	cb	= d;
	}

	if( xb > xa )
	{
		double	dz	= (zb - za) / (xb - xa);
		double	dc	= (cb - ca) / (xb - xa);
		int		ax	= (int)xa;	if( ax < 0 )	ax	= 0;	if( ax < xa )	ax++;
		int		bx	= (int)xb;	if( bx >= m_Image.GetWidth() )	bx	= m_Image.GetWidth() - 1;

		for(int x=ax; x<=bx; x++)
		{
			double	z	= za + dz * (x - xa);
			double	c	= ca + dc * (x - xa);

			_Draw_Pixel(x, y, z, _Get_Color(c, dim));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3DShapes_View_Control::_Draw_Background(void)
{
	BYTE	r, g, b, *pRGB;
	int		i, n, color;

	color	= m_Settings("BGCOLOR")->asColor();

	if( m_bStereo )
	{
		color	= (int)((SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3.0);
		color	= SG_GET_RGB(color, color, color);
	}

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
void C3DShapes_View_Control::_Draw_Frame(void)
{
	if( !m_bFrame )
	{
		return;
	}

	int			color;
	double		buffer	= 1.0;
	CSG_Rect	r(m_Extent);
	TNode		p[2][4];

	r.Inflate(buffer);

	color	= m_Settings("BGCOLOR")->asColor();
	color	= SG_GET_RGB(SG_GET_R(color) + 128, SG_GET_G(color) + 128, SG_GET_B(color) + 128);

	for(int i=0; i<2; i++)
	{
		p[i][0].x	= r.Get_XMin();	p[i][0].y	= r.Get_YMin();
		p[i][1].x	= r.Get_XMax();	p[i][1].y	= r.Get_YMin();
		p[i][2].x	= r.Get_XMax();	p[i][2].y	= r.Get_YMax();
		p[i][3].x	= r.Get_XMin();	p[i][3].y	= r.Get_YMax();

		p[i][0].z	= p[i][1].z = p[i][2].z = p[i][3].z = i == 0
			? m_zStats.Get_Minimum() - buffer * m_zStats.Get_Range() / 100.0
			: m_zStats.Get_Maximum() + buffer * m_zStats.Get_Range() / 100.0;

		for(int j=0; j<4; j++)
		{
			_Get_Projection(p[i][j]);
		}

		_Draw_Line(p[i][0], p[i][1], color);
		_Draw_Line(p[i][1], p[i][2], color);
		_Draw_Line(p[i][2], p[i][3], color);
		_Draw_Line(p[i][3], p[i][0], color);
	}

	_Draw_Line(p[0][0], p[1][0], color);
	_Draw_Line(p[0][1], p[1][1], color);
	_Draw_Line(p[0][2], p[1][2], color);
	_Draw_Line(p[0][3], p[1][3], color);
}

//---------------------------------------------------------
inline void C3DShapes_View_Control::_Draw_Pixel(int x, int y, double z, int color)
{
	if( x >= 0 && x < m_Image.GetWidth() && y >= 0 && y < m_Image.GetHeight() && z < m_Image_zMax[y][x] )
	{
		BYTE	*RGB	= m_Image.GetData() + 3 * (y * m_Image.GetWidth() + x);

		switch( m_Color_Mode )
		{
		case COLOR_MODE_RGB:
			RGB[0]	= SG_GET_R(color);
			RGB[1]	= SG_GET_G(color);
			RGB[2]	= SG_GET_B(color);
			break;

		case COLOR_MODE_RED:
			RGB[0]	= (SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3;
			break;

		case COLOR_MODE_BLUE:
			RGB[1]	= 
			RGB[2]	= (SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3;
			break;
		}

		m_Image_zMax[y][x]	= z;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int C3DShapes_View_Control::_Get_Color(double value, double dim)
{
	int		Color	= (int)(m_cScale * (value - m_cMin));

	Color	= m_pColors->Get_Color(Color < 0 ? 0 : (Color >= m_pColors->Get_Count() ? m_pColors->Get_Count() - 1 : Color));

	if( dim >= 0.0 )
	{
		int	r	= (int)(dim * SG_GET_R(Color));	if( r < 0 )	r	= 0; else if( r > 255 )	r	= 255;
		int	g	= (int)(dim * SG_GET_G(Color));	if( g < 0 )	g	= 0; else if( g > 255 )	g	= 255;
		int	b	= (int)(dim * SG_GET_B(Color));	if( b < 0 )	b	= 0; else if( b > 255 )	b	= 255;

		Color	= SG_GET_RGB(r, g, b);
	}

	return( Color );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void C3DShapes_View_Control::_Get_Projection(TNode &p)
{
	TSG_Point_Z	q;

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

	p.x	= q.x + 0.5 * m_Image.GetWidth ();
	p.y	= q.y + 0.5 * m_Image.GetHeight();
	p.z	= q.z;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
