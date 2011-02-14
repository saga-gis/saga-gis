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
//                   DLG_List_Base.h                     //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__DLG_List_Base_H
#define _HEADER_INCLUDED__SAGA_GUI__DLG_List_Base_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/button.h>
#include <wx/listbox.h>

#include "dlg_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DLG_LIST_BTN_WIDTH		40
#define DLG_LIST_BTN_DIST		4


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_List_Base : public CDLG_Base
{
	DECLARE_CLASS(CDLG_List_Base)

public:
	CDLG_List_Base(class CSG_Parameter_List *pList, wxString Caption);
	virtual ~CDLG_List_Base(void);

	void						On_DClick_Add	(wxCommandEvent &event);
	void						On_Add			(wxCommandEvent &event);
	void						On_Add_All		(wxCommandEvent &event);
	void						On_DClick_Del	(wxCommandEvent &event);
	void						On_Del			(wxCommandEvent &event);
	void						On_Del_All		(wxCommandEvent &event);
	void						On_Up			(wxCommandEvent &event);
	void						On_Down			(wxCommandEvent &event);


protected:

	int							m_btn_height;

	wxButton					*m_pBtn_Add, *m_pBtn_Add_All, *m_pBtn_Del, *m_pBtn_Del_All, *m_pBtn_Up, *m_pBtn_Down;

	wxListBox					*m_pAdd, *m_pSelect;

	class CSG_Parameter_List	*m_pList;


	virtual void				Set_Position	(wxRect r);

	virtual void				Save_Changes	(void);

	virtual void				_Set_Objects	(void)	= 0;

	void						_Add			(void);
	void						_Del			(void);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__DLG_List_Base_H
