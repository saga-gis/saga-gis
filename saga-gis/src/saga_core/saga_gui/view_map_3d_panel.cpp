/**********************************************************
 * Version $Id: view_map_3d_panel.cpp 2064 2014-03-21 13:20:57Z oconrad $
 *********************************************************/

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	EVT_KEY_DOWN	(CVIEW_Map_3DPanel::On_Key_Down)
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
	m_pDEM		= NULL;
	m_pMap		= pMap;

	m_DEM_Res	= 100;
	m_Map_Res	= 400;

	m_Parameters("DRAW_BOX")->Set_Value(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Map_3DPanel::Update_Statistics(void)
{
	//-----------------------------------------------------
	CSG_Rect	r(m_pDEM->Get_Extent());

	if( !m_pMap || !r.Intersect(m_pMap->Get_Extent()) )
	{
		m_DEM.Destroy();

		return;
	}

	//-----------------------------------------------------
	double	Cellsize	= (r.Get_XRange() > r.Get_YRange() ? r.Get_XRange() : r.Get_YRange()) / m_DEM_Res;
	
	if( Cellsize < m_pDEM->Get_Cellsize() )
		Cellsize = m_pDEM->Get_Cellsize();

	m_DEM.Create(CSG_Grid_System(Cellsize, r), SG_DATATYPE_Float);
	m_DEM.Set_NoData_Value(m_pDEM->Get_NoData_Value());

	for(int y=0; y<m_DEM.Get_NY(); y++)
	{
		double	wy	= m_DEM.Get_YMin() + y * m_DEM.Get_Cellsize();

		for(int x=0; x<m_DEM.Get_NX(); x++)
		{
			double	z, wx	= m_DEM.Get_XMin() + x * m_DEM.Get_Cellsize();

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

	m_Data_Min.x	= m_DEM.Get_XMin();	m_Data_Max.x	= m_DEM.Get_XMax();
	m_Data_Min.y	= m_DEM.Get_YMin();	m_Data_Max.y	= m_DEM.Get_YMax();
	m_Data_Min.z	= m_DEM.Get_Min ();	m_Data_Max.z	= m_DEM.Get_Max ();

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
	if( m_pDEM == pDEM && m_DEM_Res == DEM_Res && m_Map_Res == Map_Res )
	{
		return( false );	// nothing to do
	}

	//-----------------------------------------------------
	m_pDEM	= pDEM;

	if( DEM_Res >= 2 )	m_DEM_Res	= DEM_Res;
	if( Map_Res >= 2 )	m_Map_Res	= Map_Res;

	Update_Statistics();

	return( true );
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
void CVIEW_Map_3DPanel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:	CSG_3DView_Panel::On_Key_Down(event);	return;

	case WXK_F1:	m_zScale	-=  0.5;	break;
	case WXK_F2:	m_zScale	+=  0.5;	break;

	case WXK_F5:	Inc_DEM_Res(-25);		break;
	case WXK_F6:	Inc_DEM_Res( 25);		break;

	case WXK_F7:	Inc_DEM_Res(-25);		break;
	case WXK_F8:	Inc_DEM_Res( 25);		break;
	}

	//-----------------------------------------------------
	Update_View();
	Update_Parent();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Map_3DPanel::On_Before_Draw(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Projector.Set_zScaling(m_Projector.Get_xScaling() * m_zScale);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CVIEW_Map_3DPanel::Get_Node(int x, int y, TSG_Triangle_Node &Node)
{
	if( m_DEM.is_InGrid(x, y) )
	{
		Node.x	= Node.c = m_DEM.Get_System().Get_xGrid_to_World(x);
		Node.y	= Node.d = m_DEM.Get_System().Get_yGrid_to_World(y);
		Node.z	= m_DEM.asDouble(x, y);

		m_Projector.Get_Projection(Node.x, Node.y, Node.z);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Map_3DPanel::On_Draw(void)
{
	if( !m_DEM.is_Valid() )
	{
		return( false );
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=1; y<m_DEM.Get_NY(); y++)
	{
		for(int x=1; x<m_DEM.Get_NX(); x++)
		{
			TSG_Triangle_Node	p[3];

			if( Get_Node(x - 1, y - 1, p[0])
			&&  Get_Node(x    , y    , p[1]) )
			{
				if( Get_Node(x, y - 1, p[2]) )
				{
					Draw_Triangle(p, true);
				}

				if( Get_Node(x - 1, y, p[2]) )
				{
					Draw_Triangle(p, true);
				}
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
