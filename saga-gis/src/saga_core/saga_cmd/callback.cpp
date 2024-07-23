
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                   Program: SAGA_CMD                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Callback.cpp                       //
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
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
#ifdef _SAGA_MSW
#include <conio.h>
#endif

#include <wx/utils.h>

#include "callback.h"

#include "tool.h"

#include <iostream>
#include <stdio.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_XML_ERROR		SG_T("error")
#define SG_XML_WARNING		SG_T("warning")
#define SG_XML_MESSAGE		SG_T("message")
#define SG_XML_MESSAGE_PROC	SG_T("message-proc")
#define SG_XML_MESSAGE_EXEC	SG_T("message-exec")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static CCMD_Tool *g_pCMD_Tool = NULL;

//---------------------------------------------------------
void        CMD_Set_Tool          (CCMD_Tool *pCMD_Tool)
{
	g_pCMD_Tool = pCMD_Tool;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static bool g_bShow_Messages      = true;

bool        CMD_Get_Show_Messages (void)     { return( g_bShow_Messages ); }

bool        CMD_Set_Show_Messages (bool bOn) { bool bLast = g_bShow_Messages; g_bShow_Messages = bOn; return( bLast ); }

//---------------------------------------------------------
static bool g_bShow_Progress      = true;

bool        CMD_Get_Show_Progress (void)     { return( g_bShow_Progress ); }

bool        CMD_Set_Show_Progress (bool bOn) { bool bLast = g_bShow_Progress; g_bShow_Progress = bOn; return( bLast ); }

//---------------------------------------------------------
static bool g_bInteractive        = false;

bool        CMD_Get_Interactive   (void)     { return( g_bInteractive ); }

bool        CMD_Set_Interactive   (bool bOn) { bool bLast = g_bInteractive  ; g_bInteractive   = bOn; return( bLast ); }

//---------------------------------------------------------
static bool g_bXML                = false;

bool        CMD_Get_XML           (void)     { return( g_bXML ); }

bool        CMD_Set_XML           (bool bOn) { bool bLast = g_bXML          ; g_bXML           = bOn; return( bLast ); }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void			CMD_Print			(const CSG_String &Text, const CSG_String &XML_Tag)
{
	if( !g_bXML )
	{
		SG_UI_Console_Print_StdOut(Text.c_str(), '\n', true);
	}
	else if( !XML_Tag.is_Empty() )
	{
		SG_UI_Console_Print_StdOut(CSG_String::Format("<%s>%s</%s>", XML_Tag.c_str(), Text.c_str(), XML_Tag.c_str()), '\n', true);
	}
}

//---------------------------------------------------------
void			CMD_Print_Error		(const CSG_String &Text)
{
	if( !g_bXML )
	{
		SG_UI_Console_Print_StdErr(CSG_String::Format("[%s] %s", _TL("Error"), Text.c_str()), '\n', true);
	}
	else
	{
		SG_UI_Console_Print_StdErr(CSG_String::Format("<%s>%s</%s>", SG_XML_ERROR, Text.c_str(), SG_XML_ERROR), '\n', true);
	}
}

//---------------------------------------------------------
void			CMD_Print_Error		(const CSG_String &Text, const CSG_String &Info)
{
	CMD_Print_Error(CSG_String::Format("%s:\n%s", Text.c_str(), Info.c_str()));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void			CMD_Get_Pause		(void)
{
	if( g_bInteractive )
	{
		CMD_Print(CSG_String::Format("%s...", _TL("press any key")));

#ifdef _SAGA_MSW
		_getch();
#endif
	}
}

//---------------------------------------------------------
bool			CMD_Get_YesNo		(const CSG_String &Caption, const CSG_String &Message)
{
	if( g_bInteractive )
	{
#ifdef _SAGA_MSW
		CSG_String sKey, sYes("y"), sNo("n");

		CMD_Print(Caption + ": " + Message + "\n" + _TL("continue") + "? (" + sYes + "/" + sNo + ")");

		do
		{
			sKey.Printf("%c", _getch());
		}
		while( sYes.CmpNoCase(sKey) && sNo.CmpNoCase(sKey) );

		return( sYes.CmpNoCase(sKey) == 0 );
#endif
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		Callback(TSG_UI_Callback_ID ID, CSG_UI_Parameter &Param_1, CSG_UI_Parameter &Param_2)
{
	static int Progress = -1;

	int Result = 1;

	//-----------------------------------------------------
	switch( ID )
	{
	default:

		Result = 0;

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_PROCESS_GET_OKAY:

		if( g_bShow_Progress && Param_1.Boolean )
		{
			static int iBuisy = 0; static const SG_Char Buisy[4] = { '|', '/', '-', '\\' };

			SG_UI_Console_Print_StdOut(CSG_String::Format("\r%c", Buisy[iBuisy++]), '\0', true);

			iBuisy %= 4;
		}

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_OKAY:

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_PROGRESS:

		if( g_bShow_Progress && !g_bXML )
		{
			int i = Param_2.Number != 0. ? 1 + (int)(100. * Param_1.Number / Param_2.Number) : 100;

			if( Progress != i )
			{
				if( Progress < 0 || i < Progress )
				{
					SG_UI_Console_Print_StdOut("", '\n', true);
				}

				Progress = i;

				if( Progress >= 0 )
				{
					SG_UI_Console_Print_StdOut(CSG_String::Format("\r%3d%%", Progress > 100 ? 100 : Progress), '\0', true);
				}
			}
		}

		break;

	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_READY:

		if( Progress >= 0 )
		{
			SG_UI_Console_Print_StdOut("", '\n', true);
		}

		Progress = -1;

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_TEXT:

		if( g_bShow_Messages )
		{
			CMD_Print(Param_1.String, SG_XML_MESSAGE_PROC);
		}

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD:

		if( g_bShow_Messages )
		{
			CMD_Print(Param_1.String, SG_XML_MESSAGE);
		}

		break;


	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD_ERROR:

		CMD_Print_Error(Param_1.String);

		break;


	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD_EXECUTION:

		if( g_bShow_Messages )
		{
			CMD_Print(Param_1.String, SG_XML_MESSAGE_EXEC);
		}

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DLG_MESSAGE:

		if( g_bShow_Messages )
		{
			CMD_Print(Param_2.String + ": " + Param_1.String);
		}

		break;


	//-----------------------------------------------------
	case CALLBACK_DLG_CONTINUE:

		Result = CMD_Get_YesNo(Param_2.String.c_str(), Param_1.String.c_str());

		break;


	//-----------------------------------------------------
	case CALLBACK_DLG_ERROR:

		Result = CMD_Get_YesNo(Param_2.String.c_str(), Param_1.String.c_str());

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_ADD:

		Result = SG_Get_Data_Manager().Add((CSG_Data_Object *)Param_1.Pointer) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_UPDATE:

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_SHOW:

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_COLORS_GET:

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_COLORS_SET:

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_PARAMS_GET:

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_PARAMS_SET:

		break;

	//-----------------------------------------------------
	case CALLBACK_DLG_PARAMETERS:

		Result = g_pCMD_Tool && g_pCMD_Tool->Get_Parameters((CSG_Parameters *)Param_1.Pointer) ? 1 : 0;

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	}

	return( Result );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_PFNC_UI_Callback	CMD_Get_Callback	(void)
{
	return( &Callback );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
