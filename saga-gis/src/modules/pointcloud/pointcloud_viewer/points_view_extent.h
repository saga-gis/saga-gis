/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     SAGA_GUI_API                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  points_view_extent.h                 //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    SAGA User Group Association            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#ifndef HEADER_INCLUDED__points_view_extent_H
#define HEADER_INCLUDED__points_view_extent_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_gdi/saga_gdi.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPoints_View_Extent : public wxPanel
{
public:
	CPoints_View_Extent(wxWindow *pParent, CSG_PointCloud *pPoints, CSG_Parameters &Settings, wxSize Size);

	int							m_cField;

	void						Update_View				(void);

	void						On_Size					(wxSizeEvent  &event);
	void						On_EraseBackGround		(wxEraseEvent &event);
	void						On_Paint				(wxPaintEvent &event);
	void						On_Mouse_LDown			(wxMouseEvent &event);
	void						On_Mouse_LUp			(wxMouseEvent &event);
	void						On_Mouse_RDown			(wxMouseEvent &event);
	void						On_Mouse_RUp			(wxMouseEvent &event);
	void						On_Mouse_Motion			(wxMouseEvent &event);

	TSG_Rect					Get_Extent				(void);


private:

	CSG_Rect					m_Extent;

	CSG_Matrix					m_Image_Value, m_Image_Count;

	CSG_PointCloud				*m_pPoints;

	CSG_Parameters				*m_pSettings;

	wxPoint						m_Mouse_Down, m_Mouse_Move;

	wxRect						m_Select;

	wxImage						m_Image;


	bool						_Draw_Image				(void);
	void						_Draw_Pixel				(int x, int y, int color);
	void						_Draw_Inverse			(wxPoint pa, wxPoint pb);


	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__points_view_extent_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
