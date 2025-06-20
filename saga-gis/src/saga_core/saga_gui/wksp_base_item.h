
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Item_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Item_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/menu.h>
#include <wx/treectrl.h>

#include <saga_api/saga_api.h>

#include "view_base.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	WKSP_ITEM_Tool_Manager,
	WKSP_ITEM_Tool_Group,
	WKSP_ITEM_Tool_Library,
	WKSP_ITEM_Tool,

	WKSP_ITEM_Data_Manager,

	WKSP_ITEM_Table_Manager,
	WKSP_ITEM_Table,

	WKSP_ITEM_Shapes_Manager,
	WKSP_ITEM_Shapes_Type,
	WKSP_ITEM_Shapes,

	WKSP_ITEM_TIN_Manager,
	WKSP_ITEM_TIN,

	WKSP_ITEM_PointCloud_Manager,
	WKSP_ITEM_PointCloud,

	WKSP_ITEM_Grid_Manager,
	WKSP_ITEM_Grid_System,
	WKSP_ITEM_Grid,
	WKSP_ITEM_Grids,

	WKSP_ITEM_Map_Manager,
	WKSP_ITEM_Map,
	WKSP_ITEM_Map_Layer,
	WKSP_ITEM_Map_Graticule,
	WKSP_ITEM_Map_BaseMap,

	WKSP_ITEM_Undefined
}
TWKSP_Item;


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Base_Item : public wxTreeItemData
{
	friend class CWKSP_Base_Manager;

public:
	CWKSP_Base_Item(void);
	virtual ~CWKSP_Base_Item(void);

	virtual TWKSP_Item				Get_Type				(void)       { return( WKSP_ITEM_Undefined ); }

	virtual wxString				Get_Name				(void)       { return( wxEmptyString ); }
	virtual wxString				Get_Description			(void)       { return( wxEmptyString ); }

	const wxString &				Get_Unique_ID			(void);
	const wxString &				Set_Unique_ID			(const wxString &ID);
	bool							Cmp_Unique_ID			(const wxString &ID);

	virtual class wxToolBarBase *	Get_ToolBar				(void)       { return( NULL ); }
	virtual wxMenu *				Get_Menu				(void)       { return( NULL ); }

	virtual bool					On_Command				(int Cmd_ID);
	virtual bool					On_Command_UI			(wxUpdateUIEvent &event);

	virtual bool					On_Data_Deletion		(CSG_Data_Object *pObject);

	virtual CSG_Parameters *		Get_Parameters			(void);
	virtual CSG_Parameter *			Get_Parameter			(const CSG_String &Identifier);
	virtual void					Parameters_Changed		(void);

	class CWKSP_Base_Control *		Get_Control				(void);
	class CWKSP_Base_Manager *		Get_Manager				(void)       { return( m_pManager ); }

	int								Get_ID					(void)       { return( m_ID ); }
	int								Get_Index				(void);

	virtual bool					is_Manager				(void) const { return( false ); }

	bool							is_Selected				(void);

	static wxString					Get_Type_Name			(TWKSP_Item Type);


protected:

	CSG_Parameters					m_Parameters;


	virtual void					On_Create_Parameters	(void);

	virtual int						On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);


	static int						Parameter_Callback		(CSG_Parameter *pParameter, int Flags);


private:

	int								m_ID = 0;

	wxString						m_Unique_ID;

	CWKSP_Base_Manager				*m_pManager = NULL;


	bool							_On_Data_Deletion		(CSG_Parameters &Parameters, CSG_Data_Object *pObject);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Item_H
