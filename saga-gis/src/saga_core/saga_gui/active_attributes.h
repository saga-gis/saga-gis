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
//                 ACTIVE_Attributes.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_Attributes_H
#define _HEADER_INCLUDED__SAGA_GUI__ACTIVE_Attributes_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/panel.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CACTIVE_Attributes : public wxPanel
{
	DECLARE_CLASS(CACTIVE_Attributes)

public:
	CACTIVE_Attributes(wxWindow *pParent);
	virtual ~CACTIVE_Attributes(void);

	void						On_Size				(wxSizeEvent     &event);

	void						On_Choice			(wxCommandEvent  &event);

	void						On_Apply			(wxCommandEvent  &event);
	void						On_Apply_UI			(wxUpdateUIEvent &event);
	void						On_Restore			(wxCommandEvent  &event);
	void						On_Restore_UI		(wxUpdateUIEvent &event);

	void						Set_Layer			(class CWKSP_Layer *pLayer);

	void						Set_Attributes		(void);

	void						Save_Changes		(bool bConfirm);


protected:

	int							m_btn_height;

	class wxButton				*m_Btn_Restore, *m_Btn_Apply;

	class wxChoice				*m_pSelections;

	class CSG_Table				*m_pAttributes;
	
	class CSG_Shapes			*m_pShapes;

	class CVIEW_Table_Control	*m_pControl;

	class CWKSP_Layer			*m_pLayer;


	void						_Set_Positions		(void);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_Attributes_H
