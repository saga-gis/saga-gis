
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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
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
#ifdef _SAGA_MSW
static bool gSG_UI_Console_bUTF8 = false;
#else
static bool gSG_UI_Console_bUTF8 = true;
#endif

void	SG_UI_Console_Set_UTF8(bool bOn) {          gSG_UI_Console_bUTF8 = bOn; }
bool	SG_UI_Console_Get_UTF8(void)     {	return( gSG_UI_Console_bUTF8 );     }

//---------------------------------------------------------
void	SG_UI_Console_Print_StdOut(const char       *Text, SG_Char End, bool bFlush) { SG_UI_Console_Print_StdOut(CSG_String(Text), End, bFlush); }
void	SG_UI_Console_Print_StdOut(const wchar_t    *Text, SG_Char End, bool bFlush) { SG_UI_Console_Print_StdOut(CSG_String(Text), End, bFlush); }
void	SG_UI_Console_Print_StdOut(const CSG_String &Text, SG_Char End, bool bFlush)
{
	if( gSG_UI_Console_bUTF8 )
	{
		printf("%s", Text.to_UTF8 ().Get_Data());
	}
	else
	{
		printf("%s", Text.to_ASCII().Get_Data());
	}

	if( End )
	{
		printf("%c", End);
	}

	if( bFlush )
	{
		fflush(stdout);
	}
}

//---------------------------------------------------------
void	SG_UI_Console_Print_StdErr(const char       *Text, SG_Char End, bool bFlush) { SG_UI_Console_Print_StdErr(CSG_String(Text), End, bFlush); }
void	SG_UI_Console_Print_StdErr(const wchar_t    *Text, SG_Char End, bool bFlush) { SG_UI_Console_Print_StdErr(CSG_String(Text), End, bFlush); }
void	SG_UI_Console_Print_StdErr(const CSG_String &Text, SG_Char End, bool bFlush)
{
	if( gSG_UI_Console_bUTF8 )
	{
		fprintf(stderr, "%s", Text.to_UTF8 ().Get_Data());
	}
	else
	{
		fprintf(stderr, "%s", Text.to_ASCII().Get_Data());
	}

	if( End )
	{
		printf("%c", End);
	}

	if( bFlush )
	{
		fflush(stderr);
	}
}


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
	Boolean	= Copy.Boolean;
	Number	= Copy.Number ;
	Pointer	= Copy.Pointer;
	String	= Copy.String ;
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
int			SG_UI_Progress_Reset(void)
{
	int Locked = gSG_UI_Progress_Lock;

	gSG_UI_Progress_Lock = 0;

	return( Locked );
}

