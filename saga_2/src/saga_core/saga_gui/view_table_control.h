
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
#define TABLE_CTRL_FIXED_COLS	0x01
#define TABLE_CTRL_FIXED_ROWS	0x02
#define TABLE_CTRL_FIXED_TABLE	(TABLE_CTRL_FIXED_ROWS|TABLE_CTRL_FIXED_COLS)


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

	void						On_Field_Add		(wxCommandEvent  &event);
	void						On_Field_Add_UI		(wxUpdateUIEvent &event);
	void						On_Field_Del		(wxCommandEvent  &event);
	void						On_Field_Del_UI		(wxUpdateUIEvent &event);
	void						On_Field_Sort		(wxCommandEvent  &event);
	void						On_Field_Sort_UI	(wxUpdateUIEvent &event);

	void						On_Record_Add		(wxCommandEvent  &event);
	void						On_Record_Add_UI	(wxUpdateUIEvent &event);
	void						On_Record_Ins		(wxCommandEvent  &event);
	void						On_Record_Ins_UI	(wxUpdateUIEvent &event);
	void						On_Record_Del		(wxCommandEvent  &event);
	void						On_Record_Del_UI	(wxUpdateUIEvent &event);
	void						On_Record_Clr		(wxCommandEvent  &event);
	void						On_Record_Clr_UI	(wxUpdateUIEvent &event);

	void						On_Autosize_Cols	(wxCommandEvent  &event);
	void						On_Autosize_Rows	(wxCommandEvent  &event);

	void						On_Change			(wxGridEvent &event);
	void						On_LClick			(wxGridEvent &event);
	void						On_LClick_Label		(wxGridEvent &event);
	void						On_RClick_Label		(wxGridEvent &event);
	void						On_LDClick_Label	(wxGridEvent &event);
	void						On_Select			(wxGridRangeSelectEvent &event);

	bool						Load				(const wxChar *File_Name);
	bool						Save				(const wxChar *File_Name, int Format = 0);

	bool						Add_Record			(void);
	bool						Ins_Record			(void);
	bool						Del_Record			(void);
	bool						Del_Records			(void);

	void						Update_Selection	(void);
	void						Update_Table		(void);
	void						Sort_Table			(int iField, int Direction);


private:

	int							m_Constraint;

	class CSG_Table				*m_pTable;


	bool						_Set_Table			(void);

	bool						_Set_Records		(void);
	bool						_Set_Record			(int iRecord, class CSG_Table_Record *pRecord);

	void						_Select				(int iRow, bool bSelect);


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
