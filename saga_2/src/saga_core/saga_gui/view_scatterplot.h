
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
//                  VIEW_ScatterPlot.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_ScatterPlot_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_ScatterPlot_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include "view_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Add_ScatterPlot(CGrid *pGrid);
void		Add_ScatterPlot(CTable *pTable);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_ScatterPlot : public CVIEW_Base
{
public:
	CVIEW_ScatterPlot(CGrid *pGrid_A, CGrid *pGrid_B);
	CVIEW_ScatterPlot(CGrid *pGrid, CShapes *pShapes, int Field);
	CVIEW_ScatterPlot(CTable *pTable, int Field_A, int Field_B);
	virtual ~CVIEW_ScatterPlot(void);

	static class wxToolBarBase *	_Create_ToolBar				(void);
	static class wxMenu *			_Create_Menu				(void);

	void							On_Size						(wxSizeEvent &event);
	void							On_Paint					(wxPaintEvent &event);

	void							On_Parameters				(wxCommandEvent &event);
	void							On_Update					(wxCommandEvent &event);

	void							Draw						(wxDC &dc, wxRect r);

	void							Update						(void);


private:

	int								m_Method, m_xField, m_yField;

	CGrid							*m_pGrid_X, *m_pGrid_Y;

	CShapes							*m_pShapes;

	CTable							*m_pTable;

	CMAT_Regression					m_Regression;

	CParameters						m_Parameters;

	wxString						m_sTitle, m_sX, m_sY;


	void							_On_Construction			(void);

	bool							_Initialize					(void);

	bool							_Initialize_Grids			(CGrid *pGrid_X, CGrid *pGrid_Y);
	bool							_Initialize_Grid_Equal		(CGrid *pGrid_X, CGrid *pGrid_Y);
	bool							_Initialize_Grid_Unequal	(CGrid *pGrid_X, CGrid *pGrid_Y);

	bool							_Initialize_Shapes			(CGrid *pGrid_X, CShapes *pShapes_Y, int Field);

	bool							_Initialize_Table			(CTable *pTable, int Field_X, int Field_Y);

	wxRect							_Draw_Get_rDiagram			(wxRect r);
	void							_Draw_Image					(wxDC &dc, wxRect r, double dx, double dy);
	void							_Draw_Points				(wxDC &dc, wxRect r, double dx, double dy);
	void							_Draw_Regression			(wxDC &dc, wxRect r, double dx, double dy);
	void							_Draw_Frame					(wxDC &dc, wxRect r);


private:

	DECLARE_EVENT_TABLE()
	DECLARE_CLASS(CVIEW_ScatterPlot)

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_ScatterPlot_H
