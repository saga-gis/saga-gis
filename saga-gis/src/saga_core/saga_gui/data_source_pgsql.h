
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
//                  data_source_pgsql.h                  //
//                                                       //
//          Copyright (C) 2013 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__data_source_pgsql__H
#define _HEADER_INCLUDED__SAGA_GUI__data_source_pgsql__H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/treectrl.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	PGSQL_Connect			(const CSG_String &Host, const CSG_String &Port, const CSG_String &DBName);
bool	PGSQL_is_Connected		(const CSG_String &Host, const CSG_String &Port, const CSG_String &DBName);
bool	PGSQL_is_Connected		(const CSG_String &Server);

bool	PGSQL_has_Connections	(double vPostGIS = 0.0);
bool	PGSQL_Get_Connections	(CSG_Strings &Servers, double vPostGIS = 0.0);

bool	PGSQL_Save_Table		(CSG_Table  *pTable);
bool	PGSQL_Save_Shapes		(CSG_Shapes *pShapes);
bool	PGSQL_Save_Grid			(CSG_Grid   *pGrid);
bool	PGSQL_Save_Grids		(CSG_Grids  *pGrids);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CData_Source_PgSQL : public wxTreeCtrl
{
public:
	CData_Source_PgSQL(wxWindow *pParent);
	virtual ~CData_Source_PgSQL(void);

	void						Autoconnect			(void);

	void						Update_Sources		(void);
	void						Update_Source		(const wxString &Server, const wxString &Username = "", const wxString &Password = "");


private:

	void						On_Item_Activated	(wxTreeEvent &event);
	void						On_Item_RClick		(wxTreeEvent &event);
	void						On_Item_Menu		(wxTreeEvent &event);

	void						On_Refresh			(wxCommandEvent &event);
	void						On_Source_Create	(wxCommandEvent &event);
	void						On_Source_Drop		(wxCommandEvent &event);
	void						On_Source_Open		(wxCommandEvent &event);
	void						On_Source_Close		(wxCommandEvent &event);
	void						On_Sources_Close	(wxCommandEvent &event);
	void						On_Source_Delete	(wxCommandEvent &event);
	void						On_Table_Open		(wxCommandEvent &event);
	void						On_Table_From_Query	(wxCommandEvent &event);
	void						On_Table_Rename		(wxCommandEvent &event);
	void						On_Table_Info		(wxCommandEvent &event);
	void						On_Table_Drop		(wxCommandEvent &event);

	bool						Source_Create		(const wxTreeItemId &Item);
	bool						Source_Drop			(const wxTreeItemId &Item);
	bool						Source_Open			(class CData_Source_PgSQL_Data *pData, bool bDialog);
	void						Source_Open			(const wxTreeItemId &Item);
	void						Source_Close		(const wxTreeItemId &Item, bool bDelete);
	void						Sources_Close		(void);
	void						Table_Open			(const wxTreeItemId &Item);
	void						Table_From_Query	(const wxTreeItemId &Item);
	void						Table_Rename		(const wxTreeItemId &Item);
	void						Table_Info			(const wxTreeItemId &Item);
	void						Table_Drop			(const wxTreeItemId &Item);

	wxTreeItemId				Get_Server_Item		(const wxString &Server, bool bCreate);
	wxTreeItemId				Find_Source			(const wxString &Server);
	void						Update_Item			(const wxTreeItemId &Item);
	void						Update_Sources		(const wxTreeItemId &Root);
	void						Update_Source		(const wxTreeItemId &Item);
	void						Append_Table		(const wxTreeItemId &Parent, const SG_Char *Name, int Type, int Image);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__data_source_pgsql__H
