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
//                      dlg_list.h                       //
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
class CDLG_List_Base : public CDLG_Base
{
	DECLARE_CLASS(CDLG_List_Base)
	DECLARE_EVENT_TABLE()

public:
	CDLG_List_Base(class CSG_Parameter_List *pList, wxString Caption);

	void						On_DClick_Add	(wxCommandEvent &event);
	void						On_Add			(wxCommandEvent &event);
	void						On_Add_All		(wxCommandEvent &event);
	void						On_DClick_Del	(wxCommandEvent &event);
	void						On_Del			(wxCommandEvent &event);
	void						On_Del_All		(wxCommandEvent &event);
	void						On_Up			(wxCommandEvent &event);
	void						On_Down			(wxCommandEvent &event);


protected:

	TSG_Data_Object_Type		m_Type;

	wxListBox					*m_pAdd, *m_pSelect;

	class CSG_Parameter_List	*m_pList;


	virtual void				Set_Position	(wxRect r);

	void						Set_Data		(class CWKSP_Base_Manager *pManager);

	void						Save_Changes	(void);


private:

	int							m_btn_height;

	wxButton					*m_pBtn_Add, *m_pBtn_Add_All, *m_pBtn_Del, *m_pBtn_Del_All, *m_pBtn_Up, *m_pBtn_Down;


	void						_Add			(void);
	void						_Del			(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_List_Table : public CDLG_List_Base
{
	DECLARE_CLASS(CDLG_List_Table)

public:
	CDLG_List_Table(CSG_Parameter_Table_List *pList, wxString Caption);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_List_Shapes : public CDLG_List_Base
{
	DECLARE_CLASS(CDLG_List_Shapes)

public:
	CDLG_List_Shapes(CSG_Parameter_Shapes_List *pList, wxString Caption);


protected:

	int							m_Shape_Type;


	void						_Set_Data		(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_List_PointCloud : public CDLG_List_Base
{
	DECLARE_CLASS(CDLG_List_PointCloud)

public:
	CDLG_List_PointCloud(CSG_Parameter_PointCloud_List *pList, wxString Caption);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_List_TIN : public CDLG_List_Base
{
	DECLARE_CLASS(CDLG_List_TIN)

public:
	CDLG_List_TIN(CSG_Parameter_TIN_List *pList, wxString Caption);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_List_Grid_Base : public CDLG_List_Base
{
	DECLARE_CLASS(CDLG_List_Grid_Base)
	DECLARE_EVENT_TABLE()

public:
	CDLG_List_Grid_Base(CSG_Parameter_List *pList, wxString Caption);


protected:

	wxChoice					*m_pSystems;

	class CWKSP_Grid_System		*m_pSystem;


	void						On_Select_System	(wxCommandEvent &event);

	virtual void				Set_Position		(wxRect r);

	virtual void				_Set_Data			(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_List_Grid : public CDLG_List_Grid_Base
{
	DECLARE_CLASS(CDLG_List_Grid)

public:
	CDLG_List_Grid(CSG_Parameter_Grid_List *pList, wxString Caption);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_List_Grids : public CDLG_List_Grid_Base
{
	DECLARE_CLASS(CDLG_List_Grids)

public:
	CDLG_List_Grids(CSG_Parameter_Grids_List *pList, wxString Caption);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__DLG_List_Base_H
