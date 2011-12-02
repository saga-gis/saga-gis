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
//                 WKSP_Module_Manager.h                 //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_Manager_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_Manager_H


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
class CWKSP_Module_Manager : public CWKSP_Base_Manager
{
public:
	CWKSP_Module_Manager(void);
	virtual ~CWKSP_Module_Manager(void);

	bool							Initialise			(void);
	bool							Finalise			(void);

	virtual TWKSP_Item				Get_Type			(void)		{	return( WKSP_ITEM_Module_Manager );	}

	virtual wxString				Get_Name			(void);
	virtual wxString				Get_Description		(void);

	virtual wxMenu *				Get_Menu			(void);

	virtual bool					On_Command			(int Cmd_ID);
	virtual bool					On_Command_UI		(wxUpdateUIEvent &event);

	virtual void					Parameters_Changed	(void);

	void							On_Execute			(wxCommandEvent  &event);
	void							On_Execute_UI		(wxUpdateUIEvent &event);

	class CWKSP_Module_Library *	Get_Library			(int i)		{	return( (class CWKSP_Module_Library *)Get_Item(i) );	}

	class CWKSP_Module_Menu *		Get_Modules_Menu	(void)		{	return( m_pMenu );	}

	bool							Do_Beep				(void);

	void							Open				(void);
	bool							Open				(const wxString &File_Name);

	bool							Exists				(class CWKSP_Module *pModule);

	class CWKSP_Module *			Get_Module_byID		(int CMD_ID);


private:

	class CWKSP_Module_Menu			*m_pMenu;


	void							_Config_Read		(void);
	void							_Config_Write		(void);

	int								_Open_Directory		(const wxChar *sDirectory, bool bOnlySubDirectories = false);

};

//---------------------------------------------------------
extern CWKSP_Module_Manager			*g_pModules;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_Manager_H
