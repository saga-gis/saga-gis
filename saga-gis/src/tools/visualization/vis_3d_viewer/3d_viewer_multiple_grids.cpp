
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
//              3d_viewer_multiple_grids.cpp             //
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
#include "3d_viewer_multiple_grids.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_Multiple_Grids_Panel : public CSG_3DView_Panel
{
public:
	C3D_Viewer_Multiple_Grids_Panel(wxWindow *pParent, CSG_Parameter_Grid_List *pGrids);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual void				Update_Statistics		(void);
	virtual void				Update_Parent			(void);

	virtual void				On_Key_Down				(wxKeyEvent &event);

	virtual bool				On_Draw					(void);

	virtual int					Get_Color				(double Value);


private:

	bool						m_Color_bGrad;

	double						m_Color_Min, m_Color_Scale;

	CSG_Colors					m_Colors;

	CSG_Parameter_Grid_List		*m_pGrids;


	bool						Get_Node				(CSG_Grid *pGrid, int x, int y, TSG_Triangle_Node &Node);

	void						Draw_Grid				(CSG_Grid *pGrid);


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_Multiple_Grids_Panel, CSG_3DView_Panel)
	EVT_KEY_DOWN	(C3D_Viewer_Multiple_Grids_Panel::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Multiple_Grids_Panel::C3D_Viewer_Multiple_Grids_Panel(wxWindow *pParent, CSG_Parameter_Grid_List *pGrids)
	: CSG_3DView_Panel(pParent)
{
	m_pGrids = pGrids;

	m_Parameters.Add_Colors("GENERAL", "COLORS"     , _TL("Colours"     ), _TL(""));
	m_Parameters.Add_Bool  ("COLORS" , "COLORS_GRAD", _TL("Graduated"   ), _TL(""), true);

	m_Parameters.Add_Choice("GENERAL", "SHADING"    , _TL("Light Source"), _TL(""), CSG_String::Format("%s|%s", _TL("no"), _TL("yes")), 1);
	m_Parameters.Add_Double("SHADING", "SHADE_DEC"  , _TL("Height"      ), _TL(""), 45., -180., true, 180., true);
	m_Parameters.Add_Double("SHADING", "SHADE_AZI"  , _TL("Direction"   ), _TL(""), 90., -180., true, 180., true);

	Update_Statistics();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Multiple_Grids_Panel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
void C3D_Viewer_Multiple_Grids_Panel::Update_Statistics(void)
{
	m_Data_Min.x = m_pGrids->Get_Grid(0)->Get_XMin();
	m_Data_Max.x = m_pGrids->Get_Grid(0)->Get_XMax();

	m_Data_Min.y = m_pGrids->Get_Grid(0)->Get_YMin();
	m_Data_Max.y = m_pGrids->Get_Grid(0)->Get_YMax();

	m_Data_Min.z = m_pGrids->Get_Grid(0)->Get_Min();
	m_Data_Max.z = m_pGrids->Get_Grid(0)->Get_Max();

	for(int i=1; i<m_pGrids->Get_Grid_Count(); i++)
	{
		CSG_Grid *pGrid = m_pGrids->Get_Grid(i);

		if( m_Data_Min.x > pGrid->Get_XMin() )
			m_Data_Min.x = pGrid->Get_XMin();	else
		if( m_Data_Max.x < pGrid->Get_XMax() )
			m_Data_Max.x = pGrid->Get_XMax();

		if( m_Data_Min.y > pGrid->Get_YMin() )
			m_Data_Min.y = pGrid->Get_YMin();	else
		if( m_Data_Max.y < pGrid->Get_YMax() )
			m_Data_Max.y = pGrid->Get_YMax();

		if( m_Data_Min.z > pGrid->Get_Min() )
			m_Data_Min.z = pGrid->Get_Min();	else
		if( m_Data_Max.z < pGrid->Get_Max() )
			m_Data_Max.z = pGrid->Get_Max();
	}

	Update_View();
}

//---------------------------------------------------------
void C3D_Viewer_Multiple_Grids_Panel::Update_Parent(void)
{
	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_Multiple_Grids_Panel::On_Key_Down(wxKeyEvent &event)
{
	CSG_3DView_Panel::On_Key_Down(event);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Multiple_Grids_Panel::Get_Color(double Value)
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
bool C3D_Viewer_Multiple_Grids_Panel::On_Draw(void)
{
	for(int i=0; i<m_pGrids->Get_Grid_Count(); i++)
	{
		Draw_Grid(m_pGrids->Get_Grid(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool C3D_Viewer_Multiple_Grids_Panel::Get_Node(CSG_Grid *pGrid, int x, int y, TSG_Triangle_Node &Node)
{
	if( pGrid->is_InGrid(x, y) )
	{
		TSG_Point_3D	p;

		p.x = pGrid->Get_System().Get_xGrid_to_World(x);
		p.y = pGrid->Get_System().Get_yGrid_to_World(y);
		p.z = Node.c = pGrid->asDouble(x, y);

		m_Projector.Get_Projection(p);

		Node.x = p.x;
		Node.y = p.y;
		Node.z = p.z;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void C3D_Viewer_Multiple_Grids_Panel::Draw_Grid(CSG_Grid *pGrid)
{
	//-----------------------------------------------------
	if( !SG_UI_DataObject_Colors_Get(pGrid, &m_Colors) )
	{
		m_Colors  = *m_Parameters("COLORS")->asColors();
	}

	m_Color_bGrad = m_Parameters("COLORS_GRAD")->asBool();

	m_Color_Min   = pGrid->Get_Min();
	m_Color_Scale = pGrid->Get_Range() > 0. ? m_Colors.Get_Count() / pGrid->Get_Range() : 0.;

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
	for(int y=1; y<pGrid->Get_NY(); y++) for(int x=1; x<pGrid->Get_NX(); x++)
	{
		TSG_Triangle_Node p[3];

		if( Get_Node(pGrid, x - 1, y - 1, p[0])
		&&  Get_Node(pGrid, x    , y    , p[1]) )
		{
			if( Get_Node(pGrid, x, y - 1, p[2]) )
			{
				if( LightSource.Get_Size() )
					Draw_Triangle(p, false, LightSource);
				else
					Draw_Triangle(p, false);
			}

			p[2] = p[0]; p[0] = p[1]; p[1] = p[2]; // for shading, let's keep the triangle's normal vector orientation

			if( Get_Node(pGrid, x - 1, y, p[2]) )
			{
				if( LightSource.Get_Size() )
					Draw_Triangle(p, false, LightSource);
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
class C3D_Viewer_Multiple_Grids_Dialog : public CSG_3DView_Dialog
{
public:
	C3D_Viewer_Multiple_Grids_Dialog(CSG_Parameter_Grid_List *pGrids)
		: CSG_3DView_Dialog(_TL("Multiple Grids Viewer"))
	{
		Create(new C3D_Viewer_Multiple_Grids_Panel(this, pGrids));

		Add_Spacer();
		m_pShade[0] = Add_Slider(_TL("Light Source Height"   ), m_pPanel->m_Parameters("SHADE_DEC")->asDouble(), -180., 180.);
		m_pShade[1] = Add_Slider(_TL("Light Source Direction"), m_pPanel->m_Parameters("SHADE_AZI")->asDouble(), -180., 180.);
	}

	virtual void				Update_Controls			(void)
	{
		m_pShade[0]->Set_Value(m_pPanel->m_Parameters("SHADE_DEC")->asDouble());
		m_pShade[1]->Set_Value(m_pPanel->m_Parameters("SHADE_AZI")->asDouble());

		CSG_3DView_Dialog::Update_Controls();
	}


protected:

	enum
	{
		MENU_SCALE_Z_DEC = MENU_USER_FIRST,
		MENU_SCALE_Z_INC
	};

	CSGDI_Slider				*m_pShade[2];


	//-----------------------------------------------------
	virtual void				On_Update_Control		(wxCommandEvent &event)
	{
		if( event.GetEventObject() == m_pShade[0] ) { m_pPanel->m_Parameters.Set_Parameter("SHADE_DEC", m_pShade[0]->Get_Value()); m_pPanel->Update_View(); }
		if( event.GetEventObject() == m_pShade[1] ) { m_pPanel->m_Parameters.Set_Parameter("SHADE_AZI", m_pShade[1]->Get_Value()); m_pPanel->Update_View(); }

		CSG_3DView_Dialog::On_Update_Control(event);
	}

	//-----------------------------------------------------
	virtual void				Set_Menu				(wxMenu &Menu)
	{
		wxMenu *pMenu = Menu.FindChildItem(Menu.FindItem(_TL("Display")))->GetSubMenu();

		pMenu->AppendSeparator();
		pMenu->Append(MENU_SCALE_Z_DEC, _TL("Decrease Exaggeration [F1]"));
		pMenu->Append(MENU_SCALE_Z_INC, _TL("Increase Exaggeration [F2]"));
	}

	//-----------------------------------------------------
	virtual void				On_Menu					(wxCommandEvent &event)
	{
		switch( event.GetId() )
		{
		default: CSG_3DView_Dialog::On_Menu(event); return;

		#ifdef __WXMAC__
		case MENU_PROPERTIES: if( SG_UI_Dlg_Parameters(&m_pPanel->Get_Parameters(), "") ) { m_pPanel->Set_Parameters(); } return;
		#endif

		case MENU_SCALE_Z_DEC: m_pPanel->Parameter_Value_Add("Z_SCALE", -0.5); break;
		case MENU_SCALE_Z_INC: m_pPanel->Parameter_Value_Add("Z_SCALE",  0.5); break;
		}
	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Multiple_Grids::C3D_Viewer_Multiple_Grids(void)
{
	Set_Name		(_TL("Multiple Grids Viewer"));

	Set_Author		("O. Conrad (c) 2014");

	Set_Description	(_TW(
		""
	));

	Set_Description(Get_Description() + C3D_Viewer_Multiple_Grids_Panel::Get_Usage());

	Parameters.Add_Grid_List("", "GRIDS", _TL("Grids"), _TL(""), PARAMETER_INPUT);
}

//---------------------------------------------------------
bool C3D_Viewer_Multiple_Grids::On_Execute(void)
{
	CSG_Parameter_Grid_List *pGrids = Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() <= 0 )
	{
		Message_Add(_TL("invalid input"));

		return( false );
	}

	C3D_Viewer_Multiple_Grids_Dialog dlg(pGrids);

	dlg.ShowModal();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
