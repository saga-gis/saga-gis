	
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
//                WKSP_Data_Menu_File.cpp                //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include "helper.h"

#include "res_commands.h"

#include "wksp_data_manager.h"
#include "wksp_data_menu_file.h"

#include "wksp_table_manager.h"
#include "wksp_shapes_manager.h"
#include "wksp_tin_manager.h"
#include "wksp_grid_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Menu_File::CWKSP_Data_Menu_File(void)
{
	m_DataType	= SG_DATAOBJECT_TYPE_Undefined;
	m_pMenu		= NULL;
}

//---------------------------------------------------------
CWKSP_Data_Menu_File::~CWKSP_Data_Menu_File(void)
{
	Destroy();
}

//---------------------------------------------------------
void CWKSP_Data_Menu_File::Destroy(void)
{
	if( !m_Group.IsEmpty() )
	{
		CONFIG_Delete("RECENT_FILES/" + m_Group);

		for(size_t i=0; i<m_Files.Count(); i++)
		{
			CONFIG_Write("RECENT_FILES/" + m_Group, wxString::Format("FILE_%02zu", i + 1), m_Files[i]);
		}

		for(size_t i=m_Files.Count(); i<=m_CmdID[1]; i++)
		{
			CONFIG_Delete("RECENT_FILES/" + m_Group, wxString::Format("FILE_%02zu", i + 1));
		}
	}

	m_DataType	= SG_DATAOBJECT_TYPE_Undefined;
	m_pMenu		= NULL;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_Data_Menu_File::Create(TSG_Data_Object_Type DataType)
{
	Destroy();

	m_DataType	= DataType;

	m_pMenu	= new wxMenu;

	//-----------------------------------------------------
	switch( m_DataType )
	{
	case SG_DATAOBJECT_TYPE_Undefined:
		m_Group		= "Project";
		m_CmdID[0]	= ID_CMD_DATA_PROJECT_RECENT_FIRST;
		m_CmdID[1]	= ID_CMD_DATA_PROJECT_RECENT_LAST - m_CmdID[0] + 1;

		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_DATA_PROJECT_OPEN   );
		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_DATA_PROJECT_BROWSE );
		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_DATA_PROJECT_NEW  );
		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_DATA_PROJECT_SAVE   );
		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_DATA_PROJECT_SAVE_AS);
		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_DATA_PROJECT_COPY   );
		break;

	case SG_DATAOBJECT_TYPE_Table:
		m_Group		= "Table";
		m_CmdID[0]	= ID_CMD_TABLE_RECENT_FIRST;
		m_CmdID[1]	= ID_CMD_TABLE_RECENT_LAST        - m_CmdID[0] + 1;

		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_TABLE_OPEN);
		break;

	case SG_DATAOBJECT_TYPE_Shapes:
		m_Group		= "Shapes";
		m_CmdID[0]	= ID_CMD_SHAPES_RECENT_FIRST;
		m_CmdID[1]	= ID_CMD_SHAPES_RECENT_LAST       - m_CmdID[0] + 1;

		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_SHAPES_OPEN);
		break;

	case SG_DATAOBJECT_TYPE_TIN:
		m_Group		= "TIN";
		m_CmdID[0]	= ID_CMD_TIN_RECENT_FIRST;
		m_CmdID[1]	= ID_CMD_TIN_RECENT_LAST          - m_CmdID[0] + 1;

		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_TIN_OPEN);
		break;

	case SG_DATAOBJECT_TYPE_PointCloud:
		m_Group		= "Point Cloud";
		m_CmdID[0]	= ID_CMD_POINTCLOUD_RECENT_FIRST;
		m_CmdID[1]	= ID_CMD_POINTCLOUD_RECENT_LAST   - m_CmdID[0] + 1;

		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_POINTCLOUD_OPEN);
		break;

	case SG_DATAOBJECT_TYPE_Grid:
		m_Group		= "Grid";
		m_CmdID[0]	= ID_CMD_GRID_RECENT_FIRST;
		m_CmdID[1]	= ID_CMD_GRID_RECENT_LAST         - m_CmdID[0] + 1;

		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_GRID_OPEN);
		break;

	case SG_DATAOBJECT_TYPE_Grids:
		m_Group		= "Grids";
		m_CmdID[0]	= ID_CMD_GRIDS_RECENT_FIRST;
		m_CmdID[1]	= ID_CMD_GRIDS_RECENT_LAST        - m_CmdID[0] + 1;

		CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_GRIDS_OPEN);
		break;

	default:
		m_CmdID[0]	= 0;
		m_CmdID[1]	= 0;
		break;
	}

	//-----------------------------------------------------
	m_Offset	= m_pMenu->GetMenuItemCount();

	if( m_CmdID[0] )
	{
		int	i = 0; wxString File;

		while( CONFIG_Read("RECENT_FILES/" + m_Group, wxString::Format("FILE_%02d", ++i), File) )
		{
			bool bAdd = wxFileExists(File);

			for(int j=0; bAdd && j<m_Files.Count(); j++)
			{
				bAdd = File.Cmp(m_Files[j]) != 0;
			}

			if( bAdd )
			{
				m_Files.Add(File);
			}
		}
	}

	//-----------------------------------------------------
	Update();

	return( m_pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Data_Menu_File::Update(void)
{
	if( !m_pMenu )
	{
		return;
	}

	//-----------------------------------------------------
	for(int i=m_Files.GetCount(); i>0; i--)
	{
		if( !wxFileExists(m_Files[i - 1]) )
		{
			m_Files.RemoveAt(i - 1);
		}
	}

	//-----------------------------------------------------
	if( m_Files.GetCount() == 0 )
	{
		for(size_t i=m_pMenu->GetMenuItemCount(); i>m_Offset; i--)
		{
			m_pMenu->Destroy(m_pMenu->GetMenuItems()[i - 1]);
		}
	}

	//-----------------------------------------------------
	else
	{
		if( m_pMenu->GetMenuItemCount() == m_Offset )
		{
			m_pMenu->AppendSeparator();
		}

		for(size_t i=m_pMenu->GetMenuItemCount(); i>m_Offset + 1 + m_Files.GetCount(); i--)
		{
			m_pMenu->Destroy(m_pMenu->GetMenuItems()[i - 1]);
		}

		for(size_t i=0; i<m_Files.GetCount(); i++)
		{
			if( m_pMenu->GetMenuItemCount() <= m_Offset + 1 + i )
			{
				m_pMenu->Append  (m_CmdID[0] + i, m_Files[i]);
			}
			else //if( m_pMenu->GetLabel(m_CmdID[0] + i).Cmp(m_Files[i]) )
			{
				m_pMenu->SetLabel(m_CmdID[0] + i, m_Files[i]);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Data_Menu_File::Add(const wxString &File)
{
	Del(File);

	if( wxFileExists(File) )
	{
		m_Files.Insert(File, 0);

		while( m_Files.Count() > m_CmdID[1] )
		{
			m_Files.RemoveAt(m_CmdID[1]);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Data_Menu_File::Del(const wxString &File)
{
	for(size_t i=m_Files.GetCount(); i>0; i--)
	{
		if( !m_Files[i - 1].Cmp(File) )
		{
			m_Files.RemoveAt(i - 1);
		}
	}
}

//---------------------------------------------------------
bool CWKSP_Data_Menu_File::Get(wxArrayString &Files, bool bAppend)
{
	if( !bAppend )
	{
		Files.Clear();
	}

	for(int i=0; i<m_Files.GetCount(); i++)
	{
		if( wxFileExists(m_Files[i]) )
		{
			Files.Add(m_Files[i]);
		}
	}

	return( Files.Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Menu_File::Open(int CmdID)
{
	bool bSuccess = false; int i = CmdID - m_CmdID[0];

	if( i >= 0 && i < m_Files.GetCount() )
	{
		switch( m_DataType )
		{
		case SG_DATAOBJECT_TYPE_Undefined:
			bSuccess	= g_pData->Open(wxString(m_Files[i]));
			break;

		case SG_DATAOBJECT_TYPE_Table     :
		case SG_DATAOBJECT_TYPE_Shapes    :
		case SG_DATAOBJECT_TYPE_TIN       :
		case SG_DATAOBJECT_TYPE_PointCloud:
		case SG_DATAOBJECT_TYPE_Grid      :
		case SG_DATAOBJECT_TYPE_Grids     :
			bSuccess	= g_pData->Open(wxString(m_Files[i]), m_DataType) != NULL;
			break;

		default:
			break;
		}
	}

	return( bSuccess );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
