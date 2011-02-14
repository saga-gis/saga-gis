/**********************************************************
 * Version $Id$
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
//                 WKSP_Layer_Legend.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_Legend_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_Legend_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Layer_Legend
{
public: ///////////////////////////////////////////////////
	CWKSP_Layer_Legend(class CWKSP_Layer *pLayer);
	virtual ~CWKSP_Layer_Legend(void);

	wxSize						Get_Size			(double Zoom, double Zoom_Map, bool bVertical = true);
	void						Draw				(wxDC &dc, double Zoom, double Zoom_Map, wxPoint Position, wxSize *pSize = NULL, bool bVertical = true);


protected: ////////////////////////////////////////////////

	bool						m_Box_bOutline, m_Box_bFill, m_bVertical;

	int							m_xBox, m_dxBox, m_xTick, m_dxTick, m_xText, m_BoxStyle;

	double						m_Zoom, m_Zoom_Map;

	wxPoint						m_Position;

	wxSize						m_Size;

	wxPen						m_oldPen;

	wxBrush						m_oldBrush;

	wxFont						m_oldFont;

	class CWKSP_Layer			*m_pLayer;

	class CWKSP_Layer_Classify	*m_pClassify;


	void						_Set_Size			(int xSet, int yAdd);
	void						_Set_Font			(wxDC &dc, int Style);
	void						_Draw_Title			(wxDC &dc, int Style, wxString Text);
	void						_Draw_Label			(wxDC &dc, int y, wxString Text, int yAlign);
	void						_Draw_Box			(wxDC &dc, int y, int dy, wxColour Color);
	void						_Draw_Box_Image		(wxDC &dc, int y, class CSG_Grid *pGrid);

	void						_Draw_Point			(wxDC &dc, class CWKSP_Shapes_Point   *pLayer);
	void						_Draw_Point_Sizes	(wxDC &dc, class CWKSP_Shapes_Point   *pLayer, int min_Size, int max_Size, double min_Value, double d_Value);
	void						_Draw_Line			(wxDC &dc, class CWKSP_Shapes_Line    *pLayer);
	void						_Draw_Polygon		(wxDC &dc, class CWKSP_Shapes_Polygon *pLayer);
	void						_Draw_TIN			(wxDC &dc, class CWKSP_TIN            *pLayer);
	void						_Draw_Grid			(wxDC &dc, class CWKSP_Grid           *pLayer);

	void						_Draw_Boxes			(wxDC &dc, int y);
	void						_Draw_Continuum		(wxDC &dc, int y, double zFactor);
	void						_Draw_Continuum_V	(wxDC &dc, int y, double yToDC, double zMin, double zMax, double zFactor, double dz, int dyFont);
	void						_Draw_Continuum_H	(wxDC &dc, int y, double yToDC, double zMin, double zMax, double zFactor, double dz, int dyFont);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Layer_Legend_H
