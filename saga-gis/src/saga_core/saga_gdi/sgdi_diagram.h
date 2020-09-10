
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    sgdi_diagram.h                     //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_GDI_sgdi_diagram_H
#define HEADER_INCLUDED__SAGA_GDI_sgdi_diagram_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgdi_helper.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SGDI_API_DLL_EXPORT CSGDI_Diagram : public wxPanel
{
public:
	CSGDI_Diagram(wxWindow *pParent);
	virtual ~CSGDI_Diagram(void);

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

	void				_On_Mouse_Click			(wxMouseEvent	&event);
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
#endif // #ifndef HEADER_INCLUDED__SAGA_GDI_sgdi_diagram_H
