/**********************************************************
 * Version $Id: 3d_viewer_grids.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       3d_viewer                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  3d_viewer_grids.cpp                  //
//                                                       //
//                 Copyright (C) 2017 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dcclient.h>

#include "3d_viewer_grids.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	PLANE_SIDE_X	= 0,
	PLANE_SIDE_Y,
	PLANE_SIDE_Z
};

//---------------------------------------------------------
class C3D_Viewer_Grids_Panel : public CSG_3DView_Panel
{
public:
	C3D_Viewer_Grids_Panel(wxWindow *pParent, CSG_Grids *pGrids);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual void				Update_Statistics		(void);
	virtual void				Update_Parent			(void);

	virtual void				On_Key_Down				(wxKeyEvent   &event);

	virtual bool				On_Before_Draw			(void);
	virtual bool				On_Draw					(void);

	virtual int					Get_Color				(double Value);


private:

	bool						m_Color_bGrad;

	double						m_Color_Min, m_Color_Scale, m_Position[3];

	CSG_Colors					m_Colors;

	CSG_Grid					m_Plane[3];

	CSG_Grids					*m_pGrids;
	

	bool						Set_ZScale				(bool bIncrease);
	bool						Set_ZLevel				(bool bIncrease);
	bool						Set_Resolution			(bool bIncrease, bool bVertical);

	bool						Set_Planes				(void);
	bool						Set_Plane				(                 double Position, int Side);
	bool						Set_Plane				(CSG_Grid &Plane, double Position, int Side);

	bool						Get_Node				(CSG_Grid &Plane, double Position, int Side, int x, int y, TSG_Triangle_Node &Node);

	void						Draw_Plane				(CSG_Grid &Plane, double Position, int Side);


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()

	friend class C3D_Viewer_Grids_Dialog;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_Grids_Panel, CSG_3DView_Panel)
	EVT_KEY_DOWN	(C3D_Viewer_Grids_Panel::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Grids_Panel::C3D_Viewer_Grids_Panel(wxWindow *pParent, CSG_Grids *pGrids)
	: CSG_3DView_Panel(pParent)
{
	m_pGrids	= pGrids;

	//-----------------------------------------------------
	m_Parameters.Add_Double("NODE_GENERAL",
		"Z_SCALE"		, _TL("Exaggeration"),
		_TL(""),
		1.0
	);

	//-----------------------------------------------------
	m_Parameters.Add_Double("NODE_GENERAL",
		"RESOLUTION_XY"	, _TL("Horizontal Resolution"),
		_TL("Horizontal resolution (x/y) in map units. Use [F7]/[F8] keys to de-/increase."),
		m_pGrids->Get_Extent().Get_Diameter() / 200,
		m_pGrids->Get_Cellsize(), true,
		m_pGrids->Get_Extent().Get_Diameter() / 10, true
	);

	m_Parameters.Add_Choice("RESOLUTION_XY",
		"RESAMPLING_XY"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|",
			_TL("Nearest Neigbhour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation"),
			_TL("Mean Value"),
			_TL("Mean Value (cell area weighted)")
		), 0
	);

	m_Parameters.Add_Int("NODE_GENERAL",
		"RESOLUTION_Z"	, _TL("Vertical Resolution"),
		_TL("Vertical resolution (z) in number of levels. Use [F9]/[F10] keys to de-/increase."),
		M_GET_MIN(m_pGrids->Get_NZ() * 4, 200), 10, true
	);

	m_Parameters.Add_Choice("RESOLUTION_Z",
		"RESAMPLING_Z"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Nearest Neigbhour"),
			_TL("Linear Interpolation"),
			_TL("Spline Interpolation")
		), 1
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("",
		"NODE_VIEW"		, _TL("Grid View Settings"),
		_TL("")
	);

	if( !SG_UI_DataObject_Colors_Get(m_pGrids, &m_Colors) )
	{
		m_Colors.Set_Palette(SG_COLORS_RAINBOW);
	}

	m_Parameters.Add_Colors("NODE_VIEW",
		"COLORS"		, _TL("Colours"),
		_TL(""),
		&m_Colors
	);

	m_Parameters.Add_Range("COLORS",
		"COLOR_STRETCH"	, _TL("Histogram Stretch"),
		_TL(""),
		m_pGrids->Get_Mean() - 2. * m_pGrids->Get_StdDev(),
		m_pGrids->Get_Mean() + 2. * m_pGrids->Get_StdDev()
	);

	m_Parameters.Add_Bool("COLORS",
		"COLORS_GRAD"	, _TL("Graduated"),
		_TL(""),
		true
	);

	m_Parameters.Add_Choice("NODE_VIEW",
		"SHADING"		, _TL("Shading"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("none"),
			_TL("shading")
		), 1
	);

	m_Parameters.Add_Double("SHADING",
		"SHADE_DEC"		, _TL("Light Source Height"),
		_TL(""),
		45.0, -90.0, true, 90.0, true
	);

	m_Parameters.Add_Double("SHADING",
		"SHADE_AZI"		, _TL("Light Source Direction"),
		_TL(""),
		315.0, 0.0, true, 360.0, true
	);

	//-----------------------------------------------------
	m_Parameters("Z_SCALE")->Set_Value(0.2 * (m_pGrids->Get_XRange() + m_pGrids->Get_YRange()) / m_pGrids->Get_ZRange());

	m_Position[PLANE_SIDE_X]	= 0.5;
	m_Position[PLANE_SIDE_Y]	= 0.5;
	m_Position[PLANE_SIDE_Z]	= 0.5;

	m_BoxBuffer	= 0.0;

	m_Projector.Set_zShift(2000);
	m_Projector.Set_yShift(-100);
	m_Projector.Set_xRotation(M_DEG_TO_RAD * 60);
	m_Projector.Set_zRotation(M_DEG_TO_RAD * 45);

	Update_Statistics();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Grids_Panel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SHADING") )
	{
		pParameters->Set_Enabled("SHADE_DEC", pParameter->asBool());
		pParameters->Set_Enabled("SHADE_AZI", pParameter->asBool());
	}

	return( CSG_3DView_Panel::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_Grids_Panel::Update_Statistics(void)
{
	m_Data_Min.x	= m_pGrids->Get_XMin();
	m_Data_Max.x	= m_pGrids->Get_XMax();

	m_Data_Min.y	= m_pGrids->Get_YMin();
	m_Data_Max.y	= m_pGrids->Get_YMax();

	m_Data_Min.z	= m_pGrids->Get_ZMin();
	m_Data_Max.z	= m_pGrids->Get_ZMax();

	Set_Planes();	// Update_View();
}

//---------------------------------------------------------
void C3D_Viewer_Grids_Panel::Update_Parent(void)
{
	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Grids_Panel::Set_ZScale(bool bIncrease)
{
	double	d	= 0.1 * 0.25 * (m_pGrids->Get_XRange() + m_pGrids->Get_YRange()) / m_pGrids->Get_ZRange();
	m_Parameters("Z_SCALE")->Set_Value(m_Parameters("Z_SCALE")->asDouble() + (bIncrease ? d : -d));
	Update_View();

	return( true );
}

//---------------------------------------------------------
bool C3D_Viewer_Grids_Panel::Set_ZLevel(bool bIncrease)
{
	for(int i=1; i<m_pGrids->Get_NZ(); i++)
	{
		double	z	= m_pGrids->Get_Z(bIncrease ? i : m_pGrids->Get_NZ() - 1 - i);

		z	= (z - m_pGrids->Get_ZMin()) / m_pGrids->Get_ZRange();
		z	= (int)(z * 100.) / 100.;	// rounding

		if( (bIncrease ==  true && z > m_Position[PLANE_SIDE_Z])
		||  (bIncrease == false && z < m_Position[PLANE_SIDE_Z]) )
		{
			Set_Plane(z, PLANE_SIDE_Z);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool C3D_Viewer_Grids_Panel::Set_Resolution(bool bIncrease, bool bVertical)
{
	if( bVertical )
	{
		double	d	= m_Parameters("RESOLUTION_Z")->asDouble();

		m_Parameters("RESOLUTION_Z")->Set_Value(bIncrease
			? d + 5
			: d - 5
		);
	}
	else
	{
		double	d	= m_Parameters("RESOLUTION_XY")->asDouble();

		m_Parameters("RESOLUTION_XY")->Set_Value(bIncrease
			? d - m_pGrids->Get_Cellsize()
			: d + m_pGrids->Get_Cellsize()
		);
	}

	Set_Planes();

	return( true );
}

//---------------------------------------------------------
void C3D_Viewer_Grids_Panel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:
		CSG_3DView_Panel::On_Key_Down(event);
		return;

	//-----------------------------------------------------
	case WXK_F1 : Set_ZScale(false); break;
	case WXK_F2 : Set_ZScale( true); break;

	case WXK_F7 : Set_Resolution(false, false);	break;
	case WXK_F8 : Set_Resolution( true, false);	break;

	case WXK_F9 : Set_Resolution(false,  true);	break;
	case WXK_F10: Set_Resolution( true,  true);	break;

	case WXK_F11: Set_ZLevel(false);	break;
	case WXK_F12: Set_ZLevel( true);	break;
	}

	Update_Parent();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Grids_Panel::On_Before_Draw(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Projector.Set_zScaling(m_Projector.Get_xScaling() * m_Parameters("Z_SCALE")->asDouble());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Grids_Panel::Get_Color(double Value)
{
	if( m_Color_Scale <= 0.0 )
	{
		return( (int)Value );
	}

	double	c	= m_Color_Scale * (Value - m_Color_Min);

	return( m_Color_bGrad ? m_Colors.Get_Interpolated(c) : m_Colors[(int)c] );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Grids_Panel::On_Draw(void)
{
	//-----------------------------------------------------
	m_Colors		=*m_Parameters("COLORS")->asColors();
	m_Color_bGrad	= m_Parameters("COLORS_GRAD")->asBool();

	m_Color_Min		= m_Parameters("COLOR_STRETCH")->asRange()->Get_LoVal();
	double	Range	= m_Parameters("COLOR_STRETCH")->asRange()->Get_HiVal() - m_Color_Min;
	m_Color_Scale	= Range > 0.0 ? (m_Colors.Get_Count() - 1) / Range : 0.0;

	//-----------------------------------------------------
	Draw_Plane(m_Plane[PLANE_SIDE_X], m_Position[PLANE_SIDE_X], PLANE_SIDE_X);
	Draw_Plane(m_Plane[PLANE_SIDE_Y], m_Position[PLANE_SIDE_Y], PLANE_SIDE_Y);
	Draw_Plane(m_Plane[PLANE_SIDE_Z], m_Position[PLANE_SIDE_Z], PLANE_SIDE_Z);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Grids_Panel::Set_Planes(void)
{
	Set_Plane(m_Plane[PLANE_SIDE_X], m_Position[PLANE_SIDE_X], PLANE_SIDE_X);
	Set_Plane(m_Plane[PLANE_SIDE_Y], m_Position[PLANE_SIDE_Y], PLANE_SIDE_Y);
	Set_Plane(m_Plane[PLANE_SIDE_Z], m_Position[PLANE_SIDE_Z], PLANE_SIDE_Z);

	Update_View();

	return( true );
}

//---------------------------------------------------------
bool C3D_Viewer_Grids_Panel::Set_Plane(double Position, int Side)
{
	Set_Plane(m_Plane[Side], m_Position[Side] = Position, Side);

	Update_View();

	return( true );
}

//---------------------------------------------------------
bool C3D_Viewer_Grids_Panel::Set_Plane(CSG_Grid &Plane, double Position, int Side)
{
	double	Cellsize	= m_Parameters("RESOLUTION_XY")->asDouble();

	if( Cellsize < m_pGrids->Get_Cellsize() )
	{
		Cellsize = m_pGrids->Get_Cellsize();
	}

	int	zLevels	= m_Parameters("RESOLUTION_Z")->asInt();

	//-----------------------------------------------------
	TSG_Grid_Resampling	zResampling, Resampling;

	switch( m_Parameters("RESAMPLING_Z")->asInt() )
	{
	default: zResampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: zResampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: zResampling = GRID_RESAMPLING_BSpline         ; break;
	}

	switch( m_Parameters("RESAMPLING_XY")->asInt() )
	{
	default: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling = GRID_RESAMPLING_BSpline         ; break;
	case  4: Resampling = GRID_RESAMPLING_Mean_Nodes      ; break;
	case  5: Resampling = GRID_RESAMPLING_Mean_Cells      ; break;
	}

	switch( Side )
	{
	//-----------------------------------------------------
	case PLANE_SIDE_X:
		{
			if( Plane.Get_Cellsize() != Cellsize || Plane.Get_NX() != zLevels )
			{
				Plane.Create(CSG_Grid_System(Cellsize, 0.0, m_pGrids->Get_YMin(), Cellsize * zLevels, m_pGrids->Get_YMax()));
			}

			double	dz	= m_pGrids->Get_ZRange() / Plane.Get_NX();

			#ifndef _DEBUG
			#pragma omp parallel for
			#endif
			for(int y=0; y<Plane.Get_NY(); y++)
			{
				TSG_Point_Z	p;

				p.z	= m_pGrids->Get_ZMin();
				p.y	= Plane.Get_YMin() + Plane.Get_Cellsize() * y;
				p.x	= m_pGrids->Get_XMin() + Position * m_pGrids->Get_XRange();

				for(int x=0; x<Plane.Get_NX(); x++, p.z+=dz)
				{
					double	Value;

					if( m_pGrids->Get_Value(p, Value, Resampling, zResampling) )
					{
						Plane.Set_Value(x, y, Value);
					}
					else
					{
						Plane.Set_NoData(x, y);
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case PLANE_SIDE_Y:
		{
			if( Plane.Get_Cellsize() != Cellsize || Plane.Get_NY() != zLevels )
			{
				Plane.Create(CSG_Grid_System(Cellsize, m_pGrids->Get_XMin(), 0.0, m_pGrids->Get_XMax(), Cellsize * zLevels));
			}

			double	dz	= m_pGrids->Get_ZRange() / Plane.Get_NY();

			#ifndef _DEBUG
			#pragma omp parallel for
			#endif
			for(int x=0; x<Plane.Get_NX(); x++)
			{
				TSG_Point_Z	p;

				p.z	= m_pGrids->Get_ZMin();
				p.y	= m_pGrids->Get_YMin() + Position * m_pGrids->Get_YRange();
				p.x	= Plane.Get_XMin() + Plane.Get_Cellsize() * x;

				for(int y=0; y<Plane.Get_NY(); y++, p.z+=dz)
				{
					double	Value;

					if( m_pGrids->Get_Value(p, Value, Resampling, zResampling) )
					{
						Plane.Set_Value(x, y, Value);
					}
					else
					{
						Plane.Set_NoData(x, y);
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case PLANE_SIDE_Z:
		{
			if( Plane.Get_Cellsize() != Cellsize )
			{
				Plane.Create(CSG_Grid_System(Cellsize, m_pGrids->Get_System().Get_Extent()));
			}

			#ifndef _DEBUG
			#pragma omp parallel for
			#endif
			for(int y=0; y<Plane.Get_NY(); y++)
			{
				TSG_Point_Z	p;

				p.z	= m_pGrids->Get_ZMin() + Position * m_pGrids->Get_ZRange();
				p.y	= Plane.Get_YMin() + Plane.Get_Cellsize() * y;
				p.x	= Plane.Get_XMin();

				for(int x=0; x<Plane.Get_NX(); x++, p.x+=Plane.Get_Cellsize())
				{
					double	Value;

					if( m_pGrids->Get_Value(p, Value, Resampling, zResampling) )
					{
						Plane.Set_Value(x, y, Value);
					}
					else
					{
						Plane.Set_NoData(x, y);
					}
				}
			}
		}
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool C3D_Viewer_Grids_Panel::Get_Node(CSG_Grid &Plane, double Position, int Side, int x, int y, TSG_Triangle_Node &Node)
{
	if( Plane.is_InGrid(x, y) )
	{
		TSG_Point_Z	p;

		switch( Side )
		{
		case PLANE_SIDE_X:
			p.x	= Position;
			p.y	= m_pGrids->Get_YMin() + y * m_pGrids->Get_YRange() / (Plane.Get_NY() - 1);
			p.z	= m_pGrids->Get_ZMin() + x * m_pGrids->Get_ZRange() / (Plane.Get_NX() - 1);
			break;

		case PLANE_SIDE_Y:
			p.x	= m_pGrids->Get_XMin() + x * m_pGrids->Get_XRange() / (Plane.Get_NX() - 1);
			p.y	= Position;
			p.z	= m_pGrids->Get_ZMin() + y * m_pGrids->Get_ZRange() / (Plane.Get_NY() - 1);
			break;

		case PLANE_SIDE_Z:
			p.x	= m_pGrids->Get_XMin() + x * m_pGrids->Get_XRange() / (Plane.Get_NX() - 1);
			p.y	= m_pGrids->Get_YMin() + y * m_pGrids->Get_YRange() / (Plane.Get_NY() - 1);
			p.z	= Position;
			break;
		}

		m_Projector.Get_Projection(p);

		Node.x	= p.x;
		Node.y	= p.y;
		Node.z	= p.z;
		Node.c	= Plane.asDouble(x, y);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void C3D_Viewer_Grids_Panel::Draw_Plane(CSG_Grid &Plane, double Position, int Side)
{
	switch( Side )
	{
	case PLANE_SIDE_X: Position = m_pGrids->Get_XMin() + Position * m_pGrids->Get_XRange(); break;
	case PLANE_SIDE_Y: Position = m_pGrids->Get_YMin() + Position * m_pGrids->Get_YRange(); break;
	case PLANE_SIDE_Z: Position = m_pGrids->Get_ZMin() + Position * m_pGrids->Get_ZRange(); break;
	}

	//-----------------------------------------------------
	int		Shading		= m_Parameters("SHADING")->asInt();
	double	Shade_Dec	= m_Parameters("SHADE_DEC")->asDouble() * -M_DEG_TO_RAD;
	double	Shade_Azi	= m_Parameters("SHADE_AZI")->asDouble() *  M_DEG_TO_RAD;

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=1; y<Plane.Get_NY(); y++)
	{
		for(int x=1; x<Plane.Get_NX(); x++)
		{
			TSG_Triangle_Node	p[3];

			if( Get_Node(Plane, Position, Side, x - 1, y - 1, p[0])
			&&  Get_Node(Plane, Position, Side, x    , y    , p[1]) )
			{
				if( Get_Node(Plane, Position, Side, x    , y - 1, p[2]) )
				{
					if( Shading ) Draw_Triangle(p, false, Shade_Dec, Shade_Azi); else Draw_Triangle(p, false);
				}

				if( Get_Node(Plane, Position, Side, x - 1, y    , p[2]) )
				{
					if( Shading ) Draw_Triangle(p, false, Shade_Dec, Shade_Azi); else Draw_Triangle(p, false);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_Grids_Histogram : public wxDialog
{
public:
	C3D_Viewer_Grids_Histogram(void)	{}

	void						Create			(wxWindow *pParent, CSG_Grids *pGrids, C3D_Viewer_Grids_Panel *pPanel)
	{
		m_pPanel		= pPanel;
		m_pGrids		= pGrids;
		m_nClasses		= 100;
		m_bCumulative	= false;

		wxDialog::Create(pParent, wxID_ANY, _TL("Histogram"), wxDefaultPosition, wxDefaultSize,
			wxCAPTION|wxCLOSE_BOX|wxSTAY_ON_TOP
		);

		Set_Histogram(false);
	}


private:

	bool					m_bCumulative;

	int						m_nClasses;

	wxPoint					m_Mouse_Down, m_Mouse_Move;

	CSG_Histogram			m_Histogram;

	CSG_Grids				*m_pGrids;

	C3D_Viewer_Grids_Panel	*m_pPanel;


	//---------------------------------------------------------
	void					On_Mouse_LDown	(wxMouseEvent &event)
	{
		m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();

		Draw_Inverse(m_Mouse_Down, m_Mouse_Move);

		CaptureMouse();
	}

	//---------------------------------------------------------
	void					On_Mouse_Motion	(wxMouseEvent &event)
	{
		if( HasCapture() && event.Dragging() && event.LeftIsDown() )
		{
			Draw_Inverse(m_Mouse_Down, m_Mouse_Move);
			Draw_Inverse(m_Mouse_Down, event.GetPosition());
		}

		m_Mouse_Move	= event.GetPosition();
	}

	//---------------------------------------------------------
	void					On_Mouse_LUp	(wxMouseEvent &event)
	{
		if( HasCapture() )
		{
			ReleaseMouse();
		}

		if( m_Mouse_Down.x == event.GetX() )
		{
			Refresh();

			return;
		}

		wxRect	r(wxPoint(0, 0), GetClientSize());

		double	Minimum	= m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Get_LoVal();
		double	Range	= m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Get_HiVal() - Minimum;

		m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Set_Range(
			Minimum + (Range * (m_Mouse_Down.x - r.GetLeft()) / (double)r.GetWidth()),
			Minimum + (Range * (event.GetX()   - r.GetLeft()) / (double)r.GetWidth())
		);

		Set_Histogram();
	}

	//---------------------------------------------------------
	void					On_Mouse_RDown	(wxMouseEvent &event)
	{
		m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Set_Range(m_pGrids->Get_Min(), m_pGrids->Get_Max());

		Set_Histogram();
	}

	//---------------------------------------------------------
	void					On_Key_Down		(wxKeyEvent   &event)
	{
		switch( event.GetKeyCode() )
		{
		case WXK_NUMPAD_ADD:
			m_nClasses	+= 10; Set_Histogram(false);
			break;

		case WXK_NUMPAD_SUBTRACT:
			if( m_nClasses > 10 )
			{
				m_nClasses	-= 10; Set_Histogram(false);
			}
			break;

		case WXK_PAGEUP:
			Set_Size(GetClientSize().GetWidth() * 1.25, GetClientSize().GetHeight() * 1.25, true);
			break;

		case WXK_PAGEDOWN:
			Set_Size(GetClientSize().GetWidth() / 1.25, GetClientSize().GetHeight() / 1.25, true);
			break;

		case WXK_SPACE:
			m_bCumulative	= !m_bCumulative;
			Refresh();
			break;
		}
	}

	//---------------------------------------------------------
	void					On_Close		(wxCloseEvent &event)
	{
		Hide();	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
	}

	//---------------------------------------------------------
	void					On_Paint		(wxPaintEvent &WXUNUSED(event))
	{
		wxPaintDC	dc(this);

		Draw(dc, GetClientSize());
	}

	//---------------------------------------------------------
	void					Draw_Inverse	(wxPoint A, wxPoint B)
	{
		wxRect	r(GetClientSize());
		wxClientDC	dc(this);
		dc.SetLogicalFunction(wxINVERT);
		dc.DrawRectangle(A.x, 0, B.x - A.x, r.GetHeight());
	}

	//---------------------------------------------------------
	void					Set_Size		(int Width, int Height, bool bRefresh)
	{
		if( Width < 100 || Height < 100 || Width > 1000 || Height > 1000 )
		{
			return;
		}

		SetClientSize(Width, Height);

		Refresh();
	}

	//---------------------------------------------------------
	void					Set_Histogram	(bool bRefresh = true)
	{
		double	Minimum	= m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Get_LoVal();
		double	Maximum	= m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Get_HiVal();

		m_Histogram.Create(m_nClasses, Minimum, Maximum, m_pGrids, m_pGrids->Get_Max_Samples());

		Refresh();

		if( bRefresh )
		{
			m_pPanel->Update_View();
		}
	}

	//---------------------------------------------------------
	void					Draw			(wxDC &dc, const wxRect &r)
	{
		if( m_Histogram.Get_Class_Count() > 0 && m_Histogram.Get_Element_Count() > 0 )
		{
			int		ax, ay, bx, by;
			double	dx, Value;

			CSG_Colors	Colors(*m_pPanel->m_Parameters("COLORS")->asColors());

			double	dColor	= m_Histogram.Get_Class_Count() > 1 ? ((Colors.Get_Count() - 1.) / (m_Histogram.Get_Class_Count() - 1.)) : 0.;

			dx	= (double)r.GetWidth() / (double)m_Histogram.Get_Class_Count();
			ay	= r.GetBottom() + 1;
			bx	= r.GetLeft();

			for(size_t iClass=0; iClass<m_Histogram.Get_Class_Count(); iClass++)
			{
				Value	= m_bCumulative
					? m_Histogram.Get_Cumulative(iClass) / (double)m_Histogram.Get_Element_Count  ()
					: m_Histogram.Get_Elements  (iClass) / (double)m_Histogram.Get_Element_Maximum();

				ax	= bx;
				bx	= r.GetLeft() + (int)(dx * (iClass + 1.0));
				by	= ay - (int)(r.GetHeight() * Value);

				wxColour	c((unsigned long)Colors.Get_Interpolated(dColor * iClass));

				dc.SetPen  (wxPen  (c));
				dc.SetBrush(wxBrush(c));

				dc.DrawRectangle(ax, ay, bx - ax, by - ay);
			//	Draw_FillRect(dc, Color, ax, ay, bx, by);
			}
		}
	}


	DECLARE_EVENT_TABLE()

};

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_Grids_Histogram, wxDialog)
	EVT_LEFT_DOWN	(C3D_Viewer_Grids_Histogram::On_Mouse_LDown)
	EVT_LEFT_UP		(C3D_Viewer_Grids_Histogram::On_Mouse_LUp)
	EVT_RIGHT_DOWN	(C3D_Viewer_Grids_Histogram::On_Mouse_RDown)
	EVT_MOTION		(C3D_Viewer_Grids_Histogram::On_Mouse_Motion)
	EVT_KEY_DOWN	(C3D_Viewer_Grids_Histogram::On_Key_Down)
	EVT_CLOSE		(C3D_Viewer_Grids_Histogram::On_Close)
	EVT_PAINT		(C3D_Viewer_Grids_Histogram::On_Paint)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_Grids_Dialog : public CSG_3DView_Dialog
{
public:
	C3D_Viewer_Grids_Dialog(CSG_Grids *pGrids)
		: CSG_3DView_Dialog(_TL("Grid Collection Viewer"), 0)
	{
		C3D_Viewer_Grids_Panel	*pPanel	= new C3D_Viewer_Grids_Panel(this, pGrids);

		Create(pPanel);

		//-------------------------------------------------
		Add_Spacer();

		wxArrayString	Choices;

		Choices.Add(_TL("Nearest Neigbhour"   ));
		Choices.Add(_TL("Linear Interpolation"));
		Choices.Add(_TL("Spline Interpolation"));

		m_pResampling	= Add_Choice(_TL("Resampling"), Choices, 1); // Linear

		//-------------------------------------------------
		Add_Spacer();

		m_pSlide[PLANE_SIDE_X]	= Add_Slider(_TL("X"), pPanel->m_Position[PLANE_SIDE_X], 0, 1);
		m_pSlide[PLANE_SIDE_Y]	= Add_Slider(_TL("Y"), pPanel->m_Position[PLANE_SIDE_Y], 0, 1);
		m_pSlide[PLANE_SIDE_Z]	= Add_Slider(_TL("Z"), pPanel->m_Position[PLANE_SIDE_Z], 0, 1);

		//-------------------------------------------------
		Add_Spacer();

		m_pHistogram	= Add_CheckBox(_TL("Histogram"), false);

		m_Histogram.Create(this, pGrids, pPanel);
	}


protected:

	CSGDI_Slider				*m_pSlide[3];
	
	wxCheckBox					*m_pHistogram;

	wxChoice					*m_pResampling;

	C3D_Viewer_Grids_Histogram	m_Histogram;


	//-----------------------------------------------------
	enum
	{
		MENU_SCALE_Z_DEC	= MENU_USER_FIRST,
		MENU_SCALE_Z_INC,
		MENU_LEVEL_Z_DEC,
		MENU_LEVEL_Z_INC,
		MENU_RESLT_Z_DEC,
		MENU_RESLT_Z_INC,
		MENU_RESLT_XY_DEC,
		MENU_RESLT_XY_INC
	};

	//-----------------------------------------------------
	virtual void				Set_Menu			(wxMenu &Menu)
	{
		wxMenu	*pMenu	= Menu.FindChildItem(Menu.FindItem(_TL("Display")))->GetSubMenu();

		pMenu->AppendSeparator();
		pMenu->Append(MENU_SCALE_Z_DEC , _TL("Decrease Exaggeration [F1]"));
		pMenu->Append(MENU_SCALE_Z_INC , _TL("Increase Exaggeration [F2]"));

		pMenu->AppendSeparator();
		pMenu->Append(MENU_LEVEL_Z_DEC , _TL("Previous Level [F11]"));
		pMenu->Append(MENU_LEVEL_Z_INC , _TL("Next Level [F12]"));

		pMenu->AppendSeparator();
		pMenu->Append(MENU_RESLT_XY_DEC, _TL("Decrease Horizontal Resolution [F7]"));
		pMenu->Append(MENU_RESLT_XY_INC, _TL("Increase Horizontal Resolution [F8]"));

		pMenu->Append(MENU_RESLT_Z_DEC , _TL("Decrease Vertical Resolution [F9]"));
		pMenu->Append(MENU_RESLT_Z_INC , _TL("Increase Vertical Resolution [F10]"));
	}

	//-----------------------------------------------------
	virtual void				On_Menu				(wxCommandEvent &event)
	{
		C3D_Viewer_Grids_Panel	*pPanel	= (C3D_Viewer_Grids_Panel *)m_pPanel;

		switch( event.GetId() )
		{
		default:	CSG_3DView_Dialog::On_Menu(event);
			return;

		case MENU_SCALE_Z_DEC :	pPanel->Set_ZScale(false);	break;
		case MENU_SCALE_Z_INC :	pPanel->Set_ZScale( true);	break;

		case MENU_LEVEL_Z_DEC :	pPanel->Set_ZLevel(false);	break;
		case MENU_LEVEL_Z_INC :	pPanel->Set_ZLevel( true);	break;

		case MENU_RESLT_XY_DEC:	pPanel->Set_Resolution(false, false);	break;
		case MENU_RESLT_XY_INC:	pPanel->Set_Resolution( true, false);	break;

		case MENU_RESLT_Z_DEC :	pPanel->Set_Resolution(false,  true);	break;
		case MENU_RESLT_Z_INC :	pPanel->Set_Resolution( true,  true);	break;
		}
	}

	//-----------------------------------------------------
	void						On_Update_Choices(wxCommandEvent &event)
	{
		if( event.GetEventObject() == m_pResampling )
		{
			m_pPanel->m_Parameters("RESAMPLING_Z")->Set_Value(m_pResampling->GetSelection());

			((C3D_Viewer_Grids_Panel *)m_pPanel)->Set_Planes();
		}

		CSG_3DView_Dialog::On_Update_Choices(event);
	}

	//-----------------------------------------------------
	virtual void				On_Update_Control	(wxCommandEvent &event)
	{
		C3D_Viewer_Grids_Panel	*pPanel	= (C3D_Viewer_Grids_Panel *)m_pPanel;

		if( event.GetEventObject() == m_pSlide[PLANE_SIDE_X] )
		{
			pPanel->Set_Plane(m_pSlide[PLANE_SIDE_X]->Get_Value(), PLANE_SIDE_X);
		}

		if( event.GetEventObject() == m_pSlide[PLANE_SIDE_Y] )
		{
			pPanel->Set_Plane(m_pSlide[PLANE_SIDE_Y]->Get_Value(), PLANE_SIDE_Y);
		}

		if( event.GetEventObject() == m_pSlide[PLANE_SIDE_Z] )
		{
			pPanel->Set_Plane(m_pSlide[PLANE_SIDE_Z]->Get_Value(), PLANE_SIDE_Z);
		}

		if( event.GetEventObject() == m_pHistogram )
		{
#ifdef _SAGA_MSW
			m_Histogram.Show(m_pHistogram->GetValue() == 1 ? true : false);	// unluckily this does not work with linux (broken event handler chain, non-modal dialog as subprocess of a modal one!!)
#else
			m_Histogram.ShowModal();
#endif
		}

		CSG_3DView_Dialog::On_Update_Control(event);
	}

	//-----------------------------------------------------
	virtual void				Update_Controls			(void)
	{
		C3D_Viewer_Grids_Panel	*pPanel	= (C3D_Viewer_Grids_Panel *)m_pPanel;

		m_pSlide[PLANE_SIDE_X]->Set_Value(pPanel->m_Position[PLANE_SIDE_X]);
		m_pSlide[PLANE_SIDE_Y]->Set_Value(pPanel->m_Position[PLANE_SIDE_Y]);
		m_pSlide[PLANE_SIDE_Z]->Set_Value(pPanel->m_Position[PLANE_SIDE_Z]);

		m_pHistogram->SetValue(m_Histogram.IsShown());

		CSG_3DView_Dialog::Update_Controls();
	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Grids::C3D_Viewer_Grids(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Grid Collection Viewer"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"The 'Grid Collection Viewer' visualizes the data of a grid collection "
		"in threedimensional space using the map coordinates for x- and y-axis "
		"and the chosen z attribute for the z-axis. Data are displayed on three "
		"planes - one for each axis - which can be moved arbitrarily along its "
		"associated axis. The values displayed on each plane as colour are interpolated "
		"on basis of the grid collection (interpreted as 3-dimensional grid). "
		"The viewer can be used to visualize volumes, multi- or hyperspectral data, "
		"or grids representing a time series, if these are managed in a grid collection. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grids(
		""	, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);
}

//---------------------------------------------------------
bool C3D_Viewer_Grids::On_Execute(void)
{
	CSG_Grids	*pGrids	= Parameters("GRIDS")->asGrids();

	if( pGrids->Get_Grid_Count() <= 0 )
	{
		Message_Add(_TL("invalid input"));

		return( false );
	}

	C3D_Viewer_Grids_Dialog	dlg(pGrids);

	dlg.ShowModal();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
