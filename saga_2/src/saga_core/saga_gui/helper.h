
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
//                       Helper.h                        //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__Helper_H
#define _HEADER_INCLUDED__SAGA_GUI__Helper_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/colour.h>
#include <wx/settings.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class wxString		Get_SignificantDecimals_String	(double Value, int maxDecimals = 6);

//---------------------------------------------------------
double				Degree_To_Decimal				(double Deg, double Min, double Sec);
void				Decimal_To_Degree				(double Value, double &Deg, double &Min, double &Sec);

//---------------------------------------------------------
wxString			Get_FilePath_Relative			(const char *Directory, const char *FileName);
wxString			Get_FilePath_Absolute			(const char *Directory, const char *FileName);

//---------------------------------------------------------
wxString			Get_TableInfo_asHTML			(class CSG_Table *pTable);

//---------------------------------------------------------
wxColour			Get_Color_asWX					(int Color);
int					Get_Color_asInt					(wxColour Color);

wxColour			SYS_Get_Color					(wxSystemColour index);
void				SYS_Set_Color_BG				(class wxWindow *pWindow, wxSystemColour index);
void				SYS_Set_Color_BG_Window			(wxWindow *pWindow);

//---------------------------------------------------------
class wxWindow *	MDI_Get_Frame					(void);

wxPoint				MDI_Get_Def_Position			(void);
wxSize				MDI_Get_Def_Size				(void);

//---------------------------------------------------------
void				Set_Buisy_Cursor				(bool bOn);

void				Do_Beep							(int Style = 0);

//---------------------------------------------------------
void				MSG_General_Add_Line			(void);
void				MSG_General_Add					(const char *Message, bool bNewLine = true, bool bTime = false);

void				MSG_Error_Add_Line				(void);
void				MSG_Error_Add					(const char *Message, bool bNewLine = true, bool bTime = true);

void				MSG_Execution_Add_Line			(void);
void				MSG_Execution_Add				(const char *Message, bool bNewLine = true, bool bTime = false);

//---------------------------------------------------------
bool				CONFIG_Read						(const char *Group, const char *Entry, class wxString &Value);
bool				CONFIG_Read						(const char *Group, const char *Entry, long           &Value);
bool				CONFIG_Read						(const char *Group, const char *Entry, double         &Value);
bool				CONFIG_Read						(const char *Group, const char *Entry, bool           &Value);

bool				CONFIG_Write					(const char *Group, const char *Entry, const char     *Value);
bool				CONFIG_Write					(const char *Group, const char *Entry, long            Value);
bool				CONFIG_Write					(const char *Group, const char *Entry, double          Value);
bool				CONFIG_Write					(const char *Group, const char *Entry, bool            Value);

bool				CONFIG_Delete					(const char *Group);
bool				CONFIG_Delete					(const char *Group, const char *Entry);

//---------------------------------------------------------
bool				PROCESS_is_Executing			(void);

bool				PROCESS_Wait					(void);

bool				PROCESS_Get_Okay				(bool bBlink);
bool				PROCESS_Set_Okay				(bool bOkay = true);

bool				PROGRESSBAR_Set_Position		(int Position);
bool				PROGRESSBAR_Set_Position		(double Position, double Range);

//---------------------------------------------------------
bool				Open_WebBrowser					(const char *HRef);

//---------------------------------------------------------
enum
{
	STATUSBAR_DEFAULT	= 0,
	STATUSBAR_ACTIVE,
	STATUSBAR_VIEW_X,
	STATUSBAR_VIEW_Y,
	STATUSBAR_VIEW_Z,
	STATUSBAR_PROGRESS,
	STATUSBAR_COUNT
};

//---------------------------------------------------------
void				STATUSBAR_Set_Text				(const char *Text, int iPane = 0);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__Helper_H
