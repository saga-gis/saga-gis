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
	m_Recent	= NULL;
	m_DataType	= SG_DATAOBJECT_TYPE_Undefined;
}

//---------------------------------------------------------
CWKSP_Data_Menu_File::~CWKSP_Data_Menu_File(void)
{
	Destroy();
}

//---------------------------------------------------------
void CWKSP_Data_Menu_File::Destroy(void)
{
	if( m_Recent )
	{
		for(int i=0; i<m_Recent_Count; i++)
		{
			CONFIG_Write(wxString::Format("RECENT_FILES/%s", m_Recent_Group.c_str()), wxString::Format("FILE_%02d", i + 1), m_Recent[i]);
		}

		delete[](m_Recent);
	}

	m_Recent	= NULL;
	m_DataType	= SG_DATAOBJECT_TYPE_Undefined;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_Data_Menu_File::Create(TSG_Data_Object_Type DataType)
{
	Destroy();

	m_DataType	= DataType;

	//-----------------------------------------------------
	switch( m_DataType )
	{
	case SG_DATAOBJECT_TYPE_Undefined:
		m_Recent_First	= ID_CMD_DATA_PROJECT_RECENT_FIRST;
		m_Recent_Count	= ID_CMD_DATA_PROJECT_RECENT_LAST - m_Recent_First + 1;
		m_Recent_Group	= "Project";
		break;

	case SG_DATAOBJECT_TYPE_Table:
		m_Recent_First	= ID_CMD_TABLE_RECENT_FIRST;
		m_Recent_Count	= ID_CMD_TABLE_RECENT_LAST        - m_Recent_First + 1;
		m_Recent_Group	= "Table";
		break;

	case SG_DATAOBJECT_TYPE_Shapes:
		m_Recent_First	= ID_CMD_SHAPES_RECENT_FIRST;
		m_Recent_Count	= ID_CMD_SHAPES_RECENT_LAST       - m_Recent_First + 1;
		m_Recent_Group	= "Shapes";
		break;

	case SG_DATAOBJECT_TYPE_TIN:
		m_Recent_First	= ID_CMD_TIN_RECENT_FIRST;
		m_Recent_Count	= ID_CMD_TIN_RECENT_LAST          - m_Recent_First + 1;
		m_Recent_Group	= "TIN";
		break;

	case SG_DATAOBJECT_TYPE_PointCloud:
		m_Recent_First	= ID_CMD_POINTCLOUD_RECENT_FIRST;
		m_Recent_Count	= ID_CMD_POINTCLOUD_RECENT_LAST   - m_Recent_First + 1;
		m_Recent_Group	= "Point Cloud";
		break;

	case SG_DATAOBJECT_TYPE_Grid:
		m_Recent_First	= ID_CMD_GRID_RECENT_FIRST;
		m_Recent_Count	= ID_CMD_GRID_RECENT_LAST         - m_Recent_First + 1;
		m_Recent_Group	= "Grid";
		break;

	case SG_DATAOBJECT_TYPE_Grids:
		m_Recent_First	= ID_CMD_GRIDS_RECENT_FIRST;
		m_Recent_Count	= ID_CMD_GRIDS_RECENT_LAST        - m_Recent_First + 1;
		m_Recent_Group	= "Grids";
		break;

	default:
		m_Recent_First	= 0;
		m_Recent_Count	= 0;
		m_Recent_Group	= "";
		break;
	}

	if( m_Recent_Count > 0 )
	{
		m_Recent	= new wxString[m_Recent_Count];

		for(int i=0, n=0; i<m_Recent_Count; i++)
		{
			wxString	Recent;

			CONFIG_Read(wxString::Format("RECENT_FILES/%s", m_Recent_Group.c_str()), wxString::Format("FILE_%02d", i + 1), Recent);

			if( wxFileExists(Recent) )
			{
				m_Recent[n++]	= Recent;
			}
		}
	}

	//-----------------------------------------------------
	wxMenu	*pMenu	= new wxMenu;

	Update(pMenu);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Data_Menu_File::Update(wxMenu *pMenu)
{
	bool	bFirst;
	int		i;

	//-----------------------------------------------------
	if( pMenu )
	{
		for(i=pMenu->GetMenuItemCount()-1; i>=0; i--)
		{
			pMenu->Destroy(pMenu->GetMenuItems()[i]);
		}

		//-------------------------------------------------
		switch( m_DataType )
		{
		case SG_DATAOBJECT_TYPE_Undefined:
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_OPEN);
		//	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_OPEN_ADD);
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_BROWSE);
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_CLOSE);
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_SAVE);
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_SAVE_AS);
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_COPY);
		//	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_COPY_DB);
			break;

		case SG_DATAOBJECT_TYPE_Table:
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_TABLE_OPEN);
			break;

		case SG_DATAOBJECT_TYPE_Shapes:
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_OPEN);
			break;

		case SG_DATAOBJECT_TYPE_TIN:
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_TIN_OPEN);
			break;

		case SG_DATAOBJECT_TYPE_PointCloud:
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_POINTCLOUD_OPEN);
			break;

		case SG_DATAOBJECT_TYPE_Grid:
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRID_OPEN);
			break;

		case SG_DATAOBJECT_TYPE_Grids:
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRIDS_OPEN);
			break;

		default:
			return;
		}

		//-------------------------------------------------
		for(i=m_Recent_Count-1; i>=0; i--)
		{
			if( !m_Recent[i].IsEmpty() && !wxFileExists(m_Recent[i]) )
			{
				Del(m_Recent[i]);
			}
		}

		for(i=0, bFirst=false; i<m_Recent_Count; i++)
		{
			if( m_Recent[i].Length() > 0 )
			{
				if( !bFirst )
				{
					bFirst	= true;
					pMenu->AppendSeparator();
				}

				pMenu->Append(m_Recent_First + i, m_Recent[i]);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Data_Menu_File::Add(const wxString &FileName)
{
	if( m_Recent && m_Recent_Count > 0 )
	{
		wxString	s_tmp(FileName);

		Del(FileName);

		for(int i=m_Recent_Count-1; i>0; i--)
		{
			m_Recent[i]	= m_Recent[i - 1];
		}

		m_Recent[0]	= s_tmp;
	}
}

//---------------------------------------------------------
void CWKSP_Data_Menu_File::Del(const wxString &FileName)
{
	if( m_Recent && m_Recent_Count > 0 && !FileName.IsEmpty() )
	{
		wxString	s_tmp(FileName);

		for(int i=m_Recent_Count-1; i>=0; i--)
		{
			if( m_Recent[i].Cmp(s_tmp) == 0 )
			{
				_Del(m_Recent_First + i);
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Data_Menu_File::_Del(int Cmd_ID)
{
	if( m_Recent && m_Recent_First <= Cmd_ID && Cmd_ID < m_Recent_First + m_Recent_Count )
	{
		for(int i=Cmd_ID-m_Recent_First; i<m_Recent_Count-1; i++)
		{
			m_Recent[i]	= m_Recent[i + 1];
		}

		m_Recent[m_Recent_Count - 1].Clear();
	}
}


//---------------------------------------------------------
bool CWKSP_Data_Menu_File::Get(wxArrayString &FileNames, bool bAppend)
{
	if( !bAppend )
	{
		FileNames.Clear();
	}

	if( m_Recent )
	{
		for(int i=0; i<m_Recent_Count; i++)
		{
			if( wxFileExists(m_Recent[i]) )
			{
				FileNames.Add(m_Recent[i]);
			}
		}
	}

	return( FileNames.Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Menu_File::Open(int Cmd_ID)
{
	bool	bSuccess	= false;

	if( m_Recent && m_Recent_First <= Cmd_ID && Cmd_ID < m_Recent_First + m_Recent_Count )
	{
		wxString	File(m_Recent[Cmd_ID - m_Recent_First]);

		switch( m_DataType )
		{
		case SG_DATAOBJECT_TYPE_Undefined:
			bSuccess	= g_pData->Open(File);
			break;

		case SG_DATAOBJECT_TYPE_Table     :
		case SG_DATAOBJECT_TYPE_Shapes    :
		case SG_DATAOBJECT_TYPE_TIN       :
		case SG_DATAOBJECT_TYPE_PointCloud:
		case SG_DATAOBJECT_TYPE_Grid      :
		case SG_DATAOBJECT_TYPE_Grids     :
			bSuccess	= g_pData->Open(File, m_DataType) != NULL;
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
