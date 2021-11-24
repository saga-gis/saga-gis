
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
//                        WKSP.h                         //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/notebook.h>

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP : public wxNotebook
{
public:
	CWKSP(wxWindow *pParent);
	virtual ~CWKSP(void);

	void							Add_Pages				(void);

	void							On_Page_Changing		(wxNotebookEvent &event);
	void							On_Page_Changed			(wxNotebookEvent &event);

	void							On_Command				(wxCommandEvent  &event);
	void							On_Command_UI			(wxUpdateUIEvent &event);
	void							On_Command_Tool			(wxCommandEvent  &event);
	void							On_Command_UI_Tool		(wxUpdateUIEvent &event);

	class CWKSP_Tool_Control *		Get_Tools				(void)	{	return( m_pTools        );	}
	class CWKSP_Data_Control *		Get_Data				(void)	{	return( m_pData         );	}
	class CWKSP_Data_Buttons *		Get_Data_Buttons		(void)	{	return( m_pData_Buttons );	}
	class CWKSP_Map_Control *		Get_Maps				(void)	{	return( m_pMaps         );	}
	class CWKSP_Map_Buttons *		Get_Maps_Buttons		(void)	{	return( m_pMaps_Buttons );	}

	bool							Open					(void);
	bool							Open					(const wxString &File_Name);


private:

	class CWKSP_Tool_Control		*m_pTools;

	class CWKSP_Data_Control		*m_pData;

	class CWKSP_Data_Buttons		*m_pData_Buttons;

	class CWKSP_Map_Control			*m_pMaps;

	class CWKSP_Map_Buttons			*m_pMaps_Buttons;


	DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
extern CWKSP *g_pWKSP;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_H
