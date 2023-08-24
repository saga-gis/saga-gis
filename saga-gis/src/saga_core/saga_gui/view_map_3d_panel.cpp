
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 view_map_3d_panel.cpp                 //
//                                                       //
//          Copyright (C) 2014 by Olaf Conrad            //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_map.h"

#include "view_map_3d.h"
#include "view_map_3d_panel.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Map_3DPanel, CSG_3DView_Panel)
	EVT_KEY_DOWN(CVIEW_Map_3DPanel::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Map_3DPanel::CVIEW_Map_3DPanel(wxWindow *pParent, class CWKSP_Map *pMap)
	: CSG_3DView_Panel(pParent, &m_Map)
{
	m_pDEM    = NULL;
	m_pMap    = pMap;

	m_DEM_Res =  100;
	m_Map_Res = 1000;

	m_Parameters.Add_Grid("GENERAL" , "DEM"    , _TL("Elevation" ), _TL(""), PARAMETER_INPUT);
	m_Parameters.Add_Int ("DEM"     , "DEM_RES", _TL("Resolution"), _TL(""), m_DEM_Res, 2, true);

	m_Parameters.Add_Int ("MAP"     , "MAP_RES", _TL("Resolution"), _TL(""), m_Map_Res, 2, true);

	m_Parameters.Add_Bool("3D_VIEW" , "SIDES"  , _TL("Sides"     ), _TL(""), false);

	m_Parameters.Set_Enabled("MAP_DRAPE", false);

	m_Parameters["Z_SCALE"   ].Set_Value( 3.  );

	m_Parameters["ROTATION_X"].Set_Value( 55. );
	m_Parameters["ROTATION_Y"].Set_Value(  0. );
	m_Parameters["ROTATION_Z"].Set_Value(-45. );

	m_Parameters["SHIFT_X"   ].Set_Value(  0. );
	m_Parameters["SHIFT_Y"   ].Set_Value( -0.1);
	m_Parameters["SHIFT_Z"   ].Set_Value( -0.4);

	m_Parameters["BOX"       ].Set_Value(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Map_3DPanel::Update_Parameters(bool bSave)
{
	CSG_3DView_Panel::Update_Parameters(bSave);

	if( bSave )
	{
		m_Parameters["DEM_RES"].Set_Value(m_DEM_Res);
		m_Parameters["MAP_RES"].Set_Value(m_Map_Res);
	}
	else
	{
		Set_Options(
			m_Parameters["DEM"    ].asGrid(),
			m_Parameters["DEM_RES"].asInt (),
			m_Parameters["MAP_RES"].asInt ()
		);
	}

	Update_Parent();

	return( true );
}

//---------------------------------------------------------
void CVIEW_Map_3DPanel::Update_Statistics(void)
{
	CSG_Rect r(m_pDEM->Get_Extent());

	if( !m_pMap || !r.Intersect(m_pMap->Get_Extent()) )
	{
		m_DEM.Destroy();

		return;
	}

	//-----------------------------------------------------
	double Cellsize = (r.Get_XRange() > r.Get_YRange() ? r.Get_XRange() : r.Get_YRange()) / m_DEM_Res;
	
	if( Cellsize < m_pDEM->Get_Cellsize() )
	{
		Cellsize = m_pDEM->Get_Cellsize();
	}

	m_DEM.Create(CSG_Grid_System(Cellsize, r), SG_DATATYPE_Float);
	m_DEM.Set_NoData_Value(m_pDEM->Get_NoData_Value());

	for(int y=0; y<m_DEM.Get_NY(); y++)
	{
		double wy = m_DEM.Get_YMin() + y * m_DEM.Get_Cellsize();

		for(int x=0; x<m_DEM.Get_NX(); x++)
		{
			double z, wx = m_DEM.Get_XMin() + x * m_DEM.Get_Cellsize();

			if( m_pDEM->Get_Value(wx, wy, z) )
			{
				m_DEM.Set_Value(x, y, z);
			}
			else
			{
				m_DEM.Set_NoData(x, y);
			}
		}
	}

	m_Data_Min.x = m_DEM.Get_XMin(); m_Data_Max.x = m_DEM.Get_XMax();
	m_Data_Min.y = m_DEM.Get_YMin(); m_Data_Max.y = m_DEM.Get_YMax();
	m_Data_Min.z = m_DEM.Get_Min (); m_Data_Max.z = m_DEM.Get_Max ();

	m_pMap->SaveAs_Image_To_Grid(m_Map, m_Map_Res);

	Update_View();
}

//---------------------------------------------------------
void CVIEW_Map_3DPanel::Update_Parent(void)
{
	((CVIEW_Map_3D *)GetParent())->Update_StatusBar();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Map_3DPanel::Set_Options(CSG_Grid *pDEM, int DEM_Res, int Map_Res)
{
	if( !SG_Get_Data_Manager().Exists(pDEM) ) { pDEM = NULL; }

	if( DEM_Res < 2 ) { DEM_Res = 2; }
	if( Map_Res < 2 ) { Map_Res = 2; }

	if( m_pDEM != pDEM || m_DEM_Res != DEM_Res || m_Map_Res != Map_Res )
	{
		m_pDEM = pDEM; m_DEM_Res = DEM_Res; m_Map_Res = Map_Res;

		Update_Statistics();

		return( true );
	}

	return( false ); // nothing to do
}

//---------------------------------------------------------
bool CVIEW_Map_3DPanel::Inc_DEM_Res(int Step)
{
	return( m_DEM_Res + Step >= 2 ? Set_Options(m_pDEM, m_DEM_Res + Step, m_Map_Res) : false );
}

//---------------------------------------------------------
bool CVIEW_Map_3DPanel::Inc_Map_Res(int Step)
{
	return( m_Map_Res + Step >= 2 ? Set_Options(m_pDEM, m_DEM_Res, m_Map_Res + Step) : false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CVIEW_Map_3DPanel::Get_Usage(void)
{
	CSG_Table Shortcuts(CSG_3DView_Panel::Get_Shortcuts());

	#define ADD_SHORTCUT(KEY, CMD) { CSG_Table_Record &r = *Shortcuts.Add_Record(); r.Set_Value(0, KEY); r.Set_Value(1, CMD); }

	ADD_SHORTCUT("F3", _TL("Decrease Elevation Model Resolution"));
	ADD_SHORTCUT("F4", _TL("Increase Elevation Model Resolution"));

	ADD_SHORTCUT("F5", _TL("Decrease Map Resolution"  ));
	ADD_SHORTCUT("F6", _TL("Increase Map Resolution"  ));

	return( CSG_3DView_Panel::Get_Usage(Shortcuts) );
}

//---------------------------------------------------------
void CVIEW_Map_3DPanel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default     : CSG_3DView_Panel::On_Key_Down(event);	return;

	case WXK_F3 : Inc_DEM_Res(-25); break;
	case WXK_F4 : Inc_DEM_Res( 25); break;

	case WXK_F5 : Inc_Map_Res(-25); break;
	case WXK_F6 : Inc_Map_Res( 25); break;

	case 'S'    : Parameter_Value_Toggle("SIDES"); break;
	}

	//-----------------------------------------------------
	Update_View(); Update_Parent();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CVIEW_Map_3DPanel::_Get_Node(int x, int y, TSG_Triangle_Node &Node, bool bProject)
{
	if( m_DEM.is_InGrid(x, y) )
	{
		Node.x = Node.c = m_DEM.Get_System().Get_xGrid_to_World(x);
		Node.y = Node.d = m_DEM.Get_System().Get_yGrid_to_World(y);
		Node.z = m_DEM.asDouble(x, y);

		if( bProject )
		{
			m_Projector.Get_Projection(Node.x, Node.y, Node.z);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3DPanel::_Draw_Side(int xa, int xb, int ya, int yb, double zMin, int Color, const CSG_Vector &LightSource)
{
	TSG_Triangle_Node p[3], pa, pb;

	if( _Get_Node(xa, ya, pa, false) && _Get_Node(xb, yb, pb, false) )
	{
		pa.c = pb.c = Color;

		p[0] = pa; p[1] = p[2] = pb; p[1].z = zMin;

		for(int i=0; i<3; i++) { m_Projector.Get_Projection(p[i].x, p[i].y, p[i].z); } Draw_Triangle(p, true, LightSource, 1);

		p[0] = p[1] = pa; p[2] = pb; p[1].z = p[2].z = zMin;

		for(int i=0; i<3; i++) { m_Projector.Get_Projection(p[i].x, p[i].y, p[i].z); } Draw_Triangle(p, true, LightSource, 1);
	}
}

//---------------------------------------------------------
bool CVIEW_Map_3DPanel::On_Draw(void)
{
	if( m_DEM.is_Valid() )
	{
		#pragma omp parallel for
		for(int y=1; y<m_DEM.Get_NY(); y++) for(int x=1; x<m_DEM.Get_NX(); x++)
		{
			TSG_Triangle_Node p[3];

			if( _Get_Node(x - 1, y - 1, p[0])
			&&  _Get_Node(x    , y    , p[1]) )
			{
				if( _Get_Node(x, y - 1, p[2]) ) { Draw_Triangle(p, true); }
				if( _Get_Node(x - 1, y, p[2]) ) { Draw_Triangle(p, true); }
			}
		}

		//-------------------------------------------------
		if( m_Parameters["SIDES"].asBool() )
		{
			CSG_Vector LightSource(3); double decline = 45. * -M_DEG_TO_RAD, azimuth = 90. *  M_DEG_TO_RAD;
			LightSource[0] = sin(decline) * cos(azimuth);
			LightSource[1] = sin(decline) * sin(azimuth);
			LightSource[2] = cos(decline);

			Set_Drape(NULL); int Color = SG_GET_RGB(192, 192, 192); double zMin = m_DEM.Get_Min();// - 0.25 * m_DEM.Get_Range();

			TSG_Triangle_Node p[3];

			p[0].x = m_DEM.Get_XMin(); p[0].y = m_DEM.Get_YMin();
			p[1].x = m_DEM.Get_XMax(); p[1].y = m_DEM.Get_YMin();
			p[2].x = m_DEM.Get_XMax(); p[2].y = m_DEM.Get_YMax();
			p[0].z = p[1].z = p[2].z = zMin;
			p[0].c = p[1].c = p[2].c = Color;
			for(int i=0; i<3; i++) { m_Projector.Get_Projection(p[i].x, p[i].y, p[i].z); }
			Draw_Triangle(p, true, LightSource, 1);

			p[0].x = m_DEM.Get_XMin(); p[0].y = m_DEM.Get_YMin();
			p[1].x = m_DEM.Get_XMax(); p[1].y = m_DEM.Get_YMax();
			p[2].x = m_DEM.Get_XMin(); p[2].y = m_DEM.Get_YMax();
			p[0].z = p[1].z = p[2].z = zMin;
			p[0].c = p[1].c = p[2].c = Color;
			for(int i=0; i<3; i++) { m_Projector.Get_Projection(p[i].x, p[i].y, p[i].z); }
			Draw_Triangle(p, true, LightSource, 1);

			for(int i=0; i<2; i++)
			{
				for(int y=1; y<m_DEM.Get_NY(); y++)
				{
					int x = i ? 0 : m_DEM.Get_NX() - 1;

					_Draw_Side(x, x, y - 1, y, zMin, Color, LightSource);
				}

				for(int x=1; x<m_DEM.Get_NX(); x++)
				{
					int y = i ? 0 : m_DEM.Get_NY() - 1;

					_Draw_Side(x - 1, x, y, y, zMin, Color, LightSource);
				}
			}

			Set_Drape(&m_Map);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
