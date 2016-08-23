/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                Command Line Interface                 //
//                                                       //
//                   Program: SAGA_CMD                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Callback.h                        //
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
#ifndef _HEADER_INCLUDED__SAGA_CMD__Callback_H
#define _HEADER_INCLUDED__SAGA_CMD__Callback_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void					CMD_Set_Tool			(class CCMD_Tool *pCMD_Tool);

//---------------------------------------------------------
void					CMD_Set_Show_Messages	(bool bOn);
bool					CMD_Get_Show_Messages	(void);

void					CMD_Set_Show_Progress	(bool bOn);
bool					CMD_Get_Show_Progress	(void);

void					CMD_Set_Interactive		(bool bOn);
bool					CMD_Get_Interactive		(void);

void					CMD_Set_XML				(bool bOn);
bool					CMD_Get_XML				(void);

//---------------------------------------------------------
void					CMD_Print				(              const CSG_String &Text, const SG_Char *XML_Tag = NULL);
void					CMD_Print				(FILE *Stream, const CSG_String &Text, const SG_Char *XML_Tag = NULL);

void					CMD_Print_Error			(const CSG_String &Error);
void					CMD_Print_Error			(const CSG_String &Error, const CSG_String &Info);

//---------------------------------------------------------
void					CMD_Get_Pause			(void);
bool					CMD_Get_YesNo			(const CSG_String &Caption, const CSG_String &Message);

//---------------------------------------------------------
TSG_PFNC_UI_Callback	CMD_Get_Callback		(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_CMD__Callback_H
