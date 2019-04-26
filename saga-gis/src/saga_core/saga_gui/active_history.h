
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
//                   ACTIVE_History.h                    //
//                                                       //
//          Copyright (C) 2008 by Olaf Conrad            //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_History_H
#define _HEADER_INCLUDED__SAGA_GUI__ACTIVE_History_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/treectrl.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CActive_History : public wxTreeCtrl
{
	DECLARE_CLASS(CActive_History)

public:
	CActive_History(wxWindow *pParent);

	bool					Set_Item				(class CWKSP_Data_Item *pItem);


private:

	class CWKSP_Data_Item	*m_pItem;


	void					On_Mouse_RDown			(wxMouseEvent   &event);
	void					On_Clear				(wxCommandEvent &event);
	void					On_SaveAs_Model			(wxCommandEvent &event);
	void					On_Options_Expand		(wxCommandEvent &event);

	void					_Expand					(wxTreeItemId Node, const wxString &Name, bool bExpand);

	CSG_Data_Object *		_Get_Object				(void);

	int						_Get_Image				(TSG_Parameter_Type Type);
	int						_Get_Image				(const CSG_String  &Type);

	bool					_Set_History			(void);
	bool					_Add_History			(wxTreeItemId Parent, CSG_MetaData &History);
	bool					_Add_History_OLD		(wxTreeItemId Parent, CSG_MetaData &History);


	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_History_H
