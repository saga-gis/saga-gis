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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	void						On_Open_Source		(wxCommandEvent &event);
	void						On_Close_Source		(wxCommandEvent &event);
	void						On_Close_Sources	(wxCommandEvent &event);
	void						On_Delete_Source	(wxCommandEvent &event);
	void						On_Open_Table		(wxCommandEvent &event);
	void						On_Drop_Table		(wxCommandEvent &event);

	void						Open_Source			(const wxTreeItemId &Item);
	void						Close_Source		(const wxTreeItemId &Item, bool bDelete);
	void						Close_Sources		(void);
	void						Open_Table			(const wxTreeItemId &Item);
	void						Drop_Table			(const wxTreeItemId &Item);

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
