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

	int							m_zResolution;

	double						m_Color_Min, m_Color_Scale, m_Position[3], m_Resolution;

	TSG_Grid_Resampling			m_Resampling;

	CSG_Colors					m_Colors;

	CSG_Grid					m_Plane[3];

	CSG_Grids					*m_pGrids;
	

	bool						Set_zResolution			(int   zResolution);

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
		"RESOLUTION"	, _TL("Resolution"),
		_TL("Horizontal resolution in map units. Use [F7]/[F8] keys to de-/increase."),
		m_pGrids->Get_Extent().Get_Diameter() * 0.01,
		m_pGrids->Get_Cellsize(), true,
		m_pGrids->Get_Extent().Get_Diameter() * 0.1, true
	);

	m_Parameters.Add_Int("NODE_GENERAL",
		"Z_RESOLUTION"	, _TL("Resolution"),
		_TL("Vertical resolution in number of levels. Use [F9]/[F10] keys to de-/increase."),
		m_pGrids->Get_NZ() * 10, 10, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("",
		"NODE_VIEW"		, _TL("Grid View Settings"),
		_TL("")
	);

	m_Parameters.Add_Colors("NODE_VIEW",
		"COLORS"		, _TL("Colours"),
		_TL("")
	);

	if( !SG_UI_DataObject_Colors_Get(m_pGrids, &m_Colors) )
	{
		m_Parameters("COLORS")->asColors()->Assign(m_Colors);
	}

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
		), 0
	);

	m_Parameters.Add_Double("SHADING",
		"SHADE_DEC"		, _TL("Light Source Height"),
		_TL(""),
		0.0, -90.0, true, 90.0, true
	);

	m_Parameters.Add_Double("SHADING",
		"SHADE_AZI"		, _TL("Light Source Direction"),
		_TL(""),
		315.0, 0.0, true, 360.0, true
	);

	//-----------------------------------------------------
	m_Parameters("Z_SCALE")->Set_Value(0.25 * (m_pGrids->Get_XRange() + m_pGrids->Get_YRange()) / m_pGrids->Get_ZRange());

	m_Resampling	= GRID_RESAMPLING_Bilinear;	// bilinear
	m_Resolution	= m_Parameters(  "RESOLUTION")->asDouble();
	m_zResolution	= m_Parameters("Z_RESOLUTION")->asInt();

	m_Position[PLANE_SIDE_X]	= 0.5;
	m_Position[PLANE_SIDE_Y]	= 0.5;
	m_Position[PLANE_SIDE_Z]	= 0.5;

	Update_Statistics();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Grids_Panel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SHADING") )
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
void C3D_Viewer_Grids_Panel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:
		CSG_3DView_Panel::On_Key_Down(event);
		return;

	//-----------------------------------------------------
	case WXK_F1:	{
		double	d	= 0.1 * 0.25 * (m_pGrids->Get_XRange() + m_pGrids->Get_YRange()) / m_pGrids->Get_ZRange();
		m_Parameters("Z_SCALE")->Set_Value(m_Parameters("Z_SCALE")->asDouble() - d);
		Update_View(); Update_Parent();
	}	break;

	case WXK_F2:	{
		double	d	= 0.1 * 0.25 * (m_pGrids->Get_XRange() + m_pGrids->Get_YRange()) / m_pGrids->Get_ZRange();
		m_Parameters("Z_SCALE")->Set_Value(m_Parameters("Z_SCALE")->asDouble() + d);
		Update_View(); Update_Parent();
	}	break;

	//-----------------------------------------------------
	case WXK_F7:
		m_Parameters("RESOLUTION")->Set_Value(m_Parameters("RESOLUTION")->asDouble() + m_pGrids->Get_Cellsize());
		Set_Planes();
		break;

	case WXK_F8:
		m_Parameters("RESOLUTION")->Set_Value(m_Parameters("RESOLUTION")->asDouble() - m_pGrids->Get_Cellsize());
		Set_Planes();
		break;

	//-----------------------------------------------------
	case WXK_F9:
		m_Parameters("Z_RESOLUTION")->Set_Value(m_Parameters("Z_RESOLUTION")->asInt() - 5);
		Set_Planes();
		break;

	case WXK_F10:
		m_Parameters("Z_RESOLUTION")->Set_Value(m_Parameters("Z_RESOLUTION")->asInt() + 5);
		Set_Planes();
		break;
	}
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
	m_Colors		= *m_Parameters("COLORS")->asColors();
	m_Color_bGrad	= m_Parameters("COLORS_GRAD")->asBool();

	m_Color_Min		= m_pGrids->Get_Min();
	m_Color_Scale	= m_pGrids->Get_Range() > 0.0 ? m_Colors.Get_Count() / m_pGrids->Get_Range() : 0.0;

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
	m_Resolution	= m_Parameters(  "RESOLUTION")->asDouble();
	m_zResolution	= m_Parameters("Z_RESOLUTION")->asInt();

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
	if( m_Resolution < m_pGrids->Get_Cellsize() )
	{
		m_Resolution = m_pGrids->Get_Cellsize();
	}

	switch( Side )
	{
	//-----------------------------------------------------
	case PLANE_SIDE_X:
		{
			if( Plane.Get_Cellsize() != m_Resolution )
			{
				Plane.Create(CSG_Grid_System(m_Resolution, 0.0, m_pGrids->Get_YMin(), m_Resolution * m_zResolution, m_pGrids->Get_YMax()));
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

					if( m_pGrids->Get_Value(p, Value, m_Resampling) )
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
			if( Plane.Get_Cellsize() != m_Resolution )
			{
				Plane.Create(CSG_Grid_System(m_Resolution, m_pGrids->Get_XMin(), 0.0, m_pGrids->Get_XMax(), m_Resolution * m_zResolution));
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

					if( m_pGrids->Get_Value(p, Value, m_Resampling) )
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
			if( Plane.Get_Cellsize() != m_Resolution )
			{
				Plane.Create(CSG_Grid_System(m_Resolution, m_pGrids->Get_System().Get_Extent()));
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

					if( m_pGrids->Get_Value(p, Value, m_Resampling) )
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
			p.y	= Plane.Get_System().Get_yGrid_to_World(y);
			p.z	= m_pGrids->Get_ZMin() + x * m_pGrids->Get_ZRange() / Plane.Get_NX();
			break;

		case PLANE_SIDE_Y:
			p.x	= Plane.Get_System().Get_xGrid_to_World(x);
			p.y	= Position;
			p.z	= m_pGrids->Get_ZMin() + y * m_pGrids->Get_ZRange() / Plane.Get_NY();
			break;

		case PLANE_SIDE_Z:
			p.x	= Plane.Get_System().Get_xGrid_to_World(x);
			p.y	= Plane.Get_System().Get_yGrid_to_World(y);
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
class C3D_Viewer_Grids_Dialog : public CSG_3DView_Dialog
{
public:
	C3D_Viewer_Grids_Dialog(CSG_Grids *pGrids)
		: CSG_3DView_Dialog(_TL("Grid Collection Viewer"), 0)
	{
		C3D_Viewer_Grids_Panel	*pPanel	= new C3D_Viewer_Grids_Panel(this, pGrids);

		Create(pPanel);

		Add_Spacer();

		wxArrayString	Choices;

		Choices.Add(_TL("Nearest Neigbhour"           ));
		Choices.Add(_TL("Bilinear Interpolation"      ));
		Choices.Add(_TL("Bicubic Spline Interpolation"));
		Choices.Add(_TL("B-Spline Interpolation"      ));

		m_pResampling	= Add_Choice(_TL("Resampling"), Choices, 1); // GRID_RESAMPLING_Bilinear

		Add_Spacer();

		m_pSlide[PLANE_SIDE_X]	= Add_Slider(_TL("X"), pPanel->m_Position[PLANE_SIDE_X], 0, 1);
		m_pSlide[PLANE_SIDE_Y]	= Add_Slider(_TL("Y"), pPanel->m_Position[PLANE_SIDE_Y], 0, 1);
		m_pSlide[PLANE_SIDE_Z]	= Add_Slider(_TL("Z"), pPanel->m_Position[PLANE_SIDE_Z], 0, 1);

		pPanel->m_Projector.Set_xRotation(M_DEG_TO_RAD * 45);
		pPanel->m_Projector.Set_zRotation(M_DEG_TO_RAD * 35);
		Update_Controls();
	}


protected:

	CSGDI_Slider				*m_pSlide[3];

	wxChoice					*m_pResampling;


	//-----------------------------------------------------
	enum
	{
		MENU_SCALE_Z_DEC	= MENU_USER_FIRST,
		MENU_SCALE_Z_INC
	};

	//-----------------------------------------------------
	virtual void				Set_Menu			(wxMenu &Menu)
	{
		wxMenu	*pMenu	= Menu.FindChildItem(Menu.FindItem(_TL("Display")))->GetSubMenu();

		pMenu->AppendSeparator();
		pMenu->Append(MENU_SCALE_Z_DEC, _TL("Decrease Exaggeration [F1]"));
		pMenu->Append(MENU_SCALE_Z_INC, _TL("Increase Exaggeration [F2]"));
	}

	//-----------------------------------------------------
	virtual void				On_Menu				(wxCommandEvent &event)
	{
		CSG_Grids	*pGrids	= ((C3D_Viewer_Grids_Panel *)m_pPanel)->m_pGrids;

		double	d	= 0.1 * 0.25 * (pGrids->Get_XRange() + pGrids->Get_YRange()) / pGrids->Get_ZRange();

		switch( event.GetId() )
		{
		case MENU_SCALE_Z_DEC:	m_pPanel->m_Parameters("Z_SCALE")->Set_Value(m_pPanel->m_Parameters("Z_SCALE")->asDouble() - d); m_pPanel->Update_View();	return;
		case MENU_SCALE_Z_INC:	m_pPanel->m_Parameters("Z_SCALE")->Set_Value(m_pPanel->m_Parameters("Z_SCALE")->asDouble() + d); m_pPanel->Update_View();	return;
		}

		CSG_3DView_Dialog::On_Menu(event);
	}

	//-----------------------------------------------------
	void						On_Update_Choices(wxCommandEvent &event)
	{
		C3D_Viewer_Grids_Panel	*pPanel	= (C3D_Viewer_Grids_Panel *)m_pPanel;

		if( event.GetEventObject() == m_pResampling )
		{
			switch( m_pResampling->GetSelection() )
			{
			default: pPanel->m_Resampling = GRID_RESAMPLING_NearestNeighbour; break;
			case  1: pPanel->m_Resampling = GRID_RESAMPLING_Bilinear        ; break;
			case  2: pPanel->m_Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
			case  3: pPanel->m_Resampling = GRID_RESAMPLING_BSpline         ; break;
			}

			pPanel->Set_Planes();
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

		CSG_3DView_Dialog::On_Update_Control(event);
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

	Set_Author		("O. Conrad (c) 2017");

	Set_Description	(_TW(
		""
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
