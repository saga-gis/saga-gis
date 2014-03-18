/**********************************************************
 * Version $Id: wksp_data_item.cpp 1493 2012-10-19 11:31:13Z oconrad $
 *********************************************************/

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
//                  wksp_data_item.cpp                   //
//                                                       //
//          Copyright (C) 2013 by Olaf Conrad            //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "active.h"
#include "active_parameters.h"
#include "active_attributes.h"

#include "wksp_base_control.h"

#include "wksp_data_manager.h"
#include "wksp_data_menu_files.h"

#include "wksp_map_manager.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"

#include "wksp_data_layers.h"

#include "wksp_data_item.h"

#include "view_histogram.h"
#include "view_scatterplot.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Item::CWKSP_Data_Item(CSG_Data_Object *pObject)
{
	m_pObject	= pObject;

	m_bUpdating	= false;
}

//---------------------------------------------------------
CWKSP_Data_Item::~CWKSP_Data_Item(void)
{
	if( m_pObject )
	{
		m_bUpdating	= true;

		for(int i=m_Views.GetCount()-1; i>=0; i--)
		{
			if( wxDynamicCast(m_Views[i], CVIEW_ScatterPlot) != NULL )
			{
				CVIEW_ScatterPlot	*pView	= ((CVIEW_ScatterPlot *)m_Views[i]);
				pView->Destroy();
				delete(pView);
			}
		}

		//-------------------------------------------------
		CSG_Data_Object	*pObject	= m_pObject;	m_pObject	= NULL;

		MSG_General_Add(wxString::Format(wxT("%s: %s..."), _TL("Close"), pObject->Get_Name()), true, true);

		SG_Get_Data_Manager().Delete(pObject);

		MSG_General_Add(_TL("okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Item::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Item::On_Command(Cmd_ID) );

	case ID_CMD_TABLES_SAVE:
	case ID_CMD_TIN_SAVE:
	case ID_CMD_POINTCLOUD_SAVE:
	case ID_CMD_SHAPES_SAVE:
	case ID_CMD_GRIDS_SAVE:
		Save(m_pObject->Get_File_Name());
		break;

	case ID_CMD_TABLES_SAVEAS:
	case ID_CMD_TIN_SAVEAS:
	case ID_CMD_POINTCLOUD_SAVEAS:
	case ID_CMD_SHAPES_SAVEAS:
	case ID_CMD_GRIDS_SAVEAS:
		Save();
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Data_Item::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Item::On_Command_UI(event) );

	case ID_CMD_TABLES_SAVE:
	case ID_CMD_TIN_SAVE:
	case ID_CMD_POINTCLOUD_SAVE:
	case ID_CMD_SHAPES_SAVE:
	case ID_CMD_GRIDS_SAVE:
		event.Enable(m_pObject->is_Modified() && m_pObject->Get_File_Name() && *(m_pObject->Get_File_Name()));
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Data_Item::Get_Name(void)
{
	if( g_pData->Get_Numbering() < 0 )
	{
		return( m_pObject ? m_pObject->Get_Name() : SG_T("-") );
	}

	return( wxString::Format(wxT("%0*d. %s"), g_pData->Get_Numbering(), 1 + Get_ID(), m_pObject ? m_pObject->Get_Name() : SG_T("-")) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Data_Item::On_Create_Parameters(void)
{
	CWKSP_Base_Item::On_Create_Parameters();

	//-----------------------------------------------------
	// Nodes...

	m_Parameters.Add_Node(NULL, "NODE_GENERAL"  , _TL("General"  ), _TL(""));


	//-----------------------------------------------------
	// General...

	m_Parameters.Add_String(
		m_Parameters("NODE_GENERAL"), "OBJECT_NAME"   , _TL("Name")			, _TL(""),
		m_pObject->Get_Name()
	);

	m_Parameters.Add_String(
		m_Parameters("NODE_GENERAL"), "OBJECT_DESC"   , _TL("Description")	, _TL(""),
		m_pObject->Get_Description(), true
	);

	m_Parameters.Add_Range(
		m_Parameters("NODE_GENERAL"), "GENERAL_NODATA", _TL("No Data")		, _TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Item::Save(void)
{
	bool		bResult;

	wxString	FileName	= m_pObject->Get_File_Name() && *m_pObject->Get_File_Name()
		? m_pObject->Get_File_Name()
		: m_pObject->Get_Name();

	switch( Get_Type() )
	{
	default:					bResult	= false;										break;
	case WKSP_ITEM_Table:		bResult	= DLG_Save(FileName, ID_DLG_TABLES_SAVE    );	break;
	case WKSP_ITEM_TIN:
	case WKSP_ITEM_Shapes:		bResult	= DLG_Save(FileName, ID_DLG_SHAPES_SAVE    );	break;
	case WKSP_ITEM_PointCloud:	bResult	= DLG_Save(FileName, ID_DLG_POINTCLOUD_SAVE);	break;
	case WKSP_ITEM_Grid:		bResult	= DLG_Save(FileName, ID_DLG_GRIDS_SAVE     );	break;
	}

	if( bResult )
	{
		bResult	= m_pObject->Save(&FileName);

		PROCESS_Set_Okay();
	}

	return( bResult );
}

//---------------------------------------------------------
bool CWKSP_Data_Item::Save(const wxString &File_Name)
{
	if( File_Name.Length() )
	{
		bool	bResult	= m_pObject->Save(&File_Name);

		if( bResult )
		{
			g_pData->Get_Menu_Files()->Recent_Add(m_pObject->Get_ObjectType(), m_pObject->Get_File_Name());
		}

		PROCESS_Set_Okay();

		return( bResult );
	}

	return( Save() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Data_Item::Parameters_Changed(void)
{
	if( !m_bUpdating )
	{
		m_bUpdating	= true;

		On_Parameters_Changed();

		CWKSP_Base_Item::Parameters_Changed();

		m_bUpdating	= false;

		Update_Views(true);
	}
}

//---------------------------------------------------------
void CWKSP_Data_Item::On_Parameters_Changed(void)
{
	m_pObject->Set_Name       (m_Parameters("OBJECT_NAME")->asString());
	m_pObject->Set_Description(m_Parameters("OBJECT_DESC")->asString());

	m_pObject->Set_NoData_Value_Range(
		m_Parameters("GENERAL_NODATA")->asRange()->Get_LoVal(),
		m_Parameters("GENERAL_NODATA")->asRange()->Get_HiVal()
	);
}

//---------------------------------------------------------
bool CWKSP_Data_Item::DataObject_Changed(void)
{
	On_DataObject_Changed();

	g_pACTIVE->Update(this, false);

	Parameters_Changed();

	return( true );
}

//---------------------------------------------------------
#include "wksp_grid.h"

bool CWKSP_Data_Item::DataObject_Changed(CSG_Parameters *pParameters)
{
	if( !pParameters && Get_Type() == WKSP_ITEM_Grid )
	{
		return( ((CWKSP_Grid *)this)->Fit_Color_Range() );
	}

	m_Parameters.Assign_Values(pParameters);

	return( DataObject_Changed() );
}

//---------------------------------------------------------
void CWKSP_Data_Item::On_DataObject_Changed(void)
{
	m_Parameters.Set_Name(CSG_String::Format(SG_T("%02d. %s"), 1 + Get_ID(), m_pObject->Get_Name()));

	m_Parameters("OBJECT_NAME")->Set_Value(m_pObject->Get_Name());
	m_Parameters("OBJECT_DESC")->Set_Value(m_pObject->Get_Description());

	m_Parameters("GENERAL_NODATA")->asRange()->Set_Range(
		m_pObject->Get_NoData_Value(),
		m_pObject->Get_NoData_hiValue()
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Item::Add_ScatterPlot(void)
{
	new CVIEW_ScatterPlot(this);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Item::View_Opened(wxMDIChildFrame *pView)
{
	if( m_Views.Index(pView) == wxNOT_FOUND )	// only add once
	{
		m_Views.Add(pView);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Item::View_Closes(wxMDIChildFrame *pView)
{
	if( m_Views.Index(pView) != wxNOT_FOUND )
	{
		m_Views.Remove(pView);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Item::Update_Views(bool bAll)
{
	if( !m_bUpdating )
	{
		m_bUpdating	= true;

		On_Update_Views(bAll);

		if( bAll )
		{
			for(size_t i=0; i<m_Views.Count(); i++)
			{
				if( wxDynamicCast(m_Views[i], CVIEW_ScatterPlot) != NULL )
				{
					((CVIEW_ScatterPlot *)m_Views[i])->Update_Data();
				}
			}
		}

		if( g_pACTIVE->Get_Active_Data_Item() == this )
		{
			g_pACTIVE->Get_Attributes()->Set_Attributes();
		}

		m_bUpdating	= false;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
