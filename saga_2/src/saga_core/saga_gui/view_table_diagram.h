
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
//                 VIEW_Table_Diagram.h                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Diagram_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Diagram_H


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
class CVIEW_Table_Diagram : public CVIEW_Base
{
public:
	CVIEW_Table_Diagram(class CWKSP_Table *pTable);
	virtual ~CVIEW_Table_Diagram(void);

	static class wxToolBarBase *	_Create_ToolBar		(void);
	static class wxMenu *			_Create_Menu		(void);

	void							On_Size				(wxSizeEvent &event);
	void							On_Paint			(wxPaintEvent &event);

	void							On_Parameters		(wxCommandEvent &event);

	void							Draw				(wxDC &dc, wxRect rDraw);

	bool							Update_Diagram		(void);


private:

	int								m_nFields, *m_Fields, m_xField;

	double							m_zMin, m_zMax, m_xMin, m_xMax;

	CSG_Colors							m_Colors;

	CTable							*m_pTable, m_Structure;

	CParameters						m_Parameters;

	class CWKSP_Table				*m_pOwner;

	class wxFont					*m_pFont;


	void							_Destroy_Fields		(void);
	bool							_Set_Fields			(void);

	void							_Initialize			(void);

	bool							_DLG_Parameters		(void);

	void							_Draw_Diagram		(wxDC &dc, wxRect r);
	void							_Draw_Frame			(wxDC &dc, wxRect r, double dx, double dy);
	void							_Draw_Legend		(wxDC &dc, wxRect r);

	void							_Draw_Line			(wxDC &dc, wxRect r, double dx, double dy, int iField, bool bLine, bool bPoint);
	void							_Draw_Bars			(wxDC &dc, wxRect r, double dx, double dy, int iField);


private:

	DECLARE_EVENT_TABLE()
	DECLARE_CLASS(CVIEW_Table_Diagram)

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Diagram_H
