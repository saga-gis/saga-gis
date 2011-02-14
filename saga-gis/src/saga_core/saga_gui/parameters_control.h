
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
//                 Parameters_Control.h                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__Parameters_Control_H
#define _HEADER_INCLUDED__SAGA_GUI__Parameters_Control_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/toolbar.h>
#include <wx/stattext.h>
#include <wx/dcclient.h>
#include <wx/button.h>
#include <wx/propgrid/propgrid.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_Control : public wxPanel
{
	DECLARE_CLASS(CParameters_Control)

public:
	CParameters_Control(wxWindow *pParent, bool bDialog = false);
	virtual ~CParameters_Control(void);

	void						On_Size				(wxSizeEvent &event);
	void						On_PG_Selected		(wxPropertyGridEvent &event);
	void						On_PG_Changed		(wxPropertyGridEvent &event);

	bool						Update_DataObjects	(void);

	bool						Set_Parameters		(class CSG_Parameters *pParameters);
	class CSG_Parameters *		Get_Parameters		(void)	{	return( m_pParameters );	}

	bool						is_Modified			(void)	{	return( m_bModified );	}

	bool						Save_Changes		(bool bSilent);
	bool						Restore				(void);

	bool						Load				(void);
	bool						Save				(void);


private:

	bool						m_bModified;

	class CSG_Parameters		*m_pParameters, *m_pOriginal;

	class wxPropertyGrid		*m_pPG;

	class wxPropertyGridManager	*m_pPGM;


	void						_Add_Properties		(class CSG_Parameters *pParameters);
	void						_Add_Property		(class wxPGProperty *pParent, class CSG_Parameter *pParameter);
	class wxPGProperty *		_Get_Property		(class wxPGProperty *pParent, class CSG_Parameter *pParameter);

	void						_Set_Parameter		(const wxString &Identifier);
	void						_Update_Parameters	(void);
	void						_Update_Parameter	(class CSG_Parameter *pParameter);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__Parameters_Control_H
