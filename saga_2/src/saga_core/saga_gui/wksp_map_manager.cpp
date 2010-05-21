
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
#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_data_manager.h"
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

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	bool			bValue;
	double			dValue;
	long			lValue;
	CSG_Parameter	*pNode_0, *pNode_1;

	m_Parameters.Create(this, LNG(""), LNG(""));

	//-----------------------------------------------------
	pNode_0	= m_Parameters.Add_Node(
		NULL	, "NODE_DEFAULTS"	, LNG("[CAP] Defaults"),
		LNG("")
	);

	m_Parameters.Add_Value(
		pNode_0	, "GOTO_NEWLAYER"	, LNG("[CAP] Zoom to added layer"),
		LNG(""),
		PARAMETER_TYPE_Bool,
		CONFIG_Read(wxT("/MAPS"), wxT("GOTO_NEWLAYER"), bValue) ? bValue : true
	);

	//-----------------------------------------------------
	pNode_1	= m_Parameters.Add_Node(
		pNode_0	, "NODE_FRAME"		, LNG("[CAP] Frame"),
		LNG("")
	);

	m_Parameters.Add_Value(
		pNode_1	, "FRAME_SHOW"		, LNG("[CAP] Show"),
		LNG(""),
		PARAMETER_TYPE_Bool,
		CONFIG_Read(wxT("/MAPS"), wxT("FRAME_SHOW"), bValue) ? bValue : true
	);

	m_Parameters.Add_Value(
		pNode_1	, "FRAME_WIDTH"		, LNG("[CAP] Width"),
		LNG(""),
		PARAMETER_TYPE_Int,
		CONFIG_Read(wxT("/MAPS"), wxT("FRAME_WIDTH"), lValue) ? lValue : 17,
		5, true
	);

	//-----------------------------------------------------
	pNode_1	= m_Parameters.Add_Node(
		pNode_0	, "NODE_CLIPBOARD"	, LNG("[CAP] Clipboard"),
		LNG("")
	);

	m_Parameters.Add_Value(
		pNode_1	, "CLIP_NX"			, LNG("[CAP] Width"),
		LNG(""),
		PARAMETER_TYPE_Int,
		CONFIG_Read(wxT("/MAPS"), wxT("CLIP_NX"), lValue) ? lValue : 400
	);

	m_Parameters.Add_Value(
		pNode_1	, "CLIP_NY"			, LNG("[CAP] Height"),
		LNG(""),
		PARAMETER_TYPE_Int,
		CONFIG_Read(wxT("/MAPS"), wxT("CLIP_NY"), lValue) ? lValue : 400
	);

	m_Parameters.Add_Value(
		pNode_1	, "CLIP_FRAME"		, LNG("[CAP] Frame Width"),
		LNG(""),
		PARAMETER_TYPE_Int,
		CONFIG_Read(wxT("/MAPS"), wxT("CLIP_FRAME"), lValue) ? lValue : 17,
		5, true
	);

	m_Parameters.Add_Value(
		pNode_1	, "CLIP_LEGEND_SCALE", LNG("[CAP] Legend Scale"),
		LNG(""),
		PARAMETER_TYPE_Double,
		CONFIG_Read(wxT("/MAPS"), wxT("CLIP_LEGEND_SCALE"), dValue) ? dValue : 2.0,
		1.0, true
	);

	m_Parameters.Add_Value(
		pNode_1	, "CLIP_LEGEND_FRAME", LNG("[CAP] Legend Frame Width"),
		LNG(""),
		PARAMETER_TYPE_Int,
		CONFIG_Read(wxT("/MAPS"), wxT("CLIP_LEGEND_FRAME"), lValue) ? lValue : 10,
		0, true
	);

	m_Parameters.Add_Value(
		pNode_1	, "CLIP_LEGEND_COLOR", LNG("[CAP] Legend Border Colour"),
		LNG(""),
		PARAMETER_TYPE_Color,
		CONFIG_Read(wxT("/MAPS"), wxT("CLIP_LEGEND_COLOR"), lValue) ? lValue : SG_GET_RGB(0, 0, 0)
	);
}

//---------------------------------------------------------
CWKSP_Map_Manager::~CWKSP_Map_Manager(void)
{
	CONFIG_Write(wxT("/MAPS"), wxT("GOTO_NEWLAYER")	,		m_Parameters("GOTO_NEWLAYER")	->asBool());
	CONFIG_Write(wxT("/MAPS"), wxT("FRAME_SHOW")	,		m_Parameters("FRAME_SHOW")		->asBool());
	CONFIG_Write(wxT("/MAPS"), wxT("FRAME_WIDTH")	, (long)m_Parameters("FRAME_WIDTH")		->asInt());
	CONFIG_Write(wxT("/MAPS"), wxT("CLIP_NX")		, (long)m_Parameters("CLIP_NX")			->asInt());
	CONFIG_Write(wxT("/MAPS"), wxT("CLIP_NY")		, (long)m_Parameters("CLIP_NY")			->asInt());
	CONFIG_Write(wxT("/MAPS"), wxT("CLIP_FRAME")	, (long)m_Parameters("CLIP_FRAME")		->asInt());

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
	return( LNG("[CAP] Maps") );
}

//---------------------------------------------------------
wxString CWKSP_Map_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("%d %s"), Get_Count(), Get_Count() == 1 ? LNG("[CAP] Map") : LNG("[CAP] Maps"));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(LNG("[CAP] Maps"));

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
