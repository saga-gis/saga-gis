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
//                     VIEW_Base.h                       //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Base_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Base_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef MDI_TABBED
#include <wx/aui/tabmdi.h>
#define MDI_ChildFrame	wxAuiMDIChildFrame
#else
#include <wx/mdi.h>
#define MDI_ChildFrame	wxMDIChildFrame
#endif

//---------------------------------------------------------
#include <wx/menu.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Base : public MDI_ChildFrame
{
public:
	CVIEW_Base(class CWKSP_Base_Item *pOwner, int View_ID, wxString Caption, int Icon_ID, bool bShow = true);
	virtual ~CVIEW_Base(void);

	virtual void				Do_Destroy			(void);
	virtual void				Do_Update			(void);

	virtual void				On_Command_UI		(wxUpdateUIEvent &event);


protected:

	class CWKSP_Base_Item		*m_pOwner;


	void						Do_Show				(void);

	void						Set_Size_Min		(int x, int y);

	void						On_Size				(wxSizeEvent     &event);
	void						On_Activate			(wxActivateEvent &event);
	void						On_Quit				(wxCommandEvent  &event);


private:

	int							m_View_ID;

	wxSize						m_Size_Min;


	//-----------------------------------------------------
	DECLARE_CLASS(CVIEW_Base)
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Base_H
