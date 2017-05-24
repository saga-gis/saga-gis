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
//                 VIEW_Table_Control.h                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Control_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Control_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/grid.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Table_Control : public wxGrid
{
	DECLARE_CLASS(CVIEW_Table_Control)

public:
	CVIEW_Table_Control(wxWindow *pParent, class CSG_Table *pTable, int Constraint);
	virtual ~CVIEW_Table_Control(void);

	bool						Load				(const wxString &File_Name);
	bool						Save				(const wxString &File_Name, int Format = 0);

	bool						Update_Table		(void);
	bool						Update_Selection	(void);


protected:

	virtual void				AdjustScrollbars	(void);

	void						On_Scroll			(wxScrollWinEvent &event);
	void						On_Key				(wxKeyEvent       &event);
	void						On_Size				(wxSizeEvent      &event);

	void						On_Edit_Start		(wxGridEvent      &event);
	void						On_Edit_Stop		(wxGridEvent      &event);
	void						On_Changed			(wxGridEvent      &event);

	void						On_LClick			(wxGridEvent      &event);
	void						On_LClick_Label		(wxGridEvent      &event);
	void						On_LDClick_Label	(wxGridEvent      &event);

	void						On_RClick			(wxGridEvent      &event);
	void						On_RClick_Label		(wxGridEvent      &event);

	void						On_Select			(wxGridRangeSelectEvent &event);

	void						On_Sel_Only			(wxCommandEvent   &event);
	void						On_Sel_Only_UI		(wxUpdateUIEvent  &event);
	void						On_Sel_Clear		(wxCommandEvent   &event);
	void						On_Sel_Clear_UI		(wxUpdateUIEvent  &event);

	void						On_Autosize_Cols	(wxCommandEvent   &event);
	void						On_Autosize_Rows	(wxCommandEvent   &event);

	void						On_Cell_Open		(wxCommandEvent   &event);

	void						On_ToClipboard		(wxCommandEvent   &event);

	void						On_Field_Add		(wxCommandEvent   &event);
	void						On_Field_Add_UI		(wxUpdateUIEvent  &event);
	void						On_Field_Del		(wxCommandEvent   &event);
	void						On_Field_Del_UI		(wxUpdateUIEvent  &event);
	void						On_Field_Sort		(wxCommandEvent   &event);
	void						On_Field_Sort_UI	(wxUpdateUIEvent  &event);
	void						On_Field_Rename		(wxCommandEvent   &event);
	void						On_Field_Rename_UI	(wxUpdateUIEvent  &event);
	void						On_Field_Type		(wxCommandEvent   &event);
	void						On_Field_Type_UI	(wxUpdateUIEvent  &event);
	void						On_Field_Calc		(wxCommandEvent   &event);
	void						On_Field_Calc_UI	(wxUpdateUIEvent  &event);

	void						On_Record_Add		(wxCommandEvent   &event);
	void						On_Record_Add_UI	(wxUpdateUIEvent  &event);
	void						On_Record_Ins		(wxCommandEvent   &event);
	void						On_Record_Ins_UI	(wxUpdateUIEvent  &event);
	void						On_Record_Del		(wxCommandEvent   &event);
	void						On_Record_Del_UI	(wxUpdateUIEvent  &event);
	void						On_Record_Clr		(wxCommandEvent   &event);
	void						On_Record_Clr_UI	(wxUpdateUIEvent  &event);


private:

	bool						m_bSelOnly, m_bEditing;

	int							m_Scroll_Start, m_Scroll_Range, m_Cursor, m_Decimals;

	CSG_Table					*m_pTable;

	CSG_Parameters				m_Field_Calc;


	void						_Update_Views		(void);

	bool						_Update_Sorting		(int iField, int Direction);

	bool						_Update_Records		(void);

	int							_Get_Record_Count	(void);
	CSG_Table_Record *			_Get_Record			(int iRow);
	bool						_Set_Record			(int iRow);
	wxString					_Get_Value			(CSG_Table_Record *pRecord, int iField);

	int							_Get_Scroll_Start	(int Position);
	bool						_Set_Scroll_Start	(int Position, bool bEnforceUpdate = true);

	bool						_Get_DataSource		(wxString &Source);

	bool						_ToClipboard		(void);

	static int					_Parameter_Callback	(CSG_Parameter *pParameter, int Flags);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Control_H
