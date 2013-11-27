/**********************************************************
 * Version $Id: data_source_files.h 911 2011-02-14 16:38:15Z reklov_w $
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
//                  data_source_files.h                  //
//                                                       //
//           Copyright (C) 2010 by Sun Zhuo              //
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
//    contact:    Sun Zhuo                               //
//                Centre for Maritime Studies            //
//                National University of Singapore       //
//                12 Prince George's Park                //
//                Singapore                              //
//                118411                                 //
//                                                       //
//    e-mail:     mixwind@gmail.com                      //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__Data_Source_Files__H
#define _HEADER_INCLUDED__SAGA_GUI__Data_Source_Files__H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dirctrl.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CData_Source_Files : public wxPanel
{
public:
	CData_Source_Files(wxWindow *pParent);

	void						SetPath				(const wxString &Path);


private:

	wxChoice					*m_pFilter;

	wxGenericDirCtrl			*m_pControl;


	void						On_TreeEvent		(wxTreeEvent    &event);

	void						On_Refresh			(wxCommandEvent &event);

	void						On_Change_Filter	(wxCommandEvent &event);

//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__Data_Source_Files__H
