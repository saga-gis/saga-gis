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
//                     wksp_tool.h                       //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Tool_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Tool_H


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
class CWKSP_Tool : public CWKSP_Base_Item
{
public:
	CWKSP_Tool(class CSG_Tool *pTool, const wxString &Menu_Library);
	virtual ~CWKSP_Tool(void);

	virtual TWKSP_Item				Get_Type			(void)			{	return( WKSP_ITEM_Tool );	}

	virtual wxString				Get_Name			(void);
	virtual wxString				Get_Description		(void);

	virtual wxMenu *				Get_Menu			(void);

	virtual bool					On_Command			(int Cmd_ID);

	virtual class CSG_Parameters *	Get_Parameters		(void);

	class CSG_Tool *				Get_Tool			(void)			{	return( m_pTool );	}

	void							Set_Menu_ID			(int Menu_ID);
	int								Get_Menu_ID			(void)			{	return( m_Menu_ID );	}

	bool							is_Interactive		(void);
	bool							is_Executing		(void);

	bool							Execute				(bool bDialog);
	bool							Execute				(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode, int Keys);


private:

	int								m_Menu_ID;

	class CSG_Tool				*m_pTool;


	void							_Save_to_Clipboard		(void);
	void							_Save_to_Script			(void);

	CSG_String						_Get_XML				(bool bHeader);
	void							_Get_XML				(CSG_MetaData &Tool, CSG_Parameters *pParameters, const CSG_String &Prefix = "");

	CSG_String						_Get_CMD				(bool bHeader, int Type = -1);
	void							_Get_CMD				(CSG_String &Command, CSG_Parameters *pParameters);

	CSG_String						_Get_Python				(bool bHeader);
	void							_Get_Python				(CSG_String &Command, CSG_Parameters *pParameters);

};

//---------------------------------------------------------
extern CWKSP_Tool					*g_pTool;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Tool_H
