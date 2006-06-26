
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
//                     WKSP_Table.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Table_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Table_H


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
class CWKSP_Table : public CWKSP_Base_Item
{
public:
	CWKSP_Table(class CTable *pTable, CWKSP_Base_Item *pOwner);
	virtual ~CWKSP_Table(void);

	virtual TWKSP_Item			Get_Type			(void)			{	return( WKSP_ITEM_Table );	}

	virtual wxString			Get_Name			(void);
	virtual wxString			Get_Description		(void);

	virtual wxMenu *			Get_Menu			(void);

	virtual bool				On_Command			(int Cmd_ID);
	virtual bool				On_Command_UI		(wxUpdateUIEvent &event);

	CWKSP_Base_Item *			Get_Owner			(void)			{	return( m_pOwner );	}

	class CTable *				Get_Table			(void)			{	return( m_pTable );	}

	bool						Save				(void);
	bool						Save				(const char *File_Path);

	bool						DataObject_Changed	(CParameters *pParameters);

	class CVIEW_Table *			Get_View			(void)			{	return( m_pView );	}
	void						Set_View			(bool bShow);
	void						Toggle_View			(void);

	class CVIEW_Table_Diagram *	Get_Diagram			(void)			{	return( m_pDiagram );	}
	void						Set_Diagram			(bool bShow);
	void						Toggle_Diagram		(void);

	void						Update_Views		(void);
	void						View_Closes			(class wxMDIChildFrame *pView);


private:

	class CTable				*m_pTable;

	CWKSP_Base_Item				*m_pOwner;

	class CVIEW_Table			*m_pView;

	class CVIEW_Table_Diagram	*m_pDiagram;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Table_H
