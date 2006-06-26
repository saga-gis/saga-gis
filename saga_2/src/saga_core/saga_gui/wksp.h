
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/notebook.h>


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

	void							On_Command				(wxCommandEvent &event);
	void							On_Command_UI			(wxUpdateUIEvent &event);
	void							On_Command_Module		(wxCommandEvent &event);
	void							On_Command_UI_Module	(wxUpdateUIEvent &event);

	class CWKSP_Module_Control *	Get_Modules				(void)	{	return( m_pModules );	}
	class CWKSP_Data_Control *		Get_Data				(void)	{	return( m_pData );		}
	class CWKSP_Map_Control *		Get_Maps				(void)	{	return( m_pMaps );		}

	bool							Open					(void);
	bool							Open					(const char *File_Name);


private:

	class CWKSP_Module_Control		*m_pModules;

	class CWKSP_Data_Control		*m_pData;

	class CWKSP_Map_Control			*m_pMaps;


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
extern CWKSP						*g_pWKSP;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_H
