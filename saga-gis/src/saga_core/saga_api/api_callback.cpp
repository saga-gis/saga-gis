/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   api_callback.cpp                    //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
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
#include <wx/stdpaths.h>

#include "api_core.h"
#include "grid.h"
#include "parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_PFNC_UI_Callback	gSG_UI_Callback	= NULL;

//---------------------------------------------------------
bool					SG_Set_UI_Callback(TSG_PFNC_UI_Callback Function)
{
	gSG_UI_Callback	= Function;

	return( true );
}

//---------------------------------------------------------
TSG_PFNC_UI_Callback	SG_Get_UI_Callback(void)
{
	return( gSG_UI_Callback );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

CSG_UI_Parameter::CSG_UI_Parameter(const CSG_UI_Parameter &Copy)
{
	True	= Copy.True;
	Number	= Copy.Number;
	Pointer	= Copy.Pointer;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int					gSG_UI_Progress_Lock	= 0;

//---------------------------------------------------------
int			SG_UI_Progress_Lock(bool bOn)
{
	if( bOn )
	{
		gSG_UI_Progress_Lock++;
	}
	else if( gSG_UI_Progress_Lock > 0 )
	{
		gSG_UI_Progress_Lock--;
	}

	return( gSG_UI_Progress_Lock );
}

//---------------------------------------------------------
bool		SG_UI_Process_Get_Okay(bool bBlink)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter	p1(gSG_UI_Progress_Lock == 0 && bBlink), p2;

		return( gSG_UI_Callback(CALLBACK_PROCESS_GET_OKAY, p1, p2) != 0 );
	}

	if( gSG_UI_Progress_Lock == 0 && bBlink )
	{
		static const SG_Char	Buisy[4]	= {	'|', '/', '-', '\\'	};

		static int	iBuisy	= 0;

		SG_PRINTF(SG_T("\r%c   "), Buisy[iBuisy++]);

		iBuisy	%= 4;
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Okay(bool bOkay)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter	p1(bOkay), p2;

		return( gSG_UI_Callback(CALLBACK_PROCESS_SET_OKAY, p1, p2) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Progress(double Position, double Range)
{
	if( gSG_UI_Progress_Lock > 0 )
	{
		return( SG_UI_Process_Get_Okay() );
	}

	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter	p1(Position), p2(Range);

		return( gSG_UI_Callback(CALLBACK_PROCESS_SET_PROGRESS, p1, p2) != 0 );
	}

	//-----------------------------------------------------
	static int	iPercent	= -1;

	int	i	= Position < 0.0 ? -1 : Range > 0.0 ? 1 + (int)(100.0 * Position / Range) : 100;

	if( iPercent != i )
	{
		if( iPercent < 0 || i < iPercent )
		{
			SG_PRINTF(SG_T("\n"));
		}

		iPercent	= i;

		if( iPercent >= 0 )
		{
			SG_PRINTF(SG_T("\r%3d%%"), iPercent > 100 ? 100 : iPercent);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Ready(void)
{
	if( gSG_UI_Callback )
	{
		if( gSG_UI_Progress_Lock == 0 )
		{
			CSG_UI_Parameter	p1, p2;

			return( gSG_UI_Callback(CALLBACK_PROCESS_SET_READY, p1, p2) != 0 );
		}
	}

	SG_UI_Process_Set_Progress(-1, -1);

	return( true );
}

//---------------------------------------------------------
void		SG_UI_Process_Set_Text(const CSG_String &Text)
{
	if( gSG_UI_Progress_Lock == 0 )
	{
		if( gSG_UI_Callback )
		{
			CSG_UI_Parameter	p1(Text), p2;

			gSG_UI_Callback(CALLBACK_PROCESS_SET_TEXT, p1, p2);
		}
		else
		{
			SG_PRINTF(SG_T("%s\n"), Text.c_str());
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_UI_Stop_Execution(bool bDialog)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter	p1(bDialog), p2;

		return( gSG_UI_Callback(CALLBACK_STOP_EXECUTION, p1, p2) != 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		SG_UI_Dlg_Message(const CSG_String &Message, const CSG_String &Caption)
{
	if( gSG_UI_Progress_Lock == 0 )
	{
		if( gSG_UI_Callback )
		{
			CSG_UI_Parameter	p1(Message), p2(Caption);

			gSG_UI_Callback(CALLBACK_DLG_MESSAGE, p1, p2);
		}
		else
		{
			SG_PRINTF(SG_T("%s: %s\n"), Caption.c_str(), Message.c_str());
		}
	}
}

//---------------------------------------------------------
bool		SG_UI_Dlg_Continue(const CSG_String &Message, const CSG_String &Caption)
{
	if( gSG_UI_Progress_Lock == 0 )
	{
		if( gSG_UI_Callback )
		{
			CSG_UI_Parameter	p1(Message), p2(Caption);

			return( gSG_UI_Callback(CALLBACK_DLG_CONTINUE, p1, p2) != 0 );
		}
	}

	return( true );
}

//---------------------------------------------------------
int			SG_UI_Dlg_Error(const CSG_String &Message, const CSG_String &Caption)
{
	if( gSG_UI_Progress_Lock != 0 )
	{
		SG_UI_Msg_Add_Error(Caption);
		SG_UI_Msg_Add_Error(Message);

		return( 0 );
	}

	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter	p1(Message), p2(Caption);

		return( gSG_UI_Callback(CALLBACK_DLG_ERROR, p1, p2) );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_UI_Dlg_Parameters(CSG_Parameters *pParameters, const CSG_String &Caption)
{
	if( gSG_UI_Callback && pParameters )
	{
		CSG_UI_Parameter	p1(pParameters), p2(Caption);

		return( gSG_UI_Callback(CALLBACK_DLG_PARAMETERS, p1, p2) != 0 );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int			gSG_UI_Msg_Lock	= 0;

//---------------------------------------------------------
int			SG_UI_Msg_Lock(bool bOn)
{
	if( bOn )
	{
		gSG_UI_Msg_Lock++;
	}
	else if( gSG_UI_Msg_Lock > 0 )
	{
		gSG_UI_Msg_Lock--;
	}

	return( gSG_UI_Msg_Lock );
}

//---------------------------------------------------------
void		SG_UI_Msg_Add(const CSG_String &Message, bool bNewLine, TSG_UI_MSG_STYLE Style)
{
	if( gSG_UI_Msg_Lock )
		return;

	if( gSG_UI_Callback )
	{
		int		Parameters[2];

		Parameters[0]	= bNewLine ? 1 : 0;
		Parameters[1]	= Style;

		CSG_UI_Parameter	p1(Message), p2(Parameters);

		gSG_UI_Callback(CALLBACK_MESSAGE_ADD, p1, p2);
	}
	else
	{
		SG_PRINTF(SG_T("%s\n"), Message.c_str());
	}
}

//---------------------------------------------------------
void		SG_UI_Msg_Add_Error(const CSG_String &Message)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter	p1(Message), p2;

		gSG_UI_Callback(CALLBACK_MESSAGE_ADD_ERROR, p1, p2);
	}
	else
	{
		SG_FPRINTF(stderr, SG_T("%s: %s\n"), _TL("Error"), Message.c_str());
	}
}

//---------------------------------------------------------
void		SG_UI_Msg_Add_Execution(const CSG_String &Message, bool bNewLine, TSG_UI_MSG_STYLE Style)
{
	if( gSG_UI_Msg_Lock )
		return;

	if( gSG_UI_Callback )
	{
		int		Parameters[2];

		Parameters[0]	= bNewLine ? 1 : 0;
		Parameters[1]	= Style;

		CSG_UI_Parameter	p1(Message), p2(Parameters);

		gSG_UI_Callback(CALLBACK_MESSAGE_ADD_EXECUTION, p1, p2);
	}
	else
	{
		SG_PRINTF(SG_T("%s\n"), Message.c_str());
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		SG_UI_ProgressAndMsg_Lock	(bool bOn)
{
	SG_UI_Progress_Lock(bOn);
	SG_UI_Msg_Lock     (bOn);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_UI_DataObject_Add(CSG_Data_Object *pDataObject, int Show)
{
	if( gSG_UI_Callback && pDataObject )
	{
		CSG_UI_Parameter	p1(pDataObject), p2(Show ? true : false);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_ADD, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Update(CSG_Data_Object *pDataObject, int Show, CSG_Parameters *pParameters)
{
	if( gSG_UI_Callback && pDataObject )
	{
		CSG_UI_Parameter	p1(pDataObject), p2(pParameters);

		if( gSG_UI_Callback(CALLBACK_DATAOBJECT_UPDATE, p1, p2) != 0 )
		{
			if( Show != SG_UI_DATAOBJECT_UPDATE_ONLY )
			{
				SG_UI_DataObject_Show(pDataObject, Show);
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Show(CSG_Data_Object *pDataObject, int Show)
{
	if( gSG_UI_Callback && pDataObject )
	{
		CSG_UI_Parameter	p1(pDataObject), p2(Show);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_SHOW, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_asImage(CSG_Data_Object *pDataObject, CSG_Grid *pGrid)
{
	if( gSG_UI_Callback && pDataObject )
	{
		CSG_UI_Parameter	p1(pDataObject), p2(pGrid);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_ASIMAGE, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Colors_Get(CSG_Data_Object *pDataObject, CSG_Colors *pColors)
{
	if( gSG_UI_Callback && pDataObject && pColors )
	{
		CSG_UI_Parameter	p1(pDataObject), p2(pColors);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_COLORS_GET, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Colors_Set(CSG_Data_Object *pDataObject, CSG_Colors *pColors)
{
	if( gSG_UI_Callback && pDataObject && pColors )
	{
		CSG_UI_Parameter	p1(pDataObject), p2(pColors);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_COLORS_SET, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Params_Get	(CSG_Data_Object *pDataObject, CSG_Parameters *pParameters)
{
	if( gSG_UI_Callback && pDataObject && pParameters )
	{
		CSG_UI_Parameter	p1(pDataObject), p2(pParameters);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_PARAMS_GET, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Params_Set	(CSG_Data_Object *pDataObject, CSG_Parameters *pParameters)
{
	if( gSG_UI_Callback && pDataObject && pParameters )
	{
		CSG_UI_Parameter	p1(pDataObject), p2(pParameters);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_PARAMS_SET, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_ODBC_Update		(const CSG_String &Server)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter	p1(Server), p2;

		return( gSG_UI_Callback(CALLBACK_ODBC_UPDATE, p1, p2) != 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void *		SG_UI_Get_Window_Main(void)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter	p1, p2;

		gSG_UI_Callback(CALLBACK_GUI_GET_WINDOW, p1, p2);

		return( p1.Pointer );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_String	SG_UI_Get_Application_Path(void)
{
	return( CSG_String(wxStandardPaths::Get().GetExecutablePath().wc_str()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
