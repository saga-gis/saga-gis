
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

//---------------------------------------------------------
bool		SG_UI_Process_Get_Okay(bool bBlink)
{
	if( gSG_UI_Callback )
	{
		return( gSG_UI_Callback(CALLBACK_PROCESS_GET_OKAY, bBlink ? 1 : 0, 0) != 0 );
	}
	else
	{
		if( bBlink )
		{
			static int	iBuisy		= 0;
			const SG_Char	Buisy[4]	= {	'|', '/', '-', '\\'	};

			SG_PRINTF(SG_T("\r%c   "), Buisy[iBuisy++]);
			iBuisy	%= 4;
		}
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Okay(bool bOkay)
{
	if( gSG_UI_Callback )
	{
		return( gSG_UI_Callback(CALLBACK_PROCESS_SET_OKAY, bOkay ? 1 : 0, 0) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Progress(double Position, double Range)
{
	if( gSG_UI_Callback )
	{
		return( gSG_UI_Callback(CALLBACK_PROCESS_SET_PROGRESS, (long)&Position, (long)&Range) != 0 );
	}
	else
	{
		SG_PRINTF(SG_T("\r%3d%%"), Range != 0.0 ? 1 + (int)(100.0 * Position / Range) : 100);
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_UI_Process_Set_Ready(void)
{
	if( gSG_UI_Callback )
	{
		return( gSG_UI_Callback(CALLBACK_PROCESS_SET_READY, 0, 0) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
void		SG_UI_Process_Set_Text(const SG_Char *Text)
{
	if( gSG_UI_Callback )
	{
		gSG_UI_Callback(CALLBACK_PROCESS_SET_TEXT, (long)Text, 0);
	}
	else
	{
		SG_PRINTF(SG_T("\n%s"), Text);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		SG_UI_Dlg_Message(const SG_Char *Message, const SG_Char *Caption)
{
	if( gSG_UI_Callback )
	{
		gSG_UI_Callback(CALLBACK_DLG_MESSAGE, (long)Message, (long)Caption);
	}
	else
	{
		SG_PRINTF(SG_T("\n%s: %s"), Caption, Message);
	}
}

//---------------------------------------------------------
bool		SG_UI_Dlg_Continue(const SG_Char *Message, const SG_Char *Caption)
{
	if( gSG_UI_Callback )
	{
		return( gSG_UI_Callback(CALLBACK_DLG_CONTINUE, (long)Message, (long)Caption) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
int			SG_UI_Dlg_Error(const SG_Char *Message, const SG_Char *Caption)
{
	if( gSG_UI_Callback )
	{
		return( gSG_UI_Callback(CALLBACK_DLG_ERROR, (long)Message, (long)Caption) );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_UI_Dlg_Parameters(CSG_Parameters *pParameters, const SG_Char *Caption)
{
	if( gSG_UI_Callback && pParameters )
	{
		return( gSG_UI_Callback(CALLBACK_DLG_PARAMETERS, (long)pParameters, (long)Caption) != 0 );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		SG_UI_Msg_Add(const SG_Char *Message, bool bNewLine, TSG_UI_MSG_STYLE Style)
{
	if( gSG_UI_Callback )
	{
		int		Parameters[2];

		Parameters[0]	= bNewLine ? 1 : 0;
		Parameters[1]	= Style;

		gSG_UI_Callback(CALLBACK_MESSAGE_ADD, (long)Message, (long)Parameters);
	}
	else
	{
		SG_PRINTF(SG_T("%s"), Message);

		if( bNewLine )
		{
			SG_PRINTF(SG_T("\n\n"));
		}
	}
}

//---------------------------------------------------------
void		SG_UI_Msg_Add_Error(const SG_Char *Message)
{
	if( gSG_UI_Callback )
	{
		gSG_UI_Callback(CALLBACK_MESSAGE_ADD_ERROR, (long)Message, 0);
	}
	else
	{
		SG_PRINTF(SG_T("\n%s: %s"), LNG("Error"), Message);
	}
}

//---------------------------------------------------------
void		SG_UI_Msg_Add_Execution(const SG_Char *Message, bool bNewLine, TSG_UI_MSG_STYLE Style)
{
	if( gSG_UI_Callback )
	{
		int		Parameters[2];

		Parameters[0]	= bNewLine ? 1 : 0;
		Parameters[1]	= Style;

		gSG_UI_Callback(CALLBACK_MESSAGE_ADD_EXECUTION, (long)Message, (long)Parameters);
	}
	else
	{
		SG_PRINTF(SG_T("%s"), Message);

		if( bNewLine )
		{
			SG_PRINTF(SG_T("\n\n"));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Data_Object *	SG_UI_DataObject_Find(const SG_Char *File_Name, int Object_Type)
{
	if( gSG_UI_Callback && File_Name )
	{
		return( (class CSG_Data_Object *)gSG_UI_Callback(CALLBACK_DATAOBJECT_FIND_BY_FILE, (long)File_Name, Object_Type) );
	}

	return( NULL );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Check(CSG_Data_Object *pDataObject, int Object_Type)
{
	if( gSG_UI_Callback && pDataObject )
	{
		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_CHECK, (long)pDataObject, Object_Type) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Add(CSG_Data_Object *pDataObject, bool bShow)
{
	if( gSG_UI_Callback && pDataObject )
	{
		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_ADD, (long)pDataObject, bShow ? 1 : 0) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Update(CSG_Data_Object *pDataObject, bool bShow, CSG_Parameters *pParameters)
{
	CSG_Parameters	Parameters;

	if( gSG_UI_Callback && pDataObject )
	{
		if( pDataObject->Get_ObjectType() == DATAOBJECT_TYPE_Grid && pParameters == NULL )
		{
			Parameters.Add_Range(NULL, SG_T("METRIC_ZRANGE"), SG_T(""), SG_T(""),
				((CSG_Grid *)pDataObject)->Get_ZMin(true),
				((CSG_Grid *)pDataObject)->Get_ZMax(true)
			);

			pParameters	= &Parameters;
		}

		if( gSG_UI_Callback(CALLBACK_DATAOBJECT_UPDATE, (long)pDataObject, (long)pParameters) != 0 )
		{
			if( bShow )
			{
				SG_UI_DataObject_Show(pDataObject);
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Show(CSG_Data_Object *pDataObject)
{
	if( gSG_UI_Callback && pDataObject )
	{
		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_SHOW, (long)pDataObject, 1) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_asImage(CSG_Data_Object *pDataObject, CSG_Grid *pGrid)
{
	if( gSG_UI_Callback && pDataObject )
	{
		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_ASIMAGE, (long)pDataObject, (long)pGrid) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Colors_Get(CSG_Data_Object *pDataObject, CSG_Colors *pColors)
{
	if( gSG_UI_Callback && pDataObject && pColors )
	{
		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_COLORS_GET, (long)pDataObject, (long)pColors) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Colors_Set(CSG_Data_Object *pDataObject, CSG_Colors *pColors)
{
	if( gSG_UI_Callback && pDataObject && pColors )
	{
		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_COLORS_SET, (long)pDataObject, (long)pColors) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Params_Get	(CSG_Data_Object *pDataObject, CSG_Parameters *pParameters)
{
	if( gSG_UI_Callback && pDataObject && pParameters )
	{
		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_PARAMS_GET, (long)pDataObject, (long)pParameters) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Params_Set	(CSG_Data_Object *pDataObject, CSG_Parameters *pParameters)
{
	if( gSG_UI_Callback && pDataObject && pParameters )
	{
		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_PARAMS_SET, (long)pDataObject, (long)pParameters) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_UI_DataObject_Get_All(class CSG_Parameters *pParameters)
{
	if( gSG_UI_Callback && pParameters )
	{
		return( gSG_UI_Callback(CALLBACK_DATAOBJECT_GET_ALL, (long)pParameters, 0) != 0 );
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
		return( (void *)gSG_UI_Callback(CALLBACK_GUI_GET_WINDOW, 0, 0) );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
