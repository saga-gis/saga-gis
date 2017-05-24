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
//                     DLG_Table.h                       //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__DLG_Table_H
#define _HEADER_INCLUDED__SAGA_GUI__DLG_Table_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "dlg_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_Table : public CDLG_Base
{
	DECLARE_CLASS(CDLG_Table)

public:
	CDLG_Table(class CSG_Table *pTable, wxString Caption);
	virtual ~CDLG_Table(void);


protected:

	class CSG_Table				*m_pTable;

	class CDLG_Table_Control	*m_pControl;


	void						On_Key			(wxKeyEvent     &event);

	void						On_Load			(wxCommandEvent &event);
	void						On_Save			(wxCommandEvent &event);

	void						On_WKSP_Load	(wxCommandEvent &event);
	void						On_WKSP_Save	(wxCommandEvent &event);

	void						On_Add			(wxCommandEvent &event);
	void						On_Insert		(wxCommandEvent &event);
	void						On_Delete		(wxCommandEvent &event);
	void						On_Delete_All	(wxCommandEvent &event);
	void						On_Colors		(wxCommandEvent &event);

	virtual void				Set_Position	(wxRect r);

	virtual void				Save_Changes	(void);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__DLG_Table_H
