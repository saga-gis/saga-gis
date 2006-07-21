
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
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_data_manager.h"

#include "callback.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		Callback(TSG_Callback_ID ID, long Param_1, long Param_2)
{
	int		Result;

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

		Result	= PROCESS_Get_Okay(Param_1 != 0);

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_OKAY:

		Result	= PROCESS_Set_Okay(Param_1 != 0);

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_PROGRESS:

		Result	= PROGRESSBAR_Set_Position(*((double *)Param_1), *((double *)Param_2));

		break;

	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_READY:

		Result	= PROGRESSBAR_Set_Position(0);

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_TEXT:

		STATUSBAR_Set_Text((char *)Param_1);

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	//                                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD:

		MSG_General_Add		((char *)Param_1, Param_2 != 0, Param_2 != 0);

		break;


	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD_ERROR:

		MSG_Error_Add		((char *)Param_1, true);

		break;


	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD_EXECUTION:

		MSG_Execution_Add	((char *)Param_1, Param_2 != 0, Param_2 != 0);

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	//                                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DLG_MESSAGE:

		DLG_Message_Show((char *)Param_1, (char *)Param_2);

		break;


	//-----------------------------------------------------
	case CALLBACK_DLG_CONTINUE:

		Result	= DLG_Message_Confirm((char *)Param_1, (char *)Param_2);

		break;


	//-----------------------------------------------------
	case CALLBACK_DLG_ERROR:

		Result	= DLG_Message_Show_Error((char *)Param_1, (char *)Param_2);

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	//                                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DLG_PARAMETERS:

		Result	= DLG_Parameters((CParameters *)Param_1) ? 1 : 0;

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	//                                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_FIND_BY_FILE:

		Result	= (int)g_pData->Get_byFileName((const char *)Param_1, Param_2);

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_CHECK:

		Result	= g_pData->Exists((CDataObject *)Param_1, Param_2) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_ADD:

		Result	= g_pData->Add((CDataObject *)Param_1) ? 1 : 0;

		if( Result && Param_2 )
		{
			g_pData->Show((CDataObject *)Param_1);
		}

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_UPDATE:

		Result	= g_pData->Update((CDataObject *)Param_1, (CParameters *)Param_2) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_SHOW:

		Result	= g_pData->Show((CDataObject *)Param_1) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_ASIMAGE:

		Result	= g_pData->asImage((CDataObject *)Param_1, (CGrid *)Param_2) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_COLORS_GET:

		Result	= g_pData->Get_Colors((CDataObject *)Param_1, (CSG_Colors *)Param_2) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_COLORS_SET:

		Result	= g_pData->Set_Colors((CDataObject *)Param_1, (CSG_Colors *)Param_2) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_GET_ALL:

		Result	= g_pData->Get_DataObject_List((CParameters *)Param_1) ? 1 : 0;

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	//                                                   //
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_GUI_GET_WINDOW:

		Result	= (int)MDI_Get_Frame();

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
TSG_PFNC_Callback	Get_Callback(void)
{
	return( &Callback );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
