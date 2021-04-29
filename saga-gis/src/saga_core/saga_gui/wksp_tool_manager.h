
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
//                  wksp_tool_manager.h                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Tool_Manager_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Tool_Manager_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include "wksp_base_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Tool_Manager : public CWKSP_Base_Manager
{
public:
	CWKSP_Tool_Manager(void);
	virtual ~CWKSP_Tool_Manager(void);

	bool							Initialise			(void);
	bool							Finalise			(void);

	virtual TWKSP_Item				Get_Type			(void)		{	return( WKSP_ITEM_Tool_Manager );	}

	virtual wxString				Get_Name			(void);
	virtual wxString				Get_Description		(void);

	virtual wxMenu *				Get_Menu			(void);

	virtual bool					On_Command			(int Cmd_ID);
	virtual bool					On_Command_UI		(wxUpdateUIEvent &event);

	virtual int						On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);
	virtual void					Parameters_Changed	(void);

	void							On_Execute			(wxCommandEvent  &event);
	void							On_Execute_UI		(wxUpdateUIEvent &event);

	class CWKSP_Tool_Group *		Get_Group			(int i)		{	return( (class CWKSP_Tool_Group *)Get_Item(i) );	}
	class CWKSP_Tool_Group *		Get_Group			(const wxString &Group);

	class CWKSP_Tool_Library *		Get_Library			(CSG_Tool_Library *pLibrary);

	bool							Update				(void);

	wxMenu *						Get_Menu_Tools		(void);
	void							Set_Recently_Used	(class CWKSP_Tool *pTool);

	bool							Do_Beep				(void);

	void							Open				(void);
	bool							Open				(const wxString &File_Name);

	bool							Exists				(class CWKSP_Tool *pTool);

	class CWKSP_Tool *				Get_Tool_byID		(int CMD_ID);


private:

	class CWKSP_Tool_Menu			*m_pMenu_Tools;


	bool							_Reload				(void);
	bool							_Update				(bool bSyncToCtrl);

};

//---------------------------------------------------------
extern CWKSP_Tool_Manager			*g_pTools;


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Tool_Group : public CWKSP_Base_Manager
{
public:
	CWKSP_Tool_Group(const wxString &Name);
	virtual ~CWKSP_Tool_Group(void);

	virtual TWKSP_Item				Get_Type			(void)		{	return( WKSP_ITEM_Tool_Group );	}

	virtual wxString				Get_Name			(void)		{	return( m_Name );	}
	virtual wxString				Get_Description		(void);

	virtual wxMenu *				Get_Menu			(void);

	virtual bool					On_Command			(int Cmd_ID);
	virtual bool					On_Command_UI		(wxUpdateUIEvent &event);

	bool							Add_Library			(CSG_Tool_Library *pLibrary);
	class CWKSP_Tool_Library *		Get_Library			(int i)		{	return( (class CWKSP_Tool_Library *)Get_Item(i) );	}
	class CWKSP_Tool_Library *		Get_Library			(CSG_Tool_Library *pLibrary);


private:

	wxString						m_Name;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Tool_Manager_H
