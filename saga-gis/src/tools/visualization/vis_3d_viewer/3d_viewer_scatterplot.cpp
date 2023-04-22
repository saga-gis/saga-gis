
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
//                3d_viewer_scatterplot.cpp              //
//                                                       //
//                 Copyright (C) 2023 by                 //
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

#include "3d_viewer_scatterplot.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_Scatterplot_Panel : public CSG_3DView_Panel
{
public:
	C3D_Viewer_Scatterplot_Panel(wxWindow *pParent, CSG_Grid *pX, CSG_Grid *pY, CSG_Grid *pZ, int MaxBins);

	static CSG_String			Get_Usage				(void);

	void						Set_Extent				(CSG_Rect Extent);

	bool						Set_Points				(void);
	bool						Set_Aggregated			(int nBins, bool bNormalized);


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

	int							m_MaxBins;

	double						m_Color_Min, m_Color_Scale, m_Color_Dim_Min, m_Color_Dim_Max;

	CSG_Array					m_Selection;

	CSG_Colors					m_Colors;

	CSG_Rect					m_Extent;

	CSG_Grid					*m_pGrid[3];

	CSG_PointCloud				m_Points;


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_Scatterplot_Panel, CSG_3DView_Panel)
	EVT_KEY_DOWN	(C3D_Viewer_Scatterplot_Panel::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Scatterplot_Panel::C3D_Viewer_Scatterplot_Panel(wxWindow *pParent, CSG_Grid *pX, CSG_Grid *pY, CSG_Grid *pZ, int MaxBins)
	: CSG_3DView_Panel(pParent)
{
	m_pGrid[0] = pX;
	m_pGrid[1] = pY;
	m_pGrid[2] = pZ;

	m_MaxBins  = MaxBins;

	m_Points.Add_Field(_TL("Count"), SG_DATATYPE_Int);

	//-----------------------------------------------------
	CSG_String Attributes;

	for(int i=0; i<m_Points.Get_Field_Count(); i++)
	{
		Attributes += m_Points.Get_Field_Name(i); Attributes += "|";
	}

	//-----------------------------------------------------
	m_Parameters.Add_Double("GENERAL"    , "DETAIL"       , _TL("Level of Detail" ), _TL(""), 100., 0., true, 100., true);

	m_Parameters.Add_Choice("GENERAL"    , "COLORS_ATTR"  , _TL("Color Attribute" ), _TL(""), Attributes, 3);
	m_Parameters.Add_Colors("COLORS_ATTR", "COLORS"       , _TL("Colors"          ), _TL(""));
	m_Parameters.Add_Bool  ("COLORS_ATTR", "COLORS_GRAD"  , _TL("Graduated"       ), _TL(""), true);
	m_Parameters.Add_Range ("COLORS_ATTR", "COLORS_RANGE" , _TL("Value Range"     ), _TL(""));

	m_Parameters.Add_Bool  ("GENERAL"    , "DIM"          , _TL("Dim"             ), _TL(""), false);
	m_Parameters.Add_Range ("DIM"        , "DIM_RANGE"    , _TL("Distance Range"  ), _TL(""), 0., 1., 0., true);

	m_Parameters.Add_Int   ("GENERAL"    , "SIZE"         , _TL("Size"            ), _TL(""), 1, 1, true);
	m_Parameters.Add_Double("GENERAL"    , "SIZE_SCALE"   , _TL("Size Scaling"    ), _TL(""), 0., 0., true);

	//-----------------------------------------------------
	Set_Aggregated(m_MaxBins, false);

	m_Selection.Create(sizeof(sLong), 0, TSG_Array_Growth::SG_ARRAY_GROWTH_2);

	Update_Statistics();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Scatterplot_Panel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
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
bool C3D_Viewer_Scatterplot_Panel::Set_Points(void)
{
	m_Points.Del_Points();

	for(sLong i=0; i<m_pGrid[0]->Get_NCells() && SG_UI_Process_Get_Okay(); i++)
	{
		if( !m_pGrid[0]->is_NoData(i) && !m_pGrid[1]->is_NoData(i) && !m_pGrid[2]->is_NoData(i) )
		{
			m_Points.Add_Point(m_pGrid[0]->asDouble(i), m_pGrid[1]->asDouble(i), m_pGrid[2]->asDouble(i));

			m_Points.Set_Attribute(0, 1.);
		}
	}

	if( m_Points.Get_Count() > 0 )
	{
		m_Extent = m_Points.Get_Extent();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool C3D_Viewer_Scatterplot_Panel::Set_Aggregated(int nBins, bool bNormalize)
{
	static bool bBuisy = false;

	if( bBuisy )
	{
		return( false );
	}

	bBuisy = true;

	m_Points.Del_Points();

	double d[3];

	d[0] = (nBins - 1.) / m_pGrid[0]->Get_Range();
	d[1] = (nBins - 1.) / m_pGrid[1]->Get_Range();
	d[2] = (nBins - 1.) / m_pGrid[2]->Get_Range();

	CSG_Array_Int Histogram(nBins*nBins*nBins); Histogram.Assign(0);

	for(sLong i=0; i<m_pGrid[0]->Get_NCells() && SG_UI_Process_Get_Okay(); i++)
	{
		if( !m_pGrid[0]->is_NoData(i) && !m_pGrid[1]->is_NoData(i) && !m_pGrid[2]->is_NoData(i) )
		{
			int x = (int)(d[0] * (m_pGrid[0]->asDouble(i) - m_pGrid[0]->Get_Min())); if( x < 0 ) x = 0; else if( x >= nBins ) x = nBins - 1;
			int y = (int)(d[1] * (m_pGrid[1]->asDouble(i) - m_pGrid[1]->Get_Min())); if( y < 0 ) y = 0; else if( y >= nBins ) y = nBins - 1;
			int z = (int)(d[2] * (m_pGrid[2]->asDouble(i) - m_pGrid[2]->Get_Min())); if( z < 0 ) z = 0; else if( z >= nBins ) z = nBins - 1;

			Histogram[x + y * nBins + z * nBins*nBins]++;
		}
	}

	for(int z=0, iz=0; z<nBins; z++, iz+=nBins*nBins) for(int y=0, iy=iz; y<nBins; y++, iy+=nBins) for(int x=0, i=iy; x<nBins; x++, i++)
	{
		int Count = Histogram[i];

		if( Count > 0 )
		{
			if( bNormalize )
			{
				m_Points.Add_Point(
					x / (double)nBins,
					y / (double)nBins,
					z / (double)nBins
				);
			}
			else
			{
				m_Points.Add_Point(
					m_pGrid[0]->Get_Min() + x / d[0],
					m_pGrid[1]->Get_Min() + y / d[1],
					m_pGrid[2]->Get_Min() + z / d[2]
				);
			}

			m_Points.Set_Attribute(0, log((double)Count));
		}
	}

	bBuisy = false;

	if( m_Points.Get_Count() > 0 )
	{
		m_Extent = m_Points.Get_Extent();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void C3D_Viewer_Scatterplot_Panel::Set_Extent(CSG_Rect Extent)
{
	if( Extent.Get_XRange() == 0. || Extent.Get_YRange() == 0. )
	{
		Extent = m_Points.Get_Extent();
	}
	else
	{
		Extent.Intersect(m_Points.Get_Extent());
	}

	if( Extent.is_Equal(m_Extent) == false )
	{
		m_Extent = Extent;
			
		Update_View(true);
	}
}

//---------------------------------------------------------
void C3D_Viewer_Scatterplot_Panel::Update_Statistics(void)
{
	m_Data_Min.x = m_Extent.Get_XMin();
	m_Data_Max.x = m_Extent.Get_XMax();

	m_Data_Min.y = m_Extent.Get_YMin();
	m_Data_Max.y = m_Extent.Get_YMax();

	m_Selection.Set_Array(0);

	if( m_Extent.is_Equal(m_Points.Get_Extent()) )
	{
		int cField = m_Parameters("COLORS_ATTR")->asInt();

		m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
			m_Points.Get_Mean(cField) - 1.5 * m_Points.Get_StdDev(cField),
			m_Points.Get_Mean(cField) + 1.5 * m_Points.Get_StdDev(cField)
		);

		m_Data_Min.z = m_Points.Get_Minimum(2);	// Get_ZMin();	ToDo in CSG_PointCloud class!!!
		m_Data_Max.z = m_Points.Get_Maximum(2);	// Get_ZMax();	ToDo in CSG_PointCloud class!!!
	}
	else
	{
		CSG_Simple_Statistics cStats, zStats;

		int cField = m_Parameters("COLORS_ATTR")->asInt();

		for(sLong i=0; i<m_Points.Get_Count(); i++)
		{
			m_Points.Set_Cursor(i);

			if( m_Extent.Contains(m_Points.Get_X(), m_Points.Get_Y()) && m_Selection.Inc_Array() )
			{
				*((sLong *)m_Selection.Get_Entry(m_Selection.Get_Size() - 1))	= i;

				cStats += m_Points.Get_Value(cField);
				zStats += m_Points.Get_Z();
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
void C3D_Viewer_Scatterplot_Panel::Update_Parent(void)
{
	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String C3D_Viewer_Scatterplot_Panel::Get_Usage(void)
{
	CSG_Table Shortcuts(CSG_3DView_Panel::Get_Shortcuts());

	#define ADD_SHORTCUT(KEY, CMD) { CSG_Table_Record &r = *Shortcuts.Add_Record(); r.Set_Value(0, KEY); r.Set_Value(1, CMD); }

	ADD_SHORTCUT("F3", _TL("Decrease Size"        ));
	ADD_SHORTCUT("F4", _TL("Increase Size"        ));

	ADD_SHORTCUT("F5", _TL("Decrease Size Scaling"));
	ADD_SHORTCUT("F6", _TL("Increase Size Scaling"));

	return( CSG_3DView_Panel::Get_Usage(Shortcuts) );
}

//---------------------------------------------------------
void C3D_Viewer_Scatterplot_Panel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default    : CSG_3DView_Panel::On_Key_Down(event); return;

	case WXK_F3: Parameter_Value_Add("SIZE"      ,  -1.); break;
	case WXK_F4: Parameter_Value_Add("SIZE"      ,   1.); break;

	case WXK_F5: Parameter_Value_Add("SIZE_SCALE", -10.); break;
	case WXK_F6: Parameter_Value_Add("SIZE_SCALE",  10.); break;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Scatterplot_Panel::On_Before_Draw(void)
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
int C3D_Viewer_Scatterplot_Panel::Get_Color(double Value, double z)
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
bool C3D_Viewer_Scatterplot_Panel::On_Draw(void)
{
	int cField = m_Parameters("COLORS_ATTR")->asInt();

	if( m_Parameters("COLORS_RANGE")->asRange()->Get_Min()
	>=  m_Parameters("COLORS_RANGE")->asRange()->Get_Max() )
	{
		m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
			m_Points.Get_Mean(cField) - 1.5 * m_Points.Get_StdDev(cField),
			m_Points.Get_Mean(cField) + 1.5 * m_Points.Get_StdDev(cField)
		);
	}

	m_Colors      = *m_Parameters("COLORS")->asColors();
	m_Color_bGrad = m_Parameters("COLORS_GRAD")->asBool();
	m_Color_Min   = m_Parameters("COLORS_RANGE.MIN")->asDouble();
	m_Color_Scale = m_Parameters("COLORS_RANGE.MAX")->asDouble() - m_Color_Min;

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
	
	int nSkip   = 1 + (int)(0.001 * m_Points.Get_Count() * SG_Get_Square(1. - 0.01 * m_Parameters("DETAIL")->asDouble()));

	sLong	nPoints	= m_Selection.Get_Size() > 0 ? m_Selection.Get_Size() : m_Points.Get_Count();

	#pragma omp parallel for
	for(sLong iPoint=0; iPoint<nPoints; iPoint+=nSkip)
	{
		sLong jPoint = m_Selection.Get_Size() > 0 ? *((sLong *)m_Selection.Get_Entry(iPoint)) : iPoint;

		TSG_Point_3D p = m_Points.Get_Point(jPoint); m_Projector.Get_Projection(p);

		double Size = minSize; if( dSize > 0. ) { Size += (int)(50. * exp(-p.z / dSize)); }

		Draw_Point(p.x, p.y, p.z, Get_Color(m_Points.Get_Value(jPoint, cField), p.z), Size);
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
class C3D_Viewer_Scatterplot_Dialog : public CSG_3DView_Dialog
{
public:
	C3D_Viewer_Scatterplot_Dialog(CSG_Grid *pX, CSG_Grid *pY, CSG_Grid *pZ, int MaxBins)
		: CSG_3DView_Dialog(_TL("3D Scatterplot Viewer"))
	{
		Create(new C3D_Viewer_Scatterplot_Panel(this, pX, pY, pZ, MaxBins));

		//-------------------------------------------------
		Add_Spacer();

		wxArrayString Fields; Fields.Add("X"); Fields.Add("Y"); Fields.Add("Z"); Fields.Add("Count");

		m_pField_C = Add_Choice  (_TL("Color"), Fields, 3);

		//-------------------------------------------------
		Add_Spacer();

		m_pDetail  = Add_Slider  (_TL("Level of Detail"), m_pPanel->m_Parameters("DETAIL")->asDouble(), 0., 100.);
		m_pBins    = Add_Slider  (_TL("Number of Bins" ), MaxBins, 16, MaxBins);

		//-------------------------------------------------
		Add_Spacer();
	}

	virtual void				Update_Controls			(void);


protected:

	wxChoice					*m_pField_C;

	CSGDI_Slider				*m_pDetail, *m_pBins;


	//-----------------------------------------------------
	enum
	{
		MENU_SCALE_Z_DEC = MENU_USER_FIRST,
		MENU_SCALE_Z_INC,
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
BEGIN_EVENT_TABLE(C3D_Viewer_Scatterplot_Dialog, CSG_3DView_Dialog)
	EVT_CHECKBOX(wxID_ANY, C3D_Viewer_Scatterplot_Dialog::On_Update_Control)
	EVT_CHOICE  (wxID_ANY, C3D_Viewer_Scatterplot_Dialog::On_Update_Choices)
END_EVENT_TABLE()

//---------------------------------------------------------
void C3D_Viewer_Scatterplot_Dialog::On_Update_Choices(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pField_C )
	{
		m_pPanel->m_Parameters.Set_Parameter("COLORS_ATTR", m_pField_C->GetSelection());
		m_pPanel->Update_View(true);
	}

	CSG_3DView_Dialog::On_Update_Choices(event);
}

//---------------------------------------------------------
void C3D_Viewer_Scatterplot_Dialog::On_Update_Control(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pDetail )
	{
		m_pPanel->m_Parameters.Set_Parameter("DETAIL", m_pDetail->Get_Value());
		m_pPanel->Update_View();
	}

	if( event.GetEventObject() == m_pBins )
	{
		((C3D_Viewer_Scatterplot_Panel *)m_pPanel)->Set_Aggregated((int)m_pBins->Get_Value(), false);
		m_pPanel->Update_View();
	}

	CSG_3DView_Dialog::On_Update_Control(event);
}

//---------------------------------------------------------
void C3D_Viewer_Scatterplot_Dialog::Update_Controls(void)
{
	m_pField_C->SetSelection(m_pPanel->m_Parameters("COLORS_ATTR")->asInt());

	CSG_3DView_Dialog::Update_Controls();
}

//---------------------------------------------------------
void C3D_Viewer_Scatterplot_Dialog::Set_Menu(wxMenu &Menu)
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
	pMenu->AppendCheckItem(MENU_COLORS_GRAD   , _TL("Graduated Colors"));
}

//---------------------------------------------------------
void C3D_Viewer_Scatterplot_Dialog::On_Menu(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	default: CSG_3DView_Dialog::On_Menu(event); break;

	case MENU_USAGE         : SG_UI_Dlg_Info(C3D_Viewer_Scatterplot_Panel::Get_Usage(), _TL("Usage")); return;

	case MENU_SCALE_Z_DEC   : m_pPanel->Parameter_Value_Add("Z_SCALE"   , -0.5); break;
	case MENU_SCALE_Z_INC   : m_pPanel->Parameter_Value_Add("Z_SCALE"   ,  0.5); break;

	case MENU_SIZE_DEC      : m_pPanel->Parameter_Value_Add("SIZE"      , -1.0); break;
	case MENU_SIZE_INC      : m_pPanel->Parameter_Value_Add("SIZE"      ,  1.0); break;

	case MENU_SIZE_SCALE_DEC: m_pPanel->Parameter_Value_Add("SIZE_SCALE", -1.0); break;
	case MENU_SIZE_SCALE_INC: m_pPanel->Parameter_Value_Add("SIZE_SCALE",  1.0); break;

	case MENU_COLORS_GRAD   : m_pPanel->Parameter_Value_Toggle("COLORS_GRAD"  ); break;
	}
}

//---------------------------------------------------------
void C3D_Viewer_Scatterplot_Dialog::On_Menu_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		CSG_3DView_Dialog::On_Menu_UI(event);
		break;

	case MENU_COLORS_GRAD: event.Check(m_pPanel->m_Parameters("COLORS_GRAD")->asBool()); break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Scatterplot::C3D_Viewer_Scatterplot(void)
{
	Set_Name		(_TL("3D Scatterplot Viewer"));

	Set_Author		("O. Conrad (c) 2014");

	Set_Description	(_TW(
		"3D scatterplot viewer for grids."
	));

	Set_Description(Get_Description() + C3D_Viewer_Scatterplot_Panel::Get_Usage());

	//-----------------------------------------------------
	Parameters.Add_Grid("", "GRID_X", CSG_String::Format("%s (X)", _TL("Grid")), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "GRID_Y", CSG_String::Format("%s (Y)", _TL("Grid")), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "GRID_Z", CSG_String::Format("%s (Z)", _TL("Grid")), _TL(""), PARAMETER_INPUT);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Scatterplot::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("AGGREGATE") )
	{
		pParameter->Set_Children_Enabled(pParameter->asBool());
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Scatterplot::On_Execute(void)
{
	CSG_Grid *pX = Parameters("GRID_X")->asGrid(); if( pX->Get_Range() <= 0. ) { Error_Fmt("%s (X)", _TL("no variation found for grid")); return( false ); }
	CSG_Grid *pY = Parameters("GRID_Y")->asGrid(); if( pY->Get_Range() <= 0. ) { Error_Fmt("%s (Y)", _TL("no variation found for grid")); return( false ); }
	CSG_Grid *pZ = Parameters("GRID_Z")->asGrid(); if( pZ->Get_Range() <= 0. ) { Error_Fmt("%s (Z)", _TL("no variation found for grid")); return( false ); }

	C3D_Viewer_Scatterplot_Dialog dlg(pX, pY, pZ, 256);

	dlg.ShowModal();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
