
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
	PLANE_SIDE_X = 0,
	PLANE_SIDE_Y,
	PLANE_SIDE_Z
};

//---------------------------------------------------------
class C3D_Viewer_Grids_Panel : public CSG_3DView_Panel
{
public:
	C3D_Viewer_Grids_Panel(wxWindow *pParent, CSG_Grids *pGrids);

	static CSG_String			Get_Usage				(void);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual void				Update_Statistics		(void);
	virtual void				Update_Parent			(void);

	virtual void				On_Key_Down				(wxKeyEvent   &event);
	virtual void				On_Mouse_Motion			(wxMouseEvent &event);

	virtual bool				On_Draw					(void);

	virtual int					Get_Color				(double Value);


private:

	bool						m_Color_bGrad;

	double						m_Color_Min, m_Color_Scale, m_Position[3];

	CSG_Colors					m_Colors;

	CSG_Grid					m_Plane[3];

	CSG_Grids					*m_pGrids;
	

	bool						Inc_ZScale				(double Scale);
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
	EVT_KEY_DOWN(C3D_Viewer_Grids_Panel::On_Key_Down)
	EVT_MOTION  (C3D_Viewer_Grids_Panel::On_Mouse_Motion)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Grids_Panel::C3D_Viewer_Grids_Panel(wxWindow *pParent, CSG_Grids *pGrids)
	: CSG_3DView_Panel(pParent)
{
	m_pGrids = pGrids;

	//-----------------------------------------------------
	m_Parameters.Add_Double("GENERAL",
		"RESOLUTION_XY"	, _TL("Horizontal Resolution"),
		_TL("Horizontal resolution (x/y) in map units. Use [F7]/[F8] keys to de-/increase."),
		m_pGrids->Get_Extent().Get_Diameter() / 200,
		m_pGrids->Get_Cellsize(), true,
		m_pGrids->Get_Extent().Get_Diameter() / 10, true
	);

	m_Parameters.Add_Choice("RESOLUTION_XY",
		"RESAMPLING_XY"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation"),
			_TL("Mean Value"),
			_TL("Mean Value (cell area weighted)")
		), 0
	);

	m_Parameters.Add_Int("GENERAL",
		"RESOLUTION_Z"	, _TL("Vertical Resolution"),
		_TL("Vertical resolution (z) in number of levels. Use [F9]/[F10] keys to de-/increase."),
		M_GET_MIN(m_pGrids->Get_NZ() * 4, 200), 10, true
	);

