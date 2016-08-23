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
//                  wksp_tool_library.h                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Tool_Library_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Tool_Library_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_base_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Tool_Library : public CWKSP_Base_Manager
{
public:
	CWKSP_Tool_Library(class CSG_Tool_Library *pLibrary);
	virtual ~CWKSP_Tool_Library(void);

	virtual TWKSP_Item				Get_Type		(void)		{	return( WKSP_ITEM_Tool_Library );	}

	virtual wxString				Get_Name		(void);
	virtual wxString				Get_Description	(void);

	virtual wxMenu *				Get_Menu		(void);

	virtual bool					On_Command		(int Cmd_ID);
	virtual bool					On_Command_UI	(wxUpdateUIEvent &event);

	class CSG_Tool_Library *		Get_Library		(void)		{	return( m_pLibrary );	}

	void							Update			(void);

	class CWKSP_Tool *			Get_Tool		(int i)		{	return( (class CWKSP_Tool *)Get_Item(i) );	}
	class CWKSP_Tool *			Get_Tool		(class CWKSP_Tool *pTool);
	class CWKSP_Tool *			Get_Tool_byID	(int CMD_ID);

	bool							is_Valid		(void);

	wxString						Get_File_Name	(void);


private:

	class CSG_Tool_Library		*m_pLibrary;


	void							_Add_Tools	(void);
	void							_Del_Tools	(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Tool_Library_H
