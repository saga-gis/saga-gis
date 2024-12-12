
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
//                 WKSP_Data_Menu_File.h                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Menu_File_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Menu_File_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/arrstr.h>
#include <wx/menu.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Data_Menu_File
{
public:
	CWKSP_Data_Menu_File(void);
	virtual ~CWKSP_Data_Menu_File(void);

	enum class Recent_Type
	{
		Folder, Project, Data, Undefined
	};

	wxMenu *					Create		(Recent_Type Type);
	void						Destroy		(void);

	void						Update		(void);

	void						Add			(const wxString &File, bool bUpdate = false);
	void						Del			(const wxString &File, bool bUpdate = false);
	bool						Get			(wxArrayString &Files, bool bAppend);
	int							Count		(void);

	bool						Open		(int CmdID);


private:

	Recent_Type					m_Type = Recent_Type::Undefined;

	size_t						m_Offset = 0;
	
	int							m_CmdID[2] = { 0, 0 };

	wxString					m_Group;

	wxArrayString				m_Files;

	wxMenu						*m_pMenu = NULL;

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Data_Menu_File_H
