/**********************************************************
 * Version $Id: data_source_odbc.h 911 2011-02-14 16:38:15Z reklov_w $
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
//                  data_source_odbc.h                   //
//                                                       //
//          Copyright (C) 2012 by Olaf Conrad            //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__data_source_odbc__H
#define _HEADER_INCLUDED__SAGA_GUI__data_source_odbc__H


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
class CData_Source_ODBC : public wxTreeCtrl
{
public:
	CData_Source_ODBC(wxWindow *pParent);

	void						Update_Source		(const wxString &Server);
	void						Update_Sources		(void);


private:

	void						On_Refresh			(wxCommandEvent &event);
	void						On_Source_Close_All	(wxCommandEvent &event);
	void						On_Source_Close		(wxCommandEvent &event);
	void						On_Source_Open		(wxCommandEvent &event);
	void						On_Table_Open		(wxCommandEvent &event);
	void						On_Table_Delete		(wxCommandEvent &event);

	void						On_Item_Activated	(wxTreeEvent &event);
	void						On_Item_RClick		(wxTreeEvent &event);
	void						On_Item_Menu		(wxTreeEvent &event);

	void						Source_Close_All	(void);
	void						Source_Close		(const wxTreeItemId &Item);
	void						Source_Open			(const wxTreeItemId &Item);
	void						Table_Open			(const wxTreeItemId &Item);
	void						Table_Delete		(const wxTreeItemId &Item);

	void						Update_Item			(const wxTreeItemId &Item);
	void						Update_Source		(const wxTreeItemId &Item);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__data_source_odbc__H
