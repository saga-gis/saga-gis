/**********************************************************
 * Version $Id: data_source_pgsql.h 911 2011-02-14 16:38:15Z reklov_w $
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
//                  data_source_pgsql.h                  //
//                                                       //
//          Copyright (C) 2013 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'MicroCity: Spatial Analysis and //
// Simulation Framework'. MicroCity is free software;you //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// MicroCity is distributed in the hope that it will be  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__data_source_pgsql__H
#define _HEADER_INCLUDED__SAGA_GUI__data_source_pgsql__H


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
class CData_Source_PgSQL : public wxTreeCtrl
{
public:
	CData_Source_PgSQL(wxWindow *pParent);
	virtual ~CData_Source_PgSQL(void);

	void						Update_Sources		(void);
	void						Update_Source		(const wxString &Server);


private:

	void						On_Item_Activated	(wxTreeEvent &event);
	void						On_Item_RClick		(wxTreeEvent &event);
	void						On_Item_Menu		(wxTreeEvent &event);

	void						On_Refresh			(wxCommandEvent &event);
	void						On_Source_Open		(wxCommandEvent &event);
	void						On_Source_Close		(wxCommandEvent &event);
	void						On_Sources_Close	(wxCommandEvent &event);
	void						On_Source_Delete	(wxCommandEvent &event);
	void						On_Table_Open		(wxCommandEvent &event);
	void						On_Table_Rename		(wxCommandEvent &event);
	void						On_Table_Info		(wxCommandEvent &event);
	void						On_Table_Drop		(wxCommandEvent &event);

	void						Source_Open			(const wxTreeItemId &Item);
	void						Source_Close		(const wxTreeItemId &Item, bool bDelete);
	void						Sources_Close		(void);
	void						Table_Open			(const wxTreeItemId &Item);
	void						Table_Rename		(const wxTreeItemId &Item);
	void						Table_Info			(const wxTreeItemId &Item);
	void						Table_Drop			(const wxTreeItemId &Item);

	wxTreeItemId				Find_Source			(const wxString &Server);
	void						Update_Item			(const wxTreeItemId &Item);
	void						Update_Source		(const wxTreeItemId &Item);
	void						Append_Table		(const wxTreeItemId &Parent, const SG_Char *Name, int Type, int Image);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__data_source_pgsql__H
