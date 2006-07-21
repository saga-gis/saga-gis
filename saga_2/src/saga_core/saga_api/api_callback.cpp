
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
TSG_PFNC_Callback	gSG_fCallback	= NULL;

//---------------------------------------------------------
bool			SG_Set_Callback(TSG_PFNC_Callback Callback)
{
	gSG_fCallback	= Callback;

	return( true );
}

//---------------------------------------------------------
TSG_PFNC_Callback	SG_Get_Callback(void)
{
	return( gSG_fCallback );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_Callback_Process_Get_Okay(bool bBlink)
{
	if( gSG_fCallback )
	{
		return( gSG_fCallback(CALLBACK_PROCESS_GET_OKAY, bBlink ? 1 : 0, 0) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_Callback_Process_Set_Okay(bool bOkay)
{
	if( gSG_fCallback )
	{
		return( gSG_fCallback(CALLBACK_PROCESS_SET_OKAY, bOkay ? 1 : 0, 0) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_Callback_Process_Set_Progress(double Position, double Range)
{
	if( gSG_fCallback )
	{
		return( gSG_fCallback(CALLBACK_PROCESS_SET_PROGRESS, (long)&Position, (long)&Range) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
bool		SG_Callback_Process_Set_Ready(void)
{
	if( gSG_fCallback )
	{
		return( gSG_fCallback(CALLBACK_PROCESS_SET_READY, 0, 0) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
void		SG_Callback_Process_Set_Text(const char *Text)
{
	if( gSG_fCallback )
	{
		gSG_fCallback(CALLBACK_PROCESS_SET_TEXT, (long)Text, 0);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		SG_Callback_Dlg_Message(const char *Message, const char *Caption)
{
	if( gSG_fCallback )
	{
		gSG_fCallback(CALLBACK_DLG_MESSAGE, (long)Message, (long)Caption);
	}
}

//---------------------------------------------------------
bool		SG_Callback_Dlg_Continue(const char *Message, const char *Caption)
{
	if( gSG_fCallback )
	{
		return( gSG_fCallback(CALLBACK_DLG_CONTINUE, (long)Message, (long)Caption) != 0 );
	}

	return( true );
}

//---------------------------------------------------------
int			SG_Callback_Dlg_Error(const char *Message, const char *Caption)
{
	if( gSG_fCallback )
	{
		return( gSG_fCallback(CALLBACK_DLG_ERROR, (long)Message, (long)Caption) );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_Callback_Dlg_Parameters(CParameters *pParameters, const char *Caption)
{
	if( gSG_fCallback && pParameters )
	{
		return( gSG_fCallback(CALLBACK_DLG_PARAMETERS, (long)pParameters, (long)Caption) != 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		SG_Callback_Message_Add(const char *Message, bool bNewLine)
{
	if( gSG_fCallback )
	{
		gSG_fCallback(CALLBACK_MESSAGE_ADD, (long)Message, (long)bNewLine);
	}
}

//---------------------------------------------------------
void		SG_Callback_Message_Add_Error(const char *Message)
{
	if( gSG_fCallback )
	{
		gSG_fCallback(CALLBACK_MESSAGE_ADD_ERROR, (long)Message, 0);
	}
}

//---------------------------------------------------------
void		SG_Callback_Message_Add_Execution(const char *Message, bool bNewLine)
{
	if( gSG_fCallback )
	{
		gSG_fCallback(CALLBACK_MESSAGE_ADD_EXECUTION, (long)Message, (long)bNewLine);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDataObject *	SG_Callback_DataObject_Find(const char *File_Name, int Object_Type)
{
	if( gSG_fCallback && File_Name )
	{
		return( (class CDataObject *)gSG_fCallback(CALLBACK_DATAOBJECT_FIND_BY_FILE, (long)File_Name, Object_Type) );
	}

	return( NULL );
}

//---------------------------------------------------------
bool		SG_Callback_DataObject_Check(CDataObject *pDataObject, int Object_Type)
{
	if( gSG_fCallback && pDataObject )
	{
		return( gSG_fCallback(CALLBACK_DATAOBJECT_CHECK, (long)pDataObject, Object_Type) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_Callback_DataObject_Add(CDataObject *pDataObject, bool bShow)
{
	if( gSG_fCallback && pDataObject )
	{
		return( gSG_fCallback(CALLBACK_DATAOBJECT_ADD, (long)pDataObject, bShow ? 1 : 0) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_Callback_DataObject_Update(CDataObject *pDataObject, bool bShow, CParameters *pParameters)
{
	CParameters	Parameters;

	if( gSG_fCallback && pDataObject )
	{
		if( pDataObject->Get_ObjectType() == DATAOBJECT_TYPE_Grid && pParameters == NULL )
		{
			Parameters.Add_Range(NULL, "METRIC_ZRANGE", "", "",
				((CGrid *)pDataObject)->Get_ZMin(true),
				((CGrid *)pDataObject)->Get_ZMax(true)
			);

			pParameters	= &Parameters;
		}

		if( gSG_fCallback(CALLBACK_DATAOBJECT_UPDATE, (long)pDataObject, (long)pParameters) != 0 )
		{
			if( bShow )
			{
				SG_Callback_DataObject_Show(pDataObject);
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_Callback_DataObject_Show(CDataObject *pDataObject)
{
	if( gSG_fCallback && pDataObject )
	{
		return( gSG_fCallback(CALLBACK_DATAOBJECT_SHOW, (long)pDataObject, 1) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_Callback_DataObject_asImage(CDataObject *pDataObject, CGrid *pGrid)
{
	if( gSG_fCallback && pDataObject )
	{
		return( gSG_fCallback(CALLBACK_DATAOBJECT_ASIMAGE, (long)pDataObject, (long)pGrid) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_Callback_DataObject_Colors_Get(CDataObject *pDataObject, CSG_Colors *pColors)
{
	if( gSG_fCallback && pDataObject && pColors )
	{
		return( gSG_fCallback(CALLBACK_DATAOBJECT_COLORS_GET, (long)pDataObject, (long)pColors) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_Callback_DataObject_Colors_Set(CDataObject *pDataObject, CSG_Colors *pColors)
{
	if( gSG_fCallback && pDataObject && pColors )
	{
		return( gSG_fCallback(CALLBACK_DATAOBJECT_COLORS_SET, (long)pDataObject, (long)pColors) != 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool		SG_Callback_DataObject_Get_All(class CParameters *pParameters)
{
	if( gSG_fCallback && pParameters )
	{
		return( gSG_fCallback(CALLBACK_DATAOBJECT_GET_ALL, (long)pParameters, 0) != 0 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void *		SG_Callback_Get_Window_Main(void)
{
	if( gSG_fCallback )
	{
		return( (void *)gSG_fCallback(CALLBACK_GUI_GET_WINDOW, 0, 0) );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
