
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
//                    WKSP_Grid.cpp                      //
//                                                       //
//          Copyright (C) 2017 by Olaf Conrad            //
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
#include <wx/dc.h>
#include <wx/dcmemory.h>
#include <wx/image.h>
#include <wx/filename.h>
#include <wx/clipbrd.h>

#include <saga_gdi/sgdi_helper.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_dialogs.h"

#include "helper.h"

#include "active.h"
#include "active_attributes.h"

#include "wksp_map_control.h"

#include "wksp_layer_classify.h"
#include "wksp_layer_legend.h"

#include "wksp_data_manager.h"
#include "wksp_grid_manager.h"
#include "wksp_grids.h"

#include "data_source_pgsql.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grids::CWKSP_Grids(CSG_Grids *pGrids)
	: CWKSP_Layer(pGrids)
{
	On_Create_Parameters();

	DataObject_Changed();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grids::Get_Description(void)
{
	wxString s;

	//-----------------------------------------------------
	s += wxString::Format("<h4>%s</h4>", _TL("Grid Collection"));

	s += g_pData->Set_Description_Image(this);

	s += "<table border=\"0\">";

	DESC_ADD_STR (_TL("Name"               ), m_pObject->Get_Name());
	DESC_ADD_STR (_TL("Description"        ), m_pObject->Get_Description());

	if( SG_File_Exists(m_pObject->Get_File_Name(false)) )
	{
		DESC_ADD_STR(_TL("Data Source"     ), SG_File_Get_Path(m_pObject->Get_File_Name(false)      ).c_str());
		DESC_ADD_STR(_TL("File"            ), SG_File_Get_Name(m_pObject->Get_File_Name(false), true).c_str());

		if( m_pObject->Get_MetaData()("GDAL_DRIVER") )
		{
			DESC_ADD_STR(_TL("Driver"      ), m_pObject->Get_MetaData()["GDAL_DRIVER"].Get_Content().c_str());
		}
	}
	else if( m_pObject->Get_MetaData_DB().Get_Children_Count() )
	{
		DESC_ADD_STR(_TL("Data Source"     ), m_pObject->Get_File_Name(false));

	//	const CSG_MetaData	&DB	= m_pObject->Get_MetaData_DB();
	//	if( DB("DBMS") ) DESC_ADD_STR(_TL("DBMS"    ), DB["DBMS"].Get_Content().c_str());
	//	if( DB("HOST") ) DESC_ADD_STR(_TL("Host"    ), DB["DBMS"].Get_Content().c_str());
	//	if( DB("PORT") ) DESC_ADD_STR(_TL("Port"    ), DB["DBMS"].Get_Content().c_str());
	//	if( DB("NAME") ) DESC_ADD_STR(_TL("Database"), DB["NAME"].Get_Content().c_str());
	}
	else
	{
		DESC_ADD_STR(_TL("Data Source"    ), _TL("memory"));
	}

	DESC_ADD_STR (_TL("Modified"          ), m_pObject->is_Modified() ? _TL("yes") : _TL("no"));
	DESC_ADD_STR (_TL("Spatial Reference" ), m_pObject->Get_Projection().Get_Description().c_str());
	DESC_ADD_STR (_TL("West"              ), SG_Get_String(Get_Grids()->Get_XMin        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("East"              ), SG_Get_String(Get_Grids()->Get_XMax        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("West-East"         ), SG_Get_String(Get_Grids()->Get_XRange      (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("South"             ), SG_Get_String(Get_Grids()->Get_YMin        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("North"             ), SG_Get_String(Get_Grids()->Get_YMax        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("South-North"       ), SG_Get_String(Get_Grids()->Get_YRange      (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("Cell Size"         ), SG_Get_String(Get_Grids()->Get_Cellsize    (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_INT (_TL("Number of Columns" ), Get_Grids()->Get_NX          ());
	DESC_ADD_INT (_TL("Number of Rows"    ), Get_Grids()->Get_NY          ());
	DESC_ADD_INT (_TL("Number of Bands"   ), Get_Grids()->Get_NZ          ());
	DESC_ADD_LONG(_TL("Number of Cells"   ), Get_Grids()->Get_NCells      ());
	DESC_ADD_STR (_TL("Memory Size"       ), Get_nBytes_asString(Get_Grids()->Get_Memory_Size(), 2).c_str());
	DESC_ADD_STR (_TL("Z-Attribute"       ), Get_Grids()->Get_Attributes().Get_Field_Name(Get_Grids()->Get_Z_Attribute()));
	DESC_ADD_STR (_TL("Value Type"        ), SG_Data_Type_Get_Name(Get_Grids()->Get_Type()).c_str());
	DESC_ADD_STR (_TL("No Data Value"     ), Get_Grids()->Get_NoData_Value() < Get_Grids()->Get_NoData_Value(true) ? CSG_String::Format("%f - %f", Get_Grids()->Get_NoData_Value(), Get_Grids()->Get_NoData_Value(true)).c_str() : SG_Get_String(Get_Grids()->Get_NoData_Value(), -2).c_str());

	double Samples = 100. * (double)Get_Grid()->Get_Max_Samples() / (double)Get_Grid()->Get_NCells();

	DESC_ADD_STR (_TL("No Data Cells"     ), wxString::Format("%lld%s", Get_Grids()->Get_NoData_Count(), Samples < 100. ? " (*)" : ""));
	DESC_ADD_STR (_TL("Value Minimum"     ), wxString::Format("%s%s", SG_Get_String(Get_Grids()->Get_Min   (), -20).c_str(), Samples < 100. ? "*" : ""));
	DESC_ADD_STR (_TL("Value Maximum"     ), wxString::Format("%s%s", SG_Get_String(Get_Grids()->Get_Max   (), -20).c_str(), Samples < 100. ? "*" : ""));
	DESC_ADD_STR (_TL("Value Range"       ), wxString::Format("%s%s", SG_Get_String(Get_Grids()->Get_Range (), -20).c_str(), Samples < 100. ? "*" : ""));
	DESC_ADD_STR (_TL("Arithmetic Mean"   ), wxString::Format("%s%s", SG_Get_String(Get_Grids()->Get_Mean  (), -20).c_str(), Samples < 100. ? "*" : ""));
	DESC_ADD_STR (_TL("Standard Deviation"), wxString::Format("%s%s", SG_Get_String(Get_Grids()->Get_StdDev(), -20).c_str(), Samples < 100. ? "*" : ""));

	if( Samples < 100. )
	{
		DESC_ADD_STR(_TL("Sample Size"    ), wxString::Format("%lld (%.02f%%)", Get_Grids()->Get_Max_Samples(), Samples));
	}

	s += "</table>";

	if( Samples < 100. )
	{
		s += wxString::Format("<small>*) <i>%s", _TL("Statistics are based on a subset of the data set. The sample size to be used can be changed in the settings.</i></small>"));
	}

	s += wxString::Format("<hr><h4>%s</h4>", _TL("Coordinate System Details"));
	s += m_pObject->Get_Projection().Get_Description(true).c_str();

	s += Get_TableInfo_asHTML(&Get_Grids()->Get_Attributes());

	//-----------------------------------------------------
	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxToolBarBase * CWKSP_Grids::Get_ToolBar(void)
{
	static wxToolBarBase *static_pToolBar = NULL;

	if( !static_pToolBar )
	{
		static_pToolBar = CMD_ToolBar_Create(ID_TB_DATA_GRIDS);

		Add_ToolBar_Defaults(static_pToolBar);
		CMD_ToolBar_Add_Item(static_pToolBar, false, ID_CMD_DATA_SCATTERPLOT);
		CMD_ToolBar_Add_Item(static_pToolBar, false, ID_CMD_GRIDS_3DVIEW);

		CMD_ToolBar_Add(static_pToolBar, _TL("Grids"));
	}

	return( static_pToolBar );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grids::Get_Menu(void)
{
	wxMenu *pSubMenu, *pMenu = new wxMenu(m_pObject->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SHOW_MAP);
	if( MDI_Get_Active_Map() )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_ZOOM_LAYER_ACTIVE);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_PAN_LAYER_ACTIVE);
	}

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVEAS);

	if( PGSQL_has_Connections() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVETODB);

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVEAS_IMAGE);

	if( m_pObject->is_Modified() && SG_File_Exists(m_pObject->Get_File_Name(false)) )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_RELOAD);

	if( m_pObject->is_File_Native() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_DEL_FILES);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECTION);

	if( m_pObject->Get_MetaData().Get_Children_Count() > 0 )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_METADATA);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_CLASSIFY);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SETTINGS_COPY);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_FORCE_UPDATE);

	pMenu->AppendSeparator();

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Charts"), pSubMenu = new wxMenu());
	CMD_Menu_Add_Item(pSubMenu, true , ID_CMD_DATA_HISTOGRAM);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_DATA_SCATTERPLOT);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_DATA_SCATTERPLOT_3D);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grids::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default: return( CWKSP_Layer::On_Command(Cmd_ID) );

	//-----------------------------------------------------
	case ID_CMD_DATA_SAVEAS_IMAGE  : _Save_Image(); break;

	case ID_CMD_DATA_SCATTERPLOT_3D: { CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Get_Tool("vis_3d_viewer", 6);
		if(	pTool && pTool->On_Before_Execution() && pTool->Set_Parameter("TYPE", 0)
		&&  pTool->Set_Parameter("GRID_X", m_pObject->asGrids()->Get_Grid_Ptr(0))
		&&  pTool->Set_Parameter("GRID_Y", m_pObject->asGrids()->Get_Grid_Ptr(1))
		&&  pTool->Set_Parameter("GRID_Z", m_pObject->asGrids()->Get_Grid_Ptr(2))
		&& DLG_Parameters(pTool->Get_Parameters()) )
		{
			pTool->Execute();
		}
		break; }

	case ID_CMD_GRIDS_3DVIEW       : { CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Get_Tool("vis_3d_viewer", 5);
		if(	pTool && pTool->On_Before_Execution() && pTool->Set_Parameter("GRIDS", m_pObject) )
		{
			pTool->Execute();
		}
		break; }
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Grids::On_Command_UI(wxUpdateUIEvent &event)
{
	return( CWKSP_Layer::On_Command_UI(event) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grids::On_Create_Parameters(void)
{
	CWKSP_Layer::On_Create_Parameters();

	//-----------------------------------------------------
	// General...

	m_Parameters.Add_Double("NODE_GENERAL",
		"MAX_SAMPLES"	, _TL("Maximum Samples"),
		_TL("Maximum number of samples used to build statistics and histograms expressed as percent of the total number of cells."),
		100. * (double)Get_Grids()->Get_Max_Samples() / (double)Get_Grids()->Get_NCells(), 0., true, 100., true
	);

	m_Parameters.Add_String("NODE_GENERAL", "OBJECT_Z_UNIT"     , _TL("Unit"    ), _TL(""), Get_Grids()->Get_Unit   ());
	m_Parameters.Add_Double("NODE_GENERAL", "OBJECT_Z_FACTOR"   , _TL("Z-Scale" ), _TL(""), Get_Grids()->Get_Scaling());
	m_Parameters.Add_Double("NODE_GENERAL", "OBJECT_Z_OFFSET"   , _TL("Z-Offset"), _TL(""), Get_Grids()->Get_Offset ());

	//-----------------------------------------------------
	m_Parameters.Add_Choice("NODE_GENERAL", "DIM_ATTRIBUTE"	    , _TL("Attribute"),
		_TL(""),
		_Get_List_Attributes(), Get_Grids()->Get_Z_Attribute()
	);

	m_Parameters.Add_Choice("NODE_GENERAL", "DIM_NAME"			, _TL("Name"),
		_TL(""),
		_Get_List_Attributes(), Get_Grids()->Get_Z_Attribute()
	);

	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Choice("NODE_DISPLAY", "DISPLAY_RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 0
	);

	//-----------------------------------------------------
	// Transparency...

	m_Parameters.Add_Choice("DISPLAY_TRANSPARENCY", "BAND_A", _TL("Alpha Channel"),
		_TL("Alpha channel values are adjusted to the specified range minimum (full transparency) and maximum (full opacity)"),
		""
	);

	m_Parameters.Add_Range("BAND_A", "BAND_A_RANGE", _TL("Adjustment"),
		_TL(""),
		0., 255.
	);

	//-----------------------------------------------------
	// Classification...

	Set_Grid_Choices(&m_Parameters);

	//-----------------------------------------------------
	m_Fit_Colors = g_pData->Get_Parameter("STRETCH_DEFAULT")->asInt();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grids::Set_Grid_Choices(CSG_Parameters *pParameters)
{
	CSG_String	List(_Get_List_Bands((*pParameters)("DIM_NAME")->asInt()));

	bool	bReset	= (*pParameters)("BAND")->asChoice()->Get_Count() != Get_Grids()->Get_NZ();

	if( (*pParameters)("BAND"  ) ) { (*pParameters)("BAND"  )->asChoice()->Set_Items(List); }

	if( (*pParameters)("BAND_R") ) { (*pParameters)("BAND_R")->asChoice()->Set_Items(List); if( bReset ) (*pParameters)("BAND_R")->Set_Value(2); }
	if( (*pParameters)("BAND_G") ) { (*pParameters)("BAND_G")->asChoice()->Set_Items(List); if( bReset ) (*pParameters)("BAND_G")->Set_Value(1); }
	if( (*pParameters)("BAND_B") ) { (*pParameters)("BAND_B")->asChoice()->Set_Items(List); if( bReset ) (*pParameters)("BAND_B")->Set_Value(0); }

	if( (*pParameters)("BAND_A") )
	{
		(*pParameters)("BAND_A")->asChoice()->Set_Items(List + "|<" + _TL("not set") + ">");

		if( bReset )
		{
			(*pParameters)("BAND_A")->Set_Value((*pParameters)("BAND_A")->asChoice()->Get_Count() - 1);
		}
	}

	return( true );
}

//---------------------------------------------------------
CSG_Grid * CWKSP_Grids::Get_Grid(void)
{
	int	i	= m_Parameters("BAND")->asInt();

	return( i >= 0 && i < Get_Grids()->Get_NZ() ? Get_Grids()->Get_Grid_Ptr(i) : NULL );
}

//---------------------------------------------------------
CSG_Grid * CWKSP_Grids::Get_Grid(int i)
{
	switch( i )
	{
	default: i	= m_Parameters("BAND_R")->asInt(); break;
	case  1: i	= m_Parameters("BAND_G")->asInt(); break;
	case  2: i	= m_Parameters("BAND_B")->asInt(); break;
	case  3: i	= m_Parameters("BAND_A")->asInt();
		return( i >= 0 && i < Get_Grids()->Get_NZ() ? Get_Grids()->Get_Grid_Ptr(i) : NULL );
	}

	if( i >= Get_Grids()->Get_NZ() )
	{
		i	= Get_Grids()->Get_NZ() - 1;
	}

	return( i >= 0 ? Get_Grids()->Get_Grid_Ptr(i) : NULL );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CWKSP_Grids::_Get_List_Attributes(void)
{
	CSG_String	List;

	for(int i=0; i<Get_Grids()->Get_Attributes().Get_Field_Count(); i++)
	{
		List	+= Get_Grids()->Get_Attributes().Get_Field_Name(i); List += "|";
	}

	return( List );
}

//---------------------------------------------------------
CSG_String CWKSP_Grids::_Get_List_Bands(int Attribute)
{
	if( Attribute < 0 || Attribute >= Get_Grids()->Get_Attributes().Get_Field_Count() )
	{
		Attribute	= Get_Grids()->Get_Z_Name_Field();
	}

	CSG_String	List;

	for(int i=0; i<Get_Grids()->Get_NZ(); i++)
	{
		List	+= Get_Grids()->Get_Attributes()[i].asString(Attribute); List += "|";
	}

	return( List );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grids::On_DataObject_Changed(void)
{
	//-----------------------------------------------------
	m_Edit_Attributes.Create(Get_Grids()->Get_Attributes());

	//-----------------------------------------------------
	m_Parameters.Set_Parameter("OBJECT_Z_UNIT"  , Get_Grids()->Get_Unit   ());
	m_Parameters.Set_Parameter("OBJECT_Z_FACTOR", Get_Grids()->Get_Scaling());
	m_Parameters.Set_Parameter("OBJECT_Z_OFFSET", Get_Grids()->Get_Offset ());

	m_Parameters.Set_Parameter("MAX_SAMPLES"    , 100. * Get_Grids()->Get_Max_Samples() / (double)Get_Grids()->Get_NCells());

	//-----------------------------------------------------
	if( m_Parameters("STRETCH_UPDATE")->asBool() == false )	// internal update flag, set by CSG_Tool::DataObject_Update()
	{
		m_Parameters.Set_Parameter("STRETCH_UPDATE", true);
	}
	else if( m_Parameters("STRETCH_DEFAULT")->asInt() >= 3 )	// manual
	{
		m_Parameters.Set_Parameter("STRETCH_DEFAULT", m_Fit_Colors);
	}

	//-----------------------------------------------------
	CSG_String	List;

	List	= _Get_List_Attributes();

	m_Parameters("DIM_ATTRIBUTE")->asChoice()->Set_Items(List);
	m_Parameters("DIM_ATTRIBUTE")->Set_Value(Get_Grids()->Get_Z_Attribute ());

	m_Parameters("DIM_NAME"     )->asChoice()->Set_Items(List);
	m_Parameters("DIM_NAME"     )->Set_Value(Get_Grids()->Get_Z_Name_Field());

	//-----------------------------------------------------
	Set_Grid_Choices(&m_Parameters);

	if( m_Parameters("BAND_R")->asInt() == 0
	&&  m_Parameters("BAND_G")->asInt() == 0
	&&  m_Parameters("BAND_B")->asInt() == 0 )
	{
		m_Parameters.Set_Parameter("BAND_G", 1);
		m_Parameters.Set_Parameter("BAND_B", 2);
	}

	Set_Stretch(m_Parameters);
	Set_Stretch(m_Parameters, Get_Grid(0), "_R");
	Set_Stretch(m_Parameters, Get_Grid(1), "_G");
	Set_Stretch(m_Parameters, Get_Grid(2), "_B");

	//-----------------------------------------------------
	CWKSP_Layer::On_DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_Grids::On_Parameters_Changed(void)
{
	CWKSP_Layer::On_Parameters_Changed();

	//-----------------------------------------------------
	Get_Grids()->Set_Unit   (m_Parameters("OBJECT_Z_UNIT"  )->asString());
	Get_Grids()->Set_Scaling(m_Parameters("OBJECT_Z_FACTOR")->asDouble(),
	                         m_Parameters("OBJECT_Z_OFFSET")->asDouble());

	Get_Grids()->Set_Max_Samples(Get_Grids()->Get_NCells() * (m_Parameters("MAX_SAMPLES")->asDouble() / 100.) * (Get_Grids()->Get_NZ() < 1 ? 1. : 1. / Get_Grids()->Get_NZ()));

	//-----------------------------------------------------
	Get_Grids()->Set_Z_Attribute(m_Parameters("DIM_ATTRIBUTE")->asInt());

	if( Get_Grids()->Get_Z_Name_Field() != m_Parameters("DIM_NAME")->asInt() )
	{
		Get_Grids()->Set_Z_Name_Field(m_Parameters("DIM_NAME")->asInt());

		Set_Grid_Choices(&m_Parameters);
	}

	//-----------------------------------------------------
	m_Classify[0].Set_Metric(
		m_Parameters("METRIC_SCALE_MODE"  )->asInt   (),
		m_Parameters("METRIC_SCALE_LOG"   )->asDouble(),
		m_Parameters("METRIC_ZRANGE_R.MIN")->asDouble(),
		m_Parameters("METRIC_ZRANGE_R.MAX")->asDouble()
	);

	m_Classify[1].Set_Metric(
		m_Parameters("METRIC_SCALE_MODE"  )->asInt   (),
		m_Parameters("METRIC_SCALE_LOG"   )->asDouble(),
		m_Parameters("METRIC_ZRANGE_G.MIN")->asDouble(),
		m_Parameters("METRIC_ZRANGE_G.MAX")->asDouble()
	);

	m_Classify[2].Set_Metric(
		m_Parameters("METRIC_SCALE_MODE"  )->asInt   (),
		m_Parameters("METRIC_SCALE_LOG"   )->asDouble(),
		m_Parameters("METRIC_ZRANGE_B.MIN")->asDouble(),
		m_Parameters("METRIC_ZRANGE_B.MAX")->asDouble()
	);

	//-----------------------------------------------------
	if( m_Parameters("STRETCH_DEFAULT")->asInt() < 3 )	// not manual, remember last state...
	{
		m_Fit_Colors = m_Parameters("STRETCH_DEFAULT")->asInt();
	}
}

//---------------------------------------------------------
bool CWKSP_Grids::Update(CWKSP_Layer *pChanged)
{
	if( pChanged == this )
	{
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grids::Edit_Set_Attributes(void)
{
	if( Get_Grids()->Get_Attributes_Ptr()->Assign_Values(&m_Edit_Attributes) && Get_Grids()->Update_Z_Order() )
	{
		DataObject_Changed();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Grids::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	int Classify = (*pParameters)("COLORS_TYPE")->asInt();

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if( pParameter->Cmp_Identifier("OBJECT_Z_FACTOR")
		||  pParameter->Cmp_Identifier("OBJECT_Z_OFFSET") )
		{
			double newFactor = (*pParameters)("OBJECT_Z_FACTOR")->asDouble(), oldFactor	= m_Parameters("OBJECT_Z_FACTOR")->asDouble();
			double newOffset = (*pParameters)("OBJECT_Z_OFFSET")->asDouble(), oldOffset	= m_Parameters("OBJECT_Z_OFFSET")->asDouble();

			if( newFactor != 0. && oldFactor != 0. )
			{
				CSG_Parameter_Range *newRange = (*pParameters)("METRIC_ZRANGE")->asRange();
				CSG_Parameter_Range *oldRange =  m_Parameters ("METRIC_ZRANGE")->asRange();

				newRange->Set_Min(((oldRange->Get_Min() - oldOffset) / oldFactor) * newFactor + newOffset);
				newRange->Set_Max(((oldRange->Get_Max() - oldOffset) / oldFactor) * newFactor + newOffset);
			}
		}

		if( pParameter->Cmp_Identifier("DIM_NAME") )
		{
			Set_Grid_Choices(pParameters);
		}

		if( Classify == CLASSIFY_OVERLAY && (*pParameters)["OVERLAY_FIT"].asInt() == 1
		&& (pParameter->Cmp_Identifier("STRETCH_DEFAULT")
		||  pParameter->Cmp_Identifier("STRETCH_LINEAR" )
		||  pParameter->Cmp_Identifier("STRETCH_STDDEV" )
		||  pParameter->Cmp_Identifier("STRETCH_INRANGE")
		||  pParameter->Cmp_Identifier("STRETCH_PCTL"   )
		||  pParameter->Cmp_Identifier("OVERLAY_FIT"    )
		||  pParameter->Cmp_Identifier("BAND_R"         )
		||  pParameter->Cmp_Identifier("BAND_G"         )
		||  pParameter->Cmp_Identifier("BAND_B"         )) )
		{
			int i;
			
			if( (i = (*pParameters)("BAND_R")->asInt()) >= 0 && i < Get_Grids()->Get_NZ() )
			{
				Set_Stretch(*pParameters, Get_Grids()->Get_Grid_Ptr(i), "_R");
			}

			if( (i = (*pParameters)("BAND_G")->asInt()) >= 0 && i < Get_Grids()->Get_NZ() )
			{
				Set_Stretch(*pParameters, Get_Grids()->Get_Grid_Ptr(i), "_G");
			}

			if( (i = (*pParameters)("BAND_B")->asInt()) >= 0 && i < Get_Grids()->Get_NZ() )
			{
				Set_Stretch(*pParameters, Get_Grids()->Get_Grid_Ptr(i), "_B");
			}
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if( pParameter->Cmp_Identifier("COLORS_TYPE")
		||  pParameter->Cmp_Identifier("OVERLAY_FIT")
		||  pParameter->Cmp_Identifier("DISPLAY_TRANSPARENCY") )
		{
			pParameters->Set_Enabled("DISPLAY_RESAMPLING", Classify != CLASSIFY_LUT && Classify != CLASSIFY_SINGLE);

			pParameters->Set_Enabled("BAND"              , Classify == CLASSIFY_DISCRETE || Classify == CLASSIFY_GRADUATED || Classify == CLASSIFY_LUT);

			pParameters->Set_Enabled("METRIC_ZRANGE"     , Classify == CLASSIFY_DISCRETE || Classify == CLASSIFY_GRADUATED || (Classify == CLASSIFY_OVERLAY && (*pParameters)["OVERLAY_FIT"].asInt() == 0));
			pParameters->Set_Enabled("METRIC_SCALE_MODE" , Classify == CLASSIFY_DISCRETE || Classify == CLASSIFY_GRADUATED ||  Classify == CLASSIFY_OVERLAY);

			pParameters->Set_Enabled("BAND_R"            , Classify == CLASSIFY_OVERLAY);
			pParameters->Set_Enabled("BAND_G"            , Classify == CLASSIFY_OVERLAY);
			pParameters->Set_Enabled("BAND_B"            , Classify == CLASSIFY_OVERLAY);
			pParameters->Set_Enabled("BAND_A"            , Classify == CLASSIFY_OVERLAY);
			pParameters->Set_Enabled("OVERLAY_FIT"       , Classify == CLASSIFY_OVERLAY);
			pParameters->Set_Enabled("METRIC_ZRANGE_R"   , Classify == CLASSIFY_OVERLAY && (*pParameters)["OVERLAY_FIT"].asInt() == 1);
			pParameters->Set_Enabled("METRIC_ZRANGE_G"   , Classify == CLASSIFY_OVERLAY && (*pParameters)["OVERLAY_FIT"].asInt() == 1);
			pParameters->Set_Enabled("METRIC_ZRANGE_B"   , Classify == CLASSIFY_OVERLAY && (*pParameters)["OVERLAY_FIT"].asInt() == 1);
		}

		if( pParameter->Cmp_Identifier("BAND_A") )
		{
			pParameters->Set_Enabled("BAND_A_RANGE", pParameter->asInt() < pParameter->asChoice()->Get_Count() - 1);
		}
	}

	//-----------------------------------------------------
	return( CWKSP_Layer::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grids::Get_Value(CSG_Point ptWorld, double Epsilon)
{
	wxString	s;

	if( Get_Grid() )
	{
		double	Value;

		switch( m_pClassify->Get_Mode() )
		{
		case CLASSIFY_DISCRETE:
		case CLASSIFY_GRADUATED:
			if( Get_Grid()->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
			{
				s	= SG_Get_String(Value, -12).c_str();

				if( Get_Grids()->Get_Unit() && *Get_Grids()->Get_Unit() )
				{
					s += " "; s += Get_Grids()->Get_Unit();
				}
			}
			break;

		case CLASSIFY_OVERLAY:
			if( Get_Grid(0)->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
			{
				s	+= wxString::Format("R%s ", SG_Get_String(Value, -6).c_str());
			}
			if( Get_Grid(1)->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
			{
				s	+= wxString::Format("G%s ", SG_Get_String(Value, -6).c_str());
			}
			if( Get_Grid(2)->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
			{
				s	+= wxString::Format("B%s ", SG_Get_String(Value, -6).c_str());
			}
			break;

		case CLASSIFY_LUT:
			if( Get_Grid()->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
			{
				s	= m_pClassify->Get_Class_Name_byValue(Value);
			}
			break;

		default:
			break;
		}
	}

	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grids::Fit_Colors(const CSG_Rect &rWorld)
{
	if( m_Parameters("COLORS_TYPE")->asInt() == CLASSIFY_DISCRETE
	||  m_Parameters("COLORS_TYPE")->asInt() == CLASSIFY_GRADUATED )
	{
		if( _Fit_Colors(rWorld, Get_Grid(), m_pClassify) )
		{
			return( Update_Views() );
		}
	}

	if( m_Parameters("COLORS_TYPE")->asInt() == CLASSIFY_OVERLAY
	&&  m_Parameters("OVERLAY_FIT")->asInt() == 0 )	// overall band statistics
	{
		if( _Fit_Colors(rWorld, Get_Grids(), m_pClassify) )
		{
			return( Update_Views() );
		}
	}

	if( m_Parameters("COLORS_TYPE")->asInt() == CLASSIFY_OVERLAY
	&&  m_Parameters("OVERLAY_FIT")->asInt() != 0 )	// band wise statistics
	{
		if( _Fit_Colors(rWorld, Get_Grid(0), &m_Classify[0], "_R")
		&&  _Fit_Colors(rWorld, Get_Grid(1), &m_Classify[1], "_G")
		&&  _Fit_Colors(rWorld, Get_Grid(2), &m_Classify[2], "_B") )
		{
			return( Update_Views() );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grids::_Fit_Colors(const CSG_Rect &rWorld, CSG_Data_Object *pObject, CWKSP_Layer_Classify *pClassify, const CSG_String &Suffix)
{
	#define	GET_STAT (pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid ? ((CSG_Grid *)pObject)->Get_Statistics(rWorld, s) : ((CSG_Grids *)pObject)->Get_Statistics(rWorld, s))
	#define	GET_HIST (pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid ? ((CSG_Grid *)pObject)->Get_Histogram (rWorld, h) : ((CSG_Grids *)pObject)->Get_Histogram (rWorld, h))

	double	Minimum, Maximum;

	switch( m_Fit_Colors )
	{
	default: {	CSG_Simple_Statistics	s;	if( !GET_STAT )	return( false );
		double	d	= m_Parameters("STRETCH_LINEAR")->asDouble() * 0.01 * s.Get_Range();
		Minimum	= s.Get_Minimum() + d;
		Maximum	= s.Get_Maximum() - d;
		break;	}

	case  1: {	CSG_Simple_Statistics	s;	if( !GET_STAT )	return( false );
		double	d	= m_Parameters("STRETCH_STDDEV")->asDouble() * s.Get_StdDev();
		Minimum	= s.Get_Mean() - d; if( m_Parameters("STRETCH_INRANGE")->asBool() && Minimum < s.Get_Minimum() ) Minimum = s.Get_Minimum();
		Maximum	= s.Get_Mean() + d; if( m_Parameters("STRETCH_INRANGE")->asBool() && Maximum > s.Get_Maximum() ) Maximum = s.Get_Maximum();
		break;	}

	case  2: {	CSG_Histogram			h;	if( !GET_HIST ) return( false );
		Minimum	= h.Get_Quantile(m_Parameters("STRETCH_PCTL.MIN")->asDouble() / 100.);
		Maximum	= h.Get_Quantile(m_Parameters("STRETCH_PCTL.MAX")->asDouble() / 100.);
		break;	}
	}

	m_Parameters.Set_Parameter("STRETCH_DEFAULT", 3);	// manual

	m_Parameters.Set_Parameter("METRIC_ZRANGE" + Suffix + ".MIN", Minimum);
	m_Parameters.Set_Parameter("METRIC_ZRANGE" + Suffix + ".MAX", Maximum);

	pClassify->Set_Metric(
		m_Parameters("METRIC_SCALE_MODE")->asInt   (),
		m_Parameters("METRIC_SCALE_LOG" )->asDouble(),
		Minimum, Maximum
	);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grids::asImage(CSG_Grid *pImage)
{
	wxImage Image;

	if( pImage && Get_Image(Image) )
	{
		pImage->Create(Get_Grids()->Get_System(), SG_DATATYPE_Int);

		#pragma omp parallel for
		for(int y=0; y<pImage->Get_NY(); y++) for(int x=0; x<pImage->Get_NX(); x++)
		{
			if( Image.HasAlpha() )
			{
				pImage->Set_Value(x, y, SG_GET_RGBA(Image.GetRed(x, y), Image.GetGreen(x, y), Image.GetBlue(x, y), Image.GetAlpha(x, y)));
			}
			else
			{
				pImage->Set_Value(x, y, SG_GET_RGB (Image.GetRed(x, y), Image.GetGreen(x, y), Image.GetBlue(x, y)));
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grids::_Save_Image(void)
{
	static CSG_Parameters P(NULL, _TL("Save Grid Collection as Image..."), _TL(""), SG_T(""));

	if( P.Get_Count() == 0 )
	{
		P.Add_Bool  ("", "WORLD", _TL("Save Georeference"), _TL(""),  true);
		P.Add_Bool  ("", "LG"   , _TL("Legend: Save"     ), _TL(""), false);
		P.Add_Double("", "LZ"   , _TL("Legend: Zoom"     ), _TL(""), 1., 0., true);
	}

	//-----------------------------------------------------
	wxString File; int Type; wxImage Image;

	if( !DLG_Image_Save(File, Type) || !DLG_Parameters(&P) || !Get_Image(Image) )
	{
		return( false );
	}

	Image.SaveFile(File, (wxBitmapType)Type);

	//-----------------------------------------------------
	wxBitmap Bitmap;

	if( P("LG")->asBool() && Get_Bitmap_Legend(Bitmap, P("LZ")->asDouble()) )
	{
		wxFileName fn(File); fn.SetName(wxString::Format("%s_legend", fn.GetName().c_str()));

		Bitmap.SaveFile(fn.GetFullPath(), (wxBitmapType)Type);
	}

	if( P("WORLD")->asBool() )
	{
		wxFileName fn(File);

		switch( Type )
		{
		default                : fn.SetExt("world"); break;
		case wxBITMAP_TYPE_BMP : fn.SetExt("bpw"  ); break;
		case wxBITMAP_TYPE_GIF : fn.SetExt("gfw"  ); break;
		case wxBITMAP_TYPE_JPEG: fn.SetExt("jgw"  ); break;
		case wxBITMAP_TYPE_PNG : fn.SetExt("pgw"  ); break;
		case wxBITMAP_TYPE_PCX : fn.SetExt("pxw"  ); break;
		case wxBITMAP_TYPE_TIF : fn.SetExt("tfw"  ); break; 
		}

		CSG_File Stream;

		if( Stream.Open(fn.GetFullPath().wx_str(), SG_FILE_W, false) )
		{
			Stream.Printf("%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n",
				 Get_Grid()->Get_Cellsize(), 0., 0.,
				-Get_Grid()->Get_Cellsize(),
				 Get_Grid()->Get_XMin(),
				 Get_Grid()->Get_YMax()
			);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Grids::_Save_Image_Clipboard(void)
{
	wxBitmap Bitmap;

	if( Get_Bitmap(Bitmap) && wxTheClipboard->Open() )
	{
		wxBitmapDataObject *pBitmap = new wxBitmapDataObject(Bitmap);
		wxTheClipboard->SetData(pBitmap);
		wxTheClipboard->Close();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grids::Get_Image(wxImage &Image, bool bFitSize, int Width, int Height)
{
	if( bFitSize || Width  < 1 ) { Width  = Get_Grid()->Get_NX(); }
	if( bFitSize || Height < 1 ) { Height = Get_Grid()->Get_NY(); }

	if( Width > 0 && Height > 0 && Image.Create(Width, Height) )
	{
		Set_Buisy_Cursor(true);

		CSG_Map_DC dc_Map(Get_Extent(), Image.GetSize(), 1., -1, true);

		On_Draw(dc_Map, false);

		dc_Map.Get_Image(Image);

		Set_Buisy_Cursor(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grids::Get_Bitmap(wxBitmap &Bitmap, bool bFitSize, int Width, int Height)
{
	if( bFitSize || Width  < 1 ) { Width  = bFitSize ? Get_Grid()->Get_NX() : Bitmap.GetWidth (); }
	if( bFitSize || Height < 1 ) { Height = bFitSize ? Get_Grid()->Get_NY() : Bitmap.GetHeight(); }

	if( Width > 0 && Height > 0 && Bitmap.Create(Width, Height) )
	{
		Set_Buisy_Cursor(true);

		CSG_Map_DC dc_Map(Get_Extent(), Bitmap.GetSize(), 1., -1, true);

		On_Draw(dc_Map, false);

		dc_Map.Get_Bitmap(Bitmap);

		Set_Buisy_Cursor(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grids::Get_Bitmap_Legend(wxBitmap &Bitmap, double Zoom)
{
	if( Zoom > 0. )
	{
		wxMemoryDC dc; wxSize s(Get_Legend()->Get_Size(Zoom, 1.));

		Bitmap.Create(s.GetWidth(), s.GetHeight());

		dc.SelectObject(Bitmap);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		Get_Legend()->Draw(dc, Zoom, 1., wxPoint(0, 0));

		dc.SelectObject(wxNullBitmap);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grids::On_Draw(CSG_Map_DC &dc_Map, int Flags)
{
	if( Get_Grid() == NULL || Get_Grids()->Get_NZ() < 1 || Get_Extent().Intersects(dc_Map.rWorld()) == INTERSECTION_None )
	{
		return;
	}

	//-----------------------------------------------------
	CSG_Map_DC::Mode Mode = m_pClassify->Get_Mode() == CLASSIFY_OVERLAY && Get_Grid(3) ? CSG_Map_DC::Mode::Alpha : CSG_Map_DC::Mode::Transparent;

	if( !dc_Map.Draw_Image_Begin(m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100., Mode) )
	{
		return;
	}

	m_Alpha[0] = m_Parameters("BAND_A_RANGE.MIN")->asDouble();
	m_Alpha[1] = m_Parameters("BAND_A_RANGE.MAX")->asDouble() - m_Alpha[0]; m_Alpha[1] = m_Alpha[1] ? 255. / m_Alpha[1] : 1.;

	//-----------------------------------------------------
	switch( m_Parameters("COLORS_TYPE")->asInt() )
	{
	case CLASSIFY_SINGLE   : m_pClassify->Set_Mode(CLASSIFY_SINGLE   ); break;
	case CLASSIFY_LUT      : m_pClassify->Set_Mode(CLASSIFY_LUT      ); break;
	case CLASSIFY_DISCRETE : m_pClassify->Set_Mode(CLASSIFY_DISCRETE ); break;
	case CLASSIFY_GRADUATED: m_pClassify->Set_Mode(CLASSIFY_GRADUATED); break;
	case CLASSIFY_OVERLAY  : m_pClassify->Set_Mode(CLASSIFY_OVERLAY  ); break;
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	if( m_pClassify->Get_Mode() == CLASSIFY_SINGLE )
	{
		Resampling	= GRID_RESAMPLING_NearestNeighbour;
	}
	else switch( m_Parameters(m_pClassify->Get_Mode() == CLASSIFY_LUT ? "LUT_RESAMPLING" : "DISPLAY_RESAMPLING")->asInt() )
	{
	default: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	if(	dc_Map.DC2World() >= Get_Grids()->Get_Cellsize() || Resampling != GRID_RESAMPLING_NearestNeighbour )
	{
		_Draw_Grid_Nodes(dc_Map, Resampling);
	}
	else
	{
		_Draw_Grid_Cells(dc_Map);
	}

	//-----------------------------------------------------
	dc_Map.Draw_Image_End();
}

//---------------------------------------------------------
void CWKSP_Grids::_Draw_Grid_Nodes(CSG_Map_DC &dc_Map, TSG_Grid_Resampling Resampling)
{
	CSG_Grid	*pBands[4];

	if( m_pClassify->Get_Mode() == CLASSIFY_OVERLAY )
	{
		pBands[0] = Get_Grid(0);
		pBands[1] = Get_Grid(1);
		pBands[2] = Get_Grid(2);
		pBands[3] = Get_Grid(3);
	}
	else
	{
		pBands[0] = pBands[1] = pBands[2] = Get_Grid();
		pBands[3] = NULL;
	}

	//-----------------------------------------------------
	CSG_Rect	rMap(dc_Map.rWorld());	rMap.Intersect(Get_Grids()->Get_Extent(true));

	int	axDC	= (int)dc_Map.xWorld2DC(rMap.Get_XMin());	if( axDC < 0 )	axDC	= 0;
	int	bxDC	= (int)dc_Map.xWorld2DC(rMap.Get_XMax());	if( bxDC >= dc_Map.rDC().GetWidth () )	bxDC	= dc_Map.rDC().GetWidth () - 1;
	int	ayDC	= (int)dc_Map.yWorld2DC(rMap.Get_YMin());	if( ayDC >= dc_Map.rDC().GetHeight() )	ayDC	= dc_Map.rDC().GetHeight() - 1;
	int	byDC	= (int)dc_Map.yWorld2DC(rMap.Get_YMax());	if( byDC < 0 )	byDC	= 0;
	int	nyDC	= abs(ayDC - byDC);

	bool	bBandWise	= m_Parameters("OVERLAY_FIT")->asInt() != 0;	// bandwise statistics

	#pragma omp parallel for
	for(int iyDC=0; iyDC<=nyDC; iyDC++)
	{
		_Draw_Grid_Nodes(dc_Map, Resampling, pBands, bBandWise, ayDC - iyDC, axDC, bxDC);
	}
}

//---------------------------------------------------------
void CWKSP_Grids::_Draw_Grid_Nodes(CSG_Map_DC &dc_Map, TSG_Grid_Resampling Resampling, CSG_Grid *pBands[4], bool bBandWise, int yDC, int axDC, int bxDC)
{
	double xMap = dc_Map.xDC2World(axDC);
	double yMap = dc_Map.yDC2World( yDC);

	for(int xDC=axDC; xDC<=bxDC; xMap+=dc_Map.DC2World(), xDC++)
	{
		if( m_pClassify->Get_Mode() != CLASSIFY_OVERLAY )
		{
			double z; int c;

			if( pBands[0]->Get_Value(xMap, yMap, z, Resampling, false) && m_pClassify->Get_Class_Color_byValue(z, c) )
			{
				dc_Map.Draw_Image_Pixel(xDC, yDC, c);
			}
		}
		else
		{
			double z[4]; bool bOkay;

			if( pBands[3] == NULL )
			{
				bOkay = pBands[0]->Get_Value(xMap, yMap, z[0], Resampling, false)
				     && pBands[1]->Get_Value(xMap, yMap, z[1], Resampling, false)
				     && pBands[2]->Get_Value(xMap, yMap, z[2], Resampling, false);
				z[3]  = 255.;
			}
			else
			{
				z[3] = (pBands[3]->Get_Value(xMap, yMap, Resampling) - m_Alpha[0]) * m_Alpha[1];

				if( (bOkay = z[3] > 0.) == true )
				{
					if( z[3] > 255. )
					{
						z[3] = 255.;
					}

					z[0] = pBands[0]->Get_Value(xMap, yMap, Resampling);
					z[1] = pBands[1]->Get_Value(xMap, yMap, Resampling);
					z[2] = pBands[2]->Get_Value(xMap, yMap, Resampling);
				}
			}

			if( bOkay )
			{
				BYTE c[4]; c[3] = (BYTE)z[3];

				for(int i=0; i<3; i++)
				{
					double d = bBandWise
						? 255. * m_Classify[i].Get_MetricToRelative(z[i])
						: 255. * m_pClassify ->Get_MetricToRelative(z[i]);

					c[i] = d < 0. ? 0 : d > 255. ? 255 : (BYTE)d;
				}

				dc_Map.Draw_Image_Pixel(xDC, yDC, *(int *)&c);
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grids::_Draw_Grid_Cells(CSG_Map_DC &dc_Map)
{
	bool bBandWise = m_Parameters("OVERLAY_FIT")->asInt() != 0;	// bandwise statistics

	CSG_Grid *pBands[4];

	if( m_pClassify->Get_Mode() == CLASSIFY_OVERLAY )
	{
		pBands[0] = Get_Grid(0);
		pBands[1] = Get_Grid(1);
		pBands[2] = Get_Grid(2);
		pBands[3] = Get_Grid(3);
	}
	else
	{
		pBands[0] = pBands[1] = pBands[2] = Get_Grid();
		pBands[3] = NULL;
	}

	//-----------------------------------------------------
	int xa = Get_Grids()->Get_System().Get_xWorld_to_Grid(dc_Map.rWorld().Get_XMin()); if( xa <  0                     ) xa = 0;
	int ya = Get_Grids()->Get_System().Get_yWorld_to_Grid(dc_Map.rWorld().Get_YMin()); if( ya <  0                     ) ya = 0;
	int xb = Get_Grids()->Get_System().Get_xWorld_to_Grid(dc_Map.rWorld().Get_XMax()); if( xb >= Get_Grids()->Get_NX() ) xb = Get_Grids()->Get_NX() - 1;
	int yb = Get_Grids()->Get_System().Get_yWorld_to_Grid(dc_Map.rWorld().Get_YMax()); if( yb >= Get_Grids()->Get_NY() ) yb = Get_Grids()->Get_NY() - 1;

	double  dDC = Get_Grids()->Get_Cellsize() * dc_Map.World2DC();

	double axDC = dc_Map.xWorld2DC(Get_Grids()->Get_System().Get_xGrid_to_World(xa)) + dDC / 2.;
	double ayDC = dc_Map.yWorld2DC(Get_Grids()->Get_System().Get_yGrid_to_World(ya)) - dDC / 2.;

	//-----------------------------------------------------
	double  yDC = ayDC;

	for(int y=ya, yaDC=(int)(ayDC), ybDC=(int)(ayDC+dDC); y<=yb; y++, ybDC=yaDC, yaDC=(int)(yDC-=dDC))
	{
		double xDC = axDC;

		for(int x=xa, xaDC=(int)(axDC-dDC), xbDC=(int)(axDC); x<=xb; x++, xaDC=xbDC, xbDC=(int)(xDC+=dDC))
		{
			if( m_pClassify->Get_Mode() != CLASSIFY_OVERLAY )
			{
				int c;

				if( pBands[0]->is_InGrid(x, y) && m_pClassify->Get_Class_Color_byValue(pBands[0]->asDouble(x, y), c) )
				{
					dc_Map.Draw_Image_Pixels(xaDC, yaDC, xbDC, ybDC, c);
				}
			}
			else if( pBands[0]->is_InGrid(x, y, !pBands[3])
				&&   pBands[1]->is_InGrid(x, y, !pBands[3])
				&&   pBands[2]->is_InGrid(x, y, !pBands[3])
				&& (!pBands[3] || pBands[3]->is_InGrid(x, y, false)) )
			{
				BYTE c[4];

				for(int i=0; i<3; i++)
				{
					double d = bBandWise
						? 255. * m_Classify[i].Get_MetricToRelative(pBands[i]->asDouble(x, y))
						: 255. * m_pClassify ->Get_MetricToRelative(pBands[i]->asDouble(x, y));

					c[i] = d < 0. ? 0 : d > 255. ? 255 : (BYTE)d;
				}

				if( pBands[3] )
				{
					double d = (pBands[3]->asDouble(x, y) - m_Alpha[0]) * m_Alpha[1];

					if( d > 0. )
					{
						c[3] = d < 255. ? (BYTE)d : 255;

						dc_Map.Draw_Image_Pixels(xaDC, yaDC, xbDC, ybDC, *(int *)&c);
					}
				}
				else
				{
					dc_Map.Draw_Image_Pixels(xaDC, yaDC, xbDC, ybDC, *(int *)&c);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
