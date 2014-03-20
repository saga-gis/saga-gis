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
//                    VIEW_Map_3D.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_3D_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_3D_H


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
class CVIEW_Map_3D : public CVIEW_Base
{
	friend class CVIEW_Map_3D_Image;

public:
	CVIEW_Map_3D(class CWKSP_Map *pMap);
	virtual ~CVIEW_Map_3D(void);

	static class wxToolBarBase *	_Create_ToolBar			(void);
	static class wxMenu *			_Create_Menu			(void);

	virtual void					Do_Update				(void);


private:

	int								m_Play_Mode;

	double							m_xDown, m_yDown;

	CSG_Parameters					m_Parameters;

	CSG_Table						m_Play;

	wxPoint							m_Mouse_Down;

	class CVIEW_Map_3D_Image		*m_pImage;


	void							On_Command				(wxCommandEvent  &event);
	void							On_Command_UI			(wxUpdateUIEvent &event);

	void							On_Size					(wxSizeEvent  &event);
	void							On_Paint				(wxPaintEvent &event);

	void							On_Key_Down				(wxKeyEvent   &event);

	void							On_Mouse_LDown			(wxMouseEvent &event);
	void							On_Mouse_LUp			(wxMouseEvent &event);
	void							On_Mouse_RDown			(wxMouseEvent &event);
	void							On_Mouse_RUp			(wxMouseEvent &event);
	void							On_Mouse_MDown			(wxMouseEvent &event);
	void							On_Mouse_MUp			(wxMouseEvent &event);
	void							On_Mouse_Motion			(wxMouseEvent &event);
	void							On_Mouse_Wheel			(wxMouseEvent &event);

	void							_Paint					(void);
	void							_Paint					(wxDC &dc);

	void							_Parms_Create			(void);
	bool							_Parms_Update			(bool bGet);
	bool							_Parms_Update			(bool bGet, class CVIEW_Map_3D_Image *pImage);
	void							_Parms_StatusBar		(void);
	void							_Parms_Command			(int Command);
	bool							_Parms_Dlg				(void);
	void							_Parms_Changed			(void);

	void							_Play_Pos_Add			(void);
	void							_Play_Pos_Del			(void);
	void							_Play_Pos_Clr			(void);
	void							_Play_Pos_Edit			(void);
	void							_Play_Once				(void);
	void							_Play_Loop				(void);
	void							_Play_Save				(void);
	void							_Play_Stop				(void);
	bool							_Play					(void);


	//-----------------------------------------------------
	DECLARE_CLASS(CVIEW_Map_3D)
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Map_3D_H
