
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
//                3d_viewer_pointcloud.cpp               //
//                                                       //
//                 Copyright (C) 2014 by                 //
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

#include "3d_viewer_pointcloud.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_PointCloud_Panel : public CSG_3DView_Panel
{
public:
	C3D_Viewer_PointCloud_Panel(wxWindow *pParent, CSG_PointCloud *pPoints, int Field_Color);

	void						Set_Extent				(CSG_Rect Extent);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual void				Update_Statistics		(void);
	virtual void				Update_Parent			(void);

	virtual void				On_Key_Down				(wxKeyEvent   &event);

	virtual bool				On_Before_Draw			(void);
	virtual bool				On_Draw					(void);

	virtual int					Get_Color				(double Value, double z);


private:

	bool						m_Color_bGrad;

	double						m_Color_Min, m_Color_Scale, m_Color_Dim_Min, m_Color_Dim_Max;

	CSG_Array					m_Selection;

	CSG_Colors					m_Colors;

	CSG_Rect					m_Extent;

	CSG_PointCloud				*m_pPoints;


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_PointCloud_Panel, CSG_3DView_Panel)
	EVT_KEY_DOWN	(C3D_Viewer_PointCloud_Panel::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_PointCloud_Panel::C3D_Viewer_PointCloud_Panel(wxWindow *pParent, CSG_PointCloud *pPoints, int Field_Color)
	: CSG_3DView_Panel(pParent)
{
	m_pPoints = pPoints;

	//-----------------------------------------------------
	CSG_String Attributes;

	for(int i=0; i<m_pPoints->Get_Field_Count(); i++)
	{
		Attributes += m_pPoints->Get_Field_Name(i); Attributes += "|";
	}

	//-----------------------------------------------------
	m_Parameters.Add_Double("GENERAL"    , "DETAIL"       , _TL("Level of Detail" ), _TL(""), 100., 0., true, 100., true);

	m_Parameters.Add_Choice("GENERAL"    , "COLORS_ATTR"  , _TL("Color Attribute" ), _TL(""), Attributes, Field_Color);
	m_Parameters.Add_Bool  ("COLORS_ATTR", "VAL_AS_RGB"   , _TL("RGB Values"      ), _TL(""), false);
	m_Parameters.Add_Colors("COLORS_ATTR", "COLORS"       , _TL("Colors"          ), _TL(""));
	m_Parameters.Add_Bool  ("COLORS_ATTR", "COLORS_GRAD"  , _TL("Graduated"       ), _TL(""), true);
	m_Parameters.Add_Range ("COLORS_ATTR", "COLORS_RANGE" , _TL("Value Range"     ), _TL(""));

	m_Parameters.Add_Bool  ("GENERAL"    , "DIM"          , _TL("Dim"             ), _TL(""), false);
	m_Parameters.Add_Range ("DIM"        , "DIM_RANGE"    , _TL("Distance Range"  ), _TL(""), 0., 1., 0., true);

	m_Parameters.Add_Int   ("GENERAL"    , "SIZE"         , _TL("Size"            ), _TL(""), 1, 1, true);
	m_Parameters.Add_Double("GENERAL"    , "SIZE_SCALE"   , _TL("Size Scaling"    ), _TL(""), 0., 0., true);

	m_Parameters.Add_Choice("GENERAL"    , "OVERVIEW_ATTR", _TL("Overview Content"), _TL(""),
		CSG_String::Format("%s|%s",
			_TL("average value"),
			_TL("number of points")
		), 0
	);

	//-----------------------------------------------------
	m_Extent = pPoints->Get_Extent();

	m_Selection.Create(sizeof(int), 0, SG_ARRAY_GROWTH_2);

	Update_Statistics();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_PointCloud_Panel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("VAL_AS_RGB") )
	{
		pParameters->Set_Enabled("COLORS"      , pParameter->asBool() == false);
		pParameters->Set_Enabled("COLORS_RANGE", pParameter->asBool() == false);
	}

	if( pParameter->Cmp_Identifier("DIM") )
	{
		pParameters->Set_Enabled("DIM_RANGE"   , pParameter->asBool());
	}

	return( CSG_3DView_Panel::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Panel::Set_Extent(CSG_Rect Extent)
{
	if( Extent.Get_XRange() == 0. || Extent.Get_YRange() == 0. )
	{
		Extent = m_pPoints->Get_Extent();
	}
	else
	{
		Extent.Intersect(m_pPoints->Get_Extent());
	}

	if( Extent.is_Equal(m_Extent) == false )
	{
		m_Extent = Extent;
			
		Update_View(true);
	}
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Panel::Update_Statistics(void)
{
	m_Data_Min.x = m_Extent.Get_XMin();
	m_Data_Max.x = m_Extent.Get_XMax();

	m_Data_Min.y = m_Extent.Get_YMin();
	m_Data_Max.y = m_Extent.Get_YMax();

	m_Selection.Set_Array(0);

	if( m_Extent.is_Equal(m_pPoints->Get_Extent()) )
	{
		int cField = m_Parameters("COLORS_ATTR")->asInt();

		m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
			m_pPoints->Get_Mean(cField) - 1.5 * m_pPoints->Get_StdDev(cField),
			m_pPoints->Get_Mean(cField) + 1.5 * m_pPoints->Get_StdDev(cField)
		);

		m_Data_Min.z = m_pPoints->Get_Minimum(2);	// Get_ZMin();	ToDo in CSG_PointCloud class!!!
		m_Data_Max.z = m_pPoints->Get_Maximum(2);	// Get_ZMax();	ToDo in CSG_PointCloud class!!!
	}
	else
	{
		CSG_Simple_Statistics cStats, zStats;

		int cField = m_Parameters("COLORS_ATTR")->asInt();

		for(int i=0; i<m_pPoints->Get_Count(); i++)
		{
			m_pPoints->Set_Cursor(i);

			if( m_Extent.Contains(m_pPoints->Get_X(), m_pPoints->Get_Y()) && m_Selection.Inc_Array() )
			{
				*((int *)m_Selection.Get_Entry(m_Selection.Get_Size() - 1))	= i;

				cStats += m_pPoints->Get_Value(cField);
				zStats += m_pPoints->Get_Z();
			}
		}

		m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
			cStats.Get_Mean() - 1.5 * cStats.Get_StdDev(),
			cStats.Get_Mean() + 1.5 * cStats.Get_StdDev()
		);

		m_Data_Min.z = zStats.Get_Minimum();
		m_Data_Max.z = zStats.Get_Maximum();
	}
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Panel::Update_Parent(void)
{
	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Panel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default    : CSG_3DView_Panel::On_Key_Down(event); return;

	case WXK_F3: Parameter_Value_Add("SIZE"      , -1.); break;
	case WXK_F4: Parameter_Value_Add("SIZE"      ,  1.); break;

	case WXK_F5: Parameter_Value_Add("SIZE_SCALE", -1.); break;
	case WXK_F6: Parameter_Value_Add("SIZE_SCALE",  1.); break;
	}

	Update_View(); Update_Parent();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_PointCloud_Panel::On_Before_Draw(void)
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
int C3D_Viewer_PointCloud_Panel::Get_Color(double Value, double z)
{
	int	Color;

	if( m_Color_Scale <= 0. )
	{
		Color = (int)Value;
	}
	else
	{
		double c = m_Color_Scale * (Value - m_Color_Min);

		Color = m_Color_bGrad ? m_Colors.Get_Interpolated(c) : m_Colors[(int)c];
	}

	//-----------------------------------------------------
	if( m_Color_Dim_Min < m_Color_Dim_Max )
	{
		double dim = 1. - (z - m_Color_Dim_Min) / (m_Color_Dim_Max - m_Color_Dim_Min);

		if( dim < 1. )
		{
			Color = Dim_Color(Color, dim < 0.1 ? 0.1 : dim);
		}
	}

	return( Color );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_PointCloud_Panel::On_Draw(void)
{
	int cField = m_Parameters("COLORS_ATTR")->asInt();

	if( m_Parameters("COLORS_RANGE")->asRange()->Get_Min()
	>=  m_Parameters("COLORS_RANGE")->asRange()->Get_Max() )
	{
		m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
			m_pPoints->Get_Mean(cField) - 1.5 * m_pPoints->Get_StdDev(cField),
			m_pPoints->Get_Mean(cField) + 1.5 * m_pPoints->Get_StdDev(cField)
		);
	}

	m_Colors      = *m_Parameters("COLORS")->asColors();
	m_Color_bGrad = m_Parameters("COLORS_GRAD")->asBool();
	m_Color_Min   = m_Parameters("COLORS_RANGE.MIN")->asDouble();
	m_Color_Scale = m_Parameters("COLORS_RANGE.MAX")->asDouble() - m_Color_Min;
	m_Color_Scale = m_Parameters("VAL_AS_RGB")->asBool() || m_Color_Scale <= 0. ? 0. : m_Colors.Get_Count() / m_Color_Scale;

	if( m_Parameters("DIM")->asBool() )
	{
		m_Color_Dim_Min = m_Parameters("DIM_RANGE")->asRange()->Get_Min() * (m_Data_Max.z - m_Data_Min.z);
		m_Color_Dim_Max = m_Parameters("DIM_RANGE")->asRange()->Get_Max() * (m_Data_Max.z - m_Data_Min.z);
	}
	else
	{
		m_Color_Dim_Min = m_Color_Dim_Max = 0.;
	}

	//-----------------------------------------------------
	int minSize = m_Parameters("SIZE")->asInt(); double dSize = m_Parameters("SIZE_SCALE")->asDouble() / 100.;
	
	int nSkip   = 1 + (int)(0.001 * m_pPoints->Get_Count() * SG_Get_Square(1. - 0.01 * m_Parameters("DETAIL")->asDouble()));

	int	nPoints	= m_Selection.Get_Size() > 0 ? (int)m_Selection.Get_Size() : m_pPoints->Get_Count();

	#pragma omp parallel for
	for(int iPoint=0; iPoint<nPoints; iPoint+=nSkip)
	{
		int jPoint = m_Selection.Get_Size() > 0 ? *((int *)m_Selection.Get_Entry(iPoint)) : iPoint;

		TSG_Point_Z p = m_pPoints->Get_Point(jPoint); m_Projector.Get_Projection(p);

		double Size = minSize; if( dSize > 0. ) { Size += (int)(50. * exp(-p.z / dSize)); }

		Draw_Point(p.x, p.y, p.z, Get_Color(m_pPoints->Get_Value(jPoint, cField), p.z), Size);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define OVERVIEW_AS_PANEL

//---------------------------------------------------------
#ifdef OVERVIEW_AS_PANEL
class CPointCloud_Overview : public wxPanel
#else
class CPointCloud_Overview : public wxDialog
#endif
{
public:
	CPointCloud_Overview(wxWindow *pParent, CSG_PointCloud *pPoints, C3D_Viewer_PointCloud_Panel *pPanel)
	#ifdef OVERVIEW_AS_PANEL
		: wxPanel (pParent, wxID_ANY, wxDefaultPosition, wxSize(200, 200), wxTAB_TRAVERSAL|wxSTATIC_BORDER|wxNO_FULL_REPAINT_ON_RESIZE)
	#else
		: wxDialog(pParent, wxID_ANY, _TL("Overview"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxCLOSE_BOX|wxSTAY_ON_TOP)
	#endif
	{
		m_pPanel = pPanel;

		m_bCount = m_pPanel->m_Parameters("OVERVIEW_ATTR")->asInt() == 1;

		double Ratio = pPoints->Get_Extent().Get_XRange() / pPoints->Get_Extent().Get_YRange();

		int Size = GetClientSize().GetWidth(); CSG_Rect r(pPoints->Get_Extent()); CSG_Grid_System System;

	#ifdef OVERVIEW_AS_PANEL
		if( Ratio > 1. )
		{
			System.Create(r.Get_XRange() / 100.,
				r.Get_XMin(), r.Get_YCenter() - r.Get_XRange() / 2.,
				r.Get_XMax(), r.Get_YCenter() + r.Get_XRange() / 2.
			);
		}
		else
		{
			System.Create(r.Get_YRange() / 100.,
				r.Get_XCenter() - r.Get_YRange() / 2., r.Get_YMin(),
				r.Get_XCenter() + r.Get_YRange() / 2., r.Get_YMax()
			);
		}
	#else
		System.Create((Ratio > 1. ? r.Get_XRange() : r.Get_YRange()) / 100., pPoints->Get_Extent());
	#endif

		m_Count.Create(System, SG_DATATYPE_Int   );
		m_Value.Create(System, SG_DATATYPE_Double);

		for(int i=0, x, y; i<pPoints->Get_Count(); i++)
		{
			if( m_Count.Get_System().Get_World_to_Grid(x, y, pPoints->Get_X(i), pPoints->Get_Y(i)) ) // && m_Count.is_InGrid(x, y, false) )
			{
				m_Count.Add_Value(x, y, 1);
				m_Value.Add_Value(x, y, pPoints->Get_Z(i));
			}
		}

		m_Value.Divide(m_Count);

		//-------------------------------------------------
	#ifdef OVERVIEW_AS_PANEL
		Set_Size(Size, Size, false);
	#else
		if( Ratio > 1. )
		{
			Set_Size(Size, (int)(Size / Ratio), false);
		}
		else
		{
			Set_Size((int)(Size * Ratio), Size, false);
		}
	#endif
	}

	void						Set_Mode		(void)
	{
		bool bCount = m_pPanel->m_Parameters("OVERVIEW_ATTR")->asInt() == 1;

		if( m_bCount != bCount )
		{
			m_bCount  = bCount;

			Set_Image(true);
		}
	}


private:

	bool						m_bCount;

	wxPoint						m_Mouse_Down;

	CSG_Rect					m_Selection;

	wxImage						m_Image;

	CSG_Grid					m_Count, m_Value;

	C3D_Viewer_PointCloud_Panel	*m_pPanel;


	//---------------------------------------------------------
	void						On_Mouse_LDown	(wxMouseEvent &event)
	{
		CaptureMouse();

		m_Mouse_Down = event.GetPosition();
	}

	//---------------------------------------------------------
	void						On_Mouse_Motion	(wxMouseEvent &event)
	{
		if( HasCapture() && event.Dragging() && event.LeftIsDown() )
		{
			Draw_Inverse(m_Mouse_Down, event.GetPosition());
		}
	}

	//---------------------------------------------------------
	void						On_Mouse_LUp	(wxMouseEvent &event)
	{
		if( HasCapture() )
		{
			ReleaseMouse();
		}

		if( m_Mouse_Down.x != event.GetX() && m_Mouse_Down.y != event.GetY() )
		{
			Draw_Inverse(m_Mouse_Down, event.GetPosition());

			m_pPanel->Set_Extent(m_Selection);
		}
		else if( m_Selection.Get_XRange() > 0. && m_Selection.Get_YRange() > 0. )
		{
			double dx = (m_Count.Get_XMin() + event.GetX() * m_Count.Get_XRange() / GetClientSize().GetWidth ()) - m_Selection.Get_XCenter();
			double dy = (m_Count.Get_YMax() - event.GetY() * m_Count.Get_YRange() / GetClientSize().GetHeight()) - m_Selection.Get_YCenter();

			m_Selection.Move(dx, dy);

			m_pPanel->Set_Extent(m_Selection);
		}

		Refresh(false);
	}

	//---------------------------------------------------------
	void						On_Mouse_MDown	(wxMouseEvent &event)
	{
		m_pPanel->Parameter_Value_Toggle("OVERVIEW_ATTR");
		Set_Mode();
	}

	//---------------------------------------------------------
	void						On_Mouse_RDown	(wxMouseEvent &event)
	{
		m_Selection.Assign(0., 0., 0., 0.);

		m_pPanel->Set_Extent(m_Selection);

		Refresh(false);
	}

	//---------------------------------------------------------
	void						On_Key_Down		(wxKeyEvent   &event)
	{
		switch( event.GetKeyCode() )
		{
		case WXK_PAGEUP  : Set_Size(GetClientSize().GetWidth() * 1.25, GetClientSize().GetHeight() * 1.25, true); break;
		case WXK_PAGEDOWN: Set_Size(GetClientSize().GetWidth() / 1.25, GetClientSize().GetHeight() / 1.25, true); break;

		case WXK_SPACE:
			m_pPanel->Parameter_Value_Toggle("OVERVIEW_ATTR");
			Set_Image(true);
			break;
		}
	}

	//---------------------------------------------------------
	void						On_Close		(wxCloseEvent &event)
	{
		Hide();	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
	}

	//---------------------------------------------------------
	void						Set_Size		(int Width, int Height, bool bRefresh)
	{
		if( Width >= 100 && Height >= 100 && Width <= 1000 && Height <= 1000 )
		{
			SetClientSize(Width, Height);

			if( !m_Image.IsOk() || m_Image.GetWidth() != Width || m_Image.GetWidth() != Width )
			{
				m_Image.Create(Width, Height, false);

				Set_Image(bRefresh);
			}
		}
	}

	//---------------------------------------------------------
	void						On_Paint		(wxPaintEvent &WXUNUSED(event))
	{
		wxPaintDC dc(this);

		if( m_Image.IsOk() )
		{
			dc.DrawBitmap(wxBitmap(m_Image), GetClientRect().GetTopLeft());
		}

		if( m_Selection.Get_XRange() > 0. && m_Selection.Get_YRange() > 0. )
		{
			double d = GetClientSize().GetWidth () / m_Count.Get_XRange();

			int ax = (int)(d * (m_Selection.Get_XMin() - m_Count.Get_XMin()));
			int bx = (int)(d * (m_Selection.Get_XMax() - m_Count.Get_XMin()));
			int ay = (int)(d * (m_Count.Get_YMax() - m_Selection.Get_YMax()));
			int by = (int)(d * (m_Count.Get_YMax() - m_Selection.Get_YMin()));

			dc.SetPen(wxPen(*wxBLACK));
			dc.DrawLine(ax, ay, ax, by); dc.DrawLine(ax, by, bx, by);
			dc.DrawLine(bx, by, bx, ay); dc.DrawLine(bx, ay, ax, ay);

			dc.SetPen(wxPen(*wxWHITE)); ax--; bx++; ay--; by++;
			dc.DrawLine(ax, ay, ax, by); dc.DrawLine(ax, by, bx, by);
			dc.DrawLine(bx, by, bx, ay); dc.DrawLine(bx, ay, ax, ay);
		}
	}

	//---------------------------------------------------------
	void						Draw_Inverse	(wxPoint A, wxPoint B)
	{
		if( A != B )
		{
			double d = m_Count.Get_XRange() / GetClientSize().GetWidth();

			m_Selection.Assign(
				m_Count.Get_XMin() + d * A.x, m_Count.Get_YMax() - d * A.y,
				m_Count.Get_XMin() + d * B.x, m_Count.Get_YMax() - d * B.y
			);

			Refresh(false);
		}
	}

	//---------------------------------------------------------
	void						Set_Image		(bool bRefresh)
	{
		if( m_Image.IsOk() && m_Count.is_Valid() )
		{
			bool bCount = m_pPanel->m_Parameters("OVERVIEW_ATTR")->asInt() == 1;

			CSG_Colors Colors(11, SG_COLORS_RAINBOW); Colors.Set_Color(0, m_pPanel->m_Parameters("BGCOLOR")->asColor());

			double dx = m_Count.Get_XRange() / (double)m_Image.GetWidth ();
			double dy = m_Count.Get_YRange() / (double)m_Image.GetHeight();
			double dz = (Colors.Get_Count() - 2.) / (bCount ? log(1. + m_Count.Get_Max()) : 4. * m_Value.Get_StdDev());

			#pragma omp parallel for
			for(int y=0; y<m_Image.GetHeight(); y++)
			{
				double iz, ix = m_Count.Get_XMin(), iy = m_Count.Get_YMax() - y * dy;

				for(int x=0; x<m_Image.GetWidth(); x++, ix+=dx)
				{
					if( bCount )
					{
						iz = dz * (m_Count.Get_Value(ix, iy, iz) && iz > 0. ? log(1. + iz) : 0.);
					}
					else if( m_Value.Get_Value(ix, iy, iz) )
					{
						iz = dz * (iz - (m_Value.Get_Mean() - 2. * m_Value.Get_StdDev()));
					}
					else
					{
						iz = 0.;
					}

					int ic = Colors.Get_Interpolated(iz);

					m_Image.SetRGB(x, y, SG_GET_R(ic), SG_GET_G(ic), SG_GET_B(ic));
				}
			}
		}

		if( bRefresh )
		{
			Refresh(false);
		}
	}

	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
#ifdef OVERVIEW_AS_PANEL
BEGIN_EVENT_TABLE(CPointCloud_Overview, wxPanel)
#else
BEGIN_EVENT_TABLE(CPointCloud_Overview, wxDialog)
	EVT_CLOSE     (CPointCloud_Overview::On_Close)
#endif
	EVT_LEFT_DOWN  (CPointCloud_Overview::On_Mouse_LDown)
	EVT_LEFT_UP    (CPointCloud_Overview::On_Mouse_LUp)
	EVT_MIDDLE_DOWN(CPointCloud_Overview::On_Mouse_MDown)
	EVT_RIGHT_DOWN (CPointCloud_Overview::On_Mouse_RDown)
	EVT_MOTION     (CPointCloud_Overview::On_Mouse_Motion)
	EVT_KEY_DOWN   (CPointCloud_Overview::On_Key_Down)
	EVT_PAINT      (CPointCloud_Overview::On_Paint)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_PointCloud_Dialog : public CSG_3DView_Dialog
{
public:
	C3D_Viewer_PointCloud_Dialog(CSG_PointCloud *pPoints, int Field_Color)
		: CSG_3DView_Dialog(_TL("Point Cloud Viewer"))
	{
		Create(new C3D_Viewer_PointCloud_Panel(this, pPoints, Field_Color));

		//-------------------------------------------------
		Add_Spacer();

		wxArrayString Fields; for(int i=0; i<pPoints->Get_Field_Count(); i++) { Fields.Add(pPoints->Get_Field_Name(i)); }

		m_pField_C  = Add_Choice  (_TL("Color"), Fields, Field_Color);

		//-------------------------------------------------
		Add_Spacer();

		m_pDetail   = Add_Slider  (_TL("Level of Detail"), m_pPanel->m_Parameters("DETAIL")->asDouble(), 0., 100.);

		//-------------------------------------------------
		Add_Spacer();

		m_pOverview = new CPointCloud_Overview(this, pPoints, (C3D_Viewer_PointCloud_Panel *)m_pPanel);

	#ifdef OVERVIEW_AS_PANEL
		Add_CustomCtrl("", m_pOverview);
	#else
		m_pOverview_Check = Add_CheckBox(_TL("Overview"), false);
	#endif
	}

#ifndef OVERVIEW_AS_PANEL
	virtual ~C3D_Viewer_PointCloud_Dialog(void)
	{
		m_pOverview->Destroy();
	}
#endif

	virtual void				Update_Controls			(void);


protected:

	wxChoice					*m_pField_C;

	CSGDI_Slider				*m_pDetail;

	CPointCloud_Overview		*m_pOverview;

#ifndef OVERVIEW_AS_PANEL
	wxCheckBox					*m_pOverview_Check;
#endif


	//-----------------------------------------------------
	enum
	{
		MENU_SCALE_Z_DEC = MENU_USER_FIRST,
		MENU_SCALE_Z_INC,
		MENU_VAL_AS_RGB,
		MENU_COLORS_GRAD,
		MENU_SIZE_DEC,
		MENU_SIZE_INC,
		MENU_SIZE_SCALE_DEC,
		MENU_SIZE_SCALE_INC
	};

	//-----------------------------------------------------
	virtual void				On_Update_Choices		(wxCommandEvent  &event);
	virtual void				On_Update_Control		(wxCommandEvent  &event);

	virtual void				Set_Menu				(wxMenu &Menu);
	virtual void				On_Menu					(wxCommandEvent  &event);
	virtual void				On_Menu_UI				(wxUpdateUIEvent &event);


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_PointCloud_Dialog, CSG_3DView_Dialog)
	EVT_CHECKBOX(wxID_ANY, C3D_Viewer_PointCloud_Dialog::On_Update_Control)
	EVT_CHOICE  (wxID_ANY, C3D_Viewer_PointCloud_Dialog::On_Update_Choices)
END_EVENT_TABLE()

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::On_Update_Choices(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pField_C )
	{
		m_pPanel->m_Parameters.Set_Parameter("COLORS_ATTR", m_pField_C->GetSelection());
		m_pPanel->Update_View(true);
	}

	CSG_3DView_Dialog::On_Update_Choices(event);
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::On_Update_Control(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pDetail )
	{
		m_pPanel->m_Parameters.Set_Parameter("DETAIL", m_pDetail->Get_Value());
		m_pPanel->Update_View();
	}

#ifndef OVERVIEW_AS_PANEL
	if( event.GetEventObject() == m_pOverview_Check )
	{
	#ifdef _SAGA_MSW
		m_pOverview->Show(m_pOverview_Check->GetValue() == 1 ? true : false);	// unluckily this does not work with linux (broken event handler chain, non-modal dialog as subprocess of a modal one!!)
	#else
		m_pOverview->ShowModal();
	#endif
	}
#endif

	CSG_3DView_Dialog::On_Update_Control(event);
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::Update_Controls(void)
{
	m_pField_C->SetSelection(m_pPanel->m_Parameters("COLORS_ATTR")->asInt());

#ifndef OVERVIEW_AS_PANEL
	m_pOverview_Check->SetValue(m_pOverview->IsShown());
#endif

	m_pOverview->Set_Mode();

	CSG_3DView_Dialog::Update_Controls();
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::Set_Menu(wxMenu &Menu)
{
	wxMenu *pMenu = Menu.FindChildItem(Menu.FindItem(_TL("Display")))->GetSubMenu();

	pMenu->AppendSeparator();
	pMenu->Append         (MENU_SCALE_Z_DEC   , _TL("Decrease Exaggeration [F1]"));
	pMenu->Append         (MENU_SCALE_Z_INC   , _TL("Increase Exaggeration [F2]"));

	pMenu->AppendSeparator();
	pMenu->Append         (MENU_SIZE_DEC      , _TL("Decrease Size [F5]"));
	pMenu->Append         (MENU_SIZE_INC      , _TL("Increase Size [F6]"));

	pMenu->AppendSeparator();
	pMenu->Append         (MENU_SIZE_SCALE_DEC, _TL("Decrease Size Scaling Factor [F7]"));
	pMenu->Append         (MENU_SIZE_SCALE_INC, _TL("Increase Size Scaling Factor [F8]"));

	pMenu->AppendSeparator();
	pMenu->AppendCheckItem(MENU_VAL_AS_RGB    , _TL("Value as RGB"));
	pMenu->AppendCheckItem(MENU_COLORS_GRAD   , _TL("Graduated Colors"));
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::On_Menu(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	default:
		CSG_3DView_Dialog::On_Menu(event);
		break;

	case MENU_SCALE_Z_DEC   : m_pPanel->Parameter_Value_Add("Z_SCALE"   , -0.5); break;
	case MENU_SCALE_Z_INC   : m_pPanel->Parameter_Value_Add("Z_SCALE"   ,  0.5); break;

	case MENU_SIZE_DEC      : m_pPanel->Parameter_Value_Add("SIZE"      , -1.0); break;
	case MENU_SIZE_INC      : m_pPanel->Parameter_Value_Add("SIZE"      ,  1.0); break;

	case MENU_SIZE_SCALE_DEC: m_pPanel->Parameter_Value_Add("SIZE_SCALE", -1.0); break;
	case MENU_SIZE_SCALE_INC: m_pPanel->Parameter_Value_Add("SIZE_SCALE",  1.0); break;

	case MENU_VAL_AS_RGB    : m_pPanel->Parameter_Value_Toggle("VAL_AS_RGB"   ); break;
	case MENU_COLORS_GRAD   : m_pPanel->Parameter_Value_Toggle("COLORS_GRAD"  ); break;
	}
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::On_Menu_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		CSG_3DView_Dialog::On_Menu_UI(event);
		break;

	case MENU_VAL_AS_RGB : event.Check(m_pPanel->m_Parameters("VAL_AS_RGB" )->asBool()); break;
	case MENU_COLORS_GRAD: event.Check(m_pPanel->m_Parameters("COLORS_GRAD")->asBool()); break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_PointCloud::C3D_Viewer_PointCloud(void)
{
	Set_Name		(_TL("Point Cloud Viewer"));

	Set_Author		("O. Conrad (c) 2014");

	Set_Description	(_TW(
		"3D viewer for point clouds."
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud("",
		"POINTS"	, _TL("Point Cloud"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("POINTS",
		"COLOR"		, _TL("Color"),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_PointCloud::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		if( pParameter->asPointCloud() )
		{
			pParameters->Set_Parameter("COLOR", 2); // let's default to 'Z'
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_PointCloud::On_Execute(void)
{
	CSG_PointCloud *pPoints = Parameters("POINTS")->asPointCloud();

	if( pPoints->Get_Count() <= 0 )
	{
		Message_Add(_TL("point cloud viewer will not be started, because point cloud has no points"));

		return( false );
	}

	C3D_Viewer_PointCloud_Dialog dlg(pPoints, Parameters("COLOR")->asInt());

	dlg.ShowModal();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
