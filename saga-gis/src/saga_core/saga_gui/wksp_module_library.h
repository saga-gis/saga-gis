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
//                 WKSP_Module_Library.h                 //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_Library_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_Library_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dynlib.h>

#include "wksp_base_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Module_Library : public CWKSP_Base_Manager
{
public:
	CWKSP_Module_Library(const wxChar *File_Name);
	virtual ~CWKSP_Module_Library(void);

	virtual TWKSP_Item				Get_Type		(void)		{	return( WKSP_ITEM_Module_Library );	}

	virtual wxString				Get_Name		(void);
	virtual wxString				Get_Description	(void);

	virtual wxMenu *				Get_Menu		(void);

	virtual bool					On_Command		(int Cmd_ID);
	virtual bool					On_Command_UI	(wxUpdateUIEvent &event);

	class CWKSP_Module *			Get_Module		(int i)		{	return( (class CWKSP_Module *)Get_Item(i) );	}
	class CWKSP_Module *			Get_Module_byID	(int CMD_ID);

	bool							is_Valid		(void);

	wxString &						Get_File_Name	(void);

	const wxChar *					Get_Info		(int Type);

	bool							Exists			(class CWKSP_Module *pModule);


private:

	class CSG_Module_Library_Interface	*m_pInterface;

	wxString						m_File_Name;

	wxDynamicLibrary				m_Library;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_Library_H
