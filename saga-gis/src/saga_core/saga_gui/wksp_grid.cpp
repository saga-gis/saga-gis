
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
#include <wx/dc.h>
#include <wx/dcmemory.h>
#include <wx/image.h>
#include <wx/filename.h>
#include <wx/clipbrd.h>

#include <saga_gdi/sgdi_helper.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "active.h"
#include "active_attributes.h"

#include "wksp_map_control.h"

#include "wksp_layer_classify.h"
#include "wksp_layer_legend.h"

#include "wksp_data_manager.h"
#include "wksp_grid_manager.h"
#include "wksp_grid.h"
#include "wksp_grids.h"

#include "data_source_pgsql.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grid::CWKSP_Grid(CSG_Grid *pGrid)
	: CWKSP_Layer(pGrid)
{
	m_Edit_Attributes.Add_Field("ROW", SG_DATATYPE_Int);

	On_Create_Parameters();

	DataObject_Changed();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grid::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format("<h4>%s</h4>", _TL("Grid"));

	s	+= "<table border=\"0\">";

	DESC_ADD_STR (_TL("Name"               ), m_pObject->Get_Name());
	DESC_ADD_STR (_TL("Description"        ), m_pObject->Get_Description());

	if( SG_File_Exists(m_pObject->Get_File_Name(false)) )
	{
		DESC_ADD_STR (_TL("File"           ), m_pObject->Get_File_Name(false));

		if( m_pObject->Get_MetaData()("GDAL_DRIVER") )
			DESC_ADD_STR (_TL("Driver"     ), m_pObject->Get_MetaData()["GDAL_DRIVER"].Get_Content().c_str());

		if( m_pObject->Get_MetaData()("SURFER_GRID") )
			DESC_ADD_STR (_TL("Driver"     ), m_pObject->Get_MetaData()["SURFER_GRID"].Get_Content().c_str());
	}
	else if( m_pObject->Get_MetaData_DB().Get_Children_Count() )
	{
		DESC_ADD_STR(_TL("File"            ), m_pObject->Get_File_Name(false));
		//const CSG_MetaData	&DB	= m_pObject->Get_MetaData_DB();
		//if( DB("DBMS") ) DESC_ADD_STR (_TL("DBMS"    ), DB["DBMS"].Get_Content().c_str());
		//if( DB("HOST") ) DESC_ADD_STR (_TL("Host"    ), DB["DBMS"].Get_Content().c_str());
		//if( DB("PORT") ) DESC_ADD_STR (_TL("Port"    ), DB["DBMS"].Get_Content().c_str());
		//if( DB("NAME") ) DESC_ADD_STR (_TL("Database"), DB["NAME"].Get_Content().c_str());
	}
	else
	{
		DESC_ADD_STR (_TL("File"          ), _TL("memory"));
	}

	DESC_ADD_STR (_TL("Modified"          ), m_pObject->is_Modified() ? _TL("yes") : _TL("no"));
	DESC_ADD_STR (_TL("Projection"        ), m_pObject->Get_Projection().Get_Description().c_str());
	DESC_ADD_STR (_TL("West"              ), SG_Get_String(Get_Grid()->Get_XMin        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("East"              ), SG_Get_String(Get_Grid()->Get_XMax        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("West-East"         ), SG_Get_String(Get_Grid()->Get_XRange      (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("South"             ), SG_Get_String(Get_Grid()->Get_YMin        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("North"             ), SG_Get_String(Get_Grid()->Get_YMax        (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("South-North"       ), SG_Get_String(Get_Grid()->Get_YRange      (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_STR (_TL("Cell Size"         ), SG_Get_String(Get_Grid()->Get_Cellsize    (), -CSG_Grid_System::Get_Precision()).c_str());
	DESC_ADD_INT (_TL("Number of Columns" ), Get_Grid()->Get_NX          ());
	DESC_ADD_INT (_TL("Number of Rows"    ), Get_Grid()->Get_NY          ());
	DESC_ADD_LONG(_TL("Number of Cells"   ), Get_Grid()->Get_NCells      ());
	DESC_ADD_LONG(_TL("No Data Cells"     ), Get_Grid()->Get_NoData_Count());
	DESC_ADD_STR (_TL("Value Type"        ), SG_Data_Type_Get_Name(Get_Grid()->Get_Type()).c_str());
	DESC_ADD_FLT (_TL("Value Minimum"     ), Get_Grid()->Get_Min         ());
	DESC_ADD_FLT (_TL("Value Maximum"     ), Get_Grid()->Get_Max         ());
	DESC_ADD_FLT (_TL("Value Range"       ), Get_Grid()->Get_Range       ());
	DESC_ADD_STR (_TL("No Data Value"     ), Get_Grid()->Get_NoData_Value() < Get_Grid()->Get_NoData_Value(true) ? CSG_String::Format("%f - %f", Get_Grid()->Get_NoData_Value(), Get_Grid()->Get_NoData_Value(true)).c_str() : SG_Get_String(Get_Grid()->Get_NoData_Value(), -2).c_str());
	DESC_ADD_FLT (_TL("Arithmetic Mean"   ), Get_Grid()->Get_Mean        ());
	DESC_ADD_FLT (_TL("Standard Deviation"), Get_Grid()->Get_StdDev      ());
	DESC_ADD_STR (_TL("Memory Size"       ), Get_nBytes_asString(Get_Grid()->Get_Memory_Size(), 2).c_str());

	if( Get_Grid()->is_Cached() )
	{
		DESC_ADD_STR(_TL("File Cache"     ), _TL("activated"));
	}

	s	+= "</table>";

	//-----------------------------------------------------
//	s.Append(wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), _TL("Data History")));
//	s.Append(Get_Grid()->Get_History().Get_HTML());
//	s.Append(wxString::Format(wxT("</font")));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grid::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(m_pObject->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRID_SHOW);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVEAS);

	if( PGSQL_has_Connections() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVETODB);

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRID_SAVEAS_IMAGE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRID_CLIPBOARD_IMAGE);

	if( m_pObject->is_File_Native() && m_pObject->is_Modified() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_RELOAD);

	if( m_pObject->is_File_Native() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_DEL_FILES);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECTION);

	if( m_pObject->Get_MetaData().Get_Children_Count() > 0 )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_METADATA);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_GRID_HISTOGRAM);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRID_SCATTERPLOT);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRID_SET_LUT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SETTINGS_COPY);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_FORCE_UPDATE);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grid::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Layer::On_Command(Cmd_ID) );

	case ID_CMD_GRID_SAVEAS_IMAGE:
		_Save_Image();
		break;

	case ID_CMD_GRID_CLIPBOARD_IMAGE:
		_Save_Image_Clipboard();
		break;

	case ID_CMD_GRID_HISTOGRAM:
		Histogram_Toggle();
		break;

	case ID_CMD_GRID_SCATTERPLOT:
		Add_ScatterPlot();
		break;

	case ID_CMD_GRID_SET_LUT:
		_LUT_Create();
		break;

	case ID_CMD_GRID_SEL_CLEAR:
		_Edit_Clr_Selection();
		break;

	case ID_CMD_GRID_SEL_DELETE:
		_Edit_Del_Selection();
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Grid::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Layer::On_Command_UI(event) );

	case ID_CMD_GRID_SEL_CLEAR:
		event.Enable(m_Edit_Attributes.Get_Count() > 0);
		break;

	case ID_CMD_GRID_SEL_DELETE:
		event.Enable(m_Edit_Attributes.Get_Count() > 0);
		break;

	case ID_CMD_GRID_HISTOGRAM:
		event.Check(m_pHistogram != NULL);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grid::On_Create_Parameters(void)
{
	CWKSP_Layer::On_Create_Parameters();

	//-----------------------------------------------------
	// General...

	m_Parameters.Add_String("NODE_GENERAL", "OBJECT_Z_UNIT"  , _TL("Unit"    ), _TL(""), Get_Grid()->Get_Unit   ());
	m_Parameters.Add_Double("NODE_GENERAL", "OBJECT_Z_FACTOR", _TL("Z-Scale" ), _TL(""), Get_Grid()->Get_Scaling());
	m_Parameters.Add_Double("NODE_GENERAL", "OBJECT_Z_OFFSET", _TL("Z-Offset"), _TL(""), Get_Grid()->Get_Offset ());

	//-----------------------------------------------------
	// Resampling...

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

	m_Parameters.Add_Grid("DISPLAY_TRANSPARENCY", "DISPLAY_ALPHA", _TL("Alpha Channel"),
		_TL("Alpha channel values are adjusted to the specified range minimum (full transparency) and maximum (full opacity)"),
		PARAMETER_INPUT_OPTIONAL, false
	)->Get_Parent()->Set_Value((void *)&Get_Grid()->Get_System());

	m_Parameters.Add_Range("DISPLAY_ALPHA", "ALPHA_RANGE", _TL("Adjustment"),
		_TL(""),
		0., 255.
	);

	//-----------------------------------------------------
	// Shading...

	m_Parameters.Add_Choice("NODE_DISPLAY", "DISPLAY_SHADING"	, _TL("Shading"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("none"), _TL("normal"), _TL("inverse")
		), 0
	);

	m_Parameters.Add_Double("DISPLAY_SHADING", "SHADING_AZIMUTH", _TL("Azimuth"),
		_TL("Direction of the light source, measured in degree clockwise from the North direction."),
		315., 0., true, 360., true
	);

	m_Parameters.Add_Double("DISPLAY_SHADING", "SHADING_HEIGHT"	, _TL("Height"),
		_TL("Height of the light source, measured in degree above the horizon."),
		45., 0., true, 90., true
	);

	m_Parameters.Add_Double("DISPLAY_SHADING", "SHADING_EXAGG"	, _TL("Exaggeration"),
		_TL(""),
		1.
	);

	m_Parameters.Add_Double("DISPLAY_SHADING", "SHADING_MIN"	, _TL("Minimum"),
		_TL(""),
		0.
	);

	m_Parameters.Add_Double("DISPLAY_SHADING", "SHADING_MAX"	, _TL("Maximum"),
		_TL(""),
		1.5
	);

	//-----------------------------------------------------
	// Classification...

	m_Parameters.Add_Node("NODE_COLORS"	, "NODE_SHADE"		, _TL("Shade"),
		_TL("")
	);

	m_Parameters.Add_Choice("NODE_SHADE", "SHADE_MODE"		, _TL("Coloring"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("bright - dark"  ),
			_TL("dark - bright"  ),
			_TL("white - cyan"   ),
			_TL("cyan - white"   ),
			_TL("white - magenta"),
			_TL("magenta - white"),
			_TL("white - yellow" ),
			_TL("yellow - white" )
		), 0
	);

	//-----------------------------------------------------
	m_Parameters.Add_Bool("NODE_COLORS"	, "RGB_ALPHA"		, _TL("Alpha Channel"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("NODE_COLORS"	, "NODE_OVERLAY"	, _TL("RGB Composite"),
		_TL("")
	);

	m_Parameters.Add_Choice("NODE_OVERLAY", "OVERLAY_MODE"	, _TL("This Color"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Red"), _TL("Green"), _TL("Blue")
		), 0
	);

	m_Parameters.Add_Grid("NODE_OVERLAY", "OVERLAY_R", _TL("Red"  ), _TL(""), PARAMETER_INPUT_OPTIONAL, false)->Get_Parent()->Set_Value(
		(void *)&Get_Grid()->Get_System()
	);

	m_Parameters.Add_Grid("NODE_OVERLAY", "OVERLAY_G", _TL("Green"), _TL(""), PARAMETER_INPUT_OPTIONAL, false)->Get_Parent()->Set_Value(
		(void *)&Get_Grid()->Get_System()
	);

	m_Parameters.Add_Grid("NODE_OVERLAY", "OVERLAY_B", _TL("Blue" ), _TL(""), PARAMETER_INPUT_OPTIONAL, false)->Get_Parent()->Set_Value(
		(void *)&Get_Grid()->Get_System()
	);

	//-----------------------------------------------------
	// Cell Values...

	m_Parameters.Add_Bool("NODE_GENERAL", "VALUES_SHOW"		, _TL("Show Cell Values"),
		_TL("shows cell values when zoomed"),
		false
	);

	m_Parameters.Add_Font("VALUES_SHOW", "VALUES_FONT"		, _TL("Font"),
		_TL("")
	);

	m_Parameters.Add_Double("VALUES_SHOW", "VALUES_SIZE"	, _TL("Size"),
		_TL(""),
		15, 0, true , 100., true
	);

	m_Parameters.Add_Int("VALUES_SHOW", "VALUES_DECIMALS"	, _TL("Decimals"),
		_TL(""),
		2
	);

	m_Parameters.Add_Choice("VALUES_SHOW", "VALUES_EFFECT"	, _TL("Boundary Effect"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("none"        ),
			_TL("full frame"  ),
			_TL("top"         ),
			_TL("top left"    ),
			_TL("left"        ),
			_TL("bottom left" ),
			_TL("bottom"      ),
			_TL("bottom right"),
			_TL("right"       ),
			_TL("top right"   )
		), 1
	);

	m_Parameters.Add_Color("VALUES_EFFECT", "VALUES_EFFECT_COLOR", _TL("Color"),
		_TL(""),
		SG_GET_RGB(255, 255, 255)
	);

	//-----------------------------------------------------
	// Memory...

	m_Parameters.Add_Double("NODE_GENERAL",
		"MAX_SAMPLES"	, _TL("Maximum Samples"),
		_TL("Maximum number of samples used to build statistics and histograms expressed as percent of the total number of cells."),
		100. * (double)Get_Grid()->Get_Max_Samples() / (double)Get_Grid()->Get_NCells(), 0., true, 100., true
	);

	m_Parameters.Add_Bool("NODE_GENERAL",
		"FILE_CACHE"	, _TL("File Cache"),
		_TL(""),
		Get_Grid()->is_Cached()
	);

	//-----------------------------------------------------
	m_Fit_Colors	= g_pData->Get_Parameter("GRID_STRETCH_DEFAULT")->asInt();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grid::On_DataObject_Changed(void)
{
	//-----------------------------------------------------
	m_Parameters.Set_Parameter("OBJECT_Z_UNIT"  , Get_Grid()->Get_Unit   ());
	m_Parameters.Set_Parameter("OBJECT_Z_FACTOR", Get_Grid()->Get_Scaling());
	m_Parameters.Set_Parameter("OBJECT_Z_OFFSET", Get_Grid()->Get_Offset ());

	m_Parameters.Set_Parameter("MAX_SAMPLES"    , 100. * Get_Grid()->Get_Max_Samples() / (double)Get_Grid()->Get_NCells());

	m_Parameters.Set_Parameter("FILE_CACHE"     , Get_Grid()->is_Cached  ());

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
	CWKSP_Layer::On_DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_Grid::On_Parameters_Changed(void)
{
	CWKSP_Layer::On_Parameters_Changed();

	//-----------------------------------------------------
	Get_Grid()->Set_Unit   (m_Parameters("OBJECT_Z_UNIT"  )->asString());
	Get_Grid()->Set_Scaling(m_Parameters("OBJECT_Z_FACTOR")->asDouble(),
	                        m_Parameters("OBJECT_Z_OFFSET")->asDouble());

	Get_Grid()->Set_Max_Samples(Get_Grid()->Get_NCells() * (m_Parameters("MAX_SAMPLES")->asDouble() / 100.) );

	Get_Grid()->Set_Cache(m_Parameters("FILE_CACHE")->asBool());

	m_pClassify->Set_Shade_Mode(m_Parameters("SHADE_MODE")->asInt());

	//-----------------------------------------------------
	if( m_Parameters("STRETCH_DEFAULT")->asInt() < 3 )	// not manual, remember last state...
	{
		m_Fit_Colors	= m_Parameters("STRETCH_DEFAULT")->asInt();
	}
}

//---------------------------------------------------------
bool CWKSP_Grid::Update(CWKSP_Layer *pChanged)
{
	if( pChanged )
	{
		if( pChanged == this )
		{
			return( true );
		}

		if( pChanged->Get_Type() == WKSP_ITEM_Grid )
		{
			CSG_Grid	*pGrid	= ((CWKSP_Grid *)pChanged)->Get_Grid();

			return(	(m_Parameters("OVERLAY_R")->is_Enabled() && pGrid == m_Parameters("OVERLAY_R")->asGrid())
				||  (m_Parameters("OVERLAY_G")->is_Enabled() && pGrid == m_Parameters("OVERLAY_G")->asGrid())
				||  (m_Parameters("OVERLAY_B")->is_Enabled() && pGrid == m_Parameters("OVERLAY_B")->asGrid())
			);
		}

		if( pChanged->Get_Type() == WKSP_ITEM_Grids )
		{
			CSG_Grids	*pGrids	= ((CWKSP_Grids *)pChanged)->Get_Grids();

			return(	(m_Parameters("OVERLAY_R")->is_Enabled() && m_Parameters("OVERLAY_R")->asGrid() && pGrids == m_Parameters("OVERLAY_R")->asGrid()->Get_Owner())
				||  (m_Parameters("OVERLAY_G")->is_Enabled() && m_Parameters("OVERLAY_G")->asGrid() && pGrids == m_Parameters("OVERLAY_G")->asGrid()->Get_Owner())
				||  (m_Parameters("OVERLAY_B")->is_Enabled() && m_Parameters("OVERLAY_B")->asGrid() && pGrids == m_Parameters("OVERLAY_B")->asGrid()->Get_Owner())
			);
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( pParameters->Cmp_Identifier("CLASSIFY") )
	{
		if( Flags & PARAMETER_CHECK_ENABLE )
		{
			if( pParameter->Cmp_Identifier("METHOD") )
			{
				pParameters->Set_Enabled("COUNT"   , pParameter->asInt() != 0);
				pParameters->Set_Enabled("COUNTMAX", pParameter->asInt() == 0);
			}
		}

		return( CWKSP_Layer::On_Parameter_Changed(pParameters, pParameter, Flags) );
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if(	pParameter->Cmp_Identifier("OBJECT_Z_FACTOR")
		||	pParameter->Cmp_Identifier("OBJECT_Z_OFFSET") )
		{
			double	newFactor	= (*pParameters)("OBJECT_Z_FACTOR")->asDouble(), oldFactor	= m_Parameters("OBJECT_Z_FACTOR")->asDouble();
			double	newOffset	= (*pParameters)("OBJECT_Z_OFFSET")->asDouble(), oldOffset	= m_Parameters("OBJECT_Z_OFFSET")->asDouble();

			if( newFactor != 0. && oldFactor != 0. )
			{
				CSG_Parameter_Range	*newRange	= (*pParameters)("METRIC_ZRANGE")->asRange();
				CSG_Parameter_Range	*oldRange	=  m_Parameters ("METRIC_ZRANGE")->asRange();

				newRange->Set_Min(((oldRange->Get_Min() - oldOffset) / oldFactor) * newFactor + newOffset);
				newRange->Set_Max(((oldRange->Get_Max() - oldOffset) / oldFactor) * newFactor + newOffset);
			}
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	pParameter->Cmp_Identifier("DISPLAY_SHADING") )
		{
			pParameters->Set_Enabled("SHADING_AZIMUTH"    , pParameter->asInt() != 0);
			pParameters->Set_Enabled("SHADING_HEIGHT"     , pParameter->asInt() != 0);
			pParameters->Set_Enabled("SHADING_EXAGG"      , pParameter->asInt() != 0);
			pParameters->Set_Enabled("SHADING_MIN"        , pParameter->asInt() != 0);
			pParameters->Set_Enabled("SHADING_MAX"        , pParameter->asInt() != 0);
		}

		if(	pParameter->Cmp_Identifier("COLORS_TYPE") )
		{
			int		Value	= pParameter->asInt();

			pParameters->Set_Enabled("NODE_SINGLE"        , Value == CLASSIFY_SINGLE );
			pParameters->Set_Enabled("NODE_LUT"           , Value == CLASSIFY_LUT    );
			pParameters->Set_Enabled("NODE_METRIC"        , Value != CLASSIFY_SINGLE && Value != CLASSIFY_LUT);
			pParameters->Set_Enabled("NODE_SHADE"         , Value == CLASSIFY_SHADE  );
			pParameters->Set_Enabled("NODE_OVERLAY"       , Value == CLASSIFY_OVERLAY);
			pParameters->Set_Enabled("RGB_ALPHA"          , Value == CLASSIFY_RGB    );

			pParameters->Set_Enabled("DISPLAY_RESAMPLING" , Value != CLASSIFY_LUT    );
			pParameters->Set_Enabled("DISPLAY_SHADING"    , Value != CLASSIFY_SHADE  );

			pParameters->Set_Enabled("DISPLAY_ALPHA"      , Value != CLASSIFY_RGB    );
		}

		if( pParameter->Cmp_Identifier("DISPLAY_ALPHA") )
		{
			pParameters->Set_Enabled("ALPHA_RANGE"        , pParameter->asGrid() != NULL);
		}

		if(	pParameter->Cmp_Identifier("VALUES_SHOW") )
		{
			pParameters->Set_Enabled("VALUES_FONT"        , pParameter->asBool());
			pParameters->Set_Enabled("VALUES_SIZE"        , pParameter->asBool());
			pParameters->Set_Enabled("VALUES_DECIMALS"    , pParameter->asBool());
			pParameters->Set_Enabled("VALUES_EFFECT"      , pParameter->asBool());
		}

		if(	pParameter->Cmp_Identifier("VALUES_EFFECT") )
		{
			pParameters->Set_Enabled("VALUES_EFFECT_COLOR", pParameter->asInt() > 0);
		}

		if( pParameter->Cmp_Identifier("OVERLAY_MODE") )
		{
			(*pParameters)("OVERLAY_R")->Get_Parent()->Set_Enabled(pParameter->asInt() != 0);
			(*pParameters)("OVERLAY_G")->Get_Parent()->Set_Enabled(pParameter->asInt() != 1);
			(*pParameters)("OVERLAY_B")->Get_Parent()->Set_Enabled(pParameter->asInt() != 2);
		}
	}

	return( CWKSP_Layer::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grid::_LUT_Create(void)
{
	//-----------------------------------------------------
	static CSG_Parameters	PStatic;

	if( PStatic.Get_Count() == 0 )
	{
		PStatic.Create(_TL("Classify"));
		PStatic.Add_Colors("", "COLORS"  , _TL("Colors"                   ), _TL(""));
		PStatic.Add_Int   ("", "COUNT"   , _TL("Number of Classes"        ), _TL(""),   10, 1, true);
		PStatic.Add_Int   ("", "COUNTMAX", _TL("Maximum Number of Classes"), _TL(""), 1000, 1, true);
		PStatic.Add_Choice("", "METHOD"  , _TL("Classification"           ), _TL(""),
			CSG_String::Format("%s|%s|%s|%s",
				_TL("unique values"),
				_TL("equal intervals"),
				_TL("quantiles"),
				_TL("natural breaks")
			), 1
		);
	}

	CSG_Parameters	Parameters(_TL("Classify"), _TL(""), SG_T("CLASSIFY"));

	Parameters.Assign_Parameters(&PStatic);

	Parameters("COLORS")->asColors()->Assign(Get_Colors());

	Parameters.Set_Callback_On_Parameter_Changed(&Parameter_Callback);

	if( !DLG_Parameters(&Parameters) )
	{
		return;
	}

	PStatic.Assign_Values(&Parameters);

	//-----------------------------------------------------
	CSG_Colors	Colors(*Parameters("COLORS")->asColors());

	if( Parameters("METHOD")->asInt() != 0 )
	{
		Colors.Set_Count(Parameters("COUNT")->asInt());
	}

	CSG_Table	Classes(m_Parameters("LUT")->asTable());

	switch( Parameters("METHOD")->asInt() )
	{
	//-----------------------------------------------------
	case 0:	// unique values
	{
		CSG_Unique_Number_Statistics	s;

		int	maxClasses	= Parameters("COUNTMAX")->asInt();

		for(sLong iCell = 0; iCell<Get_Grid()->Get_NCells() && s.Get_Count()<maxClasses && PROGRESSBAR_Set_Position(iCell, Get_Grid()->Get_NCells()); iCell++)
		{
			if( !Get_Grid()->is_NoData(iCell) )
			{
				s	+= Get_Grid()->asDouble(iCell);
			}
		}

		Colors.Set_Count(s.Get_Count());

		for(int iClass=0; iClass<s.Get_Count(); iClass++)
		{
			double		Value	= s.Get_Value(iClass);

			CSG_String	Name	= SG_Get_String(Value, -2);

			CSG_Table_Record	*pClass	= Classes.Add_Record();

			pClass->Set_Value(0, Colors[iClass]);	// Color
			pClass->Set_Value(1, Name          );	// Name
			pClass->Set_Value(2, Name          );	// Description
			pClass->Set_Value(3, Value         );	// Minimum
			pClass->Set_Value(4, Value         );	// Maximum
		}

		break;
	}

	//-----------------------------------------------------
	case 1:	// equal intervals
		if( Get_Grid()->Get_Range() && Colors.Get_Count() > 0 )
		{
			double	Minimum, Maximum, Interval;

			Interval	= Get_Grid()->Get_Range() / (double)Colors.Get_Count();
			Minimum		= Get_Grid()->Get_Min  ();

			for(int iClass=0; iClass<Colors.Get_Count(); iClass++, Minimum+=Interval)
			{
				Maximum	= iClass < Colors.Get_Count() - 1 ? Minimum + Interval : Get_Grid()->Get_Max() + 1.;

				CSG_String	Name	= SG_Get_String(Minimum, -2)
							+ " - " + SG_Get_String(Maximum, -2);

				CSG_Table_Record	*pClass	= Classes.Add_Record();

				pClass->Set_Value(0, Colors[iClass]);	// Color
				pClass->Set_Value(1, Name          );	// Name
				pClass->Set_Value(2, Name          );	// Description
				pClass->Set_Value(3, Minimum       );	// Minimum
				pClass->Set_Value(4, Maximum       );	// Maximum
			}
		}
		break;

	//-----------------------------------------------------
	case 2:	// quantiles
		{
			if( Get_Grid()->Get_NCells() < Colors.Get_Count() )
			{
				Colors.Set_Count(Get_Grid()->Get_NCells());
			}

			double	Minimum, Maximum	= Get_Grid()->Get_Histogram().Get_Quantile(0.);

			double	Step	= 1. / Colors.Get_Count();

			for(int iClass=0; iClass<Colors.Get_Count(); iClass++)
			{
				Minimum	= Maximum;
				Maximum	= Get_Grid()->Get_Histogram().Get_Quantile((1. + iClass) / Colors.Get_Count());

				CSG_String	Name	= SG_Get_String(Minimum, -2)
							+ " - " + SG_Get_String(Maximum, -2);

				CSG_Table_Record	*pClass	= Classes.Add_Record();

				pClass->Set_Value(0, Colors[iClass]);	// Color
				pClass->Set_Value(1, Name          );	// Name
				pClass->Set_Value(2, Name          );	// Description
				pClass->Set_Value(3, Minimum       );	// Minimum
				pClass->Set_Value(4, Maximum       );	// Maximum
			}
		}
		break;

	//-----------------------------------------------------
	case 3:	// natural breaks
		{
			CSG_Natural_Breaks	Breaks(Get_Grid(), Colors.Get_Count(), 255);

			if( Breaks.Get_Count() <= Colors.Get_Count() ) return;

			for(int iClass=0; iClass<Colors.Get_Count(); iClass++)
			{
				CSG_Table_Record	*pClass	= Classes.Add_Record();

				double	Minimum	= Breaks[iClass    ];
				double	Maximum	= Breaks[iClass + 1];

				CSG_String	Name	= SG_Get_String(Minimum, -2)
							+ " - " + SG_Get_String(Maximum, -2);

				pClass->Set_Value(0, Colors[iClass]);	// Color
				pClass->Set_Value(1, Name          );	// Name
				pClass->Set_Value(2, Name          );	// Description
				pClass->Set_Value(3, Minimum       );	// Minimum
				pClass->Set_Value(4, Maximum       );	// Maximum
			}
		}
		break;
	}

	//-----------------------------------------------------
	PROGRESSBAR_Set_Position(0);

	if( Classes.Get_Count() > 0 )
	{
		m_Parameters("LUT")->asTable()->Assign_Values(&Classes);

		m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_LUT);	// Lookup Table

		Parameters_Changed();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grid::Get_Value(CSG_Point ptWorld, double Epsilon)
{
	wxString	s;	double	Value;

	if( Get_Grid()->Get_Value(ptWorld, Value, GRID_RESAMPLING_NearestNeighbour) )
	{
		switch( m_pClassify->Get_Mode() )
		{
		default:
			s	= SG_Get_String(Value, -12).c_str();

			if( !Get_Grid()->Get_Unit().is_Empty() )
			{
				s += " "; s += Get_Grid()->Get_Unit().c_str();
			}
			break;

		case CLASSIFY_RGB:
			s.Printf("R%03d G%03d B%03d", SG_GET_R((int)Value), SG_GET_G((int)Value), SG_GET_B((int)Value));
			break;

		case CLASSIFY_LUT:
			s	= m_pClassify->Get_Class_Name_byValue(Value);
			break;
		}
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CWKSP_Grid::Get_Value_Minimum(void)	{	return( ((CSG_Grid *)m_pObject)->Get_Min   () );	}
double CWKSP_Grid::Get_Value_Maximum(void)	{	return( ((CSG_Grid *)m_pObject)->Get_Max   () );	}
double CWKSP_Grid::Get_Value_Range  (void)	{	return( ((CSG_Grid *)m_pObject)->Get_Range () );	}
double CWKSP_Grid::Get_Value_Mean   (void)	{	return( ((CSG_Grid *)m_pObject)->Get_Mean  () );	}
double CWKSP_Grid::Get_Value_StdDev (void)	{	return( ((CSG_Grid *)m_pObject)->Get_StdDev() );	}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_Grid::Edit_Get_Menu(void)
{
	if( m_Edit_Attributes.Get_Count() < 1 )
	{
		return( NULL );
	}

	wxMenu	*pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, true , ID_CMD_GRID_SEL_CLEAR);
	CMD_Menu_Add_Item(pMenu, true , ID_CMD_GRID_SEL_DELETE);

	return( pMenu );
}

//---------------------------------------------------------
TSG_Rect CWKSP_Grid::Edit_Get_Extent(void)
{
	if( m_Edit_Attributes.Get_Count() > 0 )
	{
		return( CSG_Rect(
			-Get_Grid()->Get_Cellsize() / 2. + Get_Grid()->Get_System().Get_xGrid_to_World(m_xSel),
			-Get_Grid()->Get_Cellsize() / 2. + Get_Grid()->Get_System().Get_yGrid_to_World(m_ySel),
			-Get_Grid()->Get_Cellsize() / 2. + Get_Grid()->Get_System().Get_xGrid_to_World(m_xSel + m_Edit_Attributes.Get_Field_Count() - 1),
			-Get_Grid()->Get_Cellsize() / 2. + Get_Grid()->Get_System().Get_yGrid_to_World(m_ySel + m_Edit_Attributes.Get_Count()))
		);
	}

	return( Get_Grid()->Get_Extent().m_rect );
}

//---------------------------------------------------------
bool CWKSP_Grid::Edit_On_Key_Down(int KeyCode)
{
	switch( KeyCode )
	{
	default:
		return( false );

	case WXK_DELETE:
		return( _Edit_Del_Selection() );
	}
}

//---------------------------------------------------------
bool CWKSP_Grid::Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
{
	if( Key & TOOL_INTERACTIVE_KEY_LEFT )
	{
		g_pActive->Update_Attributes();

		m_Edit_Attributes.Destroy();
		m_Edit_Attributes.Add_Field("ROW", SG_DATATYPE_Int);

		CSG_Rect	rWorld(m_Edit_Mouse_Down, Point);

		if( rWorld.Intersects(Get_Grid()->Get_Extent(true)) )
		{
			m_xSel	= Get_Grid()->Get_System().Get_xWorld_to_Grid(rWorld.Get_XMin()); if( m_xSel < 0 ) m_xSel = 0;
			int	nx	= Get_Grid()->Get_System().Get_xWorld_to_Grid(rWorld.Get_XMax()); if( nx >= Get_Grid()->Get_NX() ) nx = Get_Grid()->Get_NX() - 1;
			nx	= 1 + nx - m_xSel;

			m_ySel	= Get_Grid()->Get_System().Get_yWorld_to_Grid(rWorld.Get_YMin()); if( m_ySel < 0 ) m_ySel = 0;
			int	ny	= Get_Grid()->Get_System().Get_yWorld_to_Grid(rWorld.Get_YMax()); if( ny >= Get_Grid()->Get_NY() ) ny = Get_Grid()->Get_NY() - 1;
			ny	= 1 + ny - m_ySel;

			if( nx > 0 && ny > 0 )
			{
				int	Maximum = g_pData->Get_Parameter("GRID_SELECT_MAX")->asInt();

				if( nx > Maximum )
				{
					m_xSel	+= (nx - Maximum) / 2;
					nx		= Maximum;
				}

				if( ny > Maximum )
				{
					m_ySel	+= (ny - Maximum) / 2;
					ny		= Maximum;
				}

				for(int x=0; x<nx; x++)
				{
					m_Edit_Attributes.Add_Field(CSG_String::Format("%d", m_xSel + 1 + x), Get_Grid()->Get_Type());
				}

				for(int y=0; y<ny; y++)
				{
					CSG_Table_Record	*pRecord	= m_Edit_Attributes.Add_Record();

					for(int x=0; x<nx; x++)
					{
						pRecord->Set_Value(0, m_ySel + ny - y);

						if( !Get_Grid()->is_NoData(m_xSel + x, m_ySel + ny - 1 - y) )
						{
							pRecord->Set_Value(1 + x, Get_Grid()->asDouble(m_xSel + x, m_ySel + ny - 1 - y));
						}
						else
						{
							pRecord->Set_NoData(1 + x);
						}
					}
				}
			}
		}

		g_pActive->Update_Attributes();

		Update_Views();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid::Edit_Set_Attributes(void)
{
	if( m_Edit_Attributes.Get_Count() > 0 )
	{
		for(int y=0; y<m_Edit_Attributes.Get_Count(); y++)
		{
			CSG_Table_Record	*pRecord	= m_Edit_Attributes.Get_Record(m_Edit_Attributes.Get_Count() - 1 - y);

			for(int x=0, xx=1; xx<m_Edit_Attributes.Get_Field_Count(); x++, xx++)
			{
				if( !pRecord->is_NoData(xx) )
				{
					Get_Grid()->Set_Value(m_xSel + x, m_ySel + y, pRecord->asDouble(xx));
				}
				else
				{
					Get_Grid()->Set_NoData(m_xSel + x, m_ySel + y);
				}
			}
		}

		Update_Views();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grid::_Edit_Clr_Selection(void)
{
	if( m_Edit_Attributes.Get_Count() > 0 )
	{
		m_Edit_Attributes.Destroy();

		g_pActive->Update_Attributes();

		Update_Views();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid::_Edit_Del_Selection(void)
{
	if( m_Edit_Attributes.Get_Count() > 0 && DLG_Message_Confirm(_TL("Set selected values to no data."), _TL("Delete")) )
	{
		for(int y=0; y<m_Edit_Attributes.Get_Count(); y++)
		{
			for(int x=0, xx=1; xx<m_Edit_Attributes.Get_Field_Count(); x++, xx++)
			{
				m_Edit_Attributes[y].Set_NoData(xx);

				Get_Grid()->Set_NoData(m_xSel + x, m_ySel + y);
			}
		}

		g_pActive->Update_Attributes();

		Update_Views();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grid::Fit_Colors(const CSG_Rect &rWorld)
{
	double	Minimum, Maximum;

	switch( m_Fit_Colors )
	{
	default: {	CSG_Simple_Statistics s; if( !Get_Grid()->Get_Statistics(rWorld, s) ) { return( false ); }
		Minimum	= s.Get_Minimum() + s.Get_Range() * m_Parameters("STRETCH_LINEAR.MIN")->asDouble() / 100.;
		Maximum	= s.Get_Minimum() + s.Get_Range() * m_Parameters("STRETCH_LINEAR.MAX")->asDouble() / 100.;
		break;	}

	case  1: {	CSG_Simple_Statistics s; if( !Get_Grid()->Get_Statistics(rWorld, s) ) { return( false ); }
		double	d	= m_Parameters("STRETCH_STDDEV")->asDouble() * s.Get_StdDev();
		Minimum	= s.Get_Mean() - d; if( m_Parameters("STRETCH_INRANGE")->asBool() && Minimum < Get_Grid()->Get_Min() ) Minimum = Get_Grid()->Get_Min();
		Maximum	= s.Get_Mean() + d; if( m_Parameters("STRETCH_INRANGE")->asBool() && Maximum > Get_Grid()->Get_Max() ) Maximum = Get_Grid()->Get_Max();
		break;	}

	case  2: {	CSG_Histogram         h; if( !Get_Grid()->Get_Histogram (rWorld, h) ) { return( false ); }
		Minimum	= h.Get_Quantile(m_Parameters("STRETCH_PCTL.MIN")->asDouble() / 100.);
		Maximum	= h.Get_Quantile(m_Parameters("STRETCH_PCTL.MAX")->asDouble() / 100.);
		break;	}
	}

	return( Set_Color_Range(Minimum, Maximum) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grid::asImage(CSG_Grid *pImage)
{
	wxBitmap	BMP;

	if( pImage && Get_Image_Grid(BMP) )
	{
		wxImage	IMG(BMP.ConvertToImage());

		pImage->Create(Get_Grid(), SG_DATATYPE_Int);

		#pragma omp parallel for
		for(int y=0; y<pImage->Get_NY(); y++)
		{
			for(int x=0; x<pImage->Get_NX(); x++)
			{
				pImage->Set_Value(x, y, SG_GET_RGB(IMG.GetRed(x, y), IMG.GetGreen(x, y), IMG.GetBlue(x, y)));
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid::_Save_Image(void)
{
	static CSG_Parameters	P(NULL, _TL("Save Grid as Image..."), _TL(""), SG_T(""));

	if( P.Get_Count() == 0 )
	{
		P.Add_Bool  ("", "WORLD", _TL("Save Georeference"), _TL(""), true);
		P.Add_Bool  ("", "LG"   , _TL("Legend: Save"     ), _TL(""), true);
		P.Add_Double("", "LZ"   , _TL("Legend: Zoom"     ), _TL(""), 1., 0., true);
	}

	//-----------------------------------------------------
	int			Type;
	wxString	File;
	wxBitmap	Bitmap;

	if( !DLG_Image_Save(File, Type) || !DLG_Parameters(&P) || !Get_Image_Grid(Bitmap) )
	{
		return( false );
	}

	Bitmap.SaveFile(File, (wxBitmapType)Type);

	//-----------------------------------------------------
	if( P("LG")->asBool() && Get_Image_Legend(Bitmap, P("LZ")->asDouble()) )
	{
		wxFileName	fn(File); fn.SetName(wxString::Format("%s_legend", fn.GetName().c_str()));

		Bitmap.SaveFile(fn.GetFullPath(), (wxBitmapType)Type);
	}

	if( P("WORLD")->asBool() )
	{
		wxFileName	fn(File);

		switch( Type )
		{
		default                : fn.SetExt("world");	break;
		case wxBITMAP_TYPE_BMP : fn.SetExt("bpw"  );	break;
		case wxBITMAP_TYPE_GIF : fn.SetExt("gfw"  );	break;
		case wxBITMAP_TYPE_JPEG: fn.SetExt("jgw"  );	break;
		case wxBITMAP_TYPE_PNG : fn.SetExt("pgw"  );	break;
		case wxBITMAP_TYPE_PCX : fn.SetExt("pxw"  );	break;
		case wxBITMAP_TYPE_TIF : fn.SetExt("tfw"  );	break; 
		}

		CSG_File	Stream;

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
bool CWKSP_Grid::_Save_Image_Clipboard(void)
{
	wxBitmap	Bitmap;

	if( Get_Image_Grid(Bitmap) && wxTheClipboard->Open() )
	{
		wxBitmapDataObject	*pBitmap	= new wxBitmapDataObject;
		pBitmap->SetBitmap(Bitmap);
		wxTheClipboard->SetData(pBitmap);
		wxTheClipboard->Close();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid::Get_Image_Grid(wxBitmap &BMP, bool bFitSize)
{
	if( bFitSize || (BMP.GetWidth() > 0 && BMP.GetHeight() > 0) )
	{
		Set_Buisy_Cursor(true);

		if( bFitSize )
		{
			BMP.Create(Get_Grid()->Get_NX(), Get_Grid()->Get_NY());
		}

		wxMemoryDC		dc;
		wxRect			r(0, 0, BMP.GetWidth(), BMP.GetHeight());
		CWKSP_Map_DC	dc_Map(Get_Extent(), r, 1., SG_GET_RGB(255, 255, 255));

		On_Draw(dc_Map, false);

		dc.SelectObject(BMP);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		dc_Map.Draw(dc);

		dc.SelectObject(wxNullBitmap);

		Set_Buisy_Cursor(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid::Get_Image_Legend(wxBitmap &BMP, double Zoom)
{
	if( Zoom > 0. )
	{
		wxMemoryDC	dc;
		wxSize		s(Get_Legend()->Get_Size(Zoom, 1.));

		BMP.Create(s.GetWidth(), s.GetHeight());

		dc.SelectObject(BMP);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		Get_Legend()->Draw(dc, Zoom, 1., wxPoint(0, 0));

		dc.SelectObject(wxNullBitmap);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Grid::On_Draw(CWKSP_Map_DC &dc_Map, int Flags)
{
	if(	Get_Extent().Intersects(dc_Map.m_rWorld) == INTERSECTION_None )
	{
		return;
	}

	//-----------------------------------------------------
	int	Mode	= m_pClassify->Get_Mode() == CLASSIFY_SHADE                                          ? IMG_MODE_SHADING
				: m_pClassify->Get_Mode() == CLASSIFY_RGB && m_Parameters("RGB_ALPHA"    )->asBool() ? IMG_MODE_TRANSPARENT_ALPHA
				: m_pClassify->Get_Mode() != CLASSIFY_RGB && m_Parameters("DISPLAY_ALPHA")->asGrid() ? IMG_MODE_TRANSPARENT_ALPHA
				: IMG_MODE_TRANSPARENT;

	if( !dc_Map.IMG_Draw_Begin(m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100., Mode) )
	{
		return;
	}

	//-----------------------------------------------------
	m_pAlpha   = m_Parameters("DISPLAY_ALPHA"  )->asGrid  ();
	m_Alpha[0] = m_Parameters("ALPHA_RANGE.MIN")->asDouble();
	m_Alpha[1] = m_Parameters("ALPHA_RANGE.MAX")->asDouble() - m_Alpha[0]; m_Alpha[1] = m_Alpha[1] ? 255. / m_Alpha[1] : 1.;

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	if( m_pClassify->Get_Mode() == CLASSIFY_LUT )
	{
		Resampling	= GRID_RESAMPLING_NearestNeighbour;
	}
	else switch( m_Parameters("DISPLAY_RESAMPLING")->asInt() )
	{
	default: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	m_Shade_Mode	= m_Parameters("COLORS_TYPE"    )->asInt() != CLASSIFY_SHADE
					? m_Parameters("DISPLAY_SHADING")->asInt() : 0;

	if( m_Shade_Mode && Get_Grid()->Get_Range() > 0. )
	{
	//	m_Shade_Parms[0]	= m_Parameters("SHADING_EXAGG")->asDouble() * Get_Grid()->Get_Cellsize() / Get_Grid()->Get_StdDev();
		m_Shade_Parms[0]	= m_Parameters("SHADING_EXAGG")->asDouble() * Get_Grid()->Get_Cellsize() / 25.;
		m_Shade_Parms[1]	= sin(M_DEG_TO_RAD * m_Parameters("SHADING_HEIGHT")->asDouble());
		m_Shade_Parms[2]	= cos(M_DEG_TO_RAD * m_Parameters("SHADING_HEIGHT")->asDouble());
		m_Shade_Parms[3]	=     M_DEG_TO_RAD * m_Parameters("SHADING_AZIMUTH")->asDouble();
		m_Shade_Parms[4]	= m_Parameters("SHADING_MIN")->asDouble();
		m_Shade_Parms[5]	= m_Parameters("SHADING_MAX")->asDouble() - m_Shade_Parms[4];
	}

	//-----------------------------------------------------
	m_pClassify->Set_Shade_Mode(m_Parameters("SHADE_MODE")->asInt());

	if(	dc_Map.m_DC2World >= Get_Grid()->Get_Cellsize()
	||	Resampling != GRID_RESAMPLING_NearestNeighbour
	||  m_Parameters("COLORS_TYPE")->asInt() == CLASSIFY_OVERLAY )
	{
		_Draw_Grid_Nodes(dc_Map, Resampling);
	}
	else
	{
		_Draw_Grid_Cells(dc_Map);
	}

	//-----------------------------------------------------
	dc_Map.IMG_Draw_End();

	if( (Flags & LAYER_DRAW_FLAG_NOLABELS) == 0 )
	{
		_Draw_Values(dc_Map);
	}

	if( (Flags & LAYER_DRAW_FLAG_NOEDITS) == 0 )
	{
		_Draw_Edit(dc_Map);
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Grid_Nodes(CWKSP_Map_DC &dc_Map, TSG_Grid_Resampling Resampling)
{
	CSG_Grid *pOverlay[2]; CSG_Scaler	Scaler[2];

	_Get_Overlay(pOverlay, Scaler);

	CSG_Rect	rMap(dc_Map.m_rWorld);	rMap.Intersect(Get_Grid()->Get_Extent(true));

	int	axDC	= (int)dc_Map.xWorld2DC(rMap.Get_XMin());	if( axDC < 0 )	axDC	= 0;
	int	bxDC	= (int)dc_Map.xWorld2DC(rMap.Get_XMax());	if( bxDC >= dc_Map.m_rDC.GetWidth () )	bxDC	= dc_Map.m_rDC.GetWidth () - 1;
	int	ayDC	= (int)dc_Map.yWorld2DC(rMap.Get_YMin());	if( ayDC >= dc_Map.m_rDC.GetHeight() )	ayDC	= dc_Map.m_rDC.GetHeight() - 1;
	int	byDC	= (int)dc_Map.yWorld2DC(rMap.Get_YMax());	if( byDC < 0 )	byDC	= 0;
	int	nyDC	= abs(ayDC - byDC);

	if( Get_Grid()->is_Cached() )
	{
		for(int iyDC=0; iyDC<=nyDC; iyDC++)
		{
			_Draw_Grid_Nodes(dc_Map, Resampling, ayDC - iyDC, axDC, bxDC, pOverlay, Scaler);
		}
	}
	else
	{
		#pragma omp parallel for
		for(int iyDC=0; iyDC<=nyDC; iyDC++)
		{
			_Draw_Grid_Nodes(dc_Map, Resampling, ayDC - iyDC, axDC, bxDC, pOverlay, Scaler);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Get_Overlay(CSG_Grid *pOverlay[2], CSG_Scaler Scaler[2])
{
	if( m_pClassify->Get_Mode() != CLASSIFY_OVERLAY )
	{
		pOverlay [0] = pOverlay [1] = NULL;
	}
	else
	{
		switch( m_Parameters("OVERLAY_MODE")->asInt() )
		{
		default:	// this = red
			pOverlay[0]	= m_Parameters("OVERLAY_G")->asGrid();
			pOverlay[1]	= m_Parameters("OVERLAY_B")->asGrid();
			break;

		case  1:	// this = green
			pOverlay[0]	= m_Parameters("OVERLAY_R")->asGrid();
			pOverlay[1]	= m_Parameters("OVERLAY_B")->asGrid();
			break;

		case  2:	// this = blue
			pOverlay[0]	= m_Parameters("OVERLAY_R")->asGrid();
			pOverlay[1]	= m_Parameters("OVERLAY_G")->asGrid();
			break;
		}

		for(int i=0; i<2; i++)
		{
			if( !SG_Get_Data_Manager().Exists(pOverlay[i]) )
			{
				pOverlay[i]	= NULL;
			}
			else if( pOverlay[i]->Get_Owner() )
			{
				double	Interval	=
					m_pClassify->Get_Metric_Mode() == METRIC_MODE_LOGUP   ?  m_pClassify->Get_Metric_LogFactor() :
					m_pClassify->Get_Metric_Mode() == METRIC_MODE_LOGDOWN ? -m_pClassify->Get_Metric_LogFactor() : 0.;

				switch( m_Fit_Colors )
				{
				default: Scaler[i].Set_Linear    (pOverlay[i], Interval, m_Parameters("STRETCH_LINEAR.MIN")->asDouble(), m_Parameters("STRETCH_LINEAR.MAX")->asDouble()); break;
				case  1: Scaler[i].Set_StdDev    (pOverlay[i], Interval, m_Parameters("STRETCH_STDDEV"    )->asDouble(), m_Parameters("STRETCH_INRANGE"   )->asBool  ()); break;
				case  2: Scaler[i].Set_Percentile(pOverlay[i], Interval, m_Parameters("STRETCH_PCTL.MIN"  )->asDouble(), m_Parameters("STRETCH_PCTL.MAX"  )->asDouble()); break;
				}
			}
			else if( g_pData->Get(pOverlay[i]) )
			{
				CWKSP_Layer_Classify	*pClassify	= ((CWKSP_Layer *)g_pData->Get(pOverlay[i]))->Get_Classifier();

				Scaler[i].Create(pClassify->Get_Metric_Minimum(), pClassify->Get_Metric_Maximum(),
					pClassify->Get_Metric_Mode() == METRIC_MODE_LOGUP   ?  pClassify->Get_Metric_LogFactor() :
					pClassify->Get_Metric_Mode() == METRIC_MODE_LOGDOWN ? -pClassify->Get_Metric_LogFactor() : 0.
				);
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Grid_Nodes(CWKSP_Map_DC &dc_Map, TSG_Grid_Resampling Resampling, int yDC, int axDC, int bxDC, CSG_Grid *pOverlay[2], CSG_Scaler Scaler[2])
{
	int	Overlay[3];

	switch( m_Parameters("OVERLAY_MODE")->asInt() )
	{
	default: Overlay[0] = 0; Overlay[1] = 1; Overlay[2] = 2; break;	// this = r, pOverlay[0] = g, pOverlay[1] = r
	case  1: Overlay[0] = 1; Overlay[1] = 0; Overlay[2] = 2; break;	// this = g, pOverlay[0] = r, pOverlay[1] = b
	case  2: Overlay[0] = 2; Overlay[1] = 0; Overlay[2] = 1; break;	// this = b, pOverlay[0] = r, pOverlay[1] = g
	}

	double	xMap	= dc_Map.xDC2World(axDC);
	double	yMap	= dc_Map.yDC2World( yDC);

	for(int xDC=axDC; xDC<=bxDC; xMap+=dc_Map.m_DC2World, xDC++)
	{
		double	Value;

		if( Get_Grid()->Get_Value(xMap, yMap, Value, Resampling, false, m_pClassify->Get_Mode() == CLASSIFY_RGB) )
		{
			if( m_pClassify->Get_Mode() != CLASSIFY_OVERLAY )
			{
				int  Color;

				if( m_pClassify->Get_Class_Color_byValue(Value, Color) )
				{
					dc_Map.IMG_Set_Pixel(xDC, yDC, _Get_Shading(xMap, yMap, Color, Resampling));
				}
			}
			else
			{
				BYTE Color[4]; Color[3] = 0;

				Value = m_pClassify->Get_MetricToRelative(Value);
				Color[Overlay[0]] = Value <= 0. ? 0 : Value >= 1. ? 255 : (BYTE)(255. * Value);

				Value = pOverlay[0] && pOverlay[0]->Get_Value(xMap, yMap, Value, Resampling) ? Scaler[0].to_Relative(Value) : 1.;
				Color[Overlay[1]] = Value <= 0. ? 0 : Value >= 1. ? 255 : (BYTE)(255. * Value);

				Value = pOverlay[1] && pOverlay[1]->Get_Value(xMap, yMap, Value, Resampling) ? Scaler[1].to_Relative(Value) : 1.;
				Color[Overlay[2]] = Value <= 0. ? 0 : Value >= 1. ? 255 : (BYTE)(255. * Value);

				dc_Map.IMG_Set_Pixel(xDC, yDC, _Get_Shading(xMap, yMap, *(int *)Color, Resampling));
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Grid_Cells(CWKSP_Map_DC &dc_Map)
{
	int	xa	= Get_Grid()->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMin()); if( xa <  0                    ) xa = 0;
	int	ya	= Get_Grid()->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMin()); if( ya <  0                    ) ya = 0;
	int	xb	= Get_Grid()->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMax()); if( xb >= Get_Grid()->Get_NX() ) xb = Get_Grid()->Get_NX() - 1;
	int	yb	= Get_Grid()->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMax()); if( yb >= Get_Grid()->Get_NY() ) yb = Get_Grid()->Get_NY() - 1;

	double	dDC	 = Get_Grid()->Get_Cellsize() * dc_Map.m_World2DC;

	double	axDC = dc_Map.xWorld2DC(Get_Grid()->Get_System().Get_xGrid_to_World(xa)) + dDC / 2.;
	double	ayDC = dc_Map.yWorld2DC(Get_Grid()->Get_System().Get_yGrid_to_World(ya)) - dDC / 2.;

	//-----------------------------------------------------
	double	yDC	= ayDC;

	for(int y=ya, yaDC=(int)(ayDC), ybDC=(int)(ayDC+dDC); y<=yb; y++, ybDC=yaDC, yaDC=(int)(yDC-=dDC))
	{
		double	xDC	= axDC;

		for(int x=xa, xaDC=(int)(axDC-dDC), xbDC=(int)(axDC); x<=xb; x++, xaDC=xbDC, xbDC=(int)(xDC+=dDC))
		{
			int	Color;

			if( Get_Grid()->is_InGrid(x, y) && m_pClassify->Get_Class_Color_byValue(Get_Grid()->asDouble(x, y), Color) )
			{
				dc_Map.IMG_Set_Rect(xaDC, yaDC, xbDC, ybDC, _Get_Shading(x, y, Color));
			}
		}
	}
}

//---------------------------------------------------------
inline void CWKSP_Grid::_Set_Shading(double Shade, int &Color)
{
	switch( m_Shade_Mode )
	{
	default: Shade =      Shade / M_PI_090; break;
	case  1: Shade = 1. - Shade / M_PI_090; break;
	}

	Shade	= m_Shade_Parms[5] * (Shade - m_Shade_Parms[4]);

	int	r	= (int)(Shade * SG_GET_R(Color)); if( r < 0 ) r = 0; else if( r > 255 ) r = 255;
	int	g	= (int)(Shade * SG_GET_G(Color)); if( g < 0 ) g = 0; else if( g > 255 ) g = 255;
	int	b	= (int)(Shade * SG_GET_B(Color)); if( b < 0 ) b = 0; else if( b > 255 ) b = 255;

	Color	= SG_GET_RGB(r, g, b);
}

//---------------------------------------------------------
inline int CWKSP_Grid::_Get_Shading(int x, int y, int Color)
{
	if( m_Shade_Mode )
	{
		double	s, a;

		if( Get_Grid()->Get_Gradient(x, y, s, a) )
		{
			s	= M_PI_090 - atan(m_Shade_Parms[0] * tan(s));

			_Set_Shading(acos(sin(s) * m_Shade_Parms[1] + cos(s) * m_Shade_Parms[2] * cos(a - m_Shade_Parms[3])), Color);
		}
	}

	if( m_pAlpha )
	{
		double	Alpha;	TSG_Point	p	= Get_Grid()->Get_System().Get_Grid_to_World(x, y);

		Alpha	= m_pAlpha->Get_Value(p, Alpha) ? (Alpha - m_Alpha[0]) * m_Alpha[1] : 1.;

		((BYTE *)&Color)[3] = Alpha <= 0. ? 0 : Alpha >= 255. ? 255 : (BYTE)Alpha;
	}

	return( Color );
}

//---------------------------------------------------------
inline int CWKSP_Grid::_Get_Shading(double x, double y, int Color, TSG_Grid_Resampling Resampling)
{
	if( m_Shade_Mode )
	{
		double	s, a;

		if( Get_Grid()->Get_Gradient(x, y, s, a, Resampling) )
		{
			s	= M_PI_090 - atan(m_Shade_Parms[0] * tan(s));

			_Set_Shading(acos(sin(s) * m_Shade_Parms[1] + cos(s) * m_Shade_Parms[2] * cos(a - m_Shade_Parms[3])), Color);
		}
	}

	if( m_pAlpha )
	{
		double	Alpha;
		
		Alpha	= m_pAlpha->Get_Value(x, y, Alpha, Resampling) ? (Alpha - m_Alpha[0]) * m_Alpha[1] : 1.;

		((BYTE *)&Color)[3] = Alpha <= 0. ? 0 : Alpha >= 255. ? 255 : (BYTE)Alpha;
	}

	return( Color );
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Values(CWKSP_Map_DC &dc_Map)
{
	if(	!m_Parameters("VALUES_SHOW")->asBool() || Get_Grid()->Get_Cellsize() * dc_Map.m_World2DC <= 40 )
	{
		return;
	}

	//-----------------------------------------------------
	int			ax, ay, bx, by, Decimals, Effect;
	double		axDC, ayDC, dDC;
	wxColour	Effect_Color;
	wxFont		Font;

	dDC			= Get_Grid()->Get_Cellsize() * dc_Map.m_World2DC;
	Decimals	= m_Parameters("VALUES_DECIMALS")->asInt();
	Font		= Get_Font(m_Parameters("VALUES_FONT"));
	Font.SetPointSize((int)(dDC * m_Parameters("VALUES_SIZE")->asDouble() / 100.));
	dc_Map.dc.SetFont(Font);
	dc_Map.dc.SetTextForeground(Get_Color_asWX(m_Parameters("VALUES_FONT")->asColor()));

	switch( m_Parameters("VALUES_EFFECT")->asInt() )
	{
	default: Effect	= TEXTEFFECT_NONE       ;	break;
	case  1: Effect	= TEXTEFFECT_FRAME      ;	break;
	case  2: Effect	= TEXTEFFECT_TOP        ;	break;
	case  3: Effect	= TEXTEFFECT_TOPLEFT    ;	break;
	case  4: Effect	= TEXTEFFECT_LEFT       ;	break;
	case  5: Effect	= TEXTEFFECT_BOTTOMLEFT ;	break;
	case  6: Effect	= TEXTEFFECT_BOTTOM     ;	break;
	case  7: Effect	= TEXTEFFECT_BOTTOMRIGHT;	break;
	case  8: Effect	= TEXTEFFECT_RIGHT      ;	break;
	case  9: Effect	= TEXTEFFECT_TOPRIGHT   ;	break;
	}

	Effect_Color	= m_Parameters("VALUES_EFFECT_COLOR")->asColor();

	//-------------------------------------------------
	ax		= Get_Grid()->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMin());
	ay		= Get_Grid()->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMin());
	bx		= Get_Grid()->Get_System().Get_xWorld_to_Grid(dc_Map.m_rWorld.Get_XMax());
	by		= Get_Grid()->Get_System().Get_yWorld_to_Grid(dc_Map.m_rWorld.Get_YMax());

	if( ax < 0 )	ax	= 0;	if( bx >= Get_Grid()->Get_NX() )	bx	= Get_Grid()->Get_NX() - 1;
	if( ay < 0 )	ax	= 0;	if( by >= Get_Grid()->Get_NY() )	by	= Get_Grid()->Get_NY() - 1;

	axDC	= dc_Map.xWorld2DC(Get_Grid()->Get_System().Get_xGrid_to_World(ax));
	ayDC	= dc_Map.yWorld2DC(Get_Grid()->Get_System().Get_yGrid_to_World(ay));

	int	ny	= by - ay;

	//-------------------------------------------------
	for(int y=0; y<=ny; y++)
	{
		int		x;
		double	xDC, yDC	= ayDC - y * dDC;// dc_Map.m_World2DC;

		for(x=ax, xDC=axDC; x<=bx; x++, xDC+=dDC)
		{
			if( Get_Grid()->is_InGrid(x, ay + y) )
			{
				double	Value	= Get_Grid()->asDouble(x, ay + y);

				switch( m_pClassify->Get_Mode() )
				{
				case CLASSIFY_RGB:
					Draw_Text(dc_Map.dc, TEXTALIGN_CENTER, (int)xDC, (int)yDC, wxString::Format(
						"R%03d G%03d B%03d", SG_GET_R((int)Value), SG_GET_G((int)Value), SG_GET_B((int)Value)
					), Effect, Effect_Color);
					break;

				default:
					Draw_Text(dc_Map.dc, TEXTALIGN_CENTER, (int)xDC, (int)yDC, wxString::Format(
						"%.*f", Decimals, Value
					), Effect, Effect_Color);
					break;
				}
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Grid::_Draw_Edit(CWKSP_Map_DC &dc_Map)
{
	if( m_Edit_Attributes.Get_Count() > 0 )
	{
		CSG_Rect	r(
			-Get_Grid()->Get_Cellsize() / 2. + Get_Grid()->Get_System().Get_xGrid_to_World(m_xSel),
			-Get_Grid()->Get_Cellsize() / 2. + Get_Grid()->Get_System().Get_yGrid_to_World(m_ySel),
			-Get_Grid()->Get_Cellsize() / 2. + Get_Grid()->Get_System().Get_xGrid_to_World(m_xSel + m_Edit_Attributes.Get_Field_Count() - 1),
			-Get_Grid()->Get_Cellsize() / 2. + Get_Grid()->Get_System().Get_yGrid_to_World(m_ySel + m_Edit_Attributes.Get_Count())
		);

		TSG_Point_Int	a(dc_Map.World2DC(r.Get_TopLeft    ()));
		TSG_Point_Int	b(dc_Map.World2DC(r.Get_BottomRight()));

		a.x	-= 1;
		b.x	-= 1;
		a.y	-= 1;
		b.y	-= 1;

		dc_Map.dc.SetPen(wxPen(wxColour(255, 0, 0), 2, wxPENSTYLE_SOLID));
		dc_Map.dc.DrawLine(a.x, a.y, a.x, b.y);
		dc_Map.dc.DrawLine(a.x, b.y, b.x, b.y);
		dc_Map.dc.DrawLine(b.x, b.y, b.x, a.y);
		dc_Map.dc.DrawLine(a.x, a.y, b.x, a.y);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
