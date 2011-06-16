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
//                 points_view_control.h                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#ifndef HEADER_INCLUDED__points_view_control_H
#define HEADER_INCLUDED__points_view_control_H


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
enum
{
	COLOR_MODE_RGB,
	COLOR_MODE_RED,
	COLOR_MODE_BLUE
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPoints_View_Control : public wxPanel
{
public:
	CPoints_View_Control(wxWindow *pParent, CSG_PointCloud *pPoints, CSG_Parameters &Settings);
	virtual ~CPoints_View_Control(void);

	bool						m_bCentral, m_bStereo, m_bScale;

	int							m_zField, m_cField;

	double						m_xRotate, m_yRotate, m_zRotate, m_xShift, m_yShift, m_zShift, m_dCentral, m_Detail;

	void						Update_View				(void);
	void						Update_Extent			(CSG_Rect Extent);

	void						On_Size					(wxSizeEvent  &event);
	void						On_Paint				(wxPaintEvent &event);
	void						On_Key_Down				(wxKeyEvent   &event);
	void						On_Mouse_LDown			(wxMouseEvent &event);
	void						On_Mouse_LUp			(wxMouseEvent &event);
	void						On_Mouse_RDown			(wxMouseEvent &event);
	void						On_Mouse_RUp			(wxMouseEvent &event);
	void						On_Mouse_MDown			(wxMouseEvent &event);
	void						On_Mouse_MUp			(wxMouseEvent &event);
	void						On_Mouse_Motion			(wxMouseEvent &event);
	void						On_Mouse_Wheel			(wxMouseEvent &event);


private:

	bool						m_bColorAsRGB, m_bDim;

	int							m_nSelection, *m_pSelection, m_Color_Mode, m_Size_Def, m_BGColor;

	double						m_xDown, m_yDown, m_cMin, m_cScale, m_Size_Scale, m_Dim_A, m_Dim_B, m_zMin, m_zMax;

	double						r_sin_x, r_sin_y, r_sin_z, r_cos_x, r_cos_y, r_cos_z, r_xc, r_yc, r_zc, r_Scale, r_Scale_z;

	CSG_Rect					m_Extent;

	CSG_Matrix					m_Image_zMax;

	CSG_Simple_Statistics		m_zStats, m_cStats;

	CSG_Parameters				*m_pSettings;

	CSG_Colors					*m_pColors;

	CSG_PointCloud				*m_pPoints;

	wxPoint						m_Mouse_Down;

	wxImage						m_Image;


	void						_Set_Size				(void);

	bool						_Draw_Image				(void);
	void						_Draw_Background		(void);
	void						_Draw_Point				(int iPoint);
	void						_Draw_Point				(int x, int y, double z, int color, int Size);
	void						_Draw_Pixel				(int x, int y, double z, int color);

	TSG_Point_Z					_Get_Projection			(TSG_Point_Z p);


	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__points_view_control_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
