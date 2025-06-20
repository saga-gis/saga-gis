
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_data_manager.h"
#include "wksp_map_manager.h"
#include "wksp_tool.h"
#include "data_source.h"

#include "callback.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int Callback(TSG_UI_Callback_ID ID, CSG_UI_Parameter &Param_1, CSG_UI_Parameter &Param_2)
{
	int Result = 1;

	switch( ID )
	{
	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_PROCESS_GET_OKAY:

		Result = PROCESS_Get_Okay(Param_1.Boolean);

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_OKAY:

		Result = PROCESS_Set_Okay(Param_1.Boolean);

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_BUSY:

		Result = PROCESS_Set_Busy(Param_1.Boolean, Param_2.String);

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_PROGRESS:

		Result = PROGRESSBAR_Set_Position(Param_1.Number, Param_2.Number);

		break;

	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_READY:

		Result = PROGRESSBAR_Set_Ready();

		break;


	//-----------------------------------------------------
	case CALLBACK_PROCESS_SET_TEXT:

		STATUSBAR_Set_Text(Param_1.String.c_str());

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_STOP_EXECUTION:

		if( g_pTool && g_pTool->is_Executing() )
		{
			Result = g_pTool->Execute(Param_1.Boolean) ? 1 : 0;
		}
		else if( g_pTool && g_pTool->is_Interactive() )
		{
			Result = g_pTool->Execute(Param_1.Boolean) ? 1 : 0;
		}

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD: {

		int *iArray	= (int *)Param_2.Pointer;

		MSG_General_Add		(Param_1.String.c_str(), iArray[0] != 0, iArray[0] != 0, (TSG_UI_MSG_STYLE)iArray[1]);

		break; }


	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD_ERROR:

		MSG_Error_Add		(Param_1.String.c_str(), true);

		break;


	//-----------------------------------------------------
	case CALLBACK_MESSAGE_ADD_EXECUTION: {

		int *iArray	= (int *)Param_2.Pointer;

		MSG_Execution_Add	(Param_1.String.c_str(), iArray[0] != 0, iArray[0] != 0, (TSG_UI_MSG_STYLE)iArray[1]);

		break; }


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DLG_MESSAGE:

		DLG_Message_Show(Param_1.String.c_str(), Param_2.String.c_str());

		break;


	//-----------------------------------------------------
	case CALLBACK_DLG_CONTINUE:

		Result = DLG_Message_Confirm(Param_1.String.c_str(), Param_2.String.c_str());

		break;


	//-----------------------------------------------------
	case CALLBACK_DLG_ERROR:

		Result = DLG_Message_Show_Error(Param_1.String.c_str(), Param_2.String.c_str());

		break;


	//-----------------------------------------------------
	case CALLBACK_DLG_INFO:

		Result = DLG_Info(Param_2.String.c_str(), Param_1.String.c_str());

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DLG_PARAMETERS:

		Result = DLG_Parameters((CSG_Parameters *)Param_1.Pointer, Param_2.String.c_str()) ? 1 : 0;

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_ADD:

		Result = g_pData->Add((CSG_Data_Object *)Param_1.Pointer) ? 1 : 0;

		if( Result && Param_2.Number )
		{
			g_pData->Show((CSG_Data_Object *)Param_1.Pointer, (int)Param_2.Number);
		}

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_DEL:

		Result = g_pData->Del((CSG_Data_Object *)Param_1.Pointer, Param_2.Boolean) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_UPDATE:

		Result = g_pData->Update((CSG_Data_Object *)Param_1.Pointer, (CSG_Parameters *)Param_2.Pointer) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_SHOW:

		Result = g_pData->Show((CSG_Data_Object *)Param_1.Pointer, (int)Param_2.Number) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_ASIMAGE:

		Result = g_pData->asImage((CSG_Data_Object *)Param_1.Pointer, (CSG_Grid *)Param_2.Pointer) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_COLORS_GET:

		Result = g_pData->Get_Colors((CSG_Data_Object *)Param_1.Pointer, (CSG_Colors *)Param_2.Pointer) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_COLORS_SET:

		Result = g_pData->Set_Colors((CSG_Data_Object *)Param_1.Pointer, (CSG_Colors *)Param_2.Pointer) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_CLASSIFY:

		Result = g_pData->Classify((CSG_Data_Object *)Param_1.Pointer, *((const CSG_MetaData *)Param_2.Pointer)) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_PARAMS_GET:

		Result = g_pData->Get_Parameters((CSG_Data_Object *)Param_1.Pointer, (CSG_Parameters *)Param_2.Pointer) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATAOBJECT_PARAMS_SET:

		Result = g_pData->Set_Parameters((CSG_Data_Object *)Param_1.Pointer, (CSG_Parameters *)Param_2.Pointer) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_SET_MAP_EXTENT:

		Result = g_pMaps->Set_Extent(*((CSG_Rect *)Param_1.Pointer), (int)Param_2.Number) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DIAGRAM_SHOW:

		Result = g_pData->Show_Diagram((CSG_Table *)Param_1.Pointer, (CSG_Parameters *)Param_2.Pointer) ? 1 : 0;

		break;


	//-----------------------------------------------------
	case CALLBACK_DATABASE_UPDATE:

		Result = g_pData_Source->Update_Database(Param_1.String.c_str()) ? 1 : 0;

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	case CALLBACK_GET_APP_WINDOW:

		Param_1.Pointer = MDI_Get_Frame();

		break;


	///////////////////////////////////////////////////////
	//                                                   //
	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	default:

		Result = 0;

		break;
	}

	return( Result );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_PFNC_UI_Callback Get_Callback(void)
{
	return( &Callback );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
