
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
//                 WKSP_Base_Control.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Control_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Control_H


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
class CWKSP_Base_Control : public wxTreeCtrl
{
	DECLARE_CLASS(CWKSP_Base_Control)

public:
	CWKSP_Base_Control(wxWindow *pParent, wxWindowID id);
	virtual ~CWKSP_Base_Control(void);

	class CWKSP_Base_Item *		Get_Item_Selected	(void);
	bool						Set_Item_Selected	(class CWKSP_Base_Item *pItem, bool bKeepMultipleSelection = false);
	int							Get_Selection_Count	(void);

	void						On_Command			(wxCommandEvent  &event);
	void						On_Command_UI		(wxUpdateUIEvent &event);

	void						On_Item_LClick		(wxMouseEvent    &event);
	void						On_Item_LDClick		(wxMouseEvent    &event);
	void						On_Item_RClick		(wxTreeEvent     &event);
	void						On_Item_SelChanged	(wxTreeEvent     &event);
	void						On_Item_Delete		(wxTreeEvent     &event);
	void						On_Item_KeyDown		(wxTreeEvent     &event);

	wxMenu *					Get_Context_Menu	(void);


protected:

	class CWKSP_Base_Manager	*m_pManager;


	bool						_Set_Manager		(class CWKSP_Base_Manager *pManager);

	bool						_Add_Item			(class CWKSP_Base_Item *pItem, int Image, int selImage, bool bSort = true);
	bool						_Del_Item			(class CWKSP_Base_Item *pItem, bool bSilent);
	bool						_Del_Item_Confirm	(class CWKSP_Base_Item *pItem);

	bool						_Set_Active			(void);
	bool						_Del_Active			(bool bSilent);

	bool						_Show_Active		(void);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Control_H
