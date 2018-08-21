/**********************************************************
 * Version $Id: dlg_table_control.h 2665 2015-10-28 12:55:25Z oconrad $
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
//                  dlg_table_control.h                  //
//                                                       //
//          Copyright (C) 2015 by Olaf Conrad            //
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
//                University of Hamburg                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__dlg_table_control_H
#define _HEADER_INCLUDED__SAGA_GUI__dlg_table_control_H


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
class CDLG_Table_Control : public wxGrid
{
	DECLARE_CLASS(CDLG_Table_Control)

public:
	CDLG_Table_Control(wxWindow *pParent, class CSG_Table *pTable);
	virtual ~CDLG_Table_Control(void);

	void						On_Field_Sort		(wxCommandEvent  &event);
	void						On_Field_Sort_UI	(wxUpdateUIEvent &event);

	void						On_Field_Open		(wxCommandEvent  &event);

	void						On_Record_Add		(wxCommandEvent  &event);
	void						On_Record_Add_UI	(wxUpdateUIEvent &event);
	void						On_Record_Ins		(wxCommandEvent  &event);
	void						On_Record_Ins_UI	(wxUpdateUIEvent &event);
	void						On_Record_Del		(wxCommandEvent  &event);
	void						On_Record_Del_UI	(wxUpdateUIEvent &event);
	void						On_Record_Clr		(wxCommandEvent  &event);

	void						On_Autosize_Cols	(wxCommandEvent  &event);
	void						On_Autosize_Rows	(wxCommandEvent  &event);

	void						On_Size				(wxSizeEvent     &event);
	void						On_Key				(wxKeyEvent      &event);

	void						On_Edit_Start		(wxGridEvent     &event);
	void						On_Edit_Stop		(wxGridEvent     &event);
	void						On_Changed			(wxGridEvent     &event);
	void						On_LClick			(wxGridEvent     &event);
	void						On_RClick			(wxGridEvent     &event);
	void						On_LClick_Label		(wxGridEvent     &event);
	void						On_RClick_Label		(wxGridEvent     &event);
	void						On_LDClick_Label	(wxGridEvent     &event);
	void						On_Select			(wxGridRangeSelectEvent &event);

	bool						Update_Table		(void);
	bool						Update_Selection	(void);
	bool						Update_Sorting		(int iField, int Direction);

	bool						Add_Record			(void);
	bool						Ins_Record			(void);
	bool						Del_Record			(void);
	bool						Del_Records			(void);

	bool						Load				(void);
	bool						Save				(void);

	class CSG_Table *			Get_Table			(void)	{	return( m_pTable );	}


private:

	bool						m_bEditing, m_bLUT;

	class CSG_Table				*m_pTable;


	bool						_Set_Records		(void);
	bool						_Set_Record			(int iRecord, class CSG_Table_Record *pRecord);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__dlg_table_control_H
