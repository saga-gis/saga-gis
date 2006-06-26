
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
//                    VIEW_Ruler.h                       //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Ruler_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Ruler_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/panel.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RULER_HORIZONTAL		0x00
#define RULER_VERTICAL			0x01
#define RULER_EDGE_BLACK		0x02
#define RULER_EDGE_SUNKEN		0x04
#define RULER_DESCENDING		0x08
#define RULER_MODE_NORMAL		0x10
#define RULER_MODE_CORNERS		0x20
#define RULER_MODE_SCALE		0x40
#define RULER_TICKATBOTTOM		0x80


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Ruler : public wxPanel
{
public:
	CVIEW_Ruler(wxWindow *pParent, int Style = RULER_HORIZONTAL);
	virtual ~CVIEW_Ruler(void);

	void						On_Paint			(wxPaintEvent &event);

	void						Set_Range			(double Min, double Max);
	void						Set_Range_Core		(double Min, double Max);
	void						Set_Position		(int Position);


private:

	bool						m_bHorizontal, m_bAscendent, m_bTickAtTop;

	int							m_Mode, m_Edge, m_Position;

	double						m_Min, m_Max, m_Min_Core, m_Max_Core;

	void						_Draw_Corners		(wxDC &dc, int Width, int Height);
	void						_Draw_Core			(wxDC &dc, int Width, int Height);
	void						_Draw_Position		(wxDC &dc, int Width, int Height, int Position);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Ruler_H
