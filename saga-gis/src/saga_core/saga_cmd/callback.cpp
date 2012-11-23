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
#ifdef _SAGA_MSW
#include <conio.h>
#endif

#include <wx/utils.h>

#include "callback.h"

#include "module_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static CCMD_Module	*g_pCMD_Module	= NULL;

//---------------------------------------------------------
void			CMD_Set_Module		(CCMD_Module *pCMD_Module)
{
	g_pCMD_Module	= pCMD_Module;
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

void			CMD_Set_Interactive	(bool bOn)		{	g_bInteractive	= bOn;			}

bool			CMD_Get_Interactive	(void)			{	return( g_bInteractive );		}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void			CMD_Print_Error		(const SG_Char *Error)
{
	SG_FPRINTF(stderr, SG_T("\n%s: %s\n"), _TL("error"), Error);
}

//---------------------------------------------------------
void			CMD_Print_Error		(const SG_Char *Error, const SG_Char *Info)
{
	CMD_Print_Error(CSG_String::Format(SG_T("%s [%s]"), Error, Info));
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
		SG_PRINTF(SG_T("\n%s..."), _TL("press any key"));

#ifdef _SAGA_MSW
		_getch();
#endif
	}
}

//---------------------------------------------------------
bool			CMD_Get_YesNo		(const SG_Char *caption, const SG_Char *message)
{
	if( g_bInteractive )
	{
#ifdef _SAGA_MSW
		CSG_String	sKey, sYes(SG_T("y")), sNo(SG_T("n"));

		SG_PRINTF(SG_T("\n%s: %s\n"), caption, message);

		SG_PRINTF(SG_T("%s? (%s/%s)"), _TL("continue"), sYes.c_str(), sNo.c_str());

		do
		{
			sKey.Printf(SG_T("%c"), _getch());
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
			SG_PRINTF(SG_T("\r%c   "), Buisy[iBuisy++]);

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
				SG_PRINTF(SG_T("\r%3d%%"), iPercent = i);
			}
		}

		break;

	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_READY:

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_TEXT:

		if( g_bShow_Messages )
		{
			SG_PRINTF(SG_T("\n%s\n"), Param_1.String.c_str());
		}

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD:

		if( g_bShow_Messages )
		{
			SG_PRINTF(SG_T("\n%s\n"), Param_1.String.c_str());
		}

		break;


	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD_ERROR:

		CMD_Print_Error(Param_1.String.c_str());

		break;


	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD_EXECUTION:

		if( g_bShow_Messages )
		{
			SG_PRINTF(SG_T("\n%s\n"), Param_1.String.c_str());
		}

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DLG_MESSAGE:

		if( g_bShow_Messages )
		{
			SG_PRINTF(SG_T("\n%s: %s\n"), Param_2.String.c_str(), Param_1.String.c_str());
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
	case CALLBACK_DATAOBJECT_CHECK:

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_ADD:

		Result	= 0;

		if( g_pCMD_Module )
		{
			Result	= g_pCMD_Module->Add_DataObject((CSG_Data_Object *)Param_1.Pointer) ? 1 : 0;
		}

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


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DLG_PARAMETERS:

		Result	= 0;

		if( g_pCMD_Module )
		{
			Result	= g_pCMD_Module->Get_Parameters((CSG_Parameters *)Param_1.Pointer) ? 1 : 0;
		}

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
