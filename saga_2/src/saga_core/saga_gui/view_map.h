
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
//                      VIEW_Map.h                       //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "view_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Map : public CVIEW_Base
{
public:
	CVIEW_Map(class CWKSP_Map *pMap, int Frame_Width);
	virtual ~CVIEW_Map(void);

	static class wxToolBarBase *	_Create_ToolBar				(void);
	static class wxMenu *			_Create_Menu				(void);

	void							On_Paint					(wxPaintEvent    &event);
	void							On_Size						(wxSizeEvent     &event);
	void							On_Key_Down					(wxKeyEvent      &event);

	virtual void					On_Command_UI				(wxUpdateUIEvent &event);

	void							On_Map_3D_Show				(wxCommandEvent  &event);
	void							On_Map_Layout_Show			(wxCommandEvent  &event);
	void							On_Map_Save_Image			(wxCommandEvent  &event);
	void							On_Map_Save_PDF_Indexed		(wxCommandEvent  &event);
	void							On_Map_Save_Interactive_SVG (wxCommandEvent	 &event);

	void							On_Map_Zoom_Full			(wxCommandEvent  &event);
	void							On_Map_Zoom_Last			(wxCommandEvent  &event);
	void							On_Map_Zoom_Layer			(wxCommandEvent  &event);
	void							On_Map_Zoom_Selection		(wxCommandEvent  &event);
	void							On_Map_Zoom_Extent			(wxCommandEvent  &event);
	void							On_Map_Zoom_Synchronize		(wxCommandEvent  &event);

	void							On_Map_Mode_Zoom			(wxCommandEvent  &event);
	void							On_Map_Mode_Pan				(wxCommandEvent  &event);
	void							On_Map_Mode_Select			(wxCommandEvent  &event);
	void							On_Map_Mode_Distance		(wxCommandEvent  &event);

	class CVIEW_Map_Control *		Get_Map_Control				(void)	{	return( m_pControl );	}

	void							Refresh_Map					(void);

	void							Ruler_Set_Width				(int Width);
	void							Ruler_Set_Position			(int x, int y);
	void							Ruler_Refresh				(void);


private:

	int								m_Ruler_Size;

	class CWKSP_Map					*m_pMap;

	class CVIEW_Map_Control			*m_pControl;

	class CVIEW_Ruler				*m_pRuler_X1, *m_pRuler_X2, *m_pRuler_Y1, *m_pRuler_Y2;


	void							_Set_Positions				(void);


private:

	DECLARE_EVENT_TABLE()
	DECLARE_CLASS(CVIEW_Map)

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_H
