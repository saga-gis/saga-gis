
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
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

	void						On_Mouse_LDown		(wxMouseEvent &event);
	void						On_Mouse_LUp		(wxMouseEvent &event);
	void						On_Mouse_LDClick	(wxMouseEvent &event);
	void						On_Mouse_RDown		(wxMouseEvent &event);
	void						On_Mouse_RUp		(wxMouseEvent &event);
	void						On_Mouse_RDClick	(wxMouseEvent &event);
	void						On_Mouse_MDown		(wxMouseEvent &event);
	void						On_Mouse_MUp		(wxMouseEvent &event);
	void						On_Mouse_Motion		(wxMouseEvent &event);
	void						On_Mouse_Wheel		(wxMouseEvent &event);

	void						On_Mouse_Lost		(wxMouseCaptureLostEvent &event);

	void						Set_CrossHair		(const TSG_Point &Point);
	void						Set_CrossHair_Off	(void);

	bool						Set_Mode			(int Mode);
	int							Get_Mode			(void)	{	return( m_Mode );	}

	void						Refresh_Map			(void);


private:

	class CMeasure
	{
	public:
		CMeasure(void) {}

		void			Reset			(void)			{ Points.Clear(); }

		int				Count			(void)	const	{ return( Points.Get_Count() ); }
		TSG_Point &		operator []		(int i)			{ return( Points[i] ); }

		double			Get				(const CSG_Point &Point)
		{
			return( Count() < 1 ? 0. : Length + SG_Get_Distance(Point, Points[Count() - 1]) );
		}

		void			Add				(const CSG_Point &Point)
		{
			Points.Add(Point);

			if( Points.Get_Count() < 2 )
			{
				Length  = 0.;
			}
			else
			{
				Length += SG_Get_Distance(Points[Count() - 2], Points[Count() - 1]);
			}
		}

	private:

		double					Length;

		CSG_Points				Points;

	};
	

private:

	int							m_Mode, m_Mode_Previous, m_Drag_Mode;

	wxPoint						m_Mouse_Down, m_Mouse_Move, m_CrossHair;
	
	wxBitmap					m_Bitmap;

	CMeasure					m_Measure;

	class CVIEW_Map				*m_pParent;

	class CWKSP_Map				*m_pMap;

	int 						m_Mouse_Wheel_Accumulator = 0;


	bool						_Update_Bitmap_Size	(void);

	void						_Set_StatusBar		(const TSG_Point &Point);

	wxPoint						_Get_World2Client	(const TSG_Point &Point);
	CSG_Point					_Get_Client2World	(const   wxPoint &Point, bool bToActive = false);
	double						_Get_Client2World	(double Length         , bool bToActive = false);

	bool						_Check_Interactive	(bool bProjection = true);

	void						_Draw_CrossHair		(wxDC &dc);
	void						_Draw_Measure		(wxDC &dc);
	void						_Draw_Drag			(wxDC &dc);
	void						_Draw_Pan			(wxDC &dc);
	void						_Draw_Edit_Moves	(wxDC &dc);

	bool						_Zoom				(const wxPoint &A, const wxPoint &B, bool bZoomIn);
	bool						_Zoom				(const wxPoint &A                  , bool bZoomIn);
	bool						_Zoom				(                                    bool bZoomIn);

	bool						_Move				(      wxPoint &A, const wxPoint &B);
	bool						_Move				(const wxPoint &A);


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_Control_H
