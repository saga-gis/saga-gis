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
//                     VIEW_Table.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_H


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
class CVIEW_Table : public CVIEW_Base
{
public:
	CVIEW_Table(class CWKSP_Table *pTable);

	static class wxToolBarBase *	_Create_ToolBar		(void);
	static class wxMenu *			_Create_Menu		(void);

	virtual void					Do_Update			(void);

	void							Update_Selection	(void);


private:

	class CWKSP_Table				*m_pTable;

	class CVIEW_Table_Control		*m_pControl;


	void							On_Field_Add		(wxCommandEvent  &event);
	void							On_Field_Add_UI		(wxUpdateUIEvent &event);
	void							On_Field_Del		(wxCommandEvent  &event);
	void							On_Field_Del_UI		(wxUpdateUIEvent &event);
	void							On_Field_Sort		(wxCommandEvent  &event);
	void							On_Field_Sort_UI	(wxUpdateUIEvent &event);
	void							On_Field_Rename		(wxCommandEvent  &event);
	void							On_Field_Rename_UI	(wxUpdateUIEvent &event);
	void							On_Field_Type		(wxCommandEvent  &event);
	void							On_Field_Type_UI	(wxUpdateUIEvent &event);

	void							On_Record_Add		(wxCommandEvent  &event);
	void							On_Record_Add_UI	(wxUpdateUIEvent &event);
	void							On_Record_Ins		(wxCommandEvent  &event);
	void							On_Record_Ins_UI	(wxUpdateUIEvent &event);
	void							On_Record_Del		(wxCommandEvent  &event);
	void							On_Record_Del_UI	(wxUpdateUIEvent &event);
	void							On_Record_Clr		(wxCommandEvent  &event);
	void							On_Record_Clr_UI	(wxUpdateUIEvent &event);


	//-----------------------------------------------------
	DECLARE_CLASS(CVIEW_Table)
	DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_H
