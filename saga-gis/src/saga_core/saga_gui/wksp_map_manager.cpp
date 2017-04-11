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
//                 WKSP_Map_Manager.cpp                  //
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
#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_data_manager.h"
#include "wksp_map_buttons.h"

#include "wksp_layer.h"

#include "wksp_map_control.h"
#include "wksp_map_manager.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_Manager	*g_pMaps	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_Manager::CWKSP_Map_Manager(void)
{
	g_pMaps		= this;

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_DEFAULTS", _TL("Defaults for New Maps"), _TL(""));

	m_Parameters.Add_Bool("NODE_DEFAULTS",
		"GOTO_NEWLAYER"	, _TL("Zoom to added layer"),
		_TL(""),
		true
	);

	m_Parameters.Add_Bool("NODE_DEFAULTS",
		"SCALE_BAR"		, _TL("Show Scale Bar"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("NODE_DEFAULTS", "NODE_FRAME", _TL("Frame"), _TL(""));

	m_Parameters.Add_Bool("NODE_FRAME",
		"FRAME_SHOW"	, _TL("Show"),
		_TL(""),
		true
	);

	m_Parameters.Add_Int("NODE_FRAME",
		"FRAME_WIDTH"	, _TL("Width"),
		_TL(""),
		17, 5, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_CLIPBOARD", _TL("Clipboard"), _TL(""));

	m_Parameters.Add_Int("NODE_CLIPBOARD",
		"CLIP_NX"			, _TL("Width"),
		_TL(""),
		400, 10, true
	);

	m_Parameters.Add_Int("NODE_CLIPBOARD",
		"CLIP_NY"			, _TL("Height"),
		_TL(""),
		400, 10, true
	);

	m_Parameters.Add_Int("NODE_CLIPBOARD",
		"CLIP_FRAME"		, _TL("Frame Width"),
		_TL(""),
		17, 0, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("NODE_CLIPBOARD", "NODE_CLIP_LEGEND", _TL("Legend"), _TL(""));

	m_Parameters.Add_Double("NODE_CLIP_LEGEND",
		"CLIP_LEGEND_SCALE", _TL("Scale"),
		_TL(""),
		2.0, 1.0, true
	);

	m_Parameters.Add_Int("NODE_CLIP_LEGEND",
		"CLIP_LEGEND_FRAME", _TL("Frame Width"),
		_TL(""),
		10, 0, true
	);

	m_Parameters.Add_Color("NODE_CLIP_LEGEND",
		"CLIP_LEGEND_COLOR", _TL("Border Color"),
		_TL(""),
		SG_GET_RGB(0, 0, 0)
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_THUMBNAILS", _TL("Thumbnails"), _TL(""));

	m_Parameters.Add_Int("NODE_THUMBNAILS",
		"THUMBNAIL_SIZE"		, _TL("Thumbnail Size"),
		_TL(""),
		75, 10, true
	);

	m_Parameters.Add_Color("NODE_THUMBNAILS",
		"THUMBNAIL_SELCOLOR"	, _TL("Selection Color"),
		_TL(""),
		Get_Color_asInt(SYS_Get_Color(wxSYS_COLOUR_BTNSHADOW))
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_GLOBALS", _TL("Global Settings"), _TL(""));

	m_Parameters.Add_Bool("NODE_GLOBALS",
		"CACHE"		, _TL("Cache"),
		_TL("Enable local disk cache. Allows for offline operation."),
		false
	);

	m_Parameters.Add_FilePath("CACHE",
		"CACHE_DIR"	, _TL("Cache Directory"),
		_TL("If not specified the cache will be created in the current user's temporary directory."),
		NULL, NULL, false, true
	);

	//-----------------------------------------------------
	CONFIG_Read("/MAPS", &m_Parameters);
}

//---------------------------------------------------------
CWKSP_Map_Manager::~CWKSP_Map_Manager(void)
{
	CONFIG_Write("/MAPS", &m_Parameters);

	g_pMaps		= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Map_Manager::Get_Name(void)
{
	return( _TL("Maps") );
}

//---------------------------------------------------------
wxString CWKSP_Map_Manager::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format("<h4>%s</h4>", _TL("Maps"));

	s	+= "<table border=\"0\">";

	DESC_ADD_INT(_TL("Number of Maps"), Get_Count());

	s	+= "</table>";

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(_TL("Maps"));

	if( Get_Count() > 0 )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SEARCH);
	}

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Manager::On_Command(int Cmd_ID)
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
void CWKSP_Map_Manager::Parameters_Changed(void)
{
	g_pMap_Buttons->Update_Buttons();

	CWKSP_Base_Manager::Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Manager::Exists(CWKSP_Map *pMap)
{
	if( pMap )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			if( pMap == Get_Map(i) )
			{
				return( true );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Map_Manager::Close(bool bSilent)
{
	return( g_pMap_Ctrl->Close(bSilent) );
}

//---------------------------------------------------------
bool CWKSP_Map_Manager::Add(CWKSP_Map *pMap)
{
	if( pMap )
	{
		if( !Exists(pMap) )
		{
			Add_Item(pMap);
		}

		return( true );
	}

	return( false );
}

bool CWKSP_Map_Manager::Add(CWKSP_Layer *pLayer)
{
	int		iMap;

	if( (iMap = DLG_Maps_Add()) >= 0 && Add(pLayer, Get_Map(iMap)) )
	{
		Get_Map(iMap)->View_Show(true);

		return( true );
	}

	return( false );
}

bool CWKSP_Map_Manager::Add(CWKSP_Layer *pLayer, CWKSP_Map *pMap)
{
	if( pLayer )
	{
		if( pMap == NULL )
		{
			Add_Item(pMap = new CWKSP_Map);
		}
		else if( !Exists(pMap) )
		{
			Add_Item(pMap);
		}

		pMap->Add_Layer(pLayer);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Map_Manager::Del(CWKSP_Layer *pLayer)
{
	int		i, n;

	for(i=Get_Count()-1, n=0; i>=0; i--)
	{
		if( g_pMap_Ctrl->Del_Item(Get_Map(i), pLayer) )
		{
			n++;
		}
	}

	return( n > 0 );
}

//---------------------------------------------------------
bool CWKSP_Map_Manager::Update(CWKSP_Layer *pLayer, bool bMapsOnly)
{
	int		i, n;

	for(i=0, n=0; i<Get_Count(); i++)
	{
		if( Get_Map(i)->Update(pLayer, bMapsOnly) )
		{
			n++;
		}
	}

	return( n > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
