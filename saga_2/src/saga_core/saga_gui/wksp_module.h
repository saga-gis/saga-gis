
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
//                    WKSP_Module.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_H


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
class CWKSP_Module : public CWKSP_Base_Item
{
public:
	CWKSP_Module(class CModule *pModule, const char *Menu_Path_default);
	virtual ~CWKSP_Module(void);

	virtual TWKSP_Item			Get_Type		(void)			{	return( WKSP_ITEM_Module );	}

	virtual wxString			Get_Name		(void);
	virtual wxString			Get_Description	(void);

	virtual wxMenu *			Get_Menu		(void);

	virtual bool				On_Command		(int Cmd_ID);

	virtual class CParameters *	Get_Parameters	(void);

	class CModule *				Get_Module		(void)			{	return( m_pModule );	}

	void						Set_Menu_ID		(int Menu_ID);
	int							Get_Menu_ID		(void)			{	return( m_Menu_ID );	}
	wxString &					Get_Menu_Path	(void)			{	return( m_Menu_Path );	}

	wxString &					Get_File_Name	(void);
	void						Set_File_Name	(wxString);
	bool						is_Interactive	(void);
	bool						is_Executing	(void);

	bool						Execute			(bool bDialog);
	bool						Execute			(CSG_Point ptWorld, TModule_Interactive_Mode Mode, int Keys);

	void						Make_Report		(FILE *Stream);


private:

	wxString					m_Menu_Path;

	wxString					m_File_Name;
	int							m_Menu_ID;

	class CModule				*m_pModule;


};

//---------------------------------------------------------
extern CWKSP_Module				*g_pModule;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Module_H