//---------------------------------------------------------
bool		SG_UI_Process_Get_Okay(bool bBlink)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter p1(gSG_UI_Progress_Lock == 0 && bBlink), p2;

		return( gSG_UI_Callback(CALLBACK_PROCESS_GET_OKAY, p1, p2) != 0 );
	}

	if( gSG_UI_Progress_Lock == 0 && bBlink )
	{
		static int iBuisy = 0; static const SG_Char Buisy[4] = { '|', '/', '-', '\\' };

		SG_UI_Console_Print_StdOut(CSG_String::Format("\r%c", Buisy[iBuisy++]), '\0', true);

		iBuisy %= 4;
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Okay(bool bOkay)
{
	if( gSG_UI_Progress_Lock == 0 && gSG_UI_Callback )
	{
		CSG_UI_Parameter p1(bOkay), p2;

		return( gSG_UI_Callback(CALLBACK_PROCESS_SET_OKAY, p1, p2) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Busy(bool bOn, const CSG_String &Message)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter p1(bOn), p2(Message);

		return( gSG_UI_Callback(CALLBACK_PROCESS_SET_BUSY, p1, p2) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Progress(int Position, int Range)
{
	return( SG_UI_Process_Set_Progress((double)Position, (double)Range) );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Progress(sLong Position, sLong Range)
{
	return( SG_UI_Process_Set_Progress((double)Position, (double)Range) );
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
		CSG_UI_Parameter p1(Position), p2(Range);

		return( gSG_UI_Callback(CALLBACK_PROCESS_SET_PROGRESS, p1, p2) != 0 );
	}

	//-----------------------------------------------------
	static int Progress = -1;

	int i = Position < 0. ? -1 : Range > 0. ? 1 + (int)(100. * Position / Range) : 100;

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

	return( true );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Ready(void)
{
	if( gSG_UI_Callback )
	{
		if( gSG_UI_Progress_Lock == 0 )
		{
			CSG_UI_Parameter p1, p2;

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
			CSG_UI_Parameter p1(Text), p2;

			gSG_UI_Callback(CALLBACK_PROCESS_SET_TEXT, p1, p2);
		}
		else
		{
			SG_UI_Console_Print_StdOut(Text, '\n', true);
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
		CSG_UI_Parameter p1(bDialog), p2;

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
			CSG_UI_Parameter p1(Message), p2(Caption);

			gSG_UI_Callback(CALLBACK_DLG_MESSAGE, p1, p2);
		}
		else
		{
			SG_UI_Console_Print_StdOut(CSG_String::Format("%s: %s", Caption.c_str(), Message.c_str()), '\n', true);
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
			CSG_UI_Parameter p1(Message), p2(Caption);

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
		CSG_UI_Parameter p1(Message), p2(Caption);

		return( gSG_UI_Callback(CALLBACK_DLG_ERROR, p1, p2) );
	}

	return( 0 );
}

//---------------------------------------------------------
void		SG_UI_Dlg_Info(const CSG_String &Message, const CSG_String &Caption)
{
	if( gSG_UI_Progress_Lock == 0 )
	{
		if( gSG_UI_Callback )
		{
			CSG_UI_Parameter p1(Message), p2(Caption);

			gSG_UI_Callback(CALLBACK_DLG_INFO, p1, p2);
		}
		else
		{
			SG_UI_Console_Print_StdOut(CSG_String::Format("%s: %s", Caption.c_str(), Message.c_str()), '\n', true);
		}
	}
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
		CSG_UI_Parameter p1(pParameters), p2(Caption);

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
int			SG_UI_Msg_Reset(void)
{
	int Locked = gSG_UI_Msg_Lock;

	gSG_UI_Msg_Lock = 0;

	return( Locked );
}

//---------------------------------------------------------
void		SG_UI_Msg_Add(const char       *Message, bool bNewLine, TSG_UI_MSG_STYLE Style) { SG_UI_Msg_Add(CSG_String(Message), bNewLine, Style); }
void		SG_UI_Msg_Add(const wchar_t    *Message, bool bNewLine, TSG_UI_MSG_STYLE Style) { SG_UI_Msg_Add(CSG_String(Message), bNewLine, Style); }
void		SG_UI_Msg_Add(const CSG_String &Message, bool bNewLine, TSG_UI_MSG_STYLE Style)
{
	if( !gSG_UI_Msg_Lock )
	{
		if( gSG_UI_Callback )
		{
			int Flags[2]; Flags[0] = bNewLine ? 1 : 0; Flags[1] = Style;

			CSG_UI_Parameter p1(Message), p2(Flags);

			gSG_UI_Callback(CALLBACK_MESSAGE_ADD, p1, p2);
		}
		else
		{
			if( bNewLine )
			{
				SG_UI_Console_Print_StdOut("");
			}

			SG_UI_Console_Print_StdOut(Message, '\0', true);
		}
	}
}

//---------------------------------------------------------
void		SG_UI_Msg_Add_Execution(const char       *Message, bool bNewLine, TSG_UI_MSG_STYLE Style) { SG_UI_Msg_Add_Execution(CSG_String(Message), bNewLine, Style); }
void		SG_UI_Msg_Add_Execution(const wchar_t    *Message, bool bNewLine, TSG_UI_MSG_STYLE Style) { SG_UI_Msg_Add_Execution(CSG_String(Message), bNewLine, Style); }
void		SG_UI_Msg_Add_Execution(const CSG_String &Message, bool bNewLine, TSG_UI_MSG_STYLE Style)
{
	if( !gSG_UI_Msg_Lock )
	{
		if( gSG_UI_Callback )
		{
			int Flags[2]; Flags[0] = bNewLine ? 1 : 0; Flags[1] = Style;

			CSG_UI_Parameter p1(Message), p2(Flags);

			gSG_UI_Callback(CALLBACK_MESSAGE_ADD_EXECUTION, p1, p2);
		}
		else
		{
			if( bNewLine )
			{
				SG_UI_Console_Print_StdOut("");
			}

			SG_UI_Console_Print_StdOut(Message, '\0', true);
		}
	}
}

//---------------------------------------------------------
void		SG_UI_Msg_Add_Error(const char       *Message) { SG_UI_Msg_Add_Error(CSG_String(Message)); }
void		SG_UI_Msg_Add_Error(const wchar_t    *Message) { SG_UI_Msg_Add_Error(CSG_String(Message)); }
void		SG_UI_Msg_Add_Error(const CSG_String &Message)
{
	if( !gSG_UI_Msg_Lock )
	{
		if( gSG_UI_Callback )
		{
			CSG_UI_Parameter p1(Message), p2;

			gSG_UI_Callback(CALLBACK_MESSAGE_ADD_ERROR, p1, p2);
		}
		else
		{
			SG_UI_Console_Print_StdErr(CSG_String::Format("\n[%s] %s", _TL("Error"), Message.c_str()), '\0', true);
		}
	}
}

//---------------------------------------------------------
void		SG_UI_Msg_Flush(void)
{
	fflush(stdout);
	fflush(stderr);
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

//---------------------------------------------------------
void		SG_UI_ProgressAndMsg_Reset	(void)
{
	SG_UI_Progress_Reset();
	SG_UI_Msg_Reset     ();
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
		CSG_UI_Parameter p1(pDataObject), p2(Show ? true : false);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_ADD, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Update(CSG_Data_Object *pDataObject, int Show, CSG_Parameters *pParameters)
{
	if( gSG_UI_Callback && pDataObject )
	{
		CSG_UI_Parameter p1(pDataObject->Get_Owner() ? pDataObject->Get_Owner() : pDataObject), p2(pParameters);

		if( gSG_UI_Callback(CALLBACK_DATAOBJECT_UPDATE, p1, p2) != 0 )
		{
			if( Show != SG_UI_DATAOBJECT_UPDATE )
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
		CSG_UI_Parameter p1(pDataObject), p2(Show);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_SHOW, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_asImage(CSG_Data_Object *pDataObject, CSG_Grid *pGrid)
{
	if( gSG_UI_Callback && pDataObject )
	{
		CSG_UI_Parameter p1(pDataObject), p2(pGrid);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_ASIMAGE, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Colors_Get(CSG_Data_Object *pDataObject, CSG_Colors *pColors)
{
	if( gSG_UI_Callback && pDataObject && pColors )
	{
		CSG_UI_Parameter p1(pDataObject), p2(pColors);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_COLORS_GET, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Colors_Set(CSG_Data_Object *pDataObject, CSG_Colors *pColors)
{
	if( gSG_UI_Progress_Lock == 0 && gSG_UI_Callback && pDataObject && pColors )
	{
		CSG_UI_Parameter p1(pDataObject), p2(pColors);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_COLORS_SET, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Params_Get	(CSG_Data_Object *pDataObject, CSG_Parameters *pParameters)
{
	if( gSG_UI_Callback && pDataObject && pParameters )
	{
		CSG_UI_Parameter p1(pDataObject), p2(pParameters);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_PARAMS_GET, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Params_Set	(CSG_Data_Object *pDataObject, CSG_Parameters *pParameters)
{
	if( gSG_UI_Progress_Lock == 0 && gSG_UI_Callback && pDataObject && pParameters )
	{
		CSG_UI_Parameter p1(pDataObject), p2(pParameters);

		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_PARAMS_SET, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_Set_Map_Extent		(double xMin, double yMin, double xMax, double yMax, int Maps)
{
	if( gSG_UI_Callback )
	{
		CSG_Rect r(xMin, yMin, xMax, yMax); CSG_UI_Parameter p1(&r), p2(Maps);

		return( gSG_UI_Callback(CALLBACK_SET_MAP_EXTENT, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_Diagram_Show			(class CSG_Table *pTable, class CSG_Parameters *pParameters)
{
	if( gSG_UI_Callback && SG_UI_DataObject_Update(pTable, SG_UI_DATAOBJECT_UPDATE, NULL) )
	{
		CSG_UI_Parameter p1(pTable), p2(pParameters);

		return( gSG_UI_Callback(CALLBACK_DIAGRAM_SHOW, p1, p2) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_ODBC_Update			(const CSG_String &Server)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter p1(Server), p2;

		return( gSG_UI_Callback(CALLBACK_DATABASE_UPDATE, p1, p2) != 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int			SG_UI_Window_Arrange		(int Arrange)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter p1, p2;

		gSG_UI_Callback(CALLBACK_WINDOW_ARRANGE, p1, p2);

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
void *		SG_UI_Get_Window_Main		(void)
{
	if( gSG_UI_Callback )
	{
		CSG_UI_Parameter p1, p2;

		gSG_UI_Callback(CALLBACK_GET_APP_WINDOW, p1, p2);

		return( p1.Pointer );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_String	SG_UI_Get_Application_Path	(bool bPathOnly)
{
	CSG_String	App_Path(wxStandardPaths::Get().GetExecutablePath().wc_str());

	if( bPathOnly )
	{
		App_Path = SG_File_Get_Path(App_Path);
	}

	return( SG_File_Get_Path_Absolute(App_Path) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
