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

	pNode_1	= m_Parameters.Add_Choice(
		pNode	, "SERVER"		, _TL("Server"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("Open Street Map"),
			_TL("MapQuest"),
			_TL("Google Map"),
			_TL("Google Satellite"),
			_TL("Google Hybrid"),
			_TL("Google Terrain"),
			_TL("Google Terrain, Streets and Water"),
			_TL("ArcGIS MapServer Tiles"),
			_TL("user defined")
		), 0
	);

	m_Parameters.Add_String(
		pNode_1	, "SERVER_USER"	, _TL("Server"),
		_TL(""),
		"tile.openstreetmap.org/${z}/${x}/${y}.png"
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

	m_Parameters.Add_Double(
		pNode	, "TRANSPARENCY", _TL("Transparency [%]"),
		_TL(""),
		0.0, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Bool(
		pNode	, "GRAYSCALE"	, _TL("Gray Scale Image"),
		_TL(""),
		false
	);

	m_Parameters.Add_Double(
		pNode	, "RESOLUTION"	, _TL("Resolution"),
		_TL("resolution measured in screen pixels"),
		1.0, 1.0, true
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
	s	+= "<hr>";
	s	+= _TL("Be sure to read and understand the usage agreement or terms of service before you use a base map server.");
	s	+= "<ul>";
	s	+= "<li><a href=\"www.openstreetmap.org\">Open Street Map</a></li>";
	s	+= "<li><a href=\"open.mapquest.co.uk\">MapQuest</a></li>";
	s	+= "<li><a href=\"maps.google.com/intl/en/help/terms_maps.html\">Google Maps</a></li>";
	s	+= "<li><a href=\"services.arcgisonline.com\">ArcGIS MapServer</a></li>";
	s	+= "</ul>";

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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Map_BaseMap::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SERVER") )
		{
			pParameters->Set_Enabled("SERVER_USER", pParameter->asInt() >= 8);	// user defined
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

		CSG_Grid	*pBaseMap, BaseMap;
		
		if( m_Parameters("RESOLUTION")->asDouble() > 1.0 )
		{
			BaseMap.Create(CSG_Grid_System(m_Parameters("RESOLUTION")->asDouble() * System.Get_Cellsize(), System.Get_Extent(true)), SG_DATATYPE_Int);

			pBaseMap	= &BaseMap;
		}
		else
		{
			pBaseMap	= &m_BaseMap;
		}

		pBaseMap->Get_Projection()	= Get_Map()->Get_Projection();

		pModule->Settings_Push();

		if( pModule->Set_Parameter("TARGET"     , pBaseMap)
		&&  pModule->Set_Parameter("TARGET_MAP" , pBaseMap)
		&&  pModule->Set_Parameter("SERVER"     , m_Parameters("SERVER"     ))
		&&  pModule->Set_Parameter("SERVER_USER", m_Parameters("SERVER_USER"))
		&&  pModule->Set_Parameter("GRAYSCALE"  , m_Parameters("GRAYSCALE"  ))
		&&  pModule->On_Before_Execution() && pModule->Execute() )
		{
			if( &m_BaseMap != pBaseMap )
			{
				m_BaseMap.Assign(pBaseMap, GRID_RESAMPLING_NearestNeighbour);
			}
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
	if( dc_Map.IMG_Draw_Begin(m_Parameters("TRANSPARENCY")->asDouble() / 100.0) )
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
