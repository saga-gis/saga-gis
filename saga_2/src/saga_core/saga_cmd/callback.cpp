
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
#include <wx/utils.h>

#include "callback.h"

#include "module_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static bool				g_bSilent	= false;

static CModule_Library	*g_pLibrary	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		Callback(TSG_UI_Callback_ID ID, long Param_1, long Param_2)
{
	static int	iBuisy		= 0;

	const SG_Char	Buisy[4]	= {	'|', '/', '-', '\\'	};

	int		Result;
	double	d1, d2;

	Result	= 1;

	//-----------------------------------------------------
	switch( ID )
	{
	default:

		Result	= 0;

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	//                                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_PROCESS_GET_OKAY:

		if( Param_1 != 0 )
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

		d1	= *((double *)Param_1);
		d2	= *((double *)Param_2);

		SG_PRINTF(SG_T("\r%3d%%"), d2 != 0.0 ? 1 + (int)(100.0 * d1 / d2) : 100);

		break;

	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_READY:

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_TEXT:

		SG_PRINTF(SG_T("\n%s\n"), (SG_Char *)Param_1);

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	//                                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD:

		SG_PRINTF(SG_T("\n%s\n"), (SG_Char *)Param_1);

		break;


	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD_ERROR:

		SG_PRINTF(SG_T("\n%s: %s\n"), LNG("error"), (SG_Char *)Param_1);

		break;


	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD_EXECUTION:

		SG_PRINTF(SG_T("\n%s\n"), (SG_Char *)Param_1);

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	//                                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DLG_MESSAGE:

		SG_PRINTF(SG_T("\n%s: %s\n"), (const SG_Char *)Param_2, (const SG_Char *)Param_1);

		break;


	//-----------------------------------------------------
	case CALLBACK_DLG_CONTINUE:

		Result	= Get_YesNo((const SG_Char *)Param_2, (const SG_Char *)Param_1);

		break;


	//-----------------------------------------------------
	case CALLBACK_DLG_ERROR:

		Result	= Get_YesNo((const SG_Char *)Param_2, (const SG_Char *)Param_1);

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	//                                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_CHECK:

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_ADD:

		Result	= 0;

		if( g_pLibrary )
		{
			Result	= g_pLibrary->Add_DataObject((CSG_Data_Object *)Param_1) ? 1 : 0;
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
	//                                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DLG_PARAMETERS:

		Result	= 0;

		if( g_pLibrary )
		{
			Result	= g_pLibrary->Get_Parameters((CSG_Parameters *)Param_1) ? 1 : 0;
		}

		break;
	}

	return( Result );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_PFNC_UI_Callback	Get_Callback	(void)
{
	return( &Callback );
}

//---------------------------------------------------------
void			Set_Library		(CModule_Library *pLibrary)
{
	g_pLibrary	= pLibrary;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void			Set_Silent		(bool bOn)
{
	g_bSilent	= bOn;
}

//---------------------------------------------------------
#ifdef _SAGA_MSW

#include <conio.h>

//---------------------------------------------------------
void			Get_Pause		(void)
{
	if( !g_bSilent )
	{
		SG_PRINTF(SG_T("\n%s..."), LNG("press any key"));

		_getch();
	}
}

//---------------------------------------------------------
bool			Get_YesNo		(const SG_Char *caption, const SG_Char *message)
{
	wxBell();

	SG_PRINTF(SG_T("\n%s: %s\n"), caption, message);

	if( !g_bSilent )
	{
		CSG_String	sKey, sYes(LNG("Y")), sNo(LNG("N"));

		SG_PRINTF(SG_T("%s? (%s/%s)"), LNG("continue"), sYes, sNo);

		do
		{
			sKey.Printf(SG_T("%c"), _getch());
		}
		while( sYes.CmpNoCase(sKey) && sNo.CmpNoCase(sKey) );

		return( sYes.CmpNoCase(sKey) == 0 );
	}

	return( true );
}

#else

//---------------------------------------------------------
void			Get_Pause		(void)
{
	if( !g_bSilent )
	{
		SG_PRINTF(SG_T("\n%s..."), LNG("press any key"));
	}
}

//---------------------------------------------------------
bool			Get_YesNo		(const SG_Char *caption, const SG_Char *message)
{
	return( true );
}

#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void			Print_Error		(const SG_Char *Error)
{
	SG_PRINTF(SG_T("\n%s: %s\n"), LNG("error"), Error);
}

//---------------------------------------------------------
void			Print_Error		(const SG_Char *Error, const SG_Char *Info)
{
	Print_Error(CSG_String::Format(SG_T("%s [%s]"), Error, Info));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
