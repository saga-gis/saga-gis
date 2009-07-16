
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
//              WKSP_PointCloud_Manager.cpp              //
//                                                       //
//          Copyright (C) 2009 by Olaf Conrad            //
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
#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "wksp_data_manager.h"

#include "wksp_map_manager.h"

#include "wksp_pointcloud_manager.h"
#include "wksp_pointcloud.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_PointCloud_Manager::CWKSP_PointCloud_Manager(void)
{
}

//---------------------------------------------------------
CWKSP_PointCloud_Manager::~CWKSP_PointCloud_Manager(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_PointCloud_Manager::Get_Name(void)
{
	return( LNG("[CAP] PointCloud") );
}

//---------------------------------------------------------
wxString CWKSP_PointCloud_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b>:%d<br>"), LNG("[CAP] PointCloud"), Get_Count());

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_PointCloud_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(LNG("[CAP] PointCloud"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_POINTCLOUD_OPEN);

	if( Get_Count() > 0 )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	}

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_PointCloud_Manager::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
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
CWKSP_PointCloud * CWKSP_PointCloud_Manager::Get_PointCloud(CSG_PointCloud *pPointCloud)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pPointCloud == Get_PointCloud(i)->Get_PointCloud() )
		{
			return( Get_PointCloud(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_PointCloud_Manager::Exists(CSG_PointCloud *pPointCloud)
{
	return( Get_PointCloud(pPointCloud) != NULL );
}

//---------------------------------------------------------
CWKSP_PointCloud * CWKSP_PointCloud_Manager::Add(CSG_PointCloud *pPointCloud)
{
	CWKSP_PointCloud	*pItem;

	if( pPointCloud && pPointCloud->is_Valid() && !Exists(pPointCloud) && Add_Item(pItem = new CWKSP_PointCloud(pPointCloud)) )
	{
		return( pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_PointCloud * CWKSP_PointCloud_Manager::Get_byFileName(const wxChar *File_Name)
{
	CSG_String	s(File_Name);

	for(int i=0; i<Get_Count(); i++)
	{
		if( !s.Cmp(Get_PointCloud(i)->Get_PointCloud()->Get_File_Name()) )
		{
			return( Get_PointCloud(i)->Get_PointCloud() );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_PointCloud_Manager::Update(CSG_PointCloud *pPointCloud, CSG_Parameters *pParameters)
{
	CWKSP_PointCloud	*pItem;

	if( (pItem = Get_PointCloud(pPointCloud)) != NULL )
	{
		pItem->DataObject_Changed(pParameters);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_PointCloud_Manager::Update_Views(CSG_PointCloud *pPointCloud)
{
	CWKSP_PointCloud	*pItem;

	if( (pItem = Get_PointCloud(pPointCloud)) != NULL )
	{
		pItem->Update_Views(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_PointCloud_Manager::Show(CSG_PointCloud *pPointCloud, int Map_Mode)
{
	CWKSP_PointCloud	*pItem;

	if( (pItem = Get_PointCloud(pPointCloud)) != NULL )
	{
		switch( Map_Mode )
		{
		case SG_UI_DATAOBJECT_SHOW:
			return( pItem->Show() );

		case SG_UI_DATAOBJECT_SHOW_NEW_MAP:
			g_pMaps->Add(pItem, NULL);

		case SG_UI_DATAOBJECT_SHOW_LAST_MAP:
			return( pItem->Show(g_pMaps->Get_Map(g_pMaps->Get_Count() - 1)) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_PointCloud_Manager::asImage(CSG_PointCloud *pPointCloud, CSG_Grid *pImage)
{
	CWKSP_PointCloud	*pItem;

	if( (pItem = Get_PointCloud(pPointCloud)) != NULL )
	{
		return( pItem->asImage(pImage) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_PointCloud_Manager::Get_Colors(CSG_PointCloud *pPointCloud, CSG_Colors *pColors)
{
	CWKSP_PointCloud	*pItem;

	if( (pItem = Get_PointCloud(pPointCloud)) != NULL )
	{
		return( pItem->Get_Colors(pColors) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_PointCloud_Manager::Set_Colors(CSG_PointCloud *pPointCloud, CSG_Colors *pColors)
{
	CWKSP_PointCloud	*pItem;

	if( (pItem = Get_PointCloud(pPointCloud)) != NULL )
	{
		pItem->DataObject_Changed(pColors);

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
