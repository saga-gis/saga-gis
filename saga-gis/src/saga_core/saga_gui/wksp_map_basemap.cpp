/**********************************************************
 * Version $Id: wksp_map_basemap.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                  wksp_map_basemap.cpp                 //
//                                                       //
//          Copyright (C) 2016 by Olaf Conrad            //
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
#include <wx/window.h>

#include <saga_api/saga_api.h>

#include "helper.h"
#include "dc_helper.h"

#include "res_commands.h"

#include "wksp_map.h"
#include "wksp_map_basemap.h"
#include "wksp_map_dc.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_BaseMap::CWKSP_Map_BaseMap(CSG_MetaData *pEntry)
{
	m_bShow		= true;

	//-----------------------------------------------------
	CSG_Parameter	*pNode, *pNode_1;

	m_Parameters.Set_Name      ("BASEMAP");
	m_Parameters.Set_Identifier("BASEMAP");

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_GENERAL"	,_TL("General")	, _TL(""));

	m_Parameters.Add_String(
		pNode	, "NAME"		, _TL("Name"),
		_TL(""),
		_TL("Base Map")
	);

	m_Parameters.Add_Choice(
		pNode	, "SERVER"		, _TL("Server"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Open Street Map"),
			_TL("MapQuest")
		), 0
	);

	//-----------------------------------------------------
	pNode_1	= m_Parameters.Add_Value(
		pNode	, "SHOW_ALWAYS"	, _TL("Show at all scales"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Range(
		pNode_1	, "SHOW_RANGE"	, _TL("Scale Range"),
		_TL("only show within scale range; values are given as extent measured in map units"),
		100.0, 1000.0, 0.0, true
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_DISPLAY"	,_TL("Display")	, _TL(""));

	m_Parameters.Add_Value(
		pNode	, "TRANSPARENCY", _TL("Transparency [%]"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);

	//-----------------------------------------------------
	if( pEntry )
	{
		Load(*pEntry);
	}
}

//---------------------------------------------------------
CWKSP_Map_BaseMap::~CWKSP_Map_BaseMap(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_BaseMap::Load(CSG_MetaData &Entry)
{
	return( m_Parameters.Serialize(Entry, false) );
}

//---------------------------------------------------------
bool CWKSP_Map_BaseMap::Save(CSG_MetaData &Entry)
{
	return( m_Parameters.Serialize(*Entry.Add_Child("BASEMAP"), true) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Map_BaseMap::Get_Name(void)
{
	wxString	Name(m_Parameters("NAME")->asString());

	if( !m_bShow )
	{
		return( "[" + Name + "]" );
	}

	return( Name );
}

//---------------------------------------------------------
wxString CWKSP_Map_BaseMap::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format("<h4>%s</h4>", _TL("Base Map"));

	s	+= "<table border=\"0\">";

	DESC_ADD_STR(_TL("Name"      ), m_Parameters("NAME")->asString());
	DESC_ADD_STR(_TL("Projection"), Get_Map()->Get_Projection().Get_Description().c_str());

	s	+= wxT("</table>");

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map_BaseMap::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(m_Parameters("NAME")->asString());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_LAYER_SHOW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_TOP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_UP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_DOWN);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_BOTTOM);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_BaseMap::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Item::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
	case ID_CMD_MAPS_LAYER_SHOW:
		m_bShow	= !m_bShow;
		((wxTreeCtrl *)Get_Control())->SetItemText(GetId(), Get_Name());
		((CWKSP_Map *)Get_Manager())->View_Refresh(true);
		break;

	case ID_CMD_MAPS_MOVE_TOP:
		if( Get_Manager()->Move_Top(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_BOTTOM:
		if( Get_Manager()->Move_Bottom(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_UP:
		if( Get_Manager()->Move_Up(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_DOWN:
		if( Get_Manager()->Move_Down(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map_BaseMap::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Item::On_Command_UI(event) );

	case ID_CMD_MAPS_LAYER_SHOW:
		event.Check(m_bShow);
		break;

	case ID_CMD_MAPS_MOVE_TOP:
	case ID_CMD_MAPS_MOVE_UP:
		event.Enable(Get_Index() > 0);
		break;

	case ID_CMD_MAPS_MOVE_DOWN:
	case ID_CMD_MAPS_MOVE_BOTTOM:
		event.Enable(Get_Index() < Get_Manager()->Get_Count() - 1);
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
int CWKSP_Map_BaseMap::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "INTERVAL") )
		{
			pParameters->Set_Enabled("FIXED", pParameter->asInt() == 0);
		}
	}

	return( CWKSP_Base_Item::On_Parameter_Changed(pParameters, pParameter, Flags) );
}

//---------------------------------------------------------
void CWKSP_Map_BaseMap::Parameters_Changed(void)
{
	CWKSP_Base_Item::Parameters_Changed();

	m_BaseMap.Destroy();	// forcing a base map refresh

	Get_Map()->View_Refresh(true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_BaseMap::Set_BaseMap(const CSG_Grid_System &System)
{
	m_BaseMap.Destroy();

	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module("io_gdal", 9);

	if(	pModule && Get_Map()->Get_Projection().is_Okay() )
	{
		SG_UI_Msg_Lock     (true);
		SG_UI_Progress_Lock(true);

		m_BaseMap.Create(System, SG_DATATYPE_Int);
		m_BaseMap.Get_Projection()	= Get_Map()->Get_Projection();

		pModule->Settings_Push();

		if( pModule->Set_Parameter("TARGET"    , &m_BaseMap)
		&&  pModule->Set_Parameter("TARGET_MAP", &m_BaseMap)
		&&  pModule->Set_Parameter("SERVER"    , m_Parameters("SERVER"))
		&&  pModule->On_Before_Execution() && pModule->Execute() )
		{
		}
		else
		{
			m_BaseMap.Destroy();
		}

		pModule->Settings_Pop();

		SG_UI_Msg_Lock     (false);
		SG_UI_Progress_Lock(false);
	}

	return( m_BaseMap.is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_BaseMap::Draw(CWKSP_Map_DC &dc_Map)
{
	if( !m_Parameters("SHOW_ALWAYS")->asBool() )
	{
		CSG_Parameter_Range	*pRange	= m_Parameters("SHOW_RANGE")->asRange();
		double	dRange	= dc_Map.m_rWorld.Get_XRange() > dc_Map.m_rWorld.Get_YRange() ? dc_Map.m_rWorld.Get_XRange() : dc_Map.m_rWorld.Get_YRange();

		if( dRange < pRange->Get_LoVal() || pRange->Get_HiVal() < dRange )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	CSG_Grid_System	System(dc_Map.m_DC2World, dc_Map.m_rWorld.Get_XMin(), dc_Map.m_rWorld.Get_YMin(), dc_Map.m_rDC.GetWidth(), dc_Map.m_rDC.GetHeight());

	if( !System.is_Equal(m_BaseMap.Get_System()) && !Set_BaseMap(System) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( dc_Map.IMG_Draw_Begin(m_Parameters("TRANSPARENCY")->asDouble()) )
	{
		#pragma omp parallel for
		for(int y=0; y<m_BaseMap.Get_NY(); y++)	for(int x=0, yy=m_BaseMap.Get_NY()-y-1; x<m_BaseMap.Get_NX(); x++)
		{
			dc_Map.IMG_Set_Pixel(x, y, m_BaseMap.asInt(x, yy));
		}

		dc_Map.IMG_Draw_End();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
