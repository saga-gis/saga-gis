
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
//                  VIEW_Map_Control.h                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_Control_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_Control_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/panel.h>
#include <wx/bitmap.h>

#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	MAP_MODE_NONE	= 0,
	MAP_MODE_SELECT,
	MAP_MODE_DISTANCE,
	MAP_MODE_ZOOM,
	MAP_MODE_PAN,
	MAP_MODE_PAN_DOWN
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Map_Control : public wxPanel
{
public:
	CVIEW_Map_Control(class CVIEW_Map *pParent, class CWKSP_Map *pMap);
	virtual ~CVIEW_Map_Control(void);

	void						On_Paint			(wxPaintEvent &event);
	void						On_Size				(wxSizeEvent  &event);
	void						On_Key_Down			(wxKeyEvent   &event);

	void						On_Mouse_Wheel		(wxMouseEvent &event);
	void						On_Mouse_Motion		(wxMouseEvent &event);
	void						On_Mouse_LDown		(wxMouseEvent &event);
	void						On_Mouse_LUp		(wxMouseEvent &event);
	void						On_Mouse_LDClick	(wxMouseEvent &event);
	void						On_Mouse_RDown		(wxMouseEvent &event);
	void						On_Mouse_RUp		(wxMouseEvent &event);
	void						On_Mouse_RDClick	(wxMouseEvent &event);

	bool						Set_Mode			(int Mode);
	int							Get_Mode			(void)	{	return( m_Mode );	}

	void						Refresh_Map			(void);


private:

	int							m_Mode, m_Drag_Mode;

	double						m_Distance, m_Distance_Move;

	CSG_Points					m_Distance_Pts;

	wxPoint						m_Mouse_Down, m_Mouse_Move;

	wxBitmap					m_Bitmap;

	class CVIEW_Map				*m_pParent;

	class CWKSP_Map				*m_pMap;


	void						_Set_StatusBar		(CSG_Point ptWorld);

	wxRect						_Get_Client			(void);
	wxPoint						_Get_Client			(TSG_Point Point);
	CSG_Point					_Get_World			(wxPoint ptClient);
	double						_Get_World			(double xClient);

	void						_Draw_Inverse		(wxPoint ptA, wxPoint ptB);
	void						_Draw_Inverse		(wxPoint ptA, wxPoint ptB_Old, wxPoint ptB_New);

	bool						_Zoom				(wxPoint  A, wxPoint B);
	bool						_Zoom				(CSG_Point ptCenter, bool bZoomIn);
	bool						_Pan				(wxPoint  A, wxPoint B);
	bool						_Move				(wxPoint &A, wxPoint B);
	bool						_Move				(wxPoint ptMove);

	void						_Distance_Reset		(void);
	void						_Distance_Add		(wxPoint Point);
	void						_Distance_Draw		(wxDC &dc);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_Control_H
