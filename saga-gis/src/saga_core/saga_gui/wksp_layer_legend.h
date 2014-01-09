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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
enum
{
	LEGEND_VERTICAL	= 0,
	LEGEND_HORIZONTAL
};


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

	wxSize						Get_Size			(double Zoom, double Zoom_Map);

	void						Draw				(wxDC &dc, double Zoom, double Zoom_Map, wxPoint Position, wxSize *pSize = NULL);

	int							Get_Orientation		(void)	{	return( m_Orientation );	}
	void						Set_Orientation		(int Orientation);


protected: ////////////////////////////////////////////////

	bool						m_Orientation;

	int							m_xBox, m_dxBox, m_xTick, m_dxTick, m_xText;

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

	void						_Draw_Box			(wxDC &dc, int y, int dy, int Style, int iClass);
	void						_Draw_Boxes			(wxDC &dc, int y, int Style, double zFactor = 1.0);

	void						_Draw_Point			(wxDC &dc, class CWKSP_Shapes_Point   *pLayer);
	void						_Draw_Line			(wxDC &dc, class CWKSP_Shapes_Line    *pLayer);
	void						_Draw_Polygon		(wxDC &dc, class CWKSP_Shapes_Polygon *pLayer);
	void						_Draw_TIN			(wxDC &dc, class CWKSP_TIN            *pLayer);
	void						_Draw_PointCloud	(wxDC &dc, class CWKSP_PointCloud     *pLayer);
	void						_Draw_Grid			(wxDC &dc, class CWKSP_Grid           *pLayer);

	void						_Draw_Grid_Image	(wxDC &dc, int y, class CSG_Grid *pGrid);

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
