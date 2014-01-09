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
//                    RES_Dialogs.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__RES_Dialogs_H
#define _HEADER_INCLUDED__SAGA_GUI__RES_Dialogs_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/string.h>
#include <wx/arrstr.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum ID_DIALOGS
{
	ID_DLG_CLOSE,
	ID_DLG_ABOUT,

	ID_DLG_DELETE,

	ID_DLG_WKSP_OPEN,
	ID_DLG_ALLFILES_OPEN,
	ID_DLG_MODULES_OPEN,
	ID_DLG_PROJECT_OPEN,
	ID_DLG_PROJECT_SAVE,
	ID_DLG_TABLES_OPEN,
	ID_DLG_TABLES_SAVE,
	ID_DLG_SHAPES_OPEN,
	ID_DLG_SHAPES_SAVE,
	ID_DLG_TIN_OPEN,
	ID_DLG_TIN_SAVE,
	ID_DLG_POINTCLOUD_OPEN,
	ID_DLG_POINTCLOUD_SAVE,
	ID_DLG_GRIDS_OPEN,
	ID_DLG_GRIDS_SAVE,

	ID_DLG_TEXT_OPEN,
	ID_DLG_TEXT_SAVE,

	ID_DLG_PARAMETERS_OPEN,
	ID_DLG_PARAMETERS_SAVE,

	ID_DLG_COLORS_OPEN,
	ID_DLG_COLORS_SAVE
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString			DLG_Get_Text			(int ID_DLG);
wxString			DLG_Get_Caption			(int ID_DLG);

wxString			DLG_Get_FILE_Caption	(int ID_DLG);
wxString			DLG_Get_FILE_Filter		(int ID_DLG);
wxString			DLG_Get_FILE_Config		(int ID_DLG);

//---------------------------------------------------------
class wxRect		DLG_Get_Def_Rect		(void);
class wxPoint		DLG_Get_Def_Position	(void);
class wxSize		DLG_Get_Def_Size		(void);

//---------------------------------------------------------
bool				DLG_Parameters			(class CSG_Parameters *pParameters, const wxString &Caption = wxT(""));

bool				DLG_Text				(const wxString &Caption, wxString &Text);
bool				DLG_Table				(const wxString &Caption, class CSG_Table *pTable);
bool				DLG_Table_Fields		(const wxString &Caption, class CSG_Parameter_Table_Fields *pFields);

bool				DLG_Login				(wxString &Username, wxString &Password, const wxString &Caption = wxT(""));

bool				DLG_List				(const wxString &Caption, class CSG_Parameter_List *pList);

bool				DLG_Colors				(class CSG_Colors *pColors);
bool				DLG_Colors				(int &Palette);
bool				DLG_Color				(long &Colour);

bool				DLG_Font				(class CSG_Parameter *pFont);

bool				DLG_Get_Text			(wxString &Value, const wxString &Caption, const wxString &Text);
bool				DLG_Get_Text			(wxString &Value);
bool				DLG_Get_Number			(double   &Value, const wxString &Caption, const wxString &Text);
bool				DLG_Get_Number			(double   &Value);
bool				DLG_Get_Number			(int      &Value, const wxString &Caption, const wxString &Text);
bool				DLG_Get_Number			(int      &Value);

bool				DLG_Directory			(wxString &Directory, const wxString &Caption);
bool				DLG_Save				(wxString &File_Path, int ID_DLG);
bool				DLG_Save				(wxString &File_Path, const wxString &Caption, const wxString &Filter);
bool				DLG_Open				(wxString &File_Path, int ID_DLG);
bool				DLG_Open				(wxString &File_Path, const wxString &Caption, const wxString &Filter);
bool				DLG_Open				(wxArrayString &File_Paths, int ID_DLG);
bool				DLG_Open				(wxArrayString &File_Paths, const wxString &Caption, const wxString &Filter);

bool				DLG_Image_Save			(wxString &File_Path, int &Type, const wxString &def_Dir = wxT(""), const wxString &def_File = wxT(""));

void				DLG_Message_Show		(int ID_DLG);
void				DLG_Message_Show		(const wxString &Message);
void				DLG_Message_Show		(const wxString &Message, const wxString &Caption);
int					DLG_Message_Show_Error	(int ID_DLG);
int					DLG_Message_Show_Error	(const wxString &Message, const wxString &Caption);
bool				DLG_Message_Confirm		(int ID_DLG);
bool				DLG_Message_Confirm		(const wxString &Message, const wxString &Caption);
int					DLG_Message_YesNoCancel	(int ID_DLG);
int					DLG_Message_YesNoCancel	(const wxString &Message, const wxString &Caption);

//---------------------------------------------------------
int					DLG_Maps_Add			(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__RES_Dialogs_H
