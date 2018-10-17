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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _SAGA_MSW
#include <conio.h>
#endif

#include <wx/utils.h>

#include "callback.h"

#include "tool.h"


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
#define SG_XML_PROGRESS		SG_T("progress")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static CCMD_Tool	*g_pCMD_Tool	= NULL;

//---------------------------------------------------------
void			CMD_Set_Tool		(CCMD_Tool *pCMD_Tool)
{
	g_pCMD_Tool	= pCMD_Tool;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static bool		g_bShow_Messages	= true;

void			CMD_Set_Show_Messages	(bool bOn)	{	g_bShow_Messages	= bOn;		}

bool			CMD_Get_Show_Messages	(void)		{	return( g_bShow_Messages );		}

//---------------------------------------------------------
static bool		g_bShow_Progress	= true;

void			CMD_Set_Show_Progress	(bool bOn)	{	g_bShow_Progress	= bOn;		}

bool			CMD_Get_Show_Progress	(void)		{	return( g_bShow_Progress );		}

//---------------------------------------------------------
static bool		g_bInteractive		= false;

void			CMD_Set_Interactive	(bool bOn)		{	g_bInteractive		= bOn;		}

bool			CMD_Get_Interactive	(void)			{	return( g_bInteractive );		}

//---------------------------------------------------------
static bool		g_bXML				= false;

void			CMD_Set_XML			(bool bOn)		{	g_bXML				= bOn;		}

bool			CMD_Get_XML			(void)			{	return( g_bXML );				}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void			CMD_Print			(const CSG_String &Text, const CSG_String &XML_Tag)
{
	if( g_bXML )
	{
		if( !XML_Tag.is_Empty() )
		{
			SG_Printf("<%s>%s</%s>\n", XML_Tag, Text.c_str(), XML_Tag);
		}
	}
	else
	{
		SG_Printf("%s\n", Text.c_str());
	}
}

//---------------------------------------------------------
void			CMD_Print			(FILE *Stream, const CSG_String &Text, const CSG_String &XML_Tag)
{
	if( Stream )
	{
		if( g_bXML )
		{
			if( !XML_Tag.is_Empty() )
			{
				SG_FPrintf(Stream, "<%s>%s</%s>\n", XML_Tag, Text.c_str(), XML_Tag);
			}
		}
		else
		{
			SG_FPrintf(Stream, "%s\n", Text.c_str());
		}
	}
}

//---------------------------------------------------------
void			CMD_Print_Error		(const CSG_String &Error)
{
	CMD_Print(stderr, CSG_String::Format("%s: %s", _TL("Error"), Error.c_str()), SG_XML_ERROR);
}

//---------------------------------------------------------
void			CMD_Print_Error		(const CSG_String &Error, const CSG_String &Info)
{
	CMD_Print_Error(CSG_String::Format("%s [%s]", Error.c_str(), Info.c_str()));
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
		SG_Printf("%s...\n", _TL("press any key"));

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
		CSG_String	sKey, sYes("y"), sNo("n");

		SG_Printf("%s: %s\n", Caption.c_str(), Message.c_str());

		SG_Printf("%s? (%s/%s)\n", _TL("continue"), sYes.c_str(), sNo.c_str());

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
	static int		iBuisy		= 0;
	static int		iPercent	= -1;
	const SG_Char	Buisy[4]	= {	'|', '/', '-', '\\'	};

	int		Result	= 1;

	//-----------------------------------------------------
	switch( ID )
	{
	default:

		Result	= 0;

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_PROCESS_GET_OKAY:

		if( g_bShow_Progress && Param_1.True )
		{
			SG_Printf("\r%c   ", Buisy[iBuisy++]);

			iBuisy	%= 4;
		}

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_OKAY:

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_PROGRESS:

		if( g_bShow_Progress )
		{
			int	i	= Param_2.Number != 0.0 ? 1 + (int)(100.0 * Param_1.Number / Param_2.Number) : 100;

			if( i != iPercent )
			{
				if( g_bXML )
				{
					SG_Printf("<%s>%d</%s>\n", SG_XML_PROGRESS, iPercent = i, SG_XML_PROGRESS);
				}
				else
				{
					if( iPercent < 0 || i < iPercent )
					{
						SG_Printf("\n");
					}

					SG_Printf("\r%3d%%", iPercent = i);
				}
			}
		}

		break;

	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_READY:

		iPercent	= -1;

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

		Result	= CMD_Get_YesNo(Param_2.String.c_str(), Param_1.String.c_str());

		break;


	//-----------------------------------------------------
	case CALLBACK_DLG_ERROR:

		Result	= CMD_Get_YesNo(Param_2.String.c_str(), Param_1.String.c_str());

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_ADD:

		Result	= SG_Get_Data_Manager().Add((CSG_Data_Object *)Param_1.Pointer) ? 1 : 0;

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

		Result	= g_pCMD_Tool && g_pCMD_Tool->Get_Parameters((CSG_Parameters *)Param_1.Pointer) ? 1 : 0;

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