	m_Parameters.Add_Choice("RESOLUTION_Z",
		"RESAMPLING_Z"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Linear Interpolation"),
			_TL("Spline Interpolation")
		), 1
	);

	//-----------------------------------------------------
	if( !SG_UI_DataObject_Colors_Get(m_pGrids, &m_Colors) )
	{
		m_Colors.Set_Palette(SG_COLORS_RAINBOW);
	}

	m_Parameters.Add_Colors("GENERAL", "COLORS"       , _TL("Colors"           ), _TL(""), &m_Colors);
	m_Parameters.Add_Bool  ("COLORS" , "COLORS_GRAD"  , _TL("Graduated"        ), _TL(""), true);
	m_Parameters.Add_Range ("COLORS" , "COLOR_STRETCH", _TL("Histogram Stretch"), _TL(""),
		m_pGrids->Get_Mean() - 2. * m_pGrids->Get_StdDev(),
		m_pGrids->Get_Mean() + 2. * m_pGrids->Get_StdDev()
	);

	m_Parameters.Add_Choice("GENERAL", "SHADING"      , _TL("Light Source"     ), _TL(""), CSG_String::Format("%s|%s", _TL("no"), _TL("yes")), 1);
	m_Parameters.Add_Double("SHADING", "SHADE_DEC"    , _TL("Height"           ), _TL(""), 45., -90., true, 90., true);
	m_Parameters.Add_Double("SHADING", "SHADE_AZI"    , _TL("Direction"        ), _TL(""), 45., -90., true, 90., true);

	//-----------------------------------------------------
	m_Parameters("Z_SCALE")->Set_Value(0.2 * (m_pGrids->Get_XRange() + m_pGrids->Get_YRange()) / m_pGrids->Get_ZRange());

	m_Position[PLANE_SIDE_X] = 0.5;
	m_Position[PLANE_SIDE_Y] = 0.5;
	m_Position[PLANE_SIDE_Z] = 0.5;

	m_Label_zType = m_pGrids->Get_Attributes().Get_Field_Type(m_pGrids->Get_Z_Attribute());

	m_BoxBuffer = 0.;

	m_Projector.Set_zShift(-0.4);
	m_Projector.Set_yShift(-0.1);
	m_Projector.Set_xRotation(60. * M_DEG_TO_RAD);
	m_Projector.Set_zRotation(45. * M_DEG_TO_RAD);

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
		pParameter->Set_Children_Enabled(pParameter->asInt() > 0);
	}

	return( CSG_3DView_Panel::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_Grids_Panel::Update_Statistics(void)
{
	m_Data_Min.x = m_pGrids->Get_XMin();
	m_Data_Max.x = m_pGrids->Get_XMax();

	m_Data_Min.y = m_pGrids->Get_YMin();
	m_Data_Max.y = m_pGrids->Get_YMax();

	m_Data_Min.z = m_pGrids->Get_ZMin();
	m_Data_Max.z = m_pGrids->Get_ZMax();

	Set_Planes(); // Update_View();
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
bool C3D_Viewer_Grids_Panel::Inc_ZScale(double Increase)
{
	double d = (m_pGrids->Get_XRange() + m_pGrids->Get_YRange()) / m_pGrids->Get_ZRange();

	m_Parameters("Z_SCALE")->Set_Value(m_Parameters("Z_SCALE")->asDouble() + d * Increase / 10.);

	Update_View();

	return( true );
}

//---------------------------------------------------------
bool C3D_Viewer_Grids_Panel::Set_ZLevel(bool bIncrease)
{
	for(int i=1; i<m_pGrids->Get_NZ(); i++)
	{
		double z = m_pGrids->Get_Z(bIncrease ? i : m_pGrids->Get_NZ() - 1 - i);

		z = (z - m_pGrids->Get_ZMin()) / m_pGrids->Get_ZRange();
		z = (int)(z * 100.) / 100.;	// rounding

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
		double d = m_Parameters("RESOLUTION_Z")->asDouble();

		m_Parameters("RESOLUTION_Z")->Set_Value(bIncrease
			? d + 5
			: d - 5
		);
	}
	else
	{
		double d = m_Parameters("RESOLUTION_XY")->asDouble();

		m_Parameters("RESOLUTION_XY")->Set_Value(bIncrease
			? d - m_pGrids->Get_Cellsize()
			: d + m_pGrids->Get_Cellsize()
		);
	}

	Set_Planes();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String C3D_Viewer_Grids_Panel::Get_Usage(void)
{
	CSG_Table Shortcuts(CSG_3DView_Panel::Get_Shortcuts());

	#define ADD_SHORTCUT(KEY, CMD) { CSG_Table_Record &r = *Shortcuts.Add_Record(); r.Set_Value(0, KEY); r.Set_Value(1, CMD); }

	ADD_SHORTCUT("F3", _TL("Decrease Horizontal Resolution"));
	ADD_SHORTCUT("F4", _TL("Increase Horizontal Resolution"));

	ADD_SHORTCUT("F5", _TL("Decrease Vertical Resolution"  ));
	ADD_SHORTCUT("F6", _TL("Increase Vertical Resolution"  ));

	ADD_SHORTCUT("F7", _TL("Previous Level"                ));
	ADD_SHORTCUT("F8", _TL("Next Level"                    ));

	return( CSG_3DView_Panel::Get_Usage(Shortcuts) );
}

//---------------------------------------------------------
void C3D_Viewer_Grids_Panel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default: CSG_3DView_Panel::On_Key_Down(event); return;

	case WXK_F1: Inc_ZScale(-0.5); break;
	case WXK_F2: Inc_ZScale( 0.5); break;

	case WXK_F3: Set_Resolution(false, false); break;
	case WXK_F4: Set_Resolution( true, false); break;

	case WXK_F5: Set_Resolution(false,  true); break;
	case WXK_F6: Set_Resolution( true,  true); break;

	case WXK_F7: Set_ZLevel(false); break;
	case WXK_F8: Set_ZLevel( true); break;
	}

	Update_Parent();
}

//---------------------------------------------------------
#define GET_MOUSE_X_RELDIFF	((double)(m_Down_Screen.x - event.GetX()) / (double)GetClientSize().x)
#define GET_MOUSE_Y_RELDIFF	((double)(m_Down_Screen.y - event.GetY()) / (double)GetClientSize().y)

//---------------------------------------------------------
void C3D_Viewer_Grids_Panel::On_Mouse_Motion(wxMouseEvent &event)
{
	if( HasCapture() && event.Dragging() && event.ShiftDown() && event.LeftIsDown() )
	{
		m_Projector.Set_Central_Distance(m_Down_Value.x + GET_MOUSE_X_RELDIFF);

		double d = (m_pGrids->Get_XRange() + m_pGrids->Get_YRange()) / m_pGrids->Get_ZRange();

		m_Parameters("Z_SCALE")->Set_Value(m_Down_Value.y + d * GET_MOUSE_Y_RELDIFF);

		Update_View(); Update_Parent();

		event.Skip(); return;
	}

	CSG_3DView_Panel::On_Mouse_Motion(event);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Grids_Panel::Get_Color(double Value)
{
	if( m_Color_Scale <= 0. )
	{
		return( (int)Value );
	}

	double c = m_Color_Scale * (Value - m_Color_Min);

	return( m_Color_bGrad ? m_Colors.Get_Interpolated(c) : m_Colors[(int)c] );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Grids_Panel::On_Draw(void)
{
	m_Colors      =*m_Parameters("COLORS")->asColors();
	m_Color_bGrad = m_Parameters("COLORS_GRAD")->asBool();

	m_Color_Min   = m_Parameters("COLOR_STRETCH")->asRange()->Get_Min();
	double  Range = m_Parameters("COLOR_STRETCH")->asRange()->Get_Max() - m_Color_Min;
	m_Color_Scale = Range > 0. ? (m_Colors.Get_Count() - 1) / Range : 0.;

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
	double Cellsize = m_Parameters("RESOLUTION_XY")->asDouble();

	if( Cellsize < m_pGrids->Get_Cellsize() )
	{
		Cellsize = m_pGrids->Get_Cellsize();
	}

	int zLevels = m_Parameters("RESOLUTION_Z")->asInt();

	//-----------------------------------------------------
	TSG_Grid_Resampling zResampling, Resampling;

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
				Plane.Create(CSG_Grid_System(Cellsize, 0., m_pGrids->Get_YMin(), Cellsize * zLevels, m_pGrids->Get_YMax()));
			}

			double dz = m_pGrids->Get_ZRange() / Plane.Get_NX();

			#pragma omp parallel for
			for(int y=0; y<Plane.Get_NY(); y++)
			{
				TSG_Point_3D p;

				p.z = m_pGrids->Get_ZMin();
				p.y = Plane.Get_YMin() + Plane.Get_Cellsize() * y;
				p.x = m_pGrids->Get_XMin() + Position * m_pGrids->Get_XRange();

				for(int x=0; x<Plane.Get_NX(); x++, p.z+=dz)
				{
					double Value;

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
				Plane.Create(CSG_Grid_System(Cellsize, m_pGrids->Get_XMin(), 0., m_pGrids->Get_XMax(), Cellsize * zLevels));
			}

			double dz = m_pGrids->Get_ZRange() / Plane.Get_NY();

			#pragma omp parallel for
			for(int x=0; x<Plane.Get_NX(); x++)
			{
				TSG_Point_3D p;

				p.z = m_pGrids->Get_ZMin();
				p.y = m_pGrids->Get_YMin() + Position * m_pGrids->Get_YRange();
				p.x = Plane.Get_XMin() + Plane.Get_Cellsize() * x;

				for(int y=0; y<Plane.Get_NY(); y++, p.z+=dz)
				{
					double Value;

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

			#pragma omp parallel for
			for(int y=0; y<Plane.Get_NY(); y++)
			{
				TSG_Point_3D p;

				p.z = m_pGrids->Get_ZMin() + Position * m_pGrids->Get_ZRange();
				p.y = Plane.Get_YMin() + Plane.Get_Cellsize() * y;
				p.x = Plane.Get_XMin();

				for(int x=0; x<Plane.Get_NX(); x++, p.x+=Plane.Get_Cellsize())
				{
					double Value;

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
		TSG_Point_3D p;

		switch( Side )
		{
		case PLANE_SIDE_X:
			p.x = Position;
			p.y = m_pGrids->Get_YMin() + y * m_pGrids->Get_YRange() / (Plane.Get_NY() - 1);
			p.z = m_pGrids->Get_ZMin() + x * m_pGrids->Get_ZRange() / (Plane.Get_NX() - 1);
			break;

		case PLANE_SIDE_Y:
			p.x = m_pGrids->Get_XMin() + x * m_pGrids->Get_XRange() / (Plane.Get_NX() - 1);
			p.y = Position;
			p.z = m_pGrids->Get_ZMin() + y * m_pGrids->Get_ZRange() / (Plane.Get_NY() - 1);
			break;

		case PLANE_SIDE_Z:
			p.x = m_pGrids->Get_XMin() + x * m_pGrids->Get_XRange() / (Plane.Get_NX() - 1);
			p.y = m_pGrids->Get_YMin() + y * m_pGrids->Get_YRange() / (Plane.Get_NY() - 1);
			p.z = Position;
			break;
		}

		m_Projector.Get_Projection(p);

		Node.x = p.x;
		Node.y = p.y;
		Node.z = p.z;
		Node.c = Plane.asDouble(x, y);

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
	CSG_Vector LightSource;

	if( m_Parameters("SHADING")->asInt() && LightSource.Create(3) )
	{
		double decline = m_Parameters("SHADE_DEC")->asDouble() * -M_DEG_TO_RAD;
		double azimuth = m_Parameters("SHADE_AZI")->asDouble() *  M_DEG_TO_RAD;

		LightSource[0] = sin(decline) * cos(azimuth);
		LightSource[1] = sin(decline) * sin(azimuth);
		LightSource[2] = cos(decline);
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=1; y<Plane.Get_NY(); y++) for(int x=1; x<Plane.Get_NX(); x++)
	{
		TSG_Triangle_Node p[3];

		if( Get_Node(Plane, Position, Side, x    , y    , p[1])
		&&  Get_Node(Plane, Position, Side, x - 1, y - 1, p[0]) )
		{
			if( Get_Node(Plane, Position, Side, x, y - 1, p[2]) )
			{
				if( LightSource.Get_Size() )
					Draw_Triangle(p, false, LightSource, 1);
				else
					Draw_Triangle(p, false);
			}

			p[2] = p[0]; p[0] = p[1]; p[1] = p[2]; // for shading, let's keep the triangle's normal vector orientation

			if( Get_Node(Plane, Position, Side, x - 1, y, p[2]) )
			{
				if( LightSource.Get_Size() )
					Draw_Triangle(p, false, LightSource, 1);
				else
					Draw_Triangle(p, false);
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
#define HISTOGRAM_AS_PANEL

//---------------------------------------------------------
#ifdef HISTOGRAM_AS_PANEL
class C3D_Viewer_Grids_Histogram : public wxPanel
#else
class C3D_Viewer_Grids_Histogram : public wxDialog
#endif
{
public:
	C3D_Viewer_Grids_Histogram(wxWindow *pParent, CSG_Grids *pGrids, C3D_Viewer_Grids_Panel *pPanel)
	#ifdef HISTOGRAM_AS_PANEL
		: wxPanel (pParent, wxID_ANY, wxDefaultPosition, wxSize(200, 200), wxTAB_TRAVERSAL|wxSTATIC_BORDER|wxNO_FULL_REPAINT_ON_RESIZE)
	#else
		: wxDialog(pParent, wxID_ANY, _TL("Histogram"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxCLOSE_BOX|wxSTAY_ON_TOP)
	#endif
	{
		m_pPanel      = pPanel;
		m_pGrids      = pGrids;
		m_nClasses    = 100;
		m_bCumulative = false;

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
		if( event.ControlDown() )
		{
			if( m_nClasses > 10 ) { m_nClasses -= 10; Set_Histogram(false); }
		}
		else
		{
			CaptureMouse(); m_Mouse_Down = m_Mouse_Move = event.GetPosition();
		}
	}

	//---------------------------------------------------------
	void					On_Mouse_Motion	(wxMouseEvent &event)
	{
		if( HasCapture() && event.Dragging() && event.LeftIsDown() )
		{
			m_Mouse_Move = event.GetPosition();

			if( m_Mouse_Down.x != m_Mouse_Move.x )
			{
				Refresh();
			}
		}
	}

	//---------------------------------------------------------
	void					On_Mouse_LUp	(wxMouseEvent &event)
	{
		if( HasCapture() )
		{
			ReleaseMouse();

			m_Mouse_Move = event.GetPosition();

			if( m_Mouse_Down.x == m_Mouse_Move.x )
			{
				Refresh();
			}
			else
			{
				double Minimum = m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Get_Min();
				double Range   = m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Get_Max() - Minimum;

				m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Set_Range(
					Minimum + (m_Mouse_Down.x * Range / GetClientSize().GetWidth()),
					Minimum + (m_Mouse_Move.x * Range / GetClientSize().GetWidth())
				);

				m_Mouse_Down = m_Mouse_Move = wxPoint(0, 0);

				Set_Histogram();
			}
		}
	}

	//---------------------------------------------------------
	void					On_Mouse_MDown	(wxMouseEvent &event)
	{
		m_bCumulative = !m_bCumulative; Refresh();
	}

	//---------------------------------------------------------
	void					On_Mouse_RDown	(wxMouseEvent &event)
	{
		if( event.ControlDown() )
		{
			if( m_nClasses < 1000 ) { m_nClasses += 10; Set_Histogram(false); }
		}
		else
		{
			m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Set_Range(m_pGrids->Get_Min(), m_pGrids->Get_Max());

			Set_Histogram();
		}
	}

	//---------------------------------------------------------
	void					On_Key_Down		(wxKeyEvent   &event)
	{
		switch( event.GetKeyCode() )
		{
		case WXK_SPACE: m_bCumulative = !m_bCumulative; Refresh(); break;

		case WXK_NUMPAD_ADD     : if( m_nClasses < 1000 ) { m_nClasses += 10; Set_Histogram(false); } break;
		case WXK_NUMPAD_SUBTRACT: if( m_nClasses >   10 ) { m_nClasses -= 10; Set_Histogram(false); } break;

		case WXK_PAGEUP  : Set_Size(GetClientSize().GetWidth() * 1.25, GetClientSize().GetHeight() * 1.25, true); break;
		case WXK_PAGEDOWN: Set_Size(GetClientSize().GetWidth() / 1.25, GetClientSize().GetHeight() / 1.25, true); break;
		}
	}

	//---------------------------------------------------------
	void					On_Close		(wxCloseEvent &event)
	{
		Hide();	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
	}

	//---------------------------------------------------------
	void					Set_Size		(int Width, int Height, bool bRefresh)
	{
		if( Width >= 100 && Height >= 100 && Width <= 1000 && Height <= 1000 )
		{
			SetClientSize(Width, Height); Refresh();
		}
	}

	//---------------------------------------------------------
	void					Set_Histogram	(bool bRefresh = true)
	{
		double Minimum = m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Get_Min();
		double Maximum = m_pPanel->m_Parameters("COLOR_STRETCH")->asRange()->Get_Max();

		m_Histogram.Create(m_nClasses, Minimum, Maximum, m_pGrids, m_pGrids->Get_Max_Samples());

		Refresh();

		if( bRefresh )
		{
			m_pPanel->Update_View();
		}
	}

	//---------------------------------------------------------
	void					On_Paint		(wxPaintEvent &WXUNUSED(event))
	{
		wxPaintDC dc(this); Draw(dc, GetClientSize());
	}

	//---------------------------------------------------------
	void					Draw			(wxDC &dc, const wxRect &r)
	{
		if( m_Histogram.Get_Class_Count() > 0 && m_Histogram.Get_Element_Count() > 0 )
		{
			int	Mouse_Min = m_Mouse_Down.x <  m_Mouse_Move.x ? m_Mouse_Down.x : m_Mouse_Move.x;
			int Mouse_Max = m_Mouse_Down.x >= m_Mouse_Move.x ? m_Mouse_Down.x : m_Mouse_Move.x;

			if( Mouse_Max > Mouse_Min )
			{
				dc.SetPen(*wxBLACK_PEN); dc.SetBrush(*wxWHITE_BRUSH);
				dc.DrawRectangle(Mouse_Min, r.GetTop(), Mouse_Max - Mouse_Min, r.GetHeight());
			}

			CSG_Colors Colors(*m_pPanel->m_Parameters("COLORS")->asColors());

			double dC = m_Histogram.Get_Class_Count() > 1 ? ((Colors.Get_Count() - 1.) / (m_Histogram.Get_Class_Count() - 1.)) : 0.;
			double dx = (double)r.GetWidth() / (double)m_Histogram.Get_Class_Count();

			int ay = r.GetBottom() + 1;
			int ax = r.GetLeft  ();

			for(size_t iClass=0; iClass<m_Histogram.Get_Class_Count(); iClass++)
			{
				double Value = m_bCumulative
					? m_Histogram.Get_Cumulative(iClass) / (double)m_Histogram.Get_Element_Count  ()
					: m_Histogram.Get_Elements  (iClass) / (double)m_Histogram.Get_Element_Maximum();

				int bx = ax; ax	= r.GetLeft() + (int)(dx * (iClass + 1.));
				int by = ay - (int)(r.GetHeight() * Value);

				wxColour c((unsigned long)Colors.Get_Interpolated(dC * iClass));

				dc.SetPen(wxPen(c)); dc.SetBrush(wxBrush(c));
				dc.DrawRectangle(bx, ay, ax - bx, by - ay); //	Draw_FillRect(dc, Color, ax, ay, bx, by);
			}

			if( Mouse_Max > Mouse_Min )
			{
				wxRect rBox(Mouse_Min, r.GetTop(), Mouse_Max - Mouse_Min, r.GetHeight());

				dc.SetPen(wxPen(*wxBLACK));
				dc.DrawLine(rBox.GetLeft (), rBox.GetTop   (), rBox.GetRight(), rBox.GetTop   ());
				dc.DrawLine(rBox.GetLeft (), rBox.GetBottom(), rBox.GetRight(), rBox.GetBottom());
				dc.DrawLine(rBox.GetLeft (), rBox.GetTop   (), rBox.GetLeft (), rBox.GetBottom());
				dc.DrawLine(rBox.GetRight(), rBox.GetTop   (), rBox.GetRight(), rBox.GetBottom());

				dc.SetPen(wxPen(*wxWHITE)); rBox.Deflate(1);
				dc.DrawLine(rBox.GetLeft (), rBox.GetTop   (), rBox.GetRight(), rBox.GetTop   ());
				dc.DrawLine(rBox.GetLeft (), rBox.GetBottom(), rBox.GetRight(), rBox.GetBottom());
				dc.DrawLine(rBox.GetLeft (), rBox.GetTop   (), rBox.GetLeft (), rBox.GetBottom());
				dc.DrawLine(rBox.GetRight(), rBox.GetTop   (), rBox.GetRight(), rBox.GetBottom());
			}
		}
	}

	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
#ifdef HISTOGRAM_AS_PANEL
BEGIN_EVENT_TABLE(C3D_Viewer_Grids_Histogram, wxPanel)
#else
BEGIN_EVENT_TABLE(C3D_Viewer_Grids_Histogram, wxDialog)
	EVT_CLOSE      (C3D_Viewer_Grids_Histogram::On_Close)
	EVT_KEY_DOWN   (C3D_Viewer_Grids_Histogram::On_Key_Down)
#endif
	EVT_LEFT_DOWN  (C3D_Viewer_Grids_Histogram::On_Mouse_LDown)
	EVT_LEFT_UP    (C3D_Viewer_Grids_Histogram::On_Mouse_LUp)
	EVT_MIDDLE_DOWN(C3D_Viewer_Grids_Histogram::On_Mouse_MDown)
	EVT_RIGHT_DOWN (C3D_Viewer_Grids_Histogram::On_Mouse_RDown)
	EVT_MOTION     (C3D_Viewer_Grids_Histogram::On_Mouse_Motion)
	EVT_PAINT      (C3D_Viewer_Grids_Histogram::On_Paint)
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
		C3D_Viewer_Grids_Panel *pPanel = new C3D_Viewer_Grids_Panel(this, pGrids);

		Create(pPanel);

		//-------------------------------------------------
		Add_Spacer();

		m_pSlide[PLANE_SIDE_X] = Add_Slider(wxString::Format("X %s", _TL("Pane")), pPanel->m_Position[PLANE_SIDE_X], 0, 1);
		m_pSlide[PLANE_SIDE_Y] = Add_Slider(wxString::Format("Y %s", _TL("Pane")), pPanel->m_Position[PLANE_SIDE_Y], 0, 1);
		m_pSlide[PLANE_SIDE_Z] = Add_Slider(wxString::Format("Z %s", _TL("Pane")), pPanel->m_Position[PLANE_SIDE_Z], 0, 1);

		//-------------------------------------------------
		Add_Spacer();

		m_pHistogram = new C3D_Viewer_Grids_Histogram(this, pGrids, pPanel);

	#ifdef HISTOGRAM_AS_PANEL
		Add_CustomCtrl("", m_pHistogram);
	#else
		m_pHistogram_Check = Add_CheckBox(_TL("Histogram"), false);
	#endif

		//-------------------------------------------------
		Add_Spacer();

		const wxString Choices[] = { _TL("None"), _TL("Linear"), _TL("Spline") };

		m_pResampling = Add_Choice(_TL("Resampling"), wxArrayString(3, Choices), 1); // Linear

		//-------------------------------------------------
		Add_Spacer();

		m_pLabel[0] = Add_Label (_TL("Light Source Height"   ), true);
		m_pShade[0] = Add_Slider("", m_pPanel->m_Parameters("SHADE_DEC")->asDouble(), -90., 90.);
		m_pLabel[1] = Add_Label (_TL("Light Source Direction"), true);
		m_pShade[1] = Add_Slider("", m_pPanel->m_Parameters("SHADE_AZI")->asDouble(), -90., 90.);

		m_pLabel[0]->Hide(); m_pLabel[1]->Hide();
		m_pShade[0]->Hide(); m_pShade[1]->Hide();
	}

#ifndef HISTOGRAM_AS_PANEL
	virtual ~C3D_Viewer_Grids_Dialog(void)
	{
		m_pHistogram->Destroy();
	}
#endif


protected:

	CSGDI_Slider				*m_pSlide[3], *m_pShade[2];

	wxStaticText				*m_pLabel[2];

	wxChoice					*m_pResampling;

	C3D_Viewer_Grids_Histogram	*m_pHistogram;

#ifndef HISTOGRAM_AS_PANEL
	wxCheckBox					*m_pHistogram_Check;
#endif


	//-----------------------------------------------------
	enum
	{
		MENU_SCALE_Z_DEC = MENU_USER_FIRST,
		MENU_SCALE_Z_INC,
		MENU_LEVEL_Z_DEC,
		MENU_LEVEL_Z_INC,
		MENU_RESLT_Z_DEC,
		MENU_RESLT_Z_INC,
		MENU_RESLT_XY_DEC,
		MENU_RESLT_XY_INC,
		MENU_SHOW_SHADER
	};

	//-----------------------------------------------------
	virtual void				Set_Menu			(wxMenu &Menu)
	{
		wxMenu *pMenu = Menu.FindChildItem(Menu.FindItem(_TL("Display")))->GetSubMenu();

		pMenu->AppendSeparator();
		pMenu->Append(MENU_SCALE_Z_DEC , wxString::Format("%s [F1]", _TL("Decrease Exaggeration"         )));
		pMenu->Append(MENU_SCALE_Z_INC , wxString::Format("%s [F2]", _TL("Increase Exaggeration"         )));

		pMenu->AppendSeparator();
		pMenu->Append(MENU_RESLT_XY_DEC, wxString::Format("%s [F3]", _TL("Decrease Horizontal Resolution")));
		pMenu->Append(MENU_RESLT_XY_INC, wxString::Format("%s [F4]", _TL("Increase Horizontal Resolution")));

		pMenu->Append(MENU_RESLT_Z_DEC , wxString::Format("%s [F5]", _TL("Decrease Vertical Resolution"  )));
		pMenu->Append(MENU_RESLT_Z_INC , wxString::Format("%s [F6]", _TL("Increase Vertical Resolution"  )));

		pMenu->AppendSeparator();
		pMenu->Append(MENU_LEVEL_Z_DEC , wxString::Format("%s [F7]", _TL("Previous Level"                )));
		pMenu->Append(MENU_LEVEL_Z_INC , wxString::Format("%s [F8]", _TL("Next Level"                    )));

		pMenu->AppendSeparator();
		pMenu->AppendCheckItem(MENU_SHOW_SHADER, _TL("Toggle Light Source Controls"));
	}

	//-----------------------------------------------------
	virtual void				On_Menu				(wxCommandEvent &event)
	{
		C3D_Viewer_Grids_Panel *pPanel = (C3D_Viewer_Grids_Panel *)m_pPanel;

		switch( event.GetId() )
		{
		default: CSG_3DView_Dialog::On_Menu(event); return;

		#ifdef __WXMAC__
		case MENU_PROPERTIES: if( SG_UI_Dlg_Parameters(&m_pPanel->Get_Parameters(), "") ) { m_pPanel->Set_Parameters(); } return;
		#endif

		case MENU_USAGE       : SG_UI_Dlg_Info(C3D_Viewer_Grids_Panel::Get_Usage(), _TL("Usage")); return;

		case MENU_SCALE_Z_DEC : pPanel->Inc_ZScale(-0.5); break;
		case MENU_SCALE_Z_INC : pPanel->Inc_ZScale( 0.5); break;

		case MENU_LEVEL_Z_DEC : pPanel->Set_ZLevel(false); break;
		case MENU_LEVEL_Z_INC : pPanel->Set_ZLevel( true); break;

		case MENU_RESLT_XY_DEC: pPanel->Set_Resolution(false, false); break;
		case MENU_RESLT_XY_INC: pPanel->Set_Resolution( true, false); break;

		case MENU_RESLT_Z_DEC : pPanel->Set_Resolution(false,  true); break;
		case MENU_RESLT_Z_INC : pPanel->Set_Resolution( true,  true); break;

		case MENU_SHOW_SHADER :
			if( m_pShade[0]->IsShown() )
			{
				m_pLabel[0]->Hide(); m_pLabel[1]->Hide();
				m_pShade[0]->Hide(); m_pShade[1]->Hide();
			}
			else
			{
				m_pLabel[0]->Show(); m_pLabel[1]->Show();
				m_pShade[0]->Show(); m_pShade[1]->Show();
			}

			m_pShade[0]->GetParent()->Layout();
			break;
		}
	}

	//-----------------------------------------------------
	virtual void				On_Menu_UI			(wxUpdateUIEvent &event)
	{
		switch( event.GetId() )
		{
		default: CSG_3DView_Dialog::On_Menu_UI(event); return;

		case MENU_SHOW_SHADER : event.Check(m_pShade[0]->IsShown()); break;
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

		if( event.GetEventObject() == m_pShade[0] )
		{
			m_pPanel->m_Parameters.Set_Parameter("SHADE_DEC", m_pShade[0]->Get_Value()); m_pPanel->Update_View();
		}

		if( event.GetEventObject() == m_pShade[1] )
		{
			m_pPanel->m_Parameters.Set_Parameter("SHADE_AZI", m_pShade[1]->Get_Value()); m_pPanel->Update_View();
		}

	#ifndef HISTOGRAM_AS_PANEL
		if( event.GetEventObject() == m_pHistogram_Check )
		{
		#ifdef _SAGA_MSW
			m_pHistogram->Show(m_pHistogram_Check->GetValue() == 1 ? true : false);	// unluckily this does not work with linux (broken event handler chain, non-modal dialog as subprocess of a modal one!!)
		#else
			m_pHistogram->ShowModal();
		#endif
		}
	#endif

		CSG_3DView_Dialog::On_Update_Control(event);
	}

	//-----------------------------------------------------
	virtual void				Update_Controls			(void)
	{
		C3D_Viewer_Grids_Panel *pPanel = (C3D_Viewer_Grids_Panel *)m_pPanel;

		m_pSlide[PLANE_SIDE_X]->Set_Value(pPanel->m_Position[PLANE_SIDE_X]);
		m_pSlide[PLANE_SIDE_Y]->Set_Value(pPanel->m_Position[PLANE_SIDE_Y]);
		m_pSlide[PLANE_SIDE_Z]->Set_Value(pPanel->m_Position[PLANE_SIDE_Z]);

		m_pShade[0]->Set_Value(m_pPanel->m_Parameters("SHADE_DEC")->asDouble());
		m_pShade[1]->Set_Value(m_pPanel->m_Parameters("SHADE_AZI")->asDouble());

		m_pHistogram->Refresh();

	#ifndef HISTOGRAM_AS_PANEL
		m_pHistogram_Check->SetValue(m_pHistogram->IsShown());
	#endif

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

	Set_Description(Get_Description() + C3D_Viewer_Grids_Panel::Get_Usage());

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
	CSG_Grids *pGrids = Parameters("GRIDS")->asGrids();

	if( pGrids->Get_Grid_Count() <= 0 )
	{
		Message_Add(_TL("invalid input"));

		return( false );
	}

	C3D_Viewer_Grids_Dialog dlg(pGrids);

	dlg.ShowModal();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
