
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__RES_Dialogs_H
#define _HEADER_INCLUDED__SAGA_GUI__RES_Dialogs_H


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
	ID_DLG_MODULES_OPEN,
	ID_DLG_PROJECT_OPEN,
	ID_DLG_PROJECT_SAVE,
	ID_DLG_TABLES_OPEN,
	ID_DLG_TABLES_SAVE,
	ID_DLG_SHAPES_OPEN,
	ID_DLG_SHAPES_SAVE,
	ID_DLG_TIN_OPEN,
	ID_DLG_TIN_SAVE,
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
const char *		DLG_Get_Text			(int ID_DLG);
const char *		DLG_Get_Caption			(int ID_DLG);

const char *		DLG_Get_FILE_Caption	(int ID_DLG);
const char *		DLG_Get_FILE_Filter		(int ID_DLG);
const char *		DLG_Get_FILE_Config		(int ID_DLG);

//---------------------------------------------------------
class wxRect		DLG_Get_Def_Rect		(void);
class wxPoint		DLG_Get_Def_Position	(void);
class wxSize		DLG_Get_Def_Size		(void);

//---------------------------------------------------------
bool				DLG_Parameters			(class CSG_Parameters *pParameters);

bool				DLG_Text				(const char *Caption, class wxString &Text);
bool				DLG_Table				(const char *Caption, class CSG_Table *pTable);

bool				DLG_List_Grid			(const char *Caption, class CSG_Parameter_Grid_List   *pList);
bool				DLG_List_Table			(const char *Caption, class CSG_Parameter_Table_List  *pList);
bool				DLG_List_Shapes			(const char *Caption, class CSG_Parameter_Shapes_List *pList);
bool				DLG_List_TIN			(const char *Caption, class CSG_Parameter_TIN_List    *pList);

bool				DLG_Colors				(class CSG_Colors *pColors);
bool				DLG_Colors				(int &Palette);
bool				DLG_Color				(long &Colour);

bool				DLG_Font				(class wxFont *pFont, long &Colour);

bool				DLG_Get_Number			(double &Number, const char *Caption, const char *Text);
bool				DLG_Get_Number			(double &Number);
bool				DLG_Get_Number			(int    &Number, const char *Caption, const char *Text);
bool				DLG_Get_Number			(int    &Number);

bool				DLG_Directory			(class wxString &Directory, const char *Caption);
bool				DLG_Save				(class wxString &File_Path, int ID_DLG);
bool				DLG_Save				(class wxString &File_Path, const char *Caption, const char *Filter);
bool				DLG_Open				(class wxString &File_Path, int ID_DLG);
bool				DLG_Open				(class wxString &File_Path, const char *Caption, const char *Filter);
bool				DLG_Open				(class wxArrayString &File_Paths, int ID_DLG);
bool				DLG_Open				(class wxArrayString &File_Paths, const char *Caption, const char *Filter);

bool				DLG_Image_Save			(class wxString &File_Path, int &Type, const char *def_Dir = "", const char *def_File = "");

void				DLG_Message_Show		(int ID_DLG);
void				DLG_Message_Show		(const char *Message);
void				DLG_Message_Show		(const char *Message, const char *Caption);
int					DLG_Message_Show_Error	(int ID_DLG);
int					DLG_Message_Show_Error	(const char *Message, const char *Caption);
bool				DLG_Message_Confirm		(int ID_DLG);
bool				DLG_Message_Confirm		(const char *Message, const char *Caption);

//---------------------------------------------------------
int					DLG_Maps_Add			(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__RES_Dialogs_H
