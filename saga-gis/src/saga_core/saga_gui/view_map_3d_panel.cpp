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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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
BEGIN_EVENT_TABLE(CView_Map_3DPanel, CSG_3DView_Panel)
	EVT_KEY_DOWN	(CView_Map_3DPanel::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CView_Map_3DPanel::CView_Map_3DPanel(wxWindow *pParent, class CWKSP_Map *pMap)
	: CSG_3DView_Panel(pParent, &m_Map)
{
	m_pDEM	= NULL;
	m_pMap	= pMap;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CView_Map_3DPanel::Update_Statistics(void)
{
	if( !m_pDEM || !m_pMap )
	{
		return;
	}

	CSG_Rect	r(m_pDEM->Get_Extent());

	if( !r.Intersect(m_pMap->Get_Extent()) )
	{
		return;
	}

	m_pMap->SaveAs_Image_To_Grid(m_Map, 400);

	double	c	= (r.Get_XRange() > r.Get_YRange() ? r.Get_XRange() : r.Get_YRange()) / 100.0; if( c < m_pDEM->Get_Cellsize() ) c = m_pDEM->Get_Cellsize();

	m_DEM.Create(CSG_Grid_System(c, r), SG_DATATYPE_Float);

	for(int y=0; y<m_DEM.Get_NY(); y++)
	{
		int	wy	= m_DEM.Get_YMin() + m_DEM.Get_Cellsize() * y;

		for(int x=0; x<m_DEM.Get_NX(); x++)
		{
			double	z;

			if( m_pDEM->Get_Value(m_DEM.Get_XMin() + c * x, wy, z) )
			{
				m_DEM.Set_Value(x, y, z);
			}
			else
			{
				m_DEM.Set_NoData(x, y);
			}
		}
	}

	m_Data_Min.x	= m_DEM.Get_XMin();
	m_Data_Max.x	= m_DEM.Get_XMax();

	m_Data_Min.y	= m_DEM.Get_YMin();
	m_Data_Max.y	= m_DEM.Get_YMax();

	m_Data_Min.z	= m_DEM.Get_ZMin();
	m_Data_Max.z	= m_DEM.Get_ZMax();

	Update_View();
}

//---------------------------------------------------------
bool CView_Map_3DPanel::Set_DEM(CSG_Grid *pDEM)
{
	if( m_pDEM != pDEM )
	{
		m_pDEM	= pDEM;
	}

	return( m_pDEM != NULL );
}

//---------------------------------------------------------
void CView_Map_3DPanel::Update_Parent(void)
{
	((CVIEW_Map_3D *)GetParent())->Update_StatusBar();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CView_Map_3DPanel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:	CSG_3DView_Panel::On_Key_Down(event);	return;

	case WXK_F1:	m_zScale	-=  0.5;	break;
	case WXK_F2:	m_zScale	+=  0.5;	break;
	}

	//-----------------------------------------------------
	Update_View();
	Update_Parent();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CView_Map_3DPanel::On_Before_Draw(void)
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
inline bool CView_Map_3DPanel::Get_Node(int x, int y, TSG_Triangle_Node &Node)
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		Node.x	= Node.c = m_pDEM->Get_System().Get_xGrid_to_World(x);
		Node.y	= Node.d = m_pDEM->Get_System().Get_yGrid_to_World(y);
		Node.z	= m_pDEM->asDouble(x, y);

		m_Projector.Get_Projection(Node.x, Node.y, Node.z);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CView_Map_3DPanel::On_Draw(void)
{
	if( !m_pDEM )
	{
		return( false );
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=1; y<m_pDEM->Get_NY(); y++)
	{
		for(int x=1; x<m_pDEM->Get_NX(); x++)
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
