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
#include "wksp_data_item.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Table : public CWKSP_Data_Item
{
public:
	CWKSP_Table(class CSG_Table *pTable);
	virtual ~CWKSP_Table(void);

	virtual TWKSP_Item				Get_Type				(void)			{	return( WKSP_ITEM_Table );	}

	CSG_Table *						Get_Table				(void)			{	return( (CSG_Table *)m_pObject );	}

	virtual wxString				Get_Description			(void);

	virtual wxMenu *				Get_Menu				(void);

	virtual bool					On_Command				(int Cmd_ID);
	virtual bool					On_Command_UI			(wxUpdateUIEvent &event);

	class CVIEW_Table *				Get_View				(void)			{	return( m_pView );	}
	void							Set_View				(bool bShow);
	void							Toggle_View				(void);

	class CVIEW_Table_Diagram *		Get_Diagram				(void)			{	return( m_pDiagram );	}
	void							Set_Diagram				(bool bShow);
	void							Toggle_Diagram			(void);

	virtual bool					Show					(int Flags = 0);
	virtual bool					Update_Views			(bool bAll = true);
	virtual bool					View_Closes				(class wxMDIChildFrame *pView);


private:

	class CVIEW_Table				*m_pView;

	class CVIEW_Table_Diagram		*m_pDiagram;


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Table_H
