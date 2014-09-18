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
class CACTIVE_History : public wxTreeCtrl
{
	DECLARE_CLASS(CACTIVE_History)

public:
	CACTIVE_History(wxWindow *pParent);

	bool				Set_Item				(class CWKSP_Base_Item *pItem);


private:

	void				On_Mouse_RDown			(wxMouseEvent   &event);
	void				On_Clear				(wxCommandEvent &event);
	void				On_SaveAs_Model			(wxCommandEvent &event);
	void				On_Options_Expand		(wxCommandEvent &event);

	void				_Expand					(wxTreeItemId Node, const wxString &Name, bool bExpand);

	CSG_Data_Object *	_Get_Object				(void);
	int					_Get_Image				(TSG_Parameter_Type Type);
	int					_Get_Image				(const CSG_String  &Type);

	bool				_Add_History			(wxTreeItemId Parent, CSG_MetaData &History);

	bool				_OLD_Add_History		(wxTreeItemId Parent, CSG_MetaData &History);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_History_H
