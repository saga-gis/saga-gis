
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
//                   WKSP_Base_Item.h                    //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Item_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Item_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/event.h>
#include <wx/menu.h>
#include <wx/treectrl.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	WKSP_ITEM_Module_Manager,
	WKSP_ITEM_Module_Library,
	WKSP_ITEM_Module,

	WKSP_ITEM_Data_Manager,

	WKSP_ITEM_Table_Manager,
	WKSP_ITEM_Table,

	WKSP_ITEM_Shapes_Manager,
	WKSP_ITEM_Shapes_Type,
	WKSP_ITEM_Shapes,

	WKSP_ITEM_TIN_Manager,
	WKSP_ITEM_TIN,

	WKSP_ITEM_Grid_Manager,
	WKSP_ITEM_Grid_System,
	WKSP_ITEM_Grid,

	WKSP_ITEM_Map_Manager,
	WKSP_ITEM_Map,
	WKSP_ITEM_Map_Layer,

	WKSP_ITEM_Undefined
}
TWKSP_Item;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Base_Item : public wxTreeItemData
{
	friend class CWKSP_Base_Manager;

public:
	CWKSP_Base_Item(void);
	virtual ~CWKSP_Base_Item(void);

	virtual TWKSP_Item			Get_Type			(void)	= 0;

	virtual wxString			Get_Name			(void)	= 0;
	virtual wxString			Get_Description		(void)	= 0;

	virtual wxMenu *			Get_Menu			(void)	= 0;

	virtual bool				On_Command			(int Cmd_ID);
	virtual bool				On_Command_UI		(wxUpdateUIEvent &event);

	virtual class CSG_Parameters *	Get_Parameters		(void)	{	return( NULL );			}
	virtual void				Parameters_Changed	(void);

	class CWKSP_Base_Control *	Get_Control			(void);
	class CWKSP_Base_Manager *	Get_Manager			(void)	{	return( m_pManager );	}

	int							Get_ID				(void)	{	return( m_ID );			}
	int							Get_Index			(void);

	bool						is_Selected			(void);


protected:


private:

	bool						m_bManager;

	int							m_ID;

	class CWKSP_Base_Manager	*m_pManager;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Base_Item *				Get_Active_Item		(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Item_H
