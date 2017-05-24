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
#include "view_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Map_3D : public CVIEW_Base
{
	friend class CVIEW_Map_3DPanel;

public:
	CVIEW_Map_3D(class CWKSP_Map *pMap);
	virtual ~CVIEW_Map_3D(void);

	static class wxToolBarBase *	_Create_ToolBar			(void);
	static class wxMenu *			_Create_Menu			(void);

	virtual void					Do_Update				(void);

	void							Update_StatusBar		(void);


private:

	CSG_Parameters					m_Parameters;

	CSG_Grid						m_Map;

	class CVIEW_Map_3DPanel			*m_pPanel;


	void							On_Size					(wxSizeEvent     &event);

	void							On_Command				(wxCommandEvent  &event);
	void							On_Command_UI			(wxUpdateUIEvent &event);

	void							Parameters_Create		(void);
	void							Parameters_Update		(bool bToPanel);


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
