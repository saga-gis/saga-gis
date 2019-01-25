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
#include "res_dialogs.h"

#include "wksp_map.h"
#include "wksp_map_manager.h"
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
	m_bShow	= true;
	m_pTool	= NULL;	// remember the last created base map tool instance

	//-----------------------------------------------------
	m_Parameters.Set_Name      ("BASEMAP");
	m_Parameters.Set_Identifier("BASEMAP");

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_GENERAL",_TL("General")	, _TL(""));

	m_Parameters.Add_String("NODE_GENERAL",
		"NAME"		, _TL("Name"),
		_TL(""),
		_TL("Base Map")
	);

	m_Parameters.Add_Choice("NODE_GENERAL",
		"SERVER"	, _TL("Server"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("Open Street Map"),
			_TL("Google Map"),
			_TL("Google Satellite"),
			_TL("Google Hybrid"),
			_TL("Google Terrain"),
			_TL("Google Terrain, Streets and Water"),
			_TL("ArcGIS MapServer Tiles"),
			_TL("user defined")
		), 0
	);

	m_Parameters.Add_String("SERVER",
		"SERVER_USER", _TL("Server"),
		_TL(""),
		"tile.openstreetmap.org/${z}/${x}/${y}.png"
	);

	m_Parameters.Add_Bool("NODE_GENERAL",
		"CACHE"		, _TL("Cache"),
		_TL("Enable local disk cache. Allows for offline operation."),
		g_pMaps->Get_Parameter("CACHE")->asBool()
	);

	m_Parameters.Add_FilePath("CACHE",
		"CACHE_DIR"	, _TL("Cache Directory"),
		_TL("If not specified the cache will be created in the current user's temporary directory."),
		NULL, g_pMaps->Get_Parameter("CACHE_DIR")->asString(), false, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Bool("NODE_GENERAL",
		"SHOW_ALWAYS"	, _TL("Show at all scales"),
		_TL(""),
		true
	);

	m_Parameters.Add_Range("NODE_GENERAL",
		"SHOW_RANGE"	, _TL("Scale Range"),
		_TL("only show within scale range; values are given as extent measured in map units"),
		100.0, 1000.0, 0.0, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_DISPLAY",_TL("Display")	, _TL(""));

	m_Parameters.Add_Double("NODE_DISPLAY",
		"TRANSPARENCY"	, _TL("Transparency [%]"),
		_TL(""),
		0.0, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Double("NODE_DISPLAY",
		"BRIGHTNESS"	, _TL("Maximum Brightness [%]"),
		_TL("Brightness threshold below a pixel is displayed. Set to 100% to display all (default)."),
		100.0, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Bool("NODE_DISPLAY",
		"GRAYSCALE"		, _TL("Gray Scale Image"),
		_TL(""),
		false
	);

	m_Parameters.Add_Double("NODE_DISPLAY",
		"RESOLUTION"	, _TL("Resolution"),
		_TL("resolution measured in screen pixels"),
		1.0, 1.0, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Choice("NODE_DISPLAY",
		"POSITION"	, _TL("Position"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("top"),
			_TL("bottom")
		), 1
	)->Set_Enabled(false);

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
		if(	pParameter->Cmp_Identifier("SHOW_ALWAYS") )
		{
			pParameters->Set_Enabled("SHOW_RANGE" , pParameter->asBool() == false);
		}

		if(	pParameter->Cmp_Identifier("SERVER") )
		{
			pParameters->Set_Enabled("SERVER_USER", pParameter->asInt() >= pParameter->asChoice()->Get_Count() - 1);	// user defined
		}

		if( pParameter->Cmp_Identifier("CACHE") )
		{
			pParameters->Set_Enabled("CACHE_DIR"  , pParameter->asBool());
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
bool CWKSP_Map_BaseMap::Dlg_Parameters(void)
{
	m_Parameters("POSITION")->Set_Enabled(true);

	bool	bResult	= DLG_Parameters(&m_Parameters);
	
	m_Parameters("POSITION")->Set_Enabled(false);

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_BaseMap::Set_BaseMap(const CSG_Grid_System &System)
{
	CSG_Tool	*pTool;

	if(	!Get_Map()->Get_Projection().is_Okay() || !(pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 9)) )
	{
		m_BaseMap.Destroy();

		return( false );
	}

	SG_UI_ProgressAndMsg_Lock(true);

	//-----------------------------------------------------
	CSG_Grid	BaseMap;

	if( m_Parameters("RESOLUTION")->asDouble() > 1.0 )
	{
		BaseMap.Create(CSG_Grid_System(m_Parameters("RESOLUTION")->asDouble() * System.Get_Cellsize(), System.Get_Extent(true)), SG_DATATYPE_Int);
	}
	else
	{
		BaseMap.Create(System);
	}

	BaseMap.Get_Projection()	= Get_Map()->Get_Projection();

	m_pTool	= pTool;	// remember the last created base map tool instance

	pTool->Set_Manager(NULL);

	if( pTool->Set_Parameter("TARGET"     , &BaseMap)
	&&  pTool->Set_Parameter("TARGET_MAP" , &BaseMap)
	&&  pTool->Set_Parameter("SERVER"     , m_Parameters("SERVER"     ))
	&&  pTool->Set_Parameter("SERVER_USER", m_Parameters("SERVER_USER"))
	&&  pTool->Set_Parameter("CACHE"      , m_Parameters("CACHE"      ))
	&&  pTool->Set_Parameter("CACHE_DIR"  , m_Parameters("CACHE_DIR"  ))
	&&  pTool->Set_Parameter("GRAYSCALE"  , m_Parameters("GRAYSCALE"  ))
	&&  pTool->On_Before_Execution() && pTool->Execute() && pTool == m_pTool )
	{
		m_pTool	= NULL;

		m_BaseMap.Create(System, SG_DATATYPE_Int);

		if( System == BaseMap.Get_System() )
		{
			#pragma omp parallel for
			for(int i=0; i<m_BaseMap.Get_NCells(); i++)
			{
				m_BaseMap.Set_Value(i, BaseMap.asInt(i));
			}
		}
		else
		{
			#pragma omp parallel for
			for(int y=0; y<m_BaseMap.Get_NY(); y++)	for(int x=0; x<m_BaseMap.Get_NX(); x++)
			{
				m_BaseMap.Set_Value(x, y, BaseMap.Get_Value(System.Get_Grid_to_World(x, y), GRID_RESAMPLING_BSpline, true));
			}
		}

		if( m_Parameters("BRIGHTNESS")->asDouble() < 100.0 )
		{
			int	Threshold	= (int)(0.5 + 3 * 255 * m_Parameters("BRIGHTNESS")->asDouble() / 100.0);

			#pragma omp parallel for
			for(int i=0; i<m_BaseMap.Get_NCells(); i++)
			{
				int	c	= m_BaseMap.asInt(i);

				if( Threshold < (SG_GET_R(c) + SG_GET_G(c) + SG_GET_B(c)) )
				{
					m_BaseMap.Set_NoData(i);
				}
			}
		}
	}
	else if( pTool == m_pTool )
	{
		m_pTool	= NULL;

		m_BaseMap.Destroy();
	}

	//-----------------------------------------------------
	SG_UI_ProgressAndMsg_Lock(false);

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

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

		if( dRange < pRange->Get_Min() || pRange->Get_Max() < dRange )
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
			if( !m_BaseMap.is_NoData(x, yy) )
			{
				dc_Map.IMG_Set_Pixel(x, y, m_BaseMap.asInt(x, yy));
			}
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
