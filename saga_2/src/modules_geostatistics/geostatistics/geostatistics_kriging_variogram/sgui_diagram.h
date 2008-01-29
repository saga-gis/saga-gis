
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
//                    sgui_diagram.h                     //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#ifndef HEADER_INCLUDED__SGUI_Diagram_H
#define HEADER_INCLUDED__SGUI_Diagram_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgui_helper.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SGUI_API_DLL_EXPORT CSGUI_Diagram : public wxPanel
{
public:
	CSGUI_Diagram(wxWindow *pParent);
	virtual ~CSGUI_Diagram(void);

	wxString			m_xName, m_yName;

	bool				Set_xScale				(double Minimum, double Maximum);
	bool				Set_yScale				(double Minimum, double Maximum);


protected:

	double				m_xMin, m_xMax, m_yMin, m_yMax;

	int					Get_xToScreen			(double x, bool bKeepInRange = true);
	int					Get_yToScreen			(double y, bool bKeepInRange = true);
	bool				Get_ToScreen			(wxPoint &Point, double x, double y);


	virtual void		On_Draw					(wxDC &dc, wxRect rDraw)	{}


private:

	wxRect				m_rDiagram;

	void				_On_Mouse_Click_Left	(wxMouseEvent	&event);
	void				_On_Paint				(wxPaintEvent	&event);

	bool				_Draw					(wxDC &dc);


	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SGUI_Diagram_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
