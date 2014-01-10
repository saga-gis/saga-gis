/**********************************************************
 * Version $Id: 3dshapes_view_control.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    3dshapes_viewer                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                3dshapes_view_control.h                //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#ifndef HEADER_INCLUDED__3dshapes_view_control_H
#define HEADER_INCLUDED__3dshapes_view_control_H


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

//---------------------------------------------------------
typedef struct SNode
{
	double	x, y, z, c;
}
TNode;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3DShapes_View_Control : public wxPanel
{
public:
	C3DShapes_View_Control(wxWindow *pParent, CSG_Shapes *pShapes, int Field_Color, CSG_Parameters &Settings);
	virtual ~C3DShapes_View_Control(void);

	bool						m_bCentral, m_bStereo, m_bFrame;

	int							m_cField, m_Style, m_Shading;

	double						m_xRotate, m_yRotate, m_zRotate, m_xShift, m_yShift, m_zShift, m_dCentral, m_Light_Hgt, m_Light_Dir;

	void						Update_View				(void);
	void						Update_Extent			(void);

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

	int							m_Color_Mode, m_Size_Def, m_cWire;

	double						m_xDown, m_yDown, m_cMin, m_cScale, m_Size_Scale;

	double						r_sin_x, r_sin_y, r_sin_z, r_cos_x, r_cos_y, r_cos_z, r_xc, r_yc, r_zc, r_Scale, r_Scale_z;

	CSG_Rect					m_Extent;

	CSG_Matrix					m_Image_zMax;

	CSG_Simple_Statistics		m_zStats, m_cStats;

	CSG_Parameters				*m_pSettings;

	CSG_Colors					*m_pColors;

	CSG_Shapes					*m_pShapes;

	wxPoint						m_Mouse_Down;

	wxImage						m_Image;


	void						_Set_Size				(void);

	bool						_Draw_Image				(void);

	void						_Draw_Shape				(CSG_Shape *pShape);

	void						_Draw_Point				(int iPoint);
	void						_Draw_Point				(int x, int y, double z, int color, int Size);

	void						_Draw_Line				(TNode a, TNode b, int Color);

	void						_Draw_Triangle			(TNode p[3], double dim);
	void						_Draw_Triangle_Line		(int y, double xa, double xb, double za, double zb, double ca, double cb, double dim);

	void						_Draw_Background		(void);
	void						_Draw_Frame				(void);
	void						_Draw_Pixel				(int x, int y, double z, int color);

	int							_Get_Color				(double value, double dim = -1.0);

	void						_Get_Projection			(TNode &p);


	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__3dshapes_view_control_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
