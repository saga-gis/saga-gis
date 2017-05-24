/**********************************************************
 * Version $Id: view_map_3d_panel.h 1921 2014-01-09 10:24:11Z oconrad $
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
//                 view_map_3d_panel.h                   //
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
//                Germany                                //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__view_map_3d_panel_H
#define _HEADER_INCLUDED__SAGA_GUI__view_map_3d_panel_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_gdi/3d_view.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Map_3DPanel : public ::CSG_3DView_Panel
{
public:
	CVIEW_Map_3DPanel(wxWindow *pParent, class CWKSP_Map *pMap);

	double						m_zScale;

	bool						Set_Options				(CSG_Grid *pDEM, int DEM_Res, int Map_Res);

	int							Get_DEM_Res				(void)	{	return( m_DEM_Res );	}
	bool						Inc_DEM_Res				(int Step);

	int							Get_Map_Res				(void)	{	return( m_Map_Res );	}
	bool						Inc_Map_Res				(int Step);


protected:

	virtual void				Update_Statistics		(void);
	virtual void				Update_Parent			(void);

	virtual void				On_Key_Down				(wxKeyEvent   &event);

	virtual bool				On_Before_Draw			(void);
	virtual bool				On_Draw					(void);


private:

	bool						m_Color_bGrad;

	int							m_DEM_Res, m_Map_Res;

	double						m_Color_Min, m_Color_Scale;

	CSG_Colors					m_Colors;

	CSG_Grid					*m_pDEM, m_DEM, m_Map;

	class CWKSP_Map				*m_pMap;


	bool						Get_Node				(int x, int y, TSG_Triangle_Node &Node);


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__view_map_3d_panel_H
