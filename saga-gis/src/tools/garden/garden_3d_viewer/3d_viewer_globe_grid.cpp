/**********************************************************
 * Version $Id: 3d_viewer_globe_grid.cpp 911 2011-02-14 16:38:15Z reklov_w $
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
//                3d_viewer_globe_grid.cpp               //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "3d_viewer_globe_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_Globe_Grid_Panel : public CSG_3DView_Panel
{
public:
	C3D_Viewer_Globe_Grid_Panel(wxWindow *pParent, CSG_Grid *pGrid, CSG_Grid *pZ);
	virtual ~C3D_Viewer_Globe_Grid_Panel(void);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual void				Update_Statistics		(void);
	virtual void				Update_Parent			(void);

	virtual void				On_Key_Down				(wxKeyEvent   &event);

	virtual bool				On_Draw					(void);

	virtual int					Get_Color				(double Value);


private:

	bool						m_Color_bGrad;

	double						m_Color_Min, m_Color_Scale, m_Radius;

	CSG_Colors					m_Colors;

	CSG_Grid					*m_pGrid, *m_pZ;

	TSG_Point_Z					**m_pNodes;


	bool						Create_Nodes			(void);

	bool						Get_Node				(int x, int y, TSG_Triangle_Node &Node);


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_Globe_Grid_Panel, CSG_3DView_Panel)
	EVT_KEY_DOWN	(C3D_Viewer_Globe_Grid_Panel::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Globe_Grid_Panel::C3D_Viewer_Globe_Grid_Panel(wxWindow *pParent, CSG_Grid *pGrid, CSG_Grid *pZ)
	: CSG_3DView_Panel(pParent)
{
	m_pGrid		= pGrid;
	m_pZ		= pZ;
	m_pNodes	= NULL;

	Create_Nodes();

	//-----------------------------------------------------
	CSG_Parameter	*pNode, *pNode_1, *pNode_2;

	m_Parameters("BGCOLOR" )->Set_Value((int)SG_COLOR_BLACK);
	m_Parameters("DRAW_BOX")->Set_Value(false);

	//-----------------------------------------------------
	pNode	= m_Parameters("NODE_GENERAL");

	m_Parameters.Add_Value(
		pNode	, "RADIUS"			, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Double, 6371.0, 0.0, true
	);

	m_Parameters.Add_Value(
		pNode	, "Z_SCALE"			, _TL("Exaggeration"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(
		NULL	, "NODE_VIEW"		, _TL("Grid View Settings"),
		_TL("")
	);

	pNode_1	= m_Parameters.Add_Value(
		pNode	, "DRAW_FACES"		, _TL("Draw Faces"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	pNode_2	= m_Parameters.Add_Value(
		pNode_1	, "COLOR_ASRGB"		, _TL("RGB Values"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_Colors(
		pNode_2	, "COLORS"			, _TL("Colours"),
		_TL("")
	);

	m_Parameters.Add_Value(
		pNode_2	, "COLORS_GRAD"		, _TL("Graduated"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Range(
		pNode_2	, "COLORS_RANGE"	, _TL("Value Range"),
		_TL("")
	);

	pNode_2	= m_Parameters.Add_Choice(
		pNode_1	, "SHADING"			, _TL("Shading"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("none"),
			_TL("shading")
		), 1
	);

	m_Parameters.Add_Value(
		pNode_2	, "SHADE_DEC"		, _TL("Light Source Height"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, -90.0, true, 90.0, true
	);

	m_Parameters.Add_Value(
		pNode_2	, "SHADE_AZI"		, _TL("Light Source Direction"),
		_TL(""),
		PARAMETER_TYPE_Double, 315.0, 0.0, true, 360.0, true
	);

	//-----------------------------------------------------
	pNode_1	= m_Parameters.Add_Value(
		pNode	, "DRAW_EDGES"		, _TL("Draw Wire"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_Value(
		pNode_1	, "EDGE_COLOR"		, _TL("Colour"),
		_TL(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(150, 150, 150)
	);

	//-----------------------------------------------------
	m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
		m_pGrid->Get_Mean() - 1.5 * m_pGrid->Get_StdDev(),
		m_pGrid->Get_Mean() + 1.5 * m_pGrid->Get_StdDev()
	);

	Update_Statistics();
}

//---------------------------------------------------------
C3D_Viewer_Globe_Grid_Panel::~C3D_Viewer_Globe_Grid_Panel(void)
{
	if( m_pNodes )
	{
		SG_Free(m_pNodes[0]);
		SG_Free(m_pNodes);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Globe_Grid_Panel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("COLOR_ASRGB") )
	{
		pParameters->Get_Parameter("COLORS"      )->Set_Enabled(pParameter->asBool() == false);
		pParameters->Get_Parameter("COLORS_RANGE")->Set_Enabled(pParameter->asBool() == false);
	}

	if( pParameter->Cmp_Identifier("SHADING") )
	{
		pParameters->Get_Parameter("SHADE_DEC")->Set_Enabled(pParameter->asBool());
		pParameters->Get_Parameter("SHADE_AZI")->Set_Enabled(pParameter->asBool());
	}

	if( pParameter->Cmp_Identifier("DRAW_EDGES") )
	{
		pParameters->Get_Parameter("EDGE_COLOR")->Set_Enabled(pParameter->asBool());
	}

	return( CSG_3DView_Panel::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Globe_Grid_Panel::Create_Nodes(void)
{
	m_pNodes	= (TSG_Point_Z **)SG_Malloc(m_pGrid->Get_NY    () * sizeof(TSG_Point_Z *));
	m_pNodes[0]	= (TSG_Point_Z  *)SG_Malloc(m_pGrid->Get_NCells() * sizeof(TSG_Point_Z  ));

	for(int y=0; y<m_pGrid->Get_NY(); y++)
	{
		m_pNodes[y]	= m_pNodes[0] + y * m_pGrid->Get_NX();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_Globe_Grid_Panel::Update_Statistics(void)
{
	double	Radius	= m_Parameters("RADIUS")->asDouble();
	double	zScale	= m_pZ ? m_Parameters("Z_SCALE")->asDouble() : 0.0;

	m_Data_Min.x = m_Data_Max.x = 0.0;
	m_Data_Min.y = m_Data_Max.y = 0.0;
	m_Data_Min.z = m_Data_Max.z = 0.0;

	for(int y=0; y<m_pGrid->Get_NY(); y++)
	{
		TSG_Point_Z	*pNode	= m_pNodes[y];

		double	wy	= M_DEG_TO_RAD * (m_pGrid->Get_YMin() + y * m_pGrid->Get_Cellsize());
		double	wx	= M_DEG_TO_RAD * (m_pGrid->Get_XMin());

		for(int x=0; x<m_pGrid->Get_NX(); x++, pNode++, wx+=M_DEG_TO_RAD*m_pGrid->Get_Cellsize())
		{
			if( !m_pGrid->is_NoData(x, y) )
			{
				double r	= zScale ? Radius + zScale * m_pZ->asDouble(x, y) : Radius;
				pNode->z	= r * sin(wy);
				double s	= r * cos(wy);
				pNode->x	= s * cos(wx);
				pNode->y	= s * sin(wx);

				if( m_Data_Min.x > pNode->x ) m_Data_Min.x = pNode->x; else if( m_Data_Max.x < pNode->x ) m_Data_Max.x = pNode->x;
				if( m_Data_Min.y > pNode->y ) m_Data_Min.y = pNode->y; else if( m_Data_Max.y < pNode->y ) m_Data_Max.y = pNode->y;
				if( m_Data_Min.z > pNode->z ) m_Data_Min.z = pNode->z; else if( m_Data_Max.z < pNode->z ) m_Data_Max.z = pNode->z;
			}
		}
	}

	//-----------------------------------------------------
	Update_View();
}

//---------------------------------------------------------
void C3D_Viewer_Globe_Grid_Panel::Update_Parent(void)
{
	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_Globe_Grid_Panel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:	CSG_3DView_Panel::On_Key_Down(event);	return;

	case WXK_F1:	m_Parameters("Z_SCALE")->Set_Value(m_Parameters("Z_SCALE")->asDouble() -  0.5);	break;
	case WXK_F2:	m_Parameters("Z_SCALE")->Set_Value(m_Parameters("Z_SCALE")->asDouble() +  0.5);	break;
	}

	//-----------------------------------------------------
	Update_View(true);
	Update_Parent();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Globe_Grid_Panel::Get_Color(double Value)
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
inline bool C3D_Viewer_Globe_Grid_Panel::Get_Node(int x, int y, TSG_Triangle_Node &Node)
{
	if( m_pGrid->is_InGrid(x, y) )
	{
		TSG_Point_Z	p	= m_pNodes[y][x];

		m_Projector.Get_Projection(p);

		Node.x	= p.x;
		Node.y	= p.y;
		Node.z	= p.z;
		Node.c	= m_pGrid->asDouble(x, y);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool C3D_Viewer_Globe_Grid_Panel::On_Draw(void)
{
	//-----------------------------------------------------
	if( m_Parameters("COLORS_RANGE")->asRange()->Get_Min()
	>=  m_Parameters("COLORS_RANGE")->asRange()->Get_Max() )
	{
		m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
			m_pGrid->Get_Mean() - 1.5 * m_pGrid->Get_StdDev(),
			m_pGrid->Get_Mean() + 1.5 * m_pGrid->Get_StdDev()
		);
	}

	bool	bValueAsColor	= m_Parameters("COLOR_ASRGB")->asBool();

	m_Colors		= *m_Parameters("COLORS")->asColors();
	m_Color_bGrad	= m_Parameters("COLORS_GRAD")->asBool();
	m_Color_Min		= m_Parameters("COLORS_RANGE")->asRange()->Get_Min();
	m_Color_Scale	= m_Colors.Get_Count() / (m_Parameters("COLORS_RANGE")->asRange()->Get_Max() - m_Color_Min);

	//-----------------------------------------------------
	if( m_Parameters("DRAW_FACES")->asBool() )	// Faces
	{
		int		Shading		= m_Parameters("SHADING"  )->asInt   ();
		double	Shade_Dec	= m_Parameters("SHADE_DEC")->asDouble() * -M_DEG_TO_RAD;
		double	Shade_Azi	= m_Parameters("SHADE_AZI")->asDouble() *  M_DEG_TO_RAD;

		#pragma omp parallel for
		for(int y=1; y<m_pGrid->Get_NY(); y++)
		{
			for(int x=1; x<m_pGrid->Get_NX(); x++)
			{
				TSG_Triangle_Node	p[3];

				if( Get_Node(x - 1, y - 1, p[0])
				&&  Get_Node(x    , y    , p[1]) )
				{
					if( Get_Node(x, y - 1, p[2]) )
					{
						if( Shading ) Draw_Triangle(p, bValueAsColor, Shade_Dec, Shade_Azi); else Draw_Triangle(p, bValueAsColor);
					}

					if( Get_Node(x - 1, y, p[2]) )
					{
						if( Shading ) Draw_Triangle(p, bValueAsColor, Shade_Dec, Shade_Azi); else Draw_Triangle(p, bValueAsColor);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_Parameters("DRAW_EDGES")->asBool() )	// Edges
	{
		int	Color	= m_Parameters("EDGE_COLOR")->asColor();

		#pragma omp parallel for
		for(int y=1; y<m_pGrid->Get_NY(); y++)
		{
			for(int x=1; x<m_pGrid->Get_NX(); x++)
			{
				TSG_Triangle_Node	p[2];

				if( Get_Node(x - 1, y - 1, p[0])
				&&  Get_Node(x    , y    , p[1]) )
				{
					if( !bValueAsColor )
					{
						p[0].c	= Get_Color(p[0].c);
						p[1].c	= Get_Color(p[1].c);
					}

					Draw_Line(p[0].x, p[0].y, p[0].z, p[1].x, p[1].y, p[1].z, p[0].c, p[1].c);

					if( Get_Node(x, y - 1, p[1]) )
					{
						if( !bValueAsColor )
						{
							p[1].c	= Get_Color(p[1].c);
						}

						Draw_Line(p[0].x, p[0].y, p[0].z, p[1].x, p[1].y, p[1].z, p[0].c, p[1].c);
						Draw_Line(p[0].x, p[0].y, p[0].z, p[1].x, p[1].y, p[1].z, p[0].c, p[1].c);
					}

					if( Get_Node(x - 1, y, p[1]) )
					{
						if( !bValueAsColor )
						{
							p[1].c	= Get_Color(p[1].c);
						}

						Draw_Line(p[0].x, p[0].y, p[0].z, p[1].x, p[1].y, p[1].z, p[0].c, p[1].c);
						Draw_Line(p[0].x, p[0].y, p[0].z, p[1].x, p[1].y, p[1].z, p[0].c, p[1].c);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( !m_Parameters("DRAW_FACES")->asBool() && !m_Parameters("DRAW_EDGES")->asBool() )	// Nodes
	{
		int	Color	= m_Parameters("EDGE_COLOR")->asColor();

		for(int y=0; y<m_pGrid->Get_NY(); y++)
		{
			for(int x=0; x<m_pGrid->Get_NX(); x++)
			{
				TSG_Triangle_Node	p;

				Get_Node(x, y, p);

				Draw_Point(p.x, p.y, p.z, bValueAsColor ? p.c : Get_Color(p.c), 2);
			}
		}
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
class C3D_Viewer_Globe_Grid_Dialog : public CSG_3DView_Dialog
{
public:
	C3D_Viewer_Globe_Grid_Dialog(CSG_Grid *pGrid, CSG_Grid *pZ);

	virtual void				Update_Controls			(void);


protected:

	wxCheckBox					*m_pFaces, *m_pEdges;


	virtual void				On_Update_Control		(wxCommandEvent &event);


private:

	DECLARE_EVENT_TABLE()

};

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_Globe_Grid_Dialog, CSG_3DView_Dialog)
	EVT_CHECKBOX	(wxID_ANY	, C3D_Viewer_Globe_Grid_Dialog::On_Update_Control)
END_EVENT_TABLE()

//---------------------------------------------------------
C3D_Viewer_Globe_Grid_Dialog::C3D_Viewer_Globe_Grid_Dialog(CSG_Grid *pGrid, CSG_Grid *pZ)
	: CSG_3DView_Dialog(_TL("Globe Viewer for Grids"))
{
	Create(new C3D_Viewer_Globe_Grid_Panel(this, pGrid, pZ));

	Add_Spacer();
	m_pFaces		= Add_CheckBox(_TL("Faces"), m_pPanel->m_Parameters("DRAW_FACES")->asBool());
	m_pEdges		= Add_CheckBox(_TL("Edges"), m_pPanel->m_Parameters("DRAW_EDGES")->asBool());
}

//---------------------------------------------------------
void C3D_Viewer_Globe_Grid_Dialog::On_Update_Control(wxCommandEvent &event)
{
	CHECKBOX_UPDATE(m_pFaces, "DRAW_FACES");
	CHECKBOX_UPDATE(m_pEdges, "DRAW_EDGES");

	CSG_3DView_Dialog::On_Update_Control(event);
}

//---------------------------------------------------------
void C3D_Viewer_Globe_Grid_Dialog::Update_Controls(void)
{
	m_pFaces->SetValue(m_pPanel->m_Parameters("DRAW_FACES")->asBool());
	m_pEdges->SetValue(m_pPanel->m_Parameters("DRAW_EDGES")->asBool());

	CSG_3DView_Dialog::Update_Controls();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Globe_Grid::C3D_Viewer_Globe_Grid(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Globe Viewer for Grids"));

	Set_Author		("O. Conrad (c) 2014");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "Z"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Globe_Grid::On_Execute(void)
{
	C3D_Viewer_Globe_Grid_Dialog	dlg(
		Parameters("GRID")->asGrid(),
		Parameters("Z"   )->asGrid()
	);

	dlg.ShowModal();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
